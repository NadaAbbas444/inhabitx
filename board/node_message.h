/* node_message.h - which room this board is, and the message it sends (README §3). */
#ifndef NODE_MESSAGE_H_
#define NODE_MESSAGE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ---- this board's room: change these three for another room --------- */
#define NODE_BUILDING   "Z"
#define NODE_FLOOR      "1"
#define NODE_ROOM       "Z.1.09"

/* inhabitx/Z/1/Z.1.09/state */
#define NODE_TOPIC      "inhabitx/" NODE_BUILDING "/" NODE_FLOOR "/" NODE_ROOM "/state"
/* one client id per board; never the main's (inhabitx-main-Z) */
#define NODE_CLIENT_ID  "inhabitx-node-" NODE_ROOM

#define NODE_SEND_EVERY_MS  2000u

/* Writes {"room":"Z.1.09","occupied":true,"ts":0} into buf.
 * ts = Unix time if the board knows it, else 0 (the main uses its own clock).
 * Returns the length, or 0 if buf is too small. */
size_t node_message(bool occupied, uint32_t ts, char *buf, size_t n);

#endif /* NODE_MESSAGE_H_ */
