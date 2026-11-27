/*
 * subscriber_task.c - InhabitX main on the E84: receive everything under inhabitx/Z/.
 *
 * Replaces subscriber_task.c of Infineon's PSOC Edge Wi-Fi MQTT client example
 * (same names, so its mqtt_task.c is unchanged). Subscribes to inhabitx/Z/#
 * and hands each message to the main task through main_in_q.
 *
 * Unlike Infineon's version this never prints a message: booking requests
 * carry names and emails, which must not end up in any log.
 */
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "subscriber_task.h"
#include "mqtt_task.h"
#include "mqtt_client_config.h"
#include "cy_mqtt_api.h"

#include "main_board.h"
#include "main_app.h"

#include <stdio.h>
#include <string.h>

#define MAX_SUBSCRIBE_RETRIES            (3U)
#define SUBSCRIBE_RETRY_INTERVAL_MS      (1000U)
#define SUBSCRIBER_TASK_QUEUE_LENGTH     (2U)

TaskHandle_t subscriber_task_handle;
QueueHandle_t subscriber_task_q;
uint32_t current_device_state = DEVICE_OFF_STATE;   /* kept for Infineon's files */
QueueHandle_t main_in_q;

static cy_mqtt_subscribe_info_t subscribe_info =
{
    .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
    .topic = MQTT_SUB_TOPIC,
    .topic_len = (sizeof(MQTT_SUB_TOPIC) - 1)
};

static void subscribe_to_topic(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    for (uint32_t i = 0; i < MAX_SUBSCRIBE_RETRIES; i++)
    {
        result = cy_mqtt_subscribe(mqtt_connection, &subscribe_info, 1);
        if (result == CY_RSLT_SUCCESS)
        {
            printf("main: listening on %s\n", MQTT_SUB_TOPIC);
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(SUBSCRIBE_RETRY_INTERVAL_MS));
    }
    printf("main: subscribe failed (0x%0X)\n", (int)result);
    mqtt_task_cmd_t cmd = HANDLE_MQTT_SUBSCRIBE_FAILURE;
    xQueueSend(mqtt_task_q, &cmd, portMAX_DELAY);
}

void subscriber_task(void *pvParameters)
{
    (void)pvParameters;
    subscriber_data_t cmd;

    main_app_init();
    if (main_in_q == NULL)
    {
        /* before subscribing: retained messages arrive right away */
        main_in_q = xQueueCreate(MAIN_IN_QUEUE_LEN, sizeof(main_in_msg_t));
    }
    subscriber_task_q = xQueueCreate(SUBSCRIBER_TASK_QUEUE_LENGTH, sizeof(subscriber_data_t));
    subscribe_to_topic();

    while (true)
    {
        if (pdTRUE == xQueueReceive(subscriber_task_q, &cmd, portMAX_DELAY))
        {
            if (cmd.cmd == SUBSCRIBE_TO_TOPIC)
            {
                subscribe_to_topic();
            }
            else if (cmd.cmd == UNSUBSCRIBE_FROM_TOPIC)
            {
                cy_mqtt_unsubscribe(mqtt_connection, (cy_mqtt_unsubscribe_info_t *)&subscribe_info, 1);
            }
        }
    }
}

/* Runs on Infineon's MQTT thread: copy and return, nothing else. */
void mqtt_subscription_callback(cy_mqtt_publish_info_t *msg)
{
    static main_in_msg_t m;     /* only this thread uses it; keeps 1 KB off its stack */

    if (main_in_q == NULL || msg->topic_len >= MAIN_IN_TOPIC_MAX || msg->payload_len > MAIN_IN_PAYLOAD_MAX)
    {
        return;                 /* e.g. our own building state echoed back: not needed */
    }
    memcpy(m.topic, msg->topic, msg->topic_len);
    m.topic_len = msg->topic_len;
    memcpy(m.payload, msg->payload, msg->payload_len);
    m.payload_len = msg->payload_len;
    if (pdTRUE != xQueueSend(main_in_q, &m, 0))
    {
        printf("main: busy, dropped a message\n");
    }
    memset(m.payload, 0, m.payload_len);    /* may have held a name and an email */
}
