#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../Inhabix/shared/occupancy_filter.h"
#define AUDIO_MEMORY_BARRIER() ((void)0)
#include "../Inhabix/shared/audio_queue.h"

static void policy_test(void)
{
    occupancy_filter_t f;
    const float activity[3] = {0.01f, 0.01f, 0.98f};
    const float background[3] = {0.01f, 0.98f, 0.01f};
    const float ambiguous[3] = {0.5f, 0.3f, 0.2f};
    const float invalid[3] = {NAN, 0.0f, 1.0f};
    occupancy_reset(&f);
    assert(f.state == ROOM_UNKNOWN);
    occupancy_update(&f, background, 100u);
    occupancy_update(&f, background, 20099u);
    assert(f.state == ROOM_UNKNOWN);
    occupancy_update(&f, background, 20100u);
    assert(f.state == ROOM_UNOCCUPIED);
    occupancy_reset(&f);
    occupancy_update(&f, activity, 100u);
    occupancy_update(&f, activity, 200u);
    assert(f.state == ROOM_UNKNOWN);
    occupancy_update(&f, activity, 300u);
    assert(f.state == ROOM_OCCUPIED);
    occupancy_update(&f, ambiguous, 20000u);
    assert(f.state == ROOM_OCCUPIED);
    occupancy_update(&f, background, 20100u);
    occupancy_update(&f, background, 60299u);
    assert(f.state == ROOM_OCCUPIED);
    occupancy_update(&f, background, 60300u);
    assert(f.state == ROOM_UNOCCUPIED);
    occupancy_update(&f, ambiguous, 60301u);
    assert(f.state == ROOM_UNKNOWN);
    occupancy_update(&f, invalid, 70000u);
    assert(f.state == ROOM_UNKNOWN && !f.ever_active);
    /* Unsigned elapsed-time arithmetic must survive the millisecond rollover. */
    occupancy_update(&f, background, UINT32_MAX - 10000u);
    occupancy_update(&f, background, 9998u);
    assert(f.state == ROOM_UNKNOWN);
    occupancy_update(&f, background, 9999u);
    assert(f.state == ROOM_UNOCCUPIED);
    /* A new gap/reset always requires a fresh startup observation period. */
    occupancy_reset(&f);
    occupancy_update(&f, background, 1000u);
    assert(f.state == ROOM_UNKNOWN);
}

static void queue_test(void)
{
    static audio_queue_t q;
    audio_block_t input = {0}, output;
    assert(!audio_queue_pop(&q, &output));
    for (unsigned n = 0; n < AUDIO_QUEUE_BLOCKS; ++n) {
        input.epoch = 5u;
        input.captured_ms = n;
        for (unsigned i = 0; i < AUDIO_FRAME_SAMPLES; ++i) input.pcm[i] = (int16_t)(n + i);
        assert(audio_queue_push(&q, &input));
    }
    /* A failed push must preserve all unread samples, including the oldest. */
    memset(input.pcm, 0x7f, sizeof(input.pcm));
    input.epoch = 6u;
    assert(!audio_queue_push(&q, &input));
    for (unsigned n = 0; n < AUDIO_QUEUE_BLOCKS; ++n) {
        assert(audio_queue_pop(&q, &output));
        assert(output.epoch == 5u && output.captured_ms == n);
        for (unsigned i = 0; i < AUDIO_FRAME_SAMPLES; ++i)
            assert(output.pcm[i] == (int16_t)(n + i));
    }
    assert(!audio_queue_pop(&q, &output));
    for (unsigned n = 0; n < 1000u; ++n) {
        input.captured_ms = n;
        assert(audio_queue_push(&q, &input));
        assert(audio_queue_pop(&q, &output));
        assert(output.epoch == 6u && output.captured_ms == n);
    }
    q.write = q.read = UINT32_MAX - 5u;
    for (unsigned n = 0; n < 50u; ++n) {
        input.captured_ms = n;
        assert(audio_queue_push(&q, &input));
        assert(audio_queue_pop(&q, &output));
        assert(output.captured_ms == n);
    }
}

int main(void)
{
    policy_test();
    queue_test();
    puts("PASS: occupancy boundaries, ambiguity, invalid scores, resets, queue ownership and rollover");
    return 0;
}
