#ifndef TEST_CYBSP_H
#define TEST_CYBSP_H
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t cy_rslt_t;
typedef int IRQn_Type;
typedef struct { IRQn_Type intrSrc; unsigned intrPriority; } cy_stc_sysint_t;
enum { CY_RSLT_SUCCESS=0, CY_PDM_PCM_SUCCESS=0, CY_SYSINT_SUCCESS=0,
       CY_PDM_PCM_BAD_PARAM=1, CY_PDM_PCM_WSIZE_16_BIT=16,
       CY_PDM_PCM_CHAN_CIC_DECIM_32=32, CY_PDM_PCM_CHAN_FIR0_DECIM_1=1,
       CY_PDM_PCM_CHAN_FIR1_DECIM_3=3, CY_PDM_PCM_SEL_SRSS_CLOCK=1,
       CY_PDM_PCM_RATE_FULL=0, CY_PDM_PCM_SEL_GAIN_5DB=5 };
enum { CY_PDM_PCM_INTR_RX_TRIGGER=1, CY_PDM_PCM_INTR_RX_FIR_OVERFLOW=2,
       CY_PDM_PCM_INTR_RX_OVERFLOW=4, CY_PDM_PCM_INTR_RX_IF_OVERFLOW=8,
       CY_PDM_PCM_INTR_RX_UNDERFLOW=16, CY_PDM_PCM_INTR_MASK=31 };
typedef struct {
    unsigned wordSize, rxFifoTriggerLevel, cic_decim_code, fir0_decim_code, fir1_decim_code;
    bool signExtension, fir0_enable;
} test_channel_config_t;
typedef struct { unsigned clkDiv, clksel, halverate; } test_pdm_config_t;
static const test_channel_config_t channel_3_config = {16,31,32,1,3,true,false};
static const test_pdm_config_t CYBSP_PDM_config = {7,1,0};
#define CYBSP_PDM_HW 0
#define CYBSP_PDM_CHANNEL_3_IRQ 3
#define SystemCoreClock 400000000u
#define __DMB() ((void)0)
static unsigned fake_status, fake_fifo_count, fake_index;
static int16_t fake_level;
static bool fake_alternate = true;
static unsigned Cy_PDM_PCM_Channel_GetInterruptStatusMasked(int a, unsigned b) { (void)a;(void)b;return fake_status; }
static unsigned Cy_PDM_PCM_Channel_GetNumInFifo(int a, unsigned b) { (void)a;(void)b;return fake_fifo_count; }
static uint32_t Cy_PDM_PCM_Channel_ReadFifo(int a, unsigned b) {
    (void)a;(void)b; --fake_fifo_count;
    return (uint32_t)(int32_t)(fake_alternate && (++fake_index & 1u) ? -fake_level : fake_level);
}
static void Cy_PDM_PCM_Channel_ClearInterrupt(int a,unsigned b,unsigned c) {(void)a;(void)b;(void)c;fake_status=0;}
static void Cy_PDM_PCM_Channel_SetInterruptMask(int a,unsigned b,unsigned c) {(void)a;(void)b;(void)c;}
static unsigned Cy_PDM_PCM_Init(int a,const test_pdm_config_t *b) {(void)a;(void)b;return 0;}
static unsigned Cy_PDM_PCM_Channel_Init(int a,const test_channel_config_t *b,unsigned c) {(void)a;(void)b;(void)c;return 0;}
static void Cy_PDM_PCM_Channel_Enable(int a,unsigned b) {(void)a;(void)b;}
static void Cy_PDM_PCM_SetGain(int a,unsigned b,unsigned c) {(void)a;(void)b;(void)c;}
static void Cy_PDM_PCM_Activate_Channel(int a,unsigned b) {(void)a;(void)b;}
static unsigned Cy_SysInt_Init(const cy_stc_sysint_t *a,void (*b)(void)) {(void)a;(void)b;return 0;}
static unsigned SysTick_Config(unsigned a) {(void)a;return 0;}
static void NVIC_ClearPendingIRQ(IRQn_Type a) {(void)a;}
static void NVIC_EnableIRQ(IRQn_Type a) {(void)a;}
#endif
