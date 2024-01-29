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
#define __VSF_AUDIO_I2S_CLASS_INHERIT__
#define __VSF_AIC1000A_CLASS_IMPLEMENT

#include "service/vsf_service.h"
#include "./vsf_aic1000a.h"

#include "audio_types.h"
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
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
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

#if VSF_AUDIO_USE_CAPTURE == ENABLED
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

#if VSF_AUDIO_USE_CAPTURE == ENABLED
static const uint8_t __aic1000a_mic_matrix_tbl[6][3] = {
    {0, 1, 2},
    {0, 2, 1},
    {1, 0, 2},
    {1, 2, 0},
    {2, 0, 1},
    {2, 1, 0}
};
#endif

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
    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);

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

        vsf_err_t err;
        dev->stream_num = 0;
        __vsf_component_call_peda_ifs(vk_audio_init, err, vk_audio_i2s_drv.init, 0, &dev->use_as__vk_audio_dev_t);
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        dev->stream_num = 0;
#if VSF_AUDIO_USE_PLAYBACK == ENABLED
        dev->stream[dev->stream_num].drv = &__vk_aic1000a_stream_drv_playback;
        dev->stream_num++;
        memset(&dev->dac, 0, sizeof(dev->dac));
#endif
#if VSF_AUDIO_USE_CAPTURE == ENABLED
        dev->stream[dev->stream_num].drv = &__vk_aic1000a_stream_drv_capture;
        dev->stream_num++;
        memset(&dev->adc, 0, sizeof(dev->adc));
#endif
        vsf_eda_return(vsf_eda_get_return_value());
    }
    vsf_peda_end();
}

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
static void __vk_aic1000a_clear_spk_mem(vk_aic1000a_dev_t *dev)
{
    if (!dev->dac.mem_cleared) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl),
            AIC1000AUD_AUD_CODEC_SPK_MEM_CLKSEL, AIC1000AUD_AUD_CODEC_SPK_MEM_CLKSEL);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl),
            AIC1000AUD_AUD_CODEC_SPK_MEM_CLR, AIC1000AUD_AUD_CODEC_SPK_MEM_CLR);
        while(!((__vk_aic1000a_reg_read(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl))) & AIC1000AUD_AUD_CODEC_SPK_MEM_CLR_DONE));
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl), 0, AIC1000AUD_AUD_CODEC_SPK_MEM_CLR);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl), 0, AIC1000AUD_AUD_CODEC_SPK_MEM_CLKSEL);
        dev->dac.mem_cleared = true;
    }
}

static void __vk_aic1000_dac_pu(vk_aic1000a_dev_t *dev, uint8_t ch_map)
{
    if ((dev->dac.ch_ana_pu & ch_map) == ch_map) {
        return;
    }

    uint8_t l_pu = (ch_map & AUD_CH_MAP_CH_0) ? 1 : 0;
    uint8_t r_pu = (ch_map & AUD_CH_MAP_CH_1) ? 1 : 0;

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_dac1),
            AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_DIG_RSTN
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_R * r_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_L * l_pu),
            AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_ANA_RSTN
        |   AIC1000AUD_AUD_CODEC_ABB_SDM_DAC_DIG_RSTN
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_R * r_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_L * l_pu)
    );

    if (l_pu) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_l),
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_BIAS,
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_BIAS);
    }

    if (r_pu) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_r),
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_BIAS,
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_BIAS);
    }

    dev->dac.ch_ana_pu |= ch_map;
}

static void __vk_aic1000_dac_pd(vk_aic1000a_dev_t *dev, uint8_t ch_map)
{
    if ((dev->dac.ch_ana_pu & ch_map) == 0) {
        return;
    }

    uint8_t l_pd = (ch_map & AUD_CH_MAP_CH_0) ? 1 : 0;
    uint8_t r_pd = (ch_map & AUD_CH_MAP_CH_1) ? 1 : 0;

    if (l_pd) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_l),
            0 | 0,
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_L_BIAS);
    }

    if (r_pd) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_hp_r),
            0 | 0,
            AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_MPATH | AIC1000AUD_AUD_CODEC_ABB_PU_HP_R_BIAS);
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_dac1),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_R * (1 - r_pd))
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_L * (1 - l_pd)),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_R * r_pd)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_DAC_L * l_pd)
    );

    dev->dac.ch_ana_pu &= ~ch_map;
}

void __vk_aic1000_dac_config(vk_aic1000a_dev_t *dev, vk_audio_format_t *format, uint8_t channel_mask)
{
    if ((dev->dac.ch_en & channel_mask) != 0) {
        return;
    }

    if (1 == format->channel_num) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->dac_sdm_ctrl),
                AIC1000AUD_AUD_CODEC_DAC_SDM_MUX, AIC1000AUD_AUD_CODEC_DAC_SDM_MUX);
    } else {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->dac_sdm_ctrl),
                0,  AIC1000AUD_AUD_CODEC_DAC_SDM_MUX);
    }
}

void __vk_aic1000_dac_start(vk_aic1000a_dev_t *dev, vk_audio_stream_t *audio_stream, uint8_t channel_mask)
{
    uint8_t spk_sync_en = 0;
#if VSF_AUDIO_USE_CAPTURE == ENABLED
    uint8_t adc_ch_en = dev->adc.ch_en;
#else
    uint8_t adc_ch_en = 0;
#endif
    uint8_t i2s0_trig_sel = !(adc_ch_en & (AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1));

    if ((dev->dac.ch_en & channel_mask) == channel_mask) {
        return;
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl0),
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_SPK_DAC_EN(channel_mask)
        |   (AIC1000AUD_AUD_CODEC_SPK_SYNC_EN * spk_sync_en)
        |   0,
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_SPK_DAC_EN(3)
        |   AIC1000AUD_AUD_CODEC_SPK_SYNC_EN
        |   AIC1000AUD_AUD_CODEC_ADDA_DLY_EN
    );
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl1),
            AIC1000AUD_AUD_CODEC_IIS0_IDAT_EN
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL * i2s0_trig_sel)
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN,
            AIC1000AUD_AUD_CODEC_IIS0_IDAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN
    );
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl0),
        AIC1000AUD_AUD_CODEC_IIS_EN, AIC1000AUD_AUD_CODEC_IIS_EN);
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->clk_ctrl),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80)
    );
    dev->dac.ch_en |= channel_mask;
}

void __vk_aic1000_dac_stop(vk_aic1000a_dev_t *dev, vk_audio_stream_t *audio_stream, uint8_t channel_mask)
{
    uint8_t i2s0_trig_sel = 0;
#if VSF_AUDIO_USE_CAPTURE == ENABLED
    uint8_t adc_ch_en = dev->adc.ch_en;
#else
    uint8_t adc_ch_en = 0;
#endif
    uint8_t i2s0_en = !!(dev->adc.ch_en & (AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1));

    if ((dev->dac.ch_en & channel_mask) == 0) {
        return;
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl1),
            0
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL * i2s0_trig_sel)
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN * i2s0_en),
            AIC1000AUD_AUD_CODEC_IIS0_IDAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN
    );
    if (adc_ch_en == 0) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl0),
            0, AIC1000AUD_AUD_CODEC_IIS_EN);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->clk_ctrl),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80));
    }
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl0),
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_SPK_DAC_EN(0)
        |   0
        |   0,
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_SPK_DAC_EN(3)
        |   AIC1000AUD_AUD_CODEC_SPK_SYNC_EN
        |   AIC1000AUD_AUD_CODEC_ADDA_DLY_EN
    );

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->clk_ctrl),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x0c60)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0x0c60),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x0c60)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0x0c60)
    );
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->clk_ctrl),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x0c60)
        |   AIC1000AUD_AUD_CODEC_CGEN_VAL(0x0c60)
    );

    dev->dac.ch_en &= ~channel_mask;
    dev->dac.mem_cleared = false;
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_control, vk_audio_control)
{
    vsf_peda_begin();

    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    uint16_t gain;

    switch (vsf_local.type) {
    case VSF_AUDIO_CTRL_MUTE:
        gain = 0;
        goto apply_gain;
    case VSF_AUDIO_CTRL_VOLUME_DB:
        // db: [-58db, +31db]
        // TODO: add support by setting gain and goto apply_gain
        VSF_AV_ASSERT(false);
        goto apply_gain;
    case VSF_AUDIO_CTRL_VOLUME_PERCENTAGE:
        // gain: [0, 0x7FFF]
        gain = vsf_local.value.uval16 >> 1;

    apply_gain: {
            uint8_t l = (dev->dac.ch_en & AUD_CH_MAP_CH_0) ? 1 : 0;
            uint8_t r = (dev->dac.ch_en & AUD_CH_MAP_CH_1) ? 1 : 0;

            __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->spk_gain),
                    ((l * AIC1000AUD_AUD_CODEC_SPK_GAIN0(gain))
                |   (r * AIC1000AUD_AUD_CODEC_SPK_GAIN1(gain))),
                    ((l * AIC1000AUD_AUD_CODEC_SPK_GAIN0(0x7FFF))
                |   (r * AIC1000AUD_AUD_CODEC_SPK_GAIN1(0x7FFF)))
            );
        }
        break;
    }
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;
    uint16_t sample_rate = audio_stream->format.sample_rate;
    uint8_t bitlen = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value);

    switch (evt) {
    case VSF_EVT_INIT:
        if (NULL == audio_stream->stream) {
            vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
            break;
        }
        if (dev->dac.is_started) {
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        if (    !channel_num || (channel_num > 2)
            ||  ((sample_rate > 480) && (sample_rate != 960))
            ||  ((bitlen != 16) && (bitlen != 24))) {
            vsf_eda_return(VSF_ERR_NOT_SUPPORT);
            return;
        }

        {
            uint8_t channel_mask = (1 == channel_num) ?
                AUD_CH_MAP_CH_0 : AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1;
            __vk_aic1000a_clear_spk_mem(dev);
            __vk_aic1000_dac_pu(dev, channel_mask);
            __vk_aic1000_dac_config(dev, &audio_stream->format, channel_mask);
            __vk_aic1000_dac_start(dev, audio_stream, channel_mask);
        }
        dev->dac.is_started = true;

        vsf_err_t err;

        __vsf_component_call_peda_ifs(vk_audio_start, err, vk_audio_i2s_stream_drv_playback.start, 0, &dev->use_as__vk_audio_dev_t,
            .audio_stream   = audio_stream,
        );
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_playback_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!channel_num || (channel_num > 2)) {
            vsf_eda_return(VSF_ERR_NOT_SUPPORT);
            return;
        }

        {
            uint8_t channel_mask = (1 == channel_num) ?
                AUD_CH_MAP_CH_0 : AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1;
            __vk_aic1000_dac_stop(dev, audio_stream, channel_mask);
            __vk_aic1000_dac_pd(dev, channel_mask);
        }

        vsf_err_t err;
        __vsf_component_call_peda_ifs(vk_audio_stop, err, vk_audio_i2s_stream_drv_playback.stop, 0, &dev->use_as__vk_audio_dev_t,
            .audio_stream   = audio_stream,
        );
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}
#endif

#if VSF_AUDIO_USE_CAPTURE == ENABLED
static void __vk_aic1000a_clear_mic_mem(vk_aic1000a_dev_t *dev)
{
    if (!dev->adc.mem_cleared) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl),
            AIC1000AUD_AUD_CODEC_MIC_MEM_CLKSEL, AIC1000AUD_AUD_CODEC_MIC_MEM_CLKSEL);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl),
            AIC1000AUD_AUD_CODEC_MIC_MEM_CLR, AIC1000AUD_AUD_CODEC_MIC_MEM_CLR);
        while(!((__vk_aic1000a_reg_read(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl))) & AIC1000AUD_AUD_CODEC_MIC_MEM_CLR_DONE));
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl), 0, AIC1000AUD_AUD_CODEC_MIC_MEM_CLR);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->mem_ctrl), 0, AIC1000AUD_AUD_CODEC_MIC_MEM_CLKSEL);
        dev->adc.mem_cleared = true;
    }
}

static void __vk_aic1000_adc_pu(vk_aic1000a_dev_t *dev, uint8_t ch_map)
{
    if ((dev->adc.ch_ana_pu & ch_map) == ch_map) {
        return;
    }

    uint8_t adc1_pu = 0;
    uint8_t adc2_pu = 0;
    uint8_t adc3_pu = 0;

    if (ch_map & AUD_CH_MAP_CH_0) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][0]) {
        case 0:     adc1_pu = 1;    break;
        case 1:     adc2_pu = 1;    break;
        case 2:     adc3_pu = 1;    break;
        default:                    break;
        }
    }

    if (ch_map & AUD_CH_MAP_CH_1) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][1]) {
        case 0:     adc1_pu = 1;    break;
        case 1:     adc2_pu = 1;    break;
        case 2:     adc3_pu = 1;    break;
        default:                    break;
        }
    }

    if (ch_map & AUD_CH_MAP_CH_2) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][2]) {
        case 0:     adc1_pu = 1;    break;
        case 1:     adc2_pu = 1;    break;
        case 2:     adc3_pu = 1;    break;
        default:                    break;
        }
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micbias),
        AIC1000AUD_AUD_CODEC_ABB_PU_MICBIAS, AIC1000AUD_AUD_CODEC_ABB_PU_MICBIAS);

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micpga_1),
            (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_3 * adc3_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_2 * adc2_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_1 * adc1_pu),
            (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_3 * adc3_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_2 * adc2_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_1 * adc1_pu)
    );

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_adc0),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_1 * adc1_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_2 * adc2_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_3 * adc3_pu),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_1 * adc1_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_2 * adc2_pu)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_3 * adc3_pu)
    );

    dev->adc.ch_ana_pu |= ch_map;
}

static void __vk_aic1000_adc_pd(vk_aic1000a_dev_t *dev, uint8_t ch_map)
{
    if ((dev->adc.ch_ana_pu & ch_map) == 0) {
        return;
    }

    uint8_t adc1_pd = 0;
    uint8_t adc2_pd = 0;
    uint8_t adc3_pd = 0;

    if (ch_map & AUD_CH_MAP_CH_0) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][0]) {
        case 0:     adc1_pd = 1;    break;
        case 1:     adc2_pd = 1;    break;
        case 2:     adc3_pd = 1;    break;
        default:                    break;
        }
    }

    if (ch_map & AUD_CH_MAP_CH_1) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][1]) {
        case 0:     adc1_pd = 1;    break;
        case 1:     adc2_pd = 1;    break;
        case 2:     adc3_pd = 1;    break;
        default:                    break;
        }
    }

    if (ch_map & AUD_CH_MAP_CH_2) {
        switch (__aic1000a_mic_matrix_tbl[dev->adc.mic_matrix_type][2]) {
        case 0:     adc1_pd = 1;    break;
        case 1:     adc2_pd = 1;    break;
        case 2:     adc3_pd = 1;    break;
        default:                    break;
        }
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_sdm_adc0),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_1 * (1 - adc1_pd))
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_2 * (1 - adc2_pd))
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_3 * (1 - adc3_pd)),
            (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_1 * adc1_pd)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_2 * adc2_pd)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_SDM_ADC_3 * adc3_pd)
    );

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micpga_1),
            (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_3 * (1 - adc3_pd))
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_2 * (1- adc2_pd))
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_1 * (1 - adc1_pd)),
            (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_3 * adc3_pd)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_2 * adc2_pd)
        |   (AIC1000AUD_AUD_CODEC_ABB_PU_MICPGA_1 * adc1_pd)
    );

    dev->adc.ch_ana_pu &= ~ch_map;

    if (0 == dev->adc.ch_ana_pu) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->abb_micbias),
            0, AIC1000AUD_AUD_CODEC_ABB_PU_MICBIAS);
    }
}

void __vk_aic1000_adc_config(vk_aic1000a_dev_t *dev, vk_audio_format_t *format, uint8_t channel_mask)
{
    if ((dev->adc.ch_en & channel_mask) != 0) {
        return;
    }

    uint8_t d36_en = 0, d36_d3;
    uint8_t i2s1_hcyc = 0;

    if (format->sample_rate != 480) {
        dev->adc.ch_d36_en |= channel_mask;
        if (format->sample_rate == 160) {
            dev->adc.ch_d36_d3 |= channel_mask;
        } else {
            dev->adc.ch_d36_d3 &= ~channel_mask;
        }
    } else {
        dev->adc.ch_d36_en &= ~channel_mask;
    }

    d36_d3 = dev->adc.ch_d36_d3 != 0;

    switch (dev->adc.ch_d36_en) {
    case 0:                                                     d36_en = 0; break;
    case AUD_CH_MAP_CH_0:                                       d36_en = 1; break;
    case AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1:                     d36_en = 3; break;
    case AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1 | AUD_CH_MAP_CH_2:   d36_en = 2; break;
    default:
        VSF_AV_ASSERT(false);
        return;
    }

    if ((dev->adc.ch_en | channel_mask) == (AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1 | AUD_CH_MAP_CH_2)) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl1),
            AIC1000AUD_AUD_CODEC_MIC_ANC_P0(2) | AIC1000AUD_AUD_CODEC_MIC_ANC_P1(2),
            AIC1000AUD_AUD_CODEC_MIC_ANC_P0(3) | AIC1000AUD_AUD_CODEC_MIC_ANC_P1(3));
    }
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl1),
        AIC1000AUD_AUD_CODEC_MIC_D36_D3 * d36_d3, AIC1000AUD_AUD_CODEC_MIC_D36_D3);
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl0),
        AIC1000AUD_AUD_CODEC_AUD_CFG_POS | AIC1000AUD_AUD_CODEC_MIC_D36_EN(d36_en),
        AIC1000AUD_AUD_CODEC_AUD_CFG_POS | AIC1000AUD_AUD_CODEC_MIC_D36_EN(3));

    if ((channel_mask & (AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1)) != 0) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl1),
            AIC1000AUD_AUD_CODEC_IIS0_ODAT_SEL(2), AIC1000AUD_AUD_CODEC_IIS0_ODAT_SEL(3));
    }
    if ((channel_mask & AUD_CH_MAP_CH_2) != 0) {
        switch (format->sample_rate) {
        case 80:        i2s1_hcyc = I2S_HCYC_8K;    break;
        case 160:       i2s1_hcyc = I2S_HCYC_16K;   break;
        case 480:       i2s1_hcyc = I2S_HCYC_48K;   break;
        default:        VSF_AV_ASSERT(false);       return;
        }
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl0),
            AIC1000AUD_AUD_CODEC_HCYC_IIS1(i2s1_hcyc),
            AIC1000AUD_AUD_CODEC_HCYC_IIS1(0xFF));
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl1),
            AIC1000AUD_AUD_CODEC_IIS1_ODAT_SEL(3),
            AIC1000AUD_AUD_CODEC_IIS1_ODAT_SEL(3));
    }
}

void __vk_aic1000_adc_start(vk_aic1000a_dev_t *dev, vk_audio_stream_t *audio_stream, uint8_t channel_mask)
{
    if ((dev->adc.ch_en & channel_mask) == channel_mask) {
        return;
    }

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
    uint8_t dac_ch_en = dev->dac.ch_en;
#else
    uint8_t dac_ch_en = 0;
#endif
    uint8_t adc_en = 0, anc_en = 0, tra_en = 0;
    uint8_t i2s0_en = 0, i2s1_en = 0;
    uint8_t i2s0_odata_en = 0, i2s1_odata_en = 0;
    uint8_t i2s0_trig_sel = 0, i2s1_trig_sel = 0;

    switch (dev->adc.ch_en | channel_mask) {
    case AUD_CH_MAP_CH_0:
        adc_en = 1;
        tra_en = 1;
        anc_en = 0;
        i2s0_en = 1;
        i2s1_en = 0;
        i2s0_odata_en = 1;
        i2s1_odata_en = 0;
        break;
    case AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1:
        adc_en = 3;
        tra_en = 3;
        anc_en = 0;
        i2s0_en = 1;
        i2s1_en = 0;
        i2s0_odata_en = 1;
        i2s1_odata_en = 0;
        break;
    case AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1 | AUD_CH_MAP_CH_2:
        adc_en = 7;
        tra_en = 3;
        anc_en = 1;
        i2s0_en = 1;
        i2s1_en = 1;
        i2s0_odata_en = 1;
        i2s1_odata_en = 1;
        break;
    default:
        VSF_AV_ASSERT(false);
        return;
    }

    if (dac_ch_en != 0) {
        i2s0_en = 1;
        i2s0_trig_sel = 1;
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->core_ctrl0),
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_MIC_ADC_EN(adc_en)
        |   AIC1000AUD_AUD_CODEC_MIC_ANC_EN(anc_en)
        |   AIC1000AUD_AUD_CODEC_MIC_TRA_EN(tra_en)
        |   0,
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_MIC_ADC_EN(7)
        |   AIC1000AUD_AUD_CODEC_MIC_ANC_EN(3)
        |   AIC1000AUD_AUD_CODEC_MIC_TRA_EN(3)
        |   AIC1000AUD_AUD_CODEC_ADDA_DLY_EN
    );
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl1),
            (AIC1000AUD_AUD_CODEC_IIS0_ODAT_EN * i2s0_odata_en)
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL * i2s0_trig_sel)
        |   (AIC1000AUD_AUD_CODEC_IIS1_ODAT_EN * i2s1_odata_en)
        |   (AIC1000AUD_AUD_CODEC_IIS1_TRIG_SEL * i2s1_trig_sel)
        |   (AIC1000AUD_AUD_CODEC_IIS1_TRIG_EN * i2s1_en)
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN * i2s0_en),
            AIC1000AUD_AUD_CODEC_IIS0_ODAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS1_ODAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS1_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS1_TRIG_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN
    );
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->iis_ctrl0),
        AIC1000AUD_AUD_CODEC_IIS_EN, AIC1000AUD_AUD_CODEC_IIS_EN);
    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(REG_AIC1000AUD_AUD_CODEC->clk_ctrl),
        AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80),
        AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80)
    );

    dev->adc.ch_en |= channel_mask;
}

void __vk_aic1000_adc_stop(vk_aic1000a_dev_t *dev, vk_audio_stream_t *audio_stream, uint8_t channel_mask)
{
    if ((dev->adc.ch_en & channel_mask) == 0) {
        return;
    }

#if VSF_AUDIO_USE_PLAYBACK == ENABLED
    uint8_t dac_ch_en = dev->dac.ch_en;
#else
    uint8_t dac_ch_en = 0;
#endif
    uint8_t adc_en = 0, anc_en = 0, tra_en = 0;
    uint8_t i2s0_en = 0, i2s1_en = 0;
    uint8_t i2s0_odata_en = 0, i2s1_odata_en = 0;
    uint8_t i2s0_trig_sel = 0, i2s1_trig_sel = 0;

    switch (dev->adc.ch_en & (~channel_mask)) {
    case 0:
        adc_en = 0;
        anc_en = 0;
        tra_en = 0;
        i2s0_en = 0;
        i2s1_en = 0;
        i2s0_odata_en = 0;
        i2s1_odata_en = 0;
        break;
    case AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1:
        adc_en = 3;
        anc_en = 0;
        tra_en = 3;
        i2s0_en = 1;
        i2s1_en = 0;
        i2s0_odata_en = 1;
        i2s1_odata_en = 0;
        break;
    default:
        VSF_AV_ASSERT(false);
        return;
    }

    if (dac_ch_en != 0) {
        i2s0_en = 1;
        i2s0_trig_sel = 1;
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->iis_ctrl1),
            (AIC1000AUD_AUD_CODEC_IIS0_ODAT_EN * i2s0_odata_en)
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL * i2s0_trig_sel)
        |   (AIC1000AUD_AUD_CODEC_IIS1_ODAT_EN * i2s1_odata_en)
        |   (AIC1000AUD_AUD_CODEC_IIS1_TRIG_SEL * i2s1_trig_sel)
        |   (AIC1000AUD_AUD_CODEC_IIS1_TRIG_EN * i2s1_en)
        |   (AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN * i2s0_en),
            AIC1000AUD_AUD_CODEC_IIS0_ODAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS1_ODAT_EN
        |   AIC1000AUD_AUD_CODEC_IIS1_TRIG_SEL
        |   AIC1000AUD_AUD_CODEC_IIS1_TRIG_EN
        |   AIC1000AUD_AUD_CODEC_IIS0_TRIG_EN
    );

    if ((i2s0_en == 0) && (i2s1_en == 0)) {
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->iis_ctrl0),
            0, AIC1000AUD_AUD_CODEC_IIS_EN);
        __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->clk_ctrl),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0),
            AIC1000AUD_AUD_CODEC_CGEN_FIX(0x80) | AIC1000AUD_AUD_CODEC_CGEN_VAL(0x80));
    }

    __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->core_ctrl0),
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_MIC_ADC_EN(adc_en)
        |   AIC1000AUD_AUD_CODEC_MIC_ANC_EN(anc_en)
        |   AIC1000AUD_AUD_CODEC_MIC_TRA_EN(tra_en)
        |   0,
            AIC1000AUD_AUD_CODEC_AUD_CFG_POS
        |   AIC1000AUD_AUD_CODEC_MIC_ADC_EN(7)
        |   AIC1000AUD_AUD_CODEC_MIC_ANC_EN(3)
        |   AIC1000AUD_AUD_CODEC_MIC_TRA_EN(3)
        |   AIC1000AUD_AUD_CODEC_ADDA_DLY_EN
    );

    dev->adc.ch_en &= ~channel_mask;
    dev->adc.ch_d36_en &= ~audio_stream->format.channel_num;
    dev->adc.ch_d36_d3 &= ~audio_stream->format.channel_num;

    if (dev->adc.ch_en == 0) {
        dev->adc.mem_cleared = false;
    }
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_capture_control, vk_audio_control)
{
    vsf_peda_begin();

    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    uint8_t gain;

    switch (vsf_local.type) {
    case VSF_AUDIO_CTRL_MUTE:
        gain = 0;
        goto apply_gain;
    case VSF_AUDIO_CTRL_VOLUME_DB:
        // db: [-30db, +18db]
        // TODO: add support by setting gain and goto apply_gain
        VSF_AV_ASSERT(false);
        goto apply_gain;
    case VSF_AUDIO_CTRL_VOLUME_PERCENTAGE:
        // gain: [0, 0xFF]
        gain = vsf_local.value.uval16 >> 8;

    apply_gain: {
            uint8_t adc_1 = (dev->adc.ch_en & AUD_CH_MAP_CH_0) ? 1 : 0;
            uint8_t adc_2 = (dev->adc.ch_en & AUD_CH_MAP_CH_1) ? 1 : 0;
            uint8_t adc_3 = (dev->adc.ch_en & AUD_CH_MAP_CH_2) ? 1 : 0;

            __vk_aic1000a_reg_mask_write(dev, (unsigned int)&(aic1000audAudCodec->mic_gain),
                    ((adc_1 * AIC1000AUD_AUD_CODEC_MIC_GAIN0(gain))
                |   (adc_2 * AIC1000AUD_AUD_CODEC_MIC_GAIN1(gain))
                |   (adc_3 * AIC1000AUD_AUD_CODEC_MIC_GAIN2(gain))),
                    ((adc_1 * AIC1000AUD_AUD_CODEC_MIC_GAIN0(gain))
                |   (adc_2 * AIC1000AUD_AUD_CODEC_MIC_GAIN1(gain))
                |   (adc_3 * AIC1000AUD_AUD_CODEC_MIC_GAIN2(gain)))
            );
        }
        break;
    }
    vsf_eda_return(VSF_ERR_NONE);
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_capture_start, vk_audio_start)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;
    uint16_t sample_rate = audio_stream->format.sample_rate;
    uint8_t bitlen = VSF_AUDIO_DATA_TYPE_BITLEN(audio_stream->format.datatype.value);

    switch (evt) {
    case VSF_EVT_INIT:
        if (NULL == audio_stream->stream) {
            vsf_eda_return(VSF_ERR_INVALID_PARAMETER);
            break;
        }
        if (dev->adc.is_started) {
            vsf_eda_return(VSF_ERR_NONE);
            break;
        }
        if (    !channel_num || (channel_num > 2)
            ||  ((sample_rate != 80) && (sample_rate != 160) && (sample_rate != 480))
            ||  ((bitlen != 16) && (bitlen != 24))) {
            vsf_eda_return(VSF_ERR_NOT_SUPPORT);
            return;
        }

        {
            uint8_t channel_mask = (1 == channel_num) ?
                AUD_CH_MAP_CH_0 : AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1;
            __vk_aic1000a_clear_mic_mem(dev);
            __vk_aic1000_adc_pu(dev, channel_mask);
            __vk_aic1000_adc_config(dev, &audio_stream->format, channel_mask);
            __vk_aic1000_adc_start(dev, audio_stream, channel_mask);
        }
        dev->adc.is_started = true;

        vsf_err_t err;
        dev->capture.hw_sample_rate = dev->capture.sample_rate <= 480 ? 480 : 960;
        __vsf_component_call_peda_ifs(vk_audio_start, err, vk_audio_i2s_stream_drv_capture.start, 0, &dev->use_as__vk_audio_dev_t,
            .audio_stream   = audio_stream,
        );
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}

__vsf_component_peda_ifs_entry(__vk_aic1000a_capture_stop, vk_audio_stop)
{
    vsf_peda_begin();
    vk_aic1000a_dev_t *dev = vsf_container_of(&vsf_this, vk_aic1000a_dev_t, use_as__vk_audio_dev_t);
    vk_audio_stream_t *audio_stream = vsf_local.audio_stream;
    uint8_t channel_num = audio_stream->format.channel_num;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!channel_num || (channel_num > 2)) {
            vsf_eda_return(VSF_ERR_NOT_SUPPORT);
            return;
        }

        {
            uint8_t channel_mask = (1 == channel_num) ?
                AUD_CH_MAP_CH_0 : AUD_CH_MAP_CH_0 | AUD_CH_MAP_CH_1;
            __vk_aic1000_adc_stop(dev, audio_stream, channel_mask);
            __vk_aic1000_adc_pd(dev, channel_mask);
        }

        vsf_err_t err;
        __vsf_component_call_peda_ifs(vk_audio_stop, err, vk_audio_i2s_stream_drv_capture.stop, 0, &dev->use_as__vk_audio_dev_t,
            .audio_stream   = audio_stream,
        );
        if (err != VSF_ERR_NONE) {
            vsf_eda_return(err);
        }
        break;
    case VSF_EVT_RETURN:
        vsf_eda_return(vsf_eda_get_return_value());
        break;
    }
    vsf_peda_end();
}
#endif

#endif
