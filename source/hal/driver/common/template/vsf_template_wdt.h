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

#ifndef __VSF_TEMPLATE_WDT_H__
#define __VSF_TEMPLATE_WDT_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_WDT_CFG_MULTI_CLASS
#   define VSF_WDT_CFG_MULTI_CLASS                      ENABLED
#endif

#if defined(VSF_HW_WDT_COUNT) && !defined(VSF_HW_WDT_MASK)
#   define VSF_HW_WDT_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_WDT_COUNT)
#endif

#if defined(VSF_HW_WDT_MASK) && !defined(VSF_HW_WDT_COUNT)
#   define VSF_HW_WDT_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_WDT_MASK)
#endif

// application code can redefine it
#ifndef VSF_WDT_CFG_PREFIX
#   if VSF_WDT_CFG_MULTI_CLASS == ENABLED
#       define VSF_WDT_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_WDT_COUNT) && (VSF_HW_WDT_COUNT != 0)
#       define VSF_WDT_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_WDT_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_WDT_CFG_FUNCTION_RENAME
#   define VSF_WDT_CFG_FUNCTION_RENAME                  ENABLED
#endif

#ifndef VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE            DISABLED
#endif

#ifndef VSF_WDT_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_WDT_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

#ifndef VSF_WDT_CFG_INHERT_HAL_CAPABILITY
#   define VSF_WDT_CFG_INHERT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_WDT_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            wdt, init,        VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 wdt, fini,        VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             wdt, enable,      VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             wdt, disable,     VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_wdt_capability_t, wdt, capability,  VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 wdt, feed,        VSF_MCONNECT(__prefix_name, _wdt_t) *wdt_ptr)

/*============================ TYPES =========================================*/

#if VSF_WDT_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_wdt_mode_t {
    VSF_WDT_MODE_NO_EARLY_WAKEUP = (0 << 0),
    VSF_WDT_MODE_EARLY_WAKEUP    = (1 << 0),

    VSF_WDT_MODE_RESET_NONE      = (0 << 1),
    VSF_WDT_MODE_RESET_CPU       = (1 << 1),
    VSF_WDT_MODE_RESET_SOC       = (2 << 1),
} vsf_wdt_mode_t;
#endif

enum {
    VSF_WDT_MODE_EARLY_WAKEUP_COUNT = 2,
    VSF_WDT_MODE_EARLY_WAKEUP_MASK  = VSF_WDT_MODE_NO_EARLY_WAKEUP |
                                      VSF_WDT_MODE_EARLY_WAKEUP,

    VSF_WDT_MODE_RESET_COUNT        = 3,
    VSF_WDT_MODE_RESET_MASK         = VSF_WDT_MODE_RESET_NONE |
                                      VSF_WDT_MODE_RESET_CPU |
                                      VSF_WDT_MODE_RESET_CPU,

    VSF_WDT_MODE_MASK_COUNT         = 2,
    VSF_WDT_MODE_ALL_BITS_MASK      = VSF_WDT_MODE_EARLY_WAKEUP_MASK |
                                      VSF_WDT_MODE_RESET_MASK,
};

typedef struct vsf_wdt_t vsf_wdt_t;

typedef void vsf_wdt_isr_handler_t(void *target_ptr, vsf_wdt_t *wdt_ptr);

typedef struct vsf_wdt_isr_t {
    vsf_wdt_isr_handler_t  *handler_fn;
    void                   *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_wdt_isr_t;

//! wdt configuration
typedef struct vsf_wdt_cfg_t {
    vsf_wdt_mode_t  mode;
    uint32_t        max_ms;
    uint32_t        min_ms;            //The window watchdog supports a timeout range
    vsf_wdt_isr_t   isr;
} vsf_wdt_cfg_t;

typedef struct vsf_wdt_capability_t {
#if VSF_WDT_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    uint8_t support_early_wakeup : 1;
    uint8_t support_reset_none   : 1;
    uint8_t support_reset_core   : 1;
    uint8_t support_reset_soc    : 1;
    uint8_t support_disable      : 1;
    uint32_t max_timeout_ms;
} vsf_wdt_capability_t;

typedef struct vsf_wdt_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_WDT_APIS(vsf)
} vsf_wdt_op_t;

#if VSF_WDT_CFG_MULTI_CLASS == ENABLED
struct vsf_wdt_t  {
    const vsf_wdt_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a wdt instance.
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_wdt_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if wdt was initialized, or a negative error code

 @note It is not necessary to call vsf_wdt_fini() to deinitialization.
       vsf_wdt_init() should be called before any other WDT API except vsf_wdt_capability().

 \~chinese
 @brief 初始化一个 wdt 实例
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 @param[in] cfg_ptr: 结构体 vsf_wdt_cfg_t 的指针，参考 @ref vsf_wdt_cfg_t
 @return vsf_err_t: 如果 wdt 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_wdt_fini() 反初始化。
       vsf_wdt_init() 应该在除 vsf_wdt_capability() 之外的其他 WDT API 之前调用。
 */
extern vsf_err_t vsf_wdt_init(vsf_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a wdt instance.
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 @return none

 \~chinese
 @brief 终止一个 wdt 实例
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 @param[in] cfg_ptr: 结构体 vsf_wdt_cfg_t 的指针，参考 @ref vsf_wdt_cfg_t
 @return 无。
 */
extern void vsf_wdt_fini(vsf_wdt_t *wdt_ptr);

/**
 \~english
 @brief enable interrupt masks of wdt instance.
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 @param[in] irq_mask: one or more value of enum @ref vsf_wdt_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 wdt 实例的中断
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_wdt_irq_mask_t 的值的按位或，@ref vsf_wdt_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_wdt_enable(vsf_wdt_t *wdt_ptr);

/**
 \~english
 @brief disable interrupt masks of wdt instance.
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 @param[in] irq_mask: one or more value of enum vsf_wdt_irq_mask_t, @ref vsf_wdt_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 wdt 实例的中断
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_wdt_irq_mask_t 的值的按位或，@ref vsf_wdt_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_wdt_disable(vsf_wdt_t *wdt_ptr);

/**
 \~english
 @brief get the capability of wdt instance.
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t
 @return vsf_wdt_capability_t: return all capability of current wdt @ref vsf_wdt_capability_t

 \~chinese
 @brief 获取 wdt 实例的能力
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 @return vsf_wdt_capability_t: 返回当前 wdt 的所有能力 @ref vsf_wdt_capability_t
 */
extern vsf_wdt_capability_t vsf_wdt_capability(vsf_wdt_t *wdt_ptr);

/**
 \~english
 @brief wdt feed
 @param[in] wdt_ptr: a pointer to structure @ref vsf_wdt_t

 \~chinese
 @brief wdt 喂狗
 @param[in] wdt_ptr: 结构体 vsf_wdt_t 的指针，参考 @ref vsf_wdt_t
 */
extern void vsf_wdt_feed(vsf_wdt_t *wdt_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_WDT_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_wdt_t                VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_t)
#   define vsf_wdt_init(__WDT, ...)   VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_init)                 ((__vsf_wdt_t *)(__WDT), ##__VA_ARGS__)
#   define vsf_wdt_fini(__WDT)        VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_fini)                 ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_enable(__WDT)      VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_enable)               ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_disable(__WDT)     VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_disable)              ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_capability(__WDT)  VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_capability)           ((__vsf_wdt_t *)(__WDT))
#   define vsf_wdt_feed(__WDT, ...)   VSF_MCONNECT(VSF_WDT_CFG_PREFIX, _wdt_feed)                 ((__vsf_wdt_t *)(__WDT))
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_WDT_H__*/
