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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_PWM_DEMO == ENABLED && VSF_HAL_USE_PWM == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_PWM_DEMO_CFG_PWM_PREFIX
#   undef VSF_PWM_CFG_PREFIX
#   define VSF_PWM_CFG_PREFIX                           APP_PWM_DEMO_CFG_PWM_PREFIX
#endif

#ifndef APP_PWM_DEMO_CFG_PWM
#   define APP_PWM_DEMO_CFG_PWM                         (vsf_pwm_t *)&vsf_hw_pwm0
#endif

#ifndef APP_PWM_DEMO_CFG_CHNNAL
#   define APP_PWM_DEMO_CFG_CHNNAL                      0
#endif

#ifndef APP_PWM_DEMO_CFG_FREQ
#   define APP_PWM_DEMO_CFG_FREQ                        (1 * 1000 * 1000) // 1MHz
#endif

#ifndef APP_PWM_DEMO_CFG_PREIOD
#   define APP_PWM_DEMO_CFG_PREIOD                      100
#endif

#ifndef APP_PWM_DEMO_CFG_PULSE
#   define APP_PWM_DEMO_CFG_PULSE                       50
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __pwm_demo(vsf_pwm_t * pwm, uint8_t channel,  uint32_t period, uint32_t pulse)
{
    vsf_err_t result;

    pwm_cfg_t pwm_cfg = {
        .freq = APP_PWM_DEMO_CFG_FREQ
    };

    result = vsf_pwm_init(pwm, &pwm_cfg);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_pwm_enable(pwm));

    result = vsf_pwm_set(pwm, channel, period, pulse);
    VSF_ASSERT(result == VSF_ERR_NONE);
    (void) result;
}


#if APP_USE_LINUX_DEMO == ENABLED
int pwm_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

    __pwm_demo(APP_PWM_DEMO_CFG_PWM, APP_PWM_DEMO_CFG_CHNNAL, APP_PWM_DEMO_CFG_PREIOD, APP_PWM_DEMO_CFG_PULSE);

    return 0;
}

#endif
