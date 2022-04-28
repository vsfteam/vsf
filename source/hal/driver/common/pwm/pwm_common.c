/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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


/*============================ INCLUDES ======================================*/

#define VSF_PWM_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_PWM == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#if VSF_PWM_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_pwm_init(vsf_pwm_t *pwm_ptr, pwm_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->init != NULL);

    return pwm_ptr->op->init(pwm_ptr, cfg_ptr);
}

fsm_rt_t vsf_pwm_enable(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->enable != NULL);

    return pwm_ptr->op->enable(pwm_ptr);
}

fsm_rt_t vsf_pwm_disable(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->disable != NULL);

    return pwm_ptr->op->disable(pwm_ptr);
}

pwm_capability_t vsf_pwm_capability(vsf_pwm_t *pwm_ptr)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->capability != NULL);

    return pwm_ptr->op->capability(pwm_ptr);
}

vsf_err_t vsf_pwm_set(vsf_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->set != NULL);

    return pwm_ptr->op->set(pwm_ptr, channel, period, pulse);
}


vsf_err_t vsf_pwm_set_ns(vsf_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->set_ns != NULL);

    return pwm_ptr->op->set_ns(pwm_ptr, channel, period, pulse);
}

vsf_err_t vsf_pwm_set_us(vsf_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->set_us != NULL);

    return pwm_ptr->op->set_us(pwm_ptr, channel, period, pulse);
}

vsf_err_t vsf_pwm_set_ms(vsf_pwm_t *pwm_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(pwm_ptr != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op != NULL);
    VSF_HAL_ASSERT(pwm_ptr->op->set_ms != NULL);

    return pwm_ptr->op->set_ms(pwm_ptr, channel, period, pulse);
}

#endif /* VSF_PWM_CFG_MULTI_CLASS == ENABLED */
#endif /* VSF_HAL_USE_PWM == ENABLED */
