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

#include "./rtc.h"

#if VSF_HAL_USE_RTC == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_RTC_CFG_MULTI_CLASS
#   define VSF_HW_RTC_CFG_MULTI_CLASS          VSF_RTC_CFG_MULTI_CLASS
#endif

#define VSF_RTC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RTC_CFG_IMP_UPCASE_PREFIX           VSF_HW

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) {
#if VSF_HW_RTC_CFG_MULTI_CLASS == ENABLED
    vsf_rtc_t               vsf_rtc;
#endif
    rtc_hw_t                *reg;
    IRQn_Type               irqn;
    uint32_t                rst_bit;
    vsf_rtc_isr_t           isr;
} VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __rp2040_rtc_wait_active(rtc_hw_t *reg)
{
    while (!(reg->ctrl & RTC_CTRL_RTC_ACTIVE_BITS)) {
        tight_loop_contents();
    }
}

static void __rp2040_rtc_wait_not_active(rtc_hw_t *reg)
{
    while (reg->ctrl & RTC_CTRL_RTC_ACTIVE_BITS) {
        tight_loop_contents();
    }
}

static void __rp2040_rtc_read_stable(rtc_hw_t *reg, uint32_t *out_0, uint32_t *out_1)
{
    uint32_t rtc_0, rtc_1;
    uint32_t prev_0, prev_1;

    do {
        prev_0 = reg->rtc_0;
        prev_1 = reg->rtc_1;
        rtc_0  = reg->rtc_0;
        rtc_1  = reg->rtc_1;
        // spin-wait: double-read until stable (crossing clock domains)
    } while ((rtc_0 != prev_0) || (rtc_1 != prev_1));

    *out_0 = rtc_0;
    *out_1 = rtc_1;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_init)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;

    // Unreset RTC block
    uint32_t rst_bit = rtc_ptr->rst_bit;
    resets_hw->reset = resets_hw->reset & ~rst_bit;
    while (!(resets_hw->reset_done & rst_bit));

    // Configure 1Hz divider from clk_rtc (pico-sdk default: 46875Hz)
    #define RP2040_CLK_RTC_FREQ     46875
    reg->clkdiv_m1 = RP2040_CLK_RTC_FREQ - 1;

    // Disable RTC before configuration
    reg->ctrl = 0;
    __rp2040_rtc_wait_not_active(reg);

    // Store ISR config
    rtc_ptr->isr = cfg_ptr->isr;

    // Configure NVIC if handler provided
    if (rtc_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(rtc_ptr->irqn, (uint32_t)rtc_ptr->isr.prio);
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_fini)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;
    reg->ctrl = 0;
    __rp2040_rtc_wait_not_active(reg);

    // Disable RTC IRQ
    NVIC_DisableIRQ(rtc_ptr->irqn);
}

fsm_rt_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_enable)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;
    reg->ctrl = RTC_CTRL_RTC_ENABLE_BITS;
    __rp2040_rtc_wait_active(reg);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_disable)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;
    reg->ctrl = 0;
    __rp2040_rtc_wait_not_active(reg);

    return fsm_rt_cpl;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_tm_t *rtc_tm
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_HAL_ASSERT(NULL != rtc_tm);

    rtc_hw_t *reg = rtc_ptr->reg;

    // RP2040 RTC registers are in a slower clock domain.
    uint32_t rtc_0, rtc_1;
    __rp2040_rtc_read_stable(reg, &rtc_0, &rtc_1);

    rtc_tm->tm_year = (uint16_t)((rtc_1 >> RTC_RTC_1_YEAR_LSB)  & 0xFFF);
    rtc_tm->tm_mon  = (uint8_t) ((rtc_1 >> RTC_RTC_1_MONTH_LSB) & 0xF);
    rtc_tm->tm_mday = (uint8_t) ((rtc_1 >> RTC_RTC_1_DAY_LSB)   & 0x1F);

    rtc_tm->tm_wday = (uint8_t) ((rtc_0 >> RTC_RTC_0_DOTW_LSB)  & 0x7);
    rtc_tm->tm_hour = (uint8_t) ((rtc_0 >> RTC_RTC_0_HOUR_LSB)  & 0x1F);
    rtc_tm->tm_min  = (uint8_t) ((rtc_0 >> RTC_RTC_0_MIN_LSB)   & 0x3F);
    rtc_tm->tm_sec  = (uint8_t) ((rtc_0 >> RTC_RTC_0_SEC_LSB)   & 0x3F);
    rtc_tm->tm_ms   = 0;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    const vsf_rtc_tm_t *rtc_tm
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_HAL_ASSERT(NULL != rtc_tm);

    rtc_hw_t *reg = rtc_ptr->reg;

    // Match pico-sdk rtc_set_datetime sequence exactly: always disable →
    // write setup → LOAD → ENABLE → wait active. Anything less risks the
    // LOAD bit being processed without ENABLE=1 in the slow clock domain.
    reg->ctrl = 0;
    __rp2040_rtc_wait_not_active(reg);

    reg->setup_0 = ((uint32_t)(rtc_tm->tm_year & 0xFFF) << RTC_SETUP_0_YEAR_LSB)
                 | ((uint32_t)(rtc_tm->tm_mon  & 0xF)   << RTC_SETUP_0_MONTH_LSB)
                 | ((uint32_t)(rtc_tm->tm_mday & 0x1F)  << RTC_SETUP_0_DAY_LSB);

    reg->setup_1 = ((uint32_t)(rtc_tm->tm_wday & 0x7)   << RTC_SETUP_1_DOTW_LSB)
                 | ((uint32_t)(rtc_tm->tm_hour & 0x1F)  << RTC_SETUP_1_HOUR_LSB)
                 | ((uint32_t)(rtc_tm->tm_min  & 0x3F)  << RTC_SETUP_1_MIN_LSB)
                 | ((uint32_t)(rtc_tm->tm_sec  & 0x3F)  << RTC_SETUP_1_SEC_LSB);

    reg->ctrl = RTC_CTRL_LOAD_BITS | RTC_CTRL_RTC_ENABLE_BITS;
    __rp2040_rtc_wait_active(reg);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_time)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_time_t *second_ptr,
    vsf_rtc_time_t *millisecond_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;

    uint32_t rtc_0, rtc_1;
    __rp2040_rtc_read_stable(reg, &rtc_0, &rtc_1);

    if (second_ptr != NULL) {
        *second_ptr = ((uint64_t)rtc_1 << 32) | rtc_0;
    }
    if (millisecond_ptr != NULL) {
        *millisecond_ptr = 0;
    }

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set_time)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_time_t seconds,
    vsf_rtc_time_t milliseconds
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_UNUSED_PARAM(milliseconds);

    rtc_hw_t *reg = rtc_ptr->reg;

    reg->ctrl = 0;
    __rp2040_rtc_wait_not_active(reg);

    reg->setup_0 = (uint32_t)(seconds & 0xFFFFFFFF);
    reg->setup_1 = (uint32_t)(seconds >> 32);

    reg->ctrl = RTC_CTRL_LOAD_BITS | RTC_CTRL_RTC_ENABLE_BITS;
    __rp2040_rtc_wait_active(reg);

    return VSF_ERR_NONE;
}

vsf_rtc_capability_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_capability)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    return (vsf_rtc_capability_t) {
        .irq_mask = VSF_RTC_IRQ_MASK_ALARM,
    };
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_configuration)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    cfg_ptr->isr = rtc_ptr->isr;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_ctrl)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;

    switch (ctrl) {
    case VSF_RTC_CTRL_IRQ_ENABLE:
        NVIC_EnableIRQ(rtc_ptr->irqn);
        reg->inte = RTC_INTE_RTC_BITS;
        return VSF_ERR_NONE;

    case VSF_RTC_CTRL_IRQ_DISABLE:
        NVIC_DisableIRQ(rtc_ptr->irqn);
        reg->inte = 0;
        return VSF_ERR_NONE;

    case VSF_RTC_CTRL_SET_ALARM: {
        vsf_rtc_tm_t *alarm_tm = (vsf_rtc_tm_t *)param;
        if (NULL == alarm_tm) {
            return VSF_ERR_INVALID_PARAMETER;
        }

        // Disable match while configuring
        reg->irq_setup_0 = 0;
        reg->irq_setup_1 = 0;

        // Write alarm match values with all match enables set
        reg->irq_setup_0 = RTC_IRQ_SETUP_0_YEAR_ENA_BITS
                         | RTC_IRQ_SETUP_0_MONTH_ENA_BITS
                         | RTC_IRQ_SETUP_0_DAY_ENA_BITS
                         | ((uint32_t)(alarm_tm->tm_year & 0xFFF) << RTC_IRQ_SETUP_0_YEAR_LSB)
                         | ((uint32_t)(alarm_tm->tm_mon  & 0xF)   << RTC_IRQ_SETUP_0_MONTH_LSB)
                         | ((uint32_t)(alarm_tm->tm_mday & 0x1F)  << RTC_IRQ_SETUP_0_DAY_LSB);

        reg->irq_setup_1 = RTC_IRQ_SETUP_1_DOTW_ENA_BITS
                         | RTC_IRQ_SETUP_1_HOUR_ENA_BITS
                         | RTC_IRQ_SETUP_1_MIN_ENA_BITS
                         | RTC_IRQ_SETUP_1_SEC_ENA_BITS
                         | ((uint32_t)(alarm_tm->tm_wday & 0x7)   << RTC_IRQ_SETUP_1_DOTW_LSB)
                         | ((uint32_t)(alarm_tm->tm_hour & 0x1F)  << RTC_IRQ_SETUP_1_HOUR_LSB)
                         | ((uint32_t)(alarm_tm->tm_min  & 0x3F)  << RTC_IRQ_SETUP_1_MIN_LSB)
                         | ((uint32_t)(alarm_tm->tm_sec  & 0x3F)  << RTC_IRQ_SETUP_1_SEC_LSB);

        // Enable global match
        reg->irq_setup_0 |= RTC_IRQ_SETUP_0_MATCH_ENA_BITS;

        return VSF_ERR_NONE;
    }

    default:
        return VSF_ERR_NOT_SUPPORT;
    }
}

static void VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_irqhandler)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    rtc_hw_t *reg = rtc_ptr->reg;
    // Clear interrupt by reading INTS (acknowledged by hardware)
    (void)reg->ints;

    if (rtc_ptr->isr.handler_fn != NULL) {
        rtc_ptr->isr.handler_fn(rtc_ptr->isr.target_ptr,
                                (vsf_rtc_t *)rtc_ptr,
                                VSF_RTC_IRQ_MASK_ALARM);
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_CFG_MODE_CHECK_UNIQUE                       VSF_HAL_CHECK_MODE_LOOSE
#define VSF_RTC_CFG_IRQ_MASK_CHECK_UNIQUE                   VSF_HAL_CHECK_MODE_STRICT
#define VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY              ENABLED
#define VSF_RTC_CFG_REIMPLEMENT_API_CTRL                    ENABLED
#define VSF_RTC_CFG_REIMPLEMENT_API_GET_CONFIGURATION       ENABLED

#define VSF_RTC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t)                                \
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc, __IDX) = {                       \
        .reg = (rtc_hw_t *)VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC,    \
                                        __IDX, _REG),                           \
        .irqn =                                                                 \
            VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC, __IDX, _IRQN),    \
        .rst_bit = VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC, __IDX,     \
                                _RST_BIT),                                      \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC, __IDX,  \
                                   _IRQHandler)(void) {                         \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_irqhandler)(              \
            &VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc, __IDX));                \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/rtc/rtc_template.inc"

#endif      // VSF_HAL_USE_RTC
/* EOF */
