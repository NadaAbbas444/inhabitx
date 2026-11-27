/* InhabitX master: broker messages. See ix_msg.h and README §4. */
#include "ix_msg.h"
#include "ix_model.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- small JSON reader for flat objects ---------------------------------
 * The requests are one flat object each. This walks the top level properly
 * (strings with escapes, nested values skipped), so a key that appears inside
 * a string value is never mistaken for a real key. */

typedef struct { const char *p, *end; } cur_t;

static void ws(cur_t *c)
{
    while (c->p < c->end && (*c->p == ' ' || *c->p == '\t' || *c->p == '\n' || *c->p == '\r')) c->p++;
}

/* c->p on the opening quote. Leaves c->p after the closing quote. */
static bool skip_string(cur_t *c, const char **s, size_t *len)
{
    if (c->p >= c->end || *c->p != '"') return false;
    const char *start = ++c->p;
    while (c->p < c->end && *c->p != '"') {
        if ((unsigned char)*c->p < 0x20) return false;
        if (*c->p == '\\') c->p++;
        c->p++;
    }
    if (c->p >= c->end) return false;
    if (s) *s = start;
    if (len) *len = (size_t)(c->p - start);
    c->p++;
    return true;
}

static bool skip_value(cur_t *c)
{
    ws(c);
    if (c->p >= c->end) return false;
    if (*c->p == '"') return skip_string(c, NULL, NULL);
    if (*c->p == '{' || *c->p == '[') {
        int depth = 0;
        while (c->p < c->end) {
            char ch = *c->p;
            if (ch == '"') { if (!skip_string(c, NULL, NULL)) return false; continue; }
            if (ch == '{' || ch == '[') depth++;
            else if (ch == '}' || ch == ']') { if (--depth == 0) { c->p++; return true; } }
            c->p++;
        }
        return false;
    }
    const char *start = c->p;
    while (c->p < c->end && *c->p != ',' && *c->p != '}' && *c->p != ' ' &&
           *c->p != '\t' && *c->p != '\n' && *c->p != '\r') c->p++;
    return c->p > start;
}

/* Finds key at the top level. val, vlen: the raw value (strings without
 * their quotes, still escaped). *is_str tells which. */
static bool json_get(const char *js, size_t n, const char *key,
                     const char **val, size_t *vlen, bool *is_str)
{
    cur_t c = { js, js + n };
    ws(&c);
    if (c.p >= c.end || *c.p != '{') return false;
    c.p++;
    size_t klen = strlen(key);
    for (;;) {
        ws(&c);
        if (c.p < c.end && *c.p == '}') return false;
        const char *k; size_t kl;
        if (!skip_string(&c, &k, &kl)) return false;
        ws(&c);
        if (c.p >= c.end || *c.p != ':') return false;
        c.p++;
        ws(&c);
        const char *v = c.p;
        bool str = c.p < c.end && *c.p == '"';
        if (!skip_value(&c)) return false;
        if (kl == klen && memcmp(k, key, klen) == 0) {
            if (str) { *val = v + 1; *vlen = (size_t)(c.p - v) - 2; }
            else     { *val = v;     *vlen = (size_t)(c.p - v); }
            if (is_str) *is_str = str;
            return true;
        }
        ws(&c);
        if (c.p < c.end && *c.p == ',') { c.p++; continue; }
        return false;
    }
}

static int hex4(const char *s)
{
    int v = 0;
    for (int i = 0; i < 4; i++) {
        char ch = s[i];
        v <<= 4;
        if (ch >= '0' && ch <= '9') v |= ch - '0';
        else if (ch >= 'a' && ch <= 'f') v |= ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F') v |= ch - 'A' + 10;
        else return -1;
    }
    return v;
}

static size_t put_utf8(char *o, size_t room, uint32_t cp)
{
    char t[4]; size_t k;
    if (cp < 0x80)         { t[0] = (char)cp; k = 1; }
    else if (cp < 0x800)   { t[0] = (char)(0xC0 | cp >> 6);  t[1] = (char)(0x80 | (cp & 0x3F)); k = 2; }
    else if (cp < 0x10000) { t[0] = (char)(0xE0 | cp >> 12); t[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
                             t[2] = (char)(0x80 | (cp & 0x3F)); k = 3; }
    else                   { t[0] = (char)(0xF0 | cp >> 18); t[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
                             t[2] = (char)(0x80 | ((cp >> 6) & 0x3F)); t[3] = (char)(0x80 | (cp & 0x3F)); k = 4; }
    if (k > room) return 0;
    memcpy(o, t, k);
    return k;
}

/* String value, unescaped, 0-terminated. False if missing, not a string,
 * malformed, or longer than out. */
static bool get_str(const char *js, size_t n, const char *key, char *out, size_t outn)
{
    const char *v; size_t vl; bool str;
    if (!json_get(js, n, key, &v, &vl, &str) || !str || outn == 0) return false;
    size_t o = 0;
    for (size_t i = 0; i < vl; i++) {
        uint32_t ch = (unsigned char)v[i];
        if (ch == '\\') {
            if (++i >= vl) return false;
            switch (v[i]) {
            case '"': ch = '"'; break;   case '\\': ch = '\\'; break; case '/': ch = '/'; break;
            case 'b': ch = '\b'; break;  case 'f': ch = '\f'; break;  case 'n': ch = '\n'; break;
            case 'r': ch = '\r'; break;  case 't': ch = '\t'; break;
            case 'u': {
                if (i + 4 >= vl) return false;
                int h = hex4(v + i + 1);
                if (h < 0) return false;
                i += 4;
                ch = (uint32_t)h;
                if (ch >= 0xD800 && ch < 0xDC00 && i + 6 < vl && v[i + 1] == '\\' && v[i + 2] == 'u') {
                    int lo = hex4(v + i + 3);
                    if (lo >= 0xDC00 && lo < 0xE000) { ch = 0x10000 + ((ch - 0xD800) << 10) + (uint32_t)(lo - 0xDC00); i += 6; }
                }
                if (ch >= 0xD800 && ch < 0xE000) ch = 0xFFFD;   /* lone surrogate */
                if (ch == 0) return false;
                size_t k = put_utf8(out + o, outn - 1 - o, ch);
                if (!k) return false;
                o += k;
                continue;
            }
            default: return false;
            }
        }
        if (o + 1 >= outn) return false;
        out[o++] = (char)ch;
    }
    out[o] = 0;
    return true;
}

static bool get_bool(const char *js, size_t n, const char *key, bool *out)
{
    const char *v; size_t vl; bool str;
    if (!json_get(js, n, key, &v, &vl, &str) || str) return false;
    if (vl == 4 && memcmp(v, "true", 4) == 0)  { *out = true;  return true; }
    if (vl == 5 && memcmp(v, "false", 5) == 0) { *out = false; return true; }
    return false;
}

static bool get_int(const char *js, size_t n, const char *key, long *out)
{
    const char *v; size_t vl; bool str;
    if (!json_get(js, n, key, &v, &vl, &str) || str || vl == 0 || vl > 15) return false;
    char tmp[16];
    memcpy(tmp, v, vl);
    tmp[vl] = 0;
    char *e;
    double d = strtod(tmp, &e);
    if (*e || d != (double)(long)d) return false;
    *out = (long)d;
    return true;
}

/* ---- small JSON writer -------------------------------------------------- */

typedef struct { char *buf; size_t n, len; bool over; } out_t;

static void w(out_t *o, const char *fmt, ...)
{
    if (o->over) return;
    va_list ap;
    va_start(ap, fmt);
    int k = vsnprintf(o->buf + o->len, o->n - o->len, fmt, ap);
    va_end(ap);
    if (k < 0 || (size_t)k >= o->n - o->len) { o->over = true; return; }
    o->len += (size_t)k;
}

static void w_str(out_t *o, const char *s)
{
    w(o, "\"");
    for (; *s; s++) {
        unsigned char ch = (unsigned char)*s;
        if (ch == '"' || ch == '\\') w(o, "\\%c", ch);
        else if (ch < 0x20) w(o, "\\u%04x", ch);
        else w(o, "%c", ch);
    }
    w(o, "\"");
}

/* "HH:MM". An end time at midnight is "24:00", so it sorts after the start. */
static void fmt_hhmm(char out[6], time_t t, bool end)
{
    struct tm tm;
    localtime_r(&t, &tm);
    int h = (end && tm.tm_hour == 0 && tm.tm_min == 0) ? 24 : tm.tm_hour;
    snprintf(out, 6, "%02d:%02d", h % 100, tm.tm_min);
}

static void w_hhmm(out_t *o, const char *key, time_t t, bool end)
{
    char s[6];
    fmt_hhmm(s, t, end);
    w(o, ",\"%s\":\"%s\"", key, s);
}

static size_t done(out_t *o) { return o->over ? 0 : o->len; }

/* ---- outgoing: building state and found items --------------------------- */

size_t ix_msg_rooms(time_t now, bool stable, char *buf, size_t n)
{
    out_t o = { buf, n, 0, false };
    if (stable) w(&o, "{\"rooms\":[");
    else        w(&o, "{\"ts\":%lld,\"rooms\":[", (long long)now);
    for (int i = 0; i < ix_room_count(); i++) {
        const ix_room_info_t *ri = ix_room(i);
        ix_status_t st;
        ix_room_status(i, now, &st);
        w(&o, "%s{\"id\":", i ? "," : "");
        w_str(&o, ri->id);
        w(&o, ",\"state\":\"%s\"", ix_state_name(st.state));
        if (st.state == IX_GHOST) {
            if (stable) w(&o, ",\"release_at\":%lld", (long long)(now + st.release_in_s));
            else        w(&o, ",\"release_in_s\":%d", (int)st.release_in_s);
        }
        if (st.until && st.state != IX_FREE && st.state != IX_OFFLINE) w_hhmm(&o, "until", st.until, true);
        if (st.state == IX_FREE && st.free_until) w_hhmm(&o, "free_until", st.free_until, false);
        w(&o, ",\"seats\":%u,\"floor\":%u,\"slots\":[", (unsigned)ri->seats, (unsigned)ri->floor);
        const ix_booking_t *bk[IX_MAX_BOOKINGS];
        int nb = ix_room_bookings(i, now, bk, IX_MAX_BOOKINGS);
        for (int j = 0; j < nb; j++) {
            char a[6], b[6];
            fmt_hhmm(a, bk[j]->start, false);
            fmt_hhmm(b, bk[j]->end, true);
            w(&o, "%s[\"%s\",\"%s\"]", j ? "," : "", a, b);
        }
        w(&o, "]}");
    }
    w(&o, "]}");
    return done(&o);
}

size_t ix_msg_lostfound(time_t now, bool stable, char *buf, size_t n)
{
    out_t o = { buf, n, 0, false };
    if (stable) w(&o, "{\"items\":[");
    else        w(&o, "{\"ts\":%lld,\"items\":[", (long long)now);
    const ix_found_t *f[IX_MAX_FOUND];
    int nf = ix_found_search(-1, -1, f, IX_MAX_FOUND), k = 0;
    for (int i = 0; i < nf; i++) {
        if (now - f[i]->when > IX_FOUND_KEEP_S || !ix_room(f[i]->room)) continue;
        w(&o, "%s{\"room\":", k++ ? "," : "");
        w_str(&o, ix_room(f[i]->room)->id);
        w(&o, ",\"cat\":\"%s\",\"ts\":%lld,\"online\":%s}", ix_msg_cat_key(f[i]->cat),
          (long long)f[i]->when, f[i]->online ? "true" : "false");
    }
    w(&o, "]}");
    return done(&o);
}

/* ---- categories --------------------------------------------------------- */

static const char *const s_cat_keys[IX_CAT_COUNT] = {
    "phone", "keys", "wallet", "bag", "bottle",
    "clothing", "electronics", "umbrella", "glasses", "other",
};

const char *ix_msg_cat_key(int cat)
{
    return (cat >= 0 && cat < IX_CAT_COUNT) ? s_cat_keys[cat] : "other";
}

int ix_msg_cat_from_key(const char *key)
{
    for (int i = 0; i < IX_CAT_COUNT; i++)
        if (strcmp(key, s_cat_keys[i]) == 0) return i;
    return -1;
}

/* ---- incoming ----------------------------------------------------------- */

static const char *book_err_key(ix_book_err_t e)
{
    switch (e) {
    case IX_BOOK_ERR_NAME:    return "name";
    case IX_BOOK_ERR_EMAIL:   return "email";
    case IX_BOOK_ERR_TIME:    return "time";
    case IX_BOOK_ERR_SEATS:   return "seats";
    case IX_BOOK_ERR_OVERLAP: return "overlap";
    case IX_BOOK_ERR_BUSY:    return "busy";
    case IX_BOOK_ERR_LIMIT:   return "limit";
    default:                  return "full";
    }
}

/* "HH:MM" today (by now's local date), on the 15-minute grid. */
static bool parse_hhmm(const char *s, time_t now, time_t *out)
{
    if (strlen(s) != 5 || s[2] != ':') return false;
    for (int i = 0; i < 5; i++) if (i != 2 && (s[i] < '0' || s[i] > '9')) return false;
    int h = (s[0] - '0') * 10 + (s[1] - '0'), m = (s[3] - '0') * 10 + (s[4] - '0');
    if (h > 24 || m > 59 || (h == 24 && m) || m % (IX_SLOT_S / 60)) return false;
    struct tm tm;
    localtime_r(&now, &tm);
    tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = 0; tm.tm_isdst = -1;
    *out = mktime(&tm);
    return true;
}

/* req ends up in a topic name: only letters, digits, - and _ */
static bool valid_req(const char *r)
{
    if (!*r) return false;
    for (; *r; r++)
        if (!((*r >= 'a' && *r <= 'z') || (*r >= 'A' && *r <= 'Z') || (*r >= '0' && *r <= '9') || *r == '-' || *r == '_'))
            return false;
    return true;
}

/* Recent replies, so a request the broker delivers twice (QoS 1) gets the
 * same answer instead of being booked twice or answered "overlap". */
#define REPLY_CACHE 16
static struct { char req[33]; char reply[IX_MSG_REPLY_LEN]; } s_cache[REPLY_CACHE];
static int s_cache_next;

static const char *cache_find(const char *req)
{
    for (int i = 0; i < REPLY_CACHE; i++)
        if (s_cache[i].req[0] && strcmp(s_cache[i].req, req) == 0) return s_cache[i].reply;
    return NULL;
}

static void reply(ix_msg_result_t *out, const char *req, const char *err)
{
    out->reply = true;
    snprintf(out->reply_topic, sizeof out->reply_topic, IX_TOPIC_ROOT "replies/%s", req);
    if (err) snprintf(out->reply_payload, sizeof out->reply_payload, "{\"ok\":false,\"error\":\"%s\"}", err);
    else     snprintf(out->reply_payload, sizeof out->reply_payload, "{\"ok\":true}");
    snprintf(s_cache[s_cache_next].req, sizeof s_cache[0].req, "%s", req);
    snprintf(s_cache[s_cache_next].reply, sizeof s_cache[0].reply, "%s", out->reply_payload);
    s_cache_next = (s_cache_next + 1) % REPLY_CACHE;
}

static void on_slave(const char *room, const char *js, size_t n, time_t now, ix_msg_result_t *out)
{
    bool occ;
    char proom[16];
    if (!get_bool(js, n, "occupied", &occ)) return;
    if (get_str(js, n, "room", proom, sizeof proom) && strcmp(proom, room) != 0) return;
    int idx = ix_room_find(room);
    if (idx < 0) return;
    ix_status_t before, after;
    ix_room_status(idx, now, &before);
    ix_slave_update(room, occ, now);
    ix_room_status(idx, now, &after);
    if (before.state != after.state) {
        out->changed = true;
        snprintf(out->log, sizeof out->log, "slave %s: %s", room, ix_state_name(after.state));
    }
}

static void on_booking(const char *js, size_t n, time_t now, ix_msg_result_t *out)
{
    char req[33], room[16], from[8], to[8], name[128], email[IX_EMAIL_LEN];   /* long names are cut by ix_book() */
    if (!get_str(js, n, "req", req, sizeof req) || !valid_req(req)) return;   /* nowhere to answer */
    const char *cached = cache_find(req);
    if (cached) {
        out->reply = true;
        snprintf(out->reply_topic, sizeof out->reply_topic, IX_TOPIC_ROOT "replies/%s", req);
        snprintf(out->reply_payload, sizeof out->reply_payload, "%s", cached);
        return;
    }

    ix_book_err_t e;
    long people = 0;
    time_t start, end;
    if (!get_str(js, n, "name", name, sizeof name)) e = IX_BOOK_ERR_NAME;
    else if (!get_str(js, n, "email", email, sizeof email)) e = IX_BOOK_ERR_EMAIL;
    else if (!get_str(js, n, "room", room, sizeof room) ||
             !get_str(js, n, "from", from, sizeof from) || !get_str(js, n, "to", to, sizeof to) ||
             !parse_hhmm(from, now, &start) || !parse_hhmm(to, now, &end)) e = IX_BOOK_ERR_TIME;
    else if (!get_int(js, n, "people", &people) || people < 1) e = IX_BOOK_ERR_SEATS;
    else e = ix_book(ix_room_find(room), start, end, people > 255 ? 255 : (uint8_t)people, name, email, now);

    /* clear the personal data from the stack copy right away */
    memset(name, 0, sizeof name);
    memset(email, 0, sizeof email);

    reply(out, req, e == IX_BOOK_OK ? NULL : book_err_key(e));
    out->changed = e == IX_BOOK_OK;
    if (e == IX_BOOK_OK) snprintf(out->log, sizeof out->log, "web booking %s %s-%s, %ld people: ok", room, from, to, people);
    else                 snprintf(out->log, sizeof out->log, "web booking: %s", book_err_key(e));
}

static void on_found(const char *js, size_t n, time_t now, ix_msg_result_t *out)
{
    char req[33], room[16], cat[16];
    if (!get_str(js, n, "req", req, sizeof req) || !valid_req(req)) return;
    const char *cached = cache_find(req);
    if (cached) {
        out->reply = true;
        snprintf(out->reply_topic, sizeof out->reply_topic, IX_TOPIC_ROOT "replies/%s", req);
        snprintf(out->reply_payload, sizeof out->reply_payload, "%s", cached);
        return;
    }
    int r = get_str(js, n, "room", room, sizeof room) ? ix_room_find(room) : -1;
    int c = get_str(js, n, "cat", cat, sizeof cat) ? ix_msg_cat_from_key(cat) : -1;
    const char *err = NULL;
    if (r < 0) err = "room";
    else if (c < 0) err = "cat";
    else {
        int id = ix_found_add(r, (ix_category_t)c, now);
        if (id < 0) err = "full";
        else ix_found_set_online(id, true);
    }
    reply(out, req, err);
    out->changed = err == NULL;
    if (!err) snprintf(out->log, sizeof out->log, "web found item: %s in %s", cat, room);
    else      snprintf(out->log, sizeof out->log, "web found item: %s", err);
}

void ix_msg_handle(const char *topic, size_t tl, const char *payload, size_t pl,
                   time_t now, ix_msg_result_t *out)
{
    memset(out, 0, sizeof *out);
    const size_t rl = sizeof IX_TOPIC_ROOT - 1;
    if (tl <= rl || memcmp(topic, IX_TOPIC_ROOT, rl) != 0) return;
    char rest[IX_MSG_TOPIC_LEN];
    if (tl - rl >= sizeof rest) return;
    memcpy(rest, topic + rl, tl - rl);
    rest[tl - rl] = 0;

    if (strcmp(rest, "bookings/new") == 0) { on_booking(payload, pl, now, out); return; }
    if (strcmp(rest, "lostfound/new") == 0) { on_found(payload, pl, now, out); return; }

    /* <floor>/<room>/state */
    char *s1 = strchr(rest, '/');
    char *s2 = s1 ? strchr(s1 + 1, '/') : NULL;
    if (!s1 || !s2 || s1 == rest || s2 == s1 + 1 || strcmp(s2, "/state") != 0) return;
    *s2 = 0;
    on_slave(s1 + 1, payload, pl, now, out);
}
