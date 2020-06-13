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

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
/*-----------------------------------------------------------------------------*
 * vsf_eda_queue_t                                                                 *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_queue")
static void __vsf_eda_queue_notify(vsf_eda_queue_t *this_ptr, bool tx, vsf_protect_t orig)
{
    vsf_eda_t *eda = tx ?
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            __vsf_eda_sync_get_eda_pending(&this_ptr->use_as__vsf_sync_t)
#else
            this_ptr->eda_tx
#endif
        :   this_ptr->eda_rx;

    if (eda != NULL) {
        if (tx) {
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            this_ptr->tx_processing = true;
#else
            this_ptr->eda_tx = NULL;
#endif
        } else {
            this_ptr->eda_rx = NULL;
        }

        eda->state.bits.is_sync_got = true;
        vsf_unprotect_sched(orig);

        vsf_err_t err = vsf_eda_post_evt(eda, VSF_EVT_SYNC);
        VSF_KERNEL_ASSERT(!err);
        UNUSED_PARAM(err);
    } else {
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        if (tx) {
            this_ptr->tx_processing = false;
        }
#endif
        vsf_unprotect_sched(orig);
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_init")
vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *this_ptr, uint_fast16_t max)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
    this_ptr->tx_processing = false;
#endif
    this_ptr->eda_rx = NULL;
    return vsf_eda_sync_init(&this_ptr->use_as__vsf_sync_t, 0, max);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
vsf_err_t vsf_eda_queue_send_ex(vsf_eda_queue_t *this_ptr, void *node, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_sync_t *sync = &this_ptr->use_as__vsf_sync_t;
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT((this_ptr != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            vsf_dlist_is_empty(&this_ptr->use_as__vsf_sync_t.pending_list)
        &&
#endif
            (this_ptr->use_as__vsf_sync_t.cur_union.cur_value < this_ptr->use_as__vsf_sync_t.max_union.bits.max)) {
        this_ptr->use_as__vsf_sync_t.cur_union.cur_value++;
        this_ptr->op.enqueue(this_ptr, node);
        __vsf_eda_queue_notify(this_ptr, false, origlevel);
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
vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *this_ptr, void *node, int_fast32_t timeout)
{
    return vsf_eda_queue_send_ex(this_ptr, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *this_ptr, vsf_evt_t evt, void *node)
{
    vsf_sync_t *sync = &this_ptr->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = vsf_eda_sync_get_reason(sync, evt);

    vsf_protect_t origlevel = vsf_protect_sched();
    if (VSF_SYNC_GET == reason) {
        this_ptr->op.enqueue(this_ptr, node);
        sync->cur_union.cur_value++;
        __vsf_eda_queue_notify(this_ptr, false, origlevel);

#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        origlevel = vsf_protect_sched();
        if (sync->cur_value < sync->max) {
            __vsf_eda_queue_notify(this_ptr, true, origlevel);
            return reason;
        } else {
            this_ptr->tx_processing = false;
        }
#else
        return reason;
#endif
    }
    vsf_unprotect_sched(origlevel);
    return reason;
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
vsf_err_t vsf_eda_queue_recv_ex(vsf_eda_queue_t *this_ptr, void **node, int_fast32_t timeout, vsf_eda_t *eda)
{
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT((this_ptr != NULL) && (node != NULL));

    origlevel = vsf_protect_sched();
    if (this_ptr->use_as__vsf_sync_t.cur_union.cur_value > 0) {
        this_ptr->use_as__vsf_sync_t.cur_union.cur_value--;
        this_ptr->op.dequeue(this_ptr, node);
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
        if (!this_ptr->tx_processing) {
            __vsf_eda_queue_notify(this_ptr, true, origlevel);
            return VSF_ERR_NONE;
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NONE;
#else
        __vsf_eda_queue_notify(this_ptr, true, origlevel);
        return VSF_ERR_NONE;
#endif
    } else {
        if (timeout != 0) {
            VSF_KERNEL_ASSERT(NULL == this_ptr->eda_rx);
            this_ptr->eda_rx = __vsf_eda_set_timeout(eda, timeout);
        }
        vsf_unprotect_sched(origlevel);
        return VSF_ERR_NOT_READY;
    }
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *this_ptr, void **node, int_fast32_t timeout)
{
    return vsf_eda_queue_recv_ex(this_ptr, node, timeout, NULL);
}

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *this_ptr, vsf_evt_t evt, void **node)
{
    vsf_sync_t *sync = &this_ptr->use_as__vsf_sync_t;
    vsf_sync_reason_t reason = __vsf_eda_sync_get_reason(sync, evt, false);

    vsf_protect_t origlevel = vsf_protect_sched();
    if (VSF_SYNC_GET == reason) {
        this_ptr->op.dequeue(this_ptr, node);
        sync->cur_union.cur_value--;
        __vsf_eda_queue_notify(this_ptr, true, origlevel);
        return reason;
    } else if (VSF_SYNC_TIMEOUT == reason) {
        this_ptr->eda_rx = NULL;
    }
    vsf_unprotect_sched(origlevel);
    return reason;
}
#endif      // __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE

#endif


#endif
