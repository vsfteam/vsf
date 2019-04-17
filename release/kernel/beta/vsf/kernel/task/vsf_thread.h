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

#ifndef __VSF_THREAD_H__
#define __VSF_THREAD_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL_THREAD_MODE == ENABLED
#include <setjmp.h>
#include "../vsf_eda.h"
#include "./__class_thread.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_vsf_thread(__NAME)                                            \
            typedef struct __NAME __NAME;                                       \
            typedef struct vsf_thread_##__NAME##_t vsf_thread_##__NAME##_t;
#define declare_vsf_thread(__NAME)  __declare_vsf_thread(__NAME)

#define __def_vsf_thread(__NAME, __STACK, ...)                                  \
            struct vsf_thread_##__NAME##_t {                                    \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __NAME {                                                     \
                uint64_t    stack[((__STACK)+7)/8];                             \
                implement(vsf_thread_##__NAME##_t);                             \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__NAME##_start(__NAME *ptask,              \
                                                    vsf_priority_t priority);       
#define def_vsf_thread(__NAME, __STACK, ...)                                    \
            __def_vsf_thread(__NAME, __STACK, __VA_ARGS__)

#define __implement_vsf_thread(__NAME)                                          \
            static void vsf_thread_##__NAME##_entry(                            \
                        vsf_thread_##__NAME##_t *ptTthis);                      \
            void vsf_thread_##__NAME##_start(   __NAME *ptask,                  \
                                                vsf_priority_t priority)        \
            {                                                                   \
                ASSERT(NULL != ptask);                                          \
                ptask->pentry = (vsf_thread_entry_t *)                          \
                                    &vsf_thread_##__NAME##_entry;               \
                ptask->pstack = ptask->stack;                                   \
                ptask->stack_size = sizeof(ptask->stack);                       \
                vsf_thread_start(&(ptask->use_as__vsf_thread_t), priority);     \
            }                                                                   \
            static void vsf_thread_##__NAME##_entry(                            \
                        vsf_thread_##__NAME##_t *ptThis)

#define implement_vsf_thread(__NAME)    __implement_vsf_thread(__NAME)

#define __init_vsf_thread(__NAME, __TASK, __PRI)                               \
            vsf_thread_##__NAME##_start((__TASK), (__PRI))

#define init_vsf_thread(__NAME, __TASK, __PRI)                                 \
            __init_vsf_thread(__NAME, (__TASK), (__PRI))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_thread_start(vsf_thread_t *ptThis, vsf_priority_t tPriority);

extern vsf_thread_t *vsf_thread_get_cur(void);

extern void vsf_thread_ret(void);

extern vsf_evt_t vsf_thread_wait(void);

extern void vsf_thread_wfe(vsf_evt_t evt);

extern void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt);

#if VSF_CFG_TIMER_EN
extern void vsf_thread_delay(uint_fast32_t ms);
#endif

#if VSF_CFG_SYNC_EN
extern vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *pmtx, int_fast32_t timeout);

extern vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *pmtx);

extern vsf_err_t vsf_thread_sem_post(vsf_sem_t *psem);

extern vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *psem, int_fast32_t timeout);

#   if VSF_CFG_BMPEVT_EN
extern vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *pbmpevt,
                    vsf_bmpevt_pender_t *ppender,
                    int_fast32_t timeout);
#   endif
#endif

#endif

#endif
