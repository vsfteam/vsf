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

#if !defined(RTC_HW_MASK) && defined(RTC_HW_COUNT)
#   define RTC_HW_MASK                  ((1 << RTC_HW_COUNT) - 1)
#endif

#if VSF_RTC_CFG_MULTI_CLASS != ENABLED
#   ifndef VSF_RTC_CFG_PREFIX
#       define VSF_RTC_CFG_PREFIX       vsf_hw
#   endif

#   ifndef VSF_RTC_CFG_REAL_PREFIX
#       define VSF_RTC_CFG_REAL_PREFIX  VSF_RTC_CFG_PREFIX
#   endif

#   define ____VSF_RTC_WRAPPER(__header, __api)   __header ## _ ## __api
#   define __VSF_RTC_WRAPPER(__header, __api)     ____VSF_RTC_WRAPPER(__header, __api)
#   define vsf_rtc_init                 __VSF_RTC_WRAPPER(VSF_RTC_CFG_REAL_PREFIX, rtc_init)
#   define vsf_rtc_enable               __VSF_RTC_WRAPPER(VSF_RTC_CFG_REAL_PREFIX, rtc_enable)
#   define vsf_rtc_disable              __VSF_RTC_WRAPPER(VSF_RTC_CFG_REAL_PREFIX, rtc_disable)
#   define vsf_rtc_get                  __VSF_RTC_WRAPPER(VSF_RTC_CFG_REAL_PREFIX, rtc_get)
#   define vsf_rtc_set                  __VSF_RTC_WRAPPER(VSF_RTC_CFG_REAL_PREFIX, rtc_set)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_INIT(__RTC_PTR, __CFG_PTR)                                      \
            vsf_rtc_init((__RTC_PTR), (__CFG_PTR))
#define VSF_RTC_ENABLE(__RTC_PTR)                                               \
            vsf_rtc_enable(__RTC_PTR)
#define VSF_RTC_DISABLE(__RTC_PTR)                                              \
            vsf_rtc_disable(__RTC_PTR)
#define VSF_RTC_GET(__RTC_PTR, __TM)                                            \
            vsf_rtc_get((__RTC_PTR), (__TM))
#define VSF_RTC_SET(__RTC_PTR, __TM)                                            \
            vsf_rtc_set((__RTC_PTR), (__TM))

/*============================ TYPES =========================================*/

typedef enum vsf_rtc_irq_type_t{
    VSF_RTC_IRQ_ALARM_MASK          = (1 << 0),
} vsf_rtc_irq_type_t;

typedef struct vsf_rtc_t vsf_rtc_t;

typedef void vsf_rtc_isrhandler_t(  void *target_ptr,
                                    vsf_rtc_irq_type_t type,
                                    vsf_rtc_t *rtc_ptr);

typedef struct vsf_rtc_tm_t {
    uint8_t tm_sec;         // [0 .. 59]
    uint8_t tm_min;         // [0 .. 59]
    uint8_t tm_hour;        // [0 .. 23]
    uint8_t tm_day;         // [1 .. 31]    - [January -- December]
    uint8_t tm_wday;        // [1 .. 7]     - [Monday -- Sunday]
    uint8_t tm_mon;         // [1 .. 12]
    uint16_t tm_year;       // [1900 .. ]
} vsf_rtc_tm_t;

typedef struct vsf_rtc_isr_t {
    vsf_rtc_isrhandler_t   *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_rtc_isr_t;

//! \name rtc configuration
//! @{
typedef struct rtc_cfg_t rtc_cfg_t;
struct rtc_cfg_t {
    vsf_rtc_isr_t isr;
};
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, rtc_cfg_t *cfg_ptr);

extern fsm_rt_t vsf_rtc_enable(vsf_rtc_t *rtc_ptr);
extern fsm_rt_t vsf_rtc_disable(vsf_rtc_t *rtc_ptr);

/**
 * get rtc date time
 *
 * @param[in] rtc_ptr rtc instance
 * @param[out] rtc data time
 */
extern vsf_err_t vsf_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *tm);

/**
 * set rtc date time
 *
 * @param[in] rtc_ptr rtc instance
 * @param[in] rtc data time
 */
extern vsf_err_t vsf_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *tm);

#ifdef __cplusplus
}
#endif

#endif
