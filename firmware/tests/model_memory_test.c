#include "../ClassroomOccupancy/src/model/model.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#if defined(CLASSROOM_MODEL_HEAP_TEST)
int ClassroomModel_allocate(void);
#endif

int main(void) {
#if defined(CLASSROOM_MODEL_HEAP_TEST)
    if (!ClassroomModel_allocate() || !ClassroomModel_allocate()) return 2;
#endif
    uint32_t random = 123456789;
    unsigned predictions = 0;
    for (unsigned segment = 0; segment < 3; ++segment) {
        IMAI_init();
        for (unsigned sample = 0; sample < 48000; ++sample) {
            random = random * 1664525u + 1013904223u;
            float input = segment == 0 ? 0.0f :
                0.15f * sinf((float)sample * 0.071f) +
                (float)(random & 65535u) / 262144.0f - 0.125f;
            float scores[IMAI_DATA_OUT_COUNT];
            if (IMAI_enqueue(&input) != IMAI_RET_SUCCESS) return 3;
            int result = IMAI_dequeue(scores);
            if (result == IMAI_RET_SUCCESS) {
                for (unsigned c = 0; c < IMAI_DATA_OUT_COUNT; ++c) {
                    if (!isfinite(scores[c])) return 4;
                    printf("%a ", (double)scores[c]);
                }
                puts("");
                ++predictions;
            } else if (result != IMAI_RET_NODATA) return 5;
        }
    }
    if (predictions < 6) return 6;
    printf("predictions=%u\n", predictions);
    return 0;
}
