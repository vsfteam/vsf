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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_PWM == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_PWM_CLASS_IMPLEMENT
#include "./vsf_remapped_pwm.h"

#if VSF_PWM_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_PWM_CFG_MULTI_CLASS == ENABLED
const vsf_pwm_op_t vsf_remapped_pwm_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_PWM_APIS(vsf_remapped_pwm)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_pwm_init(vsf_remapped_pwm_t *pwm, vsf_pwm_cfg_t *cfg)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_init(pwm->target, cfg);
}

void vsf_remapped_pwm_fini(vsf_remapped_pwm_t *pwm)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    vsf_pwm_fini(pwm->target);
}

vsf_err_t vsf_remapped_pwm_get_configuration(vsf_remapped_pwm_t *pwm, vsf_pwm_cfg_t *cfg)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_get_configuration(pwm->target, cfg);
}

fsm_rt_t vsf_remapped_pwm_enable(vsf_remapped_pwm_t *pwm)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_enable(pwm->target);
}

fsm_rt_t vsf_remapped_pwm_disable(vsf_remapped_pwm_t *pwm)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_disable(pwm->target);
}

vsf_pwm_capability_t vsf_remapped_pwm_capability(vsf_remapped_pwm_t *pwm)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_capability(pwm->target);
}

vsf_err_t vsf_remapped_pwm_set(vsf_remapped_pwm_t *pwm,
        uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_set(pwm->target, channel, period, pulse);
}

vsf_err_t vsf_remapped_pwm_set_ms(vsf_remapped_pwm_t *pwm,
        uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_set_ms(pwm->target, channel, period, pulse);
}

vsf_err_t vsf_remapped_pwm_set_us(vsf_remapped_pwm_t *pwm,
        uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_set_us(pwm->target, channel, period, pulse);
}

vsf_err_t vsf_remapped_pwm_set_ns(vsf_remapped_pwm_t *pwm,
        uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_set_ns(pwm->target, channel, period, pulse);
}

uint32_t vsf_remapped_pwm_get_freq(vsf_remapped_pwm_t *pwm)
{
    VSF_HAL_ASSERT((pwm != NULL) && (pwm->target != NULL));
    return vsf_pwm_get_freq(pwm->target);
}

#endif
#endif
