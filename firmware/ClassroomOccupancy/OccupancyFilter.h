#pragma once
#include <stdint.h>
#include <math.h>

// Experimental application settings. These are not trained model parameters.
constexpr float ACTIVITY_THRESHOLD = 0.80f;
constexpr float BACKGROUND_THRESHOLD = 0.75f;
constexpr unsigned ACTIVITY_CONFIRMATIONS = 3;
constexpr uint32_t OCCUPANCY_HOLD_MS = 60000;
constexpr uint32_t BACKGROUND_CONFIRM_MS = 10000;
constexpr uint32_t STARTUP_BACKGROUND_MS = 20000;

enum class RoomState { Unknown, Occupied, Unoccupied };
inline const char* stateName(RoomState s) {
    return s == RoomState::Occupied ? "OCCUPIED" :
           s == RoomState::Unoccupied ? "UNOCCUPIED_INFERRED" : "UNKNOWN";
}

class OccupancyFilter {
public:
    RoomState state = RoomState::Unknown;
    void reset() {
        state = RoomState::Unknown;
        hits = 0; backgroundStarted = false; everActive = false;
        backgroundSince = 0; lastActivity = 0;
    }
    void update(const float* p, uint32_t audioMs) {
        if (!isfinite(p[0]) || !isfinite(p[1]) || !isfinite(p[2])) {
            reset(); return;
        }
        const bool activity = p[2] >= ACTIVITY_THRESHOLD && p[2] > p[1] && p[2] > p[0];
        const bool background = p[1] >= BACKGROUND_THRESHOLD && p[1] > p[2] && p[1] > p[0];
        if (activity) {
            lastActivity = audioMs;
            backgroundStarted = false;
            if (hits < ACTIVITY_CONFIRMATIONS) ++hits;
            if (hits >= ACTIVITY_CONFIRMATIONS) {
                everActive = true;
                state = RoomState::Occupied;
            }
            return;
        }
        hits = 0;
        if (!background) {
            backgroundStarted = false;
            // Ambiguous/unlabeled predictions cannot justify an empty-room claim.
            if (state != RoomState::Occupied) state = RoomState::Unknown;
            return;
        }
        if (!backgroundStarted) {
            backgroundSince = audioMs;
            backgroundStarted = true;
        }
        const uint32_t needed = everActive ? BACKGROUND_CONFIRM_MS : STARTUP_BACKGROUND_MS;
        if (audioMs - backgroundSince >= needed &&
            (!everActive || audioMs - lastActivity >= OCCUPANCY_HOLD_MS)) {
            state = RoomState::Unoccupied;
        }
    }
private:
    unsigned hits = 0;
    bool backgroundStarted = false, everActive = false;
    uint32_t backgroundSince = 0, lastActivity = 0;
};
