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
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
        vsf_timer_queue_t   callback_timq;
        vsf_timer_queue_t   callback_timq_done;
#endif

        vsf_timer_queue_t   timq;
#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
        vsf_timer_tick_t    pre_tick;
#endif
        bool                processing;
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

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED && !defined(VSF_SYSTIMER_CFG_IMPL_MODE)
extern bool vsf_systimer_is_due(vsf_systimer_cnt_t due);
extern vsf_err_t vsf_systimer_start(void);
extern vsf_systimer_cnt_t vsf_systimer_get(void);
extern uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick);
extern vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
#endif

SECTION(".text.vsf.kernel.vsf_eda_get_valid_eda")
static vsf_eda_t * __vsf_eda_get_valid_eda(vsf_eda_t *pthis);
SECTION(".text.vsf.kernel.eda")
static vsf_err_t vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force);

/*============================ INCLUDES ======================================*/
#define __EDA_GADGET__
#include "./__eda/vsf_eda_sync.c"
#include "./__eda/vsf_eda_bmpevt.c"
#include "./__eda/vsf_eda_queue.c"
#include "./__eda/vsf_eda_timer.c"
#undef __EDA_GADGET__

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
                        uint_fast16_t count, vsf_prio_t highest_prio)
#else
void vsf_kernel_init(   vsf_prio_t highest_prio)
#endif
{
    memset(&__vsf_eda, 0, sizeof(__vsf_eda));
    __vsf_eda.highest_prio = highest_prio;
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
    vsf_slist_peek_next(__vsf_eda_frame_t,
                        use_as__vsf_slist_node_t,
                        list, frame);
    return frame;
}
#endif

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
implement_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_t)

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
            if ((uintptr_t)0 == frame->ptr.target) {
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
    UNUSED_PARAM(err);
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
        return (void *)ctx_cur->msg;
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
        
    #if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
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
            if (NULL == frame_caller && (uintptr_t)0 == frame->ptr.param) {
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
    vsf_eda_fini(pthis);
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
static vsf_err_t __vsf_eda_ensure_frame_used(vsf_eda_t *pthis, uintptr_t param)
{
    if (!pthis->state.bits.is_use_frame) {
        __vsf_eda_frame_t *frame_tmp = vsf_eda_new_frame();
        if (NULL == frame_tmp) {
            //vsf_eda_free_frame(frame);
            //VSF_KERNEL_ASSERT(false);                    //!< this error is allowed, the sub call can be tried later.
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        pthis->state.bits.is_use_frame = true;
        frame_tmp->fn.evthandler = pthis->fn.evthandler;
        
    #if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
        frame_tmp->state.bits.is_stack_owner = pthis->state.bits.is_stack_owner;
    #endif
        frame_tmp->ptr.target = param;
        vsf_slist_init(&pthis->fn.frame_list);
        vsf_eda_push(&pthis->fn.frame_list, frame_tmp);
    }

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_set")
vsf_err_t vsf_eda_frame_user_value_set(__VSF_KERNEL_CFG_FRAME_UINT_TYPE value)
{
    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t* pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    err = __vsf_eda_ensure_frame_used(pthis, (uintptr_t)pthis);
    if (VSF_ERR_NONE == err) {
        pthis->fn.frame->state.bits.user = value;
    }

    return err;
}

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_get")
vsf_err_t vsf_eda_frame_user_value_get(__VSF_KERNEL_CFG_FRAME_UINT_TYPE *pvalue)
{
    vsf_err_t err = VSF_ERR_NONE;
    vsf_eda_t* pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    do {
        if (NULL == pvalue) {
            err = VSF_ERR_INVALID_PTR;
            break;
        } else if (!pthis->state.bits.is_use_frame) {
            err = VSF_ERR_NOT_AVAILABLE;
            break;
        }
        
        (*pvalue) = pthis->fn.frame->state.bits.user;
        
    } while(0);
    return err;
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
    uintptr_t target = NULL; 
    vsf_eda_t *pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    if (pthis->state.bits.is_use_frame) {
        target = pthis->fn.frame->ptr.target;
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
    vsf_eda_t *pthis = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(NULL != pthis);

    __vsf_eda_frame_t *frame = NULL;
    if (is_sub_call) {
        frame = vsf_eda_new_frame();
        if (NULL == frame) {
            VSF_KERNEL_ASSERT(false);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
    } 
    VSF_KERNEL_ASSERT((uintptr_t)0 != func);
    
    err =  __vsf_eda_ensure_frame_used(pthis, 0);
    if (VSF_ERR_NONE != err) {
        /* 1. if is_sub_call is true, frame will not be NULL. 
           2. vsf_eda_free_frame can accept NULL as input*/
        vsf_eda_free_frame(frame);
        return err;
    }
    
    if (!is_sub_call) {
        frame = pthis->fn.frame;
    }

    frame->fn.func = func;
#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                               \
    ||  VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
    frame->state.flag = state.flag;
#endif    

#   if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
    pthis->state.bits.is_stack_owner = state.bits.is_stack_owner;
#   endif


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

    if (is_sub_call) {
        vsf_eda_push(&pthis->fn.frame_list, frame);
    }

#if VSF_KERNEL_CFG_EDA_FAST_SUB_CALL == ENABLED   //! todo: when the experimental code proved to be stable, remove the old one
        //! for eda without return value, call evthandler directly
    __vsf_dispatch_evt(pthis, VSF_EVT_INIT);
#else
    if (VSF_ERR_NONE != vsf_eda_post_evt(pthis, VSF_EVT_INIT)) {
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
vsf_err_t __vsf_eda_call_eda(uintptr_t evthandler, uintptr_t param)
{
    __vsf_eda_frame_state_t state = { .bits.is_fsm = 0,};
    return __vsf_eda_call_eda_ex(evthandler, param, state, true);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
fsm_rt_t __vsf_eda_call_fsm(vsf_fsm_entry_t entry, uintptr_t param)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    
    fsm_rt_t fsm_return_state = pthis->fsm_return_state;
    __vsf_eda_frame_state_t state = { .bits.is_fsm = true,};
    switch(fsm_return_state) {
        case fsm_rt_on_going:
        case fsm_rt_wait_for_obj:
        //case fsm_rt_asyn:
            break;
        default:
            pthis->fsm_return_state = fsm_rt_on_going;
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
static void vsf_eda_fsm_evthandler(vsf_eda_t *pthis, vsf_evt_t evt)
{
    fsm_rt_t ret;
    VSF_KERNEL_ASSERT(     pthis != NULL 
            &&  NULL != pthis->fn.frame
            &&  NULL != pthis->fn.frame->fn.fsm_entry);
            
    uintptr_t param = pthis->fn.frame->ptr.target;
    if ((uintptr_t)0 == param) {
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

static void __vsf_eda_init( vsf_eda_t *pthis, 
                            vsf_prio_t priority, 
                            bool is_stack_owner)
{
#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
    if (priority == vsf_prio_inherit) {
        
        if (__vsf_eda.evtq.cur == NULL) {
        #   ifndef WEAK_VSF_KERNEL_ERR_REPORT
            vsf_kernel_err_report(
                VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR);
        #   else
            WEAK_VSF_KERNEL_ERR_REPORT(
                VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR);
        #   endif
        }
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
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_init(vsf_eda_t *pthis, vsf_prio_t priority, bool is_stack_owner)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    __vsf_eda_init(pthis, priority, is_stack_owner);

    return vsf_eda_post_evt(pthis, VSF_EVT_INIT);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

SECTION(".text.vsf.kernel.vsf_eda_init_ex")
vsf_err_t vsf_eda_init_ex(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(     NULL != pthis 
            &&  NULL != cfg
            &&  (uintptr_t)0 != cfg->fn.func);
    
    pthis->fn.evthandler = cfg->fn.evthandler;
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if ((uintptr_t)0 == cfg->target) {
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
#   if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
    frame->state.bits.is_stack_owner = cfg->is_stack_owner;
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED 
    pthis->on_terminate = cfg->on_terminate;
#endif


    __vsf_eda_init(pthis, cfg->priority, cfg->is_stack_owner);
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        pthis->state.bits.is_use_frame = true;
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
vsf_err_t vsf_eda_fini(vsf_eda_t *pthis)
{
    pthis = (vsf_eda_t *)__vsf_eda_get_valid_eda(pthis);
    VSF_KERNEL_ASSERT(pthis != NULL);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_teda_cancel_timer((vsf_teda_t *)pthis);
#endif

    vsf_evtq_on_eda_fini(pthis);
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif


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

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg)
{
    return vsf_evtq_post_evt_msg(pthis, evt, msg);
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
        while ((teda != NULL) && vsf_systimer_is_due(teda->due)) {
            vsf_timq_dequeue(&__vsf_eda.timer.timq, teda);

            teda->use_as__vsf_eda_t.state.bits.is_timed = false;
            vsf_unprotect_sched(origlevel);

#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
            if (teda == &__vsf_eda.teda) {
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
#   endif
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

vsf_err_t vk_kernel_start(void)
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
