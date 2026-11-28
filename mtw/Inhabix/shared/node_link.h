/* node_link.h - the room state from the audio core (CM55) to the Wi-Fi core (CM33).
 *
 * A 32-byte mailbox at a fixed address in memory both cores can reach: the
 * last 64 bytes of the CM55's allocatable shared region (the linker fills
 * that region from its start, and this app puts nothing there). Once a
 * second the CM55 writes the filter's state; the CM33 reads it and sends it
 * to the broker (board/node_sensor.c). No interrupts, no IPC driver, so the
 * bare-metal audio loop stays as it is.
 *
 * Used by mtw/Inhabix/shared/occupancy_audio.h when INHABITX_NODE_LINK is
 * defined (proj_cm55/Makefile), and by the node's Wi-Fi core. */
#ifndef INHABITX_NODE_LINK_H
#define INHABITX_NODE_LINK_H

#include <stdbool.h>
#include <stdint.h>

#define NODE_LINK_MAGIC     0x314B4C4Eu     /* "NLK1" */

enum { NODE_LINK_UNKNOWN = 0, NODE_LINK_OCCUPIED = 1, NODE_LINK_EMPTY = 2 };

typedef struct {
    uint32_t magic;
    uint32_t seq;               /* +1 on every write; the reader sees it move */
    uint32_t state;             /* NODE_LINK_*                                 */
    uint32_t valid;             /* 1 = fresh, gap-free result (else: don't send) */
    int32_t  activity_pct;      /* human-activity score, 0..100                */
    int32_t  background_pct;
    int32_t  mic_dbfs;
    uint32_t reserved;
} node_link_t;                  /* 32 bytes = one cache line                   */

#ifndef NODE_LINK_ADDR          /* host tests point it somewhere else */
#include "cybsp.h"
#define NODE_LINK_ADDR      (CYMEM_m55_allocatable_shared_START + CYMEM_m55_allocatable_shared_SIZE - 64u)
#endif
#define NODE_LINK           ((volatile node_link_t *)(uintptr_t)(NODE_LINK_ADDR))

#ifndef NODE_LINK_BARRIER
#define NODE_LINK_BARRIER() __DMB()
#endif

/* audio core: publish the latest result */
static inline void node_link_write(uint32_t state, bool valid, float activity, float background, float dbfs)
{
    volatile node_link_t *l = NODE_LINK;
    l->state = state;
    l->valid = valid ? 1u : 0u;
    l->activity_pct = (int32_t)(activity * 100.0f);
    l->background_pct = (int32_t)(background * 100.0f);
    l->mic_dbfs = (int32_t)dbfs;
    NODE_LINK_BARRIER();
    l->seq = l->seq + 1u;
    l->magic = NODE_LINK_MAGIC;
    NODE_LINK_BARRIER();
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    SCB_CleanDCache_by_Addr((volatile void *)l, (int32_t)sizeof(node_link_t));    /* to memory, for the CM33 */
#endif
}

/* Wi-Fi core: copy the mailbox; false if it was never written or changed
 * while being read (then just try again next time). */
static inline bool node_link_read(node_link_t *out)
{
    volatile node_link_t *l = NODE_LINK;
    uint32_t seq = l->seq;
    NODE_LINK_BARRIER();
    out->magic = l->magic;
    out->state = l->state;
    out->valid = l->valid;
    out->activity_pct = l->activity_pct;
    out->background_pct = l->background_pct;
    out->mic_dbfs = l->mic_dbfs;
    NODE_LINK_BARRIER();
    out->seq = l->seq;
    return out->magic == NODE_LINK_MAGIC && out->seq == seq;
}

#endif /* INHABITX_NODE_LINK_H */
