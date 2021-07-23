/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
        .data_count = 0,                                                        \
        .data_index = 0,                                                        \
        .channel_count = 0,                                                     \
        .channel_index = 0,                                                     \
        .cfg_channel = NULL,                                                    \
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

static void __vsf_adc_init(vsf_adc_t *adc_ptr, adc_cfg_t *cfg_ptr)
{
    //todo:cfg_ptr
    vsf_callback_timer_init(&adc_ptr->callback_timer);
}

static void __vsf_adc_channel_config(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfgs_ptr)
{
    if (channel_cfgs_ptr->channel <= 7) {
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_mode), 0);
        if ((channel_cfgs_ptr->channel <= 1) || (channel_cfgs_ptr->channel == 13)) {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->gpio_ctrl1),
                (0x01 << channel_cfgs_ptr->channel), (0x01 << channel_cfgs_ptr->channel));
        } else {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg2),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel_cfgs_ptr->channel - 2)),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel_cfgs_ptr->channel - 2)));
        }
        vsf_gpio_config_pin(&vsf_gpio0, 1 << (channel_cfgs_ptr->channel + 16), 0);
        vsf_gpio_set_input(&vsf_gpio0, 1 << (channel_cfgs_ptr->channel + 16));
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[channel_cfgs_ptr->channel]),
            (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC),
            (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC));
    }
}

static void __vsf_adc_measure(int type)
{
    if (type == ADC_TYPE_VBAT) {
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg1),
            (AIC1000LITE_RTC_CORE_RTC_RG_PU_VRTC_SENSE | AIC1000LITE_RTC_CORE_RTC_RG_PU_VBAT_SENSE),
            (AIC1000LITE_RTC_CORE_RTC_RG_PU_VRTC_SENSE | AIC1000LITE_RTC_CORE_RTC_RG_PU_VBAT_SENSE));
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xD) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            (0 | 0 | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN));
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);
    } else if (type == GPADC_TYPE_VIO) {
        ; //TODO:
    } else if (type == GPADC_TYPE_TEMP0) {
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0x0) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            (0 | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE | 0 | 0 | 0),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN));
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);
    }
}

static vsf_err_t __vsf_adc_channel_request(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfg_ptr)
{
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteSysctrl->msadc_clk_div),
            AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_DENOM(channel_cfg_ptr->sample_time)
        |   AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_UPDATE);
    __vsf_adc_measure(channel_cfg_ptr->feature);
    if (channel_cfg_ptr->channel <= 7) {
        int neg_flag = channel_cfg_ptr->channel & 0x01;
        unsigned int mux_bit = 7 - (channel_cfg_ptr->channel >> 1);
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            ((neg_flag ? 0 : AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL) |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN),
            (AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL | AIC1000LITE_MSADC_CFG_ANA_MSADC_TS_MODE |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_MODE | AIC1000LITE_MSADC_CFG_ANA_MSADC_SDM_GAIN_BIT |
            AIC1000LITE_MSADC_CFG_ANA_MSADC_ADC_FF_EN)); // channel p sel or not
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(mux_bit) | 0),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xF) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);
        vsf_callback_timer_add_us(&adc_ptr->callback_timer, VSF_ADC_CFG_CALLBACK_TIME_US);
        return VSF_ERR_NONE;
    }
    return VSF_ERR_INVALID_RANGE;
}


static void __vk_ad_on_time(vsf_adc_t *adc_ptr)
{
    if (adc_ptr->data_index < (adc_ptr->data_count / adc_ptr->channel_count)) {
        adc_ptr->data_index++;
        __vsf_adc_channel_request(adc_ptr, &adc_ptr->cfg_channel[adc_ptr->channel_index - 1]);
        return;
    } else if(adc_ptr->channel_index < adc_ptr->channel_count) {
        adc_ptr->channel_index++;
        adc_ptr->data_index = 0;
        __vsf_adc_channel_request(adc_ptr, &adc_ptr->cfg_channel[adc_ptr->channel_index - 1]);
        return;
    }
}

static void __vk_ad_on_time_one(vsf_callback_timer_t *timer)
{
    vsf_adc_t *adc_ptr = container_of(timer, vsf_adc_t, callback_timer);
    unsigned int gpmsk = 0x01UL << adc_ptr->cfg_channel[adc_ptr->channel_index - 1].channel;
    uint32_t temp_value;
    if(0x1 != PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw))) {
        vsf_callback_timer_add_us(&adc_ptr->callback_timer, VSF_ADC_CFG_CALLBACK_TIME_US);
        return;
    }
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw), 0x1);
    temp_value = vsf_gpio0.REG.GPIO->MR;
    vsf_gpio0.REG.GPIO->MR = gpmsk;
    *((int16_t *)(adc_ptr->data) + adc_ptr->data_index + (adc_ptr->data_count * adc_ptr->channel_index)) =
        PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_ro_acc));
    vsf_gpio0.REG.GPIO->MR = temp_value;
    temp_value =
        ((int)(*((int *)(adc_ptr->data) + adc_ptr->data_index + (adc_ptr->data_count * adc_ptr->channel_index))) * 1175 / 32896 - 1175) * (((adc_ptr->cfg_channel->channel & 0x01)) ? -1 : 1);
    *((int16_t *)(adc_ptr->data) + adc_ptr->data_index + (adc_ptr->data_count * adc_ptr->channel_index)) = temp_value * (1 << VSF_AIC8800_ADC_CFG_BIT_COUNT) / 1100;
    adc_ptr->status.is_busy = false;
    if (adc_ptr->status.is_complicated) {
        __vk_ad_on_time(adc_ptr);
        return;
    }
    adc_ptr->status.is_busy = false;
    if ((NULL != adc_ptr->cfg.isr.handler_fn) && (true == adc_ptr->status.is_irq)) {
        adc_ptr->cfg.isr.handler_fn( adc_ptr->cfg.isr.target_ptr, adc_ptr);
    }
}

static vsf_err_t __vsf_adc_channel_request_base(vsf_adc_t *adc_ptr, void *buffer_ptr)
{
    adc_ptr->status.is_busy = true;
    adc_ptr->data = buffer_ptr;
    adc_ptr->data_index = 0;
    adc_ptr->channel_index = 1;
    adc_ptr->callback_timer.on_timer = __vk_ad_on_time_one;
    return __vsf_adc_channel_request(adc_ptr, adc_ptr->cfg_channel);
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

vsf_err_t vsf_adc_channel_config(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfgs_ptr, uint32_t channel_cfgs_cnt)
{
    uint32_t i;
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);
    if (false == adc_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;
    }
    for (i = 0; i < channel_cfgs_cnt; i++) {
        VSF_HAL_ASSERT(NULL != (channel_cfgs_ptr + i));
        __vsf_adc_channel_config(adc_ptr, channel_cfgs_ptr + i);
    }
    adc_ptr->channel_count = channel_cfgs_cnt;
    if (NULL != adc_ptr->cfg_channel) {
        free((void *)adc_ptr->cfg_channel);
    }
    adc_ptr->cfg_channel = (adc_channel_cfg_t *)malloc(sizeof(adc_channel_cfg_t) * channel_cfgs_cnt);
    memcpy(adc_ptr->cfg_channel, channel_cfgs_ptr, sizeof(adc_channel_cfg_t) * channel_cfgs_cnt);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_adc_channel_request_one(vsf_adc_t *adc_ptr, adc_channel_cfg_t *channel_cfg_ptr, void *buffer_ptr)
{
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr));
    if (false == adc_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;
    }
    if (true == adc_ptr->status.is_busy) {
        return VSF_ERR_ALREADY_EXISTS;
    }
    __vsf_adc_channel_config(adc_ptr, channel_cfg_ptr);
    adc_ptr->data_count = 0;
    adc_ptr->channel_count = 1;
    adc_ptr->status.is_complicated = false;
    if (NULL != adc_ptr->cfg_channel) {
        free((void *)adc_ptr->cfg_channel);
    }
    adc_ptr->cfg_channel = (adc_channel_cfg_t *)malloc(sizeof(adc_channel_cfg_t));
    memcpy(adc_ptr->cfg_channel, channel_cfg_ptr, sizeof(adc_channel_cfg_t));
    adc_ptr->callback_timer.on_timer = __vk_ad_on_time_one;
    return __vsf_adc_channel_request_base(adc_ptr, buffer_ptr);
}

vsf_err_t vsf_adc_channel_request(vsf_adc_t *adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(false);
    return VSF_ERR_NONE;
    //todo:
//    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr));
//    VSF_HAL_ASSERT(0 != count);
//    if (false == adc_ptr->status.is_enable) {
//        return VSF_ERR_NOT_READY;
//    }
//    if (true == adc_ptr->status.is_busy) {
//        return VSF_ERR_ALREADY_EXISTS;
//    }
//    adc_ptr->status.is_busy = true;
//    adc_ptr->data_count = count;
//    adc_ptr->status.is_complicated = true;
//    adc_ptr->callback_timer.on_timer = __vk_ad_on_time_one;
//    return __vsf_adc_channel_request(adc_ptr, adc_ptr->cfg_channel);
}
//todo:
//void __adc_isr_handler(void *target_ptr, vsf_adc_t *adc_ptr)
//{
//    int i;
//    for (i = 0; i < 1; i++) {
//        vsf_trace(VSF_TRACE_INFO, "[%d] = %d\t", adc_ptr->cfg_channel->channel, *((int *)adc_ptr->data + i));
//        if (adc_ptr->cfg_channel->channel == 7) {
//            vsf_trace(VSF_TRACE_INFO, "\n");
//        }
//        *((int *)adc_ptr->data + i) = 0;
//    }
//}

#endif /* VSF_HAL_USE_AD */
