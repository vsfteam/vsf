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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_RTC_DEMO == ENABLED && VSF_HAL_USE_RTC == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_RTC_DEMO_CFG_RTC_PREFIX
#   undef VSF_RTC_CFG_PREFIX
#   define VSF_RTC_CFG_PREFIX                       APP_RTC_DEMO_CFG_RTC_PREFIX
#endif

#ifndef APP_RTC_DEMO_CFG_RTC
#   define APP_RTC_DEMO_CFG_RTC                     (vsf_rtc_t *)&vsf_hw_rtc0
#endif

#ifndef APP_RTC_DEMO_CFG_SET_RTC
#   define APP_RTC_DEMO_CFG_SET_RTC                 ENABLED
#endif

#ifndef APP_RTC_DEMO_CFG_RTC_COUNT
// Tue Apr 26 2022 19:06:55 GMT+0000
// Wed Apr 27 2022 03:06:55 GMT+0800
#   define APP_RTC_DEMO_CFG_RTC_COUNT               1651000015
#endif

/*============================ IMPLEMENTATION ================================*/

void __rtc_demo_handler(void *target_ptr, em_rtc_irq_mask_t irq_mask, vsf_rtc_t *rtc_ptr)
{
    if (irq_mask == VSF_RTC_IRQ_MASK_ALARM) {
        vsf_trace_debug("enter timer overflow interrupt");
    }
}

static void __rtc_demo(void)
{
    vsf_err_t result;
    (void)result

    rtc_cfg_t rtc_cfg = {
        .isr            = {
            .handler_fn = __rtc_demo_handler,
            .target_ptr = NULL,
            .prio       = APP_RTC_DEMO_IRQ_PRIO,
        },
    };

    result = vsf_rtc_init(APP_RTC_DEMO_CFG_RTC, &rtc_cfg);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_rtc_enable(APP_RTC_DEMO_CFG_RTC));

#ifdef APP_RTC_DEMO_CFG_SET_RTC == ENABED
    result = vsf_rtc_set_second(APP_RTC_DEMO_CFG_RTC, APP_RTC_DEMO_CFG_RTC_COUNT);
    VSF_ASSERT(result == VSF_ERR_NONE);
#endif

    time_t time = vsf_rtc_get_second(APP_RTC_DEMO_CFG_RTC);
    vsf_trace_debug("current second: %lld" VSF_TRACE_NEWLINE, (long long)time);
}

#if APP_USE_LINUX_DEMO == ENABLED
int rtc_main(int argc, char *argv[])
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

    __rtc_demo();

    return 0;
}

#endif
