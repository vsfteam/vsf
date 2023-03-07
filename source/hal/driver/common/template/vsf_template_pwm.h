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

#ifndef __HAL_DRIVER_PWM_INTERFACE_H__
#define __HAL_DRIVER_PWM_INTERFACE_H__

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

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_APIS(__prefix_name)                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, init,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, vsf_pwm_cfg_t *cfg_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                  pwm, fini,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, enable,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,              pwm, disable,        VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_pwm_capability_t,  pwm, capability,     VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set,            VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set_ms,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set_us,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,             pwm, set_ns,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,              pwm, get_freq,       VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)

/*============================ TYPES =========================================*/

typedef struct vsf_pwm_cfg_t {
    union {
        uint32_t freq;        //!< The clock frequency, in Hz
        uint32_t min_freq;    //!< The minimum clock frequency, in Hz
    };
} vsf_pwm_cfg_t;

typedef struct vsf_pwm_capability_t {
    inherit(vsf_peripheral_capability_t)
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

extern fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr);

extern fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr);

extern vsf_pwm_capability_t vsf_pwm_capability(vsf_pwm_t *pwm_ptr);

/**
 *  Set the period width and pulse width for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in clock counter)
 * @param[in] pulse pwm pulse width (in clock counter)
 */
extern vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr,
                             uint8_t    channel,
                             uint32_t   period,
                             uint32_t   pulse);

/**
 *  Set the period width(ms) and pulse width(ms) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in milli-second)
 * @param[in] pulse pwm pulse width (in milli-second)
 */
extern vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(us) and pulse width(us) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in micro-seconds)
 * @param[in] pulse pwm pulse width (in micro-seconds)
 */
extern vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(ns) and pulse width(ns) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in nano second)
 * @param[in] pulse pwm pulse width (in nano second)
 */
extern vsf_err_t vsf_pwm_set_ns(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 * Get clock frequency
 *
 * @param[in] pwm_ptr pwm instance
 * @return clock frequency (in Hz)
 */
extern uint32_t vsf_pwm_get_freq(vsf_pwm_t *pwm_ptr);

/**
 *  TODO: API for query number of pwm channels
 */

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_PWM_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_pwm_t               VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t)
#   define vsf_pwm_init(__PM, ...)   VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_init)       ((__vsf_pwm_t *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_fini(__PM)        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_fini)       ((__vsf_pwm_t *)__PM)
#   define vsf_pwm_enable(__PM)      VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_enable)     ((__vsf_pwm_t *)__PM)
#   define vsf_pwm_disable(__PM)     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_disable)    ((__vsf_pwm_t *)__PM)
#   define vsf_pwm_capability(__PM)  VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_capability) ((__vsf_pwm_t *)__PM)
#   define vsf_pwm_set(__PM, ...)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set)        ((__vsf_pwm_t *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_ms(__PM, ...) VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ms)     ((__vsf_pwm_t *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_us(__PM, ...) VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_us)     ((__vsf_pwm_t *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_ns(__PM, ...) VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ns)     ((__vsf_pwm_t *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_get_freq(__PM)    VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_get_freq)   ((__vsf_pwm_t *)__PM)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_PWM_INTERFACE_H__ */
