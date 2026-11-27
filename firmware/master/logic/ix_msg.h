/*
 * InhabitX master: the broker messages of README §4, in and out.
 *
 * Plain C on top of ix_model.h. No MQTT library and no sockets: the platform
 * (PC simulator now, E84 later) owns the connection and only passes topics
 * and payloads through these calls, so this file builds unchanged on both.
 *
 * Names and emails from booking requests go into ix_book() and nowhere else:
 * they are not logged, not copied into replies and never published.
 */
#ifndef IX_MSG_H
#define IX_MSG_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IX_BUILDING
#define IX_BUILDING "Z"
#endif
#define IX_TOPIC_ROOT       "inhabitx/" IX_BUILDING "/"
#define IX_TOPIC_SUB        IX_TOPIC_ROOT "#"
#define IX_TOPIC_ROOMS      IX_TOPIC_ROOT "rooms"
#define IX_TOPIC_LOSTFOUND  IX_TOPIC_ROOT "lostfound"

#define IX_MSG_TOPIC_LEN    96
#define IX_MSG_REPLY_LEN    64

/* What to do after an incoming message. */
typedef struct {
    bool reply;                         /* publish reply_payload on reply_topic (not retained) */
    char reply_topic[IX_MSG_TOPIC_LEN];
    char reply_payload[IX_MSG_REPLY_LEN];
    bool changed;                       /* model changed: publish the building state */
    char log[96];                       /* one line for the console, no names/emails; "" = nothing */
} ix_msg_result_t;

/* One incoming message (topic as received, payload not 0-terminated). */
void ix_msg_handle(const char *topic, size_t topic_len,
                   const char *payload, size_t payload_len,
                   time_t now, ix_msg_result_t *out);

/* Building state / found items as published (README §4). Returns the length,
 * or 0 if buf is too small. With stable = true the output leaves out "ts" and
 * gives a ghost room's release time instead of the seconds left, so two calls
 * compare equal as long as nothing changed. */
size_t ix_msg_rooms(time_t now, bool stable, char *buf, size_t n);
size_t ix_msg_lostfound(time_t now, bool stable, char *buf, size_t n);

/* "phone", "keys", ... as in README §4; -1 if unknown. */
const char *ix_msg_cat_key(int cat);
int         ix_msg_cat_from_key(const char *key);

#ifdef __cplusplus
}
#endif
#endif
