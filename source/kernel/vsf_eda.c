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

#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED

#define __VSF_EDA_CLASS_IMPLEMENT
#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_os.h"
#include "./vsf_timq.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_local_t {
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    struct {
        vsf_evtq_t          *cur;
    } evtq;
#else
    vsf_evtq_ctx_t          cur;
#endif

#if     (VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED)                           \
    ||  (VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED && VSF_SYNC_CFG_SUPPORT_ISR == ENABLED)
#   define __VSF_KERNEL_TASK
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED && VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    vsf_teda_t              task;
#   else
    vsf_eda_t               task;
#   endif
    vsf_prio_t              highest_prio;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    struct {
#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
        vsf_timer_queue_t   callback_timq;
        vsf_timer_queue_t   callback_timq_done;
#   endif

        vsf_timer_queue_t   timq;
#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
#       ifdef VSF_SYSTIMER_CFG_IMPL_MODE
        vsf_systimer_tick_t pre_tick;
#       endif
        // TODO: use vsf_atomic_t instead
//        vsf_atomic_t        processing;
        bool                processing;
        vsf_arch_prio_t     arch_prio;
#   else
        vsf_systimer_tick_t cur_tick;
#   endif

#   if VSF_KERNEL_CFG_TRACE == ENABLED
        bool                is_isr_info_sent;
#   endif
    } timer;
#endif
} vsf_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_local_t __vsf_eda;

/*============================ PROTOTYPES ====================================*/

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
extern vsf_evtq_t * __vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_prio_t __vsf_os_evtq_get_priority(vsf_evtq_t *pthis);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
SECTION(".text.vsf.kernel.eda_task")
extern void __vsf_eda_task_evthandler_process_return_value(vsf_eda_t *eda, vsf_evt_t evt);
#endif

SECTION(".text.vsf.kernel.__vsf_eda_get_valid_eda")
static vsf_eda_t * __vsf_eda_get_valid_eda(vsf_eda_t *pthis);

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION(".text.vsf.kernel.eda")
static vsf_err_t __vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force);
#endif

//! should be provided by user
SECTION(".text.vsf.kernel.vsf_eda_new_frame")
extern __vsf_eda_frame_t * vsf_eda_new_frame(size_t local_size);
SECTION(".text.vsf.kernel.vsf_eda_free_frame")
extern void vsf_eda_free_frame(__vsf_eda_frame_t *frame);

extern void vsf_kernel_err_report(enum vsf_kernel_error_t err);

SECTION(".text.vsf.kernel.__vsf_evtq_get_cur_ctx")
static vsf_evtq_ctx_t * __vsf_evtq_get_cur_ctx(void);

/*============================ INCLUDES ======================================*/
#define __EDA_GADGET__
#include "./__eda/vsf_eda_sync.c"
#include "./__eda/vsf_eda_bmpevt.c"
#include "./__eda/vsf_eda_queue.c"
#include "./__eda/vsf_eda_timer.c"
#undef __EDA_GADGET__

/*============================ IMPLEMENTATION ================================*/

SECTION(".text.vsf.kernel.eda")
void __vsf_eda_on_terminate(vsf_eda_t *pthis)
{
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_fini(pthis);
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    if (pthis->on_terminate != NULL) {
        pthis->on_terminate(pthis);
    }
#endif
}

SECTION(".text.vsf.kernel.eda")
void vsf_kernel_init( const vsf_kernel_cfg_t *cfg_ptr)
{
    VSF_KERNEL_ASSERT(NULL != cfg_ptr);

    memset(&__vsf_eda, 0, sizeof(__vsf_eda));

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
    __vsf_eda.timer.arch_prio = cfg_ptr->systimer_arch_prio;
#   endif

#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    __vsf_eda.highest_prio = cfg_ptr->highest_prio;
#   endif

#endif

}

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * __vsf_eda_pop_frame(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_stack_pop(    __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void __vsf_eda_push_frame(vsf_slist_t *list, __vsf_eda_frame_t *frame)
{
    vsf_slist_stack_push(   __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * __vsf_eda_peek_frame(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_peek_next(    __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list, frame);
    return frame;
}
#endif


SECTION(".text.vsf.kernel.__vsf_eda_get_valid_eda")
static vsf_eda_t * __vsf_eda_get_valid_eda(vsf_eda_t *pthis)
{
    if (NULL == pthis) {
        return vsf_eda_get_cur();
    }
    return pthis;
}

SECTION(".text.vsf.kernel.eda")
void __vsf_dispatch_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_evt_begin(pthis, evt);
#endif

#if     VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT == ENABLED                   \
    ||  VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    vsf_protect_t origlevel = vsf_protect_int();

#   if VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT == ENABLED
        pthis->is_evt_incoming = false;
#   endif

#   if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
        vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
#       if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
        vsf_evtq_ctx_t *evtq_ctx = __vsf_evtq_get_cur_ctx();
        evtq_ctx->is_timing = true;
        evtq_ctx->preempted_ticks = 0;
#       endif
#   endif

    vsf_unprotect_int(origlevel);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (pthis->flag.feature.is_use_frame) {
        __vsf_eda_frame_t *frame = pthis->fn.frame;
        VSF_KERNEL_ASSERT(frame != NULL);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TASK == ENABLED && VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        if (pthis->flag.feature.is_subcall_has_return_value) {
            __vsf_eda_task_evthandler_process_return_value(pthis, evt);
        } else
#   endif
        if (    ((uintptr_t)NULL == frame->ptr.target)     //!< no param
            &&  (0 == frame->state.local_size)) {       //!< no local
            VSF_KERNEL_ASSERT(frame->fn.evthandler != NULL);
            frame->fn.evthandler(pthis, evt);        //!< this is a pure eda
        } else {
            VSF_KERNEL_ASSERT(frame->fn.param_evthandler != NULL);
            //frame->fn.param_evthandler(frame->ptr.target, evt);
            frame->fn.param_evthandler((uintptr_t)(frame + 1), evt);
        }
    } else {
        VSF_KERNEL_ASSERT(pthis->fn.evthandler != NULL);
        pthis->fn.evthandler(pthis, evt);
    }
#else
    VSF_KERNEL_ASSERT(pthis->fn.evthandler != NULL);
    pthis->fn.evthandler(pthis, evt);
#endif

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    origlevel = vsf_protect_int();
        if (pthis->usage.ctx != NULL) {
            pthis->usage.ctx->ticks += vsf_systimer_get_elapsed(start_tick);
#   if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
            pthis->usage.ctx->ticks -= evtq_ctx->preempted_ticks;
#   endif
        }
#   if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
        evtq_ctx->is_timing = false;
#   endif
    vsf_unprotect_int(origlevel);
#endif

#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_evt_end(pthis, evt);
#endif
}

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
SECTION(".text.vsf.kernel.__vsf_get_cur_evtq")
vsf_evtq_t * __vsf_get_cur_evtq(void)
{
    return __vsf_eda.evtq.cur;
}

SECTION(".text.vsf.kernel.__vsf_set_cur_evtq")
vsf_evtq_t * __vsf_set_cur_evtq(vsf_evtq_t *evtq)
{
    vsf_evtq_t *evtq_old = __vsf_get_cur_evtq();
    __vsf_eda.evtq.cur = evtq;
    return evtq_old;
}

SECTION(".text.vsf.kernel.__vsf_evtq_get_cur_ctx")
static vsf_evtq_ctx_t * __vsf_evtq_get_cur_ctx(void)
{
    vsf_evtq_t *evtq = __vsf_get_cur_evtq();
    if (evtq != NULL) {
        return &evtq->cur;
    } else {
        return NULL;
    }
}
#else
SECTION(".text.vsf.kernel.__vsf_evtq_post_do")
static void __vsf_evtq_post_do(vsf_eda_t *pthis, uintptr_t value)
{
    vsf_evtq_ctx_t ctx_old = __vsf_eda.cur;

    __vsf_eda.cur.eda = pthis;
    if (value & 1) {
        __vsf_eda.cur.evt = (vsf_evt_t)(value >> 1);
        __vsf_eda.cur.msg = (uintptr_t)NULL;
    } else {
        __vsf_eda.cur.evt = VSF_EVT_MESSAGE;
        __vsf_eda.cur.msg = value;
    }
    __vsf_dispatch_evt(pthis, __vsf_eda.cur.evt);
    __vsf_eda.cur = ctx_old;
}

SECTION(".text.vsf.kernel.__vsf_evtq_post")
static vsf_err_t __vsf_evtq_post(vsf_eda_t *pthis, uintptr_t value)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    if (pthis->flag.state.is_processing) {
        if (
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
                !pthis->flag.state.is_limitted
            &&
#endif
                (VSF_EVT_INVALID == pthis->evt_pending)) {
            pthis->evt_pending = value;
            return VSF_ERR_NONE;
        }
    } else {
        pthis->flag.state.is_processing = true;
        __vsf_evtq_post_do(pthis, value);

        while (pthis->evt_pending != VSF_EVT_INVALID) {
            value = pthis->evt_pending;
            pthis->evt_pending = VSF_EVT_INVALID;
            __vsf_evtq_post_do(pthis, value);
        }
        pthis->flag.state.is_processing = false;
        return VSF_ERR_NONE;
    }
    VSF_KERNEL_ASSERT(false);
    return VSF_ERR_FAIL;
}

void vsf_evtq_on_eda_init(vsf_eda_t *pthis)
{
    pthis->evt_pending = VSF_EVT_INVALID;
}

void vsf_evtq_on_eda_fini(vsf_eda_t *pthis)
{
    __vsf_eda_on_terminate(pthis);
}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt_ex")
vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
    return __vsf_evtq_post(pthis, (uintptr_t)((evt << 1) | 1));
}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt")
vsf_err_t vsf_evtq_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(pthis, evt, false);
}

SECTION(".text.vsf.kernel.vsf_evtq_post_msg")
vsf_err_t vsf_evtq_post_msg(vsf_eda_t *pthis, void *msg)
{
    return __vsf_evtq_post(pthis, (uintptr_t)msg);
}

SECTION(".text.vsf.kernel.__vsf_evtq_get_cur_ctx")
static vsf_evtq_ctx_t * __vsf_evtq_get_cur_ctx(void)
{
    return &__vsf_eda.cur;
}
#endif

#ifndef WEAK_VSF_KERNEL_ERR_REPORT
WEAK(vsf_kernel_err_report)
void vsf_kernel_err_report(enum vsf_kernel_error_t err)
{
    VSF_UNUSED_PARAM(err);
    VSF_KERNEL_ASSERT(false);
}
#endif

SECTION(".text.vsf.kernel.eda")
vsf_eda_t * vsf_eda_get_cur(void)
{
    vsf_evtq_ctx_t *ctx_cur = __vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->eda;
    } else {
        vsf_kernel_err_report(VSF_KERNEL_ERR_NULL_EDA_PTR);
        return NULL;
    }
}

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED

SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
bool vsf_eda_polling_state_get(vsf_eda_t *pthis)
{
    VSF_KERNEL_ASSERT( NULL != pthis );
    return pthis->flag.feature.user_bits;
}

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
void vsf_eda_polling_state_set(vsf_eda_t *pthis, bool state)
{
    VSF_KERNEL_ASSERT( NULL != pthis );
#if VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT == ENABLED
    if (state) {
        vsf_protect_t origlevel = vsf_protect_int();
            pthis->is_evt_incoming = true;
        vsf_unprotect_int(origlevel);
    }
#endif
    pthis->flag.feature.user_bits = state ? 1 : 0;
}

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
bool vsf_eda_is_stack_owner(vsf_eda_t *pthis)
{
    VSF_KERNEL_ASSERT(NULL != pthis);
    return pthis->flag.feature.is_stack_owner;
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
vsf_evt_t vsf_eda_get_cur_evt(void)
{
    vsf_evtq_ctx_t *ctx_cur = __vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->evt;
    }
    return VSF_EVT_INVALID;
}

SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
void * vsf_eda_get_cur_msg(void)
{
    vsf_evtq_ctx_t *ctx_cur = __vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return (void *)ctx_cur->msg;
    } else {
        return NULL;
    }
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_get_return_value")
uintptr_t vsf_eda_get_return_value(void)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    return pthis->return_value;
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_return")
bool __vsf_eda_return(uintptr_t return_value)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    __vsf_eda_frame_t *frame = NULL;
    pthis->return_value = return_value;
    if (pthis->flag.feature.is_use_frame) {
        frame = __vsf_eda_pop_frame(&pthis->fn.frame_list);
        VSF_KERNEL_ASSERT(frame != NULL);
        pthis->flag.feature = frame->state.feature;
        vsf_eda_free_frame(frame);
        if (pthis->flag.feature.is_use_frame) {
            frame = pthis->fn.frame;
        } else {
            goto do_return;
        }
    }

    if (frame != NULL) {
        /*! \note automatically free a frame if the top eda doesn't set
         *!       is_use_frame
         */
        {
            __vsf_eda_frame_t *frame_caller = __vsf_eda_peek_frame((vsf_slist_t *)frame);
            if (    (NULL == frame_caller)                      //!< top frame
                &&  !frame->state.feature.is_use_frame) {       //!< not force frame
                pthis->flag.feature = frame->state.feature;
                vsf_eda_free_frame(frame);
            }
        }

do_return:
#   if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
        if (pthis->flag.feature.is_stack_owner) {
#       if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
            extern void __vsf_eda_return_to_thread(vsf_eda_t *eda);
            __vsf_eda_return_to_thread(pthis);
#       else
            VSF_KERNEL_ASSERT(false);
#       endif
        } else {
            __vsf_dispatch_evt(pthis, VSF_EVT_RETURN);
        }
#   else
        vsf_eda_post_evt(pthis, VSF_EVT_RETURN);
#   endif
        return false;
    }
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
    vsf_eda_fini(pthis);
    return true;
}

SECTION(".text.vsf.kernel.__vsf_eda_yield")
void __vsf_eda_yield(void)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    vsf_eda_post_evt(pthis, VSF_EVT_YIELD);
}

SECTION(".text.vsf.kernel.vsf_eda_set_user_value")
void vsf_eda_set_user_value(uint8_t value)
{
    vsf_eda_t* pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    pthis->flag.feature.user_bits = value;
}

SECTION(".text.vsf.kernel.vsf_eda_get_user_value")
uint8_t vsf_eda_get_user_value(void)
{
    vsf_eda_t* pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    return pthis->flag.feature.user_bits;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

SECTION(".text.vsf.kernel.eda_nesting")
static vsf_err_t __vsf_eda_ensure_frame_used(vsf_eda_t *pthis, uintptr_t param)
{
    if (!pthis->flag.feature.is_use_frame) {
        __vsf_eda_frame_t *frame_tmp = vsf_eda_new_frame(0);
        if (NULL == frame_tmp) {
            //vsf_eda_free_frame(frame);
            //VSF_KERNEL_ASSERT(false);                    //!< this error is allowed, the sub call can be tried later.
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        frame_tmp->state.feature = pthis->flag.feature;
        pthis->flag.feature.is_use_frame = true;
        frame_tmp->fn.evthandler = pthis->fn.evthandler;
        frame_tmp->ptr.target = param;
        vsf_slist_init(&pthis->fn.frame_list);
        __vsf_eda_push_frame(&pthis->fn.frame_list, frame_tmp);
    }

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.__vsf_eda_get_local")
uintptr_t __vsf_eda_get_local(vsf_eda_t* pthis)
{
    VSF_KERNEL_ASSERT(NULL != pthis);

    if (    (!pthis->flag.feature.is_use_frame)
        ||  (0 == pthis->fn.frame->state.local_size)) {
        return (uintptr_t)NULL;
    }
    return (uintptr_t)(pthis->fn.frame+1);// - sizeof(uintptr_t);
}

SECTION(".text.vsf.kernel.vsf_eda_target_set")
vsf_err_t vsf_eda_target_set(uintptr_t param)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    vsf_err_t err = __vsf_eda_ensure_frame_used(pthis, param);
    if (VSF_ERR_NONE == err) {
        pthis->fn.frame->ptr.target = param;
    }

    return err;
}

SECTION(".text.vsf.kernel.vsf_eda_target_get")
uintptr_t vsf_eda_target_get(void)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    if (pthis->flag.feature.is_use_frame) {
        return pthis->fn.frame->ptr.target;
    }
    return (uintptr_t)NULL;
}


#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

SECTION(".text.vsf.kernel.eda_nesting")
vsf_err_t __vsf_eda_call_eda_ex_prepare(
                                uintptr_t func,
                                uintptr_t param,
                                __vsf_eda_frame_state_t state,
                                bool is_sub_call)
{
    VSF_KERNEL_ASSERT((uintptr_t)NULL != func);

    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t *pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    __vsf_eda_frame_t *frame = NULL;
    if (is_sub_call) {
        frame = vsf_eda_new_frame(state.local_size);
        if (NULL == frame) {
            VSF_KERNEL_ASSERT(false);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        __vsf_eda_push_frame(&pthis->fn.frame_list, frame);
    } else {
        err = __vsf_eda_ensure_frame_used(pthis, 0);
        if (VSF_ERR_NONE != err) {
            return err;
        }
        frame = pthis->fn.frame;
    }

    frame->fn.func = func;
    frame->state.feature = pthis->flag.feature;
    frame->state.local_size = state.local_size;
    pthis->flag.feature = state.feature;
    pthis->flag.feature.is_use_frame = true;

    /*! \note please NEVER do following things. If param is NULL, please let
     *        frame->param to be NULL
     *
        if (NULL == param) {
            frame->param = pthis;           //!< point to the current eda
        } else {
            frame->param = param;
        }
     */
    frame->ptr.param = param;

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
vsf_err_t __vsf_eda_call_eda_prepare(   uintptr_t evthandler,
                                        uintptr_t param,
                                        size_t local_size)
{
    __vsf_eda_frame_state_t state       = {
        .local_size                     = local_size,
    };
    return __vsf_eda_call_eda_ex_prepare(evthandler, param, state, true);
}

SECTION(".text.vsf.kernel.eda_nesting")
vsf_err_t __vsf_eda_call_eda_ex(uintptr_t func,
                                uintptr_t param,
                                __vsf_eda_frame_state_t state,
                                bool is_sub_call)
{
    vsf_err_t err = __vsf_eda_call_eda_ex_prepare(func, param, state, is_sub_call);
    if (VSF_ERR_NONE == err) {
        vsf_eda_t *eda = vsf_eda_get_cur();
#if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED
        __vsf_dispatch_evt(eda, VSF_EVT_INIT);
#else
        if (VSF_ERR_NONE != vsf_eda_post_evt(eda, VSF_EVT_INIT)) {
            VSF_KERNEL_ASSERT(false);
        }
#endif
    }
    return err;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


SECTION(".text.vsf.kernel.__vsf_eda_go_to_ex")
vsf_err_t __vsf_eda_go_to_ex(uintptr_t evthandler, uintptr_t param)
{
    __vsf_eda_frame_state_t state = { 0 };
    return __vsf_eda_call_eda_ex(evthandler, param, state, false);
}

SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
vsf_err_t __vsf_eda_call_eda(   uintptr_t evthandler,
                                uintptr_t param,
                                size_t local_size)
{
    __vsf_eda_frame_state_t state   = {
        .local_size             = local_size,
    };
    return __vsf_eda_call_eda_ex(evthandler, param, state, true);
}

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL



SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *pthis, vsf_eda_evthandler_t evthandler)
{
    VSF_KERNEL_ASSERT(NULL != pthis && NULL != evthandler);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (pthis->flag.feature.is_use_frame) {
        //VSF_KERNEL_ASSERT(false);
        pthis->fn.frame->fn.evthandler = evthandler;
        //return VSF_ERR_NOT_ACCESSABLE;
    } else
#endif
    {
        pthis->fn.evthandler = evthandler;
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
vsf_err_t vsf_eda_go_to(uintptr_t evthandler)
{
    vsf_err_t err;
    vsf_eda_t *pthis = vsf_eda_get_cur();

    err = vsf_eda_set_evthandler(pthis, (vsf_eda_evthandler_t) evthandler);

    if (VSF_ERR_NONE == err) {
        err = vsf_eda_post_evt(pthis, VSF_EVT_INIT);
        VSF_KERNEL_ASSERT((VSF_ERR_NONE == err));
    }

    return err;
}

static void __vsf_eda_init_member(
                            vsf_eda_t *pthis,
                            vsf_prio_t priority,
                            vsf_eda_feature_t feature)
{
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    if (priority == vsf_prio_inherit) {
        vsf_evtq_t *evtq = __vsf_get_cur_evtq();
        if (NULL == evtq) {
            vsf_kernel_err_report(
                VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR);
        }
        pthis->priority = __vsf_os_evtq_get_priority(evtq);
    } else {
        pthis->priority = priority;
    }
#endif
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    vsf_dlist_init_node(vsf_eda_t, pending_node, pthis);
#endif

    pthis->flag.value = 0;
    pthis->flag.feature = feature;
#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
    pthis->subcall_return_value = fsm_rt_on_going;
#endif

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    pthis->usage.ticks = 0;
    pthis->usage.ctx = NULL;
#endif

    vsf_evtq_on_eda_init(pthis);

#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(pthis, NULL, NULL, 0);
    vsf_kernel_trace_eda_init(pthis);
#endif
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_init(   vsf_eda_t *pthis,
                            vsf_prio_t priority,
                            vsf_eda_feature_t feature)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    __vsf_eda_init_member(pthis, priority, feature);

    return vsf_eda_post_evt(pthis, VSF_EVT_INIT);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

SECTION(".text.vsf.kernel.vsf_eda_start")
vsf_err_t vsf_eda_start(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg_ptr)
{
    VSF_KERNEL_ASSERT(  NULL != pthis
                    &&  NULL != cfg_ptr
                    &&  (uintptr_t)NULL != cfg_ptr->fn.func);

    pthis->fn.evthandler = cfg_ptr->fn.evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    pthis->on_terminate = cfg_ptr->on_terminate;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (    cfg_ptr->target
#   if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
        ||  cfg_ptr->feature.is_subcall_has_return_value
#   endif
    ) {
        //! override the is_use_frame flag
        cfg_ptr->feature.is_use_frame = true;
    }

    if (!cfg_ptr->feature.is_use_frame) {
        return vsf_eda_init(pthis, cfg_ptr->priority, cfg_ptr->feature);
    }

    __vsf_eda_frame_t *frame = vsf_eda_new_frame(cfg_ptr->local_size);
    if (NULL == frame) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    //frame->flag = 0;                  //!< clear all flags (Done with memset)

    frame->fn.func = cfg_ptr->fn.func;
    frame->ptr.target = cfg_ptr->target;
    frame->state.feature = cfg_ptr->feature;
#endif

    __vsf_eda_init_member(pthis, cfg_ptr->priority, cfg_ptr->feature);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    pthis->fn.frame = frame;
#endif
    return vsf_eda_post_evt(pthis, VSF_EVT_INIT);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

/* vsf_eda_fini() enables you to kill other eda tasks.
   We highly recommend that DO NOT use this api until you 100% sure.
   please make sure that the resources are properly freed when you trying to kill
   an eda other than your own. We highly recommend that please send a semaphore to
   the target eda to ask it killing itself after properly freeing all the resources.
 */
SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_fini(vsf_eda_t *pthis)
{
    pthis = (vsf_eda_t *)__vsf_eda_get_valid_eda(pthis);
    VSF_KERNEL_ASSERT(pthis != NULL);
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_teda_cancel_timer((vsf_teda_t *)pthis);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    __vsf_eda_frame_t *frame = NULL;
    while (pthis->flag.feature.is_use_frame) {
        frame = __vsf_eda_pop_frame(&pthis->fn.frame_list);
        if (NULL == frame) {
            break;
        }
        pthis->flag.feature = frame->state.feature;
        vsf_eda_free_frame(frame);
    }
#endif

    vsf_evtq_on_eda_fini(pthis);
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif


SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
#if VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT == ENABLED
    vsf_protect_t origlevel = vsf_protect_int();
    if (pthis->is_evt_incoming && evt == VSF_EVT_YIELD) {
        vsf_unprotect_int(origlevel);
        return VSF_ERR_NONE;
    }
    pthis->is_evt_incoming = true;
    vsf_unprotect_int(origlevel);
#endif

    return vsf_evtq_post_evt(pthis, evt);
}

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION(".text.vsf.kernel.eda")
static vsf_err_t __vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_evtq_post_evt_ex(pthis, evt, force);
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && !((uintptr_t)msg & 1));
    return vsf_evtq_post_msg(pthis, msg);
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg)
{
    return vsf_evtq_post_evt_msg(pthis, evt, msg);
}
#endif

#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
void vsf_eda_cpu_usage_start(vsf_eda_t *pthis, vsf_cpu_usage_ctx_t *ctx)
{
    pthis = __vsf_eda_get_valid_eda(pthis);

    vsf_protect_t origlevel = vsf_protect_int();
        ctx->ticks = 0;
        ctx->duration = vsf_systimer_get_tick();

        VSF_KERNEL_ASSERT(NULL != pthis->usage.ctx);
        pthis->usage.ctx = ctx;
    vsf_unprotect_int(origlevel);
}

void vsf_eda_cpu_usage_stop(vsf_eda_t *pthis)
{
    pthis = __vsf_eda_get_valid_eda(pthis);

    vsf_protect_t origlevel = vsf_protect_int();
        vsf_cpu_usage_ctx_t  *ctx = pthis->usage.ctx;
        VSF_KERNEL_ASSERT(NULL != ctx);
        pthis->usage.ctx = NULL;

        ctx->duration = vsf_systimer_get_elapsed(ctx->duration);
    vsf_unprotect_int(origlevel);
}
#endif

#ifdef __VSF_KERNEL_TASK

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

static void __vsf_kernel_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    vsf_teda_t *teda;
    vsf_protect_t origlevel;
#endif

    VSF_KERNEL_ASSERT(eda != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        break;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    case VSF_EVT_TIMER:
        origlevel = vsf_protect_sched();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        while ((teda != NULL) && __vsf_systimer_is_due(teda->due)) {
            vsf_timq_dequeue(&__vsf_eda.timer.timq, teda);

            teda->use_as__vsf_eda_t.flag.state.is_timed = false;
            vsf_unprotect_sched(origlevel);

#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
            if (teda == &__vsf_eda.task) {
                vsf_callback_timer_t *timer;
                vsf_dlist_t *done_queue = &__vsf_eda.timer.callback_timq_done;

                origlevel = __vsf_callback_timer_protect();
                vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                while ((timer != NULL) && __vsf_systimer_is_due(timer->due)) {
                    vsf_callback_timq_dequeue(&__vsf_eda.timer.callback_timq, timer);
                    __vsf_callback_timer_unprotect(origlevel);

                    vsf_callback_timq_enqueue(done_queue, timer);

                    origlevel = __vsf_callback_timer_protect();
                    vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                }

                if (timer != NULL) {
                    __vsf_teda_timer_enqueue((vsf_teda_t *)eda, timer->due);
                }
                __vsf_callback_timer_unprotect(origlevel);
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_KERNEL_EVT_CALLBACK_TIMER);
            } else
#   endif
            {
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_EVT_TIMER);
            }

            origlevel = vsf_protect_sched();
            vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        }
#if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
        __vsf_eda.timer.processing = false;
        __vsf_systimer_update(true);
#endif
        vsf_unprotect_sched(origlevel);
        break;
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER

#if     VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED                                  \
    &&  VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
    case VSF_EVT_MESSAGE: {
            vsf_sync_t *sync = vsf_eda_get_cur_msg();
            VSF_KERNEL_ASSERT(sync != NULL);
            vsf_eda_sync_increase(sync);
        }
        break;
#endif
#if     VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED                             \
    &&  VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
    case VSF_KERNEL_EVT_CALLBACK_TIMER: {
            vsf_callback_timer_t *timer;
            vsf_dlist_t *done_queue = &__vsf_eda.timer.callback_timq_done;

            vsf_callback_timq_dequeue(done_queue, timer);
            while (timer != NULL) {
                timer->due = 0;
                if (timer->on_timer != NULL) {
                    timer->on_timer(timer);
                }
                vsf_callback_timq_dequeue(done_queue, timer);
            }
        }
        break;
#   if VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR == ENABLED
    case VSF_KERNEL_EVT_CALLBACK_TIMER_ADD: {
            vsf_callback_timer_t *timer = vsf_eda_get_cur_msg();
            VSF_KERNEL_ASSERT(timer != NULL);

            vsf_systimer_tick_t now = vsf_systimer_get_tick();

            if (now >= timer->due) {
                timer->due = 0;
                if (timer->on_timer != NULL) {
                    timer->on_timer(timer);
                }
                break;
            }

            uint_fast32_t tick = now - timer->due;
            timer->due = 0;
            vsf_callback_timer_add(timer, tick);
        }
        break;
#   endif
#endif
#if     VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED                             \
    &&  VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED
    case VSF_KERNEL_EVT_QUEUE_SEND_NOTIFY: {
            vsf_eda_queue_t *queue = vsf_eda_get_cur_msg();
            VSF_KERNEL_ASSERT(queue != NULL);

            SECTION(".text.vsf.kernel.vsf_eda_queue")
            void __vsf_eda_queue_notify(vsf_eda_queue_t *pthis, bool tx, vsf_protect_t orig);

            vsf_protect_t origlevel = vsf_protect_sched();
            __vsf_eda_queue_notify(queue, false, origlevel);
        }
        break;
    case VSF_KERNEL_EVT_QUEUE_RECV_NOTIFY: {
            vsf_eda_queue_t *queue = vsf_eda_get_cur_msg();
            VSF_KERNEL_ASSERT(queue != NULL);

            SECTION(".text.vsf.kernel.vsf_eda_queue")
            void __vsf_eda_queue_notify(vsf_eda_queue_t *pthis, bool tx, vsf_protect_t orig);

            vsf_protect_t origlevel = vsf_protect_sched();
            __vsf_eda_queue_notify(queue, true, origlevel);
        }
        break;
#endif
    }
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#elif __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif

vsf_err_t vsf_kernel_start(void)
{
#ifdef __VSF_KERNEL_TASK
    vsf_err_t err;

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_systimer_init();
#   endif

    __vsf_eda.task.fn.evthandler = __vsf_kernel_evthandler;
    err = vsf_eda_init((vsf_eda_t *)&__vsf_eda.task, __vsf_eda.highest_prio);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (VSF_ERR_NONE == err) {
        __vsf_systimer_start();
    }
#   endif

    // trace will require systimer, call vsf_kernel_trace_init after systimer_start
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_init();
    vsf_kernel_trace_eda_info((vsf_eda_t *)&__vsf_eda.task, "kernel_task", NULL, 0);
#       if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_eda.timer.is_isr_info_sent = false;
#       endif
#   endif
    return err;
#else
#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_init();
#   endif
    return VSF_ERR_NONE;
#endif
}


#endif
