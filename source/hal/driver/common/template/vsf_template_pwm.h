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

#ifndef VSF_PWM_CFG_MULTI_CLASS
#   define VSF_PWM_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_PWM_CFG_MULTI_CLASS is enabled
#ifndef VSF_PWM_CFG_IMPLEMENT_OP
#   if VSF_PWM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PWM_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_PWM_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_PWM_CFG_PREFIX: use for macro vsf_pwm_{init, enable, ...}
#ifndef VSF_PWM_CFG_PREFIX
#   if VSF_PWM_CFG_MULTI_CLASS == ENABLED
#       define VSF_PWM_CFG_PREFIX               vsf
#   elif defined(VSF_HW_PWM_COUNT) && (VSF_HW_PWM_COUNT != 0)
#       define VSF_PWM_CFG_PREFIX               vsf_hw
#   endif
#endif

#ifndef VSF_PWM_CFG_FUNCTION_RENAME
#   define VSF_PWM_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_PWM_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_PWM_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_PWM_APIS(__prefix_name)                                                                                                                            \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pwm, init,           VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, pwm_cfg_t *cfg_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,          pwm, enable,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,          pwm, disable,        VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, pwm_capability_t,  pwm, capability,     VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)                                                                \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pwm, set,            VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pwm, set_ms,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pwm, set_us,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,         pwm, set_ns,         VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)\
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,          pwm, get_freq,       VSF_MCONNECT(__prefix_name, _pwm_t) *pwm_ptr)

/*============================ TYPES =========================================*/

typedef struct pwm_cfg_t {
    union {
        uint32_t freq;        //!< The clock frequency, in Hz
        uint32_t min_freq;    //!< The minimum clock frequency, in Hz
    };
} pwm_cfg_t;

#if VSF_PWM_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct pwm_capability_t {
    inherit(peripheral_capability_t)
} pwm_capability_t;
#endif

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


extern vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, pwm_cfg_t *cfg_ptr);

extern fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr);

extern fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr);

extern pwm_capability_t vsf_pwm_capability(vsf_pwm_t *pwm_ptr);

/**
 *  Set the period width and pluse width for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in clock counter)
 * @param[in] pulse pwm pluse width (in clock counter)
 */
extern vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr,
                             uint8_t    channel,
                             uint32_t   period,
                             uint32_t   pulse);

/**
 *  Set the period width(ms) and pluse width(ms) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in milli-second)
 * @param[in] pulse pwm pluse width (in milli-second)
 */
extern vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(us) and pluse width(us) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in micro-seconds)
 * @param[in] pulse pwm pluse width (in micro-seconds)
 */
extern vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr,
                                uint8_t    channel,
                                uint32_t   period,
                                uint32_t   pulse);

/**
 *  Set the period width(ns) and pluse width(ns) for a channel
 *
 * @param[in] pwm_ptr pwm instance
 * @param[in] period pwm period width (in nano second)
 * @param[in] pulse pwm pluse width (in nano second)
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
#   define vsf_pwm_init(__PM, ...)                                              \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_init)       ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_enable(__PM)                                                 \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_enable)     ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM)
#   define vsf_pwm_disable(__PM)                                                \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_disable)    ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM)
#   define vsf_pwm_capability(__PM)                                             \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_capability) ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM)
#   define vsf_pwm_set(__PM, ...)                                               \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set)        ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_ms(__PM, ...)                                            \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ms)     ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_us(__PM, ...)                                            \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_us)     ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_set_ns(__PM, ...)                                            \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ns)     ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM, ##__VA_ARGS__)
#   define vsf_pwm_get_freq(__PM)                                               \
        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_get_freq)   ((VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t) *)__PM)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_PWM_INTERFACE_H__ */
