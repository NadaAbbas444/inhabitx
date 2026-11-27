/* Lost & Found: "I found something" registers room + category;
 * "I lost something" searches by room + category. No names stored. */
#include "ui_internal.h"

#include <stdio.h>
#include <string.h>

#define SEL_NONE (-2)
#define SEL_ANY  (-1)

enum { LF_FOUND, LF_LOST };

static lv_obj_t *s_landing, *s_flow, *s_title, *s_sub, *s_action;
static lv_obj_t *s_room_grid, *s_cat_grid, *s_recent, *s_recent_count;
static int s_mode, s_room = SEL_NONE, s_cat = SEL_NONE;
static int s_recent_sig = -1;

static void fmt_when(char *buf, size_t n, time_t t, time_t now)
{
    struct tm a, b;
    localtime_r(&t, &a);
    localtime_r(&now, &b);
    static const char *wd[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    if (a.tm_yday == b.tm_yday && a.tm_year == b.tm_year)
        snprintf(buf, n, "today %02d:%02d", a.tm_hour, a.tm_min);
    else
        snprintf(buf, n, "%s %d.%d., %02d:%02d", wd[a.tm_wday], a.tm_mday, a.tm_mon + 1, a.tm_hour, a.tm_min);
}

/* ---- flow: where + what ------------------------------------------------ */

static void update_action(void)
{
    lv_obj_set_state(s_action, LV_STATE_DISABLED, s_room == SEL_NONE || s_cat == SEL_NONE);
}

static void select_in_grid(lv_obj_t *grid, lv_obj_t *btn)
{
    uint32_t n = lv_obj_get_child_count(grid);
    for (uint32_t i = 0; i < n; i++) {
        lv_obj_t *c = lv_obj_get_child(grid, i);
        lv_obj_set_state(c, LV_STATE_CHECKED, c == btn);
    }
}

static void room_cb(lv_event_t *e)
{
    s_room = (int)(intptr_t)lv_event_get_user_data(e);
    select_in_grid(s_room_grid, lv_event_get_target(e));
    ui_mark_busy();
    update_action();
}

static void cat_cb(lv_event_t *e)
{
    s_cat = (int)(intptr_t)lv_event_get_user_data(e);
    select_in_grid(s_cat_grid, lv_event_get_target(e));
    ui_mark_busy();
    update_action();
}

static lv_obj_t *grid_btn(lv_obj_t *grid, const char *txt, lv_event_cb_t cb, int val)
{
    lv_obj_t *b = ui_btn(grid, txt, UI_BTN_SECONDARY, cb, (void *)(intptr_t)val);
    lv_obj_set_width(b, lv_pct(48));
    lv_obj_set_style_pad_ver(b, 11, 0);
    lv_obj_set_style_radius(b, 12, 0);
    return b;
}

static void fill_grids(void)
{
    lv_obj_clean(s_room_grid);
    if (s_mode == LF_LOST) grid_btn(s_room_grid, "Not sure", room_cb, SEL_ANY);
    for (int i = 0; i < ix_room_count(); i++) grid_btn(s_room_grid, ix_room(i)->id, room_cb, i);

    lv_obj_clean(s_cat_grid);
    for (int c = 0; c < IX_CAT_COUNT; c++) grid_btn(s_cat_grid, ix_category_name(c), cat_cb, c);
    if (s_mode == LF_LOST) grid_btn(s_cat_grid, "Any type", cat_cb, SEL_ANY);
}

/* ---- result dialogs ---------------------------------------------------- */

static void done_cb(lv_event_t *e) { ui_close_overlays(); ui_lf_reset(); }
static void again_cb(lv_event_t *e) { ui_close_overlays(); }

static lv_obj_t *dialog(void)
{
    lv_obj_t *o = ui_overlay(true);
    lv_obj_t *d = ui_col(o, 12);
    lv_obj_set_width(d, 560);
    lv_obj_set_style_bg_color(d, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(d, 22, 0);
    lv_obj_set_style_border_color(d, UI_C_LINE, 0);
    lv_obj_set_style_border_width(d, 1, 0);
    lv_obj_set_style_pad_all(d, 28, 0);
    lv_obj_set_flex_align(d, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(d, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(d);
    return d;
}

static lv_obj_t *centered(lv_obj_t *p, const char *t, const lv_font_t *f, lv_color_t c)
{
    lv_obj_t *l = ui_label(p, t, f, c);
    lv_obj_set_width(l, lv_pct(100));
    lv_label_set_long_mode(l, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
    return l;
}

static lv_obj_t *item_row(lv_obj_t *parent, const ix_found_t *f, time_t now)
{
    lv_obj_t *r = ui_row(parent, 12);
    lv_obj_set_width(r, lv_pct(100));
    lv_obj_set_style_bg_color(r, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(r, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(r, 12, 0);
    lv_obj_set_style_pad_all(r, 10, 0);
    ui_icon_tile(r, LV_SYMBOL_DIRECTORY, UI_C_FREE, 36);
    lv_obj_t *c = ui_col(r, 0);
    ui_label(c, ix_category_name(f->cat), ui_f_label, UI_C_TEXT);
    char buf[64], when[32];
    fmt_when(when, sizeof when, f->when, now);
    snprintf(buf, sizeof buf, "found in %s%s", ix_room(f->room)->id, f->online ? " · reported online" : "");
    ui_label(c, buf, ui_f_small, UI_C_MUTED);
    ui_spacer(r);
    ui_label(r, when, ui_f_small, UI_C_MUTED);
    return r;
}

static void show_found_done(void)
{
    time_t now = ix_now();
    lv_obj_t *d = dialog();
    int id = ix_found_add(s_room, (ix_category_t)s_cat, now);
    s_recent_sig = -1;

    lv_obj_t *ok = ui_box(d);
    lv_obj_set_size(ok, 76, 76);
    lv_obj_set_style_radius(ok, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ok, id >= 0 ? UI_C_FREE : UI_C_GHOST, 0);
    lv_obj_set_style_bg_opa(ok, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_color(ok, id >= 0 ? UI_C_FREE : UI_C_GHOST, 0);
    lv_obj_set_style_shadow_width(ok, 50, 0);
    lv_obj_set_style_shadow_opa(ok, LV_OPA_40, 0);
    lv_obj_center(ui_label(ok, id >= 0 ? LV_SYMBOL_OK : LV_SYMBOL_WARNING, &lv_font_montserrat_32, lv_color_white()));

    if (id < 0) {
        centered(d, "The list is full", ui_f_h2, UI_C_TEXT);
        centered(d, "Please bring the item to " UI_LOSTFOUND_DESK " anyway.", ui_f_body, UI_C_MUTED);
    } else {
        centered(d, "Thank you!", ui_f_h1, UI_C_TEXT);
        ix_found_t just = { .used = true, .room = s_room, .cat = (ix_category_t)s_cat, .when = now };
        item_row(d, &just, now);
        centered(d, "Please bring it to " UI_LOSTFOUND_DESK ". The owner can now see on this screen "
                    "and online that it was handed in.", ui_f_body, UI_C_MUTED);
    }
    lv_obj_t *b = ui_btn(d, "Done", UI_BTN_PRIMARY, done_cb, NULL);
    lv_obj_set_width(b, 220);
}

static void show_lost_results(void)
{
    time_t now = ix_now();
    const ix_found_t *hits[4];
    int n = ix_found_search(s_room, s_cat, hits, 4);
    lv_obj_t *d = dialog();

    if (n == 0) {
        ui_icon_tile(d, LV_SYMBOL_EYE_CLOSE, UI_C_MUTED, 64);
        centered(d, "Nothing handed in yet", ui_f_h2, UI_C_TEXT);
        centered(d, "No matching item has been registered. Items show up here as soon as "
                    "someone hands them in, so check again later.", ui_f_body, UI_C_MUTED);
    } else {
        ui_icon_tile(d, LV_SYMBOL_OK, UI_C_FREE, 64);
        char buf[48];
        snprintf(buf, sizeof buf, n == 1 ? "Good news: 1 possible match" : "Good news: %d possible matches", n);
        centered(d, buf, ui_f_h2, UI_C_TEXT);
        for (int i = 0; i < n; i++) item_row(d, hits[i], now);
        centered(d, "Ask at " UI_LOSTFOUND_DESK ".", ui_f_body, UI_C_MUTED);
    }
    lv_obj_t *r = ui_row(d, 12);
    ui_btn(r, "Search again", UI_BTN_SECONDARY, again_cb, NULL);
    lv_obj_t *b = ui_btn(r, "Done", UI_BTN_PRIMARY, done_cb, NULL);
    lv_obj_set_width(b, 180);
}

static void action_cb(lv_event_t *e)
{
    if (s_room == SEL_NONE || s_cat == SEL_NONE) return;
    if (s_mode == LF_FOUND) show_found_done();
    else show_lost_results();
}

/* ---- navigation ------------------------------------------------------- */

static void open_flow(int mode)
{
    s_mode = mode;
    s_room = s_cat = SEL_NONE;
    lv_label_set_text(s_title, mode == LF_FOUND ? "I found something" : "I lost something");
    lv_label_set_text(s_sub, mode == LF_FOUND ? "Where was it, and what is it?"
                                              : "Where did you lose it, and what is it?");
    ui_btn_set_text(s_action, mode == LF_FOUND ? "Register item  " LV_SYMBOL_OK : "Search  " LV_SYMBOL_RIGHT);
    fill_grids();
    update_action();
    lv_obj_add_flag(s_landing, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_flow, LV_OBJ_FLAG_HIDDEN);
    ui_mark_busy();
}

static void open_cb(lv_event_t *e) { open_flow((int)(intptr_t)lv_event_get_user_data(e)); }
static void back_cb(lv_event_t *e) { ui_lf_reset(); }

void ui_lf_reset(void)
{
    s_room = s_cat = SEL_NONE;
    lv_obj_remove_flag(s_landing, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_flow, LV_OBJ_FLAG_HIDDEN);
}

void ui_lf_refresh(time_t now)
{
    const ix_found_t *f[3];
    int total = ix_found_search(-1, -1, f, 3);
    const ix_found_t *all[IX_MAX_FOUND];
    int count = ix_found_search(-1, -1, all, IX_MAX_FOUND);
    if (count == s_recent_sig) return;
    s_recent_sig = count;
    lv_obj_clean(s_recent);
    for (int i = 0; i < total; i++) item_row(s_recent, f[i], now);
    if (!total) ui_label(s_recent, "Nothing has been handed in this week.", ui_f_body, UI_C_MUTED);
    lv_label_set_text_fmt(s_recent_count, count == 1 ? "1 item" : "%d items", count);
}

/* ---- build ------------------------------------------------------------ */

static void big_choice(lv_obj_t *parent, const char *icon, lv_color_t c, const char *title,
                       const char *text, int mode)
{
    lv_obj_t *k = ui_card(parent);
    lv_obj_set_flex_grow(k, 1);
    lv_obj_set_height(k, lv_pct(100));
    lv_obj_set_style_pad_all(k, 22, 0);
    lv_obj_set_style_bg_grad_color(k, lv_color_mix(c, UI_C_CARD, 40), 0);
    lv_obj_set_style_bg_grad_dir(k, LV_GRAD_DIR_VER, 0);
    lv_obj_set_flex_flow(k, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(k, 8, 0);
    lv_obj_add_flag(k, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_border_color(k, c, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(k, 2, LV_STATE_PRESSED);
    lv_obj_add_event_cb(k, open_cb, LV_EVENT_CLICKED, (void *)(intptr_t)mode);

    lv_obj_t *i = ui_icon_tile(k, icon, c, 56);
    lv_obj_add_flag(i, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_pad_bottom(k, 22, 0);
    lv_obj_t *t = ui_label(k, title, ui_f_h2, UI_C_TEXT);
    lv_obj_set_style_pad_top(t, 8, 0);
    lv_obj_t *d = ui_label(k, text, ui_f_body, UI_C_MUTED);
    lv_obj_set_width(d, lv_pct(100));
    lv_label_set_long_mode(d, LV_LABEL_LONG_WRAP);
    ui_spacer(k);
    lv_obj_t *go = ui_row(k, 8);
    ui_label(go, "Start", ui_f_label, c);
    ui_label(go, LV_SYMBOL_RIGHT, &lv_font_montserrat_14, c);
    lv_obj_add_flag(go, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static lv_obj_t *grid_card(lv_obj_t *parent, const char *title, lv_obj_t **grid)
{
    lv_obj_t *c = ui_card(parent);
    lv_obj_set_flex_grow(c, 1);
    lv_obj_set_height(c, lv_pct(100));
    lv_obj_set_flex_flow(c, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(c, 10, 0);
    ui_caption(c, title);
    lv_obj_t *g = ui_box(c);
    lv_obj_set_width(g, lv_pct(100));
    lv_obj_set_flex_grow(g, 1);
    lv_obj_set_flex_flow(g, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(g, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(g, 8, 0);
    lv_obj_add_flag(g, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(g, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(g, LV_SCROLLBAR_MODE_OFF);
    *grid = g;
    return c;
}

void ui_lf_create(lv_obj_t *page)
{
    /* landing */
    s_landing = ui_col(page, UI_GAP);
    lv_obj_set_size(s_landing, UI_PAGE_W, UI_PAGE_H);

    lv_obj_t *row = ui_row(s_landing, UI_GAP);
    lv_obj_set_size(row, lv_pct(100), 236);
    big_choice(row, LV_SYMBOL_DOWNLOAD, UI_C_FREE, "I found something",
               "Tell us the room and what it is, then hand it in at the Lost & Found desk.", LF_FOUND);
    big_choice(row, LV_SYMBOL_EYE_OPEN, UI_C_ACCENT, "I lost something",
               "See if someone already handed it in, before you walk to the desk.", LF_LOST);

    lv_obj_t *rc = ui_card(s_landing);
    lv_obj_set_width(rc, lv_pct(100));
    lv_obj_set_flex_grow(rc, 1);
    lv_obj_set_flex_flow(rc, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(rc, 8, 0);
    lv_obj_t *h = ui_row(rc, 8);
    lv_obj_set_width(h, lv_pct(100));
    ui_label(h, "Recently handed in", ui_f_title, UI_C_TEXT);
    s_recent_count = ui_label(h, "", ui_f_caption, UI_C_ACCENT);
    lv_obj_set_style_bg_color(s_recent_count, UI_C_ACCENT, 0);
    lv_obj_set_style_bg_opa(s_recent_count, 40, 0);
    lv_obj_set_style_radius(s_recent_count, 6, 0);
    lv_obj_set_style_pad_hor(s_recent_count, 8, 0);
    lv_obj_set_style_pad_ver(s_recent_count, 3, 0);
    ui_spacer(h);
    ui_label(h, LV_SYMBOL_EYE_CLOSE "  Only room, type and time are stored, for 7 days",
             ui_f_small, UI_C_FAINT);
    s_recent = ui_row(rc, 10);
    lv_obj_set_width(s_recent, lv_pct(100));
    lv_obj_set_flex_flow(s_recent, LV_FLEX_FLOW_ROW_WRAP);

    /* flow */
    s_flow = ui_col(page, UI_GAP);
    lv_obj_set_size(s_flow, UI_PAGE_W, UI_PAGE_H);

    lv_obj_t *hdr = ui_row(s_flow, 14);
    lv_obj_set_width(hdr, lv_pct(100));
    ui_round_btn(hdr, LV_SYMBOL_LEFT, back_cb, NULL);
    lv_obj_t *hc = ui_col(hdr, 0);
    s_title = ui_label(hc, "", ui_f_h2, UI_C_TEXT);
    s_sub = ui_label(hc, "", ui_f_small, UI_C_MUTED);
    ui_spacer(hdr);
    s_action = ui_btn(hdr, "", UI_BTN_PRIMARY, action_cb, NULL);

    lv_obj_t *body = ui_row(s_flow, UI_GAP);
    lv_obj_set_width(body, lv_pct(100));
    lv_obj_set_flex_grow(body, 1);
    grid_card(body, "WHICH ROOM?", &s_room_grid);
    grid_card(body, "WHAT IS IT?", &s_cat_grid);

    ui_lf_reset();
}
