/*
 * InhabitX main UI, PC simulator.
 *
 * Opens an X11 window the size of the board's screen and runs the same
 * ui/ + logic/ code as the E84. Connects to the MQTT broker (README §3):
 * listens to the nodes, publishes the building state and answers bookings
 * and found items from the web page. Type "h" in the terminal for commands.
 *
 *   ./main_sim               broker from sim/broker.conf, real clock
 *   ./main_sim --offline     no broker: built-in fake nodes + demo data
 *   ./main_sim --fake        broker, plus the built-in fake nodes
 *   ./main_sim --at 10:05    pretend it's 10:05 today (bookings need 07-22 h)
 *   ./main_sim --conf FILE   broker settings from FILE
 */
#include "lvgl.h"
#include "ui.h"
#include "ix_model.h"
#include "ix_msg.h"
#include "mqtt_link.h"

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

static bool s_fakes = true;     /* built-in fake nodes running */
static bool s_online;           /* using the broker */

/* ---- fake nodes ------------------------------------------------------ */
typedef struct { const char *id; uint16_t seats; bool occupied; bool online; } fake_t;
static fake_t s_fake[] = {
    /* capacities from the AAU campus plan (campusplan.aau.at) */
    { "Z.1.08", 40, true,  true  },     /* Seminarraum           */
    { "Z.1.09", 48, false, true  },     /* Seminarraum           */
    { "Z.1.10",173, false, true  },     /* Hoersaal 4 (HS 4)     */
    { "Z.1.11", 86, true,  true  },     /* Hoersaal 3 (HS 3)     */
    { "Z.1.12",  4, false, false },     /* NOT in the AAU plan: placeholder from README, to be replaced */
};
#define N_FAKE ((int)(sizeof s_fake / sizeof s_fake[0]))

static void publish_nodes(void)
{
    if (!s_fakes) return;
    time_t now = ix_now();
    for (int i = 0; i < N_FAKE; i++)
        if (s_fake[i].online) ix_node_update(s_fake[i].id, s_fake[i].occupied, now);
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
    if (s_online) printf("  broker: %s\n", mqtt_link_connected() ? "connected" : "not connected");
}

static void help(void)
{
    printf("commands (room = full id or suffix, e.g. 09):\n"
           "  o <room>   toggle someone there / empty\n"
           "  x <room>   toggle node online / offline\n"
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
    if ((cmd == 'o' || cmd == 'x') && !s_fakes) { printf("  built-in fake nodes are off (start with --fake or --offline)\n"); return; }
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
    publish_nodes();
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

/* ---- broker (M1, M4, M6) ----------------------------------------------- */
static bool s_dirty = true;     /* publish the building state soon */

static void on_msg(const char *topic, size_t tl, const char *payload, size_t pl)
{
    ix_msg_result_t r;
    ix_msg_handle(topic, tl, payload, pl, ix_now(), &r);
    if (r.reply) mqtt_link_publish(r.reply_topic, r.reply_payload, strlen(r.reply_payload), false);
    if (r.changed) s_dirty = true;
    if (r.log[0]) printf("  %s\n", r.log);
}

static char s_last_rooms[8192], s_last_found[8192];
static mqtt_link_conf_t s_conf;

static void on_up(void)
{
    s_last_rooms[0] = s_last_found[0] = 0;  /* republish everything after a reconnect */
    s_dirty = true;
}

/* Publish rooms / lostfound (retained) when their content changed. */
static void publish_state(void)
{
    static char stable[8192], msg[8192];
    time_t now = ix_now();
    if (!mqtt_link_connected()) return;
    if (ix_msg_rooms(now, true, stable, sizeof stable) && strcmp(stable, s_last_rooms) != 0) {
        size_t n = ix_msg_rooms(now, false, msg, sizeof msg);
        if (n && mqtt_link_publish(IX_TOPIC_ROOMS, msg, n, true)) strcpy(s_last_rooms, stable);
    }
    if (ix_msg_lostfound(now, true, stable, sizeof stable) && strcmp(stable, s_last_found) != 0) {
        size_t n = ix_msg_lostfound(now, false, msg, sizeof msg);
        if (n && mqtt_link_publish(IX_TOPIC_LOSTFOUND, msg, n, true)) strcpy(s_last_found, stable);
    }
    s_dirty = false;
}

/* ---- demo data -------------------------------------------------------- */
static void seed(void)
{
    time_t now = ix_now();
    ix_init(now);
    for (int i = 0; i < N_FAKE; i++) ix_room_add(s_fake[i].id, 1, s_fake[i].seats);
    publish_nodes();
    if (s_online) return;   /* the real main starts empty */

    time_t first = ix_day_first_slot(now);
    ix_node_update("Z.1.08", false, now);      /* book it first, then people walk in */
    ix_book_err_t e1 = ix_book(ix_room_find("Z.1.08"), first, first + 3600, 20, "Lecture prep", "staff@aau.at", now);
    ix_book_err_t e2 = ix_book(ix_room_find("Z.1.09"), now, first + 3600, 3, "Sam", "sam@edu.aau.at", now);
    ix_book_err_t e3 = ix_book(ix_room_find("Z.1.10"), first + 7200, first + 10800, 4, "Felix", "felix@edu.aau.at", now);
    if (e1 || e2 || e3)
        printf("note: demo bookings need a time between %02d:00 and %02d:00, try --at 10:05\n",
               IX_DAY_OPEN_H, IX_DAY_CLOSE_H - 2);

    publish_nodes();

    /* a plausible morning for the occupancy chart */
    static const int8_t demo_hist[] = { 5, 10, 20, 35, 50, 60, 60, 55, 65, 80, 85, 75, 70, 60, 60, 55 };
    int nowb = ix_hist_bucket(now);
    for (int b = 0; b < nowb && b < (int)sizeof demo_hist; b++) ix_hist_set(b, demo_hist[b]);

    ix_found_add(ix_room_find("Z.1.10"), IX_CAT_UMBRELLA, now - 3600);
    ix_found_add(ix_room_find("Z.1.08"), IX_CAT_PHONE, now - 2 * 86400);
}

int main(int argc, char **argv)
{
    bool offline = false, fake = false;
    const char *conf_path = getenv("IX_BROKER_CONF") ? getenv("IX_BROKER_CONF") : IX_SIM_DIR "/broker.conf";
    for (int i = 1; i < argc; i++) {
        int h, m;
        if (strcmp(argv[i], "--offline") == 0) offline = true;
        else if (strcmp(argv[i], "--fake") == 0) fake = true;
        else if (strcmp(argv[i], "--conf") == 0 && i + 1 < argc) conf_path = argv[++i];
        else if (strcmp(argv[i], "--at") == 0 && i + 1 < argc && sscanf(argv[++i], "%d:%d", &h, &m) == 2) {
            time_t t = time(NULL);
            struct tm tm;
            localtime_r(&t, &tm);
            tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = 0;
            s_offset = (long)(mktime(&tm) - t);
        }
    }

    setvbuf(stdout, NULL, _IOLBF, 0);
    if (!offline) {
        mqtt_link_conf_t conf;
        char why[256];
        if (mqtt_link_load_conf(conf_path, &conf, why, sizeof why)) {
            s_conf = conf;
            s_online = true;
            s_fakes = fake;
        } else {
            printf("broker: %s\nrunning offline with the built-in fake nodes (see sim/README.md)\n", why);
        }
    }

    lv_init();
    lv_log_register_print_cb(log_cb);
    lv_tick_set_cb(tick_ms);
    lv_display_t *disp = lv_x11_window_create("InhabitX main (simulator)", UI_HOR_RES, UI_VER_RES);
    lv_x11_inputs_create(disp, NULL);

    /* The X11 driver hides the system pointer; draw a finger-sized dot instead */
    for (lv_indev_t *in = lv_indev_get_next(NULL); in; in = lv_indev_get_next(in)) {
        if (lv_indev_get_type(in) != LV_INDEV_TYPE_POINTER) continue;
        lv_obj_t *cur = lv_obj_create(lv_layer_sys());
        lv_obj_remove_style_all(cur);
        lv_obj_set_size(cur, 18, 18);
        lv_obj_set_style_radius(cur, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(cur, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(cur, LV_OPA_70, 0);
        lv_obj_set_style_border_color(cur, lv_color_hex(0x3B7BFF), 0);
        lv_obj_set_style_border_width(cur, 3, 0);
        lv_obj_set_style_translate_x(cur, -9, 0);
        lv_obj_set_style_translate_y(cur, -9, 0);
        lv_obj_remove_flag(cur, LV_OBJ_FLAG_CLICKABLE);
        lv_indev_set_cursor(in, cur);
        break;
    }
    lv_indev_t *tap = lv_indev_create();
    lv_indev_set_type(tap, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(tap, tap_read);

    seed();
    ui_init();
    if (s_online) {
        /* rooms are known now: leave "all offline" behind if we drop off */
        static char will[4096];
        ix_msg_rooms_main_offline(will, sizeof will);
        mqtt_link_start(&s_conf, "inhabitx-main-" IX_BUILDING, IX_TOPIC_SUB,
                        IX_TOPIC_ROOMS, will, on_msg, on_up);
    }

    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    printf("InhabitX main simulator %dx%d. Ghost release after %d s. %s\n", UI_HOR_RES, UI_VER_RES,
           IX_GHOST_RELEASE_S, !s_online ? "Offline." : s_fakes ? "Broker + built-in fake nodes." : "Broker.");
    help();

    uint32_t last_pub = 0, last_state = 0;
    for (;;) {
        uint32_t wait = lv_timer_handler();
        if (tick_ms() - last_pub >= 2000) { publish_nodes(); last_pub = tick_ms(); }
        if (s_online) {
            mqtt_link_poll();
            /* at most twice a second; states also change with time (offline, ghost release) */
            if (s_dirty || tick_ms() - last_state >= 500) { publish_state(); last_state = tick_ms(); }
        }
        poll_stdin();
        fflush(stdout);
        usleep((wait > 20 ? 20 : wait) * 1000);
    }
    return 0;
}
