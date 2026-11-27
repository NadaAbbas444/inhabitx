/*
 * InhabitX master UI, PC simulator.
 *
 * Opens an X11 window the size of the board's screen and runs the same
 * ui/ + logic/ code as the E84. Fake slaves are built in; control them by
 * typing commands in the terminal (type "h" for help).
 *
 *   ./master_sim               real clock
 *   ./master_sim --at 10:05    pretend it's 10:05 today (bookings need 07-22 h)
 */
#include "lvgl.h"
#include "ui.h"
#include "ix_model.h"

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* ---- clock ------------------------------------------------------------ */
static long s_offset;
time_t ix_now(void) { return time(NULL) + s_offset; }

static uint32_t tick_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/* LVGL 9.2 TinyTTF logs an error for every icon glyph before falling back to
 * the icon font; that's expected here, so keep the terminal readable. */
static void log_cb(lv_log_level_t level, const char *buf)
{
    if (strstr(buf, "cache not allocated")) return;
    fputs(buf, stdout);
}

/* ---- scripted input (for testing without a mouse) ---------------------- */
static int32_t s_tap_x, s_tap_y;
static int     s_tap_frames;

static void tap_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    data->point.x = s_tap_x;
    data->point.y = s_tap_y;
    data->state = s_tap_frames > 0 ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    if (s_tap_frames > 0) s_tap_frames--;
}

static lv_obj_t *find_focused_ta(lv_obj_t *o)
{
    if (lv_obj_check_type(o, &lv_textarea_class) && lv_obj_has_state(o, LV_STATE_FOCUSED)) return o;
    for (uint32_t i = 0; i < lv_obj_get_child_count(o); i++) {
        lv_obj_t *r = find_focused_ta(lv_obj_get_child(o, i));
        if (r) return r;
    }
    return NULL;
}

/* ---- fake slaves ------------------------------------------------------ */
typedef struct { const char *id; uint16_t seats; bool occupied; bool online; } fake_t;
static fake_t s_fake[] = {
    { "Z.1.08", 24, true,  true  },
    { "Z.1.09",  8, false, true  },
    { "Z.1.10",  6, false, true  },
    { "Z.1.11", 10, true,  true  },
    { "Z.1.12",  4, false, false },
};
#define N_FAKE ((int)(sizeof s_fake / sizeof s_fake[0]))

static void publish_slaves(void)
{
    time_t now = ix_now();
    for (int i = 0; i < N_FAKE; i++)
        if (s_fake[i].online) ix_slave_update(s_fake[i].id, s_fake[i].occupied, now);
}

static fake_t *find_fake(const char *arg)
{
    while (*arg && isspace((unsigned char)*arg)) arg++;
    size_t n = strlen(arg);
    while (n && isspace((unsigned char)arg[n - 1])) n--;
    if (!n) return NULL;
    for (int i = 0; i < N_FAKE; i++) {
        size_t l = strlen(s_fake[i].id);
        if (n <= l && strncmp(s_fake[i].id + l - n, arg, n) == 0) return &s_fake[i];
    }
    return NULL;
}

static void print_status(void)
{
    time_t now = ix_now();
    for (int i = 0; i < ix_room_count(); i++) {
        ix_status_t st;
        ix_room_status(i, now, &st);
        printf("  %s  %-8s", ix_room(i)->id, ix_state_name(st.state));
        if (st.state == IX_GHOST) printf("  releases in %ds", (int)st.release_in_s);
        printf("\n");
    }
}

static void help(void)
{
    printf("commands (room = full id or suffix, e.g. 09):\n"
           "  o <room>   toggle someone there / empty\n"
           "  x <room>   toggle slave online / offline\n"
           "  g <room>   new booking starting now, nobody there (ghost demo)\n"
           "  s          print room states\n"
           "  t <x> <y>  tap the screen at x,y\n"
           "  k <text>   type text into the focused field\n"
           "  h          this help\n");
}

static void ghost_booking(const char *room_id)
{
    time_t now = ix_now();
    int r = ix_room_find(room_id);
    ix_book_err_t e = ix_book(r, now, now + 3600 > ix_day_close(now) ? ix_day_close(now) : now + 3600,
                              1, "Demo", "demo@edu.aau.at", now);
    printf("  ghost booking on %s: %s\n", room_id, ix_book_err_text(e));
}

static void handle_line(char *line)
{
    char cmd = line[0];
    fake_t *f = line[0] ? find_fake(line + 1) : NULL;
    switch (cmd) {
    case 'o': if (f) { f->occupied = !f->occupied; printf("  %s %s\n", f->id, f->occupied ? "occupied" : "empty"); } break;
    case 'x': if (f) { f->online = !f->online; printf("  %s %s\n", f->id, f->online ? "online" : "offline (in 10 s)"); } break;
    case 'g': if (f) ghost_booking(f->id); break;
    case 's': print_status(); return;
    case 't': if (sscanf(line + 1, "%d %d", &s_tap_x, &s_tap_y) == 2) s_tap_frames = 3; return;
    case 'k': {
        lv_obj_t *ta = find_focused_ta(lv_layer_top());
        if (ta) lv_textarea_add_text(ta, line + 2); else printf("  no focused field\n");
        return;
    }
    case 'h': help(); return;
    default:  if (cmd) help(); return;
    }
    if (!f) printf("  unknown room\n");
    publish_slaves();
}

static void poll_stdin(void)
{
    static char buf[128];
    static size_t len;
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\n') { buf[len] = 0; handle_line(buf); len = 0; }
        else if (len < sizeof buf - 1) buf[len++] = c;
    }
}

/* ---- demo data -------------------------------------------------------- */
static void seed(void)
{
    time_t now = ix_now();
    ix_init(now);
    for (int i = 0; i < N_FAKE; i++) ix_room_add(s_fake[i].id, 1, s_fake[i].seats);
    publish_slaves();

    time_t first = ix_day_first_slot(now);
    ix_slave_update("Z.1.08", false, now);      /* book it first, then people walk in */
    ix_book_err_t e1 = ix_book(ix_room_find("Z.1.08"), first, first + 3600, 20, "Lecture prep", "staff@aau.at", now);
    ix_book_err_t e2 = ix_book(ix_room_find("Z.1.09"), now, first + 3600, 3, "Sam", "sam@edu.aau.at", now);
    ix_book_err_t e3 = ix_book(ix_room_find("Z.1.10"), first + 7200, first + 10800, 4, "Felix", "felix@edu.aau.at", now);
    if (e1 || e2 || e3)
        printf("note: demo bookings need a time between %02d:00 and %02d:00, try --at 10:05\n",
               IX_DAY_OPEN_H, IX_DAY_CLOSE_H - 2);

    publish_slaves();

    /* a plausible morning for the occupancy chart */
    static const int8_t demo_hist[] = { 5, 10, 20, 35, 50, 60, 60, 55, 65, 80, 85, 75, 70, 60, 60, 55 };
    int nowb = ix_hist_bucket(now);
    for (int b = 0; b < nowb && b < (int)sizeof demo_hist; b++) ix_hist_set(b, demo_hist[b]);

    ix_found_add(ix_room_find("Z.1.10"), IX_CAT_UMBRELLA, now - 3600);
    ix_found_add(ix_room_find("Z.1.08"), IX_CAT_PHONE, now - 2 * 86400);
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        int h, m;
        if (strcmp(argv[i], "--at") == 0 && i + 1 < argc && sscanf(argv[++i], "%d:%d", &h, &m) == 2) {
            time_t t = time(NULL);
            struct tm tm;
            localtime_r(&t, &tm);
            tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = 0;
            s_offset = (long)(mktime(&tm) - t);
        }
    }

    lv_init();
    lv_log_register_print_cb(log_cb);
    lv_tick_set_cb(tick_ms);
    lv_display_t *disp = lv_x11_window_create("InhabitX master (simulator)", UI_HOR_RES, UI_VER_RES);
    lv_x11_inputs_create(disp, NULL);
    lv_indev_t *tap = lv_indev_create();
    lv_indev_set_type(tap, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(tap, tap_read);

    seed();
    ui_init();

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    printf("InhabitX master simulator %dx%d. Ghost release after %d s.\n", UI_HOR_RES, UI_VER_RES, IX_GHOST_RELEASE_S);
    help();

    uint32_t last_pub = 0;
    for (;;) {
        uint32_t wait = lv_timer_handler();
        if (tick_ms() - last_pub >= 2000) { publish_slaves(); last_pub = tick_ms(); }
        poll_stdin();
        fflush(stdout);
        usleep((wait > 20 ? 20 : wait) * 1000);
    }
    return 0;
}
