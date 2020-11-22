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
    ||  (VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED)

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED && VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
#       define __VSF_KERNEL_TASK_TEDA
    vsf_teda_t              teda;
#   else
#       define __VSF_KERNEL_TASK_EDA
    vsf_eda_t               eda;
#   endif
    vsf_prio_t              highest_prio;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    struct {
#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
        vsf_timer_queue_t   callback_timq;
        vsf_timer_queue_t   callback_timq_done;
#   endif

        vsf_timer_queue_t   timq;
#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
#       ifdef VSF_SYSTIMER_CFG_IMPL_MODE
        vsf_timer_tick_t    pre_tick;
#       endif
        bool                processing;
        vsf_arch_prio_t     arch_prio;
#   else
        vsf_timer_tick_t    cur_tick;
#   endif
    } timer;
#endif
} vsf_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_local_t __vsf_eda;

/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
extern vsf_err_t __vsf_eda_set_priority(vsf_eda_t *this_ptr, vsf_prio_t priority);
extern vsf_prio_t __vsf_eda_get_cur_priority(vsf_eda_t *this_ptr);
#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
extern vsf_evtq_t * __vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *this_ptr, vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *this_ptr);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *this_ptr);
extern vsf_prio_t __vsf_os_evtq_get_priority(vsf_evtq_t *this_ptr);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
static void __vsf_eda_fsm_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
#endif

SECTION(".text.vsf.kernel.__vsf_eda_get_valid_eda")
static vsf_eda_t * __vsf_eda_get_valid_eda(vsf_eda_t *this_ptr);
SECTION(".text.vsf.kernel.eda")
static vsf_err_t __vsf_eda_post_evt_ex(vsf_eda_t *this_ptr, vsf_evt_t evt, bool force);

//! should be provided by user
SECTION(".text.vsf.kernel.vsf_eda_new_frame")
extern __vsf_eda_frame_t * vsf_eda_new_frame(size_t local_size);
SECTION(".text.vsf.kernel.vsf_eda_free_frame")
extern void vsf_eda_free_frame(__vsf_eda_frame_t *frame);

extern void vsf_kernel_err_report(enum vsf_kernel_error_t err);

/*============================ INCLUDES ======================================*/
#define __EDA_GADGET__
#include "./__eda/vsf_eda_sync.c"
#include "./__eda/vsf_eda_bmpevt.c"
#include "./__eda/vsf_eda_queue.c"
#include "./__eda/vsf_eda_timer.c"
#undef __EDA_GADGET__

/*============================ IMPLEMENTATION ================================*/

SECTION(".text.vsf.kernel.eda")
void vsf_eda_on_terminate(vsf_eda_t *this_ptr)
{
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    if (this_ptr->on_terminate != NULL) {
        this_ptr->on_terminate(this_ptr);
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

#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    __vsf_eda.highest_prio = cfg_ptr->highest_prio;
#   endif

#endif

}

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * __vsf_eda_pop(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_stack_pop(    __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void __vsf_eda_push(vsf_slist_t *list, __vsf_eda_frame_t *frame)
{
    vsf_slist_stack_push(   __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * __vsf_eda_peek(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_peek_next(__vsf_eda_frame_t,
                        use_as__vsf_slist_node_t,
                        list, frame);
    return frame;
}
#endif


SECTION(".text.vsf.kernel.__vsf_eda_get_valid_eda")
static vsf_eda_t * __vsf_eda_get_valid_eda(vsf_eda_t *this_ptr)
{
    if (NULL == this_ptr) {
        return vsf_eda_get_cur();
    }
    return this_ptr;
}

SECTION(".text.vsf.kernel.eda")
void __vsf_dispatch_evt(vsf_eda_t *this_ptr, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);

#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_eda_trace(this_ptr, evt);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED
    this_ptr->state.bits.is_evt_incoming = false;
#   endif

    if (this_ptr->state.bits.is_use_frame) {
        __vsf_eda_frame_t *frame = this_ptr->fn.frame;
        VSF_KERNEL_ASSERT(frame != NULL);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (frame->state.bits.is_fsm) {
            __vsf_eda_fsm_evthandler(this_ptr, evt);
        } else {
            if (    ((uintptr_t)NULL == frame->ptr.target)     //!< no param
                &&  (0 == frame->state.local_size)) {       //!< no local
                frame->fn.evthandler(this_ptr, evt);        //!< this is a pure eda
            } else {
                //frame->fn.param_evthandler(frame->ptr.target, evt);
                frame->fn.param_evthandler((uintptr_t)(frame + 1), evt);
            }
        }
#   else
        if (    ((uintptr_t)NULL == frame->ptr.target)         //!< no param
                &&  (0 == frame->state.local_size)) {       //!< no local
                frame->fn.evthandler(this_ptr, evt);        //!< this is a pure eda
        } else {
            //frame->fn.param_evthandler(frame->ptr.target, evt);
            frame->fn.param_evthandler((uintptr_t)(frame + 1), evt);
        }
#   endif
    } else {
        this_ptr->fn.evthandler(this_ptr, evt);
    }
#else
    this_ptr->fn.evthandler(this_ptr, evt);
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
static void __vsf_evtq_post_do(vsf_eda_t *this_ptr, uintptr_t value)
{
    vsf_evtq_ctx_t ctx_old = __vsf_eda.cur;

    __vsf_eda.cur.eda = this_ptr;
    if (value & 1) {
        __vsf_eda.cur.evt = (vsf_evt_t)(value >> 1);
        __vsf_eda.cur.msg = NULL;
    } else {
        __vsf_eda.cur.evt = VSF_EVT_MESSAGE;
        __vsf_eda.cur.msg = value;
    }
    __vsf_dispatch_evt(this_ptr, __vsf_eda.cur.evt);
    __vsf_eda.cur = ctx_old;
}

SECTION(".text.vsf.kernel.__vsf_evtq_post")
static vsf_err_t __vsf_evtq_post(vsf_eda_t *this_ptr, uintptr_t value)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);

    if (this_ptr->state.bits.is_processing) {
        if (
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
                !this_ptr->state.bits.is_limitted
            &&
#endif
                (VSF_EVT_INVALID == this_ptr->evt_pending)) {
            this_ptr->evt_pending = value;
            return VSF_ERR_NONE;
        }
    } else {
        this_ptr->state.bits.is_processing = true;
        __vsf_evtq_post_do(this_ptr, value);

        while (this_ptr->evt_pending != VSF_EVT_INVALID) {
            value = this_ptr->evt_pending;
            this_ptr->evt_pending = VSF_EVT_INVALID;
            __vsf_evtq_post_do(this_ptr, value);
        }
        this_ptr->state.bits.is_processing = false;
        return VSF_ERR_NONE;
    }
    VSF_KERNEL_ASSERT(false);
    return VSF_ERR_FAIL;
}

void vsf_evtq_on_eda_init(vsf_eda_t *this_ptr)
{
    this_ptr->evt_pending = VSF_EVT_INVALID;
}

void vsf_evtq_on_eda_fini(vsf_eda_t *this_ptr)
{
    vsf_eda_on_terminate(this_ptr);
}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt_ex")
vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *this_ptr, vsf_evt_t evt, bool force)
{
    return __vsf_evtq_post(this_ptr, (uintptr_t)((evt << 1) | 1));
}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt")
vsf_err_t vsf_evtq_post_evt(vsf_eda_t *this_ptr, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(this_ptr, evt, false);
}

SECTION(".text.vsf.kernel.vsf_evtq_post_msg")
vsf_err_t vsf_evtq_post_msg(vsf_eda_t *this_ptr, void *msg)
{
    return __vsf_evtq_post(this_ptr, (uintptr_t)msg);
}

SECTION(".text.vsf.kernel.__vsf_evtq_ctx_t")
static vsf_evtq_ctx_t * __vsf_evtq_get_cur_ctx(void)
{
    return &__vsf_eda.cur;
}
#endif

#ifndef WEAK_VSF_KERNEL_ERR_REPORT
WEAK(vsf_kernel_err_report)
void vsf_kernel_err_report(enum vsf_kernel_error_t err)
{
    UNUSED_PARAM(err);
    ASSERT(false);
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
bool vsf_eda_polling_state_get(vsf_eda_t *this_ptr)
{
    VSF_KERNEL_ASSERT( NULL != this_ptr );
    return this_ptr->state.bits.polling_state;
}

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
void vsf_eda_polling_state_set(vsf_eda_t *this_ptr, bool state)
{
    VSF_KERNEL_ASSERT( NULL != this_ptr );
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED  \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (state) {
        this_ptr->state.bits.is_evt_incoming = true;
    }
#endif
    this_ptr->state.bits.polling_state = state ? 1 : 0;
}

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
bool vsf_eda_is_stack_owner(vsf_eda_t *this_ptr)
{
    VSF_KERNEL_ASSERT(NULL != this_ptr);
    return this_ptr->state.bits.is_stack_owner;
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
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
    return this_ptr->return_value;
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_return")
bool __vsf_eda_return(uintptr_t return_value)
{
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    __vsf_eda_frame_t *frame = NULL;
    this_ptr->return_value = return_value;
    if (this_ptr->state.bits.is_use_frame) {
        frame = __vsf_eda_pop(&this_ptr->fn.frame_list);
        VSF_KERNEL_ASSERT(frame != NULL);
        vsf_eda_free_frame(frame);
        frame = this_ptr->fn.frame;

    #if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        if (NULL != frame) {
            this_ptr->state.bits.is_stack_owner = frame->state.bits.is_stack_owner;
        }
    #endif
    }

    if (frame != NULL) {
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (!frame->state.bits.is_fsm)
#   endif
        {
            __vsf_eda_frame_t *frame_caller = __vsf_eda_peek((vsf_slist_t *)frame);
            if (    (NULL == frame_caller)                  //!< top frame
                &&  ((uintptr_t)NULL == frame->ptr.param)      //!< no param
                &&  (0 == frame->state.local_size)) {       //!< no local

                this_ptr->state.bits.is_use_frame = false;
                this_ptr->fn.evthandler = frame->fn.evthandler;

                vsf_eda_free_frame(frame);
            }
        }

        vsf_eda_post_evt(this_ptr, VSF_EVT_RETURN);
        return false;
    }
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
    vsf_eda_fini(this_ptr);
    return true;
}

SECTION(".text.vsf.kernel.vsf_eda_yield")
void vsf_eda_yield(void)
{
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
    vsf_eda_post_evt(this_ptr, VSF_EVT_YIELD);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

SECTION(".text.vsf.kernel.eda_nesting")
static vsf_err_t __vsf_eda_ensure_frame_used(vsf_eda_t *this_ptr,
                                             uintptr_t param)
{
    if (!this_ptr->state.bits.is_use_frame) {

        __vsf_eda_frame_t *frame_tmp = vsf_eda_new_frame(0);
        if (NULL == frame_tmp) {
            //vsf_eda_free_frame(frame);
            //VSF_KERNEL_ASSERT(false);                    //!< this error is allowed, the sub call can be tried later.
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        this_ptr->state.bits.is_use_frame = true;
        frame_tmp->fn.evthandler = this_ptr->fn.evthandler;

    #if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
        frame_tmp->state.bits.is_stack_owner = this_ptr->state.bits.is_stack_owner;
    #endif
        frame_tmp->ptr.target = param;
        vsf_slist_init(&this_ptr->fn.frame_list);
        __vsf_eda_push(&this_ptr->fn.frame_list, frame_tmp);
    }

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.__vsf_eda_get_local")
uintptr_t __vsf_eda_get_local(vsf_eda_t* this_ptr)
{
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    do {
        if (!this_ptr->state.bits.is_use_frame) {
            break;
        } else if (0 == this_ptr->fn.frame->state.local_size) {
            break;
        }
        return (uintptr_t)(this_ptr->fn.frame+1);// - sizeof(uintptr_t);
    } while(0);

    return (uintptr_t)NULL;
}

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_set")
vsf_err_t vsf_eda_frame_user_value_set(__VSF_KERNEL_CFG_FRAME_UINT_TYPE value)
{
    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t* this_ptr = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    err = __vsf_eda_ensure_frame_used(this_ptr, (uintptr_t)this_ptr);
    if (VSF_ERR_NONE == err) {
        this_ptr->fn.frame->state.bits.user = value;
    }

    return err;
}

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_get")
vsf_err_t vsf_eda_frame_user_value_get(__VSF_KERNEL_CFG_FRAME_UINT_TYPE *pvalue)
{
    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t* this_ptr = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    do {
        if (NULL == pvalue) {
            err = VSF_ERR_INVALID_PTR;
            break;
        } else if (!this_ptr->state.bits.is_use_frame) {
            err = VSF_ERR_NOT_AVAILABLE;
            break;
        }

        (*pvalue) = this_ptr->fn.frame->state.bits.user;

    } while(0);
    return err;
}

SECTION(".text.vsf.kernel.vsf_eda_target_set")
vsf_err_t vsf_eda_target_set(uintptr_t param)
{
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    vsf_err_t err = __vsf_eda_ensure_frame_used(this_ptr, param);
    if (VSF_ERR_NONE == err) {
        this_ptr->fn.frame->ptr.target = param;
    }

    return err;
}

SECTION(".text.vsf.kernel.vsf_eda_target_get")
uintptr_t vsf_eda_target_get(void)
{
    uintptr_t target = (uintptr_t)NULL;
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    if (this_ptr->state.bits.is_use_frame) {
        target = this_ptr->fn.frame->ptr.target;
    }

    return target;
}


#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

SECTION(".text.vsf.kernel.eda_nesting")
vsf_err_t __vsf_eda_call_eda_ex(uintptr_t func,
                                uintptr_t param,
                                __vsf_eda_frame_state_t state,
                                bool is_sub_call)
{
    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t *this_ptr = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != this_ptr);

    __vsf_eda_frame_t *frame = NULL;
    if (is_sub_call) {
        frame = vsf_eda_new_frame(state.local_size);
        if (NULL == frame) {
            VSF_KERNEL_ASSERT(false);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
    }
    VSF_KERNEL_ASSERT((uintptr_t)NULL != func);

    err =  __vsf_eda_ensure_frame_used(this_ptr, 0);
    if (VSF_ERR_NONE != err) {
        /* 1. if is_sub_call is true, frame will not be NULL.
           2. vsf_eda_free_frame can accept NULL as input*/
        vsf_eda_free_frame(frame);
        return err;
    }

    if (!is_sub_call) {
        frame = this_ptr->fn.frame;
    }

    frame->fn.func = func;
#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                               \
    ||  VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
    frame->state.flag = state.flag;
#endif

#   if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
    this_ptr->state.bits.is_stack_owner = state.bits.is_stack_owner;
#   endif


    /*! \note please NEVER do following things. If param is NULL, please let
     *        frame->param to be NULL
     *
        if (NULL == param) {
            frame->param = this_ptr;           //!< point to the current eda
        } else {
            frame->param = param;
        }
     */
    frame->ptr.param = param;

    if (is_sub_call) {
        __vsf_eda_push(&this_ptr->fn.frame_list, frame);
    }

#if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED   //! todo: when the experimental code proved to be stable, remove the old one
        //! for eda without return value, call evthandler directly
    __vsf_dispatch_evt(this_ptr, VSF_EVT_INIT);
#else
    if (VSF_ERR_NONE != vsf_eda_post_evt(this_ptr, VSF_EVT_INIT)) {
        VSF_KERNEL_ASSERT(false);
    }
#endif
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


SECTION(".text.vsf.kernel.__vsf_eda_go_to_ex")
vsf_err_t __vsf_eda_go_to_ex(uintptr_t evthandler, uintptr_t param)
{
    __vsf_eda_frame_state_t state = { .bits.is_fsm = 0,};
    return __vsf_eda_call_eda_ex(evthandler, param, state, false);
}

SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
vsf_err_t __vsf_eda_call_eda(   uintptr_t evthandler,
                                uintptr_t param,
                                size_t local_size)
{
    __vsf_eda_frame_state_t state = { .bits.is_fsm = 0, .local_size = local_size};
    return __vsf_eda_call_eda_ex(evthandler, param, state, true);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
fsm_rt_t __vsf_eda_call_fsm(vsf_fsm_entry_t entry,
                            uintptr_t param,
                            size_t local_size)
{
    vsf_eda_t *this_ptr = vsf_eda_get_cur();

    fsm_rt_t fsm_return_state = this_ptr->fsm_return_state;
    __vsf_eda_frame_state_t state = { .bits.is_fsm = true, .local_size = local_size};
    switch(fsm_return_state) {
        case fsm_rt_on_going:
        case fsm_rt_wait_for_obj:
        //case fsm_rt_asyn:
            break;
        default:
            this_ptr->fsm_return_state = fsm_rt_on_going;
            return fsm_return_state;
    }


    __vsf_eda_call_eda_ex((uintptr_t)entry, param, state, true);

    /*! \note
     *  - if VSF_ERR_NOT_ENOUGH_RESOURCES is detected, yield and try it again
     *  (automatically). For tasks sharing the same frame pool, if the pool is
     *  too small, only task performance will be affected, and all sub-task call
     *  will work when frame is allocated.
     *  - if the frame is allocated and pushed to the stack, we should yield to
     *  let the sub-task run.
     *
     *  Since in either way, we will yield, no need to handle the return
     *  value of vsf_eda_call().
     */
    return fsm_rt_yield;
}

SECTION(".text.vsf.kernel.eda_fsm")
static void __vsf_eda_fsm_evthandler(vsf_eda_t *this_ptr, vsf_evt_t evt)
{
    fsm_rt_t ret;
    VSF_KERNEL_ASSERT(     this_ptr != NULL
            &&  NULL != this_ptr->fn.frame
            &&  NULL != this_ptr->fn.frame->fn.fsm_entry);

    uintptr_t param = this_ptr->fn.frame->ptr.target;
    if  (   ((uintptr_t)NULL == param)                             //!< no param
        &&  (0 == this_ptr->fn.frame->state.local_size)) {      //!< no local
        param = (uintptr_t )this_ptr;                           //!< it is a pure eda
    }

    ret = this_ptr->fn.frame->fn.fsm_entry((uintptr_t)(this_ptr->fn.frame + 1), evt);
    this_ptr->fsm_return_state = ret;
    switch(ret) {
        default:            //! return fsm_rt_err
        case fsm_rt_asyn:   //! call sub fsm later
        case fsm_rt_cpl:
            if (vsf_eda_return()) {
                return ;
            }
            break ;
        case fsm_rt_wait_for_evt:
            //! delay, wait_for, mutex_pend, sem_pend and etc...
            return ;
        case fsm_rt_yield:
            vsf_eda_yield();
            return;
    }
}

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_FSM
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL



SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *this_ptr, vsf_eda_evthandler_t evthandler)
{
    VSF_KERNEL_ASSERT(NULL != this_ptr && NULL != evthandler);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (this_ptr->state.bits.is_use_frame) {
        //VSF_KERNEL_ASSERT(false);
        this_ptr->fn.frame->fn.evthandler = evthandler;
        //return VSF_ERR_NOT_ACCESSABLE;
    } else
#endif
    {
        this_ptr->fn.evthandler = evthandler;
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
vsf_err_t vsf_eda_go_to(uintptr_t evthandler)
{
    vsf_err_t err;
    vsf_eda_t *this_ptr = vsf_eda_get_cur();

    err = vsf_eda_set_evthandler(this_ptr, (vsf_eda_evthandler_t) evthandler);

    if (VSF_ERR_NONE == err) {
        err = vsf_eda_post_evt(this_ptr, VSF_EVT_INIT);
        VSF_KERNEL_ASSERT((VSF_ERR_NONE == err));
    }

    return err;
}

static void __vsf_eda_init( vsf_eda_t *this_ptr,
                            vsf_prio_t priority,
                            bool is_stack_owner)
{
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    if (priority == vsf_prio_inherit) {
        vsf_evtq_t *evtq = __vsf_get_cur_evtq();
        if (NULL == evtq) {
            vsf_kernel_err_report(
                VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR);
        }
        this_ptr->priority = __vsf_os_evtq_get_priority(evtq);
    } else {
        this_ptr->priority = priority;
    }
#endif
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    vsf_dlist_init_node(vsf_eda_t, pending_node, this_ptr);
#endif

    this_ptr->state.flag = 0;

    vsf_evtq_on_eda_init(this_ptr);

#if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
    this_ptr->state.bits.is_stack_owner = is_stack_owner;
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    this_ptr->fsm_return_state = fsm_rt_on_going;
#endif
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_init(vsf_eda_t *this_ptr, vsf_prio_t priority, bool is_stack_owner)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    __vsf_eda_init(this_ptr, priority, is_stack_owner);

    return vsf_eda_post_evt(this_ptr, VSF_EVT_INIT);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

SECTION(".text.vsf.kernel.vsf_eda_start")
vsf_err_t vsf_eda_start(vsf_eda_t *this_ptr, vsf_eda_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(     NULL != this_ptr
            &&  NULL != cfg
            &&  (uintptr_t)NULL != cfg->fn.func);

    this_ptr->fn.evthandler = cfg->fn.evthandler;

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (    ((uintptr_t)NULL == cfg->target)               //!< no param/target
        &&  (0 == cfg->local_size)) {                   //!< no local
        return vsf_eda_init(this_ptr, cfg->priority, cfg->is_stack_owner);
    }

    __vsf_eda_frame_t *frame = vsf_eda_new_frame(cfg->local_size);
    if (NULL == frame) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    //frame->flag = 0;                  //!< clear all flags (Done with memset)

    frame->fn.fsm_entry = cfg->fn.fsm_entry;
    frame->ptr.target = cfg->target;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    frame->state.bits.is_fsm = cfg->is_fsm;        //!< update is_fsm
#   endif
#   if VSF_KERNEL_USE_SIMPLE_SHELL == ENABLED
    frame->state.bits.is_stack_owner = cfg->is_stack_owner;
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    this_ptr->on_terminate = cfg->on_terminate;
#endif


    __vsf_eda_init(this_ptr, cfg->priority, cfg->is_stack_owner);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        this_ptr->state.bits.is_use_frame = true;
        this_ptr->fn.frame = frame;
#endif
    return vsf_eda_post_evt(this_ptr, VSF_EVT_INIT);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
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
vsf_err_t vsf_eda_fini(vsf_eda_t *this_ptr)
{
    this_ptr = (vsf_eda_t *)__vsf_eda_get_valid_eda(this_ptr);
    VSF_KERNEL_ASSERT(this_ptr != NULL);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    __vsf_teda_cancel_timer((vsf_teda_t *)this_ptr);
#endif

    vsf_evtq_on_eda_fini(this_ptr);
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif


SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_post_evt(vsf_eda_t *this_ptr, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (this_ptr->state.bits.is_evt_incoming && evt == VSF_EVT_YIELD) {
        return VSF_ERR_NONE;
    }
    this_ptr->state.bits.is_evt_incoming = true;
#endif

    return vsf_evtq_post_evt(this_ptr, evt);
}

SECTION(".text.vsf.kernel.eda")
static vsf_err_t __vsf_eda_post_evt_ex(vsf_eda_t *this_ptr, vsf_evt_t evt, bool force)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    return vsf_evtq_post_evt_ex(this_ptr, evt, force);
}

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
vsf_err_t vsf_eda_post_msg(vsf_eda_t *this_ptr, void *msg)
{
    VSF_KERNEL_ASSERT((this_ptr != NULL) && !((uint_fast32_t)msg & 1));
    return vsf_evtq_post_msg(this_ptr, msg);
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *this_ptr, vsf_evt_t evt, void *msg)
{
    return vsf_evtq_post_evt_msg(this_ptr, evt, msg);
}
#endif



#if defined(__VSF_KERNEL_TASK_TEDA) || defined(__VSF_KERNEL_TASK_EDA)


#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
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
#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    case VSF_EVT_USER:
        {
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
#   endif
    case VSF_EVT_TIMER:
        // TODO: kernel_evthandler is running at vsf_prio_highest
        //   need to call vsf_protect_sched?
        origlevel = vsf_protect_sched();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        while ((teda != NULL) && __vsf_timer_is_due(teda->due)) {
            vsf_timq_dequeue(&__vsf_eda.timer.timq, teda);

            teda->use_as__vsf_eda_t.state.bits.is_timed = false;
            vsf_unprotect_sched(origlevel);

#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
            if (teda == &__vsf_eda.teda) {
                vsf_callback_timer_t *timer;
                vsf_dlist_t *done_queue = &__vsf_eda.timer.callback_timq_done;

                origlevel = vsf_protect_sched();
                vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                while ((timer != NULL) && __vsf_timer_is_due(timer->due)) {
                    vsf_callback_timq_dequeue(&__vsf_eda.timer.callback_timq, timer);
                    vsf_unprotect_sched(origlevel);

                    vsf_callback_timq_enqueue(done_queue, timer);

                    origlevel = vsf_protect_sched();
                    vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                }

                if (timer != NULL) {
                    __vsf_teda_timer_enqueue((vsf_teda_t *)eda, timer->due);
                }
                vsf_unprotect_sched(origlevel);
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_EVT_USER);
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
        __vsf_timer_update(true);
#endif
        vsf_unprotect_sched(origlevel);
        break;
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_TIMER

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
    case VSF_EVT_MESSAGE:
        {
            vsf_sync_t *sync = vsf_eda_get_cur_msg();
            VSF_KERNEL_ASSERT(sync != NULL);
            vsf_eda_sync_increase(sync);
        }
        break;
#endif
    }
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif

vsf_err_t vsf_kernel_start(void)
{
#if defined(__VSF_KERNEL_TASK_TEDA) || defined(__VSF_KERNEL_TASK_EDA)
    vsf_err_t err;

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_timer_init();
#   endif

#   if defined(__VSF_KERNEL_TASK_TEDA)
    __vsf_eda.teda.use_as__vsf_eda_t.fn.evthandler = __vsf_kernel_evthandler;
    err = vsf_eda_init(&__vsf_eda.teda.use_as__vsf_eda_t, __vsf_eda.highest_prio, false);
#   elif defined(__VSF_KERNEL_TASK_EDA)
    __vsf_eda.eda.fn.evthandler = __vsf_kernel_evthandler;
    err = vsf_eda_init(&__vsf_eda.eda, __vsf_eda.highest_prio, false);
#   endif

#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (VSF_ERR_NONE == err) {
        __vsf_timer_start();
    }
#   endif
    return err;
#else
    return VSF_ERR_NONE;
#endif
}


#endif
