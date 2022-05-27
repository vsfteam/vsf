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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#define vsf_real_timer_t          VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_t)
#define vsf_real_timer_capability VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_capability)
#define vsf_real_timer_pwm_set    VSF_MCONNECT(VSF_TIMER_CFG_PREFIX, _timer_pwm_set)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TIMER_CFG_IMPLEMENT_PWM_SET == DISABLED
vsf_err_t vsf_real_timer_pwm_set(vsf_real_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_CAPABILITY == DISABLED
timer_capability_t vsf_real_timer_capability(vsf_real_timer_t *timer_ptr)
{
    timer_capability_t timer_capability = {
        0,
    };

    return timer_capability;
}
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#define __VSF_HAL_TEMPLATE_NAME                     _timer
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              _TIMER

#ifndef VSF_TIMER_CFG_PREFIX
#   error "Please define VSF_TIMER_CFG_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_TIMER_CFG_UPPERCASE_PREFIX in timer driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_PWM */
