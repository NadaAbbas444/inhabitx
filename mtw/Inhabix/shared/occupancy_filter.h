#ifndef INHABITX_OCCUPANCY_FILTER_H
#define INHABITX_OCCUPANCY_FILTER_H
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/* Same experimental policy as the ESP32 OccupancyFilter.h. */
typedef enum { ROOM_UNKNOWN, ROOM_OCCUPIED, ROOM_UNOCCUPIED } room_state_t;
typedef struct {
    room_state_t state;
    unsigned hits;
    bool background_started, ever_active;
    uint32_t background_since, last_activity;
} occupancy_filter_t;

static inline void occupancy_reset(occupancy_filter_t *f)
{
    *f = (occupancy_filter_t){0};
}
static inline const char *occupancy_name(room_state_t state)
{
    return state == ROOM_OCCUPIED ? "OCCUPIED" :
           state == ROOM_UNOCCUPIED ? "UNOCCUPIED_INFERRED" : "UNKNOWN";
}
static inline void occupancy_update(occupancy_filter_t *f, const float p[3], uint32_t ms)
{
    if (!isfinite(p[0]) || !isfinite(p[1]) || !isfinite(p[2])) {
        occupancy_reset(f);
        return;
    }
    const bool activity = p[2] >= 0.80f && p[2] > p[1] && p[2] > p[0];
    const bool background = p[1] >= 0.75f && p[1] > p[2] && p[1] > p[0];
    if (activity) {
        f->last_activity = ms;
        f->background_started = false;
        if (f->hits < 3u) ++f->hits;
        if (f->hits >= 3u) {
            f->ever_active = true;
            f->state = ROOM_OCCUPIED;
        }
        return;
    }
    f->hits = 0u;
    if (!background) {
        f->background_started = false;
        if (f->state != ROOM_OCCUPIED) f->state = ROOM_UNKNOWN;
        return;
    }
    if (!f->background_started) {
        f->background_since = ms;
        f->background_started = true;
    }
    const uint32_t needed = f->ever_active ? 10000u : 20000u;
    if ((uint32_t)(ms - f->background_since) >= needed &&
        (!f->ever_active || (uint32_t)(ms - f->last_activity) >= 60000u)) {
        f->state = ROOM_UNOCCUPIED;
    }
}
#endif
