/*
 * publisher_task.c - InhabitX main on the E84: the main itself.
 *
 * Replaces publisher_task.c of Infineon's PSOC Edge Wi-Fi MQTT client example
 * (same names: its mqtt_task.c creates this task once Wi-Fi and the broker
 * are up, sends PUBLISHER_DEINIT when the connection drops and
 * PUBLISHER_INIT after reconnecting).
 *
 * Takes every incoming message from main_in_q and runs it through the main
 * (main_app.c: rooms, bookings, found items, replies), and twice a second
 * publishes the building state when it changed. The date and time come
 * from the internet (ix_sntp.c). Same behaviour as main_sim on the laptop,
 * without the screen.
 */
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "publisher_task.h"
#include "mqtt_task.h"
#include "mqtt_client_config.h"
#include "cy_mqtt_api.h"

#include "main_board.h"
#include "main_app.h"
#include "ix_sntp.h"

#include <stdio.h>
#include <string.h>

#define PUBLISHER_TASK_QUEUE_LENGTH     (3U)
#define POLL_EVERY_MS                   (500U)
#define SNTP_RETRY_MS                   (60U * 1000U)          /* until the first answer */
#define SNTP_EVERY_MS                   (6U * 3600U * 1000U)   /* then a check every 6 h */

TaskHandle_t publisher_task_handle;
QueueHandle_t publisher_task_q;

static bool s_connected = true;     /* the task is created once we're connected */

static bool publish(const char *topic, const char *payload, size_t len, bool retain)
{
    if (!s_connected)
    {
        return false;
    }
    cy_mqtt_publish_info_t info =
    {
        .qos = CY_MQTT_QOS1,
        .retain = retain,
        .dup = false,
        .topic = topic,
        .topic_len = (uint16_t)strlen(topic),
        .payload = payload,
        .payload_len = len
    };
    cy_rslt_t result = cy_mqtt_publish(mqtt_connection, &info);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("main: publish failed (0x%0X), reconnecting\n", (int)result);
        s_connected = false;            /* until PUBLISHER_INIT */
        mqtt_task_cmd_t cmd = HANDLE_MQTT_PUBLISH_FAILURE;
        xQueueSend(mqtt_task_q, &cmd, portMAX_DELAY);
        return false;
    }
    return true;
}

void publisher_task(void *pvParameters)
{
    (void)pvParameters;
    static main_in_msg_t m;
    publisher_data_t cmd;

    main_app_init();
    publisher_task_q = xQueueCreate(PUBLISHER_TASK_QUEUE_LENGTH, sizeof(publisher_data_t));
    main_app_connected();
    printf("main: running (client id %s)\n", MQTT_CLIENT_IDENTIFIER);

    TickType_t next_poll = xTaskGetTickCount();
    TickType_t next_sntp = xTaskGetTickCount();
    while (true)
    {
        while (pdTRUE == xQueueReceive(publisher_task_q, &cmd, 0))
        {
            if (cmd.cmd == PUBLISHER_INIT)
            {
                s_connected = true;
                main_app_connected();       /* republish the state after a reconnect */
            }
            else if (cmd.cmd == PUBLISHER_DEINIT)
            {
                s_connected = false;
            }
        }

        /* the date and time from the internet (nodes may send "ts":0) */
        if (s_connected && (int32_t)(xTaskGetTickCount() - next_sntp) >= 0)
        {
            time_t t = ix_sntp_get();
            if (t)
            {
                main_app_set_time(t);
            }
            next_sntp = xTaskGetTickCount() + pdMS_TO_TICKS(t ? SNTP_EVERY_MS : SNTP_RETRY_MS);
        }

        TickType_t now = xTaskGetTickCount();
        TickType_t wait = (int32_t)(next_poll - now) > 0 ? (next_poll - now) : 0;
        if (main_in_q == NULL)
        {
            vTaskDelay(wait ? wait : 1);
        }
        else if (pdTRUE == xQueueReceive(main_in_q, &m, wait))
        {
            main_app_on_message(m.topic, m.topic_len, m.payload, m.payload_len, publish);
            memset(m.payload, 0, m.payload_len);
        }

        if ((int32_t)(xTaskGetTickCount() - next_poll) >= 0)
        {
            next_poll = xTaskGetTickCount() + pdMS_TO_TICKS(POLL_EVERY_MS);
            if (s_connected)
            {
                main_app_poll(publish);
            }
        }
    }
}
