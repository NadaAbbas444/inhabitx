/*
 * main_app.h - the InhabitX main on a board, without the board-specific parts.
 *
 * Same job as sim/main.c on the laptop: rooms, bookings, lost & found
 * (ix_model), broker messages (ix_msg), and publishing the building state
 * when it changes. The board code only connects to the broker, passes every
 * incoming message to main_app_on_message() and calls main_app_poll()
 * about twice a second. Publishing goes through the function it hands in.
 *
 * Plain C, no RTOS and no MQTT library: builds for the E84 (both cores),
 * the XIAO ESP32-S3 and the PC.
 */
#ifndef MAIN_APP_H_
#define MAIN_APP_H_

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Client id of the main: only one main may be connected at a time. */
#define MAIN_CLIENT_ID      "inhabitx-main-Z"

/* Local time zone (POSIX TZ): Klagenfurt. Bookings use local time. */
#define MAIN_TZ             "CET-1CEST,M3.5.0,M10.5.0/3"

/* Publishes on the broker; true if it was handed to the MQTT client. */
typedef bool (*main_publish_fn)(const char *topic, const char *payload, size_t len, bool retain);

/* At start-up, before connecting. Calling it again does nothing. */
void main_app_init(void);

/* The last will to register when connecting (retained): MAIN_WILL_TOPIC /
 * MAIN_WILL_PAYLOAD in main_rooms.h, every room offline and "main":"offline". */

/* After every (re)connect + subscribe: republish everything. */
void main_app_connected(void);

/* Every message received on inhabitx/Z/# (payload not 0-terminated).
 * Never logs names or emails. */
void main_app_on_message(const char *topic, size_t topic_len,
                         const char *payload, size_t payload_len, main_publish_fn publish);

/* About twice a second: ghost release, offline rooms, publishing changes. */
void main_app_poll(main_publish_fn publish);

/* Current Unix time. The board has no clock of its own, so it is set from
 * the "ts" of the first node message that carries a real time (the fake
 * nodes on the laptop send one every 2 s). Until then main_app_time_known()
 * is false and bookings are refused as "clock". */
time_t ix_now(void);
bool main_app_time_known(void);

/* Optional: set the time from somewhere else (SNTP, RTC). */
void main_app_set_time(time_t now);

#ifdef __cplusplus
}
#endif
#endif /* MAIN_APP_H_ */
