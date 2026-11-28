/*
 * node_sensor.c - the room state for the Wi-Fi core, from the microphone.
 *
 * The listening happens on the other core: mtw/Inhabix (proj_cm55) runs the
 * microphone, the classroom model and the occupancy filter, and once a
 * second writes its result into a small mailbox both cores can read
 * (mtw/Inhabix/shared/node_link.h). This file only reads that mailbox;
 * publisher_task.c sends the answer every 2 s.
 *
 * Sent only when the audio core has a fresh, known result: UNKNOWN (warming
 * up, after an audio gap or fault) or a silent audio core sends nothing, so
 * the main shows the room as offline rather than wrongly empty.
 *
 * NODE_NO_MIC (lab_node.ps1 -NoMic): no audio core, always "empty" - only to
 * test Wi-Fi and the broker.
 */
#include "node_sensor.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

#ifndef NODE_NO_MIC
#include "node_link.h"

#define STALE_MS    (5000U)     /* audio core writes every second */

static node_link_t s_last;
static uint32_t s_seen_seq;
static TickType_t s_seen_at;
static bool s_seen;
static int s_said = -1;         /* what we last printed, to print changes only */

static void say(int what, const char *text)
{
    if (s_said != what)
    {
        s_said = what;
        printf("node: %s\n", text);
    }
}
#endif

void node_sensor_init(void)
{
#ifdef NODE_NO_MIC
    printf("node: no microphone in this build (-NoMic), always sending empty\n");
#else
    printf("node: room state comes from the audio core (microphone)\n");
#endif
}

bool node_sensor_ready(void)
{
#ifdef NODE_NO_MIC
    return true;
#else
    node_link_t now;
    TickType_t t = xTaskGetTickCount();
    if (node_link_read(&now))
    {
        if (!s_seen || now.seq != s_seen_seq)
        {
            s_seen = true;
            s_seen_seq = now.seq;
            s_seen_at = t;
        }
        s_last = now;
    }

    if (!s_seen || (TickType_t)(t - s_seen_at) > pdMS_TO_TICKS(STALE_MS))
    {
        say(0, "no news from the audio core, not sending (the main will show the room offline)");
        return false;
    }
    if (!s_last.valid || s_last.state == NODE_LINK_UNKNOWN)
    {
        say(1, "audio says UNKNOWN (warming up or after a gap), not sending");
        return false;
    }
    say(s_last.state == NODE_LINK_OCCUPIED ? 2 : 3,
        s_last.state == NODE_LINK_OCCUPIED ? "audio says OCCUPIED" : "audio says EMPTY");
    return true;
#endif
}

bool node_is_occupied(void)
{
#ifdef NODE_NO_MIC
    return false;
#else
    return s_last.state == NODE_LINK_OCCUPIED;
#endif
}
