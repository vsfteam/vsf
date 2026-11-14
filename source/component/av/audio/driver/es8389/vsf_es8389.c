/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *****************************************************************************/

//! \note ES8389 stereo audio codec control plane(i2c slave 0x20 default).
//! Register facts + sequences learned from esp_codec_dev v1.6.2
//! (Apache-2.0, device/es8389/es8389.c) and re-expressed in vsf terms -
//! no SDK text copied. The codec runs as i2s SLAVE with its clock
//! recovered from BCLK(no MCLK pad), which is how the vendor reference
//! boards wire it. The device stacks on vk_audio_i2s_dev_t: the audio
//! data plane is entirely the generic i2s adapter's, this file only adds
//! the codec register plane.

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_ES8389 == ENABLED

// class-context flags BEFORE the first vsf_av.h parse(the aic1000a order):
// the audio class bodies carry the op tables in protected sections
#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AUDIO_I2S_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_ES8389_CLASS_IMPLEMENT

#include "service/vsf_service.h"
#include "./vsf_es8389.h"
#include "component/av/vsf_av.h"
#include "../i2s/vsf_audio_i2s.h"

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error ES8389 needs EDA timer support for asynchronous I2C timeouts
#endif
#if VSF_SYNC_CFG_SUPPORT_ISR != ENABLED
#   error ES8389 needs ISR-safe EDA synchronization
#endif

/*============================ MACROS ========================================*/

// ---- register map(esp_codec_dev es8389_reg.h fact sheet) ----
// reset / clock manager / adc / dac clusters, 8-bit data
#define ES8389_REG_RESET                        0x00    // vendor open-entry value 0x7E, start 0x01, full-off 0x00.
                                                        // 0x7E is NOT a register-map reset on this silicon:
                                                        // registers retain their values across it (board-proven
                                                        // 2026-11-14, codec register read-back)
#define ES8389_REG_MISC                         0x01    // bit0 = i2s ms mode(0 = slave)
#define ES8389_REG_CLK_STATE                    0x02    // bit6 = 1: clock from BCLK, bit7: from MCLK
#define ES8389_REG_CLK_SYS_DIV                  0x03    // 0xC1 = clock on
#define ES8389_REG_CLK_PLL1                     0x04
#define ES8389_REG_CLK_PLL2                     0x05
#define ES8389_REG_CLK_PLL3                     0x06
#define ES8389_REG_CLK_PLL4                     0x07
#define ES8389_REG_CLK_PLL5                     0x08
#define ES8389_REG_CLK_PLL6                     0x09
#define ES8389_REG_CLK_DIV_L                    0x0A
#define ES8389_REG_CLK_DIV_M                    0x0B
#define ES8389_REG_FMT_DACADC                   0x0C    // [7:5] fmt: 0 = I2S
#define ES8389_REG_CLK_ADC                      0x0F
#define ES8389_REG_CLK_DAC                      0x10
#define ES8389_REG_CLK_ADCDIV1                  0x11
#define ES8389_REG_CLK_DACDIV1                  0x12
#define ES8389_REG_CLK_DACDIV2                  0x13
#define ES8389_REG_CLK_ADCDIV3                  0x14
#define ES8389_REG_CLK_ADCDIV4                  0x15
#define ES8389_REG_ADC_MCLK                     0x16
#define ES8389_REG_ADC_DIV_L                    0x17
#define ES8389_REG_ADC_DIV_M                    0x18
#define ES8389_REG_ADC_DIV_H                    0x19
#define ES8389_REG_ADC_DACLRC                   0x1A
#define ES8389_REG_DAC_DACLRC                   0x1B
#define ES8389_REG_ADC_SP                       0x20    // [4:2] fmt, [6:5] data len, [1:0] adc mute
#define ES8389_REG_ADC_ALC1                     0x21
#define ES8389_REG_ADC_ALC2                     0x22
#define ES8389_REG_ADC_ALC3                     0x23    // bit7 = internal ref signal
#define ES8389_REG_ADC_NG                       0x24
#define ES8389_REG_ADC_DRC                      0x25
#define ES8389_REG_ADC_DRC_VOL1                 0x26
#define ES8389_REG_ADC_DRC_VOL2                 0x27
#define ES8389_REG_ADC_DRC_VOL3                 0x28
#define ES8389_REG_ADC_ALC4                     0x2A
#define ES8389_REG_DAC_CTRL                     0x30
#define ES8389_REG_DAC_SP                       0x40    // [4:2] fmt, [6:5] data len, [1:0] dacref
#define ES8389_REG_DAC_FILTER                   0x41
#define ES8389_REG_DAC_VOL_MIX                  0x42
#define ES8389_REG_DAC_VOL_L                    0x46
#define ES8389_REG_DAC_VOL_R                    0x47
#define ES8389_REG_DAC_VOL_MIXC                 0x48
#define ES8389_REG_DAC_CLKFREE                  0x4D
#define ES8389_REG_VMID                         0x60
#define ES8389_REG_ANA1                         0x61
#define ES8389_REG_ANA2                         0x62
#define ES8389_REG_ANA3                         0x63
#define ES8389_REG_ANA4                         0x64
#define ES8389_REG_ANA5                         0x69
#define ES8389_REG_PGA_L                        0x72    // [3:0] gain, [6:4] input mux
#define ES8389_REG_PGA_R                        0x73
#define ES8389_REG_CHIP_MISC                    0xF0
#define ES8389_REG_CSM_STATE                    0xF1
#define ES8389_REG_PULL_DOWN                    0xF2
#define ES8389_REG_ISOLATION                    0xF3

// data length / fmt field values(I2S format, 16-bit little endian)
#define ES8389_FMT_I2S                          (0 << 2)
#define ES8389_DATALEN_S16                      (3 << 5)
#define ES8389_DATALEN_MSK                      (7 << 5)
#define ES8389_FMT_MSK                          (7 << 2)

// volume register: 0x00 = -95.5 dB .. 0xFF = +32 dB(linear in 0.5 dB steps)
#define ES8389_VOL_HALFDB_BIAS                  191     // 0 dB register value

// per-transfer i2c wait ceiling, in kernel ticks. One second accommodates
// delayed completion interrupts under load
#define __VK_ES8389_I2C_TIMEOUT_TICK            vsf_systimer_ms_to_tick(1000)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_es8389_coeff_t {
    uint32_t fs;
    // clock-table row key per the vendor formula: mclk_ratio = mclk / fs
    // (= bits * (no_dac_ref ? 2 : 4); the "Freq MCLK" column of the vendor
    // table equals fs * ratio). Only the 64 family is tabled - see set_fs
    uint16_t mclk_ratio;
    // the register cluster the vendor clock table writes per rate
    uint8_t pll[6];                     // 0x04..0x09
    uint8_t div_l;                      // 0x0A
    uint8_t clk_adc;                    // 0x0F(bits[7:6])
    uint8_t adcalc1;                    // 0x11
    uint8_t adc_alc1, adc_alc2, adc_drc1;   // 0x21/0x22/0x26
    uint8_t dac_ctrl;                   // 0x30(bits[7:6])
    uint8_t dac_filter, vol_mix;        // 0x41/0x42
    uint8_t clkfree;                    // 0x43(bit7/bit0)
    uint8_t chip_misc;                  // 0xF0(bits[6:4]/[1:0])
    uint8_t csm;                        // 0xF1(bit7)
    uint8_t adc_mclk, adc_divm, adc_divh;   // 0x16/0x18/0x19
} vk_es8389_coeff_t;

enum {
    __VK_ES8389_DIR_PLAYBACK,
    __VK_ES8389_DIR_CAPTURE,
    __VK_ES8389_PLAYBACK_MASK       = 1 << __VK_ES8389_DIR_PLAYBACK,
    __VK_ES8389_CAPTURE_MASK        = 1 << __VK_ES8389_DIR_CAPTURE,
};

enum {
    __VK_ES8389_WAIT_CMD,
    __VK_ES8389_WAIT_I2C,
    __VK_ES8389_WAIT_RETRY,
    __VK_ES8389_WAIT_RECOVER,
};

enum {
    __VK_ES8389_WORK_NONE,
    __VK_ES8389_WORK_OPEN,
    __VK_ES8389_WORK_OPEN_TAIL,
    __VK_ES8389_WORK_SET_FS,
    __VK_ES8389_WORK_ENABLE_PLAYBACK,
    __VK_ES8389_WORK_ENABLE_BIAS,
    __VK_ES8389_WORK_DEFAULT_VOLUME,
    __VK_ES8389_WORK_ENABLE_CAPTURE,
    __VK_ES8389_WORK_CONTROL,
    __VK_ES8389_WORK_QUIESCE,
};

enum {
    __VK_ES8389_XFER_WRITE,
    __VK_ES8389_XFER_READ_ADDR,
    __VK_ES8389_XFER_READ_DATA,
};

/*============================ GLOBAL VARIABLES ==============================*/

dcl_vsf_peda_methods(static, __vk_es8389_init)
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
dcl_vsf_peda_methods(static, __vk_es8389_playback_control)
dcl_vsf_peda_methods(static, __vk_es8389_playback_start)
dcl_vsf_peda_methods(static, __vk_es8389_playback_stop)
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
dcl_vsf_peda_methods(static, __vk_es8389_capture_control)
dcl_vsf_peda_methods(static, __vk_es8389_capture_start)
dcl_vsf_peda_methods(static, __vk_es8389_capture_stop)
#endif

const vk_audio_drv_t vk_es8389_drv = {
    .init       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_init),
};

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static const vk_audio_stream_drv_t __vk_es8389_stream_drv_playback = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_playback_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_playback_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_playback_stop),
};
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
static const vk_audio_stream_drv_t __vk_es8389_stream_drv_capture = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_capture_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_capture_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_es8389_capture_stop),
};
#endif

/*============================ LOCAL VARIABLES ===============================*/

// open script(the vendor es8389 open sequence: soft reset, bias defaults,
// slave mode, clock-from-BCLK, I2S 16-bit format, ~0 dB default volumes)
static const vk_es8389_reg_op_t __vk_es8389_open_seq[] = {
    { ES8389_REG_ISOLATION,     0x00 },
    { ES8389_REG_RESET,         0x7E },
    { ES8389_REG_ISOLATION,     0x38 },
    { ES8389_REG_ADC_NG,        0x64 },
    { ES8389_REG_ADC_DRC,       0x04 },
    { ES8389_REG_DAC_CLKFREE,   0x03 },
    { ES8389_REG_VMID,          0x2A },
    { ES8389_REG_ANA1,          0xC9 },
    { ES8389_REG_ANA2,          0x4F },
    { ES8389_REG_ANA3,          0x06 },
    { 0x6B,                     0x00 },
    { 0x6D,                     0x16 },
    { 0x6E,                     0xAA },
    { 0x6F,                     0x66 },
    { 0x70,                     0x99 },
    { ES8389_REG_ADC_ALC3,      0x00 },
    // pga input muxes: differential mic1 on L, mic2 on R, 0 dB steps
    { ES8389_REG_PGA_L,         1 << 4 },
    { ES8389_REG_PGA_R,         1 << 4 },
    { ES8389_REG_CLK_DAC,       0xC4 },
    { ES8389_REG_MISC,          0x08 },
    { ES8389_REG_CSM_STATE,     0x00 },
    { ES8389_REG_CLK_DACDIV1,   0x01 },
    { ES8389_REG_CLK_DACDIV2,   0x01 },
    { ES8389_REG_CLK_ADCDIV3,   0x01 },
    { ES8389_REG_CLK_ADCDIV4,   0x01 },
    { ES8389_REG_ADC_MCLK,      0x35 },
    { ES8389_REG_ADC_DIV_L,     0x09 },
    { ES8389_REG_ADC_DIV_M,     0x91 },
    { ES8389_REG_ADC_DIV_H,     0x28 },
    { ES8389_REG_ADC_DACLRC,    0x01 },
    { ES8389_REG_DAC_DACLRC,    0x01 },
    { 0x1C,                     0x11 },
    { ES8389_REG_ADC_ALC4,      0x00 },
    { ES8389_REG_ADC_SP,        ES8389_DATALEN_S16 | ES8389_FMT_I2S },
    { ES8389_REG_DAC_SP,        ES8389_DATALEN_S16 | ES8389_FMT_I2S },
    { ES8389_REG_CHIP_MISC,     0x01 },
    { ES8389_REG_CLK_STATE,     0x00 },
    { ES8389_REG_CLK_PLL1,      0x00 },
    { ES8389_REG_CLK_PLL2,      0x10 },
    { ES8389_REG_CLK_PLL3,      0x00 },
    { ES8389_REG_CLK_PLL4,      0xC0 },
    { ES8389_REG_CLK_PLL5,      0x00 },
    { ES8389_REG_CLK_PLL6,      0xC0 },
    { ES8389_REG_CLK_DIV_L,     0x80 },
    { ES8389_REG_CLK_DIV_M,     0x04 },
    { ES8389_REG_FMT_DACADC,    0x01 },
    { ES8389_REG_CLK_ADCDIV1,   0x00 },
    { ES8389_REG_ADC_ALC1,      0x1F },
    { ES8389_REG_ADC_ALC2,      0x7F },
    { 0x2F,                     0xC0 },
    { ES8389_REG_DAC_CTRL,      0xF4 },
    { 0x31,                     0x00 },
    { 0x44,                     0x00 },
    { ES8389_REG_DAC_FILTER,    0x7F },
    { ES8389_REG_DAC_VOL_MIX,   0x7F },
    { ES8389_REG_DAC_CLKFREE,   0x10 },
    { 0x49,                     0x0F },
    { 0x4C,                     0xC0 },
    { ES8389_REG_RESET,         0x00 },
    { ES8389_REG_CLK_SYS_DIV,   0xC1 },
    { ES8389_REG_RESET,         0x01 },
    { ES8389_REG_DAC_CLKFREE,   0x00 },
    { ES8389_REG_ADC_DRC_VOL1,  0xBF },
    { ES8389_REG_ADC_DRC_VOL2,  0xBF },
    { ES8389_REG_ADC_DRC_VOL3,  0xBF },
    { ES8389_REG_DAC_VOL_L,     0xBF },
    { ES8389_REG_DAC_VOL_R,     0xBF },
    { ES8389_REG_DAC_VOL_MIXC,  0xBE },
};

// bias-on script(the vendor enable path, DAC + ADC analog up)
static const vk_es8389_reg_op_t __vk_es8389_bias_on_seq[] = {
    { ES8389_REG_DAC_CLKFREE,   0x00 },
    { ES8389_REG_ANA5,          0x20 },     // bit5 on top of the open value
    { ES8389_REG_ANA1,          0xD9 },
    { ES8389_REG_ANA4,          0x8F },
    { ES8389_REG_CLK_DAC,       0xE4 },
    { ES8389_REG_RESET,         0x01 },
    { ES8389_REG_CLK_SYS_DIV,   0xC3 },
    { ES8389_REG_ADC_NG,        0x6A },
    { ES8389_REG_ADC_DRC,       0x0A },
};

// stop = the vendor es8389_suspend sequence verbatim (esp_codec_dev
// device/es8389). The previous quiesce stopped at the vendor's *bias
// standby* half and relied on RESET=0x7E wiping the remaining config,
// which this silicon does not do - the half-live analog state (VMID up,
// DAC ref on, outputs un-isolated) dissipated heat after every close
// (cb U3 case 2026-11-14). Entries with reg 0xFF are settle delays
// (value = ms), resumed in the worker's VSF_EVT_TIMER.
static const vk_es8389_reg_op_t __vk_es8389_quiesce_seq[] = {
    { ES8389_REG_DAC_SP,        0x03, 0x03 },
    { ES8389_REG_CLK_DAC,       0xD4 },
    { 0xFF,                     70,   0x00 },
    { ES8389_REG_ANA1,          0x59 },
    { ES8389_REG_ANA4,          0x00 },
    { ES8389_REG_ANA3,          0x00 },
    { ES8389_REG_CLK_SYS_DIV,   0x00 },
    { ES8389_REG_RESET,         0x7E },
    { ES8389_REG_DAC_SP,        0x00, 0x03 },
    { ES8389_REG_MISC,          0x28 },
    { ES8389_REG_ANA5,          0x00, 0x20 },
    { ES8389_REG_VMID,          0x00 },
    { ES8389_REG_RESET,         0x00 },
    { ES8389_REG_CLK_DAC,       0xCC },
    { 0xFF,                     500,  0x00 },
    { ES8389_REG_CLK_DAC,       0x00 },
    { ES8389_REG_ANA1,          0x08 },
    { ES8389_REG_ISOLATION,     0xC1 },
    { ES8389_REG_PULL_DOWN,     0x00 },
};

// per-rate clock coefficients(the vendor clock table). Only the ratio-64
// rows are kept: the ratio-32 rows saturate both ADC and DAC on this codec
// revision(set_fs refuses them), and 96k has no 32-ratio row in the vendor
// table either
static const vk_es8389_coeff_t __vk_es8389_coeff_tab[] = {
    { .fs = 8000, .mclk_ratio = 64, .pll = {0x00, 0x4D, 0x24, 0xC0, 0x03, 0xD1}, .div_l = 0xB0,
      .clk_adc = 0x00, .adcalc1 = 0x00, .adc_alc1 = 0x1F, .adc_alc2 = 0x7F,
      .adc_drc1 = 0xBF, .dac_ctrl = 0xC0, .dac_filter = 0xFF, .vol_mix = 0x7F,
      .clkfree = 0x01, .chip_misc = 0x12, .csm = 0x00,
      .adc_mclk = 0x09, .adc_divm = 0x19, .adc_divh = 0x07 },
    { .fs = 16000, .mclk_ratio = 64, .pll = {0x00, 0x45, 0x24, 0xC0, 0x01, 0xD1}, .div_l = 0x90,
      .clk_adc = 0x00, .adcalc1 = 0x00, .adc_alc1 = 0x1F, .adc_alc2 = 0x7F,
      .adc_drc1 = 0xBF, .dac_ctrl = 0xC0, .dac_filter = 0xFF, .vol_mix = 0x7F,
      .clkfree = 0x00, .chip_misc = 0x12, .csm = 0x00,
      .adc_mclk = 0x12, .adc_divm = 0x31, .adc_divh = 0x0E },
    { .fs = 44100, .mclk_ratio = 64, .pll = {0x00, 0x51, 0x00, 0xC0, 0x10, 0xC1}, .div_l = 0x80,
      .clk_adc = 0x00, .adcalc1 = 0x00, .adc_alc1 = 0x1F, .adc_alc2 = 0x7F,
      .adc_drc1 = 0xBF, .dac_ctrl = 0xC0, .dac_filter = 0x7F, .vol_mix = 0x7F,
      .clkfree = 0x00, .chip_misc = 0x12, .csm = 0x00,
      .adc_mclk = 0x35, .adc_divm = 0x91, .adc_divh = 0x28 },
    { .fs = 48000, .mclk_ratio = 64, .pll = {0x00, 0x51, 0x04, 0xC0, 0x10, 0xC1}, .div_l = 0x80,
      .clk_adc = 0x00, .adcalc1 = 0x00, .adc_alc1 = 0x1F, .adc_alc2 = 0x7F,
      .adc_drc1 = 0xBF, .dac_ctrl = 0xC0, .dac_filter = 0x7F, .vol_mix = 0x7F,
      .clkfree = 0x00, .chip_misc = 0x12, .csm = 0x00,
      .adc_mclk = 0x35, .adc_divm = 0x91, .adc_divh = 0x28 },
    { .fs = 96000, .mclk_ratio = 64, .pll = {0x00, 0x41, 0x00, 0xD0, 0x10, 0xD1}, .div_l = 0x80,
      .clk_adc = 0x00, .adcalc1 = 0x00, .adc_alc1 = 0x9F, .adc_alc2 = 0x7F,
      .adc_drc1 = 0xBF, .dac_ctrl = 0xC0, .dac_filter = 0x7F, .vol_mix = 0x7F,
      .clkfree = 0x80, .chip_misc = 0x12, .csm = 0xC0,
      .adc_mclk = 0x35, .adc_divm = 0x91, .adc_divh = 0x28 },
};

/*============================ PROTOTYPES ====================================*/

static void __vk_es8389_pa_set(vk_es8389_dev_t *dev, bool enable);
static void __vk_es8389_dispatch(vk_es8389_dev_t *dev);
static void __vk_es8389_work_begin(vk_es8389_dev_t *dev, uint8_t work);
static void __vk_es8389_step(vk_es8389_dev_t *dev);
static void __vk_es8389_work_fail(vk_es8389_dev_t *dev, vsf_err_t err);
static void __vk_es8389_work_complete(vk_es8389_dev_t *dev);
static void __vk_es8389_i2c_complete(vk_es8389_dev_t *dev, vsf_sync_reason_t reason);

/*============================ IMPLEMENTATION ================================*/

static void __vk_es8389_i2c_isr(void *target_ptr, vsf_i2c_t *i2c_ptr,
            vsf_i2c_irq_mask_t irq_mask)
{
    (void)i2c_ptr;
    vk_es8389_dev_t *dev = target_ptr;
    dev->i2c_irq = irq_mask;
    vsf_eda_sem_post_isr(&dev->i2c_sem);
}

static void __vk_es8389_stream_state(void *target_ptr,
            vk_audio_stream_t *audio_stream, bool is_started)
{
    vk_es8389_dev_t *dev = target_ptr;
    uint8_t mask = audio_stream->dir_in1out0 ?
        __VK_ES8389_CAPTURE_MASK : __VK_ES8389_PLAYBACK_MASK;
    vsf_protect_t orig = vsf_protect_int();
        if (is_started) {
            dev->started_mask |= mask;
        } else {
            dev->started_mask &= ~mask;
            if (0 == dev->started_mask) {
                dev->session++;
                dev->is_ready = false;
            }
        }
    vsf_unprotect_int(orig);
    vsf_eda_sem_post_isr(&dev->cmd_sem);
}

static void __vk_es8389_reset_codec_state(vk_es8389_dev_t *dev)
{
    dev->fs_configured = false;
    dev->analog_enabled = false;
    dev->is_opened = false;
    dev->is_ready = false;
    dev->enabled_mask = 0;
    __vk_es8389_pa_set(dev, false);
}

static void __vk_es8389_complete_control(vk_es8389_dev_t *dev, vsf_err_t err)
{
    if (dev->ctrl_pending && !dev->ctrl_completed) {
        dev->ctrl_result = err;
        dev->ctrl_completed = true;
        vsf_eda_trig_set(&dev->ctrl_trig);
    }
}

static void __vk_es8389_complete_stop(vk_es8389_dev_t *dev, vsf_err_t err)
{
    if (dev->stop_pending && !dev->stop_completed) {
        dev->stop_result = err;
        dev->stop_completed = true;
        vsf_eda_trig_set(&dev->stop_trig);
    }
}

static int32_t __vk_es8389_volume_half_db(vk_av_control_type_t type,
            vk_av_control_value_t value)
{
    switch (type) {
    case VSF_AUDIO_CTRL_VOLUME_DB:
        return value.sval16 / 128;
    case VSF_AUDIO_CTRL_VOLUME_PERCENTAGE:
        return -191 + (int32_t)(((uint32_t)value.uval16 * 255 + 32767) / 65535);
    default:
        return INT32_MIN;
    }
}

static vsf_err_t __vk_es8389_control_validate(vk_es8389_dev_t *dev,
            uint8_t dir, vk_av_control_type_t type, vk_av_control_value_t value)
{
    if (!(dev->request_mask & (1 << dir))) {
        return VSF_ERR_NOT_READY;
    }
    if (!(dev->started_mask & (1 << dir))) {
        return VSF_ERR_NOT_READY;
    }
    if (dev->ctrl_pending) {
        return VSF_ERR_BUSY;
    }
    if (dev->i2c_timed_out) {
        return VSF_ERR_TIMEOUT;
    }
    if (VSF_AUDIO_CTRL_MUTE == type) {
        return VSF_ERR_NONE;
    }
    if (__VK_ES8389_DIR_PLAYBACK == dir) {
        int32_t half_db = __vk_es8389_volume_half_db(type, value);
        if (INT32_MIN == half_db) {
            return VSF_ERR_NOT_SUPPORT;
        }
        if ((half_db < -191) || (half_db > 64)) {
            return VSF_ERR_INVALID_PARAMETER;
        }
        return VSF_ERR_NONE;
    }
    return VSF_AUDIO_CTRL_VOLUME_PERCENTAGE == type ? VSF_ERR_NONE : VSF_ERR_NOT_SUPPORT;
}

static vsf_err_t __vk_es8389_start_prepare(vk_es8389_dev_t *dev,
            uint8_t dir, uint16_t sample_rate)
{
    uint8_t mask = 1 << dir;
    if ((dev->request_mask & ~mask) && (dev->sample_rate[!dir] != sample_rate)) {
        return VSF_ERR_NOT_SUPPORT;
    }
    if ((dev->playback_volume_half_db < -191) || (dev->playback_volume_half_db > 64)
            || (dev->capture_pga_gain_steps > 15)) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    if (0 == dev->request_mask) {
        dev->session++;
        dev->started_mask = 0;
        dev->session_failed = false;
        dev->recover_retry = false;
        dev->retry = 0;
        dev->last_error = VSF_ERR_NONE;
        dev->playback_volume = dev->playback_volume_half_db;
        dev->capture_gain = dev->capture_pga_gain_steps;
        dev->playback_muted = false;
        dev->capture_muted = false;
        __vk_es8389_reset_codec_state(dev);
    }
    dev->sample_rate[dir] = sample_rate;
    dev->request_mask |= mask;
    dev->is_ready = false;
    vsf_eda_sem_post(&dev->cmd_sem);
    return VSF_ERR_NONE;
}

static void __vk_es8389_start_done(vk_es8389_dev_t *dev, uint8_t dir, vsf_err_t err)
{
    if (VSF_ERR_NONE != err) {
        dev->request_mask &= ~(1 << dir);
    }
    vsf_eda_sem_post(&dev->cmd_sem);
}

static vsf_err_t __vk_es8389_stop_prepare(vk_es8389_dev_t *dev, uint8_t dir)
{
    if (dev->stop_pending) {
        return VSF_ERR_BUSY;
    }
    dev->request_mask &= ~(1 << dir);
    dev->enabled_mask &= ~(1 << dir);
    dev->is_ready = false;
    dev->stop_final = 0 == dev->request_mask;
    if (dev->stop_final) {
        dev->session++;
    }
    if (dev->stop_final || (__VK_ES8389_DIR_PLAYBACK == dir)) {
        __vk_es8389_pa_set(dev, false);
    }
    dev->stop_completed = false;
    dev->stop_result = VSF_ERR_NONE;
    dev->stop_pending = true;
    if (dev->i2c_timed_out) {
        if (dev->stop_final) {
            dev->recover_retry = false;
            dev->work = __VK_ES8389_WORK_NONE;
            __vk_es8389_reset_codec_state(dev);
            __vk_es8389_complete_control(dev, VSF_ERR_NOT_READY);
        }
        __vk_es8389_complete_stop(dev, dev->stop_final ? VSF_ERR_TIMEOUT : VSF_ERR_NONE);
    }
    vsf_eda_sem_post(&dev->cmd_sem);
    return VSF_ERR_NONE;
}

static void __vk_es8389_prepare_open_tail(vk_es8389_dev_t *dev)
{
    dev->control_ops[0] = (vk_es8389_reg_op_t){ ES8389_REG_MISC, 0x00, 0x01 };
    dev->control_ops[1] = (vk_es8389_reg_op_t){ ES8389_REG_CLK_STATE, 0x40, 0xC0 };
    dev->control_ops[2] = (vk_es8389_reg_op_t){ ES8389_REG_ADC_ALC3,
        dev->no_dac_ref ? 0x00 : 0x80, 0x80 };
    dev->control_ops[3] = (vk_es8389_reg_op_t){ ES8389_REG_CHIP_MISC,
        dev->no_dac_ref ? 0x12 : 0x1A, 0x00 };
    dev->ops = dev->control_ops;
    dev->op_count = 4;
}

static vsf_err_t __vk_es8389_prepare_fs(vk_es8389_dev_t *dev)
{
    uint32_t sample_rate_hz = (dev->request_mask & __VK_ES8389_PLAYBACK_MASK) ?
        dev->sample_rate[__VK_ES8389_DIR_PLAYBACK] * 100 :
        dev->sample_rate[__VK_ES8389_DIR_CAPTURE] * 100;
    const vk_es8389_coeff_t *coeff = NULL;
    for (uint_fast8_t i = 0; i < dimof(__vk_es8389_coeff_tab); i++) {
        if (__vk_es8389_coeff_tab[i].fs == sample_rate_hz) {
            coeff = &__vk_es8389_coeff_tab[i];
            break;
        }
    }
    if (NULL == coeff) {
        return VSF_ERR_NOT_SUPPORT;
    }

    static const uint8_t regs[] = {
        ES8389_REG_CLK_DIV_L, ES8389_REG_CLK_ADC, ES8389_REG_CLK_ADCDIV1,
        ES8389_REG_ADC_ALC1, ES8389_REG_ADC_ALC2, ES8389_REG_ADC_DRC_VOL1,
        ES8389_REG_DAC_CTRL, ES8389_REG_DAC_FILTER, ES8389_REG_DAC_VOL_MIX,
        ES8389_REG_DAC_CLKFREE, ES8389_REG_CHIP_MISC, ES8389_REG_CSM_STATE,
        ES8389_REG_ADC_MCLK, ES8389_REG_ADC_DIV_M, ES8389_REG_ADC_DIV_H,
    };
    static const uint8_t masks[] = {
        0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
        0x00, 0x81, 0x73, 0x80, 0x00, 0x00, 0x00,
    };
    const uint8_t values[] = {
        coeff->div_l, coeff->clk_adc, coeff->adcalc1,
        coeff->adc_alc1, coeff->adc_alc2, coeff->adc_drc1,
        coeff->dac_ctrl, coeff->dac_filter, coeff->vol_mix,
        coeff->clkfree, coeff->chip_misc, coeff->csm,
        coeff->adc_mclk, coeff->adc_divm, coeff->adc_divh,
    };
    for (uint_fast8_t i = 0; i < 6; i++) {
        dev->fs_ops[i] = (vk_es8389_reg_op_t){ ES8389_REG_CLK_PLL1 + i, coeff->pll[i], 0 };
    }
    for (uint_fast8_t i = 0; i < dimof(regs); i++) {
        dev->fs_ops[6 + i] = (vk_es8389_reg_op_t){ regs[i], values[i], masks[i] };
    }
    dev->ops = dev->fs_ops;
    dev->op_count = 6 + dimof(regs);
    return VSF_ERR_NONE;
}

static void __vk_es8389_prepare_volume(vk_es8389_dev_t *dev, int32_t half_db)
{
    uint8_t value = (uint8_t)(half_db + ES8389_VOL_HALFDB_BIAS);
    dev->control_ops[0] = (vk_es8389_reg_op_t){ ES8389_REG_DAC_VOL_L, value, 0 };
    dev->control_ops[1] = (vk_es8389_reg_op_t){ ES8389_REG_DAC_VOL_R, value, 0 };
    dev->ops = dev->control_ops;
    dev->op_count = 2;
}

static void __vk_es8389_prepare_capture(vk_es8389_dev_t *dev, uint8_t gain)
{
    dev->control_ops[0] = (vk_es8389_reg_op_t){ ES8389_REG_PGA_L, (1 << 4) | gain, 0 };
    dev->control_ops[1] = (vk_es8389_reg_op_t){ ES8389_REG_PGA_R, (1 << 4) | gain, 0 };
    dev->control_ops[2] = (vk_es8389_reg_op_t){ ES8389_REG_ADC_SP, 0x00, 0x03 };
    dev->ops = dev->control_ops;
    dev->op_count = 3;
}

static vsf_err_t __vk_es8389_prepare_control(vk_es8389_dev_t *dev)
{
    if (!(dev->request_mask & (1 << dev->ctrl_dir))) {
        return VSF_ERR_NOT_READY;
    }
    if (__VK_ES8389_DIR_PLAYBACK == dev->ctrl_dir) {
        int32_t half_db = dev->playback_volume;
        if (VSF_AUDIO_CTRL_MUTE == dev->ctrl_type) {
            half_db = dev->ctrl_value.enable ? -191 : dev->playback_volume;
        } else {
            half_db = __vk_es8389_volume_half_db(dev->ctrl_type, dev->ctrl_value);
        }
        __vk_es8389_prepare_volume(dev, half_db);
    } else {
        if (VSF_AUDIO_CTRL_MUTE == dev->ctrl_type && dev->ctrl_value.enable) {
            dev->control_ops[0] = (vk_es8389_reg_op_t){ ES8389_REG_ADC_SP, 0x03, 0x03 };
            dev->ops = dev->control_ops;
            dev->op_count = 1;
        } else {
            uint8_t gain = VSF_AUDIO_CTRL_VOLUME_PERCENTAGE == dev->ctrl_type ?
                (uint8_t)(((uint32_t)dev->ctrl_value.uval16 * 15 + 32767) / 65535) :
                dev->capture_gain;
            __vk_es8389_prepare_capture(dev, gain);
        }
    }
    return VSF_ERR_NONE;
}

static void __vk_es8389_work_begin(vk_es8389_dev_t *dev, uint8_t work)
{
    vsf_err_t err = VSF_ERR_NONE;
    dev->work = work;
    dev->work_session = dev->session;
    dev->op_index = 0;
    dev->ops = NULL;
    dev->op_count = 0;

    switch (work) {
    case __VK_ES8389_WORK_OPEN:
        dev->ops = __vk_es8389_open_seq;
        dev->op_count = dimof(__vk_es8389_open_seq);
        break;
    case __VK_ES8389_WORK_OPEN_TAIL:
        __vk_es8389_prepare_open_tail(dev);
        break;
    case __VK_ES8389_WORK_SET_FS:
        err = __vk_es8389_prepare_fs(dev);
        break;
    case __VK_ES8389_WORK_ENABLE_BIAS:
    case __VK_ES8389_WORK_ENABLE_PLAYBACK:
        dev->ops = __vk_es8389_bias_on_seq;
        dev->op_count = dimof(__vk_es8389_bias_on_seq);
        break;
    case __VK_ES8389_WORK_DEFAULT_VOLUME:
        __vk_es8389_prepare_volume(dev, dev->playback_volume);
        break;
    case __VK_ES8389_WORK_ENABLE_CAPTURE:
        __vk_es8389_prepare_capture(dev, dev->capture_gain);
        break;
    case __VK_ES8389_WORK_CONTROL:
        err = __vk_es8389_prepare_control(dev);
        break;
    case __VK_ES8389_WORK_QUIESCE:
        __vk_es8389_pa_set(dev, false);
        dev->ops = __vk_es8389_quiesce_seq;
        dev->op_count = dimof(__vk_es8389_quiesce_seq);
        break;
    default:
        VSF_AV_ASSERT(false);
        return;
    }
    if (VSF_ERR_NONE != err) {
        __vk_es8389_work_fail(dev, err);
        return;
    }
    __vk_es8389_step(dev);
}

static void __vk_es8389_i2c_request(vk_es8389_dev_t *dev,
            vsf_i2c_cmd_t cmd, uint_fast16_t len, uint8_t *buffer)
{
    dev->i2c_irq = 0;
    dev->i2c_outstanding = true;
    vsf_err_t err = vsf_i2c_master_request(dev->i2c_ptr, dev->i2c_addr,
        cmd | VSF_I2C_CMD_7_BITS, len, buffer);
    if (VSF_ERR_NONE != err) {
        dev->i2c_outstanding = false;
        __vk_es8389_work_fail(dev, err);
        return;
    }

    dev->wait_state = __VK_ES8389_WAIT_I2C;
    if (VSF_ERR_NONE == vsf_eda_sem_pend(&dev->i2c_sem, __VK_ES8389_I2C_TIMEOUT_TICK)) {
        __vk_es8389_i2c_complete(dev, VSF_SYNC_GET);
    }
}

static void __vk_es8389_step(vk_es8389_dev_t *dev)
{
    if ((dev->work_session != dev->session)
            && (__VK_ES8389_WORK_QUIESCE != dev->work)) {
        dev->work = __VK_ES8389_WORK_NONE;
        __vk_es8389_dispatch(dev);
        return;
    }
    if (dev->op_index >= dev->op_count) {
        __vk_es8389_work_complete(dev);
        return;
    }

    const vk_es8389_reg_op_t *op = &dev->ops[dev->op_index];
    if (0xFF == op->reg) {
        // settle-delay entry (op->value = ms): advance past the marker
        // FIRST, then wait a real interval - i2c_sem has no outstanding
        // poster during quiesce, so this pend always consumes the full
        // timeout(in-place, same shape as the i2c wait)
        dev->op_index++;
        vsf_eda_sem_pend(&dev->i2c_sem, vsf_systimer_ms_to_tick(op->value));
        __vk_es8389_step(dev);
        return;
    }
    dev->tx_buf[0] = op->reg;
    if (0 == op->mask) {
        dev->tx_buf[1] = op->value;
        dev->xfer_phase = __VK_ES8389_XFER_WRITE;
        __vk_es8389_i2c_request(dev,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP, 2, dev->tx_buf);
    } else {
        dev->xfer_phase = __VK_ES8389_XFER_READ_ADDR;
        __vk_es8389_i2c_request(dev, VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE, 1, dev->tx_buf);
    }
}

static void __vk_es8389_i2c_complete(vk_es8389_dev_t *dev, vsf_sync_reason_t reason)
{
    if (VSF_SYNC_GET != reason) {
        dev->i2c_timed_out = dev->i2c_outstanding;
        __vk_es8389_work_fail(dev, VSF_SYNC_TIMEOUT == reason ? VSF_ERR_TIMEOUT : VSF_ERR_FAIL);
        return;
    }

    dev->i2c_outstanding = false;
    if ((dev->work_session != dev->session)
            && (__VK_ES8389_WORK_QUIESCE != dev->work)) {
        dev->work = __VK_ES8389_WORK_NONE;
        __vk_es8389_dispatch(dev);
        return;
    }
    if (!(dev->i2c_irq & VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE)) {
        __vk_es8389_work_fail(dev, VSF_ERR_FAIL);
        return;
    }

    const vk_es8389_reg_op_t *op = &dev->ops[dev->op_index];
    switch (dev->xfer_phase) {
    case __VK_ES8389_XFER_READ_ADDR:
        dev->xfer_phase = __VK_ES8389_XFER_READ_DATA;
        __vk_es8389_i2c_request(dev,
            VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP, 1, dev->rx_buf);
        return;
    case __VK_ES8389_XFER_READ_DATA:
        dev->tx_buf[0] = op->reg;
        dev->tx_buf[1] = (dev->rx_buf[0] & ~op->mask) | (op->value & op->mask);
        dev->xfer_phase = __VK_ES8389_XFER_WRITE;
        __vk_es8389_i2c_request(dev,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP, 2, dev->tx_buf);
        return;
    case __VK_ES8389_XFER_WRITE:
        dev->op_index++;
        __vk_es8389_step(dev);
        return;
    }
}

static void __vk_es8389_work_complete(vk_es8389_dev_t *dev)
{
    switch (dev->work) {
    case __VK_ES8389_WORK_OPEN:
        __vk_es8389_work_begin(dev, __VK_ES8389_WORK_OPEN_TAIL);
        return;
    case __VK_ES8389_WORK_OPEN_TAIL:
        dev->is_opened = true;
        dev->fs_configured = false;
        break;
    case __VK_ES8389_WORK_SET_FS:
        dev->fs_configured = true;
        break;
    case __VK_ES8389_WORK_ENABLE_PLAYBACK:
        dev->analog_enabled = true;
        if (dev->request_mask & dev->started_mask & __VK_ES8389_PLAYBACK_MASK) {
            __vk_es8389_pa_set(dev, true);
            __vk_es8389_work_begin(dev, __VK_ES8389_WORK_DEFAULT_VOLUME);
        } else {
            dev->work = __VK_ES8389_WORK_NONE;
            __vk_es8389_dispatch(dev);
        }
        return;
    case __VK_ES8389_WORK_ENABLE_BIAS:
        dev->analog_enabled = true;
        break;
    case __VK_ES8389_WORK_DEFAULT_VOLUME:
        dev->enabled_mask |= __VK_ES8389_PLAYBACK_MASK;
        break;
    case __VK_ES8389_WORK_ENABLE_CAPTURE:
        if (dev->request_mask & dev->started_mask & __VK_ES8389_CAPTURE_MASK) {
            dev->enabled_mask |= __VK_ES8389_CAPTURE_MASK;
        }
        break;
    case __VK_ES8389_WORK_CONTROL:
        if (!(dev->request_mask & (1 << dev->ctrl_dir))) {
            __vk_es8389_complete_control(dev, VSF_ERR_NOT_READY);
            break;
        }
        if (__VK_ES8389_DIR_PLAYBACK == dev->ctrl_dir) {
            if (VSF_AUDIO_CTRL_MUTE == dev->ctrl_type) {
                dev->playback_muted = dev->ctrl_value.enable;
            } else {
                dev->playback_volume = __vk_es8389_volume_half_db(dev->ctrl_type, dev->ctrl_value);
                dev->playback_muted = false;
            }
        } else {
            if (VSF_AUDIO_CTRL_MUTE == dev->ctrl_type) {
                dev->capture_muted = dev->ctrl_value.enable;
            } else {
                dev->capture_gain = (uint8_t)(((uint32_t)dev->ctrl_value.uval16 * 15 + 32767) / 65535);
                dev->capture_muted = false;
            }
        }
        __vk_es8389_complete_control(dev, VSF_ERR_NONE);
        break;
    case __VK_ES8389_WORK_QUIESCE:
        __vk_es8389_reset_codec_state(dev);
        __vk_es8389_complete_stop(dev, VSF_ERR_NONE);
        break;
    default:
        VSF_AV_ASSERT(false);
        break;
    }
    dev->work = __VK_ES8389_WORK_NONE;
    __vk_es8389_dispatch(dev);
}

static void __vk_es8389_retry_later(vk_es8389_dev_t *dev)
{
    dev->work = __VK_ES8389_WORK_NONE;
    dev->wait_state = __VK_ES8389_WAIT_RETRY;
    vsf_teda_set_timer_ms(100);
}

static void __vk_es8389_work_fail(vk_es8389_dev_t *dev, vsf_err_t err)
{
    uint8_t failed_work = dev->work;
    dev->work = __VK_ES8389_WORK_NONE;
    dev->last_error = err;

    if (__VK_ES8389_WORK_QUIESCE == failed_work) {
        __vk_es8389_reset_codec_state(dev);
        __vk_es8389_complete_stop(dev, err);
        __vk_es8389_dispatch(dev);
        return;
    }
    if (__VK_ES8389_WORK_CONTROL == failed_work) {
        __vk_es8389_complete_control(dev, err);
        __vk_es8389_dispatch(dev);
        return;
    }

    __vk_es8389_reset_codec_state(dev);
    if ((!dev->stop_pending || !dev->stop_final) && (dev->retry < 4)) {
        dev->retry++;
        if (dev->i2c_timed_out) {
            dev->recover_retry = true;
            __vk_es8389_dispatch(dev);
        } else {
            __vk_es8389_retry_later(dev);
        }
        return;
    }

    dev->session_failed = true;
    __vk_es8389_complete_control(dev, err);
    vsf_trace_error("es8389: startup failed %d\n", err);
    __vk_es8389_dispatch(dev);
}

static void __vk_es8389_recovery_done(vk_es8389_dev_t *dev)
{
    dev->i2c_timed_out = false;
    dev->i2c_outstanding = false;
    dev->i2c_irq = 0;
    if (dev->recover_retry) {
        dev->recover_retry = false;
        if (!dev->session_failed && (!dev->stop_pending || !dev->stop_final)) {
            __vk_es8389_retry_later(dev);
            return;
        }
    }
    __vk_es8389_dispatch(dev);
}

static void __vk_es8389_dispatch(vk_es8389_dev_t *dev)
{
    if (dev->stop_pending && !dev->stop_completed) {
        if (!dev->stop_final) {
            __vk_es8389_complete_stop(dev, VSF_ERR_NONE);
        } else {
            dev->recover_retry = false;
            __vk_es8389_complete_control(dev, VSF_ERR_NOT_READY);
            if (dev->i2c_timed_out) {
                __vk_es8389_reset_codec_state(dev);
                __vk_es8389_complete_stop(dev, VSF_ERR_TIMEOUT);
            } else if (__VK_ES8389_WORK_QUIESCE == dev->work) {
                return;
            } else if (dev->is_opened) {
                dev->work = __VK_ES8389_WORK_NONE;
                __vk_es8389_work_begin(dev, __VK_ES8389_WORK_QUIESCE);
                return;
            } else {
                dev->work = __VK_ES8389_WORK_NONE;
                __vk_es8389_reset_codec_state(dev);
                __vk_es8389_complete_stop(dev, VSF_ERR_NONE);
            }
        }
    }

    if (dev->i2c_timed_out) {
        dev->wait_state = __VK_ES8389_WAIT_RECOVER;
        if (VSF_ERR_NONE == vsf_eda_sem_pend(&dev->i2c_sem, -1)) {
            __vk_es8389_recovery_done(dev);
        }
        return;
    }
    if (__VK_ES8389_WORK_NONE != dev->work) {
        return;
    }

    if (dev->started_mask || !dev->request_mask) {
        // A real stopped edge is handled by the normal stop path. An
        // unexpected disconnect only needs to prevent further codec work.
    } else if (dev->is_opened) {
        dev->session_failed = true;
        __vk_es8389_reset_codec_state(dev);
        __vk_es8389_complete_control(dev, VSF_ERR_NOT_READY);
    }

    uint8_t active = dev->request_mask & dev->started_mask;
    if (!dev->session_failed && active && !dev->is_opened) {
        __vk_es8389_work_begin(dev, __VK_ES8389_WORK_OPEN);
        return;
    }
    if (dev->is_opened && !dev->fs_configured) {
        __vk_es8389_work_begin(dev, __VK_ES8389_WORK_SET_FS);
        return;
    }
    if (dev->is_opened && dev->fs_configured) {
        uint8_t pending = active & ~dev->enabled_mask;
        if (pending & __VK_ES8389_PLAYBACK_MASK) {
            dev->is_ready = false;
            if (!dev->analog_enabled) {
                __vk_es8389_work_begin(dev, __VK_ES8389_WORK_ENABLE_PLAYBACK);
            } else {
                __vk_es8389_pa_set(dev, true);
                __vk_es8389_work_begin(dev, __VK_ES8389_WORK_DEFAULT_VOLUME);
            }
            return;
        }
        if (pending & __VK_ES8389_CAPTURE_MASK) {
            dev->is_ready = false;
            __vk_es8389_work_begin(dev, dev->analog_enabled ?
                __VK_ES8389_WORK_ENABLE_CAPTURE : __VK_ES8389_WORK_ENABLE_BIAS);
            return;
        }
        bool is_ready = !dev->session_failed && (active == dev->enabled_mask);
        if (is_ready && !dev->is_ready) {
            uint8_t dir = (active & __VK_ES8389_PLAYBACK_MASK) ?
                __VK_ES8389_DIR_PLAYBACK : __VK_ES8389_DIR_CAPTURE;
            vsf_trace_info("es8389: ready(%u Hz)\n", dev->sample_rate[dir] * 100);
        }
        dev->is_ready = is_ready;
        if (dev->is_ready && dev->ctrl_pending && !dev->ctrl_completed) {
            __vk_es8389_work_begin(dev, __VK_ES8389_WORK_CONTROL);
            return;
        }
    }

    dev->wait_state = __VK_ES8389_WAIT_CMD;
    if (VSF_ERR_NONE == vsf_eda_sem_pend(&dev->cmd_sem, -1)) {
        __vk_es8389_dispatch(dev);
    }
}

static void __vk_es8389_worker(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_es8389_dev_t *dev = vsf_container_of(eda, vk_es8389_dev_t, teda);
    vsf_sync_reason_t reason;

    switch (evt) {
    case VSF_EVT_INIT:
        __vk_es8389_dispatch(dev);
        return;
    case VSF_EVT_TIMER:
        if (__VK_ES8389_WAIT_RETRY == dev->wait_state) {
            if (!dev->session_failed && (!dev->stop_pending || !dev->stop_final)) {
                __vk_es8389_work_begin(dev, __VK_ES8389_WORK_OPEN);
            } else {
                __vk_es8389_dispatch(dev);
            }
            return;
        }
        break;
    case VSF_EVT_SYNC:
    case VSF_EVT_SYNC_CANCEL:
        break;
    default:
        return;
    }

    switch (dev->wait_state) {
    case __VK_ES8389_WAIT_CMD:
        reason = vsf_eda_sync_get_reason(&dev->cmd_sem, evt);
        if (VSF_SYNC_GET == reason) {
            __vk_es8389_dispatch(dev);
        }
        return;
    case __VK_ES8389_WAIT_I2C:
        reason = vsf_eda_sync_get_reason(&dev->i2c_sem, evt);
        if (VSF_SYNC_PENDING != reason) {
            __vk_es8389_i2c_complete(dev, reason);
        }
        return;
    case __VK_ES8389_WAIT_RECOVER:
        reason = vsf_eda_sync_get_reason(&dev->i2c_sem, evt);
        if (VSF_SYNC_GET == reason) {
            __vk_es8389_recovery_done(dev);
        }
        return;
    }
}

static void __vk_es8389_pa_set(vk_es8389_dev_t *dev, bool enable)
{
    if ((NULL == dev->pa_port) || (0 == dev->pa_pin_mask)) {
        return;
    }
    if (enable == dev->pa_active_high) {
        vsf_gpio_set(dev->pa_port, dev->pa_pin_mask);
    } else {
        vsf_gpio_clear(dev->pa_port, dev->pa_pin_mask);
    }
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
__vsf_component_peda_ifs_entry(__vk_es8389_playback_control, vk_audio_control)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_control_validate(dev, __VK_ES8389_DIR_PLAYBACK,
            vsf_local.type, vsf_local.value);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        dev->ctrl_dir = __VK_ES8389_DIR_PLAYBACK;
        dev->ctrl_type = vsf_local.type;
        dev->ctrl_value = vsf_local.value;
        dev->ctrl_completed = false;
        dev->ctrl_pending = true;
        vsf_eda_sem_post(&dev->cmd_sem);
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&dev->ctrl_trig, -1)) {
            break;
        }
        goto control_done;
    case VSF_EVT_SYNC:
        if (VSF_SYNC_GET != vsf_eda_sync_get_reason(&dev->ctrl_trig, evt)) {
            dev->ctrl_pending = false;
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
    control_done:
        err = dev->ctrl_result;
        dev->ctrl_pending = false;
        dev->ctrl_completed = false;
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_es8389_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_start_prepare(dev, __VK_ES8389_DIR_PLAYBACK,
            vsf_local.audio_stream->format.sample_rate);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        __vsf_component_call_peda_ifs(vk_audio_start, err,
            vk_audio_i2s_stream_drv_playback.start, 0,
            &dev->use_as__vk_audio_dev_t,
            .audio_stream = vsf_local.audio_stream,
        );
        if (VSF_ERR_NONE != err) {
            __vk_es8389_start_done(dev, __VK_ES8389_DIR_PLAYBACK, err);
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        __vk_es8389_start_done(dev, __VK_ES8389_DIR_PLAYBACK, err);
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_es8389_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_stop_prepare(dev, __VK_ES8389_DIR_PLAYBACK);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&dev->stop_trig, -1)) {
            break;
        }
        goto stop_ready;
    case VSF_EVT_SYNC:
        if (VSF_SYNC_GET != vsf_eda_sync_get_reason(&dev->stop_trig, evt)) {
            dev->stop_pending = false;
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
    stop_ready:
        __vsf_component_call_peda_ifs(vk_audio_stop, err,
            vk_audio_i2s_stream_drv_playback.stop, 0,
            &dev->use_as__vk_audio_dev_t,
            .audio_stream = vsf_local.audio_stream,
        );
        if (VSF_ERR_NONE != err) {
            dev->stop_pending = false;
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        if (VSF_ERR_NONE == err) {
            err = dev->stop_result;
        }
        dev->stop_pending = false;
        dev->stop_completed = false;
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
__vsf_component_peda_ifs_entry(__vk_es8389_capture_control, vk_audio_control)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_control_validate(dev, __VK_ES8389_DIR_CAPTURE,
            vsf_local.type, vsf_local.value);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        dev->ctrl_dir = __VK_ES8389_DIR_CAPTURE;
        dev->ctrl_type = vsf_local.type;
        dev->ctrl_value = vsf_local.value;
        dev->ctrl_completed = false;
        dev->ctrl_pending = true;
        vsf_eda_sem_post(&dev->cmd_sem);
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&dev->ctrl_trig, -1)) {
            break;
        }
        goto control_done;
    case VSF_EVT_SYNC:
        if (VSF_SYNC_GET != vsf_eda_sync_get_reason(&dev->ctrl_trig, evt)) {
            dev->ctrl_pending = false;
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
    control_done:
        err = dev->ctrl_result;
        dev->ctrl_pending = false;
        dev->ctrl_completed = false;
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_es8389_capture_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_start_prepare(dev, __VK_ES8389_DIR_CAPTURE,
            vsf_local.audio_stream->format.sample_rate);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        __vsf_component_call_peda_ifs(vk_audio_start, err,
            vk_audio_i2s_stream_drv_capture.start, 0,
            &dev->use_as__vk_audio_dev_t,
            .audio_stream = vsf_local.audio_stream,
        );
        if (VSF_ERR_NONE != err) {
            __vk_es8389_start_done(dev, __VK_ES8389_DIR_CAPTURE, err);
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        __vk_es8389_start_done(dev, __VK_ES8389_DIR_CAPTURE, err);
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_es8389_capture_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        err = __vk_es8389_stop_prepare(dev, __VK_ES8389_DIR_CAPTURE);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        if (VSF_ERR_NONE != vsf_eda_trig_wait(&dev->stop_trig, -1)) {
            break;
        }
        goto stop_ready;
    case VSF_EVT_SYNC:
        if (VSF_SYNC_GET != vsf_eda_sync_get_reason(&dev->stop_trig, evt)) {
            dev->stop_pending = false;
            vsf_eda_return(VSF_ERR_FAIL);
            break;
        }
    stop_ready:
        __vsf_component_call_peda_ifs(vk_audio_stop, err,
            vk_audio_i2s_stream_drv_capture.stop, 0,
            &dev->use_as__vk_audio_dev_t,
            .audio_stream = vsf_local.audio_stream,
        );
        if (VSF_ERR_NONE != err) {
            dev->stop_pending = false;
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        if (VSF_ERR_NONE == err) {
            err = dev->stop_result;
        }
        dev->stop_pending = false;
        dev->stop_completed = false;
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}
#endif

__vsf_component_peda_ifs_entry(__vk_es8389_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_es8389_dev_t *dev = vsf_container_of(&vsf_this, vk_es8389_dev_t,
                use_as__vk_audio_dev_t);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_eda_sem_init(&dev->cmd_sem);
        vsf_eda_sem_init(&dev->i2c_sem);
        vsf_eda_trig_init(&dev->stop_trig, 0, true);
        vsf_eda_trig_init(&dev->ctrl_trig, 0, true);
        dev->i2c_irq = 0;
        dev->i2c_outstanding = false;
        dev->i2c_timed_out = false;
        dev->recover_retry = false;
        dev->request_mask = 0;
        dev->started_mask = 0;
        dev->wait_state = __VK_ES8389_WAIT_CMD;
        dev->work = __VK_ES8389_WORK_NONE;
        dev->session = 0;
        dev->session_failed = false;
        dev->retry = 0;
        dev->last_error = VSF_ERR_NONE;
        dev->stop_pending = false;
        dev->stop_completed = false;
        dev->ctrl_pending = false;
        dev->ctrl_completed = false;
        dev->playback_volume = dev->playback_volume_half_db;
        dev->capture_gain = dev->capture_pga_gain_steps;
        dev->playback_muted = false;
        dev->capture_muted = false;
        __vk_es8389_reset_codec_state(dev);

        err = vsf_i2c_init(dev->i2c_ptr, &(vsf_i2c_cfg_t){
            .mode       = VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_FAST_MODE
                        | VSF_I2C_ADDR_7_BITS,
            .clock_hz   = dev->i2c_clock_hz,
            .isr        = {
                .handler_fn = __vk_es8389_i2c_isr,
                .target_ptr = dev,
                .prio       = dev->i2c_isr_prio,
            },
        });
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        vsf_i2c_irq_enable(dev->i2c_ptr,
            VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
            | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
            | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
            | VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT);
        vsf_i2c_enable(dev->i2c_ptr);

        dev->on_stream_state = __vk_es8389_stream_state;
        dev->stream_state_target = dev;
        __vsf_component_call_peda_ifs(vk_audio_init, err, vk_audio_i2s_drv.init, 0,
                &dev->use_as__vk_audio_dev_t);
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        err = vsf_eda_get_return_value();
        if (VSF_ERR_NONE != err) {
            vsf_eda_return(err);
            break;
        }
        for (uint_fast8_t i = 0; i < dev->stream_num; i++) {
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
            if (!dev->stream[i].dir_in1out0) {
                dev->stream[i].drv = &__vk_es8389_stream_drv_playback;
                continue;
            }
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
            dev->stream[i].drv = &__vk_es8389_stream_drv_capture;
#endif
        }
        dev->teda.fn.evthandler = __vk_es8389_worker;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
        dev->teda.on_terminate = NULL;
#endif
        err = vsf_teda_init(&dev->teda, VSF_AUDIO_CFG_EDA_PRIORITY);
        vsf_eda_return(err);
        break;
    }
    vsf_peda_end();
}

#endif      // VSF_USE_AUDIO && VSF_AUDIO_USE_ES8389
/* EOF */
