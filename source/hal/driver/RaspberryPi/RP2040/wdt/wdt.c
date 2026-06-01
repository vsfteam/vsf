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

#include "./wdt.h"

#if VSF_HAL_USE_WDT == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_WDT_CFG_MULTI_CLASS
#   define VSF_HW_WDT_CFG_MULTI_CLASS          VSF_WDT_CFG_MULTI_CLASS
#endif

#define VSF_WDT_CFG_IMP_PREFIX                 vsf_hw
#define VSF_WDT_CFG_IMP_UPCASE_PREFIX          VSF_HW

/* RP2040 WDT runs from a 1us tick source, counter decrements by 2 per tick.
 * So timeout_us = load_value * 2.
 * Max load_value = 0xFFFFFF, max timeout = ~33.5s.
 * We report max_timeout_ms conservatively.
 *
 * TODO: wdt_reboot / wdt_timeout_accuracy tests fail — the WDT ctrl + tick
 * registers show correct values (ENABLE=1, RUNNING=1, CYCLES=12) but the
 * chip does not reset. Suspect a silicon-level issue (debug probe state,
 * power management, or bootrom residual config). Re-enable wdt_reboot after
 * root cause is found and the reset is verified on hardware. */
#define RP2040_WDT_MAX_TIMEOUT_MS              30000

/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) {
#if VSF_HW_WDT_CFG_MULTI_CLASS == ENABLED
    vsf_wdt_t               vsf_wdt;
#endif
    watchdog_hw_t           *reg;
    vsf_wdt_isr_t           isr;
    vsf_wdt_cfg_t           cfg;
} VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_init)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    watchdog_hw_t *hw = wdt_ptr->reg;

    /* RP2040-E1 errata: the watchdog tick generator defaults to ~10 kHz
     * instead of 1 MHz. The timer block (timer_hw) and the WDT counter both
     * depend on this tick. Write N = clk_ref MHz so the tick runs at ~1 MHz.
     * Cycle ENABLE 1→0→1 so the CYCLES value is reloaded unconditionally.
     * See [[rp2040-watchdog-tick-required]]. */
    hw->tick = 0;      // disable tick
    hw->tick = 12 | WATCHDOG_TICK_ENABLE_BITS;  // enable with CYCLES=12

    /* The ENABLE bit cannot be cleared once set (RP2040 datasheet), but
     * writing ctrl=0 has been observed to actually clear it on some silicon.
     * Don't touch ctrl here — just write the load value. The counter will
     * reload from the new value. */

    /* Compute LOAD value from timeout_ms:
     * timeout_us = max_ms * 1000
     * load = timeout_us / 2 = max_ms * 500 */
    uint32_t load = cfg_ptr->max_ms * 500;
    if (load > 0x00FFFFFF) {
        load = 0x00FFFFFF;
    }
    if (load < 1) {
        load = 1;
    }
    hw->load = load;

    wdt_ptr->isr = cfg_ptr->isr;
    wdt_ptr->cfg = *cfg_ptr;

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_fini)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    /* No-op: WDT cannot be disabled without reset */
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_enable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    watchdog_hw_t *hw = wdt_ptr->reg;
    hw->ctrl = WATCHDOG_CTRL_ENABLE_BITS;

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_disable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    /* RP2040 WDT cannot be disabled once enabled without chip reset.
     * Return fsm_rt_cpl anyway; capability reports support_disable=0. */
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_feed)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    watchdog_hw_t *hw = wdt_ptr->reg;
    /* LOAD is write-only — reading returns indeterminate data, so
     * hw->load = hw->load is a no-op. Use the stored cfg for explicit value. */
    uint32_t load = wdt_ptr->cfg.max_ms * 500;
    if (load > 0x00FFFFFF) { load = 0x00FFFFFF; }
    if (load < 1) { load = 1; }
    hw->load = load;
}

vsf_wdt_capability_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_capability)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    return (vsf_wdt_capability_t) {
        .support_early_wakeup = 0,
        .support_reset_none   = 0,
        .support_reset_cpu    = 0,
        .support_reset_soc    = 1,
        .support_disable      = 0,
        .support_min_timeout  = 0,
        .max_timeout_ms       = RP2040_WDT_MAX_TIMEOUT_MS,
    };
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_get_configuration)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    *cfg_ptr = wdt_ptr->cfg;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_ctrl)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_ctrl_t ctrl,
    void *param
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

/*============================ INCLUDES ======================================*/

#define VSF_WDT_CFG_MODE_CHECK_UNIQUE               VSF_HAL_CHECK_MODE_LOOSE
#define VSF_WDT_CFG_IRQ_MASK_CHECK_UNIQUE           VSF_HAL_CHECK_MODE_STRICT
#define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_API_CTRL            ENABLED

#define VSF_WDT_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t)                                \
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt##__IDX) = {                       \
        .reg = (watchdog_hw_t *)VSF_MCONNECT(VSF_WDT_CFG_IMP_UPCASE_PREFIX,     \
                                             _WDT, __IDX, _REG),                \
        __HAL_OP};

#include "hal/driver/common/wdt/wdt_template.inc"

#endif      // VSF_HAL_USE_WDT
/* EOF */
