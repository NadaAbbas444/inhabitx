/*
 * MQTT connection of the PC simulator: coreMQTT over an OpenSSL TLS socket.
 *
 * Single-threaded: call mqtt_link_poll() from the main loop. It connects,
 * reconnects after a drop, answers keep-alives and hands every incoming
 * message to the callback. On the E84 this file is replaced by the same
 * coreMQTT calls on Infineon's secure sockets (ModusToolbox "mqtt" library);
 * what the messages mean lives in logic/ix_msg.c and stays as it is.
 */
#ifndef MQTT_LINK_H
#define MQTT_LINK_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char host[128];
    int  port;
    char user[64];
    char password[128];
    char cafile[256];       /* "" = the system's CA certificates */
} mqtt_link_conf_t;

/* Reads key = value lines from path (missing file is fine), then the
 * environment (IX_MQTT_HOST, IX_MQTT_PORT, IX_MQTT_USER, IX_MQTT_PASSWORD,
 * IX_MQTT_CAFILE) on top. False with a reason if something is missing. */
bool mqtt_link_load_conf(const char *path, mqtt_link_conf_t *c, char *why, size_t n);

typedef void (*mqtt_link_msg_cb_t)(const char *topic, size_t topic_len,
                                   const char *payload, size_t payload_len);
typedef void (*mqtt_link_up_cb_t)(void);    /* after each (re)connect + subscribe */

void mqtt_link_start(const mqtt_link_conf_t *c, const char *client_id, const char *sub_filter,
                     mqtt_link_msg_cb_t on_msg, mqtt_link_up_cb_t on_up);
void mqtt_link_poll(void);
bool mqtt_link_connected(void);

/* QoS 1. Safe to call from inside the message callback. */
bool mqtt_link_publish(const char *topic, const char *payload, size_t len, bool retain);

#endif
