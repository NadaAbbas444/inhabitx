/* Checks for main_app.c on the PC (no board needed):
 *   cc -I. test_main_app.c main_app.c ix_model.c ix_msg.c -o t && ./t */
#include "main_app.h"
#include "main_rooms.h"
#include "ix_model.h"
#include "ix_msg.h"

#include <stdio.h>
#include <string.h>

static int s_fail;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); s_fail++; } } while (0)

static char s_sent[64][1200];
static int s_n;
static bool rec(const char *topic, const char *payload, size_t len, bool retain)
{
    if (s_n < 64) snprintf(s_sent[s_n++], sizeof s_sent[0], "%s|%d|%.*s", topic, retain, (int)len, payload);
    return true;
}
static const char *last_on(const char *prefix)
{
    for (int i = s_n - 1; i >= 0; i--) if (strncmp(s_sent[i], prefix, strlen(prefix)) == 0) return s_sent[i];
    return "";
}
static void msg(const char *topic, const char *payload)
{
    main_app_on_message(topic, strlen(topic), payload, strlen(payload), rec);
}

int main(void)
{
    main_app_init();
    main_app_init();                                   /* second call does nothing */
    CHECK(ix_room_count() == 5);

    /* last will text matches the room table */
    char will[2048];
    ix_msg_rooms_main_offline(will, sizeof will);
    CHECK(strcmp(will, MAIN_WILL_PAYLOAD) == 0);
    CHECK(strcmp(MAIN_WILL_TOPIC, IX_TOPIC_ROOMS) == 0);

    /* before the clock is set: bookings refused as "time", nothing leaks */
    CHECK(!main_app_time_known());
    msg("inhabitx/Z/bookings/new", "{\"req\":\"a1\",\"room\":\"Z.1.10\",\"from\":\"10:00\",\"to\":\"11:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}");
    CHECK(strcmp(last_on("inhabitx/Z/replies/a1"), "inhabitx/Z/replies/a1|0|{\"ok\":false,\"error\":\"time\"}") == 0);

    /* a node without a clock (ts 0) doesn't set it, a fake node does */
    msg("inhabitx/Z/1/Z.1.10/state", "{\"room\":\"Z.1.10\",\"occupied\":false,\"ts\":0}");
    CHECK(!main_app_time_known());
    struct tm tm;
    time_t t = time(NULL);
    localtime_r(&t, &tm);
    tm.tm_hour = 10; tm.tm_min = 5; tm.tm_sec = 0;
    time_t at10 = mktime(&tm);                         /* 10:05 today, local time */
    char node[128];
    snprintf(node, sizeof node, "{\"room\":\"Z.1.10\",\"occupied\":false,\"ts\":%lld}", (long long)at10);
    msg("inhabitx/Z/1/Z.1.10/state", node);
    CHECK(main_app_time_known());
    CHECK(ix_now() - at10 >= 0 && ix_now() - at10 < 3);
    snprintf(node, sizeof node, "{\"room\":\"Z.1.09\", \"occupied\": true, \"ts\": %lld}", (long long)at10);
    msg("inhabitx/Z/1/Z.1.09/state", node);

    /* publishing: retained building state with both rooms, nothing twice */
    s_n = 0;
    main_app_poll(rec);
    CHECK(strstr(last_on("inhabitx/Z/rooms|1|"), "\"id\":\"Z.1.09\",\"state\":\"occupied\""));
    CHECK(strstr(last_on("inhabitx/Z/rooms|1|"), "\"id\":\"Z.1.10\",\"state\":\"free\""));
    CHECK(strstr(last_on("inhabitx/Z/lostfound|1|"), "\"items\":[]"));
    int before = s_n;
    main_app_poll(rec);
    CHECK(s_n == before);

    /* bookings now work; 3rd with the same email: limit */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"b1\",\"room\":\"Z.1.10\",\"from\":\"11:00\",\"to\":\"12:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}");
    CHECK(strcmp(last_on("inhabitx/Z/replies/b1"), "inhabitx/Z/replies/b1|0|{\"ok\":true}") == 0);
    msg("inhabitx/Z/bookings/new", "{\"req\":\"b2\",\"room\":\"Z.1.10\",\"from\":\"13:00\",\"to\":\"14:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}");
    msg("inhabitx/Z/bookings/new", "{\"req\":\"b3\",\"room\":\"Z.1.10\",\"from\":\"15:00\",\"to\":\"16:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}");
    CHECK(strcmp(last_on("inhabitx/Z/replies/b3"), "inhabitx/Z/replies/b3|0|{\"ok\":false,\"error\":\"limit\"}") == 0);
    main_app_poll(rec);
    CHECK(strstr(last_on("inhabitx/Z/rooms|1|"), "\"free_until\":\"11:00\""));

    /* reconnect: everything is published again */
    before = s_n;
    main_app_connected();
    main_app_poll(rec);
    CHECK(s_n == before + 2);

    /* nothing sent anywhere contains a name or an email */
    for (int i = 0; i < s_n; i++) CHECK(!strstr(s_sent[i], "Ann") && !strstr(s_sent[i], "@"));

    if (s_fail) { printf("%d main_app check(s) failed\n", s_fail); return 1; }
    printf("all main_app checks passed\n");
    return 0;
}
