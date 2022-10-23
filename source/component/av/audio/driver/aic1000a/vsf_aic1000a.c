/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "../../../vsf_av_cfg.h"

#if VSF_USE_AUDIO == ENABLED && VSF_AUDIO_USE_AIC1000A == ENABLED

#define __VSF_AUDIO_CLASS_INHERIT__
#define __VSF_AIC1000A_CLASS_IMPLEMENT
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__

#include "service/vsf_service.h"
#include "./vsf_aic1000a.h"

#include "aic1000Aud_iomux.h"
#include "aic1000Aud_aud_codec.h"
#include "aic1000Aud_aud_ctrl_reg.h"
#include "aic1000Aud_sys_ctrl.h"
#include "aic1000Aud_wdg.h"

/*============================ MACROS ========================================*/

#define REG_AIC1000AUD_WDG                      ((HWP_AIC1000AUD_WDG_T *)REG_AIC1000AUD_WDG_BASE)
#define REG_AIC1000AUD_AUD_CODEC                ((HWP_AIC1000AUD_AUD_CODEC_T *)REG_AIC1000AUD_AUD_CODEC_BASE)
#define REG_AIC1000AUD_IOMUX                    ((HWP_AIC1000AUD_IOMUX_T *)REG_AIC1000AUD_IOMUX_BASE)
#define REG_AIC1000AUD_SYSCTRL                  ((HWP_AIC1000AUD_SYS_CTRL_T *)REG_AIC1000AUD_SYSCTRL_BASE)
#define REG_AIC1000AUD_AUD_CTRL                 ((HWP_AIC1000AUD_AUD_CTRL_REG_T *)REG_AIC1000AUD_AUD_CTRL_REG_BASE)

#define AIC1000A_REGW(__REG, __VALUE)           {(__REG), 0xFFFFFFFF, (__VALUE)}
#define AIC1000A_REGMW(__REG, __VALUE, __MASK)  {(__REG), (__MASK), (__VALUE)}

#define I2S_CDIV                                14
#define I2S_HCYC_8K                             149
#define I2S_HCYC_16K                            74
#define I2S_HCYC_48K                            24

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __vk_aic1000a_init)

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
dcl_vsf_peda_methods(static, __vk_aic1000a_playback_control)
dcl_vsf_peda_methods(static, __vk_aic1000a_playback_start)
dcl_vsf_peda_methods(static, __vk_aic1000a_playback_stop)

static void __vk_aic1000a_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt);
#endif

#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
dcl_vsf_peda_methods(static, __vk_aic1000a_capture_control)
dcl_vsf_peda_methods(static, __vk_aic1000a_capture_start)
dcl_vsf_peda_methods(static, __vk_aic1000a_capture_stop)
#endif

typedef struct vk_aic1000a_reg_seq_t {
    uint32_t reg;
    uint32_t mask;
    uint32_t value;
} vk_aic1000a_reg_seq_t;

/*============================ GLOBAL VARIABLES ==============================*/

const vk_audio_drv_t vk_aic1000a_drv = {
    .init       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_init),
};

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static const vk_audio_stream_drv_t __vk_aic1000a_stream_drv_playback = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_playback_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_playback_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_playback_stop),
};
#endif

#if VSF_AUDIO_CFG_USE_CATURE == ENABLED
static const vk_audio_stream_drv_t __vk_aic1000a_stream_drv_capture = {
    .control    = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_capture_control),
    .start      = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_capture_start),
    .stop       = (vsf_peda_evthandler_t)vsf_peda_func(__vk_aic1000a_capture_stop),
};
#endif

const static vk_aic1000a_reg_seq_t __vk_aic1000a_init_seq[] = {
    // clock config
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_pll_cfg0),
            AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_SDM_CLK_SEL
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_REG_VBIT(0xc)
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CP_IBIT(0x3)
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_PU_BIT(0x3)
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_DIV_SEL(0x0)
    ),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_pll_cfg1),
            AIC1000AUD_AUD_CTRL_REG_CFG_AUDPLL_SDM_FREQ_CFG(0x2C4EC4F)
    ),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_pll_cfg1),
            AIC1000AUD_AUD_CTRL_REG_CFG_AUDPLL_SDM_FREQ_CFG(0x2C4EC4F)
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUDPLL_SDM_FREQ_TOGGLE
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_pll_cfg0),
            AIC1000AUD_AUD_CTRL_REG_CFG_PU_AUPLL,
            AIC1000AUD_AUD_CTRL_REG_CFG_PU_AUPLL
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_pll_cfg0),
            AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK96M_EN
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK12M_EN
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK24M_EN,
            AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK96M_EN
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK12M_EN
        |   AIC1000AUD_AUD_CTRL_REG_CFG_AUPLL_CLK24M_EN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CTRL->aud_clk_ctrl0),
            AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_12M_GATE_EN
        |   AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_24M_GATE_EN
        |   AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_96M_GATE_EN,
            AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_12M_GATE_EN
        |   AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_24M_GATE_EN
        |   AIC1000AUD_AUD_CTRL_REG_I_REG_CLK_AUPLL2DIG_96M_GATE_EN
    ),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl0),
            AIC1000AUD_AUD_CODEC_CLKD_FSN(5)
        |   AIC1000AUD_AUD_CODEC_CLKD_FSN_LD
        |   AIC1000AUD_AUD_CODEC_CDIV_IIS(I2S_CDIV)
        |   AIC1000AUD_AUD_CODEC_HCYC_IIS0(I2S_HCYC_48K)
        |   AIC1000AUD_AUD_CODEC_HCYC_IIS1(I2S_HCYC_16K)
    ),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_WDG->WDG_CTRL),
            0x00000000
    ),
    // iomux config
    AIC1000A_REGW((unsigned int)&REG_AIC1000AUD_IOMUX->GPCFG[2], 2),
    AIC1000A_REGW((unsigned int)&REG_AIC1000AUD_IOMUX->GPCFG[4], 2),
    AIC1000A_REGW((unsigned int)&REG_AIC1000AUD_IOMUX->GPCFG[5], 2),
    AIC1000A_REGMW((unsigned int)&REG_AIC1000AUD_SYSCTRL->TPORTS_SEL,
            AIC1000AUD_SYS_CTRL_CFG_TOP_TPORTS_SEL(0x1E),
            AIC1000AUD_SYS_CTRL_CFG_TOP_TPORTS_SEL(0xFF)
    ),
    AIC1000A_REGMW((unsigned int)&REG_AIC1000AUD_SYSCTRL->TPORTS_SEL_2,
            (0x07 << 6) | (0x06 << 12) | (0x05 << 15),
            (0x07 << 6) | (0x07 << 12) | (0x07 << 15)
    ),
    AIC1000A_REGMW((unsigned int)&REG_AIC1000AUD_AUD_CODEC->intf_ctrl,
            AIC1000AUD_AUD_CODEC_TPORTS_SEL(0x1),
            AIC1000AUD_AUD_CODEC_TPORTS_SEL(0x3F)
    ),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[15]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[14]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[13]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[12]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[11]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[10]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[9]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[8]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[7]), 0x00010000),
    AIC1000A_REGW((unsigned int)&(REG_AIC1000AUD_IOMUX->GPCFG[6]), 0x00010000),
    // analog config
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->ana_clk1),
            AIC1000AUD_AUD_CODEC_HIGH_SDM_DAC_RFRSH(0x32)
        |   0
        |   AIC1000AUD_AUD_CODEC_CDIV_SDM_DAC_RFRSH(4)
        |   0
        |   AIC1000AUD_AUD_CODEC_CDIV_MIC_PGA(1)
        |   0
        |   AIC1000AUD_AUD_CODEC_CDIV_VAD_PGA_CHOP(3)
        |   0
        |   AIC1000AUD_AUD_CODEC_CDIV_VAD_PGA_RFRSH(3),
            AIC1000AUD_AUD_CODEC_HIGH_SDM_DAC_RFRSH(0x7F)
        |   AIC1000AUD_AUD_CODEC_CGEN_SDM_DAC_RFRSH
        |   AIC1000AUD_AUD_CODEC_CDIV_SDM_DAC_RFRSH(0x7)
        |   AIC1000AUD_AUD_CODEC_CGEN_MIC_PGA
        |   AIC1000AUD_AUD_CODEC_CDIV_MIC_PGA(0x7)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAD_PGA_CHOP
        |   AIC1000AUD_AUD_CODEC_CDIV_VAD_PGA_CHOP(0x7)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAD_PGA_RFRSH
        |   AIC1000AUD_AUD_CODEC_CDIV_VAD_PGA_RFRSH(0x7)
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_ivref),
            0
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_IBIAS_BIT(2)
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VREFLN_IBIT(8)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VREFLN_VBIT(8)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VCOM_MODE(2)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VCOM_SEL(9)
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VREFLN
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_IREFLN
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VREFB
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_IBIAS
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_BG,
            AIC1000AUD_AUD_CODEC_ABB_IVREF_VREFLP_IBIT
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_IBIAS_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_IBIAS_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VCOM_BIT
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VREFLN_IBIT(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VREFLN_VBIT(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VCOM_MODE(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_IVREF_VCOM_SEL(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VCOM_LP
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VREF_LP
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VREFLN
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_IREFLN
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_VREFB
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_IBIAS
        |   AIC1000AUD_AUD_CODEC_ABB_PU_IVREF_BG
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micpga_1),
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INSEL(0x12)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_IBIT(1)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INBIAS_P(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INBIAS_N(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_ICL_HPF_BIT
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_GAIN(2)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_RSTN,
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INSEL(0x3F)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_IBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INBIAS_P(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_INBIAS_N(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_ICL_HPF_BIT
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_ICL_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_GAIN(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_1_RSTN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micpga_2),
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INSEL(0x12)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_IBIT(1)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INBIAS_P(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INBIAS_N(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_ICL_HPF_BIT
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_GAIN(2)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_RSTN,
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INSEL(0x3F)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_IBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INBIAS_P(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_INBIAS_N(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_ICL_HPF_BIT
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_ICL_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_GAIN(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_2_RSTN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micpga_3),
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INSEL(0x12)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_IBIT(1)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INBIAS_P(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INBIAS_N(0)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_ICL_HPF_BIT
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_GAIN(2)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_RSTN,
            AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INSEL(0x3F)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_IBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INBIAS_P(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_INBIAS_N(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_ICL_HPF_BIT
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_ICL_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_GAIN(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICPGA_3_RSTN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micbias),
            AIC1000AUD_AUD_CODEC_ABB_MUX2MSADC_BIT(0)
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_CAL_BIT(4)
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_CC_BIT(3)
        |   0
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_VBIT(1)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_PGA_INBIAS_P(0)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_PGA_INBIAS_N(0)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_MICBIAS_BYP,
            AIC1000AUD_AUD_CODEC_ABB_MUX2MSADC_BIT(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_PU_MUX2MSADC
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_CAL_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_CC_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_HIZ_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_LP_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_LPF_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_MICBIAS_VBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_PGA_INBIAS_P(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_PGA_INBIAS_N(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_VAD_MICBIAS_BYP
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_adc0),
            AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_OP_IBIT(1)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_BIAS_IBIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DLY_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DWA_EN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VHI_BIT(3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VLO_BIT(3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VPS_BIT(2)
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_1_GAIN_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_2_GAIN_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_3_GAIN_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DIG_RSTN,
            AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_OP_IBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_BIAS_IBIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DLY_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DWA_EN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VHI_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VLO_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VPS_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VPS_DIS
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_VREG_BYPASS
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_1_GAIN_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_2_GAIN_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_3_GAIN_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_ADC_DIG_RSTN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_dac1),
            AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_RDAC_MODE
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_IBIAS_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VINT_BIT(2)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VHI_BIT(6)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VLO_BIT(1)
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_GAIN_BIT2(10)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_GAIN_BIT1(11)
        |   0
        |   0
        |   0
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_DIG_RSTN,
            AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_RDAC_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_RFRSH_EN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VCAL_EN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_IBIAS_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VINT_BIT(0x3)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VHI_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VLO_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_VREG_BYPASS
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_CLK_EDGE_SEL
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_GAIN_BIT2(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_GAIN_BIT1(0xF)
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_L_SE_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_R_SE_MODE
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_SE_MODE_DR
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_DIG_RSTN
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_l),
            AIC1000AUD_AUD_CODEC_ABB_HP_L_DACIN_SEL
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_IBIT(10)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_GAIN_BIT(5)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_DIFF_MODE,
            AIC1000AUD_AUD_CODEC_ABB_HP_L_DACIN_SEL
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_IBIT(0x1F)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_GAIN_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_L_DIFF_MODE
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_r),
            AIC1000AUD_AUD_CODEC_ABB_HP_R_DACIN_SEL
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_IBIT(10)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_GAIN_BIT(5)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_DIFF_MODE,
            AIC1000AUD_AUD_CODEC_ABB_HP_R_DACIN_SEL
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_IBIT(0x1F)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_GAIN_BIT(0x7)
        |   AIC1000AUD_AUD_CODEC_ABB_HP_R_DIFF_MODE
    ),
    AIC1000A_REGMW((unsigned int)&(REG_AIC1000AUD_AUD_CODEC->spk_gain),
            AIC1000AUD_AUD_CODEC_SPK_GAIN0(0x0340)
        |   AIC1000AUD_AUD_CODEC_SPK_GAIN1(0x0340),
            AIC1000AUD_AUD_CODEC_SPK_GAIN0(0x7FFF)
        |   AIC1000AUD_AUD_CODEC_SPK_GAIN1(0x7FFF)
    ),
    // TODO: mic_matrix_config
    // TODO: mic_signal_mode_config
};

/*============================ IMPLEMENTATION ================================*/

static void __vk_aic1000a_psi_output(vk_aic1000a_dev_t *dev, uint32_t value, uint8_t bits)
{
    uint32_t mask = 1 << (bits - 1);
    for (int i = 0; i < bits; i++) {
        if (value & mask) {
            vsf_gpio_set(dev->psi_port, 1 << dev->psi_dat_pin);
        } else {
            vsf_gpio_clear(dev->psi_port, 1 << dev->psi_dat_pin);
        }
        vsf_gpio_set(dev->psi_port, 1 << dev->psi_clk_pin);
        vsf_gpio_clear(dev->psi_port, 1 << dev->psi_clk_pin);
        value <<= 1;
    }
}

static uint32_t __vk_aic1000a_psi_input(vk_aic1000a_dev_t *dev, uint8_t bits)
{
    uint32_t value = 0, loop_value = 1 << dev->psi_dat_pin;
    uint8_t loop_cnt = 0, loop_round = 0;

    vsf_gpio_set(dev->psi_port, 1 << dev->psi_clk_pin);
    vsf_gpio_clear(dev->psi_port, 1 << dev->psi_clk_pin);

loop:
    while ((vsf_gpio_read(dev->psi_port) & (1 << dev->psi_dat_pin)) != loop_value) {
        vsf_gpio_set(dev->psi_port, 1 << dev->psi_clk_pin);
        vsf_gpio_clear(dev->psi_port, 1 << dev->psi_clk_pin);
        if (++loop_cnt >= 10) {
            vsf_trace_error("aic1000a: psi read timeout.\n");
            return 0;
        }
    }
    if (++loop_round < 3) {
        loop_value ^= 1 << dev->psi_dat_pin;
        goto loop;
    }

    for (int i = 0; i < bits; i++) {
        vsf_gpio_set(dev->psi_port, 1 << dev->psi_clk_pin);
        vsf_gpio_clear(dev->psi_port, 1 << dev->psi_clk_pin);
        value <<= 1;
        if (vsf_gpio_read(dev->psi_port) & (1 << dev->psi_dat_pin)) {
            value |= 1;
        }
    }
    return value;
}

static void __vk_aic1000a_reg_write(vk_aic1000a_dev_t *dev, uint32_t addr, uint32_t value)
{
    __vk_aic1000a_psi_output(dev, 2, 6);
    __vk_aic1000a_psi_output(dev, addr >> 1, 31);
    __vk_aic1000a_psi_output(dev, value, 32);
    __vk_aic1000a_psi_output(dev, 0, 4);
}

static uint32_t __vk_aic1000a_reg_read(vk_aic1000a_dev_t *dev, uint32_t addr)
{
    __vk_aic1000a_psi_output(dev, 2, 6);
    __vk_aic1000a_psi_output(dev, (addr >> 1) | 1, 31);
    vsf_gpio_set_input(dev->psi_port, 1 << dev->psi_dat_pin);
    uint32_t value = __vk_aic1000a_psi_input(dev, 32);
    vsf_gpio_set_output(dev->psi_port, 1 << dev->psi_dat_pin);
    __vk_aic1000a_psi_output(dev, 0, 5);
    return value;
}

static void __vk_aic1000a_reg_mask_write(vk_aic1000a_dev_t *dev, uint32_t addr, uint32_t value, uint32_t mask)
{
    __vk_aic1000a_reg_write(dev, addr, (__vk_aic1000a_reg_read(dev, addr) & ~mask) | (value & mask));
}

static void __vk_aic1000a_reg_seq(vk_aic1000a_dev_t *dev, const vk_aic1000a_reg_seq_t *seq, uint8_t num)
{
    for (int i = 0; i < num; i++, seq++) {
        if (0xFFFFFFFF == seq->mask) {
            __vk_aic1000a_reg_write(dev, seq->reg, seq->value);
        } else {
            __vk_aic1000a_reg_mask_write(dev, seq->reg, seq->value, seq->mask);
        }
    }
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_init, vk_audio_init)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);

    switch (evt) {
    case VSF_EVT_INIT:
        if (dev->is_inited) {
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }

        vsf_gpio_set_output(dev->pwrkey_port, 1 << dev->pwrkey_pin);
        vsf_gpio_set(dev->pwrkey_port, 1 << dev->pwrkey_pin);
        vsf_teda_set_timer_ms(5);
        break;
    case VSF_EVT_TIMER:
        vsf_gpio_clear(dev->pwrkey_port, 1 << dev->pwrkey_pin);
        vsf_gpio_set_output(dev->psi_port, (1 << dev->psi_clk_pin) | (1 << dev->psi_dat_pin));
        __vk_aic1000a_reg_seq(dev, __vk_aic1000a_init_seq, dimof(__vk_aic1000a_init_seq));
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_control, vk_audio_control)
{
    vsf_peda_begin();
    vsf_peda_end();
}

static bool __vk_aic1000a_playback_buffer(vk_aic1000a_dev_t *dev, uint8_t *buffer, uint_fast32_t size)
{
    vk_aic1000a_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_format_t *audio_format = &playback_ctx->audio_stream->format;
    if (playback_ctx->buffer_taken < dimof(playback_ctx->buffer)) {
        vk_aic1000a_playback_buffer_t *aic1000a_buffer =
            playback_ctx->fill_ticktock ? &playback_ctx->buffer[0] : &playback_ctx->buffer[1];

        vsf_protect_t orig = vsf_protect_int();
            playback_ctx->buffer_taken++;
        vsf_unprotect_int(orig);

        playback_ctx->fill_ticktock = !playback_ctx->fill_ticktock;

        memset(&aic1000a_buffer->timer, 0, sizeof(aic1000a_buffer->timer));
        aic1000a_buffer->timer.on_timer = __vk_aic1000a_ontimer;
        aic1000a_buffer->param = playback_ctx;
        uint_fast32_t nsamples = size / (audio_format->channel_num * audio_format->sample_bit_width >> 3);
        vsf_callback_timer_add_us(&aic1000a_buffer->timer, nsamples * 1000000 / audio_format->sample_rate);

        return true;
    }
    return false;
}

static void __vk_aic1000a_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    vk_audio_stream_t *audio_stream = param;
    vk_audio_stream_t *audio_stream_base = audio_stream - audio_stream->stream_index;
    vk_aic1000a_dev_t *dev = container_of(audio_stream_base, vk_aic1000a_dev_t, stream);
    vk_aic1000a_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    uint_fast32_t datasize;
    uint8_t *buff;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN:
        while (playback_ctx->is_playing && (playback_ctx->buffer_taken < dimof(playback_ctx->buffer))) {
            __vsf_aic1000a_trace(VSF_TRACE_DEBUG, "%d [aic1000a]: play stream evthandler\r\n", vsf_systimer_get_ms());
            datasize = vsf_stream_get_rbuf(stream, &buff);
            if (!datasize) { break; }

            if (__vk_aic1000a_playback_buffer(dev, buff, datasize)) {
                vsf_stream_read(stream, (uint8_t *)buff, datasize);
            }
        }
        break;
    }
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_aic1000a_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT:
        if (playback_ctx->is_playing) {
            VSF_AV_ASSERT(false);
        do_return_fail:
            vsf_eda_return(VSF_ERR_FAIL);
            return;
        }

        playback_ctx->audio_stream = audio_stream;
        playback_ctx->is_playing = true;
        playback_ctx->fill_ticktock = false;
        playback_ctx->buffer_taken = 0;
        audio_stream->stream->rx.param = audio_stream;
        audio_stream->stream->rx.evthandler = __vk_aic1000a_playback_evthandler;
        vsf_stream_connect_rx(audio_stream->stream);
        if (vsf_stream_get_data_size(audio_stream->stream)) {
            __vk_aic1000a_playback_evthandler(audio_stream->stream, audio_stream, VSF_STREAM_ON_IN);
        }

        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_aic1000a_playback_ctx_t *playback_ctx = &dev->playback_ctx;
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;

    switch (evt) {
    case VSF_EVT_INIT:
        playback_ctx->is_playing = false;
        // TODO: make sure play.stream will not be used
        audio_stream->stream = NULL;
        vsf_eda_return(VSF_ERR_NONE);
        break;
    }
    vsf_peda_end();
}
#endif

#endif
