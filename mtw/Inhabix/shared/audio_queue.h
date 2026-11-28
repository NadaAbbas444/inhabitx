#ifndef INHABITX_AUDIO_QUEUE_H
#define INHABITX_AUDIO_QUEUE_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define AUDIO_FRAME_SAMPLES 256u
#define AUDIO_QUEUE_BLOCKS 32u
/* Supply an architecture memory barrier before including this header. */
#ifndef AUDIO_MEMORY_BARRIER
#error Define AUDIO_MEMORY_BARRIER for this platform
#endif
typedef struct {
    int16_t pcm[AUDIO_FRAME_SAMPLES];
    uint32_t epoch, captured_ms;
} audio_block_t;
typedef struct {
    audio_block_t blocks[AUDIO_QUEUE_BLOCKS];
    volatile uint32_t write, read;
} audio_queue_t;

/* Single ISR producer, single foreground consumer. A slot stays owned by
 * the consumer until its PCM has been copied. Never overwrite unread PCM. */
static inline bool audio_queue_push(audio_queue_t *q, const audio_block_t *block)
{
    const uint32_t write = q->write;
    if ((uint32_t)(write - q->read) >= AUDIO_QUEUE_BLOCKS) return false;
    AUDIO_MEMORY_BARRIER();
    q->blocks[write % AUDIO_QUEUE_BLOCKS] = *block;
    AUDIO_MEMORY_BARRIER();
    q->write = write + 1u;
    return true;
}
static inline bool audio_queue_pop(audio_queue_t *q, audio_block_t *block)
{
    const uint32_t read = q->read;
    if (read == q->write) return false;
    AUDIO_MEMORY_BARRIER();
    *block = q->blocks[read % AUDIO_QUEUE_BLOCKS];
    AUDIO_MEMORY_BARRIER();
    q->read = read + 1u;
    return true;
}
#endif
