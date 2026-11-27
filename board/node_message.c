/* node_message.c - builds the message a node sends. See node_message.h. */
#include "node_message.h"

#include <stdio.h>

size_t node_message(bool occupied, uint32_t ts, char *buf, size_t n)
{
    int k = snprintf(buf, n, "{\"room\":\"%s\",\"occupied\":%s,\"ts\":%lu}",
                     NODE_ROOM, occupied ? "true" : "false", (unsigned long)ts);
    return (k > 0 && (size_t)k < n) ? (size_t)k : 0;
}
