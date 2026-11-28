#include <assert.h>
#include "../Inhabix/shared/occupancy_audio.h"

static void capture(bool process)
{
    wall_ms += 16u;
    for(unsigned i=0;i<8u;++i) {
        fake_status=CY_PDM_PCM_INTR_RX_TRIGGER;
        fake_fifo_count=32u;
        occupancy_pdm_isr();
    }
    if(process) (void)pdm_data_process();
}
static void start(bool activity)
{
    assert(pdm_init()==CY_RSLT_SUCCESS);
    fake_alternate=true;
    fake_level=100;
    fake_scores[0]=0.01f;
    fake_scores[1]=activity?0.01f:0.98f;
    fake_scores[2]=activity?0.98f:0.01f;
}
int main(void)
{
    start(true);
    for(unsigned i=0;i<100u;++i)capture(true);
    assert(room_filter.state==ROOM_OCCUPIED);
    for(unsigned i=0;i<40u;++i)capture(false);
    assert(dropped_blocks>0u && stream_epoch>0u);
    (void)pdm_data_process();
    assert(room_filter.state==ROOM_UNKNOWN && !have_scores);
    for(unsigned i=0;i<AUDIO_QUEUE_BLOCKS;++i)(void)pdm_data_process();
    for(unsigned i=0;i<100u;++i)capture(true);
    assert(room_filter.state==ROOM_OCCUPIED);
    fake_status=CY_PDM_PCM_INTR_RX_OVERFLOW;
    fake_fifo_count=0u;
    occupancy_pdm_isr();
    (void)pdm_data_process();
    assert(room_filter.state==ROOM_UNKNOWN && fifo_errors==1u);
    for(unsigned i=0;i<100u;++i)capture(true);
    assert(room_filter.state==ROOM_OCCUPIED);
    wall_ms+=1001u;
    (void)pdm_data_process();
    assert(room_filter.state==ROOM_UNKNOWN && !have_scores);
    start(true);
    for(unsigned i=0;i<100u;++i)capture(true);
    fake_level=32767;
    capture(true);
    assert(room_filter.state==ROOM_UNKNOWN && !have_scores);
    start(false);
    fake_alternate=false;
    fake_level=0;
    for(unsigned i=0;i<2000u;++i)capture(true);
    assert(stuck_latched && room_filter.state==ROOM_UNKNOWN && !have_scores);
    fake_alternate=true;
    fake_level=100;
    for(unsigned i=0;i<1350u;++i)capture(true);
    assert(!stuck_latched && room_filter.state==ROOM_UNOCCUPIED);
    fake_scores[0]=NAN;
    for(unsigned i=0;i<40u;++i)capture(true);
    assert(room_filter.state==ROOM_UNKNOWN && !have_scores);
    puts("PASS: pipeline queue drops, FIFO overflow, stall, clipping, persistent stuck input, recovery and invalid model scores");
    return 0;
}
