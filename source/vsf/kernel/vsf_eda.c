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

struct __vsf_local_t {
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    struct {
        vsf_evtq_t *cur;
    } evtq;
#else
    vsf_evtq_ctx_t cur;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    struct {
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
        vsf_teda_t teda;
        vsf_timer_queue_t callback_timq;
        vsf_timer_queue_t callback_timq_done;
#else
        vsf_eda_t eda;
#endif

        vsf_timer_queue_t timq;
        vsf_timer_tick_t pre_tick;
        bool processing;
    } timer;
#endif
#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    vsf_pool(vsf_eda_frame_pool) eda_frame_pool;
#endif
};
typedef struct __vsf_local_t __vsf_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT __vsf_local_t __vsf_eda;

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_KERNEL_ERR_REPORT_EXTERN)                              \
    &&  defined(WEAK_VSF_KERNEL_ERR_REPORT)
WEAK_VSF_KERNEL_ERR_REPORT_EXTERN
#endif

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
extern vsf_err_t __vsf_eda_set_priority(vsf_eda_t *pthis, vsf_prio_t priority);
extern vsf_prio_t __vsf_eda_get_cur_priority(vsf_eda_t *pthis);
#endif

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
extern vsf_evtq_t * __vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_prio_t __vsf_os_evtq_get_prio(vsf_evtq_t *pthis);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
static void vsf_eda_fsm_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
#endif

/*============================ IMPLEMENTATION ================================*/

SECTION(".text.vsf.kernel.eda")
void vsf_eda_on_terminate(vsf_eda_t *pthis)
{
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    if (pthis->on_terminate != NULL) {
        pthis->on_terminate(pthis);
    }
#endif
}

SECTION(".text.vsf.kernel.eda")
#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
void vsf_kernel_init(   vsf_pool_block(vsf_eda_frame_pool) *frame_buf_ptr,
                        uint_fast16_t count)
#else
void vsf_kernel_init(void)
#endif
{
    memset(&__vsf_eda, 0, sizeof(__vsf_eda));

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    do {
    #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
        VSF_POOL_PREPARE(vsf_eda_frame_pool, &(__vsf_eda.eda_frame_pool))
            NULL,                                                               //!< target
            (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,                       //!< region
        END_VSF_POOL_PREPARE(vsf_eda_frame_pool)
    #else
        VSF_POOL_PREPARE(vsf_eda_frame_pool, &(__vsf_eda.eda_frame_pool),
            .pTarget = NULL,
            .ptRegion = (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
        );
    #endif
    
        if ((NULL == frame_buf_ptr) || (0 == count)) {
            break;
        }
        
        VSF_POOL_ADD_BUFFER(
            vsf_eda_frame_pool,
            &(__vsf_eda.eda_frame_pool),
            frame_buf_ptr,
            sizeof(vsf_pool_block(vsf_eda_frame_pool)) * count
        );
    } while(0);
#endif
}

#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * vsf_eda_pop(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_stack_pop(    __vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void vsf_eda_push(vsf_slist_t *list, __vsf_eda_frame_t *frame)
{
    vsf_slist_stack_push(   __vsf_eda_frame_t, 
                            use_as__vsf_slist_node_t, 
                            list, 
                            frame);
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * vsf_eda_peek(vsf_slist_t *list)
{
    __vsf_eda_frame_t *frame = NULL;
    vsf_slist_peek_next(__vsf_eda_frame_t, use_as__vsf_slist_node_t,
                list, frame);
    return frame;
}
#endif

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
implement_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_t);

//SECTION(".text.vsf.kernel.eda_frame_pool")
static __vsf_eda_frame_t * vsf_eda_new_frame(void)
{
    __vsf_eda_frame_t *frame =
            VSF_POOL_ALLOC(vsf_eda_frame_pool, &__vsf_eda.eda_frame_pool);
    if (frame != NULL) {
        memset(frame, 0, sizeof(__vsf_eda_frame_t));  //!< this is important, don't remove it.
        //vsf_slist_init_node(__vsf_eda_frame_t, use_as__vsf_slist_node_t, frame);
    }
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void vsf_eda_free_frame(__vsf_eda_frame_t *frame)
{
    VSF_POOL_FREE(vsf_eda_frame_pool, &__vsf_eda.eda_frame_pool, frame);
}
#else

//! should be provided by user
extern __vsf_eda_frame_t * vsf_eda_new_frame(void);
extern void vsf_eda_free_frame(__vsf_eda_frame_t *frame);

#endif

SECTION(".text.vsf.kernel.vsf_eda_get_valid_eda")
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
    vsf_eda_trace(pthis, evt);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED 
    pthis->state.bits.is_evt_incoming = false;
#   endif

    if (pthis->state.bits.is_use_frame) {
        __vsf_eda_frame_t *frame = pthis->fn.frame;
        VSF_KERNEL_ASSERT(frame != NULL);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (frame->state.bits.is_fsm) {
            vsf_eda_fsm_evthandler(pthis, evt);
        } else {
            if (NULL == frame->ptr.target) {
                frame->fn.evthandler(pthis, evt);
            } else {
                frame->fn.param_evthandler(frame->ptr.target, evt);
            }
        }
#   else
        if (NULL == frame->ptr.target) {
            frame->fn.evthandler(pthis, evt);
        } else {
            frame->fn.param_evthandler(frame->ptr.target, evt);
        }
#   endif
    } else {
        pthis->fn.evthandler(pthis, evt);
    }
#else
    pthis->fn.evthandler(pthis, evt);
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

SECTION(".text.vsf.kernel.vsf_evtq_get_cur_ctx")
static vsf_evtq_ctx_t * vsf_evtq_get_cur_ctx(void)
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
        __vsf_eda.cur.msg = NULL;
    } else {
        __vsf_eda.cur.evt = VSF_EVT_MESSAGE;
        __vsf_eda.cur.msg = (void *)value;
    }
    __vsf_dispatch_evt(pthis, __vsf_eda.cur.evt);
    __vsf_eda.cur = ctx_old;
}

SECTION(".text.vsf.kernel.__vsf_evtq_post")
static vsf_err_t __vsf_evtq_post(vsf_eda_t *pthis, uintptr_t value)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    if (pthis->state.bits.is_processing) {
        if (
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
                !pthis->state.bits.is_limitted
            &&
#endif
                (VSF_EVT_INVALID == pthis->evt_pending)) {
            pthis->evt_pending = value;
            return VSF_ERR_NONE;
        }
    } else {
        pthis->state.bits.is_processing = true;
        __vsf_evtq_post_do(pthis, value);

        if (pthis->evt_pending != VSF_EVT_INVALID) {
            __vsf_evtq_post_do(pthis, pthis->evt_pending);
            pthis->evt_pending = VSF_EVT_INVALID;
        }
        pthis->state.bits.is_processing = false;
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

void vsf_evtq_on_eda_init(vsf_eda_t *pthis)
{
    pthis->evt_pending = VSF_EVT_INVALID;
}

void vsf_evtq_on_eda_fini(vsf_eda_t *pthis)
{
    vsf_eda_on_terminate(pthis);
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

SECTION(".text.vsf.kernel.vsf_evtq_ctx_t")
static vsf_evtq_ctx_t * vsf_evtq_get_cur_ctx(void)
{
    return &__vsf_eda.cur;
}
#endif

#ifndef WEAK_VSF_KERNEL_ERR_REPORT
WEAK(vsf_kernel_err_report)
void vsf_kernel_err_report(enum vsf_kernel_error_t err)
{
    ASSERT(false);
}
#endif

SECTION(".text.vsf.kernel.eda")
vsf_eda_t * vsf_eda_get_cur(void)
{
    vsf_evtq_ctx_t *ctx_cur = vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->eda;
    } else {
    #ifndef WEAK_VSF_KERNEL_ERR_REPORT
        vsf_kernel_err_report(VSF_KERNEL_ERR_NULL_EDA_PTR);
    #else
        WEAK_VSF_KERNEL_ERR_REPORT(VSF_KERNEL_ERR_NULL_EDA_PTR);
    #endif
        return NULL;
    }
}

#if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
bool vsf_eda_polling_state_get(vsf_eda_t *pthis)
{
    VSF_KERNEL_ASSERT( NULL != pthis );
    return pthis->state.bits.polling_state;
}

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
void vsf_eda_polling_state_set(vsf_eda_t *pthis, bool state)
{
    VSF_KERNEL_ASSERT( NULL != pthis );
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED  \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (state) {
        pthis->state.bits.is_evt_incoming = true;
    }
#endif
    pthis->state.bits.polling_state = state ? 1 : 0;
}
#if 0
//! TODO: it is an ugly access
void __vsf_eda_set_is_stack_owner(void)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    if (eda->is_use_frame) {
        eda->frame->is_stack_owner = true;
    }
#endif
    eda->is_stack_owner = true;
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
bool vsf_eda_is_stack_owner(vsf_eda_t *pthis)
{
    VSF_KERNEL_ASSERT(NULL != pthis);
    return pthis->state.bits.is_stack_owner;
}
#endif

SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
vsf_evt_t vsf_eda_get_cur_evt(void)
{
    vsf_evtq_ctx_t *ctx_cur = vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->evt;
    } 
    return VSF_EVT_INVALID;
}

SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
void * vsf_eda_get_cur_msg(void)
{
    vsf_evtq_ctx_t *ctx_cur = vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->msg;
    } else {
        return NULL;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_return")
bool vsf_eda_return(void)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    __vsf_eda_frame_t *frame = NULL;
    if (pthis->state.bits.is_use_frame) {
        frame = vsf_eda_pop(&pthis->fn.frame_list);
        VSF_KERNEL_ASSERT(frame != NULL);
        vsf_eda_free_frame(frame);
        frame = pthis->fn.frame;
        
    #if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        &&  VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
        if (NULL != frame) {
            pthis->state.bits.is_stack_owner = frame->state.bits.is_stack_owner;
        }
    #endif
    }

    if (frame != NULL) {
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (!frame->state.bits.is_fsm) {
#   endif
            __vsf_eda_frame_t *frame_caller = vsf_eda_peek((vsf_slist_t *)frame);
            if (NULL == frame_caller && NULL == frame->ptr.param) {
                pthis->state.bits.is_use_frame = false;
                pthis->fn.evthandler = frame->fn.evthandler;
                
                vsf_eda_free_frame(frame);
            }
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        }
#   endif

        vsf_eda_post_evt(pthis, VSF_EVT_RETURN);
        return false;
    }
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
    __vsf_eda_fini(pthis);
    return true;
}

SECTION(".text.vsf.kernel.vsf_eda_yield")
void vsf_eda_yield(void)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    vsf_eda_post_evt(pthis, VSF_EVT_YIELD);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED

SECTION(".text.vsf.kernel.eda_nesting")
static vsf_err_t vsf_eda_call(uintptr_t func, uintptr_t param, bool is_fsm)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    __vsf_eda_frame_t *frame = vsf_eda_new_frame();
    if (NULL == frame) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    VSF_KERNEL_ASSERT(NULL != func);
    
    
    if (!pthis->state.bits.is_use_frame) {
        __vsf_eda_frame_t *frame_tmp = vsf_eda_new_frame();
        if (NULL == frame_tmp) {
            vsf_eda_free_frame(frame);
            //VSF_KERNEL_ASSERT(false);                    //!< this error is allowed, the sub call can be tried later.
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        pthis->state.bits.is_use_frame = true;
        frame_tmp->fn.evthandler = pthis->fn.evthandler;
        
    #if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        &&  VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
        frame_tmp->state.bits.is_stack_owner = pthis->state.bits.is_stack_owner;
    #endif
        //frame_tmp->param = pthis;
        vsf_slist_init(&pthis->fn.frame_list);
        vsf_eda_push(&pthis->fn.frame_list, frame_tmp);
    }
    
    frame->fn.func = func;
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    frame->state.bits.is_fsm = is_fsm;
#endif
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
    vsf_eda_push(&pthis->fn.frame_list, frame);

    if (VSF_ERR_NONE != vsf_eda_post_evt(pthis, VSF_EVT_INIT)) {
        VSF_KERNEL_ASSERT(false);
    }

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
vsf_err_t __vsf_eda_call_eda(uintptr_t evthandler, uintptr_t param)
{
    return vsf_eda_call(evthandler, param, false);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
fsm_rt_t vsf_eda_call_fsm(vsf_fsm_entry_t entry, uintptr_t param)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    /*
    if (!pthis->is_use_frame) {
        pthis->fsm_return_state = fsm_rt_on_going;
    } else if (!pthis->frame->is_fsm) {
        pthis->fsm_return_state = fsm_rt_on_going;
    }
    */
    fsm_rt_t fsm_return_state = pthis->fsm_return_state;
    
    switch(fsm_return_state) {
        case fsm_rt_on_going:
        case fsm_rt_wait_for_obj:
        //case fsm_rt_asyn:
            break;
        default:
            pthis->fsm_return_state = fsm_rt_on_going;
            return fsm_return_state;
    }
    
    vsf_eda_call((uintptr_t)entry, param, true);
    
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


/*
SECTION(".text.vsf.kernel.eda_fsm")
static void vsf_eda_fsm_evthandler_wrapper(vsf_eda_t *pthis, vsf_evt_t evt)
{
    if (VSF_EVT_MESSAGE == evt) {
        __vsf_eda_frame_t *frame = vsf_eda_get_cur_msg();
        vsf_fsm_entry_t fsm_entry = frame->fsm_entry;
        void *param = frame->param;

        vsf_eda_free_frame(frame);
        fsm_rt_t err = vsf_eda_call_fsm(fsm_entry, param);
        UNUSED_PARAM(err);
        VSF_KERNEL_ASSERT(VSF_ERR_NONE == err);
    } else if (VSF_EVT_RETURN == evt) {
        __vsf_eda_fini(pthis);
    }
}
*/
SECTION(".text.vsf.kernel.eda_fsm")
static void vsf_eda_fsm_evthandler(vsf_eda_t *pthis, vsf_evt_t evt)
{
    fsm_rt_t ret;
    VSF_KERNEL_ASSERT(     pthis != NULL 
            &&  NULL != pthis->fn.frame
            &&  NULL != pthis->fn.frame->fn.fsm_entry);
            
    uintptr_t param = pthis->fn.frame->ptr.target;
    if (NULL == param) {
        param = (uintptr_t )pthis;
    }

    ret = pthis->fn.frame->fn.fsm_entry(param, evt);
    pthis->fsm_return_state = ret;
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
vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *pthis, vsf_eda_evthandler_t evthandler)
{
    VSF_KERNEL_ASSERT(NULL != pthis && NULL != evthandler);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (pthis->state.bits.is_use_frame) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ACCESSABLE;
    }
#endif
    pthis->fn.evthandler = evthandler;
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_init(vsf_eda_t *pthis, vsf_prio_t priority, bool is_stack_owner)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    if (priority == vsf_prio_inherit) {
        VSF_KERNEL_ASSERT(__vsf_eda.evtq.cur != NULL);
        pthis->priority = __vsf_os_evtq_get_prio(__vsf_eda.evtq.cur);
    } else {
        pthis->priority = priority;
    }
#endif
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    vsf_dlist_init_node(vsf_eda_t, pending_node, pthis);
#endif

    pthis->state.flag = 0;

    vsf_evtq_on_eda_init(pthis);

#if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
    pthis->state.bits.is_stack_owner = is_stack_owner;
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    pthis->fsm_return_state = fsm_rt_on_going;
#endif
    return vsf_eda_post_evt(pthis, VSF_EVT_INIT);
}

SECTION(".text.vsf.kernel.vsf_eda_init_ex")
vsf_err_t vsf_eda_init_ex(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg)
{
    vsf_err_t err;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(     NULL != pthis 
            &&  NULL != cfg
            &&  NULL != cfg->fn.func);
    
    pthis->fn.evthandler = cfg->fn.evthandler;
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (NULL == cfg->target) {
        return vsf_eda_init(pthis, cfg->priority, cfg->is_stack_owner);
    } 

    __vsf_eda_frame_t *frame = vsf_eda_new_frame();   
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
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED 
    pthis->on_terminate = cfg->on_terminate;
#endif

    orig = vsf_protect_sched();
        err = vsf_eda_init(pthis, cfg->priority, cfg->is_stack_owner);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        pthis->state.bits.is_use_frame = true;
        pthis->fn.frame = frame;
#endif
    vsf_unprotect_sched(orig);

    return err;
}

/* __vsf_eda_fini() enables you to kill other eda tasks.
   We highly recommend that DO NOT use this api until you 100% sure.
   please make sure that the resources are properly freed when you trying to kill
   an eda other than your own. We highly recommend that please send a semaphore to
   the target eda to ask it killing itself after properly freeing all the resources.
 */
SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis)
{
    pthis = (vsf_eda_t *)__vsf_eda_get_valid_eda(pthis);
    VSF_KERNEL_ASSERT(pthis != NULL);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_teda_cancel_timer((vsf_teda_t *)pthis);
#endif

    vsf_evtq_on_eda_fini(pthis);
    return VSF_ERR_NONE;
}


SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (pthis->state.bits.is_evt_incoming && evt == VSF_EVT_YIELD) {
        return VSF_ERR_NONE;
    }
    pthis->state.bits.is_evt_incoming = true;
#endif

    return vsf_evtq_post_evt(pthis, evt);
}

SECTION(".text.vsf.kernel.eda")
static vsf_err_t vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_evtq_post_evt_ex(pthis, evt, force);
}

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && !((uint_fast32_t)msg & 1));
    return vsf_evtq_post_msg(pthis, msg);
}

#if VSF_KERNEL_CFG_EVT_MESSAGE_EN == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg)
{
    return vsf_evtq_post_evt_msg(pthis, evt, msg);
}
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_sync_t for critical_section, semaphore, event                           *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_sync")
vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, int_fast32_t timeout)
{
    eda = __vsf_eda_get_valid_eda(eda);

    if (timeout > 0) {
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        eda->state.bits.is_limitted = true;
        vsf_teda_set_timer(timeout);
#else
    #ifndef WEAK_VSF_KERNEL_ERR_REPORT
        vsf_kernel_err_report(VSF_KERNEL_ERR_EDA_DOES_NOT_SUPPORT_TIMER);
    #else
        WEAK_VSF_KERNEL_ERR_REPORT(VSF_KERNEL_ERR_EDA_DOES_NOT_SUPPORT_TIMER);
    #endif
#endif
    }
    return eda;
}

SECTION(".text.vsf.kernel.vsf_sync")
static void __vsf_eda_sync_pend(vsf_sync_t *sync, vsf_eda_t *eda, int_fast32_t timeout)
{
    eda = __vsf_eda_get_valid_eda(eda);

    vsf_dlist_queue_enqueue(
        vsf_eda_t, pending_node,
        &sync->pending_list,
        eda);

    __vsf_eda_set_timeout(eda, timeout);
}

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_eda_t *__vsf_eda_sync_get_eda_pending(vsf_sync_t *sync)
{
    vsf_eda_t *eda;

    vsf_dlist_queue_dequeue(
            vsf_eda_t, pending_node,
            &sync->pending_list,
            eda);
    return eda;
}

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_err_t __vsf_eda_sync_remove_eda(vsf_sync_t *sync, vsf_eda_t *eda)
{
    vsf_protect_t lock_status = vsf_protect_sched();
        vsf_dlist_remove(
            vsf_eda_t, pending_node,
            &sync->pending_list,
            eda);
    vsf_unprotect_sched(lock_status);

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_sync_reason_t __vsf_eda_sync_get_reason(vsf_sync_t *sync, vsf_evt_t evt, bool dequeue_eda)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    vsf_sync_reason_t reason;

    VSF_KERNEL_ASSERT((sync != NULL) && (eda != NULL));

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (evt == VSF_EVT_TIMER) {
        if (eda->state.bits.is_sync_got) {
            return VSF_SYNC_PENDING;
        }
        if (dequeue_eda) {
            __vsf_eda_sync_remove_eda(sync, eda);
        }
        reason = VSF_SYNC_TIMEOUT;
    } else {
        vsf_teda_cancel_timer((vsf_teda_t *)eda);
#else
    {
#endif
        if (evt == VSF_EVT_SYNC) {
            reason = VSF_SYNC_GET;
        } else if (evt == VSF_EVT_SYNC_CANCEL) {
            reason = VSF_SYNC_CANCEL;
        } else {
            VSF_KERNEL_ASSERT(false);
        }
    }
    eda->state.bits.is_limitted = false;
    return reason;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur, uint_fast16_t max)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    pthis->cur_union.cur_value = cur;
    pthis->max_union.max_value = max;
    vsf_dlist_init(&pthis->pending_list);
    if (pthis->cur_union.bits.has_owner) {
        ((vsf_sync_owner_t *)pthis)->eda_owner = NULL;
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;
    vsf_eda_t *eda_pending;

    VSF_KERNEL_ASSERT(pthis != NULL)

    origlevel = vsf_protect_sched();
    if (pthis->cur_union.bits.cur >= pthis->max_union.bits.max) {
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_OVERRUN;
    }
    pthis->cur_union.bits.cur++;
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
    if (pthis->cur_union.bits.has_owner) {
        eda = __vsf_eda_get_valid_eda(eda);
        VSF_KERNEL_ASSERT(((vsf_sync_owner_t *)pthis)->eda_owner == eda);
        ((vsf_sync_owner_t *)pthis)->eda_owner = NULL;
        if (eda->cur_priority != eda->priority) {
            __vsf_eda_set_priority(eda, (vsf_prio_t)eda->priority);
        }
    }
#endif

    while (1) {
        if (pthis->cur_union.bits.cur > 0) {
            eda_pending = __vsf_eda_sync_get_eda_pending(pthis);
            if (eda_pending != NULL) {
                eda_pending->state.bits.is_sync_got = true;
                if (!pthis->max_union.bits.manual_rst) {
                    pthis->cur_union.bits.cur--;
                }
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
                if (pthis->cur_union.bits.has_owner) {
                    ((vsf_sync_owner_t *)pthis)->eda_owner = eda_pending;
                }
#endif
            }
            vsf_unprotect_sched(origlevel);

            if (eda_pending != NULL) {
                vsf_err_t err = vsf_eda_post_evt_ex(eda_pending, VSF_EVT_SYNC, true);
                VSF_KERNEL_ASSERT(!err);
                UNUSED_PARAM(err);
                origlevel = vsf_protect_sched();
                continue;
            } else {
                break;
            }
        } else {
            vsf_unprotect_sched(origlevel);
            break;
        }
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis)
{
    return vsf_eda_sync_increase_ex(pthis, NULL);
}

SECTION(".text.vsf.kernel.vsf_sync")
void vsf_eda_sync_force_reset(vsf_sync_t *pthis)
{
    vsf_protect_t origlevel = vsf_protect_sched();
        pthis->cur_union.bits.cur = 0;
    vsf_unprotect_sched(origlevel);
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT(pthis != NULL);

    eda = __vsf_eda_get_valid_eda(eda);

    origlevel = vsf_protect_sched();
    if ((pthis->cur_union.bits.cur > 0) && vsf_dlist_is_empty(&pthis->pending_list)) {
        if (!pthis->max_union.bits.manual_rst) {
            pthis->cur_union.bits.cur--;
        }
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        if (pthis->cur_union.bits.has_owner) {
            VSF_KERNEL_ASSERT((NULL == ((vsf_sync_owner_t *)pthis)->eda_owner) && (0 == pthis->cur_union.bits.cur));
            ((vsf_sync_owner_t *)pthis)->eda_owner = eda;
        }
#endif
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NONE;
    }

    if (timeout != 0) {
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        if (pthis->cur_union.bits.has_owner) {
            //! use __vsf_eda_get_cur_priority to get actual cur_priority
            vsf_prio_t cur_priority = __vsf_eda_get_cur_priority(eda);
            vsf_dlist_insert(
                vsf_eda_t, pending_node,
                &pthis->pending_list,
                eda,
                _->cur_priority < cur_priority);
            __vsf_eda_set_timeout(eda, timeout);
        } else
#endif
        {
            __vsf_eda_sync_pend(pthis, eda, timeout);
        }
#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        if (pthis->cur_union.bits.has_owner) {
            vsf_eda_t *eda_owner = ((vsf_sync_owner_t *)pthis)->eda_owner;

            if (eda->cur_priority > eda_owner->cur_priority) {
                __vsf_eda_set_priority(eda_owner, (vsf_prio_t)eda->cur_priority);
            }
        }
#endif
    }
    vsf_unprotect_sched(origlevel);
    return VSF_ERR_NOT_READY;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, int_fast32_t timeout)
{
    return vsf_eda_sync_decrease_ex(pthis, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
void vsf_eda_sync_cancel(vsf_sync_t *pthis)
{
    vsf_eda_t *eda;
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT(pthis != NULL);

    do {
        origlevel = vsf_protect_sched();
        eda = __vsf_eda_sync_get_eda_pending(pthis);
        if (eda != NULL) {
            eda->state.bits.is_sync_got = true;
            vsf_unprotect_sched(origlevel);
            vsf_eda_post_evt_ex(eda, VSF_EVT_SYNC_CANCEL, true);
        } else {
            vsf_unprotect_sched(origlevel);
        }
    } while (eda != NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt)
{
    return __vsf_eda_sync_get_reason(pthis, evt, true);
}

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_bmpevt_t                                                                *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_set")
static void __vsf_eda_bmpevt_adapter_set(vsf_bmpevt_adapter_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    vsf_eda_bmpevt_set(pthis->bmpevt_host, pthis->mask);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_eda_init")
static vsf_err_t __vsf_eda_bmpevt_adapter_eda_init(vsf_bmpevt_adapter_eda_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_eda_init(&pthis->eda, vsf_prio_inherit, false);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_sync_evthander")
static void __vsf_eda_bmpevt_adapter_sync_evthander(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_KERNEL_ASSERT(eda != NULL);
    vsf_bmpevt_adapter_sync_t *adapter_sync = (vsf_bmpevt_adapter_sync_t *)
            container_of(eda, vsf_bmpevt_adapter_eda_t, eda);
    
    switch (evt) {
        case VSF_EVT_INIT:
            if (vsf_eda_sync_decrease(adapter_sync->sync, -1)) {
                break;
            }
        case VSF_EVT_SYNC:
            __vsf_eda_bmpevt_adapter_set((vsf_bmpevt_adapter_t *)adapter_sync);
            break;
    }
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_sync_init")
static vsf_err_t __vsf_eda_bmpevt_adapter_sync_init(vsf_bmpevt_adapter_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    vsf_bmpevt_adapter_eda_t *adapter_eda = (vsf_bmpevt_adapter_eda_t *)pthis;

    adapter_eda->eda.fn.evthandler = __vsf_eda_bmpevt_adapter_sync_evthander;
    return __vsf_eda_bmpevt_adapter_eda_init(adapter_eda);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_sync_reset")
static vsf_err_t __vsf_eda_bmpevt_adapter_sync_reset(vsf_bmpevt_adapter_t *pthis)
{
    vsf_bmpevt_adapter_sync_t *adapter_sync = (vsf_bmpevt_adapter_sync_t *)pthis;
    VSF_KERNEL_ASSERT(pthis != NULL);
    vsf_sync_t *sync = adapter_sync->sync;

    if (sync->max_union.bits.manual_rst) {
        return vsf_eda_sync_decrease(sync, -1);
    }
    return __vsf_eda_bmpevt_adapter_eda_init((vsf_bmpevt_adapter_eda_t *)adapter_sync);
}

SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_sync_op")
const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_sync_op = {
    .init = __vsf_eda_bmpevt_adapter_sync_init,
    .reset = __vsf_eda_bmpevt_adapter_sync_reset,
};

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_bmpevt_evthander")
static void __vsf_eda_bmpevt_adapter_bmpevt_evthander(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_bmpevt_adapter_bmpevt_t *adapter_bmpevt;
    VSF_KERNEL_ASSERT(eda != NULL);
    adapter_bmpevt = (vsf_bmpevt_adapter_bmpevt_t *)
            container_of(eda, vsf_bmpevt_adapter_eda_t, eda);

    switch (evt) {
        case VSF_EVT_INIT:
            if (vsf_eda_bmpevt_pend(adapter_bmpevt->bmpevt, &adapter_bmpevt->pender, -1)) {
                break;
            }
        case VSF_EVT_SYNC:
            __vsf_eda_bmpevt_adapter_set((vsf_bmpevt_adapter_t *)adapter_bmpevt);
            break;
    }
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_bmpevt_init")
static vsf_err_t __vsf_eda_bmpevt_adapter_bmpevt_init(vsf_bmpevt_adapter_t *pthis)
{
    vsf_bmpevt_adapter_eda_t *adapter_eda = (vsf_bmpevt_adapter_eda_t *)pthis;

    VSF_KERNEL_ASSERT(pthis != NULL);
    adapter_eda->eda.fn.evthandler = __vsf_eda_bmpevt_adapter_bmpevt_evthander;
    return __vsf_eda_bmpevt_adapter_eda_init(adapter_eda);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_bmpevt_reset")
static vsf_err_t __vsf_eda_bmpevt_adapter_bmpevt_reset(vsf_bmpevt_adapter_t *pthis)
{
    vsf_bmpevt_adapter_bmpevt_t *adapter_bmpevt = (vsf_bmpevt_adapter_bmpevt_t *)pthis;
    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_eda_bmpevt_reset(adapter_bmpevt->bmpevt, adapter_bmpevt->pender.mask);
    return __vsf_eda_bmpevt_adapter_eda_init((vsf_bmpevt_adapter_eda_t *)adapter_bmpevt);
}

SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_bmpevt_op")
const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_bmpevt_op = {
    .init = __vsf_eda_bmpevt_adapter_bmpevt_init,
    .reset = __vsf_eda_bmpevt_adapter_bmpevt_reset,
};

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_remove_pender")
static void __vsf_eda_bmpevt_remove_pender(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (pender != NULL) && (pender->eda_pending != NULL));

    vsf_protect_t lock_status = vsf_protect_sched();
        __vsf_dlist_remove_imp(&pthis->pending_list, &pender->eda_pending->pending_node);
    vsf_unprotect_sched(lock_status);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_poll")
static bool __vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender)
{
    uint_fast32_t cur_mask;
    vsf_err_t err = VSF_ERR_FAIL;

    VSF_KERNEL_ASSERT((pthis != NULL) && (pender != NULL));

    cur_mask = pthis->value & pender->mask;
    if (pender->operator == VSF_BMPEVT_AND) {
        if (cur_mask == pender->mask) {
            err = VSF_ERR_NONE;
        }
    } else {
        if (cur_mask) {
            err = VSF_ERR_NONE;
        }
    }

    if (!err) {
        vsf_eda_bmpevt_reset(pthis, cur_mask & pthis->auto_reset);
    }
    return (err == VSF_ERR_NONE);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_post_evt")
static void __vsf_eda_bmpevt_post_evt(vsf_bmpevt_t *pthis, vsf_dlist_node_t *node, vsf_evt_t evt)
{
    bool is_poll_event = evt == VSF_EVT_SYNC_POLL;

    if (!node) {
        poll_end:
        if (is_poll_event) {
            bool is_to_repoll;

            do {
                vsf_protect_t lock_status = vsf_protect_sched();
                    is_to_repoll = pthis->state.bits.is_to_repoll;
                    if (!is_to_repoll) {
                        pthis->state.bits.is_polling = false;
                    }
                vsf_unprotect_sched(lock_status);
            } while (0);

            if (is_to_repoll) {
                repoll:
                pthis->state.bits.is_to_repoll = false;
                __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, evt);
            }
        }
    } else {
        while (node != NULL) {
            if (is_poll_event && pthis->state.bits.is_to_repoll) {
                goto repoll;
            }

            vsf_eda_t *eda = __vsf_dlist_get_host(vsf_eda_t, pending_node, node);
            if (!vsf_eda_post_evt(eda, evt)) {
                break;
            } else {
                node = node->next;
            }
        }
        if (NULL == node) {
            goto poll_end;
        }
    }
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_cancel_start")
static void __vsf_eda_bmpevt_cancel_start(vsf_bmpevt_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->state.bits.is_cancelling = true;
    vsf_unprotect_sched(lock_status);

    __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, VSF_EVT_SYNC_CANCEL);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_poll_start")
static void __vsf_eda_bmpevt_poll_start(vsf_bmpevt_t *pthis)
{
    bool is_polling;

    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        is_polling = pthis->state.bits.is_polling;
        if (is_polling) {
            pthis->state.bits.is_to_repoll = true;
        } else {
            pthis->state.bits.is_polling = true;
        }
    vsf_unprotect_sched(lock_status);

    if (!is_polling) {
        __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, VSF_EVT_SYNC_POLL);
    }
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_init")
vsf_err_t vsf_eda_bmpevt_init(vsf_bmpevt_t *pthis, uint_fast8_t adapter_count)
{
    vsf_bmpevt_adapter_t *adapter;
    uint_fast8_t i;
    VSF_KERNEL_ASSERT(pthis != NULL && adapter_count <= 32);

    vsf_dlist_init(&pthis->pending_list);
    pthis->state.flag = 0;
    pthis->value = 0;
    pthis->cancelled_value = 0;
    pthis->state.bits.adapter_count = adapter_count;

    for (i = 0; i < pthis->state.bits.adapter_count; i++) {
        adapter = pthis->adapters[i];
        if (adapter != NULL) {
            adapter->bmpevt_host = pthis;
            if (adapter->op->init != NULL) {
                adapter->op->init(adapter);
            }
        }
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_reset")
vsf_err_t vsf_eda_bmpevt_reset(vsf_bmpevt_t *pthis, uint_fast32_t mask)
{
    vsf_bmpevt_adapter_t *adapter;
    uint_fast8_t i;
    VSF_KERNEL_ASSERT(pthis != NULL);

    mask &= pthis->value;
    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->value &= ~mask;
    vsf_unprotect_sched(lock_status);

    for (i = 0; i < pthis->state.bits.adapter_count; i++) {
        adapter = pthis->adapters[i];
        if ((mask & (1 << i)) && (adapter != NULL)) {
            if (adapter->op->reset != NULL) {
                adapter->op->reset(adapter);
            }
        }
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_cancel")
vsf_err_t vsf_eda_bmpevt_cancel(vsf_bmpevt_t *pthis, uint_fast32_t mask)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->cancelled_value |= mask;
    vsf_unprotect_sched(lock_status);

    __vsf_eda_bmpevt_cancel_start(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->value |= mask;
    vsf_unprotect_sched(lock_status);

    __vsf_eda_bmpevt_poll_start(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_pend")
vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, int_fast32_t timeout)
{
    vsf_protect_t orig;
    vsf_eda_t *eda;

    VSF_KERNEL_ASSERT((pthis != NULL) && (pender != NULL));

    if (0 == timeout) {
        if (__vsf_eda_bmpevt_poll(pthis, pender)) {
            return VSF_ERR_NONE;
        } else {
            return VSF_ERR_NOT_READY;
        }
    }

    eda = pender->eda_pending = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(eda != NULL);

    vsf_dlist_init_node(vsf_eda_t, pending_node, eda);

    orig = vsf_protect_sched();
    if (pender->mask & pthis->cancelled_value) {
        vsf_unprotect_sched(orig);
        return VSF_ERR_FAIL;
    }

    __vsf_dlist_add_to_tail_imp(&pthis->pending_list, &eda->pending_node);
    __vsf_eda_set_timeout(eda, timeout);
    vsf_unprotect_sched(orig);

    __vsf_eda_bmpevt_poll_start(pthis);
    return VSF_ERR_NOT_READY;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_poll")
vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, vsf_evt_t evt)
{
    vsf_sync_reason_t reason;
    vsf_eda_t *eda;

    VSF_KERNEL_ASSERT((pthis != NULL) && (pender != NULL) && (pender->eda_pending == vsf_eda_get_cur()));
    eda = pender->eda_pending;

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (evt == VSF_EVT_TIMER) {
        __vsf_eda_bmpevt_remove_pender(pthis, pender);
        reason = VSF_SYNC_TIMEOUT;
    } else
#endif
    if (evt == VSF_EVT_SYNC_CANCEL) {
        if (pthis->cancelled_value && pender->mask) {
            __vsf_eda_bmpevt_remove_pender(pthis, pender);
            reason = VSF_SYNC_CANCEL;
        } else {
            reason = VSF_SYNC_PENDING;
        }
        __vsf_eda_bmpevt_post_evt(pthis, eda->pending_node.next, evt);
    } else if (evt == VSF_EVT_SYNC_POLL) {
        if (__vsf_eda_bmpevt_poll(pthis, pender)) {
            __vsf_eda_bmpevt_remove_pender(pthis, pender);
            reason = VSF_SYNC_GET;
        } else {
            reason = VSF_SYNC_PENDING;
        }
        __vsf_eda_bmpevt_post_evt(pthis, eda->pending_node.next, evt);
    } else {
        VSF_KERNEL_ASSERT(false);
    }
    if (reason != VSF_SYNC_PENDING) {
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        vsf_teda_cancel_timer((vsf_teda_t *)eda);
#endif
        eda->state.bits.is_limitted = false;
    }
    return reason;
}
#endif      // VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_queue_t                                                                 *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_queue")
static void __vsf_eda_queue_notify(vsf_queue_t *pthis, bool tx, vsf_protect_t orig)
{
    vsf_eda_t *eda = tx ?
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            __vsf_eda_sync_get_eda_pending(&pthis->use_as__vsf_sync_t)
#else
            pthis->eda_tx
#endif
        :   pthis->eda_rx;

    if (eda != NULL) {
        if (tx) {
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            pthis->tx_processing = true;
#else
            pthis->eda_tx = NULL;
#endif
        } else {
            pthis->eda_rx = NULL;
        }

        eda->state.bits.is_sync_got = true;
        vsf_unprotect_sched(orig);

        vsf_err_t err = vsf_eda_post_evt(eda, VSF_EVT_SYNC);
        VSF_KERNEL_ASSERT(!err);
        UNUSED_PARAM(err);
    } else {
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        if (tx) {
            pthis->tx_processing = false;
        }
#endif
        vsf_unprotect_sched(orig);
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_init")
vsf_err_t vsf_eda_queue_init(vsf_queue_t *pthis, uint_fast16_t max)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
    pthis->tx_processing = false;
#endif
    pthis->eda_rx = NULL;
    return vsf_eda_sync_init(&pthis->use_as__vsf_sync_t, 0, max);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
vsf_err_t vsf_eda_queue_send_ex(vsf_queue_t *pthis, void *node, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            vsf_dlist_is_empty(&pthis->use_as__vsf_sync_t.pending_list)
        &&
#endif
            (pthis->use_as__vsf_sync_t.cur_union.cur_value < pthis->use_as__vsf_sync_t.max_union.bits.max)) {
        pthis->use_as__vsf_sync_t.cur_union.cur_value++;
        pthis->op.enqueue(pthis, node);
        __vsf_eda_queue_notify(pthis, false, origlevel);
        return VSF_ERR_NONE;
    } else {
        if (timeout != 0) {
            __vsf_eda_sync_pend(sync, eda, timeout);
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NOT_READY;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
vsf_err_t vsf_eda_queue_send(vsf_queue_t *pthis, void *node, int_fast32_t timeout)
{
    return vsf_eda_queue_send_ex(pthis, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, void *node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sync, evt);

    vsf_protect_t origlevel = vsf_protect_sched();
    if (VSF_SYNC_GET == reason) {
        pthis->op.enqueue(pthis, node);
        sync->cur_union.cur_value++;
        __vsf_eda_queue_notify(pthis, false, origlevel);

#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        origlevel = vsf_protect_sched();
        if (sync->cur_value < sync->max) {
            __vsf_eda_queue_notify(pthis, true, origlevel);
            return reason;
        } else {
            pthis->tx_processing = false;
        }
#else
        return reason;
#endif
    }
    vsf_unprotect_sched(origlevel);
    return reason;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
vsf_err_t vsf_eda_queue_recv_ex(vsf_queue_t *pthis, void **node, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (pthis->use_as__vsf_sync_t.cur_union.cur_value > 0) {
        pthis->use_as__vsf_sync_t.cur_union.cur_value--;
        pthis->op.dequeue(pthis, node);
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        if (!pthis->tx_processing) {
            __vsf_eda_queue_notify(pthis, true, origlevel);
            return VSF_ERR_NONE;
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NONE;
#else
        __vsf_eda_queue_notify(pthis, true, origlevel);
        return VSF_ERR_NONE;
#endif
    } else {
        if (timeout != 0) {
            VSF_KERNEL_ASSERT(NULL == pthis->eda_rx);
            pthis->eda_rx = __vsf_eda_set_timeout(eda, timeout);
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NOT_READY;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
vsf_err_t vsf_eda_queue_recv(vsf_queue_t *pthis, void **node, int_fast32_t timeout)
{
    return vsf_eda_queue_recv_ex(pthis, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, void **node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = __vsf_eda_sync_get_reason(sync, evt, false);

    vsf_protect_t origlevel = vsf_protect_sched();
    if (VSF_SYNC_GET == reason) {
        pthis->op.dequeue(pthis, node);
        sync->cur_union.cur_value--;
        __vsf_eda_queue_notify(pthis, true, origlevel);
        return reason;
    } else if (VSF_SYNC_TIMEOUT == reason) {
        pthis->eda_rx = NULL;
    }
    vsf_unprotect_sched(origlevel);
    return reason;
}
#endif      // __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE
#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.teda")
static void vsf_teda_timer_enqueue(vsf_teda_t *pthis, vsf_timer_tick_t due)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && !pthis->use_as__vsf_eda_t.state.bits.is_timed);
    pthis->due = due;

    vsf_timq_insert(&__vsf_eda.timer.timq, pthis);
    pthis->use_as__vsf_eda_t.state.bits.is_timed = true;
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                      \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                       \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    pthis->use_as__vsf_eda_t.state.bits.is_evt_incoming = true;
#endif
}

static void vsf_timer_wakeup(void)
{
    if (!__vsf_eda.timer.processing) {
        __vsf_eda.timer.processing = true;
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
        if (vsf_eda_post_evt(&__vsf_eda.timer.teda.use_as__vsf_eda_t, VSF_EVT_TIMER)) {
#else
        if (vsf_eda_post_evt(&__vsf_eda.timer.eda, VSF_EVT_TIMER)) {
#endif
            __vsf_eda.timer.processing = false;
        }
    }
}

static void vsf_timer_update(bool force)
{
    vsf_teda_t *teda;

    vsf_timq_peek(&__vsf_eda.timer.timq, teda);
    if (NULL == teda) {
        vsf_systimer_set_idle();
    } else if (force || (teda->due != __vsf_eda.timer.pre_tick)) {
        __vsf_eda.timer.pre_tick = teda->due;
        if (!vsf_systimer_set(teda->due)) {
            vsf_timer_wakeup();
        }
    }
}

SECTION(".text.vsf.kernel.teda")
static vsf_err_t vsf_teda_set_timer_imp(vsf_teda_t *pthis, vsf_timer_tick_t due)
{
    vsf_teda_timer_enqueue(pthis, due);
    vsf_timer_update(false);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.teda")
static void __vsf_timer_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_teda_t *teda;
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT(eda != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        break;
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    case VSF_EVT_USER:
        do {
            vsf_callback_timer_t *timer;
            vsf_dlist_t *done_queue = &__vsf_eda.timer.callback_timq_done;

            vsf_callback_timq_dequeue(done_queue, timer);
            while (timer != NULL) {
                if (timer->on_timer != NULL) {
                    timer->on_timer(timer);
                }
                vsf_callback_timq_dequeue(done_queue, timer);
            }
        } while (0);
        break;
#endif
    case VSF_EVT_TIMER:
        origlevel = vsf_protect_sched();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);

        while ((teda != NULL) && vsf_systimer_is_due(teda->due)) {
            vsf_timq_dequeue(&__vsf_eda.timer.timq, teda);

            teda->use_as__vsf_eda_t.state.bits.is_timed = false;
            vsf_unprotect_sched(origlevel);

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
            if (teda == &__vsf_eda.timer.teda) {
                vsf_callback_timer_t *timer;
                vsf_dlist_t *done_queue = &__vsf_eda.timer.callback_timq_done;

                origlevel = vsf_protect_sched();
                vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                while ((timer != NULL) && vsf_systimer_is_due(timer->due)) {
                    vsf_callback_timq_dequeue(&__vsf_eda.timer.callback_timq, timer);
                    vsf_unprotect_sched(origlevel);

                    vsf_callback_timq_enqueue(done_queue, timer);

                    origlevel = vsf_protect_sched();
                    vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                }

                if (timer != NULL) {
                    vsf_teda_timer_enqueue((vsf_teda_t *)eda, timer->due);
                }
                vsf_unprotect_sched(origlevel);
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_EVT_USER);
            } else
#endif
            {
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_EVT_TIMER);
            }

            origlevel = vsf_protect_sched();
            vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        }
        __vsf_eda.timer.processing = false;
        vsf_timer_update(true);
        vsf_unprotect_sched(origlevel);
        break;
    }
}

//SECTION(".text.vsf.kernel.teda")
void vsf_systimer_evthandler(vsf_systimer_cnt_t tick)
{
    UNUSED_PARAM(tick);
    vsf_timer_wakeup();
}

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_timer_init(void)
{
    vsf_err_t err;

    __vsf_eda.timer.processing = false;
    vsf_timq_init(&__vsf_eda.timer.timq);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq);
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq_done);
#endif

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    __vsf_eda.timer.teda.use_as__vsf_eda_t.fn.evthandler = __vsf_timer_evthandler;
    err = vsf_eda_init(&__vsf_eda.timer.teda.use_as__vsf_eda_t, vsf_prio_highest, false);
#else
    __vsf_eda.timer.eda.evthandler = __vsf_timer_evthandler;
    err = vsf_eda_init(&__vsf_eda.timer.eda, vsf_prio_highest, false);
#endif
    if (VSF_ERR_NONE == err) {
        vsf_systimer_start();
    }
    return err;
}

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick)
{
    vsf_protect_t lock_status;
    VSF_KERNEL_ASSERT(timer != NULL);

    lock_status = vsf_protect_sched();
        timer->due = tick + vsf_timer_get_tick();
        vsf_callback_timq_insert(&__vsf_eda.timer.callback_timq, timer);

        if (NULL == timer->timer_node.prev) {
            vsf_teda_cancel_timer(&__vsf_eda.timer.teda);

            vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
            vsf_teda_set_timer_imp(&__vsf_eda.timer.teda, timer->due);
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_callback_timer_add_ms")
vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms)
{
    vsf_systimer_cnt_t tick = vsf_systimer_ms_to_tick(ms);
    return vsf_callback_timer_add(timer, tick);
}

SECTION(".text.vsf.kernel.vsf_callback_timer_add_us")
vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us)
{
    vsf_systimer_cnt_t tick = vsf_systimer_us_to_tick(us);
    return vsf_callback_timer_add(timer, tick);
}

SECTION(".text.vsf.kernel.vsf_callback_timer_remove")
vsf_err_t vsf_callback_timer_remove(vsf_callback_timer_t *timer)
{
    vsf_protect_t lock_status;
    VSF_KERNEL_ASSERT(timer != NULL);

    lock_status = vsf_protect_sched();
        vsf_callback_timq_remove(&__vsf_eda.timer.callback_timq, timer);
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

#endif

SECTION(".text.vsf.kernel.teda")
vsf_timer_tick_t vsf_timer_get_tick(void)
{
    return vsf_systimer_get();
}

SECTION(".text.vsf.kernel.vsf_timer_get_duration")
uint_fast32_t vsf_timer_get_duration(vsf_timer_tick_t from_time, vsf_timer_tick_t to_time)
{
    if (to_time >= from_time) {
        return to_time - from_time;
    } else {
        return to_time + 0xFFFFFFFF - from_time;
    }
}

SECTION(".text.vsf.kernel.vsf_timer_get_elapsed")
uint_fast32_t vsf_timer_get_elapsed(vsf_timer_tick_t from_time)
{
    return vsf_timer_get_duration(from_time, vsf_timer_get_tick());
}

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_teda_init(vsf_teda_t *pthis, vsf_prio_t priority, bool is_stack_owner)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_eda_init(&pthis->use_as__vsf_eda_t, priority, is_stack_owner);
}

SECTION(".text.vsf.kernel.vsf_teda_init_ex")
vsf_err_t vsf_teda_init_ex(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_eda_init_ex(&(pthis->use_as__vsf_eda_t), cfg);
}

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_teda_set_timer(uint_fast32_t tick)
{
    vsf_teda_t * pthis = (vsf_teda_t *)vsf_eda_get_cur();
    vsf_protect_t origlevel;
    vsf_err_t err;

    if (0 == tick) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_AVAILABLE;
    }
    origlevel = vsf_protect_sched();
        err = vsf_teda_set_timer_imp(
                pthis, vsf_timer_get_tick() + tick);
    vsf_unprotect_sched(origlevel);
    return err;
}

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ms")
vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms)
{
    vsf_systimer_cnt_t tick = vsf_systimer_ms_to_tick(ms);
    return vsf_teda_set_timer(tick);
}

SECTION(".text.vsf.kernel.vsf_teda_set_timer_us")
vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us)
{
    vsf_systimer_cnt_t tick = vsf_systimer_us_to_tick(us);
    return vsf_teda_set_timer(tick);
}

SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
vsf_err_t vsf_teda_cancel_timer(vsf_teda_t *pthis)
{
    vsf_protect_t lock_status;
    pthis = (vsf_teda_t *)__vsf_eda_get_valid_eda((vsf_eda_t *)pthis);

    VSF_KERNEL_ASSERT(pthis != NULL);

    lock_status = vsf_protect_sched();
        if (pthis->use_as__vsf_eda_t.state.bits.is_timed) {
            vsf_timq_remove(&__vsf_eda.timer.timq, pthis);
            pthis->use_as__vsf_eda_t.state.bits.is_timed = false;
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

#endif

#endif
