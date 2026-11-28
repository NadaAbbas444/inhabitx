#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../Inhabix/proj_cm55/model/model.h"

static unsigned run(float first[3])
{
    unsigned outputs = 0;
    IMAI_init();
    for (unsigned n = 0; n < 64000u; ++n) {
        float input = n < 32000u ? 0.0f : 0.02f * sinf(n * 0.3926990817f);
        float scores[3];
        assert(IMAI_enqueue(&input) == IMAI_RET_SUCCESS);
        const int status = IMAI_dequeue(scores);
        assert(status == IMAI_RET_SUCCESS || status == IMAI_RET_NODATA);
        if (status == IMAI_RET_SUCCESS) {
            float sum = 0.0f;
            for (unsigned k = 0; k < 3; ++k) {
                assert(isfinite(scores[k]) && scores[k] >= 0.0f && scores[k] <= 1.0f);
                sum += scores[k];
                if (outputs == 0) first[k] = scores[k];
            }
            assert(fabsf(sum - 1.0f) < 0.001f);
            ++outputs;
        }
    }
    assert(outputs > 0u);
    return outputs;
}
int main(void)
{
    float first[3], again[3];
    const unsigned count = run(first);
    assert(run(again) == count);
    for (unsigned k = 0; k < 3; ++k) assert(first[k] == again[k]);
    printf("PASS: generated model streaming, finite probabilities, normalized outputs, reset determinism (%u outputs/run)\n", count);
    return 0;
}
