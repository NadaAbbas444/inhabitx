/* Booking: what you need (left) + matching rooms (right), then name + email,
 * then a confirmation. Today only, 15-min steps, up to 4 h. */
#include "ui_internal.h"

#include <stdio.h>
#include <string.h>

enum { MODE_ANY, MODE_SPECIFIC };

#define LEFT_W  ((UI_PAGE_W * 46) / 100)

static int     s_mode = MODE_ANY;
static int     s_room = -1;
static uint8_t s_people = 1;
static time_t  s_from, s_to;

static lv_obj_t *s_mode_btn[2], *s_people_lbl, *s_from_lbl, *s_to_lbl, *s_dur_lbl, *s_dur_chip[3];
static lv_obj_t *s_list_title, *s_list_count, *s_list, *s_next;
static char      s_list_sig[256];

/* details overlay */
static lv_obj_t *s_name_ta, *s_email_ta, *s_kb, *s_err;

static const int s_quick_min[3] = { 30, 60, 120 };

static void refresh_now(void) { ui_book_refresh(ix_now()); }

/* ---- time range ------------------------------------------------------- */

static void clamp_times(time_t now)
{
    time_t first = ix_day_first_slot(now);
    time_t close = ix_day_close(now);
    long dur = (long)(s_to - s_from);
    if (dur < IX_SLOT_S) dur = 3600;

    if (s_from < first) s_from = first;
    if (s_from > close - IX_SLOT_S) s_from = close - IX_SLOT_S;
    s_to = s_from + dur;
    if (s_to > s_from + IX_MAX_BOOKING_S) s_to = s_from + IX_MAX_BOOKING_S;
    if (s_to > close) s_to = close;
    if (s_to < s_from + IX_SLOT_S) s_to = s_from + IX_SLOT_S;
}

static void from_cb(lv_event_t *e)
{
    long dur = (long)(s_to - s_from);
    s_from += (int)(intptr_t)lv_event_get_user_data(e) * IX_SLOT_S;
    s_to = s_from + dur;
    ui_mark_busy();
    clamp_times(ix_now());
    refresh_now();
}

static void to_cb(lv_event_t *e)
{
    time_t to = s_to + (int)(intptr_t)lv_event_get_user_data(e) * IX_SLOT_S;
    if (to >= s_from + IX_SLOT_S && to <= s_from + IX_MAX_BOOKING_S && to <= ix_day_close(ix_now()))
        s_to = to;
    ui_mark_busy();
    refresh_now();
}

static void quick_cb(lv_event_t *e)
{
    s_to = s_from + s_quick_min[(int)(intptr_t)lv_event_get_user_data(e)] * 60;
    ui_mark_busy();
    clamp_times(ix_now());
    refresh_now();
}

static void people_cb(lv_event_t *e)
{
    int p = s_people + (int)(intptr_t)lv_event_get_user_data(e);
    s_people = (uint8_t)(p < 1 ? 1 : p > 60 ? 60 : p);
    ui_mark_busy();
    refresh_now();
}

static void mode_cb(lv_event_t *e)
{
    s_mode = (int)(intptr_t)lv_event_get_user_data(e);
    ui_mark_busy();
    refresh_now();
}

static void room_cb(lv_event_t *e)
{
    s_room = (int)(intptr_t)lv_event_get_user_data(e);
    ui_mark_busy();
    s_list_sig[0] = 0;
    refresh_now();
}

/* ---- room list -------------------------------------------------------- */

static void reason_text(ix_book_err_t err, const ix_booking_t *clash, int room, char *buf, size_t n)
{
    char a[8], b[8];
    switch (err) {
    case IX_BOOK_OK:          snprintf(buf, n, "Available"); break;
    case IX_BOOK_ERR_SEATS:   snprintf(buf, n, "Only %u seats", ix_room(room)->seats); break;
    case IX_BOOK_ERR_OVERLAP:
        ui_fmt_hm(a, sizeof a, clash->start);
        ui_fmt_hm(b, sizeof b, clash->end);
        snprintf(buf, n, "Booked %s – %s", a, b);
        break;
    case IX_BOOK_ERR_BUSY: {
        ix_status_t st;
        ix_room_status(room, ix_now(), &st);
        snprintf(buf, n, st.state == IX_OFFLINE ? "Sensor offline" : "In use right now");
        break;
    }
    default:                  snprintf(buf, n, "Not bookable"); break;
    }
}

static void add_room_row(int room, ix_book_err_t err, const ix_booking_t *clash, bool best)
{
    const ix_room_info_t *r = ix_room(room);
    lv_obj_t *b = lv_button_create(s_list);
    lv_obj_remove_style_all(b);
    lv_obj_set_size(b, lv_pct(100), 58);
    lv_obj_set_style_bg_color(b, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(b, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(b, 14, 0);
    lv_obj_set_style_pad_hor(b, 12, 0);
    lv_obj_set_style_border_color(b, UI_C_LINE, 0);
    lv_obj_set_style_border_width(b, 1, 0);
    lv_obj_set_style_bg_color(b, UI_C_CARD3, LV_STATE_CHECKED);
    lv_obj_set_style_border_color(b, UI_C_ACCENT, LV_STATE_CHECKED);
    lv_obj_set_style_border_width(b, 2, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(b, UI_C_CARD3, LV_STATE_PRESSED);
    lv_obj_set_style_opa(b, LV_OPA_40, LV_STATE_DISABLED);
    lv_obj_set_flex_flow(b, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(b, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(b, 12, 0);
    lv_obj_add_event_cb(b, room_cb, LV_EVENT_CLICKED, (void *)(intptr_t)room);

    lv_color_t c = err == IX_BOOK_OK ? UI_C_FREE : UI_C_OFFLINE;
    lv_obj_t *tile = ui_box(b);
    lv_obj_set_size(tile, 38, 38);
    lv_obj_set_style_radius(tile, 11, 0);
    lv_obj_set_style_bg_color(tile, c, 0);
    lv_obj_set_style_bg_opa(tile, 45, 0);
    const char *dot = strrchr(r->id, '.');
    lv_obj_t *tl = ui_label(tile, dot ? dot + 1 : r->id, ui_f_label, lv_color_mix(c, lv_color_white(), 140));
    lv_obj_center(tl);

    lv_obj_t *col = ui_col(b, 0);
    ui_label(col, r->id, ui_f_label, UI_C_TEXT);
    char seats[16];
    snprintf(seats, sizeof seats, "%u seats", r->seats);
    ui_label(col, seats, ui_f_small, UI_C_MUTED);
    ui_spacer(b);

    if (best) {
        lv_obj_t *p = ui_pill(b);
        ui_pill_set(p, UI_C_ACCENT, "Best fit");
    } else {
        char why[40];
        reason_text(err, clash, room, why, sizeof why);
        ui_label(b, why, ui_f_small, err == IX_BOOK_OK ? UI_C_FREE : UI_C_MUTED);
    }
    if (room == s_room) {
        lv_obj_add_state(b, LV_STATE_CHECKED);
        ui_label(b, LV_SYMBOL_OK, &lv_font_montserrat_16, UI_C_ACCENT);
    }
    if (err != IX_BOOK_OK) lv_obj_add_state(b, LV_STATE_DISABLED);
}

static void empty_note(const char *title, const char *text)
{
    lv_obj_t *c = ui_col(s_list, 6);
    lv_obj_set_width(c, lv_pct(100));
    lv_obj_set_style_pad_top(c, 30, 0);
    lv_obj_set_flex_align(c, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    ui_icon_tile(c, LV_SYMBOL_WARNING, UI_C_GHOST, 48);
    ui_label(c, title, ui_f_title, UI_C_TEXT);
    lv_obj_t *l = ui_label(c, text, ui_f_small, UI_C_MUTED);
    lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
}

static void refresh_list(time_t now)
{
    int n = ix_room_count();
    ix_book_err_t err[IX_MAX_ROOMS];
    const ix_booking_t *clash[IX_MAX_ROOMS];
    for (int i = 0; i < n; i++) err[i] = ix_room_check(i, s_from, s_to, s_people, now, &clash[i]);

    int fit[IX_MAX_ROOMS];
    int nfit = ix_find_rooms(s_from, s_to, s_people, now, fit, IX_MAX_ROOMS);

    if (s_mode == MODE_ANY) {
        bool ok = false;
        for (int i = 0; i < nfit; i++) if (fit[i] == s_room) ok = true;
        if (!ok) s_room = nfit ? fit[0] : -1;
    }
    bool closed = ix_day_first_slot(now) > ix_day_close(now) - IX_SLOT_S;

    char sig[sizeof s_list_sig];
    int len = snprintf(sig, sizeof sig, "%d;%d;%d;", s_mode, s_room, closed);
    for (int i = 0; i < n && len < (int)sizeof sig - 8; i++)
        len += snprintf(sig + len, sizeof sig - len, "%d%c", (int)err[i], clash[i] ? 'c' : '-');
    if (strcmp(sig, s_list_sig) != 0) {
        strcpy(s_list_sig, sig);
        lv_obj_clean(s_list);
        if (closed) {
            empty_note("Booking is closed for today", "Rooms can be booked from 07:00 to 22:00.");
        } else if (s_mode == MODE_ANY) {
            for (int i = 0; i < nfit; i++) add_room_row(fit[i], IX_BOOK_OK, NULL, i == 0);
            if (!nfit) empty_note("No room fits", "Try a later start, a shorter slot\nor a smaller group.");
        } else {
            for (int i = 0; i < n; i++) add_room_row(i, err[i], clash[i], false);
        }
    }

    lv_label_set_text(s_list_title, s_mode == MODE_ANY ? "Rooms that fit" : "Pick a room");
    lv_label_set_text_fmt(s_list_count, "%d of %d available", nfit, n);
    bool can = !closed && s_room >= 0 && err[s_room] == IX_BOOK_OK;
    lv_obj_set_state(s_next, LV_STATE_DISABLED, !can);
    if (s_room >= 0 && can) {
        char buf[48];
        snprintf(buf, sizeof buf, "Continue with %s  " LV_SYMBOL_RIGHT, ix_room(s_room)->id);
        ui_btn_set_text(s_next, buf);
    } else {
        ui_btn_set_text(s_next, "Pick a room");
    }
}

void ui_book_refresh(time_t now)
{
    clamp_times(now);
    for (int i = 0; i < 2; i++) lv_obj_set_state(s_mode_btn[i], LV_STATE_CHECKED, i == s_mode);
    lv_label_set_text_fmt(s_people_lbl, "%u", s_people);

    char a[8], b[8], d[24];
    if (s_from <= now) snprintf(a, sizeof a, "Now");
    else ui_fmt_hm(a, sizeof a, s_from);
    ui_fmt_hm(b, sizeof b, s_to);
    lv_label_set_text(s_from_lbl, a);
    lv_label_set_text(s_to_lbl, b);
    long dur = (long)(s_to - s_from);
    ui_fmt_dur(d, sizeof d, dur);
    lv_label_set_text_fmt(s_dur_lbl, "%s  ·  today  ·  max 4 h", d);
    for (int i = 0; i < 3; i++) lv_obj_set_state(s_dur_chip[i], LV_STATE_CHECKED, dur == s_quick_min[i] * 60);

    refresh_list(now);
}

void ui_book_start(int room, uint8_t people)
{
    time_t now = ix_now();
    s_mode = room >= 0 ? MODE_SPECIFIC : MODE_ANY;
    s_room = room;
    s_people = people ? people : 1;
    s_from = ix_day_first_slot(now);
    s_to = s_from + 3600;
    s_list_sig[0] = 0;
    clamp_times(now);
    ui_mark_busy();
}

void ui_book_reset(void) { ui_book_start(-1, 1); }

/* ---- step 2: name + email ---------------------------------------------- */

static void summary(char *buf, size_t n)
{
    char a[8], b[8];
    ui_fmt_hm(a, sizeof a, s_from);
    ui_fmt_hm(b, sizeof b, s_to);
    snprintf(buf, n, "%s  ·  Today %s – %s  ·  %u %s", ix_room(s_room)->id, a, b,
             s_people, s_people == 1 ? "person" : "people");
}

static void close_cb(lv_event_t *e) { ui_close_overlays(); }

static void done_cb(lv_event_t *e)
{
    ui_close_overlays();
    ui_book_reset();
    ui_overview_reset();
    ui_show_page(UI_PAGE_OVERVIEW);
}

static lv_obj_t *s_confirm;
static void done_timer_cb(lv_timer_t *t)
{
    if (s_confirm && lv_obj_is_valid(s_confirm)) done_cb(NULL);
    s_confirm = NULL;
}

static void show_confirmation(void)
{
    ui_close_overlays();
    lv_obj_t *o = ui_overlay(false);
    s_confirm = o;
    lv_obj_set_flex_flow(o, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(o, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(o, 14, 0);

    lv_obj_t *ok = ui_box(o);
    lv_obj_set_size(ok, 96, 96);
    lv_obj_set_style_radius(ok, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(ok, UI_C_FREE, 0);
    lv_obj_set_style_bg_opa(ok, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_color(ok, UI_C_FREE, 0);
    lv_obj_set_style_shadow_width(ok, 60, 0);
    lv_obj_set_style_shadow_opa(ok, LV_OPA_40, 0);
    lv_obj_t *ic = ui_label(ok, LV_SYMBOL_OK, &lv_font_montserrat_32, lv_color_white());
    lv_obj_center(ic);

    char buf[128];
    snprintf(buf, sizeof buf, "%s is yours", ix_room(s_room)->id);
    lv_obj_t *t = ui_label(o, buf, ui_f_h1, UI_C_TEXT);
    lv_obj_set_style_pad_top(t, 10, 0);
    summary(buf, sizeof buf);
    ui_label(o, buf, ui_f_body, UI_C_MUTED);

    lv_obj_t *info = ui_row(o, 12);
    lv_obj_set_style_bg_color(info, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(info, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(info, 14, 0);
    lv_obj_set_style_pad_all(info, 14, 0);
    lv_obj_set_style_border_color(info, UI_C_LINE, 0);
    lv_obj_set_style_border_width(info, 1, 0);
    ui_icon_tile(info, LV_SYMBOL_BELL, UI_C_GHOST, 40);
    char a[8], when[24];
    time_t now = ix_now();
    ui_fmt_hm(a, sizeof a, s_from > now ? s_from : now);
    if (IX_GHOST_RELEASE_S < 60) snprintf(when, sizeof when, "%d seconds", IX_GHOST_RELEASE_S);
    else snprintf(when, sizeof when, "%d minutes", IX_GHOST_RELEASE_S / 60);
    lv_obj_t *col = ui_col(info, 2);
    snprintf(buf, sizeof buf, "Be there within %s of %s", when, a);
    ui_label(col, buf, ui_f_label, UI_C_TEXT);
    ui_label(col, "Just walk in, the room sensor checks you in. Otherwise the room is released.",
             ui_f_small, UI_C_MUTED);

    lv_obj_t *d = ui_btn(o, "Done", UI_BTN_PRIMARY, done_cb, NULL);
    lv_obj_set_width(d, 240);
    lv_obj_set_style_pad_top(o, 0, 0);

    lv_timer_t *tm = lv_timer_create(done_timer_cb, 15000, NULL);
    lv_timer_set_repeat_count(tm, 1);
}

static void submit(void)
{
    ix_book_err_t r = ix_book(s_room, s_from, s_to, s_people,
                              lv_textarea_get_text(s_name_ta), lv_textarea_get_text(s_email_ta), ix_now());
    if (r == IX_BOOK_OK) { show_confirmation(); return; }
    lv_label_set_text_fmt(s_err, LV_SYMBOL_WARNING "  %s", ix_book_err_text(r));
}

static void submit_cb(lv_event_t *e) { submit(); }

static void focus_ta(lv_obj_t *ta)
{
    lv_obj_remove_state(s_name_ta, LV_STATE_FOCUSED);
    lv_obj_remove_state(s_email_ta, LV_STATE_FOCUSED);
    lv_obj_add_state(ta, LV_STATE_FOCUSED);
    lv_keyboard_set_textarea(s_kb, ta);
}

static void ta_cb(lv_event_t *e)
{
    focus_ta(lv_event_get_target(e));
    lv_label_set_text(s_err, "");
}

static void kb_cb(lv_event_t *e)
{
    ui_mark_busy();
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_READY) {
        if (lv_keyboard_get_textarea(s_kb) == s_name_ta) focus_ta(s_email_ta);
        else submit();
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text(s_err, "");
    }
}

static void domain_cb(lv_event_t *e)
{
    const char *d = lv_event_get_user_data(e);
    const char *cur = lv_textarea_get_text(s_email_ta);
    const char *at = strchr(cur, '@');
    char buf[IX_EMAIL_LEN];
    size_t keep = at ? (size_t)(at - cur) : strlen(cur);
    if (keep > sizeof buf - strlen(d) - 1) keep = sizeof buf - strlen(d) - 1;
    memcpy(buf, cur, keep);
    strcpy(buf + keep, d);
    lv_textarea_set_text(s_email_ta, buf);
    focus_ta(s_email_ta);
}

static lv_obj_t *field(lv_obj_t *parent, const char *title, const char *placeholder, int max)
{
    lv_obj_t *col = ui_col(parent, 6);
    lv_obj_set_flex_grow(col, 1);
    ui_caption(col, title);
    lv_obj_t *ta = lv_textarea_create(col);
    lv_obj_set_width(ta, lv_pct(100));
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_max_length(ta, max - 1);
    lv_textarea_set_placeholder_text(ta, placeholder);
    lv_obj_set_style_text_font(ta, ui_f_title, 0);
    lv_obj_set_style_text_color(ta, UI_C_TEXT, 0);
    lv_obj_set_style_text_color(ta, UI_C_FAINT, LV_PART_TEXTAREA_PLACEHOLDER);
    lv_obj_set_style_bg_color(ta, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(ta, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ta, 12, 0);
    lv_obj_set_style_pad_all(ta, 14, 0);
    lv_obj_set_style_border_color(ta, UI_C_LINE, 0);
    lv_obj_set_style_border_width(ta, 1, 0);
    lv_obj_set_style_border_color(ta, UI_C_ACCENT, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(ta, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(ta, UI_C_ACCENT, LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_obj_add_event_cb(ta, ta_cb, LV_EVENT_CLICKED, NULL);
    return ta;
}

/* LVGL's default letters layout with "@" instead of "_" (emails!) */
#define KB_BTN(w) (LV_BUTTONMATRIX_CTRL_POPOVER | (w))
#define KB_CHK    LV_BUTTONMATRIX_CTRL_CHECKED
static const char *const s_kb_lc[] = {
    "1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
    "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
    "@", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
    LV_SYMBOL_KEYBOARD, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, "" };
static const char *const s_kb_uc[] = {
    "1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
    "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
    "@", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
    LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, "" };
static const lv_buttonmatrix_ctrl_t s_kb_ctrl[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 5, KB_BTN(4), KB_BTN(4), KB_BTN(4), KB_BTN(4), KB_BTN(4),
    KB_BTN(4), KB_BTN(4), KB_BTN(4), KB_BTN(4), KB_BTN(4), KB_CHK | 7,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 6, KB_BTN(3), KB_BTN(3), KB_BTN(3), KB_BTN(3), KB_BTN(3),
    KB_BTN(3), KB_BTN(3), KB_BTN(3), KB_BTN(3), KB_CHK | 7,
    KB_BTN(1), KB_CHK | KB_BTN(1), KB_BTN(1), KB_BTN(1), KB_BTN(1), KB_BTN(1), KB_BTN(1),
    KB_BTN(1), KB_BTN(1), KB_CHK | KB_BTN(1), KB_CHK | KB_BTN(1), KB_CHK | KB_BTN(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2, KB_CHK | 2, 6, KB_CHK | 2, LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2 };

static void style_keyboard(lv_obj_t *kb)
{
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_TEXT_LOWER, s_kb_lc, s_kb_ctrl);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_TEXT_UPPER, s_kb_uc, s_kb_ctrl);
    lv_obj_set_style_bg_color(kb, UI_C_SIDEBAR, 0);
    lv_obj_set_style_bg_opa(kb, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(kb, 0, 0);
    lv_obj_set_style_pad_all(kb, 8, 0);
    lv_obj_set_style_pad_gap(kb, 6, 0);
    lv_obj_set_style_bg_color(kb, UI_C_CARD2, LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(kb, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_text_color(kb, UI_C_TEXT, LV_PART_ITEMS);
    lv_obj_set_style_text_font(kb, ui_f_title, LV_PART_ITEMS);
    lv_obj_set_style_radius(kb, 10, LV_PART_ITEMS);
    lv_obj_set_style_border_width(kb, 0, LV_PART_ITEMS);
    lv_obj_set_style_shadow_width(kb, 0, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(kb, UI_C_CARD3, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(kb, UI_C_ACCENT, LV_PART_ITEMS | LV_STATE_PRESSED);
}

static void details_cb(lv_event_t *e)
{
    if (s_room < 0) return;
    lv_obj_t *o = ui_overlay(false);
    lv_obj_set_style_bg_grad_color(o, lv_color_hex(0x0F1933), 0);
    lv_obj_set_style_bg_grad_dir(o, LV_GRAD_DIR_VER, 0);

    lv_obj_t *hdr = ui_row(o, 14);
    lv_obj_set_pos(hdr, 24, 16);
    lv_obj_set_width(hdr, UI_HOR_RES - 48);
    ui_round_btn(hdr, LV_SYMBOL_LEFT, close_cb, NULL);
    lv_obj_t *hc = ui_col(hdr, 2);
    ui_caption(hc, "ALMOST DONE");
    char buf[96];
    summary(buf, sizeof buf);
    ui_label(hc, buf, ui_f_h2, UI_C_TEXT);
    ui_spacer(hdr);
    ui_btn(hdr, "Book now  " LV_SYMBOL_OK, UI_BTN_PRIMARY, submit_cb, NULL);

    lv_obj_t *row = ui_row(o, 16);
    lv_obj_set_pos(row, 24, 86);
    lv_obj_set_width(row, UI_HOR_RES - 48);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    s_name_ta = field(row, "YOUR NAME", "e.g. Yousef", IX_NAME_LEN);
    s_email_ta = field(row, "EMAIL  ·  NO ACCOUNT NEEDED", "name@edu.aau.at", IX_EMAIL_LEN);

    lv_obj_t *hint = ui_row(o, 8);
    lv_obj_set_pos(hint, 24, 178);
    lv_obj_set_width(hint, UI_HOR_RES - 48);
    s_err = ui_label(hint, "", ui_f_label, UI_C_BUSY);
    lv_obj_t *priv = ui_label(hint, LV_SYMBOL_EYE_CLOSE "  Name and email stay on this screen, never online.",
                              ui_f_small, UI_C_FAINT);
    (void)priv;
    ui_spacer(hint);
    ui_btn(hint, "@edu.aau.at", UI_BTN_CHIP, domain_cb, "@edu.aau.at");
    ui_btn(hint, "@aau.at", UI_BTN_CHIP, domain_cb, "@aau.at");

    s_kb = lv_keyboard_create(o);
    lv_obj_set_size(s_kb, UI_HOR_RES, UI_VER_RES - 232);
    lv_obj_align(s_kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    style_keyboard(s_kb);
    lv_obj_add_event_cb(s_kb, kb_cb, LV_EVENT_ALL, NULL);
    focus_ta(s_name_ta);
}

/* ---- page -------------------------------------------------------------- */

void ui_book_create(lv_obj_t *page)
{
    lv_obj_t *left = ui_card(page);
    lv_obj_set_size(left, LEFT_W, UI_PAGE_H);
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(left, 10, 0);
    lv_obj_set_style_pad_all(left, 18, 0);

    ui_caption(left, "WHAT DO YOU NEED?");

    /* segmented control */
    lv_obj_t *seg = ui_row(left, 4);
    lv_obj_set_width(seg, lv_pct(100));
    lv_obj_set_style_bg_color(seg, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(seg, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(seg, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(seg, 4, 0);
    const char *names[2] = { "Any free room", "Specific room" };
    for (int i = 0; i < 2; i++) {
        lv_obj_t *b = ui_btn(seg, names[i], UI_BTN_GHOST, mode_cb, (void *)(intptr_t)i);
        lv_obj_set_flex_grow(b, 1);
        lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_pad_ver(b, 10, 0);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_STATE_CHECKED);
        lv_obj_set_style_bg_color(b, UI_C_ACCENT, LV_STATE_CHECKED);
        lv_obj_set_style_bg_grad_color(b, UI_C_ACCENT2, LV_STATE_CHECKED);
        lv_obj_set_style_bg_grad_dir(b, LV_GRAD_DIR_HOR, LV_STATE_CHECKED);
        lv_obj_set_style_text_color(b, lv_color_white(), LV_STATE_CHECKED);
        s_mode_btn[i] = b;
    }

    ui_stepper(left, "People", LV_SYMBOL_EYE_OPEN, &s_people_lbl, people_cb);
    ui_stepper(left, "From", LV_SYMBOL_PLAY, &s_from_lbl, from_cb);
    ui_stepper(left, "Until", LV_SYMBOL_STOP, &s_to_lbl, to_cb);

    lv_obj_t *q = ui_row(left, 8);
    lv_obj_set_width(q, lv_pct(100));
    const char *qn[3] = { "30 min", "1 h", "2 h" };
    for (int i = 0; i < 3; i++) {
        s_dur_chip[i] = ui_btn(q, qn[i], UI_BTN_CHIP, quick_cb, (void *)(intptr_t)i);
        lv_obj_set_flex_grow(s_dur_chip[i], 1);
    }
    s_dur_lbl = ui_label(left, "", ui_f_small, UI_C_MUTED);

    lv_obj_t *right = ui_card(page);
    lv_obj_set_pos(right, LEFT_W + UI_GAP, 0);
    lv_obj_set_size(right, UI_PAGE_W - LEFT_W - UI_GAP, UI_PAGE_H);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(right, 10, 0);
    lv_obj_set_style_pad_all(right, 18, 0);

    lv_obj_t *hdr = ui_row(right, 8);
    lv_obj_set_width(hdr, lv_pct(100));
    s_list_title = ui_label(hdr, "", ui_f_title, UI_C_TEXT);
    ui_spacer(hdr);
    s_list_count = ui_label(hdr, "", ui_f_small, UI_C_MUTED);

    s_list = ui_col(right, 8);
    lv_obj_set_width(s_list, lv_pct(100));
    lv_obj_set_flex_grow(s_list, 1);
    lv_obj_add_flag(s_list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(s_list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(s_list, LV_SCROLLBAR_MODE_OFF);

    s_next = ui_btn(right, "", UI_BTN_PRIMARY, details_cb, NULL);
    lv_obj_set_width(s_next, lv_pct(100));

    ui_book_reset();
}
