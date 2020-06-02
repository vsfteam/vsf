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

#if VSF_USE_KERNEL == ENABLED && defined(__EDA_GADGET__)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

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
    if (pender->op == VSF_BMPEVT_AND) {
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
            vsf_protect_t lock_status = vsf_protect_sched();
                is_to_repoll = pthis->state.bits.is_to_repoll;
                if (!is_to_repoll) {
                    pthis->state.bits.is_polling = false;
                }
            vsf_unprotect_sched(lock_status);

            if (is_to_repoll) {
                goto repoll;
            }
        }
    } else {
        while (node != NULL) {
            if (is_poll_event && pthis->state.bits.is_to_repoll) {
            repoll:
                pthis->state.bits.is_to_repoll = false;
                __vsf_eda_bmpevt_post_evt(pthis, pthis->pending_list.head, evt);
                return;
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

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_poll")
vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, vsf_evt_t evt)
{
    vsf_sync_reason_t reason = VSF_SYNC_FAIL;
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

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif


#endif      // VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT

#endif


#endif
