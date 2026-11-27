/*
 * InhabitX master: room state, bookings, ghost release, lost & found.
 *
 * Plain C, no LVGL and no MQTT, so the same file builds for the E84 and for
 * the PC simulator. The UI only talks to the building through this header.
 *
 * All times are Unix seconds (time_t). The caller passes "now" in, so the
 * logic never reads a clock itself and is easy to test.
 */
#ifndef IX_MODEL_H
#define IX_MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- tuning (README §5–§6) ------------------------------------------- */
#ifndef IX_GHOST_RELEASE_S
#define IX_GHOST_RELEASE_S      30      /* demo value; real: 10 * 60        */
#endif
#define IX_OFFLINE_AFTER_S      10      /* no slave message for this long   */
#define IX_MAX_BOOKINGS_PER_MAIL 2
#define IX_MAX_BOOKING_S        (4 * 3600)
#define IX_SLOT_S               (15 * 60)
#define IX_DAY_OPEN_H           7       /* bookable from 07:00 ...          */
#define IX_DAY_CLOSE_H          22      /* ... until 22:00, today only      */
#define IX_FOUND_KEEP_S         (7 * 24 * 3600)

#define IX_MAX_ROOMS            32
#define IX_MAX_BOOKINGS         64
#define IX_MAX_FOUND            64
#define IX_NAME_LEN             32
#define IX_EMAIL_LEN            64

/* ---- rooms ------------------------------------------------------------ */
typedef enum {
    IX_FREE,        /* nobody there, no booking running                     */
    IX_BOOKED,      /* booking running, people checked in, room empty now   */
    IX_GHOST,       /* booking running, nobody showed up, countdown runs    */
    IX_OCCUPIED,    /* slave reports someone there                          */
    IX_OFFLINE,     /* no message from the slave for IX_OFFLINE_AFTER_S     */
} ix_state_t;

typedef struct {
    char     id[12];        /* "Z.1.09"                                      */
    uint8_t  floor;
    uint16_t seats;
} ix_room_info_t;

typedef struct {
    ix_state_t state;
    int32_t    release_in_s;    /* IX_GHOST: seconds until release           */
    time_t     until;           /* IX_BOOKED/IX_OCCUPIED: end of booking, 0 if none */
    time_t     free_until;      /* IX_FREE: start of next booking, 0 = rest of day */
} ix_status_t;

void ix_init(time_t now);
int  ix_room_add(const char *id, uint8_t floor, uint16_t seats);
int  ix_room_count(void);
const ix_room_info_t *ix_room(int idx);
int  ix_room_find(const char *id);              /* -1 if unknown */
void ix_room_status(int idx, time_t now, ix_status_t *out);
const char *ix_state_name(ix_state_t s);        /* "free", "ghost", ... (T0 names) */

/* from M1: one call per slave message */
void ix_slave_update(const char *room_id, bool occupied, time_t now);

/* call about once a second: checks in, releases ghosts, expires old items */
void ix_tick(time_t now);

/* ---- bookings --------------------------------------------------------- */
typedef enum {
    IX_BOOK_OK = 0,
    IX_BOOK_ERR_NAME,       /* name empty                                    */
    IX_BOOK_ERR_EMAIL,      /* not an email address                          */
    IX_BOOK_ERR_TIME,       /* end <= start, too long, past, outside the day */
    IX_BOOK_ERR_SEATS,      /* room too small for that many people           */
    IX_BOOK_ERR_OVERLAP,    /* another booking in that room at that time     */
    IX_BOOK_ERR_BUSY,       /* starts now but the room is occupied / offline */
    IX_BOOK_ERR_LIMIT,      /* this email already has 2 active bookings      */
    IX_BOOK_ERR_FULL,       /* no space left in the booking table            */
} ix_book_err_t;

typedef struct {
    bool     used;
    int      room;
    time_t   start, end;
    time_t   grace_from;    /* no-show countdown starts here: max(start, booked at) */
    uint8_t  people;
    bool     checked_in;
    char     name[IX_NAME_LEN];     /* stays on the master, never published */
    char     email[IX_EMAIL_LEN];   /* stays on the master, never published */
} ix_booking_t;

/* Why a room can't be booked for start..end (IX_BOOK_OK if it can).
 * Doesn't look at name/email. If it clashes, *clash points at the booking. */
ix_book_err_t ix_room_check(int room, time_t start, time_t end, uint8_t people,
                            time_t now, const ix_booking_t **clash);

ix_book_err_t ix_book(int room, time_t start, time_t end, uint8_t people,
                      const char *name, const char *email, time_t now);

/* Rooms that fit start..end and people, smallest room first. */
int ix_find_rooms(time_t start, time_t end, uint8_t people, time_t now,
                  int *out, int max);

/* Bookings of one room that end after "from", sorted by start. */
int ix_room_bookings(int room, time_t from, const ix_booking_t **out, int max);

int ix_active_bookings_for(const char *email, time_t now);
const char *ix_book_err_text(ix_book_err_t e);

/* Today's bookable window: first slot (now rounded down to 15 min, not
 * before opening) and closing time. */
time_t ix_day_first_slot(time_t now);
time_t ix_day_close(time_t now);

/* ---- lost & found ----------------------------------------------------- */
typedef enum {
    IX_CAT_PHONE, IX_CAT_KEYS, IX_CAT_WALLET, IX_CAT_BAG, IX_CAT_BOTTLE,
    IX_CAT_CLOTHING, IX_CAT_ELECTRONICS, IX_CAT_UMBRELLA, IX_CAT_GLASSES,
    IX_CAT_OTHER,
    IX_CAT_COUNT
} ix_category_t;

typedef struct {
    bool          used;
    int           room;
    ix_category_t cat;
    time_t        when;
} ix_found_t;

const char *ix_category_name(ix_category_t c);
int  ix_found_add(int room, ix_category_t cat, time_t now);   /* <0 if full */
/* room or cat < 0 means "any". Newest first. */
int  ix_found_search(int room, int cat, const ix_found_t **out, int max);

/* ---- occupancy history (for the "today" chart) ------------------------
 * One value per 15 min from opening to closing: % of online rooms in use.
 * -1 = no data yet. Filled by ix_tick(). */
#define IX_HIST_BUCKETS ((IX_DAY_CLOSE_H - IX_DAY_OPEN_H) * 4)
int  ix_hist_bucket(time_t now);            /* -1 outside opening hours */
int  ix_hist_get(int bucket);
void ix_hist_set(int bucket, int pct);      /* for demo data */
int  ix_occupancy_now(time_t now);          /* % of online rooms in use */

/* ---- events for the UI (toasts) --------------------------------------- */
typedef enum { IX_EV_RELEASED, IX_EV_CHECKED_IN } ix_event_t;
typedef void (*ix_event_cb_t)(ix_event_t ev, int room);
void ix_set_event_cb(ix_event_cb_t cb);

#ifdef __cplusplus
}
#endif
#endif
