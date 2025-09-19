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

#if VSF_HAL_USE_RTC == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_${RTC_IP}_RTC_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_${RTC_IP}_RTC_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_${RTC_IP}_RTC_CLASS_IMPLEMENT
// IPCore end
// HW using ${RTC_IP} IPCore driver
#define __VSF_HAL_${RTC_IP}_RTC_CLASS_INHERIT__
// HW end

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_RTC_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${RTC_IP}_RTC_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_RTC_CFG_MULTI_CLASS
#   define VSF_HW_RTC_CFG_MULTI_CLASS           VSF_RTC_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_RTC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_RTC_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end
// IPCore
#define VSF_RTC_CFG_IMP_PREFIX                  vsf_${rtc_ip}
#define VSF_RTC_CFG_IMP_UPCASE_PREFIX           VSF_${RTC_IP}
// IPCore end

/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) {
#if VSF_HW_RTC_CFG_MULTI_CLASS == ENABLED
    vsf_rtc_t               vsf_rtc;
#endif
    void                    *reg;
    vsf_rtc_isr_t           isr;
} VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw rtc only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_init)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != rtc_ptr) && (NULL != cfg_ptr));
    // configure according to cfg_ptr
    rtc_ptr->isr = cfg_ptr->isr;
    // configure interrupt according to cfg_ptr->isr
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_fini)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_enable)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_disable)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return fsm_rt_cpl;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_tm_t *rtc_tm
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    const vsf_rtc_tm_t *rtc_tm
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_get_time)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_time_t *second_ptr,
    vsf_rtc_time_t *millisecond_ptr
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_set_time)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr,
    vsf_rtc_time_t second,
    vsf_rtc_time_t millisecond
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return VSF_ERR_NONE;
}

vsf_rtc_capability_t VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_capability)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(rtc_ptr != NULL);

    return (vsf_rtc_capability_t) {
        .irq_mask = VSF_RTC_IRQ_MASK_ALARM,
    };
}

static vsf_rtc_irq_mask_t VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_get_irq_mask)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_irqhandler)(
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t) *rtc_ptr
) {
    VSF_HAL_ASSERT(NULL != rtc_ptr);

    vsf_rtc_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_get_irq_mask)(rtc_ptr);
    vsf_rtc_isr_t *isr_ptr = &rtc_ptr->isr;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_rtc_t *)rtc_ptr, irq_mask);
    }
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw rtc only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_RTC_CFG_REIMPLEMENT_API_CAPABILITY             ENABLED
#define VSF_RTC_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc_t)                                \
        VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc, __IDX) = {                   \
        .reg                = VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC, __IDX,_REG),\
        __HAL_OP                                                                \
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_RTC_CFG_IMP_UPCASE_PREFIX, _RTC, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_RTC_CFG_IMP_PREFIX, _rtc_irqhandler)(              \
            &VSF_MCONNECT(VSF_RTC_CFG_IMP_PREFIX, _rtc, __IDX)                  \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/rtc/rtc_template.inc"
// HW end

#endif /* VSF_HAL_USE_RTC */
