/*
 * publisher_task.c - InhabitX main with screen, Wi-Fi core: the relay.
 *
 * Replaces publisher_task.c of Infineon's PSOC Edge Wi-Fi MQTT client example
 * (same names: its mqtt_task.c creates this task once Wi-Fi and the broker
 * are up, sends PUBLISHER_DEINIT when the connection drops and
 * PUBLISHER_INIT after reconnecting).
 *
 * The main itself runs on the screen core. This task only carries messages:
 *   broker -> screen core:  everything from main_in_q (subscriber_task.c),
 *                           plus "connected" / "disconnected", and the
 *                           time from the internet (ix_sntp.c) as a
 *                           message on ".../_clock"
 *   screen core -> broker:  publish requests from ix_ipc_cm33_down_q
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
#include "ix_ipc.h"
#include "ix_sntp.h"

#include <stdio.h>
#include <string.h>

#define PUBLISHER_TASK_QUEUE_LENGTH     (3U)
#define WAIT_MS                         (20U)
#define SNTP_RETRY_MS                   (60U * 1000U)          /* until the first answer */
#define SNTP_EVERY_MS                   (6U * 3600U * 1000U)   /* then a check every 6 h */

TaskHandle_t publisher_task_handle;
QueueHandle_t publisher_task_q;

static bool s_connected = true;     /* the task is created once we're connected */

static void publish(const ix_ipc_down_t *d)
{
    if (!s_connected)
    {
        return;                     /* the screen core republishes after "connected" */
    }
    cy_mqtt_publish_info_t info =
    {
        .qos = CY_MQTT_QOS1,
        .retain = d->retain != 0,
        .dup = false,
        .topic = d->topic,
        .topic_len = d->topic_len,
        .payload = d->payload,
        .payload_len = d->payload_len
    };
    cy_rslt_t result = cy_mqtt_publish(mqtt_connection, &info);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("relay: publish failed (0x%0X), reconnecting\n", (int)result);
        s_connected = false;
        ix_ipc_cm33_send(IX_IPC_DISCONNECTED, NULL, 0, NULL, 0);
        mqtt_task_cmd_t cmd = HANDLE_MQTT_PUBLISH_FAILURE;
        xQueueSend(mqtt_task_q, &cmd, portMAX_DELAY);
    }
}

void publisher_task(void *pvParameters)
{
    (void)pvParameters;
    static main_in_msg_t m;
    static ix_ipc_down_t d;
    publisher_data_t cmd;

    publisher_task_q = xQueueCreate(PUBLISHER_TASK_QUEUE_LENGTH, sizeof(publisher_data_t));
    printf("relay: connected, passing messages to the screen core\n");
    ix_ipc_cm33_send(IX_IPC_CONNECTED, NULL, 0, NULL, 0);
    TickType_t next_sntp = xTaskGetTickCount();

    while (true)
    {
        /* the date and time from the internet, for the main on the screen core */
        if (s_connected && (int32_t)(xTaskGetTickCount() - next_sntp) >= 0)
        {
            static const char topic[] = "inhabitx/Z/_clock";
            char payload[32];
            time_t t = ix_sntp_get();
            if (t)
            {
                int n = snprintf(payload, sizeof payload, "{\"ts\":%lld}", (long long)t);
                ix_ipc_cm33_send(IX_IPC_MSG, topic, sizeof topic - 1, payload, (size_t)n);
            }
            next_sntp = xTaskGetTickCount() + pdMS_TO_TICKS(t ? SNTP_EVERY_MS : SNTP_RETRY_MS);
        }

        while (pdTRUE == xQueueReceive(publisher_task_q, &cmd, 0))
        {
            if (cmd.cmd == PUBLISHER_INIT)
            {
                s_connected = true;
                ix_ipc_cm33_send(IX_IPC_CONNECTED, NULL, 0, NULL, 0);
            }
            else if (cmd.cmd == PUBLISHER_DEINIT)
            {
                s_connected = false;
                ix_ipc_cm33_send(IX_IPC_DISCONNECTED, NULL, 0, NULL, 0);
            }
        }

        /* broker -> screen core */
        if (main_in_q != NULL && pdTRUE == xQueueReceive(main_in_q, &m, pdMS_TO_TICKS(WAIT_MS)))
        {
            if (!ix_ipc_cm33_send(IX_IPC_MSG, m.topic, m.topic_len, m.payload, m.payload_len))
            {
                printf("relay: screen core not answering, message dropped\n");
            }
            memset(m.payload, 0, m.payload_len);
        }
        else if (main_in_q == NULL)
        {
            vTaskDelay(pdMS_TO_TICKS(WAIT_MS));
        }

        /* screen core -> broker */
        while (ix_ipc_cm33_down_q != NULL && pdTRUE == xQueueReceive((QueueHandle_t)ix_ipc_cm33_down_q, &d, 0))
        {
            publish(&d);
        }
    }
}
