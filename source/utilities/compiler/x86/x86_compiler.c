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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// strlcpy is from FreeBSD library, not available in libc
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

#ifdef __WIN__
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

char * strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

char * strtok_r(char *str, const char *delim, char **saveptr)
{
    extern char * strtok_s(char *str, const char *delim, char **saveptr);
    return strtok_s(str, delim, saveptr);
}

void srandom(unsigned int seed)
{
    srand(seed);
}

long int random(void)
{
    return rand();
}

#   if !(VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_TIME == ENABLED)
#       if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#           if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
int nanosleep(const struct timespec *requested_time, struct timespec *remaining)
{
    // TODO: assert current context is vsf_thread
    vsf_systimer_tick_t tick =  vsf_systimer_ms_to_tick(requested_time->tv_sec *1000)
                            +   vsf_systimer_us_to_tick(requested_time->tv_nsec / 1000);
    if (!tick) {
        tick = vsf_systimer_us_to_tick(1);
    }
    vsf_thread_delay(tick);
    if (remaining != NULL) {
        remaining->tv_nsec = 0;
        remaining->tv_sec = 0;
    }
    return 0;
}
#           endif       // VSF_KERNEL_CFG_SUPPORT_THREAD

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
#       endif           // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#   endif
#endif
