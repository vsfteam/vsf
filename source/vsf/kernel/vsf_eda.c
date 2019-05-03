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
#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
        vsf_teda_t teda;
        vsf_timer_queue_t callback_timq;
#else
        vsf_eda_t eda;
#endif

        vsf_timer_queue_t timq;
        vsf_timer_tick_t pre_tick;
        bool processing;
    } timer;
#endif
};
typedef struct __vsf_local_t __vsf_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT __vsf_local_t __vsf_eda;

/*============================ PROTOTYPES ====================================*/

#ifdef VSF_CFG_EVTQ_EN
extern vsf_evtq_t * __vsf_os_evtq_get(vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_priority_t __vsf_os_evtq_get_prio(vsf_evtq_t *pthis);
#endif

/*============================ IMPLEMENTATION ================================*/

SECTION(".text.vsf.kernel.eda")
void vsf_kernel_init(void)
{
    memset(&__vsf_eda, 0, sizeof(__vsf_eda));
}

SECTION(".text.vsf.kernel.eda")
void __vsf_dispatch_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    ASSERT(pthis != NULL);

#if VSF_CFG_EVT_TRACE_EN == ENABLED
    vsf_eda_trace(pthis, evt);
#endif
    pthis->evthandler(pthis, evt);
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
                (VSF_EVT_NONE == pthis->evt_pending)
            &&  (value & 1)) {
            pthis->evt_pending = (vsf_evt_t)(value >> 1);
            return VSF_ERR_NONE;
        }
    } else {
        pthis->is_processing = true;
        __vsf_evtq_post_do(pthis, value);

        if (pthis->evt_pending != VSF_EVT_NONE) {
            __vsf_evtq_post_do(pthis, (pthis->evt_pending << 1) | 1);
            pthis->evt_pending = VSF_EVT_NONE;
        }
        pthis->is_processing = false;
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

void vsf_evtq_on_eda_init(vsf_eda_t *pthis) {}
void vsf_evtq_on_eda_fini(vsf_eda_t *pthis) {}

SECTION(".text.vsf.kernel.vsf_evtq_post_evt")
vsf_err_t vsf_evtq_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    return __vsf_evtq_post(pthis, (uint32_t)((evt << 1) | 1));
}

SECTION(".text.vsf.kernel.vsf_evtq_post_msg")
vsf_err_t vsf_evtq_post_msg(vsf_eda_t *pthis, void *msg)
{
    return __vsf_evtq_post(pthis, (uint32_t)msg);
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
    pthis->polling_state = state ? 1 : 0;
}

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

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_init(vsf_eda_t *pthis, vsf_priority_t priority, bool is_stack_owner)
{
    ASSERT(pthis != NULL);

    pthis->polling_state = 0;
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

    pthis->is_stack_owner = is_stack_owner;

    return vsf_eda_post_evt(pthis, VSF_EVT_INIT);
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_fini(vsf_eda_t *pthis)
{
    ASSERT(pthis != NULL);

    vsf_evtq_on_eda_fini(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
bool vsf_eda_is_stack_owner(vsf_eda_t *pthis)
{
    ASSERT(NULL != pthis);
    return pthis->is_stack_owner;
}

SECTION(".text.vsf.kernel.eda")
vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    ASSERT(pthis != NULL);
    return vsf_evtq_post_evt(pthis, evt);
}

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg)
{
    ASSERT((pthis != NULL) && !((uint32_t)msg & 1));
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
static void __vsf_eda_sync_pend(vsf_sync_t *pthis, vsf_eda_t *eda)
{
    vsf_dlist_queue_enqueue(
        vsf_eda_t, pending_node,
        &pthis->pending_list,
        eda);
}

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_eda_t *__vsf_eda_sync_get_eda_pending(vsf_sync_t *pthis)
{
    vsf_eda_t *eda;

    vsf_dlist_queue_dequeue(
            vsf_eda_t, pending_node,
            &pthis->pending_list,
            eda);
    return eda;
}

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_err_t __vsf_eda_sync_remove_eda(vsf_sync_t *pthis, vsf_eda_t *eda)
{
    ASSERT((pthis != NULL) && (eda != NULL));

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        vsf_dlist_remove(
            vsf_eda_t, pending_node,
            &pthis->pending_list,
            eda);
    vsf_sched_unlock(lock_status);

    return VSF_ERR_NONE;
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
    vsf_sched_lock_status_t origlevel;
    vsf_eda_t *eda_pending;

    ASSERT((pthis != NULL) && (pthis->cur < pthis->max));

    origlevel = vsf_sched_lock();
    if (pthis->cur >= pthis->max) {
        vsf_sched_unlock(origlevel);
        return VSF_ERR_BUG;
    }
    pthis->cur++;
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
    if (pthis->has_owner) {
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
            vsf_sched_unlock(origlevel);

            if (eda_pending != NULL) {
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
                if (pthis->has_owner) {
                    ((vsf_sync_owner_t *)pthis)->eda_owner = eda_pending;
                }
#endif
                if (vsf_eda_post_evt(eda_pending, VSF_EVT_SYNC) != VSF_ERR_NONE) {
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
                    if (pthis->has_owner) {
                        ((vsf_sync_owner_t *)pthis)->eda_owner = NULL;
                    }
#endif
                }
                eda_pending->is_sync_got = false;
                origlevel = vsf_sched_lock();
                continue;
            } else {
                break;
            }
        } else {
            vsf_sched_unlock(origlevel);
            break;
        }
    }
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis)
{
    return vsf_eda_sync_increase_ex(pthis, vsf_eda_get_cur());
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_sched_lock_status_t origlevel;

    ASSERT((pthis != NULL) && (eda != NULL));

    origlevel = vsf_sched_lock();
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
        vsf_sched_unlock(origlevel);
        return VSF_ERR_NONE;
    }

    if (timeout != 0) {
#if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
        if (pthis->has_owner) {
            vsf_dlist_insert(
                vsf_eda_t, pending_node,
                &pthis->pending_list,
                eda,
                ptarget->cur_priority >= eda->cur_priority);
        } else
#endif
        {
            __vsf_eda_sync_pend(pthis, eda);
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
        vsf_sched_unlock(origlevel);

        if (timeout > 0) {
#if VSF_CFG_TIMER_EN == ENABLED
            eda->is_limitted = true;
            vsf_teda_set_timer(timeout);
#else
            ASSERT(false);
#endif
        }
    } else {
        vsf_sched_unlock(origlevel);
    }
    return VSF_ERR_NOT_READY;
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, int_fast32_t timeout)
{
    return vsf_eda_sync_decrease_ex(pthis, timeout, vsf_eda_get_cur());
}

SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
void vsf_eda_sync_cancel(vsf_sync_t *pthis)
{
    vsf_eda_t *eda;

    ASSERT(pthis != NULL);

    do {
        eda = __vsf_eda_sync_get_eda_pending(pthis);
        if (eda != NULL) {
            vsf_eda_post_evt(eda, VSF_EVT_SYNC_CANCEL);
        }
    } while (eda != NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    vsf_sync_reason_t reason;

    ASSERT((pthis != NULL) && (eda != NULL));

#if VSF_CFG_TIMER_EN == ENABLED
    if (evt == VSF_EVT_TIMER) {
        if (eda->is_sync_got) {
            return VSF_SYNC_PENDING;
        }
        __vsf_eda_sync_remove_eda(pthis, eda);
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

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        __vsf_dlist_remove_imp(&pthis->pending_list, &pender->eda_pending->pending_node);
    vsf_sched_unlock(lock_status);
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
                vsf_sched_lock_status_t lock_status = vsf_sched_lock();
                    is_to_repoll = pthis->is_to_repoll;
                    if (!is_to_repoll) {
                        pthis->is_polling = false;
                    }
                vsf_sched_unlock(lock_status);
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

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        pthis->is_cancelling = true;
    vsf_sched_unlock(lock_status);

    __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, VSF_EVT_SYNC_CANCEL);
}

SECTION(".text.vsf.kernel.__vsf_eda_bmpevt_poll_start")
static void __vsf_eda_bmpevt_poll_start(vsf_bmpevt_t *pthis)
{
    bool is_polling;

    ASSERT(pthis != NULL);

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        is_polling = pthis->is_polling;
        if (is_polling) {
            pthis->is_to_repoll = true;
        } else {
            pthis->is_polling = true;
        }
    vsf_sched_unlock(lock_status);

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
    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        pthis->value &= ~mask;
    vsf_sched_unlock(lock_status);

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

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        pthis->cancelled_value |= mask;
    vsf_sched_unlock(lock_status);

    __vsf_eda_bmpevt_cancel_start(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask)
{
    ASSERT(pthis != NULL);

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        pthis->value |= mask;
    vsf_sched_unlock(lock_status);

    __vsf_eda_bmpevt_poll_start(pthis);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_pend")
vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, int_fast32_t timeout)
{
    vsf_sched_lock_status_t orig;
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

    orig = vsf_sched_lock();
    if (pender->mask & pthis->cancelled_value) {
        vsf_sched_unlock(orig);
        return VSF_ERR_FAIL;
    }

    __vsf_dlist_add_to_tail_imp(&pthis->pending_list, &eda->pending_node);
    vsf_sched_unlock(orig);

    __vsf_eda_bmpevt_poll_start(pthis);

    if (timeout > 0) {
#if VSF_CFG_TIMER_EN == ENABLED
        eda->is_limitted = true;
        vsf_teda_set_timer(timeout);
#else
        ASSERT(false);
#endif
    }
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

SECTION(".text.vsf.kernel.vsf_eda_queue_init")
vsf_err_t vsf_eda_queue_init(vsf_queue_t *pthis, uint_fast16_t max)
{
    ASSERT((pthis != NULL) && !(max & 0x1000));
    vsf_slist_queue_init(&pthis->msgq);
    return vsf_eda_sync_init(&pthis->use_as__vsf_sync_t, 0, max);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
vsf_err_t vsf_eda_queue_send(vsf_queue_t *pthis, vsf_slist_node_t *node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sched_lock_status_t origlevel;
    vsf_err_t err;
    vsf_eda_t *eda;

    ASSERT((pthis != NULL) && (node != NULL));
    node->next = NULL;

    origlevel = vsf_sched_lock();
    if (!sync->max_value || (sync->max_value > sync->cur_value)) {
        sync->cur_value++;

        vsf_slist_node_t *node_tail = pthis->msgq.tail.next;
        pthis->msgq.tail.next = node;
        if (node_tail != NULL) {
            node_tail->next = node;
        } else {
            pthis->msgq.head.next = node;
        }

        while (1) {
            eda = __vsf_eda_sync_get_eda_pending(sync);
            if (eda != NULL) {
                eda->is_sync_got = true;

                vsf_sched_unlock(origlevel);
                err = vsf_eda_post_evt(eda, VSF_EVT_SYNC);
                eda->is_sync_got = false;
                if (!err) { return err; }
                origlevel = vsf_sched_lock();
            } else {
                break;
            }
        }

        vsf_sched_unlock(origlevel);
        return VSF_ERR_NONE;
    } else {
        vsf_sched_unlock(origlevel);
        return VSF_ERR_FAIL;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
vsf_err_t vsf_eda_queue_recv(vsf_queue_t *pthis, vsf_slist_node_t **node, int_fast32_t timeout)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sched_lock_status_t origlevel;
    vsf_eda_t *eda;

    ASSERT((pthis != NULL) && (node != NULL));

    origlevel = vsf_sched_lock();

    *node = pthis->msgq.head.next;
    if (*node != NULL) {
        pthis->msgq.head.next = (*node)->next;
        if (NULL == pthis->msgq.head.next) {
            pthis->msgq.tail.next = NULL;
        }
        (*node)->next = NULL;

        ASSERT(sync->cur_value > 0);
        sync->cur_value--;
        vsf_sched_unlock(origlevel);
        return VSF_ERR_NONE;
    } else if (timeout != 0) {
        eda = vsf_eda_get_cur();
        ASSERT((eda != NULL) && vsf_dlist_is_empty(&sync->pending_list));
        __vsf_eda_sync_pend(sync, eda);
        vsf_sched_unlock(origlevel);

        if (timeout > 0) {
#if VSF_CFG_TIMER_EN == ENABLED
            eda->is_limitted = true;
            vsf_teda_set_timer(timeout);
#else
            ASSERT(false);
#endif
        }
    } else {
        vsf_sched_unlock(origlevel);
    }

    return VSF_ERR_NOT_READY;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_get_reason")
vsf_sync_reason_t vsf_eda_queue_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, vsf_slist_node_t **node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sync, evt);

    if (reason == VSF_SYNC_GET) {
        vsf_sched_lock_status_t lock_status = vsf_sched_lock();
            *node = pthis->msgq.head.next;
            if (*node != NULL) {
                pthis->msgq.head.next = (*node)->next;
                if (NULL == pthis->msgq.head.next) {
                    pthis->msgq.tail.next = NULL;
                }
                (*node)->next = NULL;

                ASSERT(sync->cur_value > 0);
                sync->cur_value--;
            }
        vsf_sched_unlock(lock_status);
    }
    return reason;
}
#endif      // VSF_CFG_QUEUE_EN
#endif      // VSF_CFG_SYNC_EN

#if VSF_CFG_TIMER_EN == ENABLED
SECTION(".text.vsf.kernel.teda")
static void vsf_teda_timer_enqueue(vsf_teda_t *pthis, vsf_timer_tick_t due)
{
    ASSERT((pthis != NULL) && !pthis->is_timed);
    pthis->due = due;

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        vsf_timq_insert(&__vsf_eda.timer.timq, pthis);
        pthis->is_timed = true;
    vsf_sched_unlock(lock_status);
}

static void vsf_timer_wakeup(void)
{
    if (!__vsf_eda.timer.processing) {
        __vsf_eda.timer.processing = true;
#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
        if (vsf_eda_post_evt(&__vsf_eda.timer.teda.use_as__vsf_eda_t, VSF_EVT_TIMER)) {
#else
        if (vsf_eda_post_evt(&__vsf_eda.timer.eda, VSF_EVT_TIMER)) {
#endif
            __vsf_eda.timer.processing = false;
        }
    }
}

static void vsf_timer_update(void)
{
    vsf_teda_t *teda;
    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        if (NULL == teda) {
            vsf_systimer_set_idle();
        } else if (teda->due != __vsf_eda.timer.pre_tick) {
            __vsf_eda.timer.pre_tick = teda->due;
            if (!vsf_systimer_set(teda->due)) {
                vsf_timer_wakeup();
            }
        }
    vsf_sched_unlock(lock_status);
}

SECTION(".text.vsf.kernel.teda")
static vsf_err_t vsf_teda_set_timer_imp(vsf_teda_t *pthis, vsf_timer_tick_t due)
{
    vsf_teda_timer_enqueue(pthis, due);
    vsf_timer_update();
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.teda")
static void __vsf_timer_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_teda_t *teda;
    vsf_sched_lock_status_t origlevel;
    bool timed = false;

    ASSERT(eda != NULL);

    switch (evt) {
    case VSF_EVT_INIT:
        break;
    case VSF_EVT_TIMER:
        origlevel = vsf_sched_lock();
        vsf_timq_peek(&__vsf_eda.timer.timq, teda);

        while ((teda != NULL) && vsf_systimer_is_due(teda->due)) {
            timed = true;
            vsf_timq_dequeue(&__vsf_eda.timer.timq, teda);

            teda->is_timed = false;
            vsf_sched_unlock(origlevel);

#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
            if (teda == &__vsf_eda.timer.teda) {
                vsf_callback_timer_t *timer;

                origlevel = vsf_sched_lock();
                vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                while ((timer != NULL) && vsf_systimer_is_due(timer->due)) {
                    vsf_callback_timq_dequeue(&__vsf_eda.timer.callback_timq, timer);
                    vsf_sched_unlock(origlevel);

                    if (timer->on_timer != NULL) {
                        timer->on_timer(timer->param);
                    }

                    origlevel = vsf_sched_lock();
                    vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
                }

                if ((timer != NULL) && !eda->is_timed) {
                    vsf_teda_timer_enqueue((vsf_teda_t *)eda, timer->due);
                }
                vsf_sched_unlock(origlevel);
            } else
#endif
            {
                vsf_eda_post_evt(&teda->use_as__vsf_eda_t, VSF_EVT_TIMER);
            }

            origlevel = vsf_sched_lock();
            vsf_timq_peek(&__vsf_eda.timer.timq, teda);
        }
        vsf_sched_unlock(origlevel);
        __vsf_eda.timer.processing = false;
        if (timed) {
            vsf_timer_update();
        }
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
#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq);
#endif

#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
    __vsf_eda.timer.teda.evthandler = __vsf_timer_evthandler;
    return vsf_eda_init(&__vsf_eda.timer.teda.use_as__vsf_eda_t, vsf_priority_highest, false);
#else
    __vsf_eda.timer.eda.evthandler = __vsf_timer_evthandler;
    return vsf_eda_init(&__vsf_eda.timer.eda, vsf_priority_highest, false);
#endif
}

#if VSF_CFG_CALLBACK_TIMER_EN == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick)
{
    vsf_timer_tick_t due = tick + vsf_timer_get_tick();
    ASSERT(timer != NULL);

    timer->due = due;

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        vsf_callback_timq_insert(&__vsf_eda.timer.callback_timq, timer);

        if (NULL == timer->timer_node.prev) {
            vsf_teda_cancel_timer(&__vsf_eda.timer.teda);

            vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
            vsf_teda_set_timer_imp(&__vsf_eda.timer.teda, timer->due);
        }
    vsf_sched_unlock(lock_status);
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

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_teda_fini(vsf_teda_t *pthis)
{
    if (NULL == pthis) {
        pthis = (vsf_teda_t *)vsf_eda_get_cur();
    }
    ASSERT(pthis != NULL);
    vsf_teda_cancel_timer(pthis);
    return vsf_eda_fini(&pthis->use_as__vsf_eda_t);
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
    if (NULL == pthis) {
        pthis = (vsf_teda_t *)vsf_eda_get_cur();
    }

    ASSERT(pthis != NULL);

    vsf_sched_lock_status_t lock_status = vsf_sched_lock();
        if (pthis->is_timed) {
            vsf_timq_remove(&__vsf_eda.timer.timq, pthis);
            pthis->is_timed = false;
        }
    vsf_sched_unlock(lock_status);
    return VSF_ERR_NONE;
}

#endif
