/* Overview: hero "free right now", live rooms table, floor plan,
 * occupancy today. Tapping a room opens the detail drawer. */
#include "ui_internal.h"

#include <stdio.h>
#include <string.h>

#define COL_A_W   ((UI_PAGE_W * 61) / 100)
#define COL_B_X   (COL_A_W + UI_GAP)
#define COL_B_W   (UI_PAGE_W - COL_B_X)
#define HERO_H    160

/* ---- state ------------------------------------------------------------ */
static uint8_t s_people = 1;

static lv_obj_t *s_hero_num, *s_hero_text, *s_hero_sub, *s_stat[3];

typedef struct {
    int room;
    lv_obj_t *row, *tile, *tile_lbl, *seats, *pill, *detail, *book, *count;
} row_t;
static row_t s_rows[IX_MAX_ROOMS];
static int   s_nrows;
static lv_obj_t *s_people_lbl;

typedef struct { int room; lv_obj_t *tile, *lbl; } plan_tile_t;
static plan_tile_t s_ptiles[IX_MAX_ROOMS];
static int s_nptiles;

static lv_obj_t *s_chart, *s_occ_val;
static lv_chart_series_t *s_ser;

/* ---- hero ------------------------------------------------------------- */

static void refresh_hero(const ix_status_t *st, int n)
{
    int free = 0, busy = 0, ghost = 0, off = 0;
    for (int i = 0; i < n; i++) {
        if (ix_room(i)->seats < s_people) continue;
        switch (st[i].state) {
        case IX_FREE: free++; break;
        case IX_GHOST: ghost++; break;
        case IX_OFFLINE: off++; break;
        default: busy++; break;
        }
    }
    lv_label_set_text_fmt(s_hero_num, "%d", free);
    lv_label_set_text(s_hero_text, free == 1 ? "room free" : "rooms free");
    if (s_people > 1) lv_label_set_text_fmt(s_hero_sub, "for %u people  ·  floor 1", s_people);
    else lv_label_set_text(s_hero_sub, "floor 1  ·  Zentralgebäude");
    lv_label_set_text_fmt(lv_obj_get_child(s_stat[0], 1), "%d in use", busy);
    lv_label_set_text_fmt(lv_obj_get_child(s_stat[1], 1), "%d may free up", ghost);
    lv_label_set_text_fmt(lv_obj_get_child(s_stat[2], 1), "%d offline", off);
}

static lv_obj_t *hero_stat(lv_obj_t *parent, lv_color_t c)
{
    lv_obj_t *r = ui_row(parent, 8);
    lv_obj_set_width(r, 150);
    lv_obj_set_style_bg_color(r, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(r, 26, 0);
    lv_obj_set_style_radius(r, 10, 0);
    lv_obj_set_style_pad_hor(r, 10, 0);
    lv_obj_set_style_pad_ver(r, 7, 0);
    lv_obj_t *dot = ui_box(r);
    lv_obj_set_size(dot, 8, 8);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, c, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(dot, lv_color_white(), 0);
    lv_obj_set_style_border_width(dot, 1, 0);
    ui_label(r, "", ui_f_label, lv_color_white());
    return r;
}

static void build_hero(lv_obj_t *page)
{
    lv_obj_t *h = ui_box(page);
    lv_obj_set_size(h, COL_A_W, HERO_H);
    lv_obj_set_style_radius(h, UI_R_CARD, 0);
    lv_obj_set_style_bg_opa(h, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(h, lv_color_hex(0x2563EB), 0);
    lv_obj_set_style_bg_grad_color(h, UI_C_ACCENT2, 0);
    lv_obj_set_style_bg_grad_dir(h, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_clip_corner(h, true, 0);
    lv_obj_set_style_pad_all(h, 20, 0);
    lv_obj_set_style_shadow_color(h, UI_C_ACCENT, 0);
    lv_obj_set_style_shadow_width(h, 40, 0);
    lv_obj_set_style_shadow_opa(h, LV_OPA_20, 0);

    /* soft decorative circles */
    for (int i = 0; i < 2; i++) {
        lv_obj_t *c = ui_box(h);
        int d = i ? 140 : 220;
        lv_obj_set_size(c, d, d);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(c, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(c, i ? 14 : 10, 0);
        lv_obj_add_flag(c, LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING);
        lv_obj_align(c, LV_ALIGN_TOP_RIGHT, i ? -150 : 70, i ? 70 : -90);
    }

    lv_obj_t *cap = ui_caption(h, "FREE RIGHT NOW");
    lv_obj_set_style_text_color(cap, lv_color_white(), 0);
    lv_obj_set_style_text_opa(cap, 190, 0);
    lv_obj_align(cap, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *r = ui_row(h, 14);
    lv_obj_align(r, LV_ALIGN_BOTTOM_LEFT, 0, 4);
    s_hero_num = ui_label(r, "0", ui_f_hero, lv_color_white());
    lv_obj_t *c = ui_col(r, 2);
    s_hero_text = ui_label(c, "", ui_f_h2, lv_color_white());
    s_hero_sub = ui_label(c, "", ui_f_small, lv_color_white());
    lv_obj_set_style_text_opa(s_hero_sub, 200, 0);

    lv_obj_t *stats = ui_col(h, 6);
    lv_obj_align(stats, LV_ALIGN_RIGHT_MID, 0, 0);
    s_stat[0] = hero_stat(stats, UI_C_BUSY);
    s_stat[1] = hero_stat(stats, UI_C_GHOST);
    s_stat[2] = hero_stat(stats, UI_C_OFFLINE);
}

/* ---- rooms table ------------------------------------------------------ */

static void row_cb(lv_event_t *e) { ui_room_drawer((int)(intptr_t)lv_event_get_user_data(e)); }

static void book_cb(lv_event_t *e)
{
    ui_book_start((int)(intptr_t)lv_event_get_user_data(e), s_people);
    ui_show_page(UI_PAGE_BOOK);
}

static void people_cb(lv_event_t *e)
{
    int p = s_people + (int)(intptr_t)lv_event_get_user_data(e);
    s_people = (uint8_t)(p < 1 ? 1 : p > 60 ? 60 : p);
    ui_mark_busy();
    ui_overview_refresh(ix_now());
}

static void build_rooms(lv_obj_t *page)
{
    lv_obj_t *card = ui_card(page);
    lv_obj_set_pos(card, 0, HERO_H + UI_GAP);
    lv_obj_set_size(card, COL_A_W, UI_PAGE_H - HERO_H - UI_GAP);
    lv_obj_set_style_pad_all(card, 14, 0);
    lv_obj_set_style_pad_left(card, 18, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(card, 4, 0);

    lv_obj_t *hdr = ui_row(card, 8);
    lv_obj_set_width(hdr, lv_pct(100));
    lv_obj_set_style_pad_bottom(hdr, 4, 0);
    ui_label(hdr, "Rooms", ui_f_title, UI_C_TEXT);
    lv_obj_t *fl = ui_label(hdr, "Floor 1", ui_f_caption, UI_C_ACCENT);
    lv_obj_set_style_bg_color(fl, UI_C_ACCENT, 0);
    lv_obj_set_style_bg_opa(fl, 40, 0);
    lv_obj_set_style_radius(fl, 6, 0);
    lv_obj_set_style_pad_hor(fl, 8, 0);
    lv_obj_set_style_pad_ver(fl, 3, 0);
    ui_spacer(hdr);
    ui_label(hdr, "Group size", ui_f_small, UI_C_MUTED);
    lv_obj_t *m = ui_round_btn(hdr, LV_SYMBOL_MINUS, people_cb, (void *)(intptr_t)-1);
    lv_obj_set_size(m, 36, 36);
    s_people_lbl = ui_label(hdr, "1", ui_f_title, UI_C_TEXT);
    lv_obj_set_width(s_people_lbl, 28);
    lv_obj_set_style_text_align(s_people_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_t *p = ui_round_btn(hdr, LV_SYMBOL_PLUS, people_cb, (void *)(intptr_t)1);
    lv_obj_set_size(p, 36, 36);

    lv_obj_t *list = ui_col(card, 0);
    lv_obj_set_width(list, lv_pct(100));
    lv_obj_set_flex_grow(list, 1);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    s_nrows = 0;
    for (int i = 0; i < ix_room_count() && s_nrows < IX_MAX_ROOMS; i++) {
        row_t *r = &s_rows[s_nrows++];
        r->room = i;
        r->row = ui_row(list, 12);
        lv_obj_set_size(r->row, lv_pct(100), 46);
        lv_obj_set_style_border_side(r->row, LV_BORDER_SIDE_TOP, 0);
        lv_obj_set_style_border_color(r->row, UI_C_LINE, 0);
        lv_obj_set_style_border_width(r->row, i ? 1 : 0, 0);
        lv_obj_set_style_bg_color(r->row, UI_C_CARD2, LV_STATE_PRESSED);
        lv_obj_set_style_bg_opa(r->row, LV_OPA_COVER, LV_STATE_PRESSED);
        lv_obj_add_flag(r->row, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(r->row, row_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        const char *id = ix_room(i)->id;
        r->tile = ui_box(r->row);
        lv_obj_set_size(r->tile, 36, 36);
        lv_obj_set_style_radius(r->tile, 10, 0);
        lv_obj_set_style_bg_opa(r->tile, 45, 0);
        const char *dot = strrchr(id, '.');
        r->tile_lbl = ui_label(r->tile, dot ? dot + 1 : id, ui_f_label, UI_C_TEXT);
        lv_obj_center(r->tile_lbl);

        lv_obj_t *c = ui_col(r->row, 0);
        lv_obj_set_width(c, 92);
        lv_obj_add_flag(c, LV_OBJ_FLAG_EVENT_BUBBLE);
        ui_label(c, id, ui_f_label, UI_C_TEXT);
        r->seats = ui_label(c, "", ui_f_small, UI_C_MUTED);

        r->pill = ui_pill(r->row);
        lv_obj_set_width(r->pill, 96);
        r->detail = ui_label(r->row, "", ui_f_small, UI_C_MUTED);
        lv_obj_set_flex_grow(r->detail, 1);

        r->book = ui_btn(r->row, "Book", UI_BTN_PRIMARY, book_cb, (void *)(intptr_t)i);
        lv_obj_set_style_pad_ver(r->book, 7, 0);
        lv_obj_set_style_pad_hor(r->book, 16, 0);
        lv_obj_set_style_shadow_width(r->book, 0, 0);
    }
}

static void refresh_rooms(time_t now, const ix_status_t *st)
{
    lv_label_set_text_fmt(s_people_lbl, "%u", s_people);
    for (int i = 0; i < s_nrows; i++) {
        row_t *r = &s_rows[i];
        const ix_status_t *s = &st[r->room];
        lv_color_t c = ui_state_color(s->state);
        bool fits = ix_room(r->room)->seats >= s_people;

        lv_obj_set_style_bg_color(r->tile, c, 0);
        lv_obj_set_style_text_color(r->tile_lbl, lv_color_mix(c, lv_color_white(), 140), 0);
        lv_label_set_text_fmt(r->seats, "%u seats", ix_room(r->room)->seats);
        ui_pill_set(r->pill, c, ui_state_text(s->state));

        char d[32];
        if (!fits) snprintf(d, sizeof d, "too small for %u", s_people);
        else ui_status_detail(s, d, sizeof d);
        lv_label_set_text(r->detail, d);
        lv_obj_set_style_text_color(r->detail, s->state == IX_GHOST ? UI_C_GHOST : UI_C_MUTED, 0);

        bool can = fits && s->state == IX_FREE;
        if (can) lv_obj_remove_flag(r->book, LV_OBJ_FLAG_HIDDEN);
        else     lv_obj_add_flag(r->book, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_opa(r->row, fits ? LV_OPA_COVER : LV_OPA_50, 0);
    }
}

/* ---- floor plan ------------------------------------------------------- */

static void plan_cb(lv_event_t *e) { ui_room_drawer((int)(intptr_t)lv_event_get_user_data(e)); }

/* Draws live room tiles, faint labels and the "you are here" dot on a plan image. */
static int plan_overlay(lv_obj_t *img, const ui_plan_room_t *rooms, int count, lv_point_t here,
                        plan_tile_t *tiles, bool big)
{
    int n = 0;
    for (int i = 0; i < count; i++) {
        const ui_plan_room_t *pr = &rooms[i];
        int room = pr->kind == UI_PLAN_LIVE ? ix_room_find(pr->id) : -1;
        if (pr->kind == UI_PLAN_LIVE && room >= 0) {
            plan_tile_t *t = &tiles[n++];
            t->room = room;
            t->tile = ui_box(img);
            lv_obj_set_pos(t->tile, pr->x + 2, pr->y + 2);
            lv_obj_set_size(t->tile, pr->w - 3, pr->h - 3);
            lv_obj_set_style_radius(t->tile, 3, 0);
            lv_obj_set_style_bg_opa(t->tile, 90, 0);
            lv_obj_set_style_border_width(t->tile, 2, 0);
            lv_obj_set_style_border_opa(t->tile, 200, 0);
            lv_obj_add_flag(t->tile, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(t->tile, plan_cb, LV_EVENT_CLICKED, (void *)(intptr_t)room);
            /* official name if it has one ("HS 4"), else the short code ("1.08") */
            bool named = strcmp(pr->name, pr->id) != 0;
            char buf[32];
            if (big && named) snprintf(buf, sizeof buf, "%s\n%s", pr->name, pr->id);
            else snprintf(buf, sizeof buf, "%s", named ? pr->name : (big ? pr->id : pr->id + 2));
            t->lbl = ui_label(t->tile, buf, big ? ui_f_label : ui_f_caption, UI_C_TEXT);
            lv_obj_set_style_text_align(t->lbl, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_center(t->lbl);
        } else if (pr->kind != UI_PLAN_HALL && pr->w >= (big ? 30 : 36) && pr->h >= (big ? 12 : 16)) {
            bool named = strcmp(pr->name, pr->id) != 0;
            lv_obj_t *l = ui_label(img, named ? pr->name : pr->kind == UI_PLAN_ROOM ? pr->id + 2 : pr->id,
                                   big ? ui_f_small : ui_f_caption, UI_C_FAINT);
            lv_obj_update_layout(l);
            lv_obj_set_pos(l, pr->x + (pr->w - lv_obj_get_width(l)) / 2, pr->y + (pr->h - lv_obj_get_height(l)) / 2);
        }
    }

    lv_obj_t *dot = ui_box(img);
    lv_obj_set_size(dot, 12, 12);
    lv_obj_set_pos(dot, here.x - 6, here.y - 6);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(dot, UI_C_ACCENT, 0);
    lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(dot, lv_color_white(), 0);
    lv_obj_set_style_border_width(dot, 2, 0);
    lv_obj_set_style_shadow_color(dot, UI_C_ACCENT, 0);
    lv_obj_set_style_shadow_width(dot, 14, 0);
    lv_obj_set_style_shadow_spread(dot, 3, 0);
    lv_obj_t *yl = ui_label(img, "You are here", ui_f_caption, UI_C_ACCENT);
    lv_obj_set_style_bg_color(yl, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(yl, 220, 0);
    lv_obj_set_style_pad_hor(yl, 4, 0);
    lv_obj_set_style_radius(yl, 4, 0);
    lv_obj_update_layout(yl);
    lv_obj_set_pos(yl, here.x + 10, here.y - lv_obj_get_height(yl) / 2);
    return n;
}

static void paint_tiles(plan_tile_t *tiles, int n, const ix_status_t *st)
{
    for (int i = 0; i < n; i++) {
        lv_color_t c = ui_state_color(st[tiles[i].room].state);
        lv_obj_set_style_bg_color(tiles[i].tile, c, 0);
        lv_obj_set_style_border_color(tiles[i].tile, c, 0);
        lv_obj_set_style_text_color(tiles[i].lbl, lv_color_mix(c, lv_color_white(), 70), 0);
    }
}

/* ---- whole floor, full screen ------------------------------------------ */
static lv_obj_t *s_full;
static plan_tile_t s_ftiles[IX_MAX_ROOMS];
static int s_nftiles;

static void full_close_cb(lv_event_t *e) { lv_obj_delete(s_full); s_full = NULL; }

static void open_full_cb(lv_event_t *e)
{
    if (lv_event_get_target(e) != lv_event_get_current_target(e)) return;   /* a room tile was tapped */
    s_full = ui_overlay(false);
    lv_obj_set_style_bg_grad_color(s_full, lv_color_hex(0x0F1933), 0);
    lv_obj_set_style_bg_grad_dir(s_full, LV_GRAD_DIR_VER, 0);

    lv_obj_t *hdr = ui_row(s_full, 14);
    lv_obj_set_pos(hdr, 20, 14);
    lv_obj_set_width(hdr, UI_HOR_RES - 40);
    ui_round_btn(hdr, LV_SYMBOL_LEFT, full_close_cb, NULL);
    lv_obj_t *c = ui_col(hdr, 0);
    ui_caption(c, "ZENTRALGEBÄUDE");
    ui_label(c, "Floor 1", ui_f_h2, UI_C_TEXT);
    ui_spacer(hdr);
    static const ix_state_t legend[] = { IX_FREE, IX_OCCUPIED, IX_GHOST, IX_BOOKED, IX_OFFLINE };
    for (int i = 0; i < 5; i++) {
        lv_obj_t *p = ui_pill(hdr);
        ui_pill_set(p, ui_state_color(legend[i]), ui_state_text(legend[i]));
    }

    lv_obj_t *card = ui_card(s_full);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_set_size(card, ui_img_floorplan_full.header.w, ui_img_floorplan_full.header.h);
    lv_obj_align(card, LV_ALIGN_CENTER, 0, 16);
    lv_obj_t *img = lv_image_create(card);
    lv_image_set_src(img, &ui_img_floorplan_full);
    s_nftiles = plan_overlay(img, ui_plan_full_rooms, ui_plan_full_room_count, ui_plan_full_here, s_ftiles, true);

    lv_obj_t *note = ui_label(s_full, "Tap a coloured room for details and booking.   Plan traced from campusplan.aau.at",
                              ui_f_small, UI_C_FAINT);
    lv_obj_align(note, LV_ALIGN_BOTTOM_MID, 0, -18);
    ui_overview_refresh(ix_now());
}

static void build_plan(lv_obj_t *page, int h)
{
    lv_obj_t *card = ui_card(page);
    lv_obj_set_pos(card, COL_B_X, 0);
    lv_obj_set_size(card, COL_B_W, h);

    lv_obj_t *hdr = ui_row(card, 8);
    lv_obj_set_width(hdr, lv_pct(100));
    ui_label(hdr, "Floor 1", ui_f_title, UI_C_TEXT);
    ui_spacer(hdr);
    lv_obj_t *all = ui_btn(hdr, "Whole floor  " LV_SYMBOL_RIGHT, UI_BTN_CHIP, open_full_cb, NULL);
    lv_obj_set_style_pad_ver(all, 6, 0);

    /* walls, doors, stairs: pre-drawn from the real building outline; tap = whole floor */
    lv_obj_t *plan = lv_image_create(card);
    lv_image_set_src(plan, &ui_img_floorplan);
    lv_obj_set_pos(plan, 0, 40);
    lv_obj_add_flag(plan, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(plan, open_full_cb, LV_EVENT_CLICKED, NULL);
    s_nptiles = plan_overlay(plan, ui_plan_rooms, ui_plan_room_count, ui_plan_here, s_ptiles, false);
}

static void refresh_plan(const ix_status_t *st)
{
    paint_tiles(s_ptiles, s_nptiles, st);
    if (s_full && lv_obj_is_valid(s_full)) paint_tiles(s_ftiles, s_nftiles, st);
    else s_full = NULL;
}

/* ---- occupancy chart ------------------------------------------------- */

static void build_occupancy(lv_obj_t *page, int y, int h)
{
    lv_obj_t *card = ui_card(page);
    lv_obj_set_pos(card, COL_B_X, y);
    lv_obj_set_size(card, COL_B_W, h);

    ui_caption(card, "OCCUPANCY TODAY");
    lv_obj_t *r = ui_row(card, 8);
    lv_obj_set_pos(r, 0, 20);
    s_occ_val = ui_label(r, "0%", ui_f_h1, UI_C_TEXT);
    ui_label(r, "of rooms in use", ui_f_small, UI_C_MUTED);

    s_chart = lv_chart_create(card);
    lv_obj_set_size(s_chart, COL_B_W - 32, h - 32 - 84);
    lv_obj_set_pos(s_chart, 0, 62);
    lv_obj_set_style_bg_opa(s_chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(s_chart, 0, 0);
    lv_obj_set_style_pad_all(s_chart, 0, 0);
    lv_obj_set_style_line_color(s_chart, UI_C_LINE, 0);
    lv_obj_set_style_line_width(s_chart, 3, LV_PART_ITEMS);
    lv_obj_set_style_size(s_chart, 0, 0, LV_PART_INDICATOR);
    lv_chart_set_type(s_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_div_line_count(s_chart, 3, 0);
    lv_chart_set_range(s_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_chart_set_point_count(s_chart, IX_HIST_BUCKETS);
    s_ser = lv_chart_add_series(s_chart, UI_C_ACCENT, LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_t *ax = ui_box(card);
    lv_obj_set_width(ax, COL_B_W - 32);
    lv_obj_align(ax, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    char buf[8];
    for (int i = 0; i <= 3; i++) {
        int hr = IX_DAY_OPEN_H + i * (IX_DAY_CLOSE_H - IX_DAY_OPEN_H) / 3;
        snprintf(buf, sizeof buf, "%02d:00", hr);
        lv_obj_t *l = ui_label(ax, buf, ui_f_caption, UI_C_FAINT);
        lv_obj_align(l, i == 0 ? LV_ALIGN_LEFT_MID : i == 3 ? LV_ALIGN_RIGHT_MID : LV_ALIGN_CENTER,
                     i == 1 ? -(COL_B_W - 32) / 6 : i == 2 ? (COL_B_W - 32) / 6 : 0, 0);
    }
}

static void refresh_occupancy(time_t now)
{
    lv_label_set_text_fmt(s_occ_val, "%d%%", ix_occupancy_now(now));
    for (int b = 0; b < IX_HIST_BUCKETS; b++) {
        int v = ix_hist_get(b);
        lv_chart_set_value_by_id(s_chart, s_ser, b, v < 0 ? LV_CHART_POINT_NONE : v);
    }
    lv_chart_refresh(s_chart);
}

/* ---- room drawer ------------------------------------------------------ */

static int s_drawer_room = -1;

static void drawer_book_cb(lv_event_t *e)
{
    int room = s_drawer_room;
    ui_close_overlays();
    ui_book_start(room, s_people);
    ui_show_page(UI_PAGE_BOOK);
}

static void drawer_close_cb(lv_event_t *e) { lv_obj_delete(lv_event_get_user_data(e)); }

static void x_cb(void *obj, int32_t v) { lv_obj_set_x(obj, v); }

void ui_room_drawer(int room)
{
    time_t now = ix_now();
    s_drawer_room = room;
    const ix_room_info_t *r = ix_room(room);
    ix_status_t st;
    ix_room_status(room, now, &st);

    lv_obj_t *o = ui_overlay(true);
    int w = 400;
    lv_obj_t *d = ui_box(o);
    lv_obj_set_size(d, w, UI_VER_RES);
    lv_obj_set_style_bg_color(d, UI_C_CARD, 0);
    lv_obj_set_style_bg_opa(d, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(d, LV_BORDER_SIDE_LEFT, 0);
    lv_obj_set_style_border_color(d, UI_C_LINE, 0);
    lv_obj_set_style_border_width(d, 1, 0);
    lv_obj_set_style_pad_all(d, 24, 0);
    lv_obj_set_flex_flow(d, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(d, 10, 0);
    lv_obj_add_flag(d, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *top = ui_row(d, 8);
    lv_obj_set_width(top, lv_pct(100));
    ui_caption(top, "ROOM DETAILS");
    ui_spacer(top);
    ui_round_btn(top, LV_SYMBOL_CLOSE, drawer_close_cb, o);

    lv_obj_t *idrow = ui_row(d, 12);
    ui_icon_tile(idrow, LV_SYMBOL_HOME, ui_state_color(st.state), 52);
    lv_obj_t *c = ui_col(idrow, 2);
    ui_label(c, r->id, ui_f_h1, UI_C_TEXT);
    char buf[64];
    snprintf(buf, sizeof buf, "%u seats  ·  floor %u  ·  Zentralgebäude", r->seats, r->floor);
    ui_label(c, buf, ui_f_small, UI_C_MUTED);

    lv_obj_t *sr = ui_row(d, 10);
    lv_obj_t *pill = ui_pill(sr);
    ui_pill_set(pill, ui_state_color(st.state), ui_state_text(st.state));
    ui_status_detail(&st, buf, sizeof buf);
    ui_label(sr, buf, ui_f_small, st.state == IX_GHOST ? UI_C_GHOST : UI_C_MUTED);

    lv_obj_t *cap = ui_caption(d, "TODAY");
    lv_obj_set_style_pad_top(cap, 14, 0);

    /* timeline 07:00 - 22:00 */
    int tw = w - 48;
    time_t open = ix_day_first_slot(now);
    struct tm tm;
    localtime_r(&now, &tm);
    tm.tm_hour = IX_DAY_OPEN_H; tm.tm_min = 0; tm.tm_sec = 0;
    open = mktime(&tm);
    long span = (IX_DAY_CLOSE_H - IX_DAY_OPEN_H) * 3600L;

    lv_obj_t *tl = ui_box(d);
    lv_obj_set_size(tl, tw, 40);
    lv_obj_set_style_bg_color(tl, UI_C_CARD2, 0);
    lv_obj_set_style_bg_opa(tl, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(tl, 10, 0);
    lv_obj_set_style_clip_corner(tl, true, 0);

    const ix_booking_t *bk[8];
    int n = ix_room_bookings(room, open, bk, 8);
    for (int i = 0; i < n; i++) {
        long a = (long)(bk[i]->start - open), b = (long)(bk[i]->end - open);
        if (a < 0) a = 0;
        if (b > span) b = span;
        lv_obj_t *seg = ui_box(tl);
        lv_obj_set_pos(seg, (int)(a * tw / span), 0);
        lv_obj_set_size(seg, (int)((b - a) * tw / span) < 4 ? 4 : (int)((b - a) * tw / span), 40);
        lv_obj_set_style_bg_color(seg, UI_C_BOOKED, 0);
        lv_obj_set_style_bg_opa(seg, 150, 0);
    }
    long nowx = (long)(now - open);
    if (nowx >= 0 && nowx <= span) {
        lv_obj_t *nl = ui_box(tl);
        lv_obj_set_pos(nl, (int)(nowx * tw / span) - 1, 0);
        lv_obj_set_size(nl, 3, 40);
        lv_obj_set_style_bg_color(nl, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(nl, LV_OPA_COVER, 0);
    }
    lv_obj_t *ax = ui_box(d);
    lv_obj_set_width(ax, tw);
    for (int h = IX_DAY_OPEN_H; h <= IX_DAY_CLOSE_H; h += 3) {
        char t[8];
        snprintf(t, sizeof t, "%02d", h);
        lv_obj_t *l = ui_label(ax, t, ui_f_caption, UI_C_FAINT);
        lv_obj_set_x(l, (h - IX_DAY_OPEN_H) * 3600L * tw / span - (h == IX_DAY_CLOSE_H ? 14 : 0));
    }

    if (n == 0) {
        ui_label(d, "No bookings for the rest of today.", ui_f_body, UI_C_MUTED);
    }
    for (int i = 0; i < n && i < 4; i++) {
        lv_obj_t *br = ui_row(d, 10);
        lv_obj_t *dd = ui_box(br);
        lv_obj_set_size(dd, 8, 8);
        lv_obj_set_style_radius(dd, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dd, UI_C_BOOKED, 0);
        lv_obj_set_style_bg_opa(dd, LV_OPA_COVER, 0);
        char a[8], b[8];
        ui_fmt_hm(a, sizeof a, bk[i]->start);
        ui_fmt_hm(b, sizeof b, bk[i]->end);
        snprintf(buf, sizeof buf, "%s – %s", a, b);
        ui_label(br, buf, ui_f_label, UI_C_TEXT);
        snprintf(buf, sizeof buf, "booked  ·  %u %s", bk[i]->people, bk[i]->people == 1 ? "person" : "people");
        ui_label(br, buf, ui_f_small, UI_C_MUTED);
    }

    ui_spacer(d);
    lv_obj_t *bb = ui_btn(d, "Book this room  " LV_SYMBOL_RIGHT, UI_BTN_PRIMARY, drawer_book_cb, NULL);
    lv_obj_set_width(bb, lv_pct(100));
    if (st.state == IX_OFFLINE) lv_obj_add_state(bb, LV_STATE_DISABLED);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, d);
    lv_anim_set_values(&a, UI_HOR_RES, UI_HOR_RES - w);
    lv_anim_set_duration(&a, 220);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a, x_cb);
    lv_anim_start(&a);
}

/* ---- page ------------------------------------------------------------- */

void ui_overview_refresh(time_t now)
{
    ix_status_t st[IX_MAX_ROOMS];
    int n = ix_room_count();
    for (int i = 0; i < n; i++) ix_room_status(i, now, &st[i]);
    refresh_hero(st, n);
    refresh_rooms(now, st);
    refresh_plan(st);
    refresh_occupancy(now);
}

void ui_overview_reset(void) { s_people = 1; }

void ui_overview_create(lv_obj_t *page)
{
    build_hero(page);
    build_rooms(page);
    int plan_h = UI_PAGE_H * 60 / 100;   /* keep in sync with assets/gen_floorplan.py */
    build_plan(page, plan_h);
    build_occupancy(page, plan_h + UI_GAP, UI_PAGE_H - plan_h - UI_GAP);
}
