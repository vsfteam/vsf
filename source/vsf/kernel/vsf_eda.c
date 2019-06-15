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
#define __VSF_EDA_CLASS_IMPLEMENT
#include "kernel/vsf_kernel_cfg.h"

#include "./vsf_kernel_common.h"
#include "./vsf_eda.h"
#include "./vsf_evtq.h"
#include "./vsf_os.h"
#include "./vsf_timq.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __vsf_local_t {
#ifdef VSF_CFG_EVTQ_EN
    struct {
        vsf_evtq_t *cur;
    } evtq;
#else
    vsf_evtq_ctx_t cur;
#endif

#if VSF_CFG_TIMER_EN == ENABLED
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
#if VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    vsf_pool(vsf_eda_frame_pool) eda_frame_pool;
#   ifdef VSF_CFG_EDA_FRAME_POOL_COUNT
    vsf_eda_frame_t eda_frame[VSF_CFG_EDA_FRAME_POOL_COUNT];
#   endif
#endif
};
typedef struct __vsf_local_t __vsf_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT __vsf_local_t __vsf_eda;

/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

#ifdef VSF_CFG_EVTQ_EN
extern vsf_evtq_t * __vsf_os_evtq_get(vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_priority_t __vsf_os_evtq_get_prio(vsf_evtq_t *pthis);
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
#if VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
void vsf_kernel_init(   vsf_pool_block(vsf_eda_frame_pool) *frame_buf_ptr,
                        uint_fast16_t count)
#else
void vsf_kernel_init(void)
#endif
{
    memset(&__vsf_eda, 0, sizeof(__vsf_eda));

#if VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
    do {
        VSF_POOL_PREPARE(vsf_eda_frame_pool, &(__vsf_eda.eda_frame_pool),
            .pTarget = NULL,
            .ptRegion = (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
        );
        
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

#if VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
implement_vsf_pool(vsf_eda_frame_pool, vsf_eda_frame_t);

//SECTION(".text.vsf.kernel.eda_frame_pool")
static vsf_eda_frame_t * vsf_eda_pop(vsf_slist_t *list)
{
    vsf_eda_frame_t *frame = NULL;
    vsf_slist_stack_pop(    vsf_eda_frame_t,
                            use_as__vsf_slist_node_t,
                            list,
                            frame);
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void vsf_eda_push(vsf_slist_t *list, vsf_eda_frame_t *frame)
{
    vsf_slist_stack_push(   vsf_eda_frame_t, 
                            use_as__vsf_slist_node_t, 
                            list, 
                            frame);
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static vsf_eda_frame_t * vsf_eda_peek(vsf_slist_t *list)
{
    vsf_eda_frame_t *frame = NULL;
    vsf_slist_peek_next(vsf_eda_frame_t, use_as__vsf_slist_node_t,
                list, frame);
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static vsf_eda_frame_t * vsf_eda_new_frame(void)
{
    vsf_eda_frame_t *frame =
            VSF_POOL_ALLOC(vsf_eda_frame_pool, &__vsf_eda.eda_frame_pool);
    if (frame != NULL) {
        memset(frame, 0, sizeof(vsf_eda_frame_t));  //!< this is important, don't remove it.
        //vsf_slist_init_node(vsf_eda_frame_t, use_as__vsf_slist_node_t, frame);
    }
    return frame;
}

//SECTION(".text.vsf.kernel.eda_frame_pool")
static void vsf_eda_free_frame(vsf_eda_frame_t *frame)
{
    VSF_POOL_FREE(vsf_eda_frame_pool, &__vsf_eda.eda_frame_pool, frame);
}
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
    ASSERT(pthis != NULL);

#if VSF_CFG_EVT_TRACE_EN == ENABLED
    vsf_eda_trace(pthis, evt);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED 
    pthis->is_evt_incoming = false;
#   endif

    if (pthis->is_use_frame) {
        vsf_eda_frame_t *frame = pthis->frame;
        ASSERT(frame != NULL);

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (frame->is_fsm) {
            vsf_eda_fsm_evthandler(pthis, evt);
        } else {
            if (NULL == frame->target) {
                frame->evthandler(pthis, evt);
            } else {
                frame->param_evthandler(frame, evt);
            }
        }
#   else
        if (NULL == frame->target) {
            frame->evthandler(pthis, evt);
        } else {
            frame->param_evthandler(frame, evt);
        }
#   endif
    } else {
        pthis->evthandler(pthis, evt);
    }
#else
    pthis->evthandler(pthis, evt);
#endif
}

#ifdef VSF_CFG_EVTQ_EN
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
static void __vsf_evtq_post_do(vsf_eda_t *pthis, uint_fast32_t value)
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
static vsf_err_t __vsf_evtq_post(vsf_eda_t *pthis, uint_fast32_t value)
{
    ASSERT(pthis != NULL);

    if (pthis->is_processing) {
        if (
#if VSF_CFG_SYNC_EN == ENABLED
                !pthis->is_limitted
            &&
#endif
                (VSF_EVT_INVALID == pthis->evt_pending)) {
            pthis->evt_pending = value;
            return VSF_ERR_NONE;
        }
    } else {
        pthis->is_processing = true;
        __vsf_evtq_post_do(pthis, value);

        if (pthis->evt_pending != VSF_EVT_INVALID) {
            __vsf_evtq_post_do(pthis, pthis->evt_pending);
            pthis->evt_pending = VSF_EVT_INVALID;
        }
        pthis->is_processing = false;
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
    return __vsf_evtq_post(pthis, (uint_fast32_t)((evt << 1) | 1));
}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt")
vsf_err_t vsf_evtq_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(pthis, evt, false);
}

SECTION(".text.vsf.kernel.vsf_evtq_post_msg")
vsf_err_t vsf_evtq_post_msg(vsf_eda_t *pthis, void *msg)
{
    return __vsf_evtq_post(pthis, (uint_fast32_t)msg);
}

SECTION(".text.vsf.kernel.vsf_evtq_ctx_t")
static vsf_evtq_ctx_t * vsf_evtq_get_cur_ctx(void)
{
    return &__vsf_eda.cur;
}
#endif

SECTION(".text.vsf.kernel.eda")
vsf_eda_t * vsf_eda_get_cur(void)
{
    vsf_evtq_ctx_t *ctx_cur = vsf_evtq_get_cur_ctx();
    if (ctx_cur != NULL) {
        return ctx_cur->eda;
    } else {
        return NULL;
    }
}

#if VSF_USE_SIMPLE_SHELL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
bool vsf_eda_polling_state_get(vsf_eda_t *pthis)
{
    ASSERT( NULL != pthis );
    return pthis->polling_state;
}

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
void vsf_eda_polling_state_set(vsf_eda_t *pthis, bool state)
{
    ASSERT( NULL != pthis );
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED  \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (state) {
        pthis->is_evt_incoming = true;
    }
#endif
    pthis->polling_state = state ? 1 : 0;
}

//! TODO: it is an ugly access
void __vsf_eda_set_is_stack_owner(void)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    if (eda->is_use_frame) {
        eda->frame->is_stack_owner = true;
    }
    eda->is_stack_owner = true;
}

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
bool vsf_eda_is_stack_owner(vsf_eda_t *pthis)
{
    ASSERT(NULL != pthis);
    return pthis->is_stack_owner;
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
    vsf_eda_frame_t *frame = NULL;
    if (pthis->is_use_frame) {
        frame = vsf_eda_pop(&pthis->frame_list);
        ASSERT(frame != NULL);
        vsf_eda_free_frame(frame);
        frame = pthis->frame;
        
    #if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        &&  VSF_USE_SIMPLE_SHELL == ENABLED
        pthis->is_stack_owner = frame->is_stack_owner;
    #endif
    }

    if (frame != NULL) {
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        if (!frame->is_fsm) {
#   endif
            vsf_eda_frame_t *frame_caller = vsf_eda_peek((vsf_slist_t *)frame);
            if (NULL == frame_caller && NULL == frame->param) {
                pthis->is_use_frame = false;
                pthis->evthandler = frame->evthandler;
                
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
static vsf_err_t vsf_eda_call(void *func, void *param, bool is_fsm)
{
    vsf_eda_t *pthis = vsf_eda_get_cur();
    vsf_eda_frame_t *frame = vsf_eda_new_frame();
    if (NULL == frame) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    ASSERT(NULL != func);
    
    
    if (!pthis->is_use_frame) {
        vsf_eda_frame_t *frame_tmp = vsf_eda_new_frame();
        if (NULL == frame_tmp) {
            vsf_eda_free_frame(frame);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        pthis->is_use_frame = true;
        frame_tmp->evthandler = pthis->evthandler;
        
    #if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        &&  VSF_USE_SIMPLE_SHELL == ENABLED
        frame_tmp->is_stack_owner = pthis->is_stack_owner;
    #endif
        //frame_tmp->param = pthis;
        vsf_slist_init(&pthis->frame_list);
        vsf_eda_push(&pthis->frame_list, frame_tmp);
    }
    
    frame->func = func;
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    frame->is_fsm = is_fsm;
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
    frame->param = param;
    vsf_eda_push(&pthis->frame_list, frame);

    if (VSF_ERR_NONE != vsf_eda_post_evt(pthis, VSF_EVT_INIT)) {
        ASSERT(false);
    }

    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
vsf_err_t __vsf_eda_call_eda(void *evthandler, void *param)
{
    return vsf_eda_call(evthandler, param, false);
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
fsm_rt_t vsf_eda_call_fsm(vsf_fsm_entry_t entry, void *param)
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
    
    vsf_eda_call((void *)entry, param, true);
    
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
        vsf_eda_frame_t *frame = vsf_eda_get_cur_msg();
        vsf_fsm_entry_t fsm_entry = frame->fsm_entry;
        void *param = frame->param;

        vsf_eda_free_frame(frame);
        fsm_rt_t err = vsf_eda_call_fsm(fsm_entry, param);
        UNUSED_PARAM(err);
        ASSERT(VSF_ERR_NONE == err);
    } else if (VSF_EVT_RETURN == evt) {
        __vsf_eda_fini(pthis);
    }
}
*/
SECTION(".text.vsf.kernel.eda_fsm")
static void vsf_eda_fsm_evthandler(vsf_eda_t *pthis, vsf_evt_t evt)
{
    fsm_rt_t ret;
    ASSERT(     pthis != NULL 
            &&  NULL != pthis->frame
            &&  NULL != pthis->frame->fsm_entry);
            
    void *param = pthis->frame->target;
    if (NULL == param) {
        param = pthis;
    }

    ret = pthis->frame->fsm_entry(pthis->frame, evt);
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


/*
vsf_err_t vsf_eda_fsm_init(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg)
{
    vsf_err_t err;
    ASSERT(     NULL != pthis 
            &&  NULL != cfg
            &&  NULL != cfg->entry);

    vsf_eda_frame_t *frame = vsf_eda_new_frame();
    if (NULL == frame) { return VSF_ERR_NOT_ENOUGH_RESOURCES; }

    frame->fsm_entry = cfg->entry;
    frame->target = cfg->target;

    pthis->evthandler = vsf_eda_fsm_evthandler_wrapper;
    err = vsf_eda_init(pthis, cfg->priority, false);
    if (VSF_ERR_NONE == err) {
        err = vsf_eda_post_msg(pthis, frame);
    }
    return err;
}
*/
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_FSM
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_init(vsf_eda_t *pthis, vsf_priority_t priority, bool is_stack_owner)
{
    ASSERT(pthis != NULL);

#ifdef VSF_CFG_EVTQ_EN
    if (priority == vsf_priority_inherit) {
        ASSERT(__vsf_eda.evtq.cur != NULL);
        pthis->priority = __vsf_os_evtq_get_prio(__vsf_eda.evtq.cur);
    } else {
        pthis->priority = priority;
    }
#endif
#if VSF_CFG_SYNC_EN == ENABLED
    vsf_dlist_init_node(vsf_eda_t, pending_node, pthis);
#endif

    pthis->flag = 0;

    vsf_evtq_on_eda_init(pthis);

#if VSF_USE_SIMPLE_SHELL == ENABLED
    pthis->is_stack_owner = is_stack_owner;
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
    ASSERT(     NULL != pthis 
            &&  NULL != cfg
            &&  NULL != cfg->func);
    
    pthis->evthandler = cfg->evthandler;
    
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    if (NULL == cfg->target) {
        return vsf_eda_init(pthis, cfg->priority, cfg->is_stack_owner);
    } 

    vsf_eda_frame_t *frame = vsf_eda_new_frame();   
    if (NULL == frame) { 
        return VSF_ERR_NOT_ENOUGH_RESOURCES; 
    }
    //frame->flag = 0;                  //!< clear all flags (Done with memset)

    frame->fsm_entry = cfg->fsm_entry;
    frame->target = cfg->target;
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    frame->is_fsm = cfg->is_fsm;        //!< update is_fsm
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED 
    pthis->on_terminate = cfg->on_terminate;
#endif

    __vsf_sched_safe(
        err = vsf_eda_init(pthis, cfg->priority, cfg->is_stack_owner);
    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        pthis->is_use_frame = true;
        pthis->frame = frame;
    #endif
    )

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
    ASSERT(pthis != NULL);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_teda_cancel_timer((vsf_teda_t *)pthis);
#endif

    vsf_evtq_on_eda_fini(pthis);
    return VSF_ERR_NONE;
}


SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    ASSERT(pthis != NULL);
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                          \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    if (pthis->is_evt_incoming && evt == VSF_EVT_YIELD) {
        return VSF_ERR_NONE;
    }
    pthis->is_evt_incoming = true;
#endif

    return vsf_evtq_post_evt(pthis, evt);
}

SECTION(".text.vsf.kernel.eda")
static vsf_err_t vsf_eda_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
    ASSERT(pthis != NULL);
    return vsf_evtq_post_evt_ex(pthis, evt, force);
}

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg)
{
    ASSERT((pthis != NULL) && !((uint_fast32_t)msg & 1));
    return vsf_evtq_post_msg(pthis, msg);
}

#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg)
{
    return vsf_evtq_post_evt_msg(pthis, evt, msg);
}
#endif

#if VSF_CFG_SYNC_EN == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_sync_t for critical_section, semaphore, event                           *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_sync")
vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, int_fast32_t timeout)
{
    eda = __vsf_eda_get_valid_eda(eda);

    if (timeout > 0) {
#if VSF_CFG_TIMER_EN == ENABLED
        eda->is_limitted = true;
        vsf_teda_set_timer(timeout);
#else
        ASSERT(false);
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

    ASSERT((sync != NULL) && (eda != NULL));

#if VSF_CFG_TIMER_EN == ENABLED
    if (evt == VSF_EVT_TIMER) {
        if (eda->is_sync_got) {
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
        }
    }
    eda->is_limitted = false;
    return reason;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur, uint_fast16_t max)
{
    ASSERT(pthis != NULL);

    pthis->cur_value = cur;
    pthis->max_value = max;
    vsf_dlist_init(&pthis->pending_list);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;
    vsf_eda_t *eda_pending;

    ASSERT((pthis != NULL) && (pthis->cur < pthis->max));

    origlevel = vsf_protect_sched();
    if (pthis->cur >= pthis->max) {
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_BUG;
    }
    pthis->cur++;
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
    if (pthis->has_owner) {
        eda = __vsf_eda_get_valid_eda(eda);
        ASSERT(((vsf_sync_owner_t *)pthis)->eda_owner == eda);
        ((vsf_sync_owner_t *)pthis)->eda_owner = NULL;
        if (eda->cur_priority != eda->priority) {
            vsf_eda_set_priority(eda, (vsf_priority_t)eda->priority);
        }
    }
#endif

    while (1) {
        if (pthis->cur > 0) {
            eda_pending = __vsf_eda_sync_get_eda_pending(pthis);
            if (eda_pending != NULL) {
                eda_pending->is_sync_got = true;
                if (!pthis->manual_rst) {
                    pthis->cur--;
                }
            }
            vsf_unprotect_sched(origlevel);

            if (eda_pending != NULL) {
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
                if (pthis->has_owner) {
                    ((vsf_sync_owner_t *)pthis)->eda_owner = eda_pending;
                }
#endif
                vsf_err_t err = vsf_eda_post_evt_ex(eda_pending, VSF_EVT_SYNC, true);
                ASSERT(!err);
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
vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;

    ASSERT(pthis != NULL);

    eda = __vsf_eda_get_valid_eda(eda);

    origlevel = vsf_protect_sched();
    if ((pthis->cur > 0) && vsf_dlist_is_empty(&pthis->pending_list)) {
        if (!pthis->manual_rst) {
            pthis->cur--;
        }
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
        if (pthis->has_owner) {
            ASSERT((NULL == ((vsf_sync_owner_t *)pthis)->eda_owner) && (0 == pthis->cur));
            ((vsf_sync_owner_t *)pthis)->eda_owner = eda;
        }
#endif
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NONE;
    }

    if (timeout != 0) {
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
        if (pthis->has_owner) {
            vsf_dlist_insert(
                vsf_eda_t, pending_node,
                &pthis->pending_list,
                eda,
                _->cur_priority >= eda->cur_priority);
            __vsf_eda_set_timeout(eda, timeout);
        } else
#endif
        {
            __vsf_eda_sync_pend(pthis, eda, timeout);
        }
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
        if (pthis->has_owner) {
            vsf_eda_t *eda_owner = ((vsf_sync_owner_t *)pthis)->eda_owner;

            if (eda->cur_priority > eda_owner->cur_priority) {
                vsf_eda_set_priority(
                        eda_owner, (vsf_priority_t)eda->cur_priority);
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

    ASSERT(pthis != NULL);

    do {
        origlevel = vsf_protect_sched();
        eda = __vsf_eda_sync_get_eda_pending(pthis);
        if (eda != NULL) {
            eda->is_sync_got = true;
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

#if VSF_CFG_BMPEVT_EN == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_bmpevt_t                                                                *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_set")
static void __vsf_eda_bmpevt_adapter_set(vsf_bmpevt_adapter_t *pthis)
{
    ASSERT(pthis != NULL);
    vsf_eda_bmpevt_set(pthis->bmpevt_host, pthis->mask);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_eda_init")
static vsf_err_t __vsf_eda_bmpevt_adapter_eda_init(vsf_bmpevt_adapter_eda_t *pthis)
{
    ASSERT(pthis != NULL);
    return vsf_eda_init(&pthis->eda, vsf_priority_inherit, false);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_sync_evthander")
static void __vsf_eda_bmpevt_adapter_sync_evthander(vsf_eda_t *eda, vsf_evt_t evt)
{
    ASSERT(eda != NULL);
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
    ASSERT(pthis != NULL);
    vsf_bmpevt_adapter_eda_t *adapter_eda = (vsf_bmpevt_adapter_eda_t *)pthis;

    adapter_eda->eda.evthandler = __vsf_eda_bmpevt_adapter_sync_evthander;
    return __vsf_eda_bmpevt_adapter_eda_init(adapter_eda);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_sync_reset")
static vsf_err_t __vsf_eda_bmpevt_adapter_sync_reset(vsf_bmpevt_adapter_t *pthis)
{
    vsf_bmpevt_adapter_sync_t *adapter_sync = (vsf_bmpevt_adapter_sync_t *)pthis;
    ASSERT(pthis != NULL);
    vsf_sync_t *sync = adapter_sync->sync;

    if (sync->manual_rst) {
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
    ASSERT(eda != NULL);
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

    ASSERT(pthis != NULL);
    adapter_eda->eda.evthandler = __vsf_eda_bmpevt_adapter_bmpevt_evthander;
    return __vsf_eda_bmpevt_adapter_eda_init(adapter_eda);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_adapter_bmpevt_reset")
static vsf_err_t __vsf_eda_bmpevt_adapter_bmpevt_reset(vsf_bmpevt_adapter_t *pthis)
{
    vsf_bmpevt_adapter_bmpevt_t *adapter_bmpevt = (vsf_bmpevt_adapter_bmpevt_t *)pthis;
    ASSERT(pthis != NULL);

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
    ASSERT((pthis != NULL) && (pender != NULL) && (pender->eda_pending != NULL));

    vsf_protect_t lock_status = vsf_protect_sched();
        __vsf_dlist_remove_imp(&pthis->pending_list, &pender->eda_pending->pending_node);
    vsf_unprotect_sched(lock_status);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_poll")
static bool __vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender)
{
    uint_fast32_t cur_mask;
    vsf_err_t err = VSF_ERR_FAIL;

    ASSERT((pthis != NULL) && (pender != NULL));

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
                    is_to_repoll = pthis->is_to_repoll;
                    if (!is_to_repoll) {
                        pthis->is_polling = false;
                    }
                vsf_unprotect_sched(lock_status);
            } while (0);

            if (is_to_repoll) {
                repoll:
                pthis->is_to_repoll = false;
                __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, evt);
            }
        }
    } else {
        while (node != NULL) {
            if (is_poll_event && pthis->is_to_repoll) {
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
    ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->is_cancelling = true;
    vsf_unprotect_sched(lock_status);

    __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, VSF_EVT_SYNC_CANCEL);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_poll_start")
static void __vsf_eda_bmpevt_poll_start(vsf_bmpevt_t *pthis)
{
    bool is_polling;

    ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        is_polling = pthis->is_polling;
        if (is_polling) {
            pthis->is_to_repoll = true;
        } else {
            pthis->is_polling = true;
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
    ASSERT(pthis != NULL && adapter_count <= 32);

    vsf_dlist_init(&pthis->pending_list);
    pthis->flag = 0;
    pthis->value = 0;
    pthis->cancelled_value = 0;
    pthis->adapter_count = adapter_count;

    for (uint_fast8_t i = 0; i < pthis->adapter_count; i++) {
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

    ASSERT(pthis != NULL);

    mask &= pthis->value;
    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->value &= ~mask;
    vsf_unprotect_sched(lock_status);

    for (uint_fast8_t i = 0; i < pthis->adapter_count; i++) {
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
    ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        pthis->cancelled_value |= mask;
    vsf_unprotect_sched(lock_status);

    __vsf_eda_bmpevt_cancel_start(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask)
{
    ASSERT(pthis != NULL);

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

    ASSERT((pthis != NULL) && (pender != NULL));

    if (0 == timeout) {
        if (__vsf_eda_bmpevt_poll(pthis, pender)) {
            return VSF_ERR_NONE;
        } else {
            return VSF_ERR_NOT_READY;
        }
    }

    eda = pender->eda_pending = vsf_eda_get_cur();
    ASSERT(eda != NULL);

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

    ASSERT((pthis != NULL) && (pender != NULL) && (pender->eda_pending == vsf_eda_get_cur()));
    eda = pender->eda_pending;

#if VSF_CFG_TIMER_EN == ENABLED
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
    }
    if (reason != VSF_SYNC_PENDING) {
#if VSF_CFG_TIMER_EN == ENABLED
        vsf_teda_cancel_timer((vsf_teda_t *)eda);
#endif
        eda->is_limitted = false;
    }
    return reason;
}
#endif      // VSF_CFG_BMPEVT_EN

#if VSF_CFG_QUEUE_EN == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_queue_t                                                                 *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_queue")
static void __vsf_eda_queue_notify(vsf_queue_t *pthis, bool tx, vsf_protect_t orig)
{
    vsf_eda_t *eda = tx ?
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
            __vsf_eda_sync_get_eda_pending(&pthis->use_as__vsf_sync_t)
#else
            pthis->eda_tx
#endif
        :   pthis->eda_rx;

    if (eda != NULL) {
        if (tx) {
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
            pthis->tx_processing = true;
#else
            pthis->eda_tx = NULL;
#endif
        } else {
            pthis->eda_rx = NULL;
        }

        eda->is_sync_got = true;
        vsf_unprotect_sched(orig);

        vsf_err_t err = vsf_eda_post_evt(eda, VSF_EVT_SYNC);
        ASSERT(!err);
        UNUSED_PARAM(err);
    } else {
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
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
    ASSERT(pthis != NULL);
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
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

    ASSERT((pthis != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
            vsf_dlist_is_empty(&pthis->use_as__vsf_sync_t.pending_list)
        &&
#endif
            (pthis->use_as__vsf_sync_t.cur_value < pthis->use_as__vsf_sync_t.max)) {
        pthis->use_as__vsf_sync_t.cur_value++;
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
        sync->cur_value++;
        __vsf_eda_queue_notify(pthis, false, origlevel);

#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
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

    ASSERT((pthis != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (pthis->use_as__vsf_sync_t.cur_value > 0) {
        pthis->use_as__vsf_sync_t.cur_value--;
        pthis->op.dequeue(pthis, node);
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
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
            ASSERT(NULL == pthis->eda_rx);
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
        sync->cur_value--;
        __vsf_eda_queue_notify(pthis, true, origlevel);
        return reason;
    } else if (VSF_SYNC_TIMEOUT == reason) {
        pthis->eda_rx = NULL;
    }
    vsf_unprotect_sched(origlevel);
    return reason;
}
#endif      // VSF_CFG_QUEUE_EN
#endif      // VSF_CFG_SYNC_EN

#if VSF_CFG_TIMER_EN == ENABLED
SECTION(".text.vsf.kernel.teda")
static void vsf_teda_timer_enqueue(vsf_teda_t *pthis, vsf_timer_tick_t due)
{
    ASSERT((pthis != NULL) && !pthis->use_as__vsf_eda_t.is_timed);
    pthis->due = due;

    vsf_protect_t lock_status = vsf_protect_sched();
        vsf_timq_insert(&__vsf_eda.timer.timq, pthis);
        pthis->use_as__vsf_eda_t.is_timed = true;
    #if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                      \
        &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                       \
            ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
        pthis->use_as__vsf_eda_t.is_evt_incoming = true;
    #endif
    vsf_unprotect_sched(lock_status);
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
    vsf_protect_t lock_status = vsf_protect_sched();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        if (NULL == teda) {
            vsf_systimer_set_idle();
        } else if (force || (teda->due != __vsf_eda.timer.pre_tick)) {
            __vsf_eda.timer.pre_tick = teda->due;
            if (!vsf_systimer_set(teda->due)) {
                vsf_timer_wakeup();
            }
        }
    vsf_unprotect_sched(lock_status);
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

    ASSERT(eda != NULL);

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

            teda->use_as__vsf_eda_t.is_timed = false;
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
        vsf_unprotect_sched(origlevel);
        __vsf_eda.timer.processing = false;
        vsf_timer_update(true);
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
    __vsf_eda.timer.processing = false;
    vsf_timq_init(&__vsf_eda.timer.timq);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq);
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq_done);
#endif

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    __vsf_eda.timer.teda.use_as__vsf_eda_t.evthandler = __vsf_timer_evthandler;
    return vsf_eda_init(&__vsf_eda.timer.teda.use_as__vsf_eda_t, vsf_priority_highest, false);
#else
    __vsf_eda.timer.eda.evthandler = __vsf_timer_evthandler;
    return vsf_eda_init(&__vsf_eda.timer.eda, vsf_priority_highest, false);
#endif
}

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick)
{
    vsf_timer_tick_t due = tick + vsf_timer_get_tick();
    ASSERT(timer != NULL);

    timer->due = due;

    vsf_protect_t lock_status = vsf_protect_sched();
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
vsf_err_t vsf_teda_init(vsf_teda_t *pthis, vsf_priority_t priority, bool is_stack_owner)
{
    ASSERT(pthis != NULL);
    return vsf_eda_init(&pthis->use_as__vsf_eda_t, priority, is_stack_owner);
}

SECTION(".text.vsf.kernel.vsf_teda_init_ex")
vsf_err_t vsf_teda_init_ex(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg)
{
    ASSERT(pthis != NULL);
    return vsf_eda_init_ex(&(pthis->use_as__vsf_eda_t), cfg);
}

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_teda_set_timer(uint_fast32_t tick)
{
    if (0 == tick) {
        ASSERT(false);
        return VSF_ERR_NOT_AVAILABLE;
    }
    return vsf_teda_set_timer_imp(
                    (vsf_teda_t *)vsf_eda_get_cur(),
                    vsf_timer_get_tick() + tick);
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
    pthis = (vsf_teda_t *)__vsf_eda_get_valid_eda((vsf_eda_t *)pthis);

    ASSERT(pthis != NULL);

    vsf_protect_t lock_status = vsf_protect_sched();
        if (pthis->use_as__vsf_eda_t.is_timed) {
            vsf_timq_remove(&__vsf_eda.timer.timq, pthis);
            pthis->use_as__vsf_eda_t.is_timed = false;
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

#endif
