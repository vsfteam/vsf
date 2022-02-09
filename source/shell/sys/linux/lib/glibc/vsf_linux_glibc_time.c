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

#include "../../vsf_linux_cfg.h"

#if     VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED        \
    &&  VSF_LINUX_USE_SIMPLE_TIME == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/simple_libc/time.h"
#   include "../../include/sys/time.h"
#else
#   include <unistd.h>
#   include <time.h>
#   include <sys/time.h>
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

time_t mktime(struct tm *tm)
{
    const uint16_t yday_month[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    time_t t;

    // tm_year in struct tm starts from 1900, time_t is calculated from 1970
    VSF_LINUX_ASSERT(tm->tm_year >= 70);
    int year = tm->tm_year - 70;
    int leap_years = year / 4;
    leap_years -= year / 100;
    leap_years += (year + 300) / 400;

    t = year * 365 + leap_years;
    t += yday_month[tm->tm_mon] + tm->tm_mday - 1;
    t *= (time_t)86400;
    t += (((tm->tm_hour * 60) + tm->tm_min) * 60) + tm->tm_sec;
    return t;
}

time_t time(time_t *t)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (t != NULL) {
        *t = tv.tv_sec;
    }
    return tv.tv_sec;
}

int nanosleep(const struct timespec *requested_time, struct timespec *remaining)
{
    vsf_timeout_tick_t ticks;
    ticks = 1000ULL * 1000 * requested_time->tv_sec + requested_time->tv_nsec / 1000;
    ticks = vsf_systimer_us_to_tick(ticks);

    vsf_systimer_tick_t remain_ticks = vsf_linux_sleep(ticks);

    if (remaining != NULL) {
        vsf_timeout_tick_t us = vsf_systimer_tick_to_us(remain_ticks);
        remaining->tv_sec = us / (1000 * 1000);
        remaining->tv_nsec = (us % (1000 * 1000)) * 1000;
    }
    return 0;
}

clock_t clock(void)
{
    return vsf_systimer_get_ms();
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    switch (clk_id) {
    case CLOCK_MONOTONIC: {
            vsf_systimer_tick_t us = vsf_systimer_get_us();
            time_t sec = us / 1000000;
            us = us % 1000000;
            tp->tv_sec = sec;
            tp->tv_nsec = us * 1000;
        }
        return 0;
    default:
        return -1;
    }
}

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wvisibility"
#elif __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

int getitimer(int which, struct itimerval *curr_value)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int setitimer(int which, const struct itimerval *new_valie, struct itimerval *old_value)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_TIME
