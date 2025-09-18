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

#if VSF_HAL_USE_WDT == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${WDT_IP}_WDT_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${WDT_IP}_WDT_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${WDT_IP}_WDT_CLASS_IMPLEMENT
// IPCore end
// HW using ${WDT_IP} IPCore driver
#define __VSF_HAL_${WDT_IP}_WDT_CLASS_INHERIT__
// HW end

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
// IPCore
#define VSF_WDT_CFG_IMP_PREFIX                  vsf_${wdt_ip}
#define VSF_WDT_CFG_IMP_UPCASE_PREFIX           VSF_${WDT_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) {
#if VSF_HW_WDT_CFG_MULTI_CLASS == ENABLED
    vsf_wdt_t               vsf_wdt;
#endif
    void                    *reg;
    vsf_wdt_isr_t           isr;
} VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw wdt only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_init)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr,
    vsf_wdt_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != wdt_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    wdt_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_fini)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_enable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_disable)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    return fsm_rt_cpl;
}

vsf_wdt_capability_t VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_capability)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);

    return (vsf_wdt_capability_t) {
        .support_early_wakeup = 1,
        .support_reset_none   = 1,
        .support_reset_cpu   = 1,
        .support_reset_soc    = 1,
        .support_disable      = 1,
        .max_timeout_ms       = 100 * 1000,
    };
}

void VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_feed)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(wdt_ptr != NULL);
}

static void VSF_MCONNECT(__, VSF_WDT_CFG_IMP_PREFIX, _wdt_irqhandler)(
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t) *wdt_ptr
) {
    VSF_HAL_ASSERT(NULL != wdt_ptr);

    vsf_wdt_isr_t *isr_ptr = &wdt_ptr->isr;
    if (isr_ptr->handler_fn != NULL) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_wdt_t *)wdt_ptr);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw wdt only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_WDT_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt_t)                                \
        VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_WDT_CFG_IMP_UPCASE_PREFIX, _WDT, __IDX,_REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_WDT_CFG_IMP_UPCASE_PREFIX, _WDT, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_WDT_CFG_IMP_PREFIX, _wdt_irqhandler)(              \
            &VSF_MCONNECT(VSF_WDT_CFG_IMP_PREFIX, _wdt, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/wdt/wdt_template.inc"
// HW end

#endif /* VSF_HAL_USE_WDT */
