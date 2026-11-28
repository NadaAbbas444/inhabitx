#ifndef TEST_AUDIO_H
#define TEST_AUDIO_H
#include "cybsp.h"
#define IMAI_DATA_IN_COUNT 1
#define IMAI_DATA_OUT_COUNT 3
#define IMAI_RET_SUCCESS 0
#define IMAI_RET_NODATA -1
#define PDM_PCM_DATA_NOT_READY -1
static unsigned fake_model_samples;
static float fake_scores[3] = {0.01f,0.98f,0.01f};
static void IMAI_init(void) {fake_model_samples=0;}
static int IMAI_enqueue(float *sample) {(void)sample;++fake_model_samples;return 0;}
static int IMAI_dequeue(float *scores) {
    if(fake_model_samples % 8000u != 0u) return IMAI_RET_NODATA;
    for(unsigned i=0;i<3;++i)scores[i]=fake_scores[i];
    return 0;
}
#endif
