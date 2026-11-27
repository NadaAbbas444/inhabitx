/* Shell: sidebar navigation, header with greeting + clock, pages, toast,
 * idle reset. */
#include "ui_internal.h"

#include <stdio.h>

static lv_obj_t *s_pages[UI_PAGE_COUNT];
static lv_obj_t *s_nav[UI_PAGE_COUNT];
static lv_obj_t *s_greet, *s_title, *s_time, *s_date;
static lv_obj_t *s_live_pill, *s_live_sub;
static lv_obj_t *s_toast;
static lv_timer_t *s_toast_timer;
static ui_page_t s_page = UI_PAGE_OVERVIEW;
static bool s_busy;

static const struct { const char *icon, *text, *title; } s_nav_def[UI_PAGE_COUNT] = {
    { LV_SYMBOL_HOME,      "Overview",     "Find a free room" },
    { LV_SYMBOL_GPS,       "Campus map",   "Universität Klagenfurt" },
    { LV_SYMBOL_PLUS,      "Book a room",  "Book a room" },
    { LV_SYMBOL_DIRECTORY, "Lost & Found", "Lost & Found" },
};

void ui_mark_busy(void) { s_busy = true; }

static void refresh_page(time_t now)
{
    switch (s_page) {
    case UI_PAGE_OVERVIEW:  ui_overview_refresh(now); break;
    case UI_PAGE_CAMPUS:    ui_campus_refresh(now); break;
    case UI_PAGE_BOOK:      ui_book_refresh(now); break;
    case UI_PAGE_LOSTFOUND: ui_lf_refresh(now); break;
    default: break;
    }
}

static void opa_cb(void *obj, int32_t v) { lv_obj_set_style_opa(obj, (lv_opa_t)v, 0); }

void ui_show_page(ui_page_t p)
{
    for (int i = 0; i < UI_PAGE_COUNT; i++) {
        bool on = i == (int)p;
        if (on) lv_obj_remove_flag(s_pages[i], LV_OBJ_FLAG_HIDDEN);
        else    lv_obj_add_flag(s_pages[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_state(s_nav[i], LV_STATE_CHECKED, on);
    }
    s_page = p;
    lv_label_set_text(s_title, s_nav_def[p].title);
    if (p != UI_PAGE_OVERVIEW) ui_mark_busy();
    refresh_page(ix_now());

    /* short fade-in */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, s_pages[p]);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_duration(&a, 200);
    lv_anim_set_exec_cb(&a, opa_cb);
    lv_anim_start(&a);
}

static void nav_cb(lv_event_t *e)
{
    ui_page_t p = (ui_page_t)(intptr_t)lv_event_get_user_data(e);
    ui_close_overlays();
    if (p == UI_PAGE_BOOK && s_page != UI_PAGE_BOOK) ui_book_start(-1, 0);
    if (p == UI_PAGE_LOSTFOUND) ui_lf_reset();
    if (p == UI_PAGE_OVERVIEW) ui_overview_reset();
    ui_show_page(p);
}

/* ---- toast ------------------------------------------------------------ */

static void toast_hide(lv_timer_t *t)
{
    lv_obj_add_flag(s_toast, LV_OBJ_FLAG_HIDDEN);
    lv_timer_pause(t);
}

void ui_toast(const char *msg)
{
    lv_label_set_text(lv_obj_get_child(s_toast, 1), msg);
    lv_obj_remove_flag(s_toast, LV_OBJ_FLAG_HIDDEN);
    lv_timer_reset(s_toast_timer);
    lv_timer_resume(s_toast_timer);
}

static void on_model_event(ix_event_t ev, int room)
{
    char buf[80];
    const ix_room_info_t *r = ix_room(room);
    if (!r) return;
    if (ev == IX_EV_RELEASED) snprintf(buf, sizeof buf, "%s released: nobody showed up", r->id);
    else                      snprintf(buf, sizeof buf, "%s: welcome, you're checked in", r->id);
    ui_toast(buf);
}

/* ---- clock, live status, idle reset ----------------------------------- */

static void reset_all(void)
{
    ui_close_overlays();
    ui_book_reset();
    ui_lf_reset();
    ui_overview_reset();
    ui_show_page(UI_PAGE_OVERVIEW);
    s_busy = false;
}

static void tick_cb(lv_timer_t *t)
{
    time_t now = ix_now();
    ix_tick(now);

    struct tm tm;
    localtime_r(&now, &tm);
    static const char *wd[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    static const char *mo[] = { "January", "February", "March", "April", "May", "June", "July",
                                "August", "September", "October", "November", "December" };
    lv_label_set_text_fmt(s_time, "%02d:%02d", tm.tm_hour, tm.tm_min);
    lv_label_set_text_fmt(s_date, "%s, %d %s", wd[tm.tm_wday], tm.tm_mday, mo[tm.tm_mon]);
    lv_label_set_text(s_greet, tm.tm_hour < 12 ? "Good morning" : tm.tm_hour < 18 ? "Good afternoon" : "Good evening");

    int online = 0, n = ix_room_count();
    for (int i = 0; i < n; i++) {
        ix_status_t st;
        ix_room_status(i, now, &st);
        if (st.state != IX_OFFLINE) online++;
    }
    ui_pill_set(s_live_pill, online == n ? UI_C_FREE : UI_C_GHOST, online == n ? "All sensors live" : "Live");
    lv_label_set_text_fmt(s_live_sub, "%d of %d room sensors online", online, n);

    refresh_page(now);
    if (s_busy && lv_display_get_inactive_time(NULL) > UI_IDLE_RESET_MS) reset_all();
}

/* ---- build ------------------------------------------------------------ */

static void build_sidebar(lv_obj_t *scr)
{
    lv_obj_t *sb = ui_box(scr);
    lv_obj_set_size(sb, UI_SIDEBAR_W, UI_VER_RES);
    lv_obj_set_style_bg_color(sb, UI_C_SIDEBAR, 0);
    lv_obj_set_style_bg_opa(sb, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(sb, LV_BORDER_SIDE_RIGHT, 0);
    lv_obj_set_style_border_color(sb, UI_C_LINE, 0);
    lv_obj_set_style_border_width(sb, 1, 0);
    lv_obj_set_style_pad_all(sb, 16, 0);
    lv_obj_set_style_pad_top(sb, 22, 0);
    lv_obj_set_flex_flow(sb, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(sb, 6, 0);

    /* text version of the InhabitX wordmark until the logo image is added */
    lv_obj_t *logo = ui_row(sb, 0);
    lv_obj_set_style_pad_left(logo, 6, 0);
    ui_label(logo, "Inhabi", ui_f_h1, UI_C_TEXT);
    ui_label(logo, "X", ui_f_h1, UI_C_ACCENT);
    lv_obj_t *sub = ui_label(sb, "Smart Campus  ·  AAU", ui_f_small, UI_C_MUTED);
    lv_obj_set_style_pad_left(sub, 6, 0);
    lv_obj_set_style_pad_bottom(sub, 18, 0);

    for (int i = 0; i < UI_PAGE_COUNT; i++) {
        lv_obj_t *b = lv_button_create(sb);
        lv_obj_remove_style_all(b);
        lv_obj_set_size(b, lv_pct(100), 46);
        lv_obj_set_style_radius(b, UI_R_CTRL, 0);
        lv_obj_set_style_pad_hor(b, 14, 0);
        lv_obj_set_style_text_color(b, UI_C_MUTED, 0);
        lv_obj_set_style_bg_color(b, UI_C_CARD3, LV_STATE_CHECKED);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_STATE_CHECKED);
        lv_obj_set_style_bg_color(b, UI_C_CARD2, LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(b, LV_OPA_COVER, LV_STATE_PRESSED);
        lv_obj_set_style_text_color(b, UI_C_TEXT, LV_STATE_CHECKED);
        lv_obj_set_flex_flow(b, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(b, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(b, 12, 0);

        lv_obj_t *ic = lv_label_create(b);
        lv_label_set_text(ic, s_nav_def[i].icon);
        lv_obj_set_style_text_font(ic, &lv_font_montserrat_16, 0);
        lv_obj_set_width(ic, 20);
        lv_obj_t *l = lv_label_create(b);
        lv_label_set_text(l, s_nav_def[i].text);
        lv_obj_set_style_text_font(l, ui_f_label, 0);

        lv_obj_add_event_cb(b, nav_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        s_nav[i] = b;
    }

    ui_spacer(sb);

    lv_obj_t *bc = ui_card(sb);
    lv_obj_set_width(bc, lv_pct(100));
    lv_obj_set_style_pad_all(bc, 14, 0);
    lv_obj_set_style_radius(bc, 14, 0);
    lv_obj_set_flex_flow(bc, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(bc, 6, 0);
    ui_label(bc, UI_BUILDING_NAME, ui_f_title, UI_C_TEXT);
    ui_label(bc, "Zentralgebäude", ui_f_small, UI_C_MUTED);
    s_live_pill = ui_pill(bc);
    s_live_sub = ui_label(bc, "", ui_f_caption, UI_C_FAINT);
    lv_obj_set_width(s_live_sub, lv_pct(100));
    lv_label_set_long_mode(s_live_sub, LV_LABEL_LONG_WRAP);

    lv_obj_t *pw = ui_label(sb, "Powered by Infineon PSOC™ Edge", ui_f_caption, UI_C_FAINT);
    lv_obj_set_style_pad_top(pw, 6, 0);
    lv_obj_set_style_pad_left(pw, 4, 0);
}

static void build_header(lv_obj_t *scr)
{
    lv_obj_t *h = ui_box(scr);
    lv_obj_set_pos(h, UI_PAGE_X, 0);
    lv_obj_set_size(h, UI_PAGE_W, UI_HEADER_H);

    s_greet = ui_label(h, "", ui_f_body, UI_C_MUTED);
    lv_obj_set_pos(s_greet, 2, 18);
    s_title = ui_label(h, "", ui_f_h1, UI_C_TEXT);
    lv_obj_set_pos(s_title, 0, 38);

    s_time = ui_label(h, "", ui_f_h1, UI_C_TEXT);
    lv_obj_align(s_time, LV_ALIGN_TOP_RIGHT, 0, 16);
    s_date = ui_label(h, "", ui_f_small, UI_C_MUTED);
    lv_obj_align(s_date, LV_ALIGN_TOP_RIGHT, 0, 54);
}

static void build_toast(void)
{
    s_toast = ui_row(lv_layer_sys(), 10);
    lv_obj_set_style_bg_color(s_toast, UI_C_CARD3, 0);
    lv_obj_set_style_bg_opa(s_toast, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(s_toast, UI_C_ACCENT, 0);
    lv_obj_set_style_border_width(s_toast, 1, 0);
    lv_obj_set_style_radius(s_toast, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_hor(s_toast, 18, 0);
    lv_obj_set_style_pad_ver(s_toast, 10, 0);
    lv_obj_set_style_shadow_width(s_toast, 30, 0);
    lv_obj_set_style_shadow_opa(s_toast, LV_OPA_50, 0);
    lv_obj_set_style_shadow_color(s_toast, lv_color_black(), 0);
    ui_label(s_toast, LV_SYMBOL_BELL, &lv_font_montserrat_16, UI_C_ACCENT);
    ui_label(s_toast, "", ui_f_label, UI_C_TEXT);
    lv_obj_align(s_toast, LV_ALIGN_TOP_MID, UI_SIDEBAR_W / 2, 22);
    lv_obj_add_flag(s_toast, LV_OBJ_FLAG_HIDDEN);
    s_toast_timer = lv_timer_create(toast_hide, 3500, NULL);
    lv_timer_pause(s_toast_timer);
}

void ui_init(void)
{
    ui_fonts_init();

    lv_display_t *disp = lv_display_get_default();
    lv_theme_t *th = lv_theme_default_init(disp, UI_C_ACCENT, UI_C_ACCENT2, true, ui_f_body);
    lv_display_set_theme(disp, th);

    lv_obj_t *scr = lv_screen_active();
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, UI_C_BG, 0);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x0F1933), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_text_font(scr, ui_f_body, 0);
    lv_obj_set_style_text_color(scr, UI_C_TEXT, 0);

    build_sidebar(scr);
    build_header(scr);

    for (int i = 0; i < UI_PAGE_COUNT; i++) {
        lv_obj_t *p = ui_box(scr);
        lv_obj_set_pos(p, UI_PAGE_X, UI_PAGE_Y);
        lv_obj_set_size(p, UI_PAGE_W, UI_PAGE_H);
        s_pages[i] = p;
    }
    ui_overview_create(s_pages[UI_PAGE_OVERVIEW]);
    ui_campus_create(s_pages[UI_PAGE_CAMPUS]);
    ui_book_create(s_pages[UI_PAGE_BOOK]);
    ui_lf_create(s_pages[UI_PAGE_LOSTFOUND]);

    build_toast();

    ix_set_event_cb(on_model_event);
    lv_timer_t *t = lv_timer_create(tick_cb, 1000, NULL);
    ui_show_page(UI_PAGE_OVERVIEW);
    tick_cb(t);
    s_busy = false;
}
