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

#if VSF_HAL_PWM_IMP_TIME_SETTING == ENABLED

/*============================ INCLUDES ======================================*/

#include "../../AIC/AIC8800/pwm/pwm.h"
#if VSF_HAL_USE_PWM == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#undef VSF_AIC8800_PWM_CFG_UNIT_NS
#define VSF_AIC8800_PWM_CFG_UNIT_NS                         1000000

#undef VSF_AIC8800_PWM_CFG_UNIT_US
#define VSF_AIC8800_PWM_CFG_UNIT_US                         1000

#undef VSF_AIC8800_PWM_CFG_UNIT_MS
#define VSF_AIC8800_PWM_CFG_UNIT_MS                         1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint32_t __pwm_time_to_tick( vsf_pwm_t *pwm_ptr, uint32_t time, uint32_t unit)
{
    uint64_t temp_ret, temp_time;
    temp_time = time;
    temp_ret = vsf_pwm_get_freq(pwm_ptr);
    if (0 == temp_ret) {
        return 0;
    }
    temp_ret = temp_ret / 1000 * temp_time / unit;
    if (temp_ret >= ((uint64_t)1 << 32)) {
        return 0;
    }
    return temp_ret;
}

vsf_err_t vsf_pwm_set_ms(   vsf_pwm_t *pwm_ptr, uint8_t channel,
                            uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);

    uint32_t temp_period, temp_pulse;

    temp_period = __pwm_time_to_tick(pwm_ptr, period, VSF_AIC8800_PWM_CFG_UNIT_MS);
    temp_pulse = __pwm_time_to_tick(pwm_ptr, pulse, VSF_AIC8800_PWM_CFG_UNIT_MS);
    if ((0 == temp_period) || (0 == temp_pulse)) {
        return VSF_ERR_FAIL;
    }

    return vsf_pwm_set(pwm_ptr, channel, temp_period, temp_pulse);
}

vsf_err_t vsf_pwm_set_us(   vsf_pwm_t *pwm_ptr, uint8_t channel,
                            uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);

    uint32_t temp_period, temp_pulse;

    temp_period = __pwm_time_to_tick(pwm_ptr, period, VSF_AIC8800_PWM_CFG_UNIT_US);
    temp_pulse = __pwm_time_to_tick(pwm_ptr, pulse, VSF_AIC8800_PWM_CFG_UNIT_US);
    if ((0 == temp_period) || (0 == temp_pulse)) {
        return VSF_ERR_FAIL;
    }

    return vsf_pwm_set(pwm_ptr, channel, temp_period, temp_pulse);
}

vsf_err_t vsf_pwm_set_ns(   vsf_pwm_t *pwm_ptr, uint8_t channel,
                            uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != pwm_ptr);

    if (period < 80 || pulse < 50) {
        return VSF_ERR_INVALID_PARAMETER;
    }
    uint32_t temp_period, temp_pulse;

    temp_period = __pwm_time_to_tick(pwm_ptr, period, VSF_AIC8800_PWM_CFG_UNIT_NS);
    temp_pulse = __pwm_time_to_tick(pwm_ptr, pulse, VSF_AIC8800_PWM_CFG_UNIT_NS);
    if ((0 == temp_period) || (0 == temp_pulse)) {
        return VSF_ERR_FAIL;
    }

    return vsf_pwm_set(pwm_ptr, channel, temp_period, temp_pulse);
}

#endif  /* VSF_HAL_USE_PWM */
#endif  /* VSF_HAL_PWM_IMP_TIME_SETTING */
