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

#ifndef __HAL_DRIVER_RTC_INTERFACE_H__
#define __HAL_DRIVER_RTC_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_RTC_CFG_MULTI_CLASS
#   define VSF_RTC_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_RTC_CFG_MULTI_CLASS is enabled
#ifndef VSF_RTC_CFG_IMPLEMENT_OP
#   if VSF_RTC_CFG_MULTI_CLASS == ENABLED
#       define VSF_RTC_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_RTC_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_RTC_CFG_PREFIX: use for macro vsf_rtc_{init, enable, ...}
#ifndef VSF_RTC_CFG_PREFIX
#   if VSF_RTC_CFG_MULTI_CLASS == ENABLED
#       define VSF_RTC_CFG_PREFIX               vsf
#   elif defined(VSF_HW_RTC_COUNT) && (VSF_HW_RTC_COUNT != 0)
#       define VSF_RTC_CFG_PREFIX               vsf_hw
#   endif
#endif

#ifndef VSF_RTC_CFG_FUNCTION_RENAME
#   define VSF_RTC_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_RTC_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_RTC_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

#ifndef VSF_RTC_CFG_REIMPLEMENT_IRQ_TYPE
#   define VSF_RTC_CFG_REIMPLEMENT_IRQ_TYPE     DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rtc, init,       VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, rtc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         rtc, enable,     VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         rtc, disable,    VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, rtc_capability_t, rtc, capability, VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rtc, get,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rtc, set,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, const vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rtc, get_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, time_t *second_ptr, time_t *millisecond_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        rtc, set_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, time_t second, time_t millisecond)

/*============================ TYPES =========================================*/

#if VSF_RTC_CFG_REIMPLEMENT_IRQ_TYPE == DISABLED
typedef enum em_rtc_irq_mask_t {
    VSF_RTC_IRQ_MASK_ALARM = (1 << 0),
} em_rtc_irq_mask_t;
#endif

typedef struct vsf_rtc_tm_t {
    uint8_t tm_sec;         // [0 .. 59]
    uint8_t tm_min;         // [0 .. 59]
    uint8_t tm_hour;        // [0 .. 23]
    uint8_t tm_mday;        // [1 .. 31]
    uint8_t tm_wday;        // [1 .. 7]     - [Sunday, Monday, ..., Saturday]
    uint8_t tm_mon;         // [1 .. 12]    - [January -- December]
    uint16_t tm_year;       // [1900 .. ]
    uint16_t tm_ms;
} vsf_rtc_tm_t;

typedef struct vsf_rtc_t vsf_rtc_t;

typedef void vsf_rtc_isr_handler_t(void *target_ptr, em_rtc_irq_mask_t irq_mask, vsf_rtc_t *rtc_ptr);

typedef struct vsf_rtc_isr_t {
    vsf_rtc_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_rtc_isr_t;

//! rtc configuration
typedef struct rtc_cfg_t {
    vsf_rtc_isr_t isr;
} rtc_cfg_t;

#if VSF_RTC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct rtc_capability_t {
    inherit(peripheral_capability_t)
} rtc_capability_t;
#endif

typedef struct vsf_rtc_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_RTC_APIS(vsf)
} vsf_rtc_op_t;

#if VSF_RTC_CFG_MULTI_CLASS == ENABLED
struct vsf_rtc_t  {
    const vsf_rtc_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr);
extern fsm_rt_t vsf_rtc_enable(vsf_rtc_t *rtc_ptr);
extern fsm_rt_t vsf_rtc_disable(vsf_rtc_t *rtc_ptr);
extern rtc_capability_t vsf_rtc_capability(vsf_rtc_t *rtc_ptr);

/**
 * get rtc date time
 *
 * @param[in] rtc_ptr rtc instance
 * @param[out] rtc data time
 */
extern vsf_err_t vsf_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm);

/**
 * set rtc date time
 *
 * @param[in] rtc_ptr rtc instance
 * @param[in] rtc data time
 */
extern vsf_err_t vsf_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *rtc_tm);

extern vsf_err_t vsf_rtc_get_time(vsf_rtc_t *rtc_ptr, time_t *second_ptr, time_t *millisecond_ptr);

extern vsf_err_t vsf_rtc_set_time(vsf_rtc_t *rtc_ptr, time_t second, time_t millisecond);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_RTC_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_rtc_init(__RTC, ...)                                             \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_init)         ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC, ##__VA_ARGS__)
#   define vsf_rtc_enable(__RTC)                                                \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_enable)       ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC)
#   define vsf_rtc_disable(__RTC)                                               \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_disable)      ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC)
#   define vsf_rtc_capability(__RTC)                                            \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_capability)   ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC)
#   define vsf_rtc_get(__RTC, ...)                                              \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get)          ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC, ##__VA_ARGS__)
#   define vsf_rtc_set(__RTC, ...)                                              \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set)          ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC, ##__VA_ARGS__)
#   define vsf_rtc_get_time(__RTC, ...)                                         \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get_second)   ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC, ##__VA_ARGS__)
#   define vsf_rtc_set_time(__RTC, ...)                                         \
        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set_second)   ((VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t) *)__RTC, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_RTC_INTERFACE_H__*/
