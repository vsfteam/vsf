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

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

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

#define __VSF_HAL_TEMPLATE_NAME                     timer
#define __VSF_HAL_TEMPLATE_UPCASE_NAME              TIMER

#ifndef VSF_TIMER_CFG_PREFIX
#   error "Please define VSF_TIMER_CFG_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_UPPERCASE_PREFIX
#   error "Please define VSF_TIMER_CFG_UPPERCASE_PREFIX in timer driver"
#endif

#include "hal/driver/common/template/vsf_template_implementation.h"

#endif  /* VSF_HAL_USE_PWM */
