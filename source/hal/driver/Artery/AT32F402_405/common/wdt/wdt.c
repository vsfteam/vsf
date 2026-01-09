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

/*============================ MACROS ========================================*/

/*\note VSF_HW_WDT_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${WDT_IP}_WDT_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_WDT_CFG_MULTI_CLASS
#   define VSF_HW_WDT_CFG_MULTI_CLASS           VSF_WDT_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_WDT_CFG_IMP_PREFIX                  vsf_hw
#define VSF_WDT_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end

// WDT clock frequency (LICK = 40kHz)
#define WDT_CLK_FREQ_HZ                         40000

// WDT divider values
#define WDT_DIV_4                               0
#define WDT_DIV_8                               1
#define WDT_DIV_16                              2
#define WDT_DIV_32                              3
#define WDT_DIV_64                              4
#define WDT_DIV_128                             5
#define WDT_DIV_256                             6

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) {
#if VSF_HW_WDT_CFG_MULTI_CLASS == ENABLED
    vsf_wdt_t                                   vsf_wdt;
#endif
    wdt_type                                    *reg;
    vsf_wdt_isr_t                               isr;
    uint32_t                                    timeout_ms;
    bool                                        enabled;
} VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t);
// HW end

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// HW
static uint32_t __vsf_hw_wdt_calc_timeout(uint32_t timeout_ms, uint8_t *divider, uint16_t *reload);
// HW end

/*============================ IMPLEMENTATION ================================*/

// HW
static uint32_t __vsf_hw_wdt_calc_timeout(uint32_t timeout_ms, uint8_t *divider, uint16_t *reload)
{
    // WDT timeout = reload_value * (divider / LICK_freq)
    // LICK_freq = 40000 Hz
    // reload_value range: 0x0000~0x0FFF (0~4095)
    // divider: 4, 8, 16, 32, 64, 128, 256

    uint32_t ticks = (timeout_ms * WDT_CLK_FREQ_HZ) / 1000;
    uint8_t div_idx;
    uint16_t reload_val;

    // Try different dividers to find the best match
    for (div_idx = 0; div_idx <= WDT_DIV_256; div_idx++) {
        uint32_t div_value = 4 << div_idx;
        reload_val = (ticks + div_value - 1) / div_value;  // Round up

        if (reload_val <= 0x0FFF) {
            *divider = div_idx;
            *reload = reload_val - 1;  // Hardware uses reload_val - 1
            return ((reload_val * div_value * 1000) / WDT_CLK_FREQ_HZ);
        }
    }

    // Use maximum values if timeout is too large
    *divider = WDT_DIV_256;
    *reload = 0x0FFF;
    return ((0x1000 * 256 * 1000) / WDT_CLK_FREQ_HZ);
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_init)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != wdt_ptr) && (NULL != cfg_ptr));

#if VSF_HW_WDT_MASK & (1 << 0)
    wdt_ptr->reg = VSF_HW_WDT0_REG;
#else
    return VSF_ERR_NOT_SUPPORT;
#endif

    wdt_type *reg = wdt_ptr->reg;

    // IWDG only supports reset mode, not interrupt mode
    if ((cfg_ptr->mode & VSF_WDT_MODE_RESET_MASK) == VSF_WDT_MODE_RESET_NONE) {
        return VSF_ERR_NOT_SUPPORT;
    }

    // IWDG does not support early wakeup
    if (cfg_ptr->mode & VSF_WDT_MODE_EARLY_WAKEUP) {
        return VSF_ERR_NOT_SUPPORT;
    }

    wdt_ptr->isr = cfg_ptr->isr;
    wdt_ptr->timeout_ms = cfg_ptr->max_ms;
    wdt_ptr->enabled = false;

    // Disable register write protection
    reg->cmd = 0x5555;  // WDT_CMD_UNLOCK

    // Calculate divider and reload value
    uint8_t divider;
    uint16_t reload;
    __vsf_hw_wdt_calc_timeout(cfg_ptr->max_ms, &divider, &reload);

    // Set divider
    reg->div_bit.div = divider;

    // Wait for divider update flag
    while ((reg->sts & 0x0001) == 0);  // WDT_DIVF_UPDATE_FLAG

    // Set reload value
    reg->rld = reload;

    // Wait for reload value update flag
    while ((reg->sts & 0x0002) == 0);  // WDT_RLDF_UPDATE_FLAG

    // Reload counter
    reg->cmd = 0xAAAA;  // WDT_CMD_RELOAD

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_fini)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    // WDT cannot be disabled once enabled, so we just mark it as disabled
    wdt_ptr->enabled = false;
}

vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_get_configuration)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != wdt_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    cfg_ptr->isr = wdt_ptr->isr;
    cfg_ptr->max_ms = wdt_ptr->timeout_ms;
    cfg_ptr->min_ms = 0;  // WDT doesn't use min timeout

    return VSF_ERR_NONE;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_enable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    wdt_type *reg = wdt_ptr->reg;

    // Enable WDT
    reg->cmd = 0xCCCC;  // WDT_CMD_ENABLE

    wdt_ptr->enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_disable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    // WDT cannot be disabled once enabled in hardware
    // We just mark it as disabled in software
    wdt_ptr->enabled = false;
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_feed)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    wdt_type *reg = wdt_ptr->reg;

    // Feed WDT
    reg->cmd = 0xAAAA;  // WDT_CMD_RELOAD
}

vsf_wdt_capability_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_capability)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    return (vsf_wdt_capability_t) {
        .support_early_wakeup = 0,  // IWDG does not support early wakeup interrupt
        .support_reset_none   = 0,  // IWDG does not support interrupt mode, only reset
        .support_reset_cpu    = 1,  // IWDG supports CPU reset
        .support_reset_soc    = 1,  // IWDG supports SOC reset
        .support_disable      = 0,  // WDT cannot be disabled once enabled
        .support_min_timeout  = 0,  // WDT doesn't support min timeout
        .max_timeout_ms       = ((0x1000 * 256 * 1000) / WDT_CLK_FREQ_HZ),  // Max timeout with max divider and reload
    };
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw wdt only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_WDT_CFG_REIMPLEMENT_API_GET_CONFIGURATION ENABLED
#define VSF_WDT_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t)                                \
        VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_WDT_CFG_IMP_UPCASE_PREFIX, _WDT, __IDX, _REG),\
        __HAL_OP                                                                \
    };
#include "hal/driver/common/wdt/wdt_template.inc"
// HW end

#endif /* VSF_HAL_USE_WDT */
/* EOF */
