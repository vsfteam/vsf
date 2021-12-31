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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED

#define __VSF_ADC_CLASS_IMPLEMENT

#include "./adc.h"
#include "../gpio/gpio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_AIC8800_ADC_CFG_BIT_COUNT
#   define VSF_AIC8800_ADC_CFG_BIT_COUNT                        10
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_ADC_IMP_LV0(__COUNT, __dont_care)                              \
    vsf_adc_t vsf_adc##__COUNT = {                                              \
        .data = NULL,                                                           \
        .channel_count = 0,                                                     \
        .channel_index = 0,                                                     \
        .current_channel = NULL,                                                \
        .status = {                                                             \
            .is_enable = false,                                                 \
            .is_busy = false,                                                   \
            .is_irq = false,                                                    \
        },                                                                      \
    };

#define aic8800_adc_def(__count)                                                \
    VSF_MREPEAT(__count, __VSF_HW_ADC_IMP_LV0, NULL)

/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

aic8800_adc_def(1)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vsf_adc_get_callback_time_us(vsf_adc_t *adc_ptr)
{
    return 26000000 / adc_ptr->cfg.clock_freq * 20 + 6 + VSF_ADC_CFG_CALLBACK_TIME_POSTPONE_US;
}

static void __vsf_adc_measure(int type)
{
    if (type == ADC_REF_VDD_1) {
#if PLF_PMIC_VER_LITE
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg1),
            (AIC1000LITE_RTC_CORE_RTC_RG_PU_VRTC_SENSE
                |   AIC1000LITE_RTC_CORE_RTC_RG_PU_VBAT_SENSE),
            (AIC1000LITE_RTC_CORE_RTC_RG_PU_VRTC_SENSE
                |   AIC1000LITE_RTC_CORE_RTC_RG_PU_VBAT_SENSE));

        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xD)
                |   AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF)
                |   AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));

        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            (0  |   0
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN));

        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);
#endif
    } else if (type == ADC_REF_VDD_1_2) {
        VSF_HAL_ASSERT(false); //TODO:
    } else if (type == ADC_REF_VDD_1_3) {
#if PLF_PMIC_VER_LITE
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0x0)
                |   AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF)
                |   AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            (0 | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE | 0 | 0 | 0),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL
                | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN));

        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);
#endif
    }
}

static void __vsf_adc_init(vsf_adc_t *adc_ptr, adc_cfg_t *cfg_ptr)
{
    uint32_t temp_clock_div;
    //todo:cfg_ptr
    __vsf_adc_measure(adc_ptr->cfg.feature & ADC_REF_VDD);
#if PLF_PMIC_VER_LITE
    if (101960 > adc_ptr->cfg.clock_freq) {
        adc_ptr->cfg.clock_freq = 101960;
    }else if(13000000 < adc_ptr->cfg.clock_freq) {
        adc_ptr->cfg.clock_freq = 13000000;
    }
    temp_clock_div = 26000000 / adc_ptr->cfg.clock_freq;
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteSysctrl->msadc_clk_div),
            AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_DENOM(temp_clock_div)
        |   AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_UPDATE);
#endif
    vsf_callback_timer_init(&adc_ptr->callback_timer);
}

static void __vsf_adc_channel_config(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfgs_ptr)
{
    uint8_t channel = channel_cfgs_ptr->channel;
    if (channel <= 7) {
#if PLF_PMIC_VER_LITE
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_mode), 0);
        if ((channel <= 1) || (channel == 13)) {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->gpio_ctrl1),
                (0x01 << channel), (0x01 << channel));
        } else {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg2),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel - 2)),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel - 2)));
        }
#endif

        vsf_hw_gpio_config_pin((vsf_gpio_t *)&vsf_gpio0, 1 << (channel + 16), 0);
        vsf_hw_gpio_set_input((vsf_gpio_t *)&vsf_gpio0, 1 << (channel + 16));

#if PLF_PMIC_VER_LITE
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[channel]),
            (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC),
            (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC));
#endif
    }
}

static vsf_err_t __vsf_adc_channel_request(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfg_ptr)
{
    if (channel_cfg_ptr->channel <= 7) {
#if PLF_PMIC_VER_LITE
        int neg_flag = channel_cfg_ptr->channel & 0x01;
        unsigned int mux_bit = 7 - (channel_cfg_ptr->channel >> 1);

        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            ((neg_flag ? 0 : AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL)
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT
                |   AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN));

        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(mux_bit) | 0),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF)
                |   AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));

        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);

        vsf_callback_timer_add_us(&adc_ptr->callback_timer, __vsf_adc_get_callback_time_us(adc_ptr));
        return VSF_ERR_NONE;
#endif
    }
    return VSF_ERR_INVALID_RANGE;
}

static void __vk_adc_on_time(vsf_callback_timer_t *timer)
{
    vsf_adc_t *adc_ptr = container_of(timer, vsf_adc_t, callback_timer);
    unsigned int gpmsk = 0x01UL << adc_ptr->current_channel->channel;
    uint32_t temp_mask;
    int temp_value;
    if(0x1 != PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw))) {
        vsf_callback_timer_add_us(&adc_ptr->callback_timer, __vsf_adc_get_callback_time_us(adc_ptr));
        return;
    }

    PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw), 0x1);

    temp_mask = REG_GPIO0->MR;
    REG_GPIO0->MR = gpmsk;
    temp_value = PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_ro_acc));
    REG_GPIO0->MR = temp_mask;

    temp_value = temp_value * 1175 / 32896 - 1175;
    if (adc_ptr->current_channel->channel & 0x01) {
        temp_value = -temp_value;
    }
    if (temp_value < 0) {
        temp_value = 0;//todo:
    }
    VSF_HAL_ASSERT(temp_value <= 1175);
    *(uint16_t *)adc_ptr->data = temp_value * (1 << VSF_AIC8800_ADC_CFG_BIT_COUNT) / 1175;

    adc_ptr->status.is_busy = false;
    if (    (NULL != adc_ptr->cfg.isr.handler_fn)
        &&  adc_ptr->status.is_irq) {
        adc_ptr->cfg.isr.handler_fn( adc_ptr->cfg.isr.target_ptr, adc_ptr);
    }
}

static vsf_err_t __vsf_adc_channel_request_base(vsf_adc_t *adc_ptr,
                                                void *buffer_ptr)
{
    adc_ptr->status.is_busy = true;
    adc_ptr->data = buffer_ptr;
    adc_ptr->callback_timer.on_timer = __vk_adc_on_time;
    return __vsf_adc_channel_request(adc_ptr, adc_ptr->current_channel);
}

vsf_err_t vsf_adc_init(vsf_adc_t *adc_ptr, adc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(NULL != &(adc_ptr->callback_timer));
    adc_ptr->cfg = *cfg_ptr;
    __vsf_adc_init(adc_ptr, cfg_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_adc_enable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->status.is_enable = true;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_adc_disable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->status.is_enable = false;
    return VSF_ERR_NONE;
}

void vsf_adc_irq_enable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->status.is_irq = true;
}

void vsf_adc_irq_disable(vsf_adc_t *adc_ptr)
{
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->status.is_irq = false;
}

vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr,
                                 adc_channel_cfg_t *channel_cfgs_ptr,
                                 uint32_t channel_cfgs_cnt)
{
    uint32_t i;
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);
    if (!adc_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;//todo
    }

    for (i = 0; i < channel_cfgs_cnt; i++) {
        VSF_HAL_ASSERT(NULL != (channel_cfgs_ptr + i));
        __vsf_adc_channel_config(adc_ptr, channel_cfgs_ptr + i);
        adc_ptr->cfg_channel[i] = channel_cfgs_ptr[i];
    }

    adc_ptr->channel_count = channel_cfgs_cnt;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_adc_channel_request_once(vsf_adc_t *adc_ptr,
                                      adc_channel_cfg_t *channel_cfg_ptr,
                                      void *buffer_ptr)
{
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr));
    if (!adc_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;//todo
    }
    if (adc_ptr->status.is_busy) {
        return VSF_ERR_ALREADY_EXISTS;//todo
    }
    adc_ptr->status.is_busy = true;
    __vsf_adc_channel_config(adc_ptr, channel_cfg_ptr);
    adc_ptr->current_channel = channel_cfg_ptr;
    return __vsf_adc_channel_request_base(adc_ptr, buffer_ptr);
}

#if VSF_HAL_ADC_IMP_REQUEST_MULTI == ENABLED
#   include "hal/driver/common/adc/__adc_common.inc"
#endif

#endif /* VSF_HAL_USE_AD */
