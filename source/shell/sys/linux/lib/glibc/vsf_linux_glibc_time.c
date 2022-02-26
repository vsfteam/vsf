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

// for itimers
#define __VSF_LINUX_CLASS_INHERIT__
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

struct tm * gmtime_r(const time_t *timep, struct tm *result)
{
#ifdef VSF_LINUX_CFG_RTC
    vsf_rtc_tm_t rtc_tm;
    VSF_RTC_GET(&VSF_LINUX_CFG_RTC, &rtc_tm);

    VSF_LINUX_ASSERT(rtc_tm.tm_year >= 1900);
    VSF_LINUX_ASSERT(result != NULL);
    result->tm_sec = rtc_tm.tm_sec;
    result->tm_min = rtc_tm.tm_min;
    result->tm_hour = rtc_tm.tm_hour;
    result->tm_mday = rtc_tm.tm_mday;
    result->tm_mon = rtc_tm.tm_mon - 1;
    result->tm_year = rtc_tm.tm_year - 1900;
#endif
    return result;
}

struct tm * gmtime(const time_t *timep)
{
    return gmtime_r(timep, &__vsf_linux.tm);
}

struct tm * localtime_r(const time_t *timep, struct tm *result)
{
    return gmtime_r(timep, result);
}

struct tm * localtime(const time_t *timep)
{
    return localtime_r(timep, &__vsf_linux.tm);
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
    VSF_LINUX_ASSERT(which < ITIMER_NUM);
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    vsf_systimer_tick_t elapsed_value;
    switch (which) {
    case ITIMER_REAL:
        elapsed_value = vsf_systimer_get_elapsed(process->timers[which].start);
        break;
    default:
        elapsed_value = 0;
        break;
    }

    if (curr_value != NULL) {
        curr_value->it_interval = process->timers[which].value.it_interval;

        elapsed_value = vsf_systimer_tick_to_us(elapsed_value);
        curr_value->it_value.tv_sec = elapsed_value / (1000 * 1000);
        curr_value->it_value.tv_usec = elapsed_value % (1000 * 1000);
    }
    return 0;
}

static void __vsf_linux_prepare_real_timer(vsf_linux_process_t *process)
{
    vsf_linux_timer_t *timer = &process->timers[ITIMER_REAL];
    vsf_systimer_tick_t ticks = timer->value.it_value.tv_sec * 1000 * 1000 + timer->value.it_value.tv_usec;
    ticks = vsf_systimer_us_to_tick(ticks);
    if (ticks != 0) {
        process->timers[ITIMER_REAL].start = vsf_systimer_get_tick();
        vsf_callback_timer_add(&process->real_timer, ticks);
    }
}

static void __vsf_linux_on_real_timer(vsf_callback_timer_t *timer)
{
    vsf_linux_process_t *process = container_of(timer, vsf_linux_process_t, real_timer);
    kill(process->id.pid, SIGALRM);
    process->timers[ITIMER_REAL].value.it_value = process->timers[ITIMER_REAL].value.it_interval;
    __vsf_linux_prepare_real_timer(process);
}

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value)
{
    VSF_LINUX_ASSERT(which < ITIMER_NUM);
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    if (old_value != NULL) {
        *old_value = process->timers[which].value;
    }
    process->timers[which].value = *new_value;

    if (ITIMER_REAL == which) {
        vsf_callback_timer_remove(&process->real_timer);
        process->real_timer.on_timer = __vsf_linux_on_real_timer;
        __vsf_linux_prepare_real_timer(process);
    }
    return 0;
}

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_TIME
