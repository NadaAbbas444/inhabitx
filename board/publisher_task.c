/*
 * publisher_task.c - InhabitX node: send "someone there or not" every 2 s.
 *
 * Replaces publisher_task.c of Infineon's PSOC Edge Wi-Fi MQTT client example
 * (mtb-example-psoc-edge-wifi-mqtt-client). Same task, queue and command names,
 * so the example's mqtt_task.c starts and stops it unchanged:
 *   - created by mqtt_task.c once Wi-Fi and the broker are connected
 *   - PUBLISHER_DEINIT when the connection drops: stop sending
 *   - PUBLISHER_INIT after reconnecting: send again
 * Instead of the example's button, a 2 s timeout drives the sending. What is
 * sent comes from node_is_occupied() in node_sensor.c.
 */
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "publisher_task.h"
#include "mqtt_task.h"
#include "mqtt_client_config.h"
#include "cy_mqtt_api.h"

#include "node_message.h"
#include "node_sensor.h"

#include <stdio.h>
#include <string.h>

#define PUBLISHER_TASK_QUEUE_LENGTH     (3U)

TaskHandle_t publisher_task_handle;
QueueHandle_t publisher_task_q;

static cy_mqtt_publish_info_t publish_info =
{
    .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
    .topic = MQTT_PUB_TOPIC,
    .topic_len = (sizeof(MQTT_PUB_TOPIC) - 1),
    .retain = false,        /* never retained: a dead node must turn "offline" */
    .dup = false
};

static void send_state(void)
{
    char payload[64];
    bool occupied = node_is_occupied();
    size_t len = node_message(occupied, 0, payload, sizeof payload);
    if (len == 0)
    {
        return;
    }

    publish_info.payload = payload;
    publish_info.payload_len = len;
    cy_rslt_t result = cy_mqtt_publish(mqtt_connection, &publish_info);
    if (result == CY_RSLT_SUCCESS)
    {
        printf("node: %s -> %s\n", MQTT_PUB_TOPIC, payload);
    }
    else
    {
        printf("node: publish failed (0x%0X), reconnecting\n", (int)result);
        /* let the example's MQTT task reconnect, like the original did */
        mqtt_task_cmd_t mqtt_task_cmd = HANDLE_MQTT_PUBLISH_FAILURE;
        xQueueSend(mqtt_task_q, &mqtt_task_cmd, portMAX_DELAY);
    }
}

void publisher_task(void *pvParameters)
{
    (void)pvParameters;
    publisher_data_t cmd;
    bool sending = true;            /* the task is created once we're connected */
    static bool sensor_ready = false;

    if (!sensor_ready)
    {
        node_sensor_init();
        sensor_ready = true;
    }

    publisher_task_q = xQueueCreate(PUBLISHER_TASK_QUEUE_LENGTH, sizeof(publisher_data_t));
    printf("node: room %s, sending every %u ms on %s\n", NODE_ROOM, (unsigned)NODE_SEND_EVERY_MS, MQTT_PUB_TOPIC);

    TickType_t next = xTaskGetTickCount();
    while (true)
    {
        TickType_t now = xTaskGetTickCount();
        TickType_t wait = (next > now) ? (next - now) : 0;

        if (pdTRUE == xQueueReceive(publisher_task_q, &cmd, wait))
        {
            if (cmd.cmd == PUBLISHER_INIT)
            {
                sending = true;
                next = xTaskGetTickCount();     /* send right away after a reconnect */
            }
            else if (cmd.cmd == PUBLISHER_DEINIT)
            {
                sending = false;
            }
            continue;
        }

        /* 2 s are up */
        next = xTaskGetTickCount() + pdMS_TO_TICKS(NODE_SEND_EVERY_MS);
        if (sending)
        {
            send_state();
        }
    }
}
