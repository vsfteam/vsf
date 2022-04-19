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

#if VSF_USE_KERNEL == ENABLED && defined(__EDA_GADGET__)

/*============================ MACROS ========================================*/

#if VSF_EDA_QUEUE_CFG_REGION != ENABLED
#   if VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED
#       define __vsf_eda_queue_protect              vsf_protect(interrupt)
#       define __vsf_eda_queue_unprotect            vsf_unprotect(interrupt)
#   else
#       define __vsf_eda_queue_protect              vsf_protect(scheduler)
#       define __vsf_eda_queue_unprotect            vsf_unprotect(scheduler)
#   endif
#else
#   define vsf_protect_eda_queue_region()           pthis->region->enter()
#   define vsf_unprotect_eda_queue_region(__orig)   pthis->region->leave(__orig)
#   define __vsf_eda_queue_protect                  vsf_protect(eda_queue_region)
#   define __vsf_eda_queue_unprotect                vsf_unprotect(eda_queue_region)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED && VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_eda_queue_t                                                             *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_eda_queue")
static bool __vsf_eda_queue_dequeue(vsf_eda_queue_t *pthis, void **node)
{
    bool is_dequeued = false;
    vsf_protect_t origlevel = __vsf_eda_queue_protect();
    if (pthis->cur_union.cur_value > 0) {
        pthis->cur_union.cur_value--;
        pthis->op.dequeue(pthis, node);
        is_dequeued = true;
    }
    __vsf_eda_queue_unprotect(origlevel);
    return is_dequeued;
}

SECTION(".text.vsf.kernel.vsf_eda_queue")
static bool __vsf_eda_queue_enqueue(vsf_eda_queue_t *pthis, void *node)
{
    bool is_enqueued = false;
    vsf_protect_t origlevel = __vsf_eda_queue_protect();
    if (pthis->cur_union.cur_value < pthis->max_union.max_value) {
        pthis->cur_union.cur_value++;
        pthis->op.enqueue(pthis, node);
        is_enqueued = true;
    }
    __vsf_eda_queue_unprotect(origlevel);
    return is_enqueued;
}

SECTION(".text.vsf.kernel.vsf_eda_queue")
void __vsf_eda_queue_notify(vsf_eda_queue_t *pthis, bool tx, vsf_protect_t orig)
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
#if VSF_KERNEL_CFG_QUEUE_HAS_RX_NOTIFIED == ENABLED
            pthis->rx_notified = true;
#endif
        }

        eda->flag.state.is_sync_got = true;
        vsf_unprotect_sched(orig);

        vsf_evtq_post_evt_ex(eda, VSF_EVT_SYNC, true);
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
vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *pthis, uint_fast16_t max)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (max > 0));
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
    pthis->tx_processing = false;
#endif
    pthis->eda_rx = NULL;
#if VSF_EDA_QUEUE_CFG_REGION == ENABLED
    if (NULL == pthis->region) {
        pthis->region = (vsf_protect_region_t *)&vsf_protect_region_sched;
    }
#endif
#if VSF_KERNEL_CFG_QUEUE_HAS_RX_NOTIFIED == ENABLED
    pthis->rx_notified = false;
#endif
    return vsf_eda_sync_init(&pthis->use_as__vsf_sync_t, 0, max);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
vsf_err_t vsf_eda_queue_send_ex(vsf_eda_queue_t *pthis, void *node, vsf_timeout_tick_t timeout, vsf_eda_t *eda)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    bool is_enqueued = __vsf_eda_queue_enqueue(pthis, node);
    vsf_protect_t origlevel = vsf_protect_sched();
    if (is_enqueued) {
        __vsf_eda_queue_notify(pthis, false, origlevel);
        return VSF_ERR_NONE;
    } else {
        if (timeout != 0) {
            vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
            __vsf_eda_sync_pend(sync, eda, timeout);
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NOT_READY;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *pthis, void *node, vsf_timeout_tick_t timeout)
{
    return vsf_eda_queue_send_ex(pthis, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void *node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sync, evt);
    if (VSF_SYNC_GET == reason) {
        bool is_enqueued = __vsf_eda_queue_enqueue(pthis, node);
        vsf_protect_t origlevel = vsf_protect_sched();
        if (is_enqueued) {
            __vsf_eda_queue_notify(pthis, false, origlevel);

#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            origlevel = vsf_protect_sched();
            if (sync->cur_union.cur_value < sync->max) {
                __vsf_eda_queue_notify(pthis, true, origlevel);
                return reason;
            } else {
                pthis->tx_processing = false;
            }
#else
            return reason;
#endif
        } else {
            vsf_unprotect_sched(origlevel);

            // TODO: re-send again?
            VSF_KERNEL_ASSERT(false);
        }
    }
    return reason;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
vsf_err_t vsf_eda_queue_recv_ex(vsf_eda_queue_t *pthis, void **node, vsf_timeout_tick_t timeout, vsf_eda_t *eda)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    bool is_dequeued = __vsf_eda_queue_dequeue(pthis, node);
    vsf_protect_t origlevel = vsf_protect_sched();
    if (is_dequeued) {
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
vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *pthis, void **node, vsf_timeout_tick_t timeout)
{
    return vsf_eda_queue_recv_ex(pthis, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void **node)
{
    vsf_sync_t *sync = &pthis->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = __vsf_eda_sync_get_reason(sync, evt, false);
    if (VSF_SYNC_GET == reason) {
        bool is_dequeued = __vsf_eda_queue_dequeue(pthis, node);
        vsf_protect_t origlevel = vsf_protect_sched();
        if (is_dequeued) {
#if VSF_KERNEL_CFG_QUEUE_HAS_RX_NOTIFIED == ENABLED
            pthis->rx_notified = false;
#endif
            __vsf_eda_queue_notify(pthis, true, origlevel);
            return reason;
        }
        // this happens when queue is read by other task with 0 timeout after VSF_SYNC_GET is sent to here
        // TODO: re-recv again?
        reason = VSF_SYNC_PENDING;
    } else if (VSF_SYNC_TIMEOUT == reason) {
        pthis->eda_rx = NULL;
    }
    return reason;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_get_cnt")
uint_fast16_t vsf_eda_queue_get_cnt(vsf_eda_queue_t *pthis)
{
    uint_fast16_t cnt;
    vsf_protect_t origlevel = __vsf_eda_queue_protect();
    cnt = pthis->cur_union.cur_value;
    __vsf_eda_queue_unprotect(origlevel);
    return cnt;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_cancel")
void vsf_eda_queue_cancel(vsf_eda_queue_t *pthis)
{
    vsf_eda_sync_cancel(&pthis->use_as__vsf_sync_t);
    if (pthis->eda_rx != NULL) {
        vsf_eda_t *eda = pthis->eda_rx;
        pthis->eda_rx = NULL;
        __vsf_eda_post_evt_ex(eda, VSF_EVT_SYNC_CANCEL, true);
    }
}

#if VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_queue_send_isr")
vsf_err_t vsf_eda_queue_send_isr(vsf_eda_queue_t *pthis, void *node)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    if (!__vsf_eda_queue_enqueue(pthis, node)) {
        return VSF_ERR_NOT_READY;
    }
    return vsf_eda_post_evt_msg((vsf_eda_t *)&__vsf_eda.task, VSF_KERNEL_EVT_QUEUE_SEND_NOTIFY, pthis);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_isr")
vsf_err_t vsf_eda_queue_recv_isr(vsf_eda_queue_t *pthis, void **node)
{
    VSF_KERNEL_ASSERT((pthis != NULL) && (node != NULL));

    if (!__vsf_eda_queue_dequeue(pthis, node)) {
        return VSF_ERR_NOT_READY;
    }
    return vsf_eda_post_evt_msg((vsf_eda_t *)&__vsf_eda.task, VSF_KERNEL_EVT_QUEUE_RECV_NOTIFY, pthis);
}
#endif

#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC && VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE
#endif      // VSF_USE_KERNEL == ENABLED && __EDA_GADGET__
