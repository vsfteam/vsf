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

#if VSF_HAL_USE_TIMER == ENABLED

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#ifndef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#   define  VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY              DISALBED
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
#   ifndef VSF_TIMER_CFG_CAPABILITY_IRQ_MASK
#       define VSF_TIMER_CFG_CAPABILITY_IRQ_MASK        VSF_TIMER_IRQ_ALL_BITS_MASK
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN
#       define VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN    32
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM
#       define VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM     1
#   endif
#   ifndef VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT
#       define VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT 32
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#define vsf_real_timer_t          VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_t)
#define vsf_real_timer_capability VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_capability)
#define vsf_real_timer_pwm_set    VSF_MCONNECT(VSF_TIMER_CFG_IMP_PREFIX, _timer_pwm_set)

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TIMER_CFG_IMP_PWM_SET == DISABLED
vsf_err_t vsf_real_timer_pwm_set(vsf_real_timer_t *timer_ptr, uint8_t channel, uint32_t period, uint32_t pulse)
{
    VSF_HAL_ASSERT(NULL != timer_ptr);

    VSF_HAL_ASSERT(0);
    return VSF_ERR_FAIL;
}
#endif

#if VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY == DISABLED
vsf_timer_capability_t vsf_real_timer_capability(vsf_real_timer_t *timer_ptr)
{
    vsf_timer_capability_t timer_capability = {
        .irq_mask           = VSF_TIMER_CFG_CAPABILITY_IRQ_MASK,
        .timer_bitlen       = VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN,
        .support_pwm        = VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM,
        .pwm_channel_cnt    = VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT,
    };

    return timer_capability;
}
#endif

/*============================ MACROS ========================================*/

#undef VSF_TIMER_CFG_REIMPLEMENT_API_CAPABILITY
#undef VSF_TIMER_CFG_CAPABILITY_IRQ_MASK,
#undef VSF_TIMER_CFG_CAPABILITY_TIMER_BITLEN,
#undef VSF_TIMER_CFG_CAPABILITY_SUPPORT_PWM,
#undef VSF_TIMER_CFG_CAPABILITY_PWM_CHANNEL_CNT,
#undef vsf_real_timer_t
#undef vsf_real_timer_capability
#undef vsf_real_timer_pwm_set

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_HAL_TEMPLATE_IMP_NAME                   _timer
#define VSF_HAL_TEMPLATE_IMP_UPCASE_NAME            _TIMER

#ifndef VSF_TIMER_CFG_IMP_PREFIX
#   error "Please define VSF_TIMER_CFG_IMP_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#   error "Please define VSF_TIMER_CFG_IMP_UPCASE_PREFIX in timer driver"
#endif

#ifndef VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX
#   define VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX      VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#endif

#include "hal/driver/common/template/vsf_template_instance_implementation.h"

#undef VSF_TIMER_CFG_IMP_PREFIX
#undef VSF_TIMER_CFG_IMP_COUNT_MASK_PREFIX
#undef VSF_TIMER_CFG_IMP_UPCASE_PREFIX
#undef VSF_TIMER_CFG_IMP_LV0

#endif  /* VSF_HAL_USE_TIEMR */
