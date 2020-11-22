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

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#   include "../../include/semaphore.h"
#else
#   include <unistd.h>
#   include <semaphore.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "../../include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_LINUX_CFG_PRINT_BUFF_SIZE
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE        256
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int __sync_pend(vsf_sync_t *sem);

/*============================ IMPLEMENTATION ================================*/

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

int sem_destory(sem_t *sem)
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
        return __sync_pend(sem);
    }
    return 0;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    int_fast32_t timeout_us = abs_timeout->tv_sec * 1000000 + abs_timeout->tv_nsec / 1000;
    if (vsf_eda_sem_pend(sem, vsf_systimer_us_to_tick(timeout_us))) {
        return __sync_pend(sem);
    }
    return 0;
}
#endif

#endif      // VSF_USE_LINUX
