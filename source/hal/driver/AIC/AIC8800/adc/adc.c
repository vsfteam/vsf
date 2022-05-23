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

#define VSF_ADC_CFG_PREFIX                  vsf_hw
#define VSF_ADC_CFG_UPPERCASE_PREFIX        VSF_HW

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED

#include "./kernel/vsf_kernel.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/gpadc/gpadc_api.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_msadc.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_analog_reg.h"
#include "./hal/driver/AIC/AIC8800/vendor/plf/aic8800/src/driver/aic1000lite_regs/aic1000Lite_rtc_core.h"
#include "../gpio/gpio.h"
#include "./adc.h"

/*============================ MACROS ========================================*/

#if PLF_PMIC_VER_LITE == 0
#   error "TODO"
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_adc_request_t {
    struct {
        uint_fast32_t   cnt;
        uint_fast32_t   idx;
        void           *buf;
    };
} vsf_hw_adc_request_t;

typedef struct vsf_hw_adc_t {
#if VSF_ADC_CFG_IMPLEMENT_OP == ENABLED
    vsf_adc_t vsf_adc;
#endif

    adc_cfg_t               cfg;

    vsf_callback_timer_t    callback_timer;

    struct {
        adc_channel_cfg_t  *current_channel;
        void               *buffer_ptr;
    };

    struct {
        adc_channel_cfg_t   cfgs[VSF_HW_ADC_CFG_CHANNEL_COUNT];
        uint_fast32_t       count;
        uint_fast32_t       index;
    } chns;

    struct {
        uint32_t            is_enable       : 1;
        uint32_t            is_busy         : 1;
        uint32_t            is_irq          : 1;
        uint32_t                            : 29;
    } status;

    vsf_hw_adc_request_t request;
} vsf_hw_adc_t;

/*============================ INCLUDES ======================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vsf_adc_get_callback_time_us(vsf_hw_adc_t *hw_adc_ptr)
{
    return 26000000 / hw_adc_ptr->cfg.clock_freq * 20 + 6 + VSF_HW_ADC_CFG_CALLBACK_TIME_POSTPONE_US;
}

static void __vsf_adc_measure(int type)
{
    if (type == ADC_REF_VDD_1) {
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
    } else if (type == ADC_REF_VDD_1_2) {
        VSF_HAL_ASSERT(false); //TODO:
    } else if (type == ADC_REF_VDD_1_3) {
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
    }
}

static vsf_err_t __vsf_adc_channel_config(vsf_hw_adc_t *hw_adc_ptr, adc_channel_cfg_t *channel_cfgs_ptr)
{
    uint8_t channel = channel_cfgs_ptr->channel;
    if (channel > 7) {
        // TODO:
        return VSF_ERR_INVALID_RANGE;
    }

    PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_mode), 0);
    if ((channel <= 1) || (channel == 13)) {
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteAnalogReg->gpio_ctrl1),
            (0x01 << channel), (0x01 << channel));
    } else {
        PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteRtcCore->rtc_rg_por_ctrl_cfg2),
            AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel - 2)),
            AIC1000LITE_RTC_CORE_RTC_RG_GPIO27_MUX0_EN(0x01 << (channel - 2)));
    }

    vsf_hw_gpio_config_pin(&vsf_hw_gpio1, channel, 0);
    vsf_hw_gpio_set_input(&vsf_hw_gpio1, channel);

    PMIC_MEM_MASK_WRITE((unsigned int)(&aic1000liteIomux->GPCFG[channel]),
        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC),
        (AIC1000LITE_IOMUX_PAD_GPIO_PULL_FRC));

    return VSF_ERR_NONE;
}

static vsf_err_t __vsf_adc_channel_request(vsf_hw_adc_t *hw_adc_ptr, adc_channel_cfg_t *channel_cfg_ptr)
{
    uint8_t channel = channel_cfg_ptr->channel;
    if (channel > 7) {              // TODO:
        VSF_HAL_ASSERT(0);
        return VSF_ERR_INVALID_RANGE;
    }

    int neg_flag = channel & 0x01;
    unsigned int mux_bit = 7 - (channel >> 1);

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

    vsf_callback_timer_add_us(&hw_adc_ptr->callback_timer, __vsf_adc_get_callback_time_us(hw_adc_ptr));

    return VSF_ERR_NONE;
}

static void __vk_adc_on_time(vsf_callback_timer_t *timer)
{
    vsf_hw_adc_t *hw_adc_ptr = container_of(timer, vsf_hw_adc_t, callback_timer);

    if (0x1 != PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw))) {
        vsf_callback_timer_add_us(&hw_adc_ptr->callback_timer, __vsf_adc_get_callback_time_us(hw_adc_ptr));
        return;
    }
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteMsadc->cfg_msadc_int_raw), 0x1);

    int value = PMIC_MEM_READ((unsigned int)(&aic1000liteMsadc->cfg_msadc_ro_acc));
    value = value * 1175 / 32896 - 1175;
    if (hw_adc_ptr->current_channel->channel & 0x01) {
        value = -value;
    }
    if (value < 0) {
        value = 0;  //todo:
    }
    VSF_HAL_ASSERT(value <= 1175);
    *(uint16_t *)hw_adc_ptr->buffer_ptr = value;

    hw_adc_ptr->status.is_busy = false;
    if ((NULL != hw_adc_ptr->cfg.isr.handler_fn) && hw_adc_ptr->status.is_irq) {
        hw_adc_ptr->cfg.isr.handler_fn(hw_adc_ptr->cfg.isr.target_ptr, (vsf_adc_t *)hw_adc_ptr);
    }
}

vsf_err_t vsf_hw_adc_init(vsf_hw_adc_t *hw_adc_ptr, adc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != hw_adc_ptr) && (NULL != cfg_ptr));

    hw_adc_ptr->cfg = *cfg_ptr;

    hw_adc_ptr->cfg.clock_freq = vsf_max(hw_adc_ptr->cfg.clock_freq, 101960);
    hw_adc_ptr->cfg.clock_freq = vsf_min(hw_adc_ptr->cfg.clock_freq, 13000000);

    uint32_t temp_clock_div = 26000000 / hw_adc_ptr->cfg.clock_freq;
    PMIC_MEM_WRITE((unsigned int)(&aic1000liteSysctrl->msadc_clk_div),
            AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_DENOM(temp_clock_div)
        |   AIC1000LITE_SYS_CTRL_CFG_CLK_MSADC_DIV_UPDATE);

    hw_adc_ptr->callback_timer.on_timer = __vk_adc_on_time;
    vsf_callback_timer_init(&hw_adc_ptr->callback_timer);

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_adc_enable(vsf_hw_adc_t *hw_adc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_adc_ptr);

    hw_adc_ptr->status.is_enable = true;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_adc_disable(vsf_hw_adc_t *hw_adc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_adc_ptr);

    hw_adc_ptr->status.is_enable = false;

    return fsm_rt_cpl;
}

void vsf_hw_adc_irq_enable(vsf_hw_adc_t *hw_adc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_adc_ptr);

    hw_adc_ptr->status.is_irq = true;
}

void vsf_hw_adc_irq_disable(vsf_hw_adc_t *hw_adc_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_adc_ptr);

    hw_adc_ptr->status.is_irq = false;
}

vsf_err_t vsf_hw_adc_channel_request_once(vsf_hw_adc_t *hw_adc_ptr,
                                          adc_channel_cfg_t *channel_cfg_ptr,
                                          void *buffer_ptr)
{
    VSF_HAL_ASSERT((NULL != hw_adc_ptr) && (NULL != buffer_ptr));

    VSF_HAL_ASSERT(hw_adc_ptr->status.is_enable);
    VSF_HAL_ASSERT(!hw_adc_ptr->status.is_busy);

    hw_adc_ptr->status.is_busy = true;

    vsf_err_t result = __vsf_adc_channel_config(hw_adc_ptr, channel_cfg_ptr);
    if (result == VSF_ERR_NONE) {
        hw_adc_ptr->current_channel = channel_cfg_ptr;
        hw_adc_ptr->buffer_ptr = buffer_ptr;

        result = __vsf_adc_channel_request(hw_adc_ptr, hw_adc_ptr->current_channel);
    }

    return result;
}

vsf_err_t vsf_hw_adc_channel_config(vsf_hw_adc_t *hw_adc_ptr,
                                    adc_channel_cfg_t *channel_cfgs_ptr,
                                    uint32_t channel_cfgs_cnt)
{
    VSF_HAL_ASSERT((NULL != hw_adc_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);

    VSF_HAL_ASSERT(hw_adc_ptr->status.is_enable);

    vsf_err_t result;
    for (int i = 0; i < channel_cfgs_cnt; i++) {
        hw_adc_ptr->chns.cfgs[i] = channel_cfgs_ptr[i];

        result = __vsf_adc_channel_config(hw_adc_ptr, channel_cfgs_ptr + i);
        if (VSF_ERR_NONE != result) {
            return result;
        }
    }

    hw_adc_ptr->chns.count = channel_cfgs_cnt;

    return VSF_ERR_NONE;
}

static void __adc_request_isr_handler(void *target, vsf_adc_t *adc_ptr)
{
    vsf_hw_adc_t *hw_adc_ptr = (vsf_hw_adc_t *)adc_ptr;
    VSF_HAL_ASSERT(NULL != hw_adc_ptr);

    if (hw_adc_ptr->request.idx < hw_adc_ptr->request.cnt) {
        vsf_hw_adc_channel_request_once(hw_adc_ptr,
                                     &hw_adc_ptr->chns.cfgs[hw_adc_ptr->chns.index],
                                     (void *)((uint16_t *)hw_adc_ptr->request.buf + hw_adc_ptr->request.idx));
        if (hw_adc_ptr->chns.index < hw_adc_ptr->chns.count) {
            hw_adc_ptr->chns.index++;
        } else {
            hw_adc_ptr->chns.index = 0;
        }
        hw_adc_ptr->request.idx++;
        return;
    } else {
        hw_adc_ptr->status.is_busy = 0;
        vsf_hw_adc_irq_disable(hw_adc_ptr);
        vsf_adc_isr_t *isr = &hw_adc_ptr->cfg.isr;
        hw_adc_ptr->buffer_ptr = hw_adc_ptr->request.buf;
        if (NULL != isr->handler_fn) {
            isr->handler_fn(isr->target_ptr, adc_ptr);
        }
        vsf_hw_adc_irq_enable(hw_adc_ptr);
    }
}

static void __adc_req_init(vsf_hw_adc_t *hw_adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    hw_adc_ptr->request.buf = buffer_ptr;
    hw_adc_ptr->request.cnt = count;
    hw_adc_ptr->request.idx = 0;

    if (    (hw_adc_ptr->cfg.isr.handler_fn != NULL)
        &&  (hw_adc_ptr->cfg.isr.handler_fn != __adc_request_isr_handler)) {
        hw_adc_ptr->cfg.isr.handler_fn = __adc_request_isr_handler;
    }
}

vsf_err_t vsf_hw_adc_channel_request(vsf_hw_adc_t *hw_adc_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT((NULL != hw_adc_ptr) && (NULL != buffer_ptr) && (count > 0));

    VSF_HAL_ASSERT(hw_adc_ptr->status.is_enable);
    VSF_HAL_ASSERT(!hw_adc_ptr->status.is_busy);

    __adc_req_init(hw_adc_ptr, buffer_ptr, count);
    hw_adc_ptr->chns.index = 0;

    vsf_protect_t orig = vsf_protect(interrupt)();
        __adc_request_isr_handler(NULL, (vsf_adc_t *)hw_adc_ptr);
    vsf_unprotect(interrupt)(orig);

    return VSF_ERR_NONE;
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_ADC_CFG_IMP_LV0(__COUNT, __HAL_OP)                                  \
    vsf_hw_adc_t vsf_hw_adc##__COUNT = {                                        \
        .buffer_ptr = NULL,                                                     \
        .chns.count = 0,                                                        \
        .chns.index = 0,                                                        \
        .current_channel = NULL,                                                \
        .status = {                                                             \
            .is_enable = false,                                                 \
            .is_busy = false,                                                   \
            .is_irq = false,                                                    \
        },                                                                      \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/adc/adc_template.inc"

#endif /* VSF_HAL_USE_AD */
