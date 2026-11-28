#pragma once
#include <stdint.h>

inline bool voiceRecoveryReady(uint32_t silentSamples, uint32_t sampleRate,
                               uint32_t elapsedMs, bool decoderTimedOut) {
    return decoderTimedOut && elapsedMs >= 6000 && silentSamples >= sampleRate * 6;
}

// Speech continuity depends on captured PCM, not on occupancy inference backlog.
class VoiceContinuity {
public:
    bool check(uint32_t sequence, uint32_t captureEpoch) {
        const bool continuous = !started || (sequence == expected && captureEpoch == epoch);
        started = true;
        expected = sequence + 1;
        epoch = captureEpoch;
        return continuous;
    }
private:
    bool started = false;
    uint32_t expected = 0, epoch = 0;
};
