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

#ifndef __LWIP_SYS_ARCH_H__
#define __LWIP_SYS_ARCH_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "lwipopts.h"

/*============================ MACROS ========================================*/

#if !NO_SYS
#   if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#       error "VSF_KERNEL_CFG_SUPPORT_THREAD must be enabled for SYS mode of lwip"
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if !NO_SYS

struct sys_mbox_t {
    implement(vsf_queue_t)
    uint16_t head;
    uint16_t tail;
    void **queue;
};
typedef struct sys_mbox_t sys_mbox_t;
typedef vsf_sem_t sys_sem_t;
typedef vsf_mutex_t sys_mutex_t;

#if SYS_LIGHTWEIGHT_PROT
typedef vsf_protect_t sys_prot_t;
#endif

typedef struct vsf_rtos_thread_t * sys_thread_t;

#endif      // NO_SYS

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif	// __LWIP_SYS_ARCH_H__
