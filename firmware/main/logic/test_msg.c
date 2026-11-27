/* Tests for ix_msg.c (broker messages, README §4).
 *   cc -I. test_msg.c ix_msg.c ix_model.c -o test_msg && ./test_msg */
#include "ix_msg.h"
#include "ix_model.h"

#include <stdio.h>
#include <string.h>

static int s_fail;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); s_fail++; } } while (0)

static time_t today(int h, int m)
{
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = 0;
    return mktime(&tm);
}

static ix_msg_result_t r;
static void msg(const char *topic, const char *payload, time_t now)
{
    ix_msg_handle(topic, strlen(topic), payload, strlen(payload), now, &r);
}

static bool nothing_private(void)
{
    char buf[4096];
    time_t now = today(10, 0);
    const char *bad[] = { "Yousef", "yousef@uni.example", "Ann", "ann@uni.example" };
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; i++) {
        if (strstr(r.reply_payload, bad[i]) || strstr(r.log, bad[i]) || strstr(r.reply_topic, bad[i])) return false;
        if (ix_msg_rooms(now, false, buf, sizeof buf) && strstr(buf, bad[i])) return false;
        if (ix_msg_lostfound(now, false, buf, sizeof buf) && strstr(buf, bad[i])) return false;
    }
    return true;
}

int main(void)
{
    time_t now = today(10, 0);
    ix_init(now);
    ix_room_add("Z.1.09", 1, 8);
    ix_room_add("Z.1.10", 1, 6);
    char buf[4096];

    /* M1: node messages */
    msg("inhabitx/Z/1/Z.1.09/state", "{\"room\":\"Z.1.09\",\"occupied\":true,\"ts\":1764237600}", now);
    ix_status_t st;
    ix_room_status(ix_room_find("Z.1.09"), now, &st);
    CHECK(st.state == IX_OCCUPIED && r.changed && !r.reply);
    msg("inhabitx/Z/1/Z.1.10/state", "{ \"occupied\" : false }", now);
    ix_room_status(ix_room_find("Z.1.10"), now, &st);
    CHECK(st.state == IX_FREE);
    msg("inhabitx/Z/1/Z.1.09/state", "{\"room\":\"Z.1.10\",\"occupied\":false}", now);   /* room mismatch */
    ix_room_status(ix_room_find("Z.1.09"), now, &st);
    CHECK(st.state == IX_OCCUPIED);
    msg("inhabitx/Z/1/Z.1.09/state", "{\"occupied\":\"no\"}", now);                       /* not a bool */
    msg("inhabitx/Z/1/Z.1.99/state", "{\"occupied\":true}", now);                         /* unknown room */
    msg("inhabitx/Y/1/Z.1.09/state", "{\"occupied\":false}", now);                        /* other building */
    msg("inhabitx/Z/1/Z.1.09/other", "{\"occupied\":false}", now);
    ix_room_status(ix_room_find("Z.1.09"), now, &st);
    CHECK(st.state == IX_OCCUPIED);
    ix_room_status(ix_room_find("Z.1.10"), now + IX_OFFLINE_AFTER_S + 1, &st);
    CHECK(st.state == IX_OFFLINE);

    /* M6: bookings */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"k3j9x0a1\",\"room\":\"Z.1.10\",\"from\":\"11:00\",\"to\":\"12:00\","
        "\"people\":4,\"name\":\"Yousef\",\"email\":\"yousef@uni.example\"}", now);
    CHECK(r.reply && strcmp(r.reply_topic, "inhabitx/Z/replies/k3j9x0a1") == 0);
    CHECK(strcmp(r.reply_payload, "{\"ok\":true}") == 0 && r.changed);
    CHECK(nothing_private());
    ix_room_status(ix_room_find("Z.1.10"), now, &st);
    CHECK(st.state == IX_FREE && st.free_until == today(11, 0));

    /* same request delivered twice: same answer, booked once */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"k3j9x0a1\",\"room\":\"Z.1.10\",\"from\":\"11:00\",\"to\":\"12:00\","
        "\"people\":4,\"name\":\"Yousef\",\"email\":\"yousef@uni.example\"}", now);
    CHECK(r.reply && strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    CHECK(ix_active_bookings_for("yousef@uni.example", now) == 1);

    msg("inhabitx/Z/bookings/new", "{\"req\":\"b2\",\"room\":\"Z.1.10\",\"from\":\"11:30\",\"to\":\"12:30\","
        "\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":false,\"error\":\"overlap\"}") == 0 && !r.changed);
    msg("inhabitx/Z/bookings/new", "{\"req\":\"b3\",\"room\":\"Z.1.10\",\"from\":\"13:00\",\"to\":\"14:00\","
        "\"people\":2,\"name\":\"Yousef\",\"email\":\"Yousef@Uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    msg("inhabitx/Z/bookings/new", "{\"req\":\"b4\",\"room\":\"Z.1.10\",\"from\":\"15:00\",\"to\":\"16:00\","
        "\"people\":2,\"name\":\"Yousef\",\"email\":\"yousef@uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":false,\"error\":\"limit\"}") == 0);
    CHECK(nothing_private());

    /* errors */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e1\",\"room\":\"Z.1.10\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":2,\"name\":\" \",\"email\":\"ann@uni.example\"}", now);
    CHECK(strstr(r.reply_payload, "\"name\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e2\",\"room\":\"Z.1.10\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann\"}", now);
    CHECK(strstr(r.reply_payload, "\"email\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e3\",\"room\":\"Z.1.10\",\"from\":\"16:10\",\"to\":\"17:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strstr(r.reply_payload, "\"time\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e4\",\"room\":\"Z.1.77\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strstr(r.reply_payload, "\"time\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e5\",\"room\":\"Z.1.10\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":7,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strstr(r.reply_payload, "\"seats\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"e6\",\"room\":\"Z.1.10\",\"from\":\"09:00\",\"to\":\"09:30\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strstr(r.reply_payload, "\"time\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"a/b\",\"room\":\"Z.1.10\"}", now);           /* unsafe req: no reply */
    CHECK(!r.reply);
    msg("inhabitx/Z/bookings/new", "not json", now);
    CHECK(!r.reply);

    /* escapes, and a key inside a value is not a key */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"u1\",\"room\":\"Z.1.09\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":2,"
        "\"name\":\"J\\u00fcrgen \\\"J\\\" \\ud83d\\ude00\",\"email\":\"\\\"email\\\":x\"}", now);
    CHECK(strstr(r.reply_payload, "\"email\""));
    msg("inhabitx/Z/bookings/new", "{\"req\":\"u2\",\"room\":\"Z.1.09\",\"from\":\"16:00\",\"to\":\"17:00\",\"people\":2,"
        "\"name\":\"J\\u00fcrgen \\\"J\\\" \\ud83d\\ude00\",\"email\":\"jj@uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    const ix_booking_t *bk[4];
    int nb = ix_room_bookings(ix_room_find("Z.1.09"), now, bk, 4);
    CHECK(nb == 1 && strcmp(bk[0]->name, "J\xc3\xbcrgen \"J\" \xf0\x9f\x98\x80") == 0);

    /* a booking until midnight ends at "24:00", not "00:00" */
    msg("inhabitx/Z/bookings/new", "{\"req\":\"m1\",\"room\":\"Z.1.09\",\"from\":\"23:00\",\"to\":\"24:00\",\"people\":2,\"name\":\"Ann\",\"email\":\"ann2@uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    CHECK(ix_msg_rooms(now, false, buf, sizeof buf) > 0 && strstr(buf, "[\"23:00\",\"24:00\"]"));
    ix_node_update("Z.1.09", true, today(23, 30));
    CHECK(ix_msg_rooms(today(23, 30), false, buf, sizeof buf) > 0 && strstr(buf, "\"until\":\"24:00\""));
    ix_node_update("Z.1.09", true, now);

    /* M4: building state */
    CHECK(ix_msg_rooms(now, false, buf, sizeof buf) > 0);
    CHECK(strstr(buf, "{\"id\":\"Z.1.10\",\"state\":\"free\",\"free_until\":\"11:00\",\"seats\":6,\"floor\":1,"
                      "\"slots\":[[\"11:00\",\"12:00\"],[\"13:00\",\"14:00\"]]}"));
    CHECK(strstr(buf, "\"state\":\"occupied\""));
    CHECK(ix_msg_rooms(now, false, buf, 20) == 0);          /* too small */

    /* ghost: countdown in the message, stable form doesn't change per second */
    ix_node_update("Z.1.10", false, now);
    msg("inhabitx/Z/bookings/new", "{\"req\":\"g1\",\"room\":\"Z.1.10\",\"from\":\"10:00\",\"to\":\"10:45\",\"people\":1,\"name\":\"Ann\",\"email\":\"ann@uni.example\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    char a[4096], b[4096];
    ix_msg_rooms(now + 1, false, buf, sizeof buf);
    CHECK(strstr(buf, "\"state\":\"ghost\",\"release_in_s\":"));
    CHECK(strstr(buf, "\"until\":\"10:45\""));
    ix_node_update("Z.1.09", true, now + 2);
    ix_node_update("Z.1.10", false, now + 2);
    ix_msg_rooms(now + 1, true, a, sizeof a);
    ix_msg_rooms(now + 3, true, b, sizeof b);
    CHECK(strcmp(a, b) == 0 && !strstr(a, "\"ts\""));

    /* M6: found items */
    msg("inhabitx/Z/lostfound/new", "{\"req\":\"p0c7m2zz\",\"room\":\"Z.1.09\",\"cat\":\"keys\"}", now);
    CHECK(strcmp(r.reply_topic, "inhabitx/Z/replies/p0c7m2zz") == 0 && strcmp(r.reply_payload, "{\"ok\":true}") == 0);
    msg("inhabitx/Z/lostfound/new", "{\"req\":\"p0c7m2zz\",\"room\":\"Z.1.09\",\"cat\":\"keys\"}", now);   /* duplicate */
    msg("inhabitx/Z/lostfound/new", "{\"req\":\"p2\",\"room\":\"Z.1.09\",\"cat\":\"car\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":false,\"error\":\"cat\"}") == 0);
    msg("inhabitx/Z/lostfound/new", "{\"req\":\"p3\",\"room\":\"X\",\"cat\":\"bag\"}", now);
    CHECK(strcmp(r.reply_payload, "{\"ok\":false,\"error\":\"room\"}") == 0);
    ix_found_add(ix_room_find("Z.1.10"), IX_CAT_UMBRELLA, now - 3600);
    ix_found_add(ix_room_find("Z.1.10"), IX_CAT_BAG, now - 8 * 86400);   /* too old */
    CHECK(ix_msg_lostfound(now, false, buf, sizeof buf) > 0);
    char want[256];
    snprintf(want, sizeof want, "{\"ts\":%lld,\"items\":[{\"room\":\"Z.1.09\",\"cat\":\"keys\",\"ts\":%lld,\"online\":true},"
             "{\"room\":\"Z.1.10\",\"cat\":\"umbrella\",\"ts\":%lld,\"online\":false}]}",
             (long long)now, (long long)now, (long long)(now - 3600));
    CHECK(strcmp(buf, want) == 0);
    CHECK(nothing_private());

    /* last will: every room offline, no names */
    CHECK(ix_msg_rooms_main_offline(buf, sizeof buf) > 0);
    CHECK(strcmp(buf, "{\"ts\":0,\"main\":\"offline\",\"rooms\":[{\"id\":\"Z.1.09\",\"state\":\"offline\",\"seats\":8,\"floor\":1},"
                      "{\"id\":\"Z.1.10\",\"state\":\"offline\",\"seats\":6,\"floor\":1}]}") == 0);

    if (s_fail) { printf("%d message check(s) failed\n", s_fail); return 1; }
    printf("all message checks passed\n");
    return 0;
}
