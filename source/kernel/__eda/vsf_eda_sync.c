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
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
extern vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *pthis);

/*============================ IMPLEMENTATION ================================*/


#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED

/*-----------------------------------------------------------------------------*
 * vsf_sync_t for critical_section, semaphore, event                           *
 *-----------------------------------------------------------------------------*/

SECTION(".text.vsf.kernel.vsf_sync")
vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, vsf_timeout_tick_t timeout)
{
    eda = __vsf_eda_get_valid_eda(eda);

    if (timeout > 0) {
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        eda->flag.state.is_limitted = true;
        vsf_teda_set_timer_ex((vsf_teda_t *)eda, timeout);
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
void __vsf_eda_sync_pend(vsf_sync_t *sync, vsf_eda_t *eda, vsf_timeout_tick_t timeout)
{
    eda = __vsf_eda_get_valid_eda(eda);
    VSF_KERNEL_ASSERT(eda != NULL);

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

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.vsf_sync")
static vsf_sync_reason_t __vsf_eda_sync_get_reason( vsf_sync_t *sync,
                                                    vsf_evt_t evt,
                                                    bool dequeue_eda)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    vsf_sync_reason_t reason = VSF_SYNC_FAIL;

    VSF_KERNEL_ASSERT((sync != NULL) && (eda != NULL));

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    if (evt == VSF_EVT_TIMER) {
        vsf_protect_t origlevel = vsf_protect_sched();
        if (eda->flag.state.is_sync_got) {
            vsf_unprotect_sched(origlevel);
            return VSF_SYNC_PENDING;
        }
        if (dequeue_eda) {
            vsf_dlist_remove(
                vsf_eda_t, pending_node,
                &sync->pending_list,
                eda);
        }
        vsf_unprotect_sched(origlevel);
        reason = VSF_SYNC_TIMEOUT;
    } else {
        __vsf_teda_cancel_timer((vsf_teda_t *)eda);
#else
    {
#endif
        if (evt == VSF_EVT_SYNC) {
            reason = VSF_SYNC_GET;
            eda->flag.state.is_sync_got = false;
        } else if (evt == VSF_EVT_SYNC_CANCEL) {
            reason = VSF_SYNC_CANCEL;
        } else {
            VSF_KERNEL_ASSERT(false);
        }
    }
    eda->flag.state.is_limitted = false;
    return reason;
}

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

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

#if VSF_SYNC_CFG_SUPPORT_ISR == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase_isr(vsf_sync_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return vsf_eda_post_msg((vsf_eda_t *)&__vsf_eda.task, pthis);
}
#endif          // VSF_SYNC_CFG_SUPPORT_ISR

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t __vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda, bool manual)
{
    vsf_protect_t origlevel;
    vsf_eda_t *eda_pending;

    VSF_KERNEL_ASSERT(pthis != NULL);

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
        } else if (eda->flag.state.is_new_prio) {
            vsf_evtq_t *evtq;
            // assert new_priority is a priority boost
            VSF_KERNEL_ASSERT(eda->new_priority > eda->priority);
            eda->flag.state.is_new_prio = false;

            evtq = __vsf_os_evtq_get((vsf_prio_t)eda->priority);
            VSF_KERNEL_ASSERT(evtq != NULL);
            __vsf_os_evtq_set_priority(evtq, eda->priority);
        }
    }
#endif

    while (1) {
        if (pthis->cur_union.bits.cur > 0) {
            eda_pending = __vsf_eda_sync_get_eda_pending(pthis);
            if (eda_pending != NULL) {
                eda_pending->flag.state.is_sync_got = true;
                if (!manual) {
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
                vsf_err_t err = __vsf_eda_post_evt_ex(eda_pending, VSF_EVT_SYNC, true);
                VSF_KERNEL_ASSERT(!err);
                VSF_UNUSED_PARAM(err);
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
vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda)
{
    return __vsf_eda_sync_increase_ex(pthis, eda, pthis->max_union.bits.manual_rst);
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
vsf_err_t __vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, vsf_timeout_tick_t timeout, vsf_eda_t *eda, bool manual)
{
    vsf_protect_t origlevel;

    VSF_KERNEL_ASSERT(pthis != NULL);

    eda = __vsf_eda_get_valid_eda(eda);

    origlevel = vsf_protect_sched();
    if ((pthis->cur_union.bits.cur > 0) && vsf_dlist_is_empty(&pthis->pending_list)) {
        if (!manual) {
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
vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, vsf_timeout_tick_t timeout, vsf_eda_t *eda)
{
    return __vsf_eda_sync_decrease_ex(pthis, timeout, eda, pthis->max_union.bits.manual_rst);
}

SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, vsf_timeout_tick_t timeout)
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
            eda->flag.state.is_sync_got = true;
            vsf_unprotect_sched(origlevel);
            __vsf_eda_post_evt_ex(eda, VSF_EVT_SYNC_CANCEL, true);
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


#endif


#endif
