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

#if VSF_USE_LINUX == ENABLED

// pending_list in vsf_sem_t
#define __VSF_EDA_CLASS_INHERIT__
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/semaphore.h"
#   include "../../include/errno.h"
#else
#   include <unistd.h>
#   include <semaphore.h>
#   include <errno.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "../../include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static int __sync_pend(vsf_sync_t *sem)
{
    while (1) {
        vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sem, vsf_thread_wait());
        switch (reason) {
        case VSF_SYNC_TIMEOUT:      return -ETIMEDOUT;
        case VSF_SYNC_PENDING:      break;
        case VSF_SYNC_GET:          return 0;
        case VSF_SYNC_CANCEL:       return -EAGAIN;
        }
    }
}

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    vsf_eda_sem_init(sem, value);
    return 0;
}

int sem_post(sem_t *sem)
{
    vsf_eda_sem_post(sem);
    return 0;
}

int sem_destroy(sem_t *sem)
{
    vsf_eda_sync_cancel(sem);
    return 0;
}

int sem_wait(sem_t *sem)
{
    if (vsf_eda_sem_pend(sem, -1)) {
        return __sync_pend(sem);
    }
    return 0;
}

int sem_trywait(sem_t *sem)
{
    if (vsf_eda_sem_pend(sem, 0)) {
        errno = EAGAIN;
        return -1;
    }
    return 0;
}

int sem_getvalue(sem_t *sem, int *value)
{
    vsf_protect_t orig = vsf_protect_sched();
    int value_tmp = sem->cur_union.bits.cur;
    if (!value_tmp) {
        __vsf_dlist_foreach_unsafe(vsf_eda_t, pending_node, &sem->pending_list) {
            value_tmp--;
        }
    }
    vsf_unprotect_sched(orig);

    if (value != NULL) {
        *value = value_tmp;
    }
    return 0;
}

sem_t * sem_open(const char *name, int oflag, mode_t mode, unsigned int value)
{
    VSF_LINUX_ASSERT(false);
    return SEM_FAILED;
}

int sem_unlink(const char *name)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int sem_close(sem_t *sem)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    vsf_systimer_tick_t now_us = now.tv_sec * 1000000 + now.tv_nsec / 1000;
    vsf_systimer_tick_t timeout_us = abs_timeout->tv_sec * 1000000 + abs_timeout->tv_nsec / 1000;
    vsf_systimer_tick_t diff_tick = vsf_systimer_us_to_tick(now_us > timeout_us ? 0 : timeout_us - now_us);

    if (vsf_eda_sem_pend(sem, diff_tick)) {
        if (diff_tick != 0) {
            return __sync_pend(sem);
        } else {
            return -ETIMEDOUT;
        }
    }
    return 0;
}
#endif

#if VSF_LINUX_APPLET_USE_SEMAPHORE == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_semaphore_vplt_t vsf_linux_semaphore_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_semaphore_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(sem_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_wait),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_trywait),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_timedwait),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_post),
    VSF_APPLET_VPLT_ENTRY_FUNC(sem_getvalue),
};
#endif

#endif      // VSF_USE_LINUX
