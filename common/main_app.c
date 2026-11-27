/* main_app.c - the InhabitX main on a board. See main_app.h. */
#include "main_app.h"
#include "ix_model.h"
#include "ix_msg.h"
#include "main_rooms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct { const char *id; unsigned char floor; unsigned short seats; } s_rooms[] = { MAIN_ROOMS };
#define N_ROOMS ((int)(sizeof s_rooms / sizeof s_rooms[0]))

/* ---- clock --------------------------------------------------------------- */

static long s_offset;           /* added to time(NULL) */
static bool s_time_known;

time_t ix_now(void) { return time(NULL) + s_offset; }
bool main_app_time_known(void) { return s_time_known; }

void main_app_set_time(time_t now)
{
    s_offset = (long)(now - time(NULL));
    if (!s_time_known) printf("main: clock set\n");
    s_time_known = true;
}

/* "ts":1764237600 in a node message: a real time sets (or corrects) the clock. */
static void time_from_node(const char *p, size_t n)
{
    static const char key[] = "\"ts\"";
    for (size_t i = 0; i + sizeof key < n; i++) {
        if (memcmp(p + i, key, sizeof key - 1) != 0) continue;
        size_t j = i + sizeof key - 1;
        while (j < n && (p[j] == ' ' || p[j] == ':')) j++;
        long long ts = 0;
        int digits = 0;
        while (j < n && p[j] >= '0' && p[j] <= '9' && digits < 12) { ts = ts * 10 + (p[j++] - '0'); digits++; }
        if (ts < 1700000000LL) return;                          /* 0 = node has no clock */
        long long drift = ts - (long long)ix_now();
        if (!s_time_known || drift > 30 || drift < -30) main_app_set_time((time_t)ts);
        return;
    }
}

/* ---- publishing ---------------------------------------------------------- */

static char s_last_rooms[4096], s_last_found[4096];

static void publish_state(main_publish_fn publish)
{
    static char stable[4096], msg[4096];
    time_t now = ix_now();
    if (ix_msg_rooms(now, true, stable, sizeof stable) && strcmp(stable, s_last_rooms) != 0) {
        size_t n = ix_msg_rooms(now, false, msg, sizeof msg);
        if (n && publish(IX_TOPIC_ROOMS, msg, n, true)) strcpy(s_last_rooms, stable);
    }
    if (ix_msg_lostfound(now, true, stable, sizeof stable) && strcmp(stable, s_last_found) != 0) {
        size_t n = ix_msg_lostfound(now, false, msg, sizeof msg);
        if (n && publish(IX_TOPIC_LOSTFOUND, msg, n, true)) strcpy(s_last_found, stable);
    }
}

/* ---- the app ------------------------------------------------------------- */

void main_app_init(void)
{
    static bool done;
    if (done) return;               /* safe to call from more than one place */
    done = true;
    setenv("TZ", MAIN_TZ, 1);
    tzset();
    ix_init(0);                     /* no clock needed here: safe before the RTC is up */
    for (int i = 0; i < N_ROOMS; i++) ix_room_add(s_rooms[i].id, s_rooms[i].floor, s_rooms[i].seats);
}

void main_app_connected(void)
{
    s_last_rooms[0] = s_last_found[0] = 0;
}

void main_app_on_message(const char *topic, size_t tl, const char *payload, size_t pl, main_publish_fn publish)
{
    if (tl > 6 && memcmp(topic + tl - 6, "/state", 6) == 0) time_from_node(payload, pl);

    if (!s_time_known && tl > 13 && memcmp(topic + tl - 13, "/bookings/new", 13) == 0) {
        /* without the real date and time every booking would be wrong */
        char copy[1024], req[40] = "";
        size_t n = pl < sizeof copy - 1 ? pl : sizeof copy - 1;
        memcpy(copy, payload, n);
        copy[n] = 0;
        const char *k = strstr(copy, "\"req\"");
        if (k) sscanf(k, "\"req\"%*[ :]\"%39[A-Za-z0-9_-]\"", req);
        memset(copy, 0, sizeof copy);                   /* held a name and an email */
        if (req[0]) {
            char t[IX_MSG_TOPIC_LEN];
            snprintf(t, sizeof t, IX_TOPIC_ROOT "replies/%s", req);
            publish(t, "{\"ok\":false,\"error\":\"time\"}", 28, false);
        }
        printf("main: booking refused, clock not set yet (are the fake nodes running?)\n");
        return;
    }
    ix_msg_result_t r;
    ix_msg_handle(topic, tl, payload, pl, ix_now(), &r);
    if (r.reply) publish(r.reply_topic, r.reply_payload, strlen(r.reply_payload), false);
    if (r.log[0]) printf("main: %s\n", r.log);
}

void main_app_poll(main_publish_fn publish)
{
    static time_t last_tick;
    time_t now = ix_now();
    if (now != last_tick) { ix_tick(now); last_tick = now; }
    publish_state(publish);
}
