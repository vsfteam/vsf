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

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED
#include "../vsf_eda.h"


/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if     defined(__VSF_THREAD_CLASS_IMPLEMENT)
#   undef __VSF_THREAD_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_THREAD_CLASS_INHERIT__)
#   undef __VSF_THREAD_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE	    1
#endif
#ifndef VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE
#   define VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE    0
#endif

#ifndef VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT
#   ifdef VSF_ARCH_CFG_STACK_ALIGN_BIT
#       define VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT    VSF_ARCH_CFG_STACK_ALIGN_BIT
#   else
#       define VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT    3
#   endif
#endif

#define __VSF_THREAD_STACK_SAFE_SIZE(__stack)                                   \
            (   (   (   ((__stack) + VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1) \
                    /   VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                  \
                * VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                        \
            + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)


/*============================ MACROFIED FUNCTIONS ===========================*/

#define __declare_vsf_thread(__name)                                            \
            typedef struct __name __name;                                       \
            typedef struct thread_cb_##__name##_t thread_cb_##__name##_t;
#define declare_vsf_thread(__name)      __declare_vsf_thread(__name)
#define declare_vsf_thread_ex(__name)   __declare_vsf_thread(__name)

#define dcl_vsf_thread(__name)          declare_vsf_thread(__name)
#define dcl_vsf_thread_ex(__name)       declare_vsf_thread(__name)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __def_vsf_thread(__name, __stack, ...)                               \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
                uint32_t canary;                                                \
                uint64_t stack_arr[(__VSF_THREAD_STACK_SAFE_SIZE(__stack)+7)/8] \
                        ALIGN(1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT);      \
            };                                                                  \
            struct __name {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__name##_t, param)                     \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                            struct thread_cb_##__name##_t *ptTthis);

#   define __implement_vsf_thread(__name)                                       \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *ptTthis);                \
            void vsf_thread_##__name##_start( struct __name *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                thread_cb_##__name##_t *this_ptr =  &(task->param);             \
                this_ptr->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)\
                                    &vsf_thread_##__name##_entry;               \
                this_ptr->use_as__vsf_thread_cb_t.stack = task->param.stack_arr;\
                this_ptr->use_as__vsf_thread_cb_t.stack_size =                  \
                    sizeof(task->param.stack_arr);                              \
                this_ptr->canary = 0xDEADBEEF;                                  \
                vsf_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(task->param.use_as__vsf_thread_cb_t),     \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *this_ptr)

#   define __vsf_eda_call_thread_prepare__(__name, __thread_cb)                 \
            do {                                                                \
                thread_cb_##__name##_t *this_ptr = (__thread_cb);               \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__name##_entry,               \
                    .stack = (__thread_cb)->stack_arr,                          \
                    .stack_size = sizeof((__thread_cb)->stack_arr),             \
                };                                                              \
                vk_eda_call_thread_prepare(&(this_ptr->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)

#   define vsf_eda_call_thread_prepare(__name, __thread_cb)                     \
                __vsf_eda_call_thread_prepare__(__name, __thread_cb)

#   define vsf_eda_call_thread(__thread_cb)                                     \
                vk_eda_call_thread(&((__thread_cb)->use_as__vsf_thread_cb_t))


#   define __def_vsf_thread_ex(__name, ...)                                     \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_cb_t)                                      \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __name {                                                     \
                implement(vsf_thread_t)                                         \
                implement_ex(thread_cb_##__name##_t, param)                     \
            };                                                                  \
            extern void vsf_thread_##__name##_start( struct __name *task,       \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size);            \
            extern void vsf_thread_##__name##_entry(                            \
                            struct thread_cb_##__name##_t *ptTthis);


#   define __implement_vsf_thread_ex(__name)                                    \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *ptTthis);                \
            void vsf_thread_##__name##_start(   struct __name *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                thread_cb_##__name##_t *this_ptr =  &(task->param);             \
                this_ptr->use_as__vsf_thread_cb_t.entry = (vsf_thread_entry_t *)\
                                    &vsf_thread_##__name##_entry;               \
                this_ptr->use_as__vsf_thread_cb_t.stack = stack;                \
                this_ptr->use_as__vsf_thread_cb_t.stack_size = size;            \
                vsf_thread_start(   &(task->use_as__vsf_thread_t),              \
                                    &(task->param.use_as__vsf_thread_cb_t),     \
                                    priority);                                  \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *this_ptr)

#   define __vsf_eda_call_thread_prepare_ex__(  __name,                         \
                                                __thread_cb,                    \
                                                __stack,                        \
                                                __size)                         \
            do {                                                                \
                VSF_KERNEL_ASSERT((NULL != (__stack)) && (0 != (__size)))       \
                thread_cb_##__name##_t *this_ptr = (__thread_cb);               \
                const vsf_thread_prepare_cfg_t cfg = {                          \
                    .entry = (vsf_thread_entry_t *)                             \
                                    &vsf_thread_##__name##_entry,               \
                    .stack = (__stack),                                         \
                    .stack_size = (__size),                                     \
                };                                                              \
                vk_eda_call_thread_prepare(&(this_ptr->use_as__vsf_thread_cb_t),\
                                              (vsf_thread_prepare_cfg_t *)&cfg);\
            } while(0)


#   define vsf_eda_call_thread_prepare_ex(  __name,                             \
                                            __thread_cb,                        \
                                            __stack,                            \
                                            __size)                             \
                __vsf_eda_call_thread_prepare_ex__( __name,                     \
                                                    (__thread_cb),              \
                                                    (__stack),                  \
                                                    (__size))

#   define vsf_eda_call_thread_ex(__thread_cb)                                  \
                vk_eda_call_thread(&((__thread_cb)->use_as__vsf_thread_cb_t))

#else
#   define __def_vsf_thread(__name, __stack, ...)                               \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __name {                                                     \
                uint64_t stack_arr[(__VSF_THREAD_STACK_SAFE_SIZE(__stack)+7)/8] \
                        ALIGN(1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT);      \
                implement_ex(thread_cb_##__name##_t, param);                    \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                        struct thread_cb_##__name##_t *ptTthis);

#   define __implement_vsf_thread(__name)                                       \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *ptTthis);                \
            void vsf_thread_##__name##_start( struct __name *task,              \
                                                vsf_prio_t priority)            \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task);                                \
                vsf_thread_t *this_ptr =                                        \
                    &(task->param.use_as__vsf_thread_t);                        \
                this_ptr->entry = (vsf_thread_entry_t *)                        \
                                    &vsf_thread_##__name##_entry;               \
                this_ptr->stack = task->stack_arr;                              \
                this_ptr->stack_size = sizeof(task->stack_arr);                 \
                vsf_thread_start(this_ptr, priority);                           \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *this_ptr)

#   define __def_vsf_thread_ex(__name, ...)                                     \
            struct thread_cb_##__name##_t {                                     \
                implement(vsf_thread_t)                                         \
                __VA_ARGS__                                                     \
            };                                                                  \
            struct __name {                                                     \
                implement_ex(thread_cb_##__name##_t, param);                    \
            }ALIGN(8);                                                          \
            extern void vsf_thread_##__name##_start(struct __name *task,        \
                                                    vsf_prio_t priority);       \
            extern void vsf_thread_##__name##_entry(                            \
                        struct thread_cb_##__name##_t *this_ptr);


#   define __implement_vsf_thread_ex(__name)                                    \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *this_ptr);               \
            void vsf_thread_##__name##_start(   struct __name *task,            \
                                                vsf_prio_t priority,            \
                                                void *stack,                    \
                                                uint_fast32_t size)             \
            {                                                                   \
                VSF_KERNEL_ASSERT(NULL != task && 0 != size && NULL != stack);  \
                vsf_thread_t *this_ptr =                                        \
                    &(task->param.use_as__vsf_thread_t);                        \
                this_ptr->entry = (vsf_thread_entry_t *)                        \
                                    &vsf_thread_##__name##_entry;               \
                this_ptr->stack = stack;                                        \
                this_ptr->stack_size = size;                                    \
                vsf_thread_start(this_ptr, priority);                           \
            }                                                                   \
            void vsf_thread_##__name##_entry(                                   \
                        struct thread_cb_##__name##_t *this_ptr)

#endif


#define def_vsf_thread(__name, __stack, ...)                                    \
            __def_vsf_thread(__name, __stack, __VA_ARGS__)

#define define_vsf_thread(__name, __stack, ...)                                 \
            def_vsf_thread(__name, __stack, __VA_ARGS__)

#define def_vsf_thread_ex(__name, ...)                                          \
            __def_vsf_thread_ex(__name, __VA_ARGS__)

#define define_vsf_thread_ex(__name, ...)                                       \
            def_vsf_thread_ex(__name, __VA_ARGS__)

#define end_def_vsf_thread(...)
#define end_define_vsf_thread(...)

#define implement_vsf_thread(__name)        __implement_vsf_thread(__name)
#define implement_vsf_thread_ex(__name)     __implement_vsf_thread_ex(__name)

#define imp_vsf_thread(__name)              implement_vsf_thread(__name)
#define imp_vsf_thread_ex(__name)           implement_vsf_thread_ex(__name)

#define __init_vsf_thread(__name, __task, __pri)                                \
            vsf_thread_##__name##_start((__task), (__pri))

#define __init_vsf_thread_ex(__name, __task, __pri, __stack, __size)            \
            vsf_thread_##__name##_start((__task), (__pri), (__stack), (__size))

#define init_vsf_thread(__name, __task, __pri)                                  \
            __init_vsf_thread(__name, (__task), (__pri))

#define init_vsf_thread_ex(__name, __task, __pri, __stack, __size)              \
            __init_vsf_thread_ex(__name, (__task), (__pri), (__stack), (__size))

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread(__name)      thread_cb_##__name##_t
#   define vsf_thread(__name)        __vsf_thread(__name)

#endif

#define vsf_thread_wfm      vsf_thread_wait_for_msg
#define vsf_thread_wfe      vsf_thread_wait_for_evt

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
#   define vsf_thread_wfem     vsf_thread_wait_for_evt_msg
#endif


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define __vsf_thread_call_sub(__name, __target, ...)                         \
            vk_thread_call_eda( (uintptr_t)(__name),                            \
                                (uintptr_t)(__target),                          \
                                (0, ##__VA_ARGS__),                             \
                                0,                                              \
                                0)


#   define vsf_thread_call_sub(__name, __target, ...)                           \
            __vsf_thread_call_sub(__name, (__target), (0, ##__VA_ARGS__))


#   define vsf_thread_call_pt(__name, __target, ...)                            \
            (__target)->fsm_state = 0;                                          \
            vsf_thread_call_sub(vsf_pt_func(__name), (__target), (0, ##__VA_ARGS__))

#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED

#   define vsf_thread_call_fsm(__name, __target, ...)                           \
                vk_thread_call_fsm(  (vsf_fsm_entry_t)(__name),                 \
                                        (uintptr_t)(__target), (0, ##__VA_ARGS__))

#   define vsf_thread_call_task(__name, __target, ...)                          \
                vsf_thread_call_fsm(vsf_task_func(__name), __target, (0, ##__VA_ARGS__))
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define vsf_thread_delay_ms(__ms)            vsf_thread_delay(vsf_systimer_ms_to_tick(__ms))
#   define vsf_thread_delay_us(__us)            vsf_thread_delay(vsf_systimer_us_to_tick(__us))
#endif

/*============================ TYPES =========================================*/

declare_class(vsf_thread_t)

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
declare_class(vsf_thread_cb_t)

typedef void vsf_thread_entry_t(vsf_thread_cb_t *thread);

def_class( vsf_thread_t,
    which(
    #   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
    #   else
        implement(vsf_eda_t)
    #   endif
    )
)
end_def_class(vsf_thread_t)
//! \name thread
//! @{
def_class(vsf_thread_cb_t,

    public_member(
        vsf_thread_entry_t      *entry;
        uint16_t                stack_size;
        uint64_t                *stack;                 //!< stack must be 8byte aligned
    )

    private_member(
#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
        vsf_arch_irq_thread_t   host_thread;
        vsf_arch_irq_request_t  req, *rep;
        vsf_evt_t               evt;
#else
        jmp_buf                 *pos;
        jmp_buf                 *ret;
#endif
    )
)
end_def_class(vsf_thread_cb_t)
//! @}

typedef struct {
    vsf_thread_entry_t          *entry;
    void                        *stack;
    uint_fast32_t               stack_size;
    //vsf_prio_t                  priority;         //!< TODO do we need this??
} vsf_thread_prepare_cfg_t;


#else


typedef void vsf_thread_entry_t(vsf_thread_t *thread);

//! \name thread
//! @{
def_class(vsf_thread_t,
    which(
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        implement(vsf_teda_t)
#else
        implement(vsf_eda_t)
#endif
    )

    public_member(
        vsf_thread_entry_t      *entry;
        uint32_t                stack_size;
        uint64_t                *stack;                 //!< stack must be 8byte aligned
    ),

    private_member(
#ifdef VSF_ARCH_LIMIT_NO_SET_STACK
        vsf_arch_irq_thread_t   host_thread;
        vsf_arch_irq_request_t  req, *rep;
        vsf_evt_t               evt;
#else
        jmp_buf                 *pos;
        jmp_buf                 *ret;
#endif
    )
)
end_def_class(vsf_thread_t)
//! @}
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.vk_eda_call_thread_prepare")
extern vsf_err_t vk_eda_call_thread_prepare( vsf_thread_cb_t *thread_cb,
                                                vsf_thread_prepare_cfg_t *cfg);

SECTION(".text.vsf.kernel.vk_eda_call_thread")
extern vsf_err_t vk_eda_call_thread(vsf_thread_cb_t *thread_cb);

SECTION(".text.vsf.kernel.vk_thread_call_fsm")
extern
fsm_rt_t vk_thread_call_fsm(vsf_fsm_entry_t eda_handler, uintptr_t param, size_t local_size);

SECTION(".text.vsf.kernel.vk_thread_call_eda")
extern
vsf_err_t vk_thread_call_eda(   uintptr_t eda_handler,
                                uintptr_t param,
                                size_t local_size,
                                size_t local_buff_size,
                                uintptr_t local_buff);

SECTION(".text.vsf.kernel.vsf_thread_call_thread")
extern vsf_err_t vk_thread_call_thread(     vsf_thread_cb_t *thread_cb,
                                            vsf_thread_prepare_cfg_t *cfg);

#endif


SECTION(".text.vsf.kernel.vsf_thread")
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
extern vsf_err_t vsf_thread_start(  vsf_thread_t *thread,
                                    vsf_thread_cb_t *thread_cb,
                                    vsf_prio_t priority);
#else
extern vsf_err_t vsf_thread_start(vsf_thread_t *this_ptr, vsf_prio_t priority);
#endif

SECTION(".text.vsf.kernel.vsf_thread_exit")
extern void vsf_thread_exit(void);

SECTION(".text.vsf.kernel.vsf_thread_get_cur")
extern vsf_thread_t *vsf_thread_get_cur(void);

SECTION(".text.vsf.kernel.vsf_thread_wait")
extern vsf_evt_t vsf_thread_wait(void);

SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt")
extern void vsf_thread_wait_for_evt(vsf_evt_t evt);

SECTION(".text.vsf.kernel.vsf_thread_sendevt")
extern void vsf_thread_sendevt(vsf_thread_t *thread, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
extern uintptr_t vsf_thread_wait_for_evt_msg(vsf_evt_t evt);
#endif

SECTION(".text.vsf.kernel.vsf_thread_wait_for_evt_msg")
extern uintptr_t vsf_thread_wait_for_msg(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_delay")
extern void vsf_thread_delay(uint_fast32_t tick);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_mutex")
extern vsf_sync_reason_t vsf_thread_mutex_enter(vsf_mutex_t *mtx, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_thread_mutex")
extern vsf_err_t vsf_thread_mutex_leave(vsf_mutex_t *mtx);

SECTION(".text.vsf.kernel.vsf_thread_sem_post")
extern vsf_err_t vsf_thread_sem_post(vsf_sem_t *sem);

SECTION(".text.vsf.kernel.__vsf_thread_wait_for_sync")
extern vsf_sync_reason_t vsf_thread_sem_pend(vsf_sem_t *sem, int_fast32_t timeout);

SECTION(".text.vsf.kernel.__vsf_thread_wait_for_sync")
extern vsf_sync_reason_t vsf_thread_trig_pend(vsf_trig_t *trig, int_fast32_t timeout);

#   if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
SECTION(".text.vsf.kernel.vsf_thread_bmpevt_pend")
extern vsf_sync_reason_t vsf_thread_bmpevt_pend(
                    vsf_bmpevt_t *bmpevt,
                    vsf_bmpevt_pender_t *pender,
                    int_fast32_t timeout);
#   endif
#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
