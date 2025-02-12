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

#ifndef __VSF_TEMPLATE_RTC_H__
#define __VSF_TEMPLATE_RTC_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_RTC_CFG_MULTI_CLASS
#   define VSF_RTC_CFG_MULTI_CLASS                  ENABLED
#endif

#if defined(VSF_HW_RTC_COUNT) && !defined(VSF_HW_RTC_MASK)
#   define VSF_HW_RTC_MASK                          VSF_HAL_COUNT_TO_MASK(VSF_HW_RTC_COUNT)
#endif

#if defined(VSF_HW_RTC_MASK) && !defined(VSF_HW_RTC_COUNT)
#   define VSF_HW_RTC_COUNT                         VSF_HAL_MASK_TO_COUNT(VSF_HW_RTC_MASK)
#endif

// application code can redefine it
#ifndef VSF_RTC_CFG_PREFIX
#   if VSF_RTC_CFG_MULTI_CLASS == ENABLED
#       define VSF_RTC_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_RTC_COUNT) && (VSF_HW_RTC_COUNT != 0)
#       define VSF_RTC_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_RTC_CFG_PREFIX                   vsf
#   endif
#endif

#ifndef VSF_RTC_CFG_FUNCTION_RENAME
#   define VSF_RTC_CFG_FUNCTION_RENAME              ENABLED
#endif

#ifndef VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

#ifndef VSF_RTC_CFG_TIME_TYPE
#   define VSF_RTC_CFG_TIME_TYPE                    uint64_t
#endif

//! Redefine struct vsf_rtc_cfg_t. The vsf_rtc_isr_handler_t type also needs to
//! be redefined For compatibility, members should not be deleted when struct
//! @ref vsf_rtc_cfg_t redefining.
#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

//! Redefine struct vsf_rtc_capability_t.
//! For compatibility, members should not be deleted when struct @ref
//! vsf_rtc_capability_t redefining.
#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

#ifndef VSF_RTC_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_RTC_CFG_INHERIT_HAL_CAPABILITY        ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_RTC_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, init,       VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 rtc, fini,       VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             rtc, enable,     VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             rtc, disable,    VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_rtc_capability_t, rtc, capability, VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, get,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, set,        VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, const vsf_rtc_tm_t *rtc_tm) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, get_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *millisecond_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            rtc, set_time,   VSF_MCONNECT(__prefix_name, _rtc_t) *rtc_ptr, vsf_rtc_time_t second, vsf_rtc_time_t millisecond)

/*============================ TYPES =========================================*/

#if VSF_RTC_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_rtc_irq_mask_t {
    VSF_RTC_IRQ_MASK_ALARM = (1 << 0),
} vsf_rtc_irq_mask_t;
#endif

enum {
    VSF_RTC_IRQ_COUNT         = 1,
    VSF_RTC_IRQ_ALL_BITS_MASK = VSF_RTC_IRQ_MASK_ALARM,
};

typedef VSF_RTC_CFG_TIME_TYPE vsf_rtc_time_t;

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


#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_rtc_t vsf_rtc_t;
typedef void vsf_rtc_isr_handler_t(void *target_ptr, vsf_rtc_t *rtc_ptr, vsf_rtc_irq_mask_t irq_mask);

typedef struct vsf_rtc_isr_t {
    vsf_rtc_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_rtc_isr_t;

//! rtc configuration
typedef struct vsf_rtc_cfg_t {
    vsf_rtc_isr_t isr;
} vsf_rtc_cfg_t;
#endif

#if VSF_RTC_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_rtc_capability_t {
#if VSF_RTC_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    vsf_rtc_irq_mask_t irq_mask;
} vsf_rtc_capability_t;
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

/**
 \~english
 @brief initialize a rtc instance.
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_rtc_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if rtc was initialized, or a negative error code

 @note It is not necessary to call vsf_rtc_fini() to deinitialization.
       vsf_rtc_init() should be called before any other rtc API except vsf_rtc_capability().

 \~chinese
 @brief 初始化一个 rtc 实例
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @param[in] cfg_ptr: 结构体 vsf_rtc_cfg_t 的指针，参考 @ref vsf_rtc_cfg_t
 @return vsf_err_t: 如果 rtc 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_rtc_fini() 反初始化。
       vsf_rtc_init() 应该在除 vsf_rtc_capability() 之外的其他 rtc API 之前调用。
 */
extern vsf_err_t vsf_rtc_init(vsf_rtc_t *rtc_ptr, vsf_rtc_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a rtc instance.
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @return none

 \~chinese
 @brief 终止一个 rtc 实例
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @param[in] cfg_ptr: 结构体 vsf_rtc_cfg_t 的指针，参考 @ref vsf_rtc_cfg_t
 @return 无。
 */
extern void vsf_rtc_fini(vsf_rtc_t *rtc_ptr);

/**
 \~english
 @brief enable interrupt masks of rtc instance.
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] irq_mask: one or more value of enum @ref vsf_rtc_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 rtc 实例的中断
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_rtc_irq_mask_t 的值的按位或，@ref vsf_rtc_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_rtc_enable(vsf_rtc_t *rtc_ptr);

/**
 \~english
 @brief disable interrupt masks of rtc instance.
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] irq_mask: one or more value of enum vsf_rtc_irq_mask_t, @ref vsf_rtc_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 rtc 实例的中断
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_rtc_irq_mask_t 的值的按位或，@ref vsf_rtc_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_rtc_disable(vsf_rtc_t *rtc_ptr);

/**
 \~english
 @brief get the capability of rtc instance.
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @return vsf_rtc_capability_t: return all capability of current rtc @ref vsf_rtc_capability_t

 \~chinese
 @brief 获取 rtc 实例的能力
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @return vsf_rtc_capability_t: 返回当前 rtc 的所有能力 @ref vsf_rtc_capability_t
 */
extern vsf_rtc_capability_t vsf_rtc_capability(vsf_rtc_t *rtc_ptr);

/**
 \~english
 @brief get rtc date time
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] rtc_tm: a pointer to structure @ref vsf_rtc_tm_t
 @return vsf_err_t: VSF_ERR_NONE if rtc get data time was successful, or a negative error code

 \~chinese
 @brief 获取 rtc 日期时间
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @return vsf_err_t: 如果 rtc 获取日期时间成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_rtc_get(vsf_rtc_t *rtc_ptr, vsf_rtc_tm_t *rtc_tm);

/**
 \~english
 @brief set rtc date time
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] rtc_tm: a pointer to structure @ref vsf_rtc_tm_t
 @return vsf_err_t: VSF_ERR_NONE if rtc set data time was successful, or a negative error code

 \~chinese
 @brief 设置 rtc 日期时间
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @return vsf_err_t: 如果 rtc 设置日期时间成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_rtc_set(vsf_rtc_t *rtc_ptr, const vsf_rtc_tm_t *rtc_tm);

/**
 \~english
 @brief get rtc second and milli second of unix time
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] second_ptr: a pointer to type @ref vsf_rtc_time_t
 @param[in] millisecond_ptr: a pointer to type @ref vsf_rtc_time_t
 @return vsf_err_t: VSF_ERR_NONE if rtc get second and millisecond successful, or a negative error code

 \~chinese
 @brief 获取 rtc 的 unix 时间的秒和毫秒
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @param[in] second_ptr: 类型 vsf_rtc_time_t 的指针，参考 @ref vsf_rtc_time_t
 @param[in] millisecond_ptr: 类型 vsf_rtc_time_t 的指针，参考 @ref vsf_rtc_time_t
 @return vsf_err_t: 如果 rtc 获取秒和微秒时间成功返回 VSF_ERR_NONE , 否则返回负数。
 */
/**
 * get rtc date time
 *
 * @param[in] rtc_ptr rtc instance
 * @param[out] rtc data time
 */
extern vsf_err_t vsf_rtc_get_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t *second_ptr, vsf_rtc_time_t *millisecond_ptr);

/**
 \~english
 @brief set rtc date time
 @param[in] rtc_ptr: a pointer to structure @ref vsf_rtc_t
 @param[in] rtc_tm: a pointer to structure @ref vsf_rtc_tm_t
 @return vsf_err_t: VSF_ERR_NONE if rtc set second and millisecond was successful, or a negative error code

 \~chinese
 @brief 设置 rtc 日期时间
 @param[in] rtc_ptr: 结构体 vsf_rtc_t 的指针，参考 @ref vsf_rtc_t
 @return vsf_err_t: 如果 rtc 设置秒和微秒时间成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_rtc_set_time(vsf_rtc_t *rtc_ptr, vsf_rtc_time_t second, vsf_rtc_time_t millisecond);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_RTC_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_rtc_t                  VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_t)
#   define vsf_rtc_init(__RTC, ...)     VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_init)         ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_enable(__RTC)        VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_enable)       ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_disable(__RTC)       VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_disable)      ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_capability(__RTC)    VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_capability)   ((__vsf_rtc_t *)(__RTC))
#   define vsf_rtc_get(__RTC, ...)      VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get)          ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_set(__RTC, ...)      VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set)          ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_get_time(__RTC, ...) VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_get_time)     ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#   define vsf_rtc_set_time(__RTC, ...) VSF_MCONNECT(VSF_RTC_CFG_PREFIX, _rtc_set_time)     ((__vsf_rtc_t *)(__RTC), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_RTC_H__*/
