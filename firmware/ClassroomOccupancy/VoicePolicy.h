#pragma once
#include <stdint.h>
#include "OccupancyFilter.h"

enum class VoiceCommand : uint8_t { Wake = 1, Arrive = 2, Leave = 3 };
constexpr uint32_t VOICE_COMMAND_WINDOW_MS = 8000;
constexpr uint32_t VOICE_ARRIVAL_HOLD_MS = 120000;
constexpr uint32_t VOICE_LEAVING_HOLD_MS = 10 * 60 * 1000;

// Owned by loop(); the recognizer only queues events. All arithmetic handles millis wrap.
class VoicePolicy {
public:
    void cancelListening() { armed = false; }
    bool listening(uint32_t now) const {
        return armed && uint32_t(now - wakeMs) <= VOICE_COMMAND_WINDOW_MS;
    }
    bool command(VoiceCommand cmd, uint32_t now) {
        if (cmd == VoiceCommand::Wake) { armed = true; wakeMs = now; return false; }
        if (!listening(now)) { armed = false; return false; }
        armed = false; // one command per wake phrase
        if (cmd != VoiceCommand::Arrive && cmd != VoiceCommand::Leave) return false;
        manualState = cmd == VoiceCommand::Arrive ? RoomState::Occupied : RoomState::Unoccupied;
        manualMs = now;
        durationMs = cmd == VoiceCommand::Arrive ? VOICE_ARRIVAL_HOLD_MS : VOICE_LEAVING_HOLD_MS;
        hasManual = true;
        ++revision;
        return true;
    }
    bool active(uint32_t now) const {
        return hasManual && uint32_t(now - manualMs) < durationMs;
    }
    RoomState state(RoomState automatic, uint32_t now) const {
        return active(now) ? manualState : automatic;
    }
    uint32_t eventId() const { return revision; }
private:
    bool armed = false, hasManual = false;
    uint32_t wakeMs = 0, manualMs = 0, durationMs = 0, revision = 0;
    RoomState manualState = RoomState::Unknown;
};
