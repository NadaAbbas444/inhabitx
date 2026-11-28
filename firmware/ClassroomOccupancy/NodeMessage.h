#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "OccupancyFilter.h"

struct NodeSnapshot {
    RoomState state;
    uint32_t capturedMs;
    uint32_t epoch;
    bool userConfirmed = false;
    uint32_t confirmationId = 0;
};

inline bool nodeSnapshotValid(const NodeSnapshot& snapshot, uint32_t now,
                              uint32_t currentEpoch, uint32_t maxAgeMs) {
    return snapshot.state != RoomState::Unknown && snapshot.epoch == currentEpoch &&
           uint32_t(now - snapshot.capturedMs) <= maxAgeMs;
}

// ts=0 is explicitly accepted by InhabitX; its main uses receipt time.
inline size_t nodeMessage(const char* room, RoomState state, char* buffer, size_t size,
                          bool userConfirmed = false) {
    if (state == RoomState::Unknown) return 0;
    const char* confirmation = state == RoomState::Occupied ?
        ",\"source\":\"voice\",\"message\":\"Room occupied confirmed by user\"" :
        ",\"source\":\"voice\",\"message\":\"Room marked empty by user\"";
    const int length = snprintf(buffer, size, "{\"room\":\"%s\",\"occupied\":%s,\"ts\":0%s}",
                                room, state == RoomState::Occupied ? "true" : "false",
                                userConfirmed ? confirmation : "");
    return length > 0 && size_t(length) < size ? size_t(length) : 0;
}
