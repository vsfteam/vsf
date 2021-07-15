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

#include "./ad.h"
#if VSF_HAL_USE_AD == ENABLED
#include "../gpio/gpio.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_hw_ad_imp_lv0(__count, __dont_care)                               \
    vsf_ad_t vsf_ad##__count = {                                                \
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

#define aic8800_ad_def(__count)                                                 \
    VSF_MREPEAT(__count, __vsf_hw_ad_imp_lv0, NULL)


/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

aic8800_ad_def(1)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_ad_init(vsf_ad_t *ad_ptr, ad_cfg_t *cfg_ptr)
{
    //todo:cfg_ptr
    vsf_callback_timer_init(&ad_ptr->callback_timer);
}

static void __vsf_ad_channel_config(vsf_ad_t *ad_ptr, ad_channel_cfg_t *channel_cfgs_ptr)
{
    if (channel_cfgs_ptr->channel <= 7) {
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_mode), 0);
        if (channel_cfgs_ptr->channel <= 1) {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->gpio_ctrl1),
                (0x01 << channel_cfgs_ptr->channel), (0x01 << channel_cfgs_ptr->channel));
        } else {
            PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg2),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel_cfgs_ptr->channel - 2)),
                AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel_cfgs_ptr->channel - 2)));
        }
        vsf_gpio_set_input(&vsf_gpio0, 1 << (channel_cfgs_ptr->channel + 16));
    }
}

vsf_err_t __vsf_ad_channel_request(vsf_ad_t *ad_ptr)
{
    if (ad_ptr->cfg_channel[ad_ptr->channel_index].channel <= 7) {
        int neg_flag = ad_ptr->cfg_channel[ad_ptr->channel_index].channel & 0x01;
        unsigned int mux_bit = 7 - (ad_ptr->cfg_channel[ad_ptr->channel_index].channel >> 1);
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_ana_ctrl0),
            (neg_flag ? 0 : AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL),
            AIC1000LITE_MSADC_CFG_ANA_MSADC_CHNP_SEL);
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl1),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(mux_bit) | 0),
            (AIC1000LITE_MSADC_CFG_MSADC_SW_MUX_BITS(0xf) | AIC1000LITE_MSADC_CFG_MSADC_SW_DIFF_MODE));
        PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_sw_ctrl0),
            AIC1000LITE_MSADC_CFG_MSADC_SW_START_PULSE);

        vsf_callback_timer_add_us(&ad_ptr->callback_timer, VSF_AD_CFG_CALLBACK_TIME_US);
        return VSF_ERR_NONE;
    }
    return VSF_ERR_INVALID_RANGE;
}

void __vk_ad_on_time(vsf_callback_timer_t *timer)
{
    vsf_ad_t *ad_ptr = container_of(timer, vsf_ad_t, callback_timer);
    unsigned int gpmsk = 0x01UL << ad_ptr->cfg_channel[ad_ptr->channel_index].channel;
    uint32_t temp_value;
    int neg_flag = ad_ptr->cfg_channel[ad_ptr->channel_index].channel & 0x01;
    if(0x1 != PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw))) {//can not
        vsf_callback_timer_add_us(&ad_ptr->callback_timer, VSF_AD_CFG_CALLBACK_TIME_US);
        return;
    }
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw), 0x1);
    temp_value = vsf_gpio0.REG.GPIO->MR;
    vsf_gpio0.REG.GPIO->MR = gpmsk;
    *((int *)(ad_ptr->data) + ad_ptr->data_index + (ad_ptr->data_count * ad_ptr->channel_index)) =
        PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_ro_acc)) * (neg_flag ? -1 : 1);
    vsf_gpio0.REG.GPIO->MR = temp_value;
    if (ad_ptr->data_index < ad_ptr->data_count) {
        ad_ptr->data_index++;
        __vsf_ad_channel_request(ad_ptr);
        return;
    } else if (ad_ptr->channel_index < ad_ptr->channel_count) {
        ad_ptr->channel_index++;
        ad_ptr->data_index = 0;
        __vsf_ad_channel_request(ad_ptr);
        return;
    }
    ad_ptr->status.is_busy = false;
    if (NULL != ad_ptr->cfg.isr.handler_fn) {
        ad_ptr->cfg.isr.handler_fn( ad_ptr->cfg.isr.target_ptr, ad_ptr);
    }
}

vsf_err_t vsf_ad_init(vsf_ad_t *ad_ptr, ad_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != ad_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(NULL != &(ad_ptr->callback_timer));
    ad_ptr->cfg = *cfg_ptr;
    __vsf_ad_init(ad_ptr, cfg_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_ad_enable(vsf_ad_t *ad_ptr)
{
    VSF_HAL_ASSERT(NULL != ad_ptr);
    ad_ptr->status.is_enable = true;
    return VSF_ERR_NONE;
}


vsf_err_t vsf_ad_disable(vsf_ad_t *ad_ptr)
{
    VSF_HAL_ASSERT(NULL != ad_ptr);
    ad_ptr->status.is_enable = false;
    return VSF_ERR_NONE;
}

void vsf_ad_irq_enable(vsf_ad_t *ad_ptr)
{
    VSF_HAL_ASSERT(NULL != ad_ptr);
    ad_ptr->status.is_irq = true;
}
void vsf_ad_irq_disable(vsf_ad_t *ad_ptr)
{
    VSF_HAL_ASSERT(NULL != ad_ptr);
    ad_ptr->status.is_irq = false;
}

vsf_err_t vsf_ad_channel_config(vsf_ad_t *ad_ptr, ad_channel_cfg_t channel_cfgs_ptr[], uint32_t channel_cfgs_cnt)
{
    uint32_t i;
    VSF_HAL_ASSERT((NULL != ad_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);
    if (false == ad_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;
    }
    for (i = 0; i < channel_cfgs_cnt; i++) {
        VSF_HAL_ASSERT(NULL != (channel_cfgs_ptr + i));
        __vsf_ad_channel_config(ad_ptr, channel_cfgs_ptr + i);
    }
    ad_ptr->channel_count = channel_cfgs_cnt - 1;
    if (NULL != ad_ptr->cfg_channel) {
        free((void *)ad_ptr->cfg_channel);
    }
    ad_ptr->cfg_channel = (ad_channel_cfg_t *)malloc(sizeof(ad_channel_cfg_t) * channel_cfgs_cnt);
    memcpy(ad_ptr->cfg_channel, channel_cfgs_ptr, sizeof(ad_channel_cfg_t) * channel_cfgs_cnt);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_ad_channel_request(vsf_ad_t *ad_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT((NULL != ad_ptr) && (NULL != buffer_ptr));
    VSF_HAL_ASSERT(0 != count);
    if (false == ad_ptr->status.is_enable) {
        return VSF_ERR_NOT_READY;
    }
    if (true == ad_ptr->status.is_busy) {
        return VSF_ERR_ALREADY_EXISTS;
    }
    ad_ptr->status.is_busy = true;
    ad_ptr->data_count = count;
    ad_ptr->data_index = 0;
    ad_ptr->channel_index = 0;
    ad_ptr->data = buffer_ptr;
    ad_ptr->callback_timer.on_timer = __vk_ad_on_time;
    return __vsf_ad_channel_request(ad_ptr);
}
#endif /* VSF_HAL_USE_AD */

