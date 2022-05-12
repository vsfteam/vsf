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

#include "kernel/vsf_kernel.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_IAR__ || __IS_COMILER_GCC__
char * strcasestr(const char *str, const char *substr)
{
    do {
        for (int i = 0; ; i++) {
            if (!substr[i]) {
                return (char *)str;
            } else if (tolower(substr[i]) != tolower(str[i])) {
                break;
            }
        }
    } while (*str++);
    return NULL;
}
#endif

#if __IS_COMPILER_IAR__
size_t strlcpy(char *dst, const char *src, size_t dsize)
{
    const char *osrc = src;
    size_t nleft = dsize;
    /* Copy as many bytes as will fit. */
    if (nleft != 0) {
        while (--nleft != 0) {
            if ((*dst++ = *src++) == '\0')
                break;
        }
    }
    /* Not enough room in dst, add NUL and traverse rest of src. */
    if (nleft == 0) {
        if (dsize != 0)
            *dst = '\0';        /* NUL-terminate dst */
        while (*src++)
            ;
    }
    return(src - osrc - 1);     /* count does not include NUL */
}

// implement APIs not supported in time.h in IAR
#   if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED)
#       if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    switch (clk_id) {
    case CLOCK_MONOTONIC: {
            vsf_systimer_tick_t us = vsf_systimer_get_us();
            tp->tv_sec = us / 1000000;
            tp->tv_nsec = us * 1000;
        }
        return 0;
    default:
        return -1;
    }
}

int nanosleep(const struct timespec *requested_time, struct timespec *remaining)
{
    vsf_timeout_tick_t ticks;
    ticks = 1000ULL * 1000 * requested_time->tv_sec + requested_time->tv_nsec / 1000;
    ticks = vsf_systimer_us_to_tick(ticks);

    vsf_thread_delay(ticks);

    if (remaining != NULL) {
        remaining->tv_sec = 0;
        remaining->tv_nsec = 0;
    }
    return 0;
}
#       endif           // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#   endif

#if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED)
int fseeko(FILE *f, off_t offset, int whence)
{
    return fseek(f, (long)offset, whence);
}

off_t ftello(FILE *f)
{
    return (off_t)ftell(f);
}

int fseeko64(FILE *f, off64_t offset, int whence)
{
    return fseek(f, (long)offset, whence);
}

off64_t ftello64(FILE *f)
{
    return (off_t)ftell(f);
}
#endif      // !(VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_STDIO)

#endif
