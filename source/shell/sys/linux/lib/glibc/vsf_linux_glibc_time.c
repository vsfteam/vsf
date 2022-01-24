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

time_t time(time_t *t)
{
    time_t sec = vsf_systimer_get_ms() / 1000;
    if (t != NULL) {
        *t = sec;
    }
    return sec;
}

int nanosleep(const struct timespec *requested_time, struct timespec *remaining)
{
    if (requested_time->tv_sec) {
        sleep(requested_time->tv_sec);
    }
    if (requested_time->tv_nsec) {
        usleep(requested_time->tv_nsec / 1000);
    }
    if (remaining != NULL) {
        remaining->tv_nsec = 0;
        remaining->tv_sec = 0;
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
