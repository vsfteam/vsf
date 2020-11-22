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

#define __VSF_ARCH_WIN_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"

#if VSF_ARCH_PRI_NUM != 1 || VSF_ARCH_SWI_NUM != 0

// for trace
#include "service/vsf_service.h"
// for list
#include "utilities/vsf_utilities.h"
#include "hal/arch/__vsf_arch_interface.h"

#include <Windows.h>

/*============================ MACROS ========================================*/

// configuration start
//#define VSF_ARCH_CFG_BACKGROUND_TRACE           ENABLED
#define VSF_ARCH_CFG_LOW_LANTECY                DISABLED
//#define VSF_ARCH_CFG_BACKGROUND_TRACE_FILTER    &__vsf_x86.systimer.use_as__vsf_arch_irq_thread_t
//#define VSF_ARCH_CFG_BACKGROUND_TRACE_NUM       (32 * 1024)
//#define VSF_ARCH_CFG_TRACE_IRQ                  ENABLED
//#define VSF_ARCH_CFG_TRACE_CALLSTACK            ENABLED
//#define VSF_ARCH_CFG_TRACE_SYSTIMER             ENABLED
//#define VSF_ARCH_CFG_TRACE_STATUS               ENABLED
//#define VSF_ARCH_CFG_TRACE_FUNCTION             ENABLED
// configuration end






//! GetSystemTimeAsFileTime has 100ns resolution, which is 10MHz
#define VSF_ARCH_SYSTIMER_FREQ              (10 * 1000 * 1000)

#if VSF_ARCH_CFG_LOW_LANTECY == ENABLED
#   define vsf_arch_yield()                 Sleep(0)
#else
#   define vsf_arch_yield()                 Sleep(1)
#endif

#if     defined(__CPU_X86__)
#   define VSF_ARCH_TRACE_HEADER            "arch/x86"
#elif   defined(__CPU_X64__)
#   define VSF_ARCH_TRACE_HEADER            "arch/x64"
#endif

#undef vsf_arch_trace_callstack
#if VSF_ARCH_CFG_TRACE_CALLSTACK == ENABLED
#   define vsf_arch_trace_callstack(...)                                        \
        vsf_arch_trace(VSF_ARCH_TRACE_HEADER,                                   \
            __vsf_x86_trace_color[VSF_ARCH_TRACE_CALLSTACK_COLOR], __VA_ARGS__)
#else
#   define vsf_arch_trace_callstack(...)
#endif

#undef vsf_arch_trace_irq
#if VSF_ARCH_TRACE_IRQ_EN == ENABLED
#   define vsf_arch_trace_irq(...)                                              \
        vsf_arch_trace(VSF_ARCH_TRACE_HEADER,                                   \
            __vsf_x86_trace_color[VSF_ARCH_TRACE_IRQ_COLOR], __VA_ARGS__)
#else
#   define vsf_arch_trace_irq(...)
#endif

#undef vsf_arch_trace_systimer
#if VSF_ARCH_CFG_TRACE_SYSTIMER == ENABLED
#   define vsf_arch_trace_systimer(...)                                         \
        vsf_arch_trace(VSF_ARCH_TRACE_HEADER,                                   \
            __vsf_x86_trace_color[VSF_ARCH_TRACE_SYSTIMER_COLOR], __VA_ARGS__)
#else
#   define vsf_arch_trace_systimer(...)
#endif

#undef vsf_arch_trace_status
#if VSF_ARCH_CFG_TRACE_STATUS == ENABLED
#   define vsf_arch_trace_status(...)                                           \
        vsf_arch_trace(VSF_ARCH_TRACE_HEADER,                                   \
            __vsf_x86_trace_color[VSF_ARCH_TRACE_STATUS_COLOR], __VA_ARGS__)
#else
#   define vsf_arch_trace_status(...)
#endif

#undef vsf_arch_trace_function
#if VSF_ARCH_CFG_TRACE_FUNCTION == ENABLED
#   define vsf_arch_trace_function(...)                                         \
        vsf_arch_trace(VSF_ARCH_TRACE_HEADER,                                   \
            __vsf_x86_trace_color[VSF_ARCH_TRACE_FUNCTION_COLOR], __VA_ARGS__)
#else
#   define vsf_arch_trace_function(...)
#endif

#if VSF_ARCH_CFG_TRACE_FUNCTION == ENABLED
#endif

#if VSF_ARCH_CFG_BACKGROUND_TRACE == ENABLED
#   ifndef VSF_ARCH_CFG_BACKGROUND_TRACE_NUM
#       define VSF_ARCH_CFG_BACKGROUND_TRACE_NUM        1024
#   endif
#endif

#if     VSF_ARCH_CFG_TRACE_IRQ == ENABLED                                       \
    ||  VSF_ARCH_CFG_TRACE_CALLSTACK == ENABLED                                 \
    ||  VSF_ARCH_CFG_TRACE_SYSTIMER == ENABLED                                  \
    ||  VSF_ARCH_CFG_TRACE_STATUS == ENABLED                                    \
    ||  VSF_ARCH_CFG_TRACE_FUNCTION == ENABLED
#   define __VSF_ARCH_CFG_TRACE                 ENABLED
#endif

// disable VSF_ARCH_CFG_NO_UNLOCKED_SUSPEND_EN will be unstable
#define VSF_ARCH_CFG_NO_UNLOCKED_SUSPEND_EN     ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_arch_swi_ctx_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t request;
    bool inited;

    vsf_irq_handler_t *handler;
    void *param;
} vsf_arch_swi_ctx_t;

typedef struct vsf_arch_systimer_ctx_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t timer_request;
    HANDLE timer;
    vsf_systimer_cnt_t start_tick;
} vsf_arch_systimer_ctx_t;

typedef struct vsf_x86_t {
    CRITICAL_SECTION lock;
    CRITICAL_SECTION trace_lock;

    vsf_arch_irq_thread_t por_thread;    // power on reset
    vsf_arch_swi_ctx_t swi[VSF_ARCH_SWI_NUM];
    vsf_arch_systimer_ctx_t systimer;

    vsf_dlist_t irq_list;
    vsf_dlist_t irq_rdy_list;
    vsf_arch_irq_thread_t *cur_thread;
    vsf_arch_irq_thread_t *cur_lock_owner;
    vsf_arch_irq_thread_t *lock_owner;
    vsf_arch_prio_t cur_priority;
    vsf_arch_prio_t prio_base;
    vsf_gint_state_t gint_state;
} vsf_x86_t;

//! preempt_param
typedef enum vsf_arch_irq_pp_t {
    VSF_ARCH_IRQ_PP_NORMAL,
    // below is ONLY used in __vsf_arch_irq_deactivate
    // indicating the irq is end, and should current thread suspend or still running
    VSF_ARCH_IRQ_PP_EXIT_SUSPEND,
    VSF_ARCH_IRQ_PP_EXIT_RUNNING,
} vsf_arch_irq_pp_t;

#if __VSF_ARCH_CFG_TRACE == ENABLED
enum {
    VSF_ARCH_TRACE_CALLSTACK_COLOR,
    VSF_ARCH_TRACE_IRQ_COLOR,
    VSF_ARCH_TRACE_SYSTIMER_COLOR,
    VSF_ARCH_TRACE_STATUS_COLOR,
    VSF_ARCH_TRACE_FUNCTION_COLOR,
};
#endif

typedef enum vsf_arch_trace_event_t {
    VSF_ARCH_TRACE_PREEMPT_ENTER,
    VSF_ARCH_TRACE_PREEMPT_LEAVE,
    VSF_ARCH_TRACE_CUR_THREAD,
    VSF_ARCH_TRACE_ACTIVATE,
    VSF_ARCH_TRACE_DEACTIVATE,
    VSF_ARCH_TRACE_SUSPEND,
    VSF_ARCH_TRACE_RESUME,
    VSF_ARCH_TRACE_IRQ_STATE,
    VSF_ARCH_TRACE_ACTIVATED,
    VSF_ARCH_TRACE_DEACTIVATED,
    VSF_ARCH_TRACE_STATUS,
    VSF_ARCH_TRACE_LOCK,
    VSF_ARCH_TRACE_POP,
    VSF_ARCH_TRACE_INHERIT,
    VSF_ARCH_TRACE_PUSH,
} vsf_arch_trace_event_t;

#if VSF_ARCH_CFG_BACKGROUND_TRACE == ENABLED
typedef struct vsf_arch_background_trace_event_t {
    uint32_t idx;
    vsf_arch_trace_event_t event;

    vsf_arch_irq_thread_t *caller_thread;
    vsf_arch_irq_thread_t *cur_thread;
    uint32_t status;
    uint32_t info;
    uint32_t pos;
} vsf_arch_background_trace_event_t;

typedef struct vsf_arch_background_trace_t {
    vsf_arch_background_trace_event_t events[1024];
    uint32_t pos;
    uint32_t idx;
} vsf_arch_background_trace_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

NO_INIT static vsf_x86_t __vsf_x86;

#if __VSF_ARCH_CFG_TRACE == ENABLED
static const char *__vsf_x86_trace_color[] = {
    [VSF_ARCH_TRACE_CALLSTACK_COLOR]    = "\033[1;32m",
    [VSF_ARCH_TRACE_IRQ_COLOR]          = "\033[1;33m",
    [VSF_ARCH_TRACE_SYSTIMER_COLOR]     = "\033[1;35m",
    [VSF_ARCH_TRACE_STATUS_COLOR]       = "\033[1;36m",
    [VSF_ARCH_TRACE_FUNCTION_COLOR]     = "\033[1;34m",
};
#endif

#if VSF_ARCH_CFG_BACKGROUND_TRACE == ENABLED
static vsf_arch_background_trace_t __vsf_arch_background_trace;
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_ARCH_CFG_NO_UNLOCKED_SUSPEND_EN == ENABLED
static void __vsf_arch_irq_pend(vsf_arch_irq_thread_t *irq_thread);
#endif

/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * infrastructure                                                             *
 *----------------------------------------------------------------------------*/


#if VSF_ARCH_CFG_BACKGROUND_TRACE == ENABLED
static vsf_arch_background_trace_event_t * __vsf_arch_bg_trace_get(void)
{
    vsf_arch_background_trace_event_t *event = &__vsf_arch_background_trace.events[__vsf_arch_background_trace.pos++];
    if (__vsf_arch_background_trace.pos >= dimof(__vsf_arch_background_trace.events)) {
        __vsf_arch_background_trace.pos = 0;
    }
    event->idx = __vsf_arch_background_trace.idx++;
    return event;
}

static void __vsf_arch_bg_trace(int event, vsf_arch_irq_thread_t *caller_thread,
                vsf_arch_irq_thread_t *cur_thread, int info, int pos)
{
#ifdef VSF_ARCH_CFG_BACKGROUND_TRACE_FILTER
    if (    (VSF_ARCH_CFG_BACKGROUND_TRACE_FILTER == caller_thread)
        ||  (VSF_ARCH_CFG_BACKGROUND_TRACE_FILTER == cur_thread))
#endif
    {
        vsf_arch_background_trace_event_t *trace_event = __vsf_arch_bg_trace_get();
        trace_event->caller_thread = caller_thread;
        trace_event->cur_thread = cur_thread;
        trace_event->event = event;
        trace_event->info = info;
        trace_event->pos = pos;
        trace_event->status = ((__vsf_x86.prio_base & 0xFF) << 8) | ((__vsf_x86.gint_state & 0xFF) << 0);
    }
}
#else
static void __vsf_arch_bg_trace(int event, vsf_arch_irq_thread_t *caller_thread,
                vsf_arch_irq_thread_t *cur_thread, int info, int pos)
{
}
#endif

static vsf_arch_irq_thread_t * __vsf_arch_get_cur_irq_thread(void)
{
    DWORD thread_id = GetCurrentThreadId();

    __vsf_dlist_foreach_unsafe(vsf_arch_irq_thread_t, irq_node, &__vsf_x86.irq_list) {
        if (thread_id == _->thread_id) {
            return _;
        }
    }
    return NULL;
}

void __vsf_arch_trace_lock(void)
{
    BOOL is_entered;
    while (1) {
        is_entered = TryEnterCriticalSection(&__vsf_x86.trace_lock);
        if (is_entered) {
            break;
        } else {
            vsf_arch_yield();
        }
    }
}

void __vsf_arch_trace_unlock(void)
{
    LeaveCriticalSection(&__vsf_x86.trace_lock);
}

static void __vsf_arch_lock(void)
{
    // TODO: use EnterCriticalSection after stable
//    vsf_arch_irq_thread_t *irq_thread = __vsf_arch_get_cur_irq_thread();
//    VSF_HAL_ASSERT(irq_thread != NULL);
    BOOL is_entered;
    while (1) {
        is_entered = TryEnterCriticalSection(&__vsf_x86.lock);
        if (is_entered) {
            __vsf_x86.cur_lock_owner = __vsf_arch_get_cur_irq_thread();
            if (__vsf_x86.cur_lock_owner != __vsf_x86.lock_owner) {
                __vsf_x86.lock_owner = __vsf_x86.cur_lock_owner;
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_LOCK, __vsf_x86.cur_lock_owner, NULL, 1, 0);
            }
            break;
        } else {
            vsf_arch_yield();
        }
    }
//    EnterCriticalSection(&__vsf_x86.lock);
}

static void __vsf_arch_unlock(void)
{
//    __vsf_arch_bg_trace(VSF_ARCH_TRACE_LOCK, __vsf_x86.cur_lock_owner, NULL, 0, 0);
    __vsf_x86.cur_lock_owner = NULL;
    LeaveCriticalSection(&__vsf_x86.lock);
}

//! __vsf_arch_can_preempt MUST be called arch_locked
//  no_cur means no need to preempt if __vsf_x86.cur_thread is irq_thread
static bool __vsf_arch_can_preempt(vsf_arch_irq_thread_t *irq_thread, bool no_cur)
{
    if (no_cur) {
        // MCU status(prio_base or gint_state) changed
        return (irq_thread != __vsf_x86.cur_thread)
            &&  (irq_thread->priority > __vsf_x86.cur_thread->priority)
            &&  (irq_thread->priority > __vsf_x86.prio_base)
            &&  __vsf_x86.gint_state;
    } else {
        // interrupt activated
        // __vsf_x86.cur_thread can be irq_thread, consider this:
        //      threadA(background_running) activated self and wait to be preemptable
        //          threadB running
        //          threadB yield to threadA(set __vsf_x86.cur_thread to threadA)
        // irq_thread->is_running is checked in __vsf_arch_irq_activate
        return  (irq_thread->priority > __vsf_x86.cur_thread->priority)
            &&  (irq_thread->priority > __vsf_x86.prio_base)
            &&  __vsf_x86.gint_state;
    }
}

static void __vsf_arch_wait_suspended(vsf_arch_irq_thread_t *irq_thread)
{
    CONTEXT ctx;
    BOOL ret;

    while (1) {
        // call GetThreadContext to sync
        memset(&ctx, 0, sizeof(ctx));
        ret = GetThreadContext(irq_thread->thread, &ctx);
        if (ret) {
            break;
        } else {
            vsf_arch_yield();
        }
    }
}

/* __vsf_arch_preempt MUST be called arch_locked,
    and will unlock before return.
*/
static void __vsf_arch_preempt(vsf_arch_irq_pp_t pp)
{
    vsf_arch_irq_thread_t *irq_thread = NULL;
    vsf_arch_irq_thread_t *cur_irq_thread = __vsf_x86.cur_thread;
    vsf_arch_irq_thread_t *caller_irq_thread = __vsf_arch_get_cur_irq_thread();
    bool cur_thread_suspended = false, is_switched = false, is_popped = false;
    DWORD suspend_cnt;

    bool exit_suspend = (VSF_ARCH_IRQ_PP_EXIT_SUSPEND == pp);
    bool exit_running = (VSF_ARCH_IRQ_PP_EXIT_RUNNING == pp);
    bool irq_to_exit = exit_running || exit_suspend;

    vsf_dlist_queue_peek(vsf_arch_irq_thread_t, rdy_node,
        &__vsf_x86.irq_rdy_list, irq_thread);
    VSF_HAL_ASSERT(caller_irq_thread != NULL);
    VSF_HAL_ASSERT(cur_irq_thread != NULL);

    __vsf_arch_bg_trace(VSF_ARCH_TRACE_PREEMPT_ENTER, caller_irq_thread, cur_irq_thread,
            (exit_suspend << 0) | (exit_running << 8), 0);

    if (irq_to_exit) {
        VSF_HAL_ASSERT(cur_irq_thread == caller_irq_thread);
    }

    vsf_arch_trace_status("system status: gint:%d, base:%d\r\n",
            __vsf_x86.gint_state, (int)__vsf_x86.prio_base);

    if (cur_irq_thread != caller_irq_thread) {
        VSF_HAL_ASSERT(!exit_suspend);

        if (VSF_ARCH_IRQ_STATE_FOREGROUND == cur_irq_thread->state) {
            vsf_arch_trace_irq("%s suspend\r\n", cur_irq_thread->name);

            suspend_cnt = SuspendThread(cur_irq_thread->thread);
            __vsf_arch_bg_trace(VSF_ARCH_TRACE_SUSPEND, caller_irq_thread, cur_irq_thread, suspend_cnt, 0);
            // TODO: for por_thread, there are cases that suspend_cnt is 0
            //      work around here, but need to debug why later
            if (cur_irq_thread != &__vsf_x86.por_thread) {
                VSF_HAL_ASSERT(0 == suspend_cnt);
            }
            cur_irq_thread->state = VSF_ARCH_IRQ_STATE_ACTIVE;
            __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, cur_irq_thread, cur_irq_thread->state, 0);
            cur_thread_suspended = true;

            // sync
            __vsf_arch_wait_suspended(cur_irq_thread);
        }
    }

    if (    (irq_thread != NULL)
        &&  (irq_thread != cur_irq_thread)
        &&  (irq_to_exit || (irq_thread->priority > __vsf_x86.cur_priority))
        &&  __vsf_x86.gint_state
        &&  (   (__vsf_x86.prio_base < 0)
            ||  (NULL == irq_thread)
            ||  (irq_thread->priority > __vsf_x86.prio_base))) {

    do_pop:
        VSF_HAL_ASSERT(irq_thread != NULL);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_CUR_THREAD, caller_irq_thread, irq_thread, 0, 0);
        __vsf_x86.cur_thread = irq_thread;

        if (irq_thread != NULL) {
            is_switched = true;
            __vsf_x86.cur_priority = irq_thread->priority;

            if (    is_popped
                ||  (irq_to_exit && (irq_thread == cur_irq_thread->prev))) {
                // pop call stack;
//                vsf_arch_trace_callstack("%s popped from %s\r\n", irq_thread->name, cur_irq_thread->name);
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_POP, cur_irq_thread, irq_thread, 0, 0);
            } else if (irq_to_exit) {
                // inherit call stack
//                vsf_arch_trace_callstack("%s inherited from %s\r\n", irq_thread->name, cur_irq_thread->name);
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_INHERIT, irq_thread, cur_irq_thread, 0, 0);
                if (irq_thread != &__vsf_x86.por_thread) {
                    irq_thread->prev = cur_irq_thread->prev;
                }
            } else {
                // push call stack
                VSF_HAL_ASSERT(irq_thread != &__vsf_x86.por_thread);
//                vsf_arch_trace_callstack("%s pushed in %s\r\n", cur_irq_thread->name, irq_thread->name);
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_PUSH, irq_thread, cur_irq_thread, 0, 0);
                irq_thread->prev = cur_irq_thread;
            }

            vsf_arch_trace_callstack("%s(%d,%s) ==> %s(%d,%s)\r\n",
                cur_irq_thread->name, cur_irq_thread->priority,
                (NULL == cur_irq_thread->prev) ? "NULL" : cur_irq_thread->prev->name,
                irq_thread->name, irq_thread->priority,
                (NULL == irq_thread->prev) ? "NULL" : irq_thread->prev->name);

            VSF_HAL_ASSERT(irq_thread->state != VSF_ARCH_IRQ_STATE_FOREGROUND);
            if (irq_thread->state != VSF_ARCH_IRQ_STATE_BACKGROUND) {
                VSF_HAL_ASSERT(irq_thread != caller_irq_thread);
                vsf_arch_trace_irq("%s resume\r\n", irq_thread->name);

                // maybe SuspendThread is called, but not really suspend, wait first
                __vsf_arch_wait_suspended(irq_thread);
                suspend_cnt = ResumeThread(irq_thread->thread);
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_RESUME, caller_irq_thread, irq_thread, suspend_cnt, 0);
                // TODO: for por_thread, there are cases that suspend_cnt is 0
                //      work around here, but need to debug why later
                if (irq_thread != &__vsf_x86.por_thread) {
                    VSF_HAL_ASSERT(1 == suspend_cnt);
                }
            } else {
                vsf_arch_trace_irq("%s back ground running\r\n", irq_thread->name);
            }
            irq_thread->state = VSF_ARCH_IRQ_STATE_FOREGROUND;
            __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, irq_thread, irq_thread->state, 0);
        }
    }

    if (cur_thread_suspended) {
        if (!is_switched) {
            VSF_HAL_ASSERT(cur_irq_thread->state != VSF_ARCH_IRQ_STATE_FOREGROUND);
            cur_irq_thread->state = VSF_ARCH_IRQ_STATE_FOREGROUND;
            __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, cur_irq_thread, cur_irq_thread->state, 0);
            vsf_arch_trace_irq("%s resume\r\n", cur_irq_thread->name);
            suspend_cnt = ResumeThread(cur_irq_thread->thread);
            __vsf_arch_bg_trace(VSF_ARCH_TRACE_RESUME, caller_irq_thread, cur_irq_thread, suspend_cnt, 1);
            VSF_HAL_ASSERT(1 == suspend_cnt);
        }
    } else if (irq_to_exit || is_switched) {
        if (    irq_to_exit
            &&  !is_switched
            &&  (irq_thread != NULL)
            &&  (irq_thread != cur_irq_thread)) {

            VSF_HAL_ASSERT(cur_irq_thread->prev != NULL);
            irq_thread = cur_irq_thread->prev;
            is_popped = true;
            goto do_pop;
        }

        if (VSF_ARCH_IRQ_STATE_FOREGROUND == cur_irq_thread->state) {
            if (!exit_running) {
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_SUSPEND, caller_irq_thread, cur_irq_thread, -1, 1);
                if (cur_irq_thread != &__vsf_x86.por_thread) {
                    VSF_HAL_ASSERT(cur_irq_thread != __vsf_x86.cur_thread);
                }
                VSF_HAL_ASSERT(cur_irq_thread == caller_irq_thread);

#if VSF_ARCH_CFG_NO_UNLOCKED_SUSPEND_EN == ENABLED
                cur_irq_thread->state = VSF_ARCH_IRQ_STATE_BACKGROUND;
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, cur_irq_thread, cur_irq_thread->state, 0);
                vsf_arch_trace_irq("%s background suspend\r\n", cur_irq_thread->name);
                __vsf_arch_irq_pend(cur_irq_thread);
#else
                cur_irq_thread->state = VSF_ARCH_IRQ_STATE_IDLE;
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, cur_irq_thread, cur_irq_thread->state, 0);
                vsf_arch_trace_irq("%s suspend\r\n", cur_irq_thread->name);
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_PREEMPT_LEAVE, caller_irq_thread, cur_irq_thread, 0, 0);
                __vsf_arch_unlock();
                suspend_cnt = SuspendThread(cur_irq_thread->thread);
                if (cur_irq_thread != &__vsf_x86.por_thread) {
                    // TODO: debug only
                    __vsf_arch_lock();
                        __vsf_arch_bg_trace(VSF_ARCH_TRACE_SUSPEND, caller_irq_thread, cur_irq_thread, suspend_cnt, 3);
                    __vsf_arch_unlock();
                }
                return;
#endif
            } else {
                cur_irq_thread->state = VSF_ARCH_IRQ_STATE_BACKGROUND;
                __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, caller_irq_thread, cur_irq_thread, cur_irq_thread->state, 0);
            }
        }
    }

    __vsf_arch_bg_trace(VSF_ARCH_TRACE_PREEMPT_LEAVE, caller_irq_thread, cur_irq_thread, 0, 0);
    __vsf_arch_unlock();
}

#if VSF_ARCH_CFG_NO_UNLOCKED_SUSPEND_EN == ENABLED
// __vsf_arch_irq_pend MUST be called with arch_unlocked
static void __vsf_arch_irq_pend(vsf_arch_irq_thread_t *irq_thread)
{
    while (1) {
        if (VSF_ARCH_IRQ_STATE_FOREGROUND == irq_thread->state) {
            break;
        }

        __vsf_arch_unlock();
            vsf_arch_yield();
        __vsf_arch_lock();
    }
}
#endif

static void __vsf_arch_irq_activate(vsf_arch_irq_thread_t *irq_thread)
{
    __vsf_arch_lock();
    if (!vsf_dlist_is_in(vsf_arch_irq_thread_t, rdy_node,
            &__vsf_x86.irq_rdy_list, irq_thread)) {

        vsf_dlist_insert(vsf_arch_irq_thread_t, rdy_node,
            &__vsf_x86.irq_rdy_list, irq_thread,
            _->priority < irq_thread->priority);

        vsf_arch_trace_irq("%s activated\r\n", irq_thread->name);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_ACTIVATED, NULL, irq_thread, 0, 0);

        bool force = irq_thread == __vsf_arch_get_cur_irq_thread();
        while (1) {
            if (VSF_ARCH_IRQ_STATE_FOREGROUND == irq_thread->state) {
                break;
            }

            if (__vsf_arch_can_preempt(irq_thread, false)) {
                __vsf_arch_preempt(VSF_ARCH_IRQ_PP_NORMAL);
                if (irq_thread->reply != NULL) {
                    __vsf_arch_irq_request_send(irq_thread->reply);
                    irq_thread->reply = NULL;
                }
                return;
            }
            if (!force) {
                break;
            } else {
                if (irq_thread->reply != NULL) {
                    __vsf_arch_irq_request_send(irq_thread->reply);
                    irq_thread->reply = NULL;
                }
                __vsf_arch_unlock();
                    vsf_arch_yield();
                __vsf_arch_lock();
            }
        }
    }
    __vsf_arch_unlock();
}

static void __vsf_arch_irq_deactivate(vsf_arch_irq_thread_t *irq_thread, bool is_terminate)
{
    __vsf_arch_lock();
    if (irq_thread != &__vsf_x86.por_thread) {
        vsf_dlist_remove(vsf_arch_irq_thread_t, rdy_node,
            &__vsf_x86.irq_rdy_list, irq_thread);
        vsf_arch_trace_irq("%s deactivated\r\n", irq_thread->name);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_DEACTIVATED, NULL, irq_thread, 0, 0);
    }

    __vsf_arch_preempt(is_terminate ? VSF_ARCH_IRQ_PP_EXIT_SUSPEND : VSF_ARCH_IRQ_PP_EXIT_RUNNING);
}

static void __vsf_arch_irq_set_priority(vsf_arch_irq_thread_t *irq_thread, vsf_arch_prio_t priority)
{
    bool is_in;
    __vsf_arch_lock();
        is_in = vsf_dlist_is_in(vsf_arch_irq_thread_t, rdy_node,
                &__vsf_x86.irq_rdy_list, irq_thread);
        if (is_in) {
            vsf_dlist_remove(vsf_arch_irq_thread_t, rdy_node,
                &__vsf_x86.irq_rdy_list, irq_thread);
        }
        irq_thread->priority = priority;
        if (is_in) {
            vsf_dlist_insert(vsf_arch_irq_thread_t, rdy_node,
                &__vsf_x86.irq_rdy_list, irq_thread,
                _->priority < irq_thread->priority);
        }
    __vsf_arch_unlock();
}

void __vsf_arch_irq_start(vsf_arch_irq_thread_t *irq_thread)
{
    vsf_arch_trace_function("%s(irq_thread: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, irq_thread);

    __vsf_arch_irq_activate(irq_thread);

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_end(vsf_arch_irq_thread_t *irq_thread, bool is_terminate)
{
    vsf_arch_trace_function("%s(irq_thread: 0x" VSF_TRACE_POINTER_HEX ", is_terminate %s)" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, irq_thread, is_terminate ? "true" : "false");

    __vsf_arch_irq_deactivate(irq_thread, is_terminate);

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_fini(vsf_arch_irq_thread_t *irq_thread)
{
    vsf_arch_trace_function("%s(irq_thread: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, irq_thread);
    __vsf_arch_lock();
        vsf_dlist_remove(vsf_arch_irq_thread_t, irq_node, &__vsf_x86.irq_list, irq_thread);
    __vsf_arch_unlock();

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_init(vsf_arch_irq_thread_t *irq_thread, char *name,
    vsf_arch_irq_entry_t entry, vsf_arch_prio_t priority)
{
    vsf_arch_trace_function("%s(irq_thread: 0x" VSF_TRACE_POINTER_HEX ", name: %s, entry: 0x" VSF_TRACE_POINTER_HEX ", priority: %d)" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, irq_thread, name, entry, priority);

    VSF_HAL_ASSERT(strlen(name) < sizeof(irq_thread->name) - 1);
    VSF_HAL_ASSERT(NULL == irq_thread->thread);
    strcpy((char *)irq_thread->name, name);

    vsf_dlist_init_node(vsf_arch_irq_thread_t, irq_node, irq_thread);
    irq_thread->priority = priority;
    irq_thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)entry,
                irq_thread, CREATE_SUSPENDED, NULL);
    if (NULL == irq_thread->thread) {
        VSF_HAL_ASSERT(false);
    }
    irq_thread->thread_id = GetThreadId(irq_thread->thread);
    irq_thread->state = VSF_ARCH_IRQ_STATE_IDLE;

    __vsf_arch_lock();
        vsf_dlist_add_to_head(vsf_arch_irq_thread_t, irq_node, &__vsf_x86.irq_list, irq_thread);
    __vsf_arch_unlock();
    ResumeThread(irq_thread->thread);

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_set_background(vsf_arch_irq_thread_t *irq_thread)
{
    vsf_arch_trace_function("%s(irq_thread: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, irq_thread);

    __vsf_arch_lock();
        irq_thread->state = VSF_ARCH_IRQ_STATE_BACKGROUND;
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_IRQ_STATE, NULL, irq_thread, irq_thread->state, 0);
    __vsf_arch_unlock();

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request)
{
    vsf_arch_trace_function("%s(request: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, request);

    VSF_HAL_ASSERT(!request->is_inited);
    request->event = CreateEvent(NULL, false, false, NULL);
    request->is_inited = true;

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    vsf_arch_trace_function("%s(0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, request);

    VSF_HAL_ASSERT(request->is_inited);
    CloseHandle(request->event);
    request->is_inited = false;

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    vsf_arch_trace_function("%s(request: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, request);

    VSF_HAL_ASSERT(request->is_inited);
    WaitForSingleObject(request->event, INFINITE);

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    vsf_arch_trace_function("%s(request: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, request);

    VSF_HAL_ASSERT(request->is_inited);
    SetEvent(request->event);

    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}



/*----------------------------------------------------------------------------*
 * Systimer Timer Implementation                                              *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

static void __vsf_systimer_thread(void *arg)
{
    vsf_arch_systimer_ctx_t *ctx = arg;
    __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t);
    while (1) {
        __vsf_arch_irq_request_pend(&__vsf_x86.systimer.timer_request);
        WaitForSingleObject(ctx->timer, INFINITE);

        __vsf_arch_irq_start(&ctx->use_as__vsf_arch_irq_thread_t);

            vsf_systimer_cnt_t tick = vsf_systimer_get();
            vsf_arch_trace_systimer("systimer triggered: %lld\r\n", tick);
            vsf_systimer_timeout_evt_hanlder(tick);

        __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false);
    }
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_systimer_init(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);

    __vsf_x86.systimer.start_tick = vsf_systimer_get();

    __vsf_x86.systimer.timer = CreateWaitableTimer(NULL, true, NULL);
    VSF_HAL_ASSERT(NULL != __vsf_x86.systimer.timer);

    __vsf_arch_irq_request_init(&__vsf_x86.systimer.timer_request);

    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_systimer_start(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);

    __vsf_arch_irq_init(&__vsf_x86.systimer.use_as__vsf_arch_irq_thread_t,
                "timer", __vsf_systimer_thread, vsf_arch_prio_32);

    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, VSF_ERR_NONE);
    return VSF_ERR_NONE;
}

void vsf_systimer_set_idle(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

vsf_systimer_cnt_t __vsf_systimer_get(void)
{
    LARGE_INTEGER li;
    GetSystemTimeAsFileTime((LPFILETIME)&li);

    return (vsf_systimer_cnt_t)li.QuadPart - __vsf_x86.systimer.start_tick;
}

vsf_systimer_cnt_t vsf_systimer_get(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    vsf_systimer_cnt_t ret = __vsf_systimer_get();
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)ret);
    return ret;
}

bool vsf_systimer_set(vsf_systimer_cnt_t due)
{
    vsf_arch_trace_function("%s(%d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)due);

    LARGE_INTEGER li = {
        .QuadPart = __vsf_x86.systimer.start_tick + due,
    };
    vsf_arch_trace_systimer("systimer update: %lld\r\n", due);
    if (!SetWaitableTimer(__vsf_x86.systimer.timer, &li, 0, NULL, NULL, false)) {
        VSF_HAL_ASSERT(false);
        vsf_arch_trace_function("%s exited with false" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
        return false;
    }
    __vsf_arch_irq_request_send(&__vsf_x86.systimer.timer_request);
    vsf_arch_trace_function("%s exited with true" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    return true;
}

bool vsf_systimer_is_due(vsf_systimer_cnt_t due)
{
    vsf_arch_trace_function("%s(due: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)due);
    bool ret = vsf_systimer_get() >= due;
    vsf_arch_trace_function("%s exited with %s" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ret ? "true" : "false");
    return ret;
}

vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    vsf_arch_trace_function("%s(time_us: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, time_us);
    vsf_systimer_cnt_t ret = (vsf_systimer_cnt_t)(((uint64_t)time_us * VSF_ARCH_SYSTIMER_FREQ) / 1000000UL);
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)ret);
    return ret;
}

vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    vsf_arch_trace_function("%s(time_ms: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, time_ms);
    vsf_systimer_cnt_t ret = (vsf_systimer_cnt_t)(((uint64_t)time_ms * VSF_ARCH_SYSTIMER_FREQ) / 1000UL);
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)ret);
    return ret;
}

uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick)
{
    vsf_arch_trace_function("%s(tick: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)tick);
    uint_fast32_t ret = tick * 1000000ul / VSF_ARCH_SYSTIMER_FREQ;
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ret);
    return ret;
}

uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick)
{
    vsf_arch_trace_function("%s(tick: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (int)tick);
    uint_fast32_t ret = tick * 1000ul / VSF_ARCH_SYSTIMER_FREQ;
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ret);
    return ret;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{
    vsf_arch_trace_function("%s(priority: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, priority);
    __vsf_arch_irq_set_priority(&__vsf_x86.systimer.use_as__vsf_arch_irq_thread_t, priority);
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

#endif


/*----------------------------------------------------------------------------*
 * SWI Implementation                                                         *
 *----------------------------------------------------------------------------*/
static void __vsf_arch_swi_thread(void *arg)
{
    vsf_arch_swi_ctx_t *ctx = arg;
    __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t);
    while (1) {
        __vsf_arch_irq_request_pend(&ctx->request);

        __vsf_arch_irq_activate(&ctx->use_as__vsf_arch_irq_thread_t);

            if (ctx->handler != NULL) {
                ctx->handler(ctx->param);
            }

        __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false);
    }
}

/*! \brief initialise a software interrupt
 *! \param idx the index of the software interrupt
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_arch_swi_init(uint_fast8_t idx, vsf_arch_prio_t priority,
    vsf_swi_handler_t *handler, void *param)
{
    vsf_arch_trace_function("%s(idx: %d, priority: %d, handler: 0x" VSF_TRACE_POINTER_HEX ", param: 0x" VSF_TRACE_POINTER_HEX ")" VSF_TRACE_CFG_LINEEND,
            __FUNCTION__, idx, priority, handler, param);
    if (idx < dimof(__vsf_x86.swi)) {
        vsf_arch_swi_ctx_t *ctx = &__vsf_x86.swi[idx];

        ctx->handler = handler;
        ctx->param = param;
        if (!ctx->inited) {
            char swi_name[7];

            ctx->inited = true;
            sprintf(swi_name, "swi%d", idx);
            __vsf_arch_irq_request_init(&ctx->request);
            __vsf_arch_irq_init(&ctx->use_as__vsf_arch_irq_thread_t, swi_name,
                        __vsf_arch_swi_thread, priority);
        }

        vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, VSF_ERR_NONE);
        return VSF_ERR_NONE;
    }
    VSF_HAL_ASSERT(false);
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, VSF_ERR_INVALID_PARAMETER);
    return VSF_ERR_INVALID_PARAMETER;
}

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
void vsf_arch_swi_trigger(uint_fast8_t idx)
{
    vsf_arch_trace_function("%s(idx: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, idx);

    if (idx < dimof(__vsf_x86.swi)) {
        vsf_arch_swi_ctx_t *ctx = &__vsf_x86.swi[idx];
        vsf_arch_irq_request_t reply = { 0 };
        bool is_to_pend;

        __vsf_arch_lock();
            is_to_pend = (__vsf_x86.cur_thread->priority < ctx->priority);
            if (is_to_pend) {
                if (ctx->reply != NULL) {
                    __vsf_arch_irq_request_send(ctx->reply);
                }

                __vsf_arch_irq_request_init(&reply);
                ctx->reply = &reply;
            }
        __vsf_arch_unlock();

        __vsf_arch_irq_request_send(&__vsf_x86.swi[idx].request);
        if (is_to_pend) {
            __vsf_arch_irq_request_pend(&reply);
        }
        vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
        return;
    }
    VSF_HAL_ASSERT(false);
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    vsf_arch_trace_function("%s(priority: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, priority);

    __vsf_arch_lock();
    vsf_arch_prio_t orig = __vsf_x86.prio_base;
    if (orig != priority) {
        __vsf_x86.prio_base = priority;
        vsf_arch_trace_status("prio_base: %d\r\n", (int)__vsf_x86.prio_base);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_STATUS, __vsf_arch_get_cur_irq_thread(), NULL, 0, 1);
        if (priority < orig) {
            vsf_arch_irq_thread_t *irq_thread = NULL;
            vsf_dlist_queue_peek(vsf_arch_irq_thread_t, rdy_node,
                &__vsf_x86.irq_rdy_list, irq_thread);

            if (__vsf_arch_can_preempt(irq_thread, true)) {
                __vsf_arch_preempt(VSF_ARCH_IRQ_PP_NORMAL);
                vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, orig);
                return orig;
            }
        }
    }
    __vsf_arch_unlock();
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, orig);
    return orig;
}




/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    memset(&__vsf_x86, 0, sizeof(__vsf_x86));

    __vsf_x86.prio_base = vsf_arch_prio_invalid;
    __vsf_x86.gint_state = true;

    InitializeCriticalSection(&__vsf_x86.lock);
    InitializeCriticalSection(&__vsf_x86.trace_lock);

    __vsf_x86.por_thread.thread_id = GetCurrentThreadId();
    __vsf_x86.por_thread.thread = OpenThread(THREAD_ALL_ACCESS, false, __vsf_x86.por_thread.thread_id);
    __vsf_x86.por_thread.priority = vsf_arch_prio_invalid;
    strcpy((char *)__vsf_x86.por_thread.name, "por");
    __vsf_x86.por_thread.state = VSF_ARCH_IRQ_STATE_FOREGROUND;
    __vsf_x86.cur_priority = vsf_arch_prio_invalid;
    __vsf_x86.cur_thread = &__vsf_x86.por_thread;
    vsf_dlist_init(&__vsf_x86.irq_rdy_list);
    vsf_dlist_insert(vsf_arch_irq_thread_t, rdy_node,
        &__vsf_x86.irq_rdy_list, &__vsf_x86.por_thread,
        _->priority < vsf_arch_prio_invalid);
    vsf_dlist_add_to_head(vsf_arch_irq_thread_t, irq_node, &__vsf_x86.irq_list, &__vsf_x86.por_thread);

    return true;
}


vsf_gint_state_t vsf_get_interrupt(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    __vsf_arch_lock();
        vsf_gint_state_t state = __vsf_x86.gint_state;
    __vsf_arch_unlock();
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, state);
    return state;
}

void vsf_set_interrupt(vsf_gint_state_t level)
{
    vsf_arch_trace_function("%s(level: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, level);
    __vsf_arch_lock();
    if (__vsf_x86.gint_state != level) {
        __vsf_x86.gint_state = level;
        vsf_arch_trace_status("gint_state: %d\r\n", __vsf_x86.gint_state);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_STATUS, __vsf_arch_get_cur_irq_thread(), NULL, 0, 0);
        if (level) {
            vsf_arch_irq_thread_t *irq_thread = NULL;
            vsf_dlist_queue_peek(vsf_arch_irq_thread_t, rdy_node,
                &__vsf_x86.irq_rdy_list, irq_thread);

            if (__vsf_arch_can_preempt(irq_thread, true)) {
                __vsf_arch_preempt(VSF_ARCH_IRQ_PP_NORMAL);
                vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
                return;
            }
        }
    }
    __vsf_arch_unlock();
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

vsf_gint_state_t vsf_disable_interrupt(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    __vsf_arch_lock();
    vsf_gint_state_t orig = __vsf_x86.gint_state;
    if (orig != false) {
        __vsf_x86.gint_state = false;
        vsf_arch_trace_status("gint_state: %d\r\n", __vsf_x86.gint_state);
        __vsf_arch_bg_trace(VSF_ARCH_TRACE_STATUS, __vsf_arch_get_cur_irq_thread(), NULL, 0, 0);
    }
    __vsf_arch_unlock();
    vsf_arch_trace_function("%s exited with %d" VSF_TRACE_CFG_LINEEND, __FUNCTION__, orig);
    return orig;
}

void vsf_enable_interrupt(void)
{
    vsf_arch_trace_function("%s(void)" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
    vsf_set_interrupt(true);
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

/*----------------------------------------------------------------------------*
 * Others: sleep, reset and etc.                                              *
 *----------------------------------------------------------------------------*/
void __vsf_arch_irq_sleep(uint_fast32_t ms)
{
    vsf_arch_trace_function("%s(ms: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ms);
    Sleep(ms);
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

void vsf_arch_sleep(uint_fast32_t mode)
{
    vsf_arch_trace_function("%s(mode: %d)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, mode);
    if (GetCurrentThreadId() == __vsf_x86.por_thread.thread_id) {
        __vsf_arch_irq_end(&__vsf_x86.por_thread, true);
    } else {
        // TODO: what will happen if call sleep in interrupt?
        VSF_HAL_ASSERT(false);
    }
    vsf_arch_trace_function("%s exited" VSF_TRACE_CFG_LINEEND, __FUNCTION__);
}

#endif
/* EOF */
