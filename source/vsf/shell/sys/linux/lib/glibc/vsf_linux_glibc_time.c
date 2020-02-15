/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#if VSF_USE_LINUX == ENABLED

#include <unistd.h>
#include <sys/time.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// TODO: wakeup after signal

void usleep(int usec)
{
    vsf_teda_set_timer_us(usec);
    vsf_thread_wfe(VSF_EVT_TIMER);
}

unsigned sleep(unsigned sec)
{
    vsf_teda_set_timer_ms(sec * 1000);
    vsf_thread_wfe(VSF_EVT_TIMER);
    return 0;
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
    return vsf_systimer_tick_to_us(vsf_timer_get_tick());
}

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    switch (clk_id) {
    case CLOCK_MONOTONIC: {
            uint_fast32_t us = vsf_systimer_tick_to_us(vsf_timer_get_tick());
            tp->tv_sec = us / 1000000;
            tp->tv_nsec = us * 1000;
        }
        return 0;
    default:
        return -1;
    }
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
    return 0;
}

int getitimer(int which, struct itimerval *curr_value)
{
    VSF_LINUX_ASSERT(false);
}

int setitimer(int which, const struct itimerval *new_valie, struct itimerval *old_value)
{
    VSF_LINUX_ASSERT(false);
}

#endif      // VSF_USE_LINUX
