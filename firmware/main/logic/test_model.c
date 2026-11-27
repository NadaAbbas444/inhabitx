/* Tests for ix_model.c (the M2 checks from TASKS.md + a few more).
 *   cc -I. test_model.c ix_model.c -o test_model && ./test_model */
#include "ix_model.h"

#include <stdio.h>
#include <string.h>

static int s_fail, s_released;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); s_fail++; } } while (0)

static void on_ev(ix_event_t ev, int room) { if (ev == IX_EV_RELEASED) s_released++; }

static time_t today(int h, int m)
{
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = 0;
    return mktime(&tm);
}

static void setup(time_t now)
{
    ix_init(now);
    ix_set_event_cb(on_ev);
    s_released = 0;
    ix_room_add("Z.1.09", 1, 8);
    ix_room_add("Z.1.10", 1, 6);
    ix_node_update("Z.1.09", false, now);
    ix_node_update("Z.1.10", false, now);
}

static void run(time_t from, time_t to, bool occ09)
{
    for (time_t t = from; t <= to; t++) {
        if (t % 2 == 0) { ix_node_update("Z.1.09", occ09, t); ix_node_update("Z.1.10", false, t); }
        ix_tick(t);
    }
}

int main(void)
{
    time_t t0 = today(10, 0);
    ix_status_t st;

    /* 1. booking, nobody there -> ghost, then released after the timeout */
    setup(t0);
    CHECK(ix_book(0, t0, t0 + 3600, 2, "Sam", "sam@edu.aau.at", t0) == IX_BOOK_OK);
    run(t0, t0 + 5, false);
    ix_room_status(0, t0 + 5, &st);
    CHECK(st.state == IX_GHOST);
    run(t0 + 6, t0 + IX_GHOST_RELEASE_S + 1, false);
    ix_room_status(0, t0 + IX_GHOST_RELEASE_S + 1, &st);
    CHECK(st.state == IX_FREE);
    CHECK(s_released == 1);

    /* 2. booking, people arrive before the timeout -> not released */
    setup(t0);
    ix_book(0, t0, t0 + 3600, 2, "Sam", "sam@edu.aau.at", t0);
    run(t0, t0 + 10, false);
    run(t0 + 11, t0 + IX_GHOST_RELEASE_S + 20, true);
    ix_room_status(0, t0 + IX_GHOST_RELEASE_S + 20, &st);
    CHECK(st.state == IX_OCCUPIED);
    CHECK(s_released == 0);
    run(t0 + IX_GHOST_RELEASE_S + 21, t0 + IX_GHOST_RELEASE_S + 30, false);
    ix_room_status(0, t0 + IX_GHOST_RELEASE_S + 30, &st);
    CHECK(st.state == IX_BOOKED);           /* checked in, stepped out */

    /* 3. third active booking with the same email -> rejected (case-insensitive) */
    setup(t0);
    CHECK(ix_book(0, t0 + 3600, t0 + 7200, 1, "A", "felix@edu.aau.at", t0) == IX_BOOK_OK);
    CHECK(ix_book(1, t0 + 3600, t0 + 7200, 1, "A", "Felix@EDU.aau.at ", t0) == IX_BOOK_OK);
    CHECK(ix_book(0, t0 + 7200, t0 + 9000, 1, "A", "felix@edu.aau.at", t0) == IX_BOOK_ERR_LIMIT);

    /* 4. booking "now" in a slot that started earlier: grace starts at booking time */
    setup(t0 + 300);
    CHECK(ix_book(0, t0, t0 + 3600, 1, "B", "b@aau.at", t0 + 300) == IX_BOOK_OK);
    run(t0 + 300, t0 + 305, false);
    ix_room_status(0, t0 + 305, &st);
    CHECK(st.state == IX_GHOST && st.release_in_s > 0);
    CHECK(s_released == 0);

    /* 5. overlaps, seats, limits, find_rooms order */
    setup(t0);
    ix_book(0, t0 + 3600, t0 + 7200, 1, "C", "c@aau.at", t0);
    const ix_booking_t *clash;
    CHECK(ix_room_check(0, t0 + 5400, t0 + 9000, 1, t0, &clash) == IX_BOOK_ERR_OVERLAP && clash);
    CHECK(ix_room_check(1, t0 + 3600, t0 + 7200, 7, t0, NULL) == IX_BOOK_ERR_SEATS);
    CHECK(ix_room_check(1, t0, t0 + 5 * 3600, 1, t0, NULL) == IX_BOOK_ERR_TIME);
    CHECK(ix_book(1, t0, t0 + 3600, 1, "", "x@aau.at", t0) == IX_BOOK_ERR_NAME);
    CHECK(ix_book(1, t0, t0 + 3600, 1, "X", "not-an-email", t0) == IX_BOOK_ERR_EMAIL);
    int out[4];
    int n = ix_find_rooms(t0, t0 + 1800, 2, t0, out, 4);
    CHECK(n == 2 && out[0] == 1 && out[1] == 0);    /* smallest first */

    /* 6. offline after 10 s of silence */
    setup(t0);
    ix_room_status(0, t0 + IX_OFFLINE_AFTER_S + 1, &st);
    CHECK(st.state == IX_OFFLINE);

    /* 7. lost & found: search + 7-day expiry */
    setup(t0);
    ix_found_add(1, IX_CAT_UMBRELLA, t0);
    ix_found_add(0, IX_CAT_PHONE, t0 - 6 * 86400);
    const ix_found_t *f[4];
    CHECK(ix_found_search(1, IX_CAT_UMBRELLA, f, 4) == 1);
    CHECK(ix_found_search(-1, -1, f, 4) == 2 && f[0]->cat == IX_CAT_UMBRELLA);   /* newest first */
    ix_tick(t0 + 2 * 86400);
    CHECK(ix_found_search(-1, -1, f, 4) == 1);

    printf(s_fail ? "%d check(s) failed\n" : "all model checks passed\n", s_fail);
    return s_fail != 0;
}
