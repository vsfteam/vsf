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

#ifndef __VSF_TEMPLATE_TIMER_H__
#define __VSF_TEMPLATE_TIMER_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_TIMER_CFG_MULTI_CLASS
#   define VSF_TIMER_CFG_MULTI_CLASS                ENABLED
#endif

#if defined(VSF_HW_TIMER_COUNT) && !defined(VSF_HW_TIMER_MASK)
#   define VSF_HW_TIMER_MASK                        VSF_HAL_COUNT_TO_MASK(VSF_HW_TIMER_COUNT)
#endif

#if defined(VSF_HW_TIMER_MASK) && !defined(VSF_HW_TIMER_COUNT)
#   define VSF_HW_TIMER_COUNT                       VSF_HAL_MASK_TO_COUNT(VSF_HW_TIMER_MASK)
#endif

// application code can redefine it
#ifndef VSF_TIMER_CFG_PREFIX
#   if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
#       define VSF_TIMER_CFG_PREFIX                 vsf
#   elif defined(VSF_HW_TIMER_COUNT) && (VSF_HW_TIMER_COUNT != 0)
#       define VSF_TIMER_CFG_PREFIX                 vsf_hw
#   else
#       define VSF_TIMER_CFG_PREFIX                 vsf
#   endif
#endif

#ifndef VSF_TIMER_CFG_FUNCTION_RENAME
#   define VSF_TIMER_CFG_FUNCTION_RENAME            ENABLED
#endif

#ifndef VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK  DISABLED
#endif

#ifndef VSF_TIMER_CFG_INHERT_HAL_CAPABILITY
#   define VSF_TIMER_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_TIMER_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, init,        VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, fini,        VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, enable,      VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               timer, disable,     VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_enable,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   timer, irq_disable, VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, vsf_timer_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_timer_capability_t, timer, capability,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, set_period,  VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr, uint32_t period) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              timer, pwm_set,     VSF_MCONNECT(__prefix_name, _timer_t) *timer_ptr,  uint8_t channel, uint32_t period, uint32_t pulse)

/*============================ TYPES =========================================*/

// TODO: Add more feature support, for example match interrupt

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_timer_mode_t {
    VSF_TIMER_MODE_ONESHOT      = (0x00 << 0),
    VSF_TIMER_MODE_CONTINUES    = (0x01 << 0),

    VSF_TIMER_MODE_PWM          = (0x01 << 1),
    VSF_TIMER_MODE_NO_PWM       = (0x00 << 1),
} vsf_timer_mode_t;
#endif

enum {
    VSF_TIMER_MODE_RESTART_COUNT = 2,
    VSF_TIMER_MODE_RESTART_MASK  = VSF_TIMER_MODE_ONESHOT |
                                   VSF_TIMER_MODE_CONTINUES,

    VSF_TIMER_MODE_PWM_COUNT     = 2,
    VSF_TIMER_MODE_PWM_MASK      = VSF_TIMER_MODE_PWM |
                                   VSF_TIMER_MODE_NO_PWM,

    VSF_TIMER_MODE_MASK_COUNT    = 3,
    VSF_TIMER_MODE_ALL_BITS_MASK = VSF_TIMER_MODE_RESTART_MASK,
};

#if VSF_TIMER_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_timer_irq_mask_t {
    VSF_TIMER_IRQ_MASK_OVERFLOW = (0x01 << 0),
} vsf_timer_irq_mask_t;
#endif

enum {
    VSF_TIMER_IRQ_COUNT         = 1,
    VSF_TIMER_IRQ_ALL_BITS_MASK = VSF_TIMER_IRQ_MASK_OVERFLOW,
};

typedef struct vsf_timer_t vsf_timer_t;

typedef void vsf_timer_isr_handler_t(void *target_ptr,
                                     vsf_timer_t *timer_ptr,
                                     vsf_timer_irq_mask_t irq_mask);

typedef struct vsf_timer_isr_t {
    vsf_timer_isr_handler_t *handler_fn;
    void *target_ptr;
    vsf_arch_prio_t prio;
} vsf_timer_isr_t;

//! timer configuration
typedef struct vsf_timer_cfg_t {
    vsf_timer_mode_t mode;
    // counting period
    uint32_t period;
    union {
        uint32_t freq;
        uint32_t min_freq;
    };

    vsf_timer_isr_t isr;
} vsf_timer_cfg_t;


typedef struct vsf_timer_capability_t {
#if VSF_TIMER_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    vsf_timer_irq_mask_t irq_mask;

    uint8_t timer_bitlen;

    uint8_t support_pwm   : 1;
    uint8_t pwm_channel_cnt;

} vsf_timer_capability_t;

typedef struct vsf_timer_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_TIMER_APIS(vsf)
} vsf_timer_op_t;

#if VSF_TIMER_CFG_MULTI_CLASS == ENABLED
struct vsf_timer_t  {
    const vsf_timer_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_timer_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if timer was initialized, or a negative error code

 @note It is not necessary to call vsf_timer_fini() to deinitialization.
       vsf_timer_init() should be called before any other timer API except vsf_timer_capability().

 \~chinese
 @brief 初始化一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] cfg_ptr: 结构体 vsf_timer_cfg_t 的指针，参考 @ref vsf_timer_cfg_t
 @return vsf_err_t: 如果 timer 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_timer_fini() 反初始化。
       vsf_timer_init() 应该在除 vsf_timer_capability() 之外的其他 timer API 之前调用。
 */
extern vsf_err_t vsf_timer_init(vsf_timer_t *timer_ptr, vsf_timer_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return none

 \~chinese
 @brief 终止一个 timer 实例
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return 无。
 */
extern void vsf_timer_fini(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief enable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum @ref vsf_timer_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_timer_enable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief disable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum vsf_timer_irq_mask_t, @ref vsf_timer_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_timer_disable(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief get the capability of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @return vsf_timer_capability_t: return all capability of current timer @ref vsf_timer_capability_t

 \~chinese
 @brief 获取 timer 实例的能力
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @return vsf_timer_capability_t: 返回当前 timer 的所有能力 @ref vsf_timer_capability_t
 */
extern vsf_timer_capability_t vsf_timer_capability(vsf_timer_t *timer_ptr);

/**
 \~english
 @brief enable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum @ref vsf_timer_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 */
extern void vsf_timer_irq_enable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of timer instance.
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] irq_mask: one or more value of enum vsf_timer_irq_mask_t, @ref vsf_timer_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 timer 实例的中断
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_timer_irq_mask_t 的值的按位或，@ref vsf_timer_irq_mask_t
 @return 无。
 */
extern void vsf_timer_irq_disable(vsf_timer_t *timer_ptr, vsf_timer_irq_mask_t irq_mask);

/**
 \~english
 @brief timer set period (maximum count)
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] period: timer period width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the timer set period was successfully, or a negative error code

 \~chinese
 @brief timer 设置一个定时器的周期（最大计数值）
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] period: timer 周期宽度 (时钟计数)
 @return vsf_err_t: 如果 timer 设置周期成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_timer_set_period(vsf_timer_t *timer_ptr, uint32_t period);

/**
 \~english
 @brief timer pwm set the period width and pulse width for a channel
 @param[in] timer_ptr: a pointer to structure @ref vsf_timer_t
 @param[in] channel: timer pwm channel
 @param[in] period: timer pwm period width (in clock counter)
 @param[in] pulse: timer pwm pulse width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the timer pwm  set was successfully, or a negative error code

 \~chinese
 @brief timer pwm  设置一个通道的周期和宽度
 @param[in] timer_ptr: 结构体 vsf_timer_t 的指针，参考 @ref vsf_timer_t
 @param[in] channel: timer pwm 通道
 @param[in] period: timer pwm 周期宽度 (时钟计数)
 @param[in] pulse: timer pwm 脉冲宽度 (时钟计数)
 @return vsf_err_t: 如果 timer pwm 设置成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_timer_pwm_set(vsf_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_TIMER_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_timer_t                      VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t)
#   define vsf_timer_init(__TIME, ...)        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_init)        ((__vsf_timer_t *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_fini(__TIME, ...)        VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_fini)        ((__vsf_timer_t *)__TIME)
#   define vsf_timer_enable(__TIME)           VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_enable)      ((__vsf_timer_t *)__TIME)
#   define vsf_timer_disable(__TIME)          VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_disable)     ((__vsf_timer_t *)__TIME)
#   define vsf_timer_capability(__TIME)       VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_capability)  ((__vsf_timer_t *)__TIME)
#   define vsf_timer_irq_enable(__TIME, ...)  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_enable)  ((__vsf_timer_t *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_irq_disable(__TIME, ...) VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_irq_disable) ((__vsf_timer_t *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_set_period(__TIME, ...)  VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_set_period)  ((__vsf_timer_t *)__TIME, ##__VA_ARGS__)
#   define vsf_timer_pwm_set(__TIME, ...)     VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_pwm_set)     ((__vsf_timer_t *)__TIME, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_TIMER_H__*/
