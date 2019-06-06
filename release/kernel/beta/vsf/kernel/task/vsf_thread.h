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


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_THREAD_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_THREAD_CLASS_IMPLEMENT
#elif   defined(__VSF_THREAD_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_THREAD_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

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
                uint64_t    stack_arr[((__STACK)+7)/8];                         \
                implement_ex(vsf_thread_##__NAME##_t, param);                   \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__NAME##_start(__NAME *task,               \
                                                    vsf_priority_t priority);       
#define def_vsf_thread(__NAME, __STACK, ...)                                    \
            __def_vsf_thread(__NAME, __STACK, __VA_ARGS__)

#define __implement_vsf_thread(__NAME)                                          \
            static void vsf_thread_##__NAME##_entry(                            \
                        vsf_thread_##__NAME##_t *ptTthis);                      \
            void vsf_thread_##__NAME##_start(   __NAME *task,                   \
                                                vsf_priority_t priority)        \
            {                                                                   \
                ASSERT(NULL != task);                                           \
                vsf_thread_t *pthis =                                           \
                    &(task->param.use_as__vsf_thread_t);                        \
                pthis->entry = (vsf_thread_entry_t *)                           \
                                    &vsf_thread_##__NAME##_entry;               \
                pthis->stack = task->stack_arr;                                 \
                pthis->stack_size = sizeof(task->stack_arr);                    \
                vsf_thread_start(pthis, priority);                              \
            }                                                                   \
            static void vsf_thread_##__NAME##_entry(                            \
                        vsf_thread_##__NAME##_t *ptThis)

#define implement_vsf_thread(__NAME)    __implement_vsf_thread(__NAME)

#define __init_vsf_thread(__NAME, __TASK, __PRI)                               \
            vsf_thread_##__NAME##_start((__TASK), (__PRI))

#define init_vsf_thread(__NAME, __TASK, __PRI)                                 \
            __init_vsf_thread(__NAME, (__TASK), (__PRI))


/*============================ TYPES =========================================*/

declare_class(vsf_thread_t)

typedef void vsf_thread_entry_t(vsf_thread_t *thread);

//! \name thread
//! @{
def_class(vsf_thread_t,
    which(
#if VSF_CFG_TIMER_EN
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    )

    public_member(
        // you can add public member here
        vsf_thread_entry_t  *entry;
        uint16_t            stack_size;
        uint64_t            *stack;                 //!< stack must be 8byte aligned
    ),

    private_member(
        jmp_buf         *pos;
        jmp_buf         *ret;
    )
)
end_def_class(vsf_thread_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

SECTION("text.vsf.kernel.vsf_thread")
extern vsf_err_t vsf_thread_start(vsf_thread_t *ptThis, vsf_priority_t tPriority);

SECTION("text.vsf.kernel.vsf_thread_get_cur")
extern vsf_thread_t *vsf_thread_get_cur(void);

SECTION("text.vsf.kernel.vsf_thread_ret")
extern void vsf_thread_ret(void);

SECTION("text.vsf.kernel.vsf_thread_wait")
extern vsf_evt_t vsf_thread_wait(void);

SECTION("text.vsf.kernel.vsf_thread_wfe")
extern void vsf_thread_wfe(vsf_evt_t evt);

SECTION("text.vsf.kernel.vsf_thread_sendevt")
extern void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt);

#if VSF_CFG_TIMER_EN
SECTION("text.vsf.kernel.vsf_thread_delay")
extern void vsf_thread_delay(uint_fast32_t tick);
#endif

#if VSF_CFG_SYNC_EN
SECTION("text.vsf.kernel.vsf_thread_mutex")
extern vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, int_fast32_t timeout);

SECTION("text.vsf.kernel.vsf_thread_mutex")
extern vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

SECTION("text.vsf.kernel.vsf_thread_sem_post")
extern vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem);

SECTION("text.vsf.kernel.vsf_thread_mutex")
extern vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *sem, int_fast32_t timeout);

#   if VSF_CFG_BMPEVT_EN
SECTION("text.vsf.kernel.vsf_thread_bmpevt_pend")
extern vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    int_fast32_t timeout);
#   endif
#endif

#endif

#endif
