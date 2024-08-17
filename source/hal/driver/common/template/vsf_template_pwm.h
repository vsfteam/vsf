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

#ifndef __VSF_TEMPLATE_PWM_H__
#define __VSF_TEMPLATE_PWM_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_PWM_CFG_MULTI_CLASS
#   define VSF_PWM_CFG_MULTI_CLASS              ENABLED
#endif

#if defined(VSF_HW_PWM_COUNT) && !defined(VSF_HW_PWM_MASK)
#   define VSF_HW_PWM_MASK                      VSF_HAL_COUNT_TO_MASK(VSF_HW_PWM_COUNT)
#endif

#if defined(VSF_HW_PWM_MASK) && !defined(VSF_HW_PWM_COUNT)
#   define VSF_HW_PWM_COUNT                     VSF_HAL_MASK_TO_COUNT(VSF_HW_PWM_MASK)
#endif

// application code can redefine it
#ifndef VSF_PWM_CFG_PREFIX
#   if VSF_PWM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PWM_CFG_PREFIX               vsf
#   elif defined(VSF_HW_PWM_COUNT) && (VSF_HW_PWM_COUNT != 0)
#       define VSF_PWM_CFG_PREFIX               vsf_hw
#   else
#       define VSF_PWM_CFG_PREFIX               vsf
#   endif
#endif

#ifndef VSF_PWM_CFG_FUNCTION_RENAME
#   define VSF_PWM_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_PWM_CFG_INHERT_HAL_CAPABILITY
#   define VSF_PWM_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_APIS(__prefix_name)                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, init,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  pwm, fini,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, enable,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, disable,        VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pwm_capability_t,  pwm, capability,     VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set,            VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,              pwm, get_freq,       VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)

/*============================ TYPES =========================================*/

typedef struct vsf_pwm_cfg_t {
    union {
        uint32_t freq;        //!< The clock frequency, in Hz
        uint32_t min_freq;    //!< The minimum clock frequency, in Hz
    };
} vsf_pwm_cfg_t;

typedef struct vsf_pwm_capability_t {
#if VSF_PWM_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif

    uint32_t max_freq;
    uint32_t min_freq;
} vsf_pwm_capability_t;

typedef struct vsf_pwm_t vsf_pwm_t;

typedef struct vsf_pwm_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_PWM_APIS(vsf)
} vsf_pwm_op_t;

#if VSF_PWM_CFG_MULTI_CLASS == ENABLED
struct vsf_pwm_t  {
    const vsf_pwm_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a pwm instance.
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_pwm_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if pwm was initialized, or a negative error code

 @note It is not necessary to call vsf_pwm_fini() to deinitialization.
       vsf_pwm_init() should be called before any other pwm API except vsf_pwm_capability().

 \~chinese
 @brief 初始化一个 pwm 实例
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] cfg_ptr: 结构体 vsf_pwm_cfg_t 的指针，参考 @ref vsf_pwm_cfg_t
 @return vsf_err_t: 如果 pwm 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_pwm_fini() 反初始化。
       vsf_pwm_init() 应该在除 vsf_pwm_capability() 之外的其他 pwm API 之前调用。
 */
extern vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a pwm instance.
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return none

 \~chinese
 @brief 终止一个 pwm 实例
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] cfg_ptr: 结构体 vsf_pwm_cfg_t 的指针，参考 @ref vsf_pwm_cfg_t
 @return 无。
 */
extern void vsf_pwm_fini(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief enable interrupt masks of pwm instance.
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] irq_mask: one or more value of enum @ref vsf_pwm_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 pwm 实例的中断
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_pwm_irq_mask_t 的值的按位或，@ref vsf_pwm_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief disable interrupt masks of pwm instance.
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] irq_mask: one or more value of enum vsf_pwm_irq_mask_t, @ref vsf_pwm_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 pwm 实例的中断
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_pwm_irq_mask_t 的值的按位或，@ref vsf_pwm_irq_mask_t
 @return 无。
 */
extern fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief get the capability of pwm instance.
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return vsf_pwm_capability_t: return all capability of current pwm @ref vsf_pwm_capability_t

 \~chinese
 @brief 获取 pwm 实例的能力
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @return vsf_pwm_capability_t: 返回当前 pwm 的所有能力 @ref vsf_pwm_capability_t
 */
extern vsf_pwm_capability_t vsf_pwm_capability(vsf_pwm_t *pwm_ptr);

/**
 \~english
 @brief pwm set the period width and pulse width for a channel
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: pwm channel
 @param[in] period: pwm period width (in clock counter)
 @param[in] pulse: pwm pulse width (in clock counter)
 @return vsf_err_t: VSF_ERR_NONE if the pwm set was successfully, or a negative error code

 \~chinese
 @brief pwm 设置一个通道的周期和宽度
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] channel: pwm 通道
 @param[in] period: pwm 周期宽度 (时钟计数)
 @param[in] pulse: pwm 脉冲宽度 (时钟计数)
 @return vsf_err_t: 如果 pwm 设置成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr, uint8_t channel,
                             uint32_t period, uint32_t pulse);

/**
 * Get clock frequency
 *
 * @param[in] pwm_ptr pwm instance
 * @return clock frequency (in Hz)
 */
/**
 \~english
 @brief pwm get clock frequency
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @return uint32_t: pwm clock frequency

 \~chinese
 @brief pwm 获取时钟频率
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @return uint32_t: pwm 时钟频率
 */
extern uint32_t vsf_pwm_get_freq(vsf_pwm_t *pwm_ptr);


// TODO: move to pwm_commom.h
/**
 \~english
 @brief pwm set the period width and pulse width for a channel
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: pwm channel
 @param[in] period: pwm period width (in milli-second)
 @param[in] pulse: pwm pulse width (in milli-second)
 @return vsf_err_t: VSF_ERR_NONE if the pwm set was successfully, or a negative error code

 \~chinese
 @brief pwm 设置一个通道的周期和宽度
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] channel: pwm 通道
 @param[in] period: pwm 周期宽度 (毫秒)
 @param[in] pulse: pwm 脉冲宽度 (毫秒)
 @return vsf_err_t: 如果 pwm 设置成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 \~english
 @brief pwm set the period width and pulse width for a channel
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: pwm channel
 @param[in] period: pwm period width (in micro-second)
 @param[in] pulse: pwm pulse width (in micro-second)
 @return vsf_err_t: VSF_ERR_NONE if the pwm set was successfully, or a negative error code

 \~chinese
 @brief pwm 设置一个通道的周期和宽度
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] channel: pwm 通道
 @param[in] period: pwm 周期宽度 (微秒)
 @param[in] pulse: pwm 脉冲宽度 (微秒)
 @return vsf_err_t: 如果 pwm 设置成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 \~english
 @brief pwm set the period width and pulse width for a channel
 @param[in] pwm_ptr: a pointer to structure @ref vsf_pwm_t
 @param[in] channel: pwm channel
 @param[in] period: pwm period width (in nano-second)
 @param[in] pulse: pwm pulse width (in nano-second)
 @return vsf_err_t: VSF_ERR_NONE if the pwm set was successfully, or a negative error code

 \~chinese
 @brief pwm 设置一个通道的周期和宽度
 @param[in] pwm_ptr: 结构体 vsf_pwm_t 的指针，参考 @ref vsf_pwm_t
 @param[in] channel: pwm 通道
 @param[in] period: pwm 周期宽度 (纳秒)
 @param[in] pulse: pwm 脉冲宽度 (纳秒)
 @return vsf_err_t: 如果 pwm 设置成功成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_pwm_set_ns(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);


/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_PWM_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_pwm_t               VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t)
#   define vsf_pwm_init(__PM, ...)   VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_init)       ((__vsf_pwm_t *)(__PM), ##__VA_ARGS__)
#   define vsf_pwm_fini(__PM)        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_fini)       ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_enable(__PM)      VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_enable)     ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_disable(__PM)     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_disable)    ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_capability(__PM)  VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_capability) ((__vsf_pwm_t *)(__PM))
#   define vsf_pwm_set(__PM, ...)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set)        ((__vsf_pwm_t *)(__PM), ##__VA_ARGS__)
#   define vsf_pwm_get_freq(__PM)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_get_freq)   ((__vsf_pwm_t *)(__PM))
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_PWM_H__ */
