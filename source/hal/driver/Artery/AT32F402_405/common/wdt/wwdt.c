/*****************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_WDT == ENABLED

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

#include "./wdt.h"
#include "../../__device.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_WWDT_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${WDT_IP}_WDT_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_WWDT_CFG_MULTI_CLASS
#   define VSF_HW_WWDT_CFG_MULTI_CLASS           VSF_WDT_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_WDT_CFG_IMP_DEVICE_PREFIX           vsf_hw_wwdt
#define VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX    VSF_HW_WWDT
#define VSF_WDT_CFG_IMP_RENAME_DEVICE_PREFIX    ENABLED
// HW end

// WWDT divider values (actual divider = 4096 * (div + 1))
#define WWDT_DIV_4096                           0
#define WWDT_DIV_8192                           1
#define WWDT_DIV_16384                          2
#define WWDT_DIV_32768                          3

// WWDT counter range
#define WWDT_COUNTER_MIN                        0x40
#define WWDT_COUNTER_MAX                        0x7F

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) {
#if VSF_HW_WWDT_CFG_MULTI_CLASS == ENABLED
    vsf_wdt_t                                   vsf_wdt;
#endif
    wwdt_type                                   *reg;
    vsf_hw_peripheral_en_t                      en;
    vsf_hw_peripheral_rst_t                     rst;
    vsf_wdt_isr_t                               isr;
    uint32_t                                    timeout_ms;
    uint32_t                                    min_timeout_ms;
    bool                                        enabled;
    uint8_t                                     irqn;
    uint8_t                                     counter;  // Saved counter value for feed
} VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t);
// HW end

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// HW
static uint32_t __vsf_hw_wwdt_calc_timeout(uint32_t timeout_ms, uint32_t min_timeout_ms, uint8_t *divider, uint8_t *counter, uint8_t *window);
// HW end

/*============================ IMPLEMENTATION ================================*/

// HW
static uint32_t __vsf_hw_wwdt_calc_timeout(uint32_t timeout_ms, uint32_t min_timeout_ms, uint8_t *divider, uint8_t *counter, uint8_t *window)
{
    // WWDT timeout = (counter - 0x40) * (divider / PCLK1_freq)
    // WWDT window = (counter - window) * (divider / PCLK1_freq)
    // counter range: 0x40~0x7F (64~127) - initial counter value
    // window range: 0x40~0x7F (64~127) - window register value
    // divider: 4096, 8192, 16384, 32768
    // Note: counter starts from counter value and decrements to window value to trigger interrupt
    //       then continues to 0x40 to cause reset

    // Get PCLK1 frequency
    uint32_t pclk1_freq = vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_APB1);
    if (pclk1_freq == 0) {
        pclk1_freq = 108000000;  // Default PCLK1 frequency
    }

    uint32_t timeout_ticks = (timeout_ms * pclk1_freq) / 1000;
    uint32_t window_ticks = (min_timeout_ms * pclk1_freq) / 1000;

    uint8_t div_idx;
    uint8_t cnt_val, win_val;

    // Try different dividers
    for (div_idx = 0; div_idx <= WWDT_DIV_32768; div_idx++) {
        uint32_t div_value = 4096 << div_idx;
        // timeout_count = (counter - 0x40), so counter = timeout_count + 0x40
        uint32_t timeout_count = (timeout_ticks + div_value - 1) / div_value;
        // window_count = (counter - window), so window = counter - window_count
        uint32_t window_count = (window_ticks + div_value - 1) / div_value;

        // Calculate counter and window values
        // counter must be >= timeout_count + 0x40, and <= 0x7F
        cnt_val = timeout_count + WWDT_COUNTER_MIN;
        if (cnt_val > WWDT_COUNTER_MAX) {
            continue;  // This divider is too small, try next
        }

        // window = counter - window_count, must be >= 0x40 and < counter
        if (cnt_val <= window_count) {
            continue;  // Invalid, window would be <= 0x40
        }
        win_val = cnt_val - window_count;

        // Ensure window is in valid range
        if (win_val < WWDT_COUNTER_MIN) {
            win_val = WWDT_COUNTER_MIN;
        }

        // Check if values are in valid range
        if (cnt_val >= WWDT_COUNTER_MIN && cnt_val <= WWDT_COUNTER_MAX &&
            win_val >= WWDT_COUNTER_MIN && win_val <= WWDT_COUNTER_MAX &&
            cnt_val > win_val) {
            *divider = div_idx;
            *counter = cnt_val;
            *window = win_val;
            return timeout_ms;  // Approximate
        }
    }

    // Use maximum values if timeout is too large
    *divider = WWDT_DIV_32768;
    *counter = WWDT_COUNTER_MAX;
    *window = WWDT_COUNTER_MIN;
    return timeout_ms;
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _init)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != wdt_ptr) && (NULL != cfg_ptr));

#if VSF_HW_WWDT_MASK & (1 << 0)
    wdt_ptr->reg = VSF_HW_WWDT0_REG;
#else
    return VSF_ERR_NOT_SUPPORT;
#endif

    wwdt_type *reg = wdt_ptr->reg;

    // Enable WWDT clock using vsf_hw_clk API
    vsf_hw_clkrst_region_set_bit(wdt_ptr->en);

    wdt_ptr->isr = cfg_ptr->isr;
    wdt_ptr->timeout_ms = cfg_ptr->max_ms;
    wdt_ptr->min_timeout_ms = cfg_ptr->min_ms;
    wdt_ptr->enabled = false;

    // Calculate divider, counter and window
    uint8_t divider, counter, window;
    __vsf_hw_wwdt_calc_timeout(cfg_ptr->max_ms, cfg_ptr->min_ms, &divider, &counter, &window);

    // Save counter value for feed operation
    wdt_ptr->counter = counter;

    // Set divider - directly access WWDT->cfg register
    reg->cfg_bit.div = divider;

    // Set window counter - directly access WWDT->cfg register
    reg->cfg_bit.win = window;

    // Clear any pending interrupt flag
    reg->sts = 0;

    // Enable interrupt if configured - directly access WWDT->cfg register
    if (cfg_ptr->isr.handler_fn != NULL) {
        reg->cfg_bit.rldien = TRUE;
        // Enable NVIC interrupt
        NVIC_SetPriority(wdt_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(wdt_ptr->irqn);
    } else {
        // Disable NVIC interrupt if no handler
        NVIC_DisableIRQ(wdt_ptr->irqn);
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _fini)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    // Disable NVIC interrupt
    NVIC_DisableIRQ(wdt_ptr->irqn);

    // WWDT cannot be disabled once enabled, just mark as disabled
    wdt_ptr->enabled = false;
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _get_configuration)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != wdt_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    cfg_ptr->isr = wdt_ptr->isr;
    cfg_ptr->max_ms = wdt_ptr->timeout_ms;
    cfg_ptr->min_ms = wdt_ptr->min_timeout_ms;

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _enable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    wwdt_type *reg = wdt_ptr->reg;

    // Clear any pending interrupt flag before enabling
    reg->sts = 0;

    // Enable WWDT with calculated counter value - directly access WWDT->ctrl register
    uint8_t divider, counter, window;
    __vsf_hw_wwdt_calc_timeout(wdt_ptr->timeout_ms, wdt_ptr->min_timeout_ms, &divider, &counter, &window);
    reg->ctrl = counter | 0x80;  // WWDT_EN_BIT = 0x80

    wdt_ptr->enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _disable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    // WWDT cannot be disabled once enabled
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _feed)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    wwdt_type *reg = wdt_ptr->reg;

    // Feed WWDT immediately - reload counter to saved initial value
    // Use saved counter value to avoid recalculation delay
    reg->ctrl_bit.cnt = wdt_ptr->counter;
}

static void VSF_MCONNECT(__, VSF_WDT_CFG_IMP_DEVICE_PREFIX, _irqhandler)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(NULL != wdt_ptr);

    wwdt_type *reg = wdt_ptr->reg;

    // Clear interrupt flag
    reg->sts = 0;

    vsf_wdt_isr_t *isr_ptr = &wdt_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_wdt_t *)wdt_ptr);
    }
}

vsf_wdt_capability_t VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _capability)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    // Calculate max timeout
    uint32_t pclk1_freq = vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_APB1);
    if (pclk1_freq == 0) {
        pclk1_freq = 108000000;
    }

    // According to datasheet: TWWDT = TPCLK1 × 4096 × 2^DIV[1:0] × (CNT[5:0] + 1)
    // Maximum timeout: DIV=3 (32768), CNT[5:0]=0x3F (63), so CNT[5:0]+1=64
    // TWWDT = (1000 / PCLK1_freq) × 32768 × 64 = (32768 × 64 × 1000) / PCLK1_freq
    uint32_t max_timeout_ms = (32768UL * 64 * 1000) / pclk1_freq;

    return (vsf_wdt_capability_t) {
        .support_early_wakeup = 0,  // WWDT doesn't support early wakeup
        .support_reset_none   = 1,
        .support_reset_cpu    = 1,
        .support_reset_soc    = 1,
        .support_disable      = 0,  // WWDT cannot be disabled once enabled
        .support_min_timeout  = 1,  // WWDT supports window (min timeout)
        .max_timeout_ms       = max_timeout_ms,
    };
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw wwdt only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY          ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_API_GET_CONFIGURATION   ENABLED
#define VSF_WDT_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, _t)                             \
        VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, __IDX) = {                  \
        .reg      = VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX, __IDX, _REG),   \
        .en       = VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX, __IDX, _EN),    \
        .rst      = VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX, __IDX, _RST),   \
        .irqn     = VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX, __IDX, _IRQN),  \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_UPCASE_PREFIX, __IDX, _IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_WDT_CFG_IMP_DEVICE_PREFIX, _irqhandler)(           \
            &VSF_MCONNECT(VSF_WDT_CFG_IMP_DEVICE_PREFIX, __IDX)                 \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/wdt/wdt_template.inc"
// HW end

#endif /* VSF_HAL_USE_WDT */
/* EOF */

