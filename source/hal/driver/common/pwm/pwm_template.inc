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

#if VSF_HAL_USE_PWM == ENABLED

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/

#ifndef VSF_PWM_CFG_REIMPLEMENT_SET_MS
#   define VSF_PWM_CFG_REIMPLEMENT_SET_MS       DISABLED
#endif

#ifndef VSF_PWM_CFG_REIMPLEMENT_SET_US
#   define VSF_PWM_CFG_REIMPLEMENT_SET_US       DISABLED
#endif

#ifndef VSF_PWM_CFG_REIMPLEMENT_SET_NS
#   define VSF_PWM_CFG_REIMPLEMENT_SET_NS       DISABLED
#endif

#define vsf_real_pwm_t          VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_t)
#define vsf_real_pwm_set        VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set)
#define vsf_real_pwm_set_ms     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ms)
#define vsf_real_pwm_set_us     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_us)
#define vsf_real_pwm_set_ns     VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_set_ns)
#define vsf_real_pwm_get_freq   VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_get_freq)
#define vsf_real_pwm_capability VSF_MCONNECT(VSF_PWM_CFG_PREFIX, _pwm_capability)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_PWM_CFG_REIMPLEMENT_SET_MS == DISABLED
vsf_err_t vsf_real_pwm_set_ms(vsf_real_pwm_t *pwm_ptr, uint8_t channel, uint32_t period_ms, uint32_t pulse_ms)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);

    uint32_t freq = vsf_real_pwm_get_freq(pwm_ptr);
    VSF_HAL_ASSERT(freq != 0);

    uint32_t period = (uint64_t)1000 * 1000 * period_ms / freq;
    uint32_t pulse  = (uint64_t)1000 * 1000 * pulse_ms  / freq;

    return vsf_real_pwm_set(pwm_ptr, channel, period, pulse);
}
#endif

#if VSF_PWM_CFG_REIMPLEMENT_SET_US == DISABLED
vsf_err_t vsf_real_pwm_set_us(vsf_real_pwm_t *pwm_ptr, uint8_t channel, uint32_t period_us, uint32_t pulse_us)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);

    uint32_t freq = vsf_real_pwm_get_freq(pwm_ptr);
    VSF_HAL_ASSERT(freq != 0);

    uint32_t period = (uint64_t)1000 * period_us / freq;
    uint32_t pulse  = (uint64_t)1000 * pulse_us  / freq;

    return vsf_real_pwm_set(pwm_ptr, channel, period, pulse);
}
#endif

#if VSF_PWM_CFG_REIMPLEMENT_SET_NS == DISABLED
vsf_err_t vsf_real_pwm_set_ns(vsf_real_pwm_t *pwm_ptr, uint8_t channel, uint32_t period_ns, uint32_t pulse_ns)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);


    uint32_t freq = vsf_real_pwm_get_freq(pwm_ptr);
    VSF_HAL_ASSERT(freq != 0);

    uint32_t period = period_ns / freq;
    uint32_t pulse  = pulse_ns  / freq;

    return vsf_real_pwm_set(pwm_ptr, channel, period, pulse);
}
#endif

#if VSF_PWM_CFG_REIMPLEMENT_CAPABILITY == DISABLED
pwm_capability_t vsf_real_pwm_capability(vsf_real_pwm_t *pwm_ptr)
{
    pwm_capability_t pwm_capability = {
        0,
    };

    return pwm_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _pwm
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _PWM

#ifndef VSF_PWM_CFG_PREFIX
#   error "Please define VSF_PWM_CFG_PREFIX in pwm driver"
#endif

#ifndef VSF_PWM_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_PWM_CFG_UPPERCASE_PREFIX in pwm driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_PWM */
