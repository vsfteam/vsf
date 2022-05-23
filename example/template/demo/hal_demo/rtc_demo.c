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
#   define APP_RTC_DEMO_CFG_RTC                     &vsf_hw_rtc0
#endif

#ifndef APP_RTC_DEMO_CFG_SECOND_DEMO
#   define  APP_RTC_DEMO_CFG_SECOND_DEMO            DISABLED
#endif

#ifndef APP_RTC_DEMO_CFG_TM_DEMO
#   define  APP_RTC_DEMO_CFG_TM_DEMO                ENABLED
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

#if APP_RTC_DEMO_CFG_SECOND_DEMO == ENABLED
static void __rtc_second_demo(void)
{
    vsf_err_t result;
    time_t time;

    result = vsf_rtc_init(APP_RTC_DEMO_CFG_RTC, NULL);
    VSF_ASSERT(result == VSF_ERR_NONE);

#if APP_RTC_DEMO_CFG_SET_RTC == ENABLED
    time = vsf_rtc_get_second(APP_RTC_DEMO_CFG_RTC);
    if (time < APP_RTC_DEMO_CFG_RTC_COUNT) {
        result = vsf_rtc_set_second(APP_RTC_DEMO_CFG_RTC, APP_RTC_DEMO_CFG_RTC_COUNT);
        VSF_ASSERT(result == VSF_ERR_NONE);
    }
#endif

    time = vsf_rtc_get_second(APP_RTC_DEMO_CFG_RTC);
    vsf_trace_debug("current second: %lld" VSF_TRACE_CFG_LINEEND, (long long)time);
}
#endif

#if APP_RTC_DEMO_CFG_TM_DEMO == ENABLED
static void __rtc_tm_demo(void)
{
    static const char *__weeks[] = {
     	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday",
    };

    vsf_err_t result;
    vsf_rtc_tm_t rtc_tm;

    result = vsf_rtc_init(APP_RTC_DEMO_CFG_RTC, NULL);
    VSF_ASSERT(result == VSF_ERR_NONE);

#if APP_RTC_DEMO_CFG_SET_RTC == ENABLED
    result = vsf_rtc_get(APP_RTC_DEMO_CFG_RTC, &rtc_tm);
    VSF_ASSERT(result == VSF_ERR_NONE);

    if (rtc_tm.tm_year < 2022) {
        // Sat Jan 01 2022 00:00:00 GMT+0000 1640995200
        rtc_tm.tm_sec  = 0x00;         // [0 .. 59]
        rtc_tm.tm_min  = 0x00;         // [0 .. 59]
        rtc_tm.tm_hour = 0x00;         // [0 .. 23]
        rtc_tm.tm_mday = 1;            // [1 .. 31]
        rtc_tm.tm_mon  = 1;            // [1 .. 12]  - [January -- December]
        rtc_tm.tm_year = 2022;         // [1900 .. ]

        result = vsf_rtc_set(APP_RTC_DEMO_CFG_RTC, &rtc_tm);
        VSF_ASSERT(result == VSF_ERR_NONE);
    }
#endif

    result = vsf_rtc_get(APP_RTC_DEMO_CFG_RTC, &rtc_tm);
    vsf_trace_debug("%04d/%02d/%02d %02d:%02d:%02d %s" VSF_TRACE_CFG_LINEEND,
                    rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday,
                    rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec,
                    __weeks[rtc_tm.tm_wday - 1]);
}
#endif

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

#if APP_RTC_DEMO_CFG_SECOND_DEMO == ENABLED
    __rtc_second_demo();
#elif APP_RTC_DEMO_CFG_TM_DEMO == ENABLED
    __rtc_tm_demo();
#endif

    return 0;
}

#endif
