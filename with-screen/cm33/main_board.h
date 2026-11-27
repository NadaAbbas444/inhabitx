/*
 * main_board.h - InhabitX main on the E84: how incoming messages leave the MQTT thread.
 *
 * Infineon's MQTT thread calls mqtt_subscription_callback() (subscriber_task.c),
 * which only copies the message into main_in_q. publisher_task.c takes it
 * from there, so the MQTT thread never waits on anything else.
 */
#ifndef MAIN_BOARD_H_
#define MAIN_BOARD_H_

#include "FreeRTOS.h"
#include "queue.h"
#include <stddef.h>

#define MAIN_IN_TOPIC_MAX     96
#define MAIN_IN_PAYLOAD_MAX   1024     /* bookings are ~200 bytes */
#define MAIN_IN_QUEUE_LEN     8

typedef struct {
    char   topic[MAIN_IN_TOPIC_MAX];
    size_t topic_len;
    char   payload[MAIN_IN_PAYLOAD_MAX];
    size_t payload_len;
} main_in_msg_t;

extern QueueHandle_t main_in_q;

#endif /* MAIN_BOARD_H_ */
