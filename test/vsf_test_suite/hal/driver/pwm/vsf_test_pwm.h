/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
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
 *****************************************************************************/

#ifndef __VSF_TEST_PWM_H__
#define __VSF_TEST_PWM_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "test_params_generated.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_PWM_BASIC_ENABLE
#   define VSF_TEST_PWM_BASIC_ENABLE           ENABLED
#endif

#ifndef VSF_TEST_PWM_DUAL_CHANNEL_ENABLE
#   define VSF_TEST_PWM_DUAL_CHANNEL_ENABLE    ENABLED
#endif

// TODO: pwm_irq test uses raw hardware register access (pwm_hw->, timer_hw->)
// and is not portable. Needs rewrite using portable HAL APIs:
// vsf_pwm_irq_enable/disable, vsf_systimer_get, vsf_pwm_get_configuration.
#ifndef VSF_TEST_PWM_IRQ_ENABLE
#   define VSF_TEST_PWM_IRQ_ENABLE             DISABLED
#endif

#ifndef VSF_TEST_PWM_ENABLE
#   define VSF_TEST_PWM_ENABLE    ENABLED
#endif

#include "suite/vsf_test_pwm_basic.h"
#include "suite/vsf_test_pwm_dual_channel.h"
#include "suite/vsf_test_pwm_irq.h"
#ifdef __cplusplus
}
#endif
#endif /* __VSF_TEST_PWM_H__ */
/* EOF */