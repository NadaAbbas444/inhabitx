/* InhabitX main logic. See ix_model.h. */
#include "ix_model.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    ix_room_info_t info;
    bool   occupied;
    time_t last_msg;        /* 0 = never heard from the node */
} room_t;

static room_t       s_rooms[IX_MAX_ROOMS];
static int          s_room_count;
static ix_booking_t s_bookings[IX_MAX_BOOKINGS];
static ix_found_t   s_found[IX_MAX_FOUND];
static ix_event_cb_t s_event_cb;
static int8_t       s_hist[IX_HIST_BUCKETS];

static void emit(ix_event_t ev, int room)
{
    if (s_event_cb) s_event_cb(ev, room);
}

void ix_set_event_cb(ix_event_cb_t cb) { s_event_cb = cb; }

void ix_init(time_t now)
{
    (void)now;
    memset(s_rooms, 0, sizeof s_rooms);
    memset(s_bookings, 0, sizeof s_bookings);
    memset(s_found, 0, sizeof s_found);
    memset(s_hist, -1, sizeof s_hist);
    s_room_count = 0;
}

/* ---- rooms ------------------------------------------------------------ */

int ix_room_add(const char *id, uint8_t floor, uint16_t seats)
{
    if (s_room_count >= IX_MAX_ROOMS) return -1;
    room_t *r = &s_rooms[s_room_count];
    memset(r, 0, sizeof *r);
    snprintf(r->info.id, sizeof r->info.id, "%s", id);
    r->info.floor = floor;
    r->info.seats = seats;
    return s_room_count++;
}

int ix_room_count(void) { return s_room_count; }

const ix_room_info_t *ix_room(int idx)
{
    return (idx >= 0 && idx < s_room_count) ? &s_rooms[idx].info : NULL;
}

int ix_room_find(const char *id)
{
    for (int i = 0; i < s_room_count; i++)
        if (strcmp(s_rooms[i].info.id, id) == 0) return i;
    return -1;
}

static bool is_offline(int idx, time_t now)
{
    const room_t *r = &s_rooms[idx];
    return r->last_msg == 0 || now - r->last_msg > IX_OFFLINE_AFTER_S;
}

static ix_booking_t *current_booking(int room, time_t now)
{
    for (int i = 0; i < IX_MAX_BOOKINGS; i++) {
        ix_booking_t *b = &s_bookings[i];
        if (b->used && b->room == room && b->start <= now && now < b->end) return b;
    }
    return NULL;
}

static ix_booking_t *next_booking(int room, time_t now)
{
    ix_booking_t *best = NULL;
    for (int i = 0; i < IX_MAX_BOOKINGS; i++) {
        ix_booking_t *b = &s_bookings[i];
        if (b->used && b->room == room && b->start > now && (!best || b->start < best->start))
            best = b;
    }
    return best;
}

void ix_room_status(int idx, time_t now, ix_status_t *out)
{
    memset(out, 0, sizeof *out);
    if (idx < 0 || idx >= s_room_count) { out->state = IX_OFFLINE; return; }

    if (is_offline(idx, now)) { out->state = IX_OFFLINE; return; }

    const ix_booking_t *b = current_booking(idx, now);
    if (s_rooms[idx].occupied) {
        out->state = IX_OCCUPIED;
        out->until = b ? b->end : 0;
    } else if (b && !b->checked_in) {
        out->state = IX_GHOST;
        int32_t left = (int32_t)(b->grace_from + IX_GHOST_RELEASE_S - now);
        out->release_in_s = left < 0 ? 0 : left;
        out->until = b->end;
    } else if (b) {
        out->state = IX_BOOKED;
        out->until = b->end;
    } else {
        out->state = IX_FREE;
        const ix_booking_t *n = next_booking(idx, now);
        out->free_until = n ? n->start : 0;
    }
}

const char *ix_state_name(ix_state_t s)
{
    switch (s) {
    case IX_FREE:     return "free";
    case IX_BOOKED:   return "booked";
    case IX_GHOST:    return "ghost";
    case IX_OCCUPIED: return "occupied";
    default:          return "offline";
    }
}

void ix_node_update(const char *room_id, bool occupied, time_t now)
{
    int i = ix_room_find(room_id);
    if (i < 0) return;
    s_rooms[i].occupied = occupied;
    s_rooms[i].last_msg = now;
}

void ix_tick(time_t now)
{
    for (int i = 0; i < IX_MAX_BOOKINGS; i++) {
        ix_booking_t *b = &s_bookings[i];
        if (!b->used) continue;
        if (b->end <= now) { b->used = false; continue; }      /* over */
        if (b->start > now) continue;                           /* not yet */
        if (is_offline(b->room, now)) continue;                 /* don't judge blind */

        if (!b->checked_in && s_rooms[b->room].occupied) {
            b->checked_in = true;
            emit(IX_EV_CHECKED_IN, b->room);
        } else if (!b->checked_in && now - b->grace_from >= IX_GHOST_RELEASE_S) {
            b->used = false;
            emit(IX_EV_RELEASED, b->room);
        }
    }
    for (int i = 0; i < IX_MAX_FOUND; i++)
        if (s_found[i].used && now - s_found[i].when > IX_FOUND_KEEP_S)
            s_found[i].used = false;

    int b = ix_hist_bucket(now);
    if (b >= 0) s_hist[b] = (int8_t)ix_occupancy_now(now);
}

/* ---- occupancy history ------------------------------------------------ */

static time_t today_at(time_t now, int hour);

int ix_hist_bucket(time_t now)
{
    time_t open = today_at(now, IX_DAY_OPEN_H);
    if (now < open) return -1;
    long b = (long)((now - open) / (15 * 60));
    return b < IX_HIST_BUCKETS ? (int)b : -1;
}

int  ix_hist_get(int b) { return (b >= 0 && b < IX_HIST_BUCKETS) ? s_hist[b] : -1; }
void ix_hist_set(int b, int pct) { if (b >= 0 && b < IX_HIST_BUCKETS) s_hist[b] = (int8_t)pct; }

int ix_occupancy_now(time_t now)
{
    int online = 0, used = 0;
    for (int i = 0; i < s_room_count; i++) {
        if (is_offline(i, now)) continue;
        online++;
        if (s_rooms[i].occupied) used++;
    }
    return online ? (used * 100) / online : 0;
}

/* ---- time helpers ----------------------------------------------------- */

static time_t today_at(time_t now, int hour)
{
    struct tm tm;
    localtime_r(&now, &tm);
    tm.tm_hour = hour; tm.tm_min = 0; tm.tm_sec = 0;
    return mktime(&tm);
}

time_t ix_day_first_slot(time_t now)
{
    time_t open = today_at(now, IX_DAY_OPEN_H);
    if (now < open) return open;
    return open + ((now - open) / IX_SLOT_S) * IX_SLOT_S;
}

time_t ix_day_close(time_t now) { return today_at(now, IX_DAY_CLOSE_H); }

/* ---- bookings --------------------------------------------------------- */

static void lower_copy(char *dst, size_t n, const char *src)
{
    size_t i = 0;
    while (*src && isspace((unsigned char)*src)) src++;
    for (; src[i] && i + 1 < n; i++) dst[i] = (char)tolower((unsigned char)src[i]);
    dst[i] = 0;
    while (i > 0 && isspace((unsigned char)dst[i - 1])) dst[--i] = 0;
}

static bool valid_email(const char *e)
{
    const char *at = strchr(e, '@');
    if (!at || at == e || strchr(at + 1, '@')) return false;
    const char *dot = strrchr(at, '.');
    if (!dot || dot == at + 1 || dot[1] == 0) return false;
    for (const char *p = e; *p; p++)
        if (isspace((unsigned char)*p)) return false;
    return true;
}

int ix_active_bookings_for(const char *email, time_t now)
{
    char key[IX_EMAIL_LEN];
    lower_copy(key, sizeof key, email);
    int n = 0;
    for (int i = 0; i < IX_MAX_BOOKINGS; i++)
        if (s_bookings[i].used && s_bookings[i].end > now && strcmp(s_bookings[i].email, key) == 0)
            n++;
    return n;
}

static ix_book_err_t room_check_strict(int room, time_t start, time_t end, uint8_t people,
                                       time_t now, const ix_booking_t **clash)
{
    if (clash) *clash = NULL;
    if (room < 0 || room >= s_room_count) return IX_BOOK_ERR_TIME;
    if (end <= start || end - start > IX_MAX_BOOKING_S) return IX_BOOK_ERR_TIME;
    if (start < ix_day_first_slot(now) || end > ix_day_close(now)) return IX_BOOK_ERR_TIME;
    if (people > s_rooms[room].info.seats) return IX_BOOK_ERR_SEATS;

    for (int i = 0; i < IX_MAX_BOOKINGS; i++) {
        const ix_booking_t *b = &s_bookings[i];
        if (b->used && b->room == room && b->start < end && start < b->end) {
            if (clash) *clash = b;
            return IX_BOOK_ERR_OVERLAP;
        }
    }
    if (start <= now && (s_rooms[room].occupied || is_offline(room, now)))
        return IX_BOOK_ERR_BUSY;
    return IX_BOOK_OK;
}

ix_book_err_t ix_room_check(int room, time_t start, time_t end, uint8_t people,
                            time_t now, const ix_booking_t **clash)
{
    ix_book_err_t err = room_check_strict(room, start, end, people, now, clash);
    if (err != IX_BOOK_ERR_BUSY || is_offline(room, now)) return err;

    /* Someone is in the room but nobody booked it. It stays off the list
     * while another room fits; if none does, it can be booked after all. */
    for (int i = 0; i < s_room_count; i++)
        if (i != room && room_check_strict(i, start, end, people, now, NULL) == IX_BOOK_OK)
            return IX_BOOK_ERR_BUSY;
    return IX_BOOK_OK;
}

ix_book_err_t ix_book(int room, time_t start, time_t end, uint8_t people,
                      const char *name, const char *email, time_t now)
{
    char n[IX_NAME_LEN], e[IX_EMAIL_LEN];
    snprintf(n, sizeof n, "%s", name ? name : "");
    lower_copy(e, sizeof e, email ? email : "");
    /* trim name */
    char *p = n; while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return IX_BOOK_ERR_NAME;
    if (!valid_email(e)) return IX_BOOK_ERR_EMAIL;

    ix_book_err_t err = ix_room_check(room, start, end, people, now, NULL);
    if (err != IX_BOOK_OK) return err;
    if (ix_active_bookings_for(e, now) >= IX_MAX_BOOKINGS_PER_MAIL) return IX_BOOK_ERR_LIMIT;

    for (int i = 0; i < IX_MAX_BOOKINGS; i++) {
        ix_booking_t *b = &s_bookings[i];
        if (b->used) continue;
        memset(b, 0, sizeof *b);
        b->used = true;
        b->room = room;
        b->start = start;
        b->end = end;
        b->grace_from = start > now ? start : now;
        b->people = people;
        snprintf(b->name, sizeof b->name, "%s", p);
        snprintf(b->email, sizeof b->email, "%s", e);
        /* booked a room you're already sitting in: counts as checked in */
        b->checked_in = start <= now && s_rooms[room].occupied;
        return IX_BOOK_OK;
    }
    return IX_BOOK_ERR_FULL;
}

int ix_find_rooms(time_t start, time_t end, uint8_t people, time_t now, int *out, int max)
{
    int n = 0;
    for (int i = 0; i < s_room_count && n < max; i++)
        if (ix_room_check(i, start, end, people, now, NULL) == IX_BOOK_OK) out[n++] = i;
    /* smallest room that fits first, so a group of 4 doesn't take the lecture hall */
    for (int i = 1; i < n; i++) {
        int v = out[i], j = i;
        while (j > 0 && s_rooms[out[j - 1]].info.seats > s_rooms[v].info.seats) { out[j] = out[j - 1]; j--; }
        out[j] = v;
    }
    return n;
}

int ix_room_bookings(int room, time_t from, const ix_booking_t **out, int max)
{
    int n = 0;
    for (int i = 0; i < IX_MAX_BOOKINGS && n < max; i++)
        if (s_bookings[i].used && s_bookings[i].room == room && s_bookings[i].end > from)
            out[n++] = &s_bookings[i];
    for (int i = 1; i < n; i++) {
        const ix_booking_t *v = out[i]; int j = i;
        while (j > 0 && out[j - 1]->start > v->start) { out[j] = out[j - 1]; j--; }
        out[j] = v;
    }
    return n;
}

const char *ix_book_err_text(ix_book_err_t e)
{
    switch (e) {
    case IX_BOOK_OK:          return "Booked.";
    case IX_BOOK_ERR_NAME:    return "Please enter your name.";
    case IX_BOOK_ERR_EMAIL:   return "Please enter a valid email address.";
    case IX_BOOK_ERR_TIME:    return "Pick a time today, up to 4 hours long.";
    case IX_BOOK_ERR_SEATS:   return "This room is too small for your group.";
    case IX_BOOK_ERR_OVERLAP: return "Someone else has booked this room then.";
    case IX_BOOK_ERR_BUSY:    return "This room is in use right now. Pick a later start.";
    case IX_BOOK_ERR_LIMIT:   return "This email already has 2 active bookings.";
    default:                  return "No space for more bookings right now.";
    }
}

/* ---- lost & found ----------------------------------------------------- */

const char *ix_category_name(ix_category_t c)
{
    static const char *names[IX_CAT_COUNT] = {
        "Phone", "Keys", "Wallet / cards", "Bag", "Bottle",
        "Clothing", "Laptop / electronics", "Umbrella", "Glasses", "Other",
    };
    return (c >= 0 && c < IX_CAT_COUNT) ? names[c] : "?";
}

int ix_found_add(int room, ix_category_t cat, time_t now)
{
    for (int i = 0; i < IX_MAX_FOUND; i++) {
        if (s_found[i].used) continue;
        s_found[i] = (ix_found_t){ .used = true, .room = room, .cat = cat, .when = now };
        return i;
    }
    return -1;
}

void ix_found_set_online(int id, bool online)
{
    if (id >= 0 && id < IX_MAX_FOUND && s_found[id].used) s_found[id].online = online;
}

int ix_found_search(int room, int cat, const ix_found_t **out, int max)
{
    int n = 0;
    for (int i = 0; i < IX_MAX_FOUND && n < max; i++) {
        const ix_found_t *f = &s_found[i];
        if (!f->used) continue;
        if (room >= 0 && f->room != room) continue;
        if (cat >= 0 && (int)f->cat != cat) continue;
        out[n++] = f;
    }
    for (int i = 1; i < n; i++) {
        const ix_found_t *v = out[i]; int j = i;
        while (j > 0 && out[j - 1]->when < v->when) { out[j] = out[j - 1]; j--; }
        out[j] = v;
    }
    return n;
}
