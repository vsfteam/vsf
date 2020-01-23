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

#if VSF_USE_KERNEL == ENABLED

#include "./vsf_kernel_common.h"

#include "./vsf_eda.h"
#include "./vsf_evtq.h"

#include "./vsf_os.h"

#ifdef __VSF_OS_CFG_EVTQ_LIST

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
extern void vsf_eda_on_terminate(vsf_eda_t *pthis);

SECTION(".text.vsf.kernel.__vsf_set_cur_evtq")
extern vsf_evtq_t * __vsf_set_cur_evtq(vsf_evtq_t *evtq);

SECTION(".text.vsf.kernel.eda")
extern void __vsf_dispatch_evt(vsf_eda_t *pthis, vsf_evt_t evt);

extern vsf_evtq_t * __vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_prio_t __vsf_os_evtq_get_prio(vsf_evtq_t *pthis);

extern vsf_evt_node_t * __vsf_os_alloc_evt_node(void);
extern void __vsf_os_free_evt_node(vsf_evt_node_t *node);

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_eda_update_priotiry(vsf_eda_t *pthis, vsf_prio_t priority)
{
    if (pthis->state.bits.is_ready) {
        vsf_evtq_t *evtq = __vsf_os_evtq_get((vsf_prio_t)pthis->cur_priority);
        vsf_dlist_remove(
                vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);

        evtq = __vsf_os_evtq_get(priority);
        vsf_dlist_queue_enqueue(vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);
    }
    pthis->cur_priority = priority;
    return VSF_ERR_NONE;
}

vsf_prio_t __vsf_eda_get_cur_priority(vsf_eda_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    return (vsf_prio_t)(pthis->state.bits.is_new_prio ? pthis->new_priority : pthis->cur_priority);
}

vsf_err_t __vsf_eda_set_priority(vsf_eda_t *pthis, vsf_prio_t priority)
{
    VSF_KERNEL_ASSERT(pthis != NULL);

    vsf_protect_t orig = vsf_protect_int();
        if (pthis->cur_priority != priority) {
            vsf_evtq_t *evtq = __vsf_os_evtq_get((vsf_prio_t)pthis->cur_priority);
            VSF_KERNEL_ASSERT(evtq != NULL);

            if ((evtq->cur.eda != pthis) && !pthis->state.bits.is_new_prio) {
                __vsf_eda_update_priotiry(pthis, priority);
            } else {
                pthis->state.bits.is_new_prio = true;
                pthis->new_priority = priority;
                if (priority > pthis->cur_priority) {
                    __vsf_os_evtq_set_priority(evtq, priority);
                }
            }
        }
    vsf_unprotect_int(orig);
    return VSF_ERR_NONE;
}

void vsf_evtq_on_eda_init(vsf_eda_t *pthis)
{
    pthis->cur_priority = pthis->priority;
    vsf_slist_queue_init(&pthis->evt_list);
}

void vsf_evtq_on_eda_fini(vsf_eda_t *pthis)
{
    vsf_evt_node_t *node;
    vsf_evtq_t *evtq;
    vsf_protect_t orig;

    orig = vsf_protect_int();
    evtq = __vsf_os_evtq_get((vsf_prio_t)pthis->cur_priority);
    if (evtq->cur.eda == pthis) {
        pthis->state.bits.is_to_exit = true;
        vsf_unprotect_int(orig);
        return;
    } else if (pthis->state.bits.is_ready) {
        vsf_dlist_remove(vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);
    }
    vsf_unprotect_int(orig);

    while (!vsf_slist_queue_is_empty(&pthis->evt_list)) {
        vsf_slist_queue_dequeue(
                vsf_evt_node_t, use_as__vsf_slist_node_t,
                &pthis->evt_list,
                node);

        __vsf_os_free_evt_node(node);
    }

    vsf_eda_on_terminate(pthis);
}

vsf_err_t vsf_evtq_init(vsf_evtq_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    pthis->cur.eda = NULL;
    pthis->cur.evt = VSF_EVT_INVALID;
    pthis->cur.msg = NULL;
    vsf_dlist_init(&pthis->rdy_list);
    return __vsf_os_evtq_init(pthis);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, vsf_evt_t evt, void *msg, bool force)
#else
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, uintptr_t value, bool force)
#endif
{
    vsf_evt_node_t *node;
    vsf_evtq_t *evtq;
    vsf_err_t err;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(eda != NULL);

    node = __vsf_os_alloc_evt_node();
    if (NULL == node) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    vsf_slist_init_node(vsf_evtq_node_t, use_as__vsf_slist_node_t, node);
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    node->evt = evt;
    node->msg = msg;
#else
    node->evt_union.value = value;
#endif

    orig = vsf_protect_int();
    if (eda->state.bits.is_limitted && eda->state.bits.is_ready && !force) {
        vsf_unprotect_int(orig);
        __vsf_os_free_evt_node(node);
        return VSF_ERR_FAIL;
    }

    vsf_slist_queue_enqueue(vsf_evt_node_t, use_as__vsf_slist_node_t, &eda->evt_list, node);
    evtq = __vsf_os_evtq_get((vsf_prio_t)eda->cur_priority);
    if (!eda->state.bits.is_ready) {
        eda->state.bits.is_ready = true;
        vsf_dlist_queue_enqueue(vsf_eda_t, rdy_node,
                    &evtq->rdy_list,
                    eda);
    }
    err = __vsf_os_evtq_activate(evtq);
    vsf_unprotect_int(orig);

    return err;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    return __vsf_evtq_post(pthis, evt, NULL, force);
#else
    return __vsf_evtq_post(pthis, (uintptr_t)((evt << 1) | 1), force);
#endif
}

vsf_err_t vsf_evtq_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(pthis, evt, false);
}

vsf_err_t vsf_evtq_post_msg(vsf_eda_t *pthis, void *msg)
{
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    return __vsf_evtq_post(pthis, VSF_EVT_MESSAGE, msg, false);
#else
    return __vsf_evtq_post(pthis, (uintptr_t)msg, false);
#endif
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
vsf_err_t vsf_evtq_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg)
{
    return __vsf_evtq_post(pthis, evt, msg, false);
}
#endif

vsf_err_t vsf_evtq_poll(vsf_evtq_t *pthis)
{
    vsf_evtq_t *evtq_orig;
    vsf_evt_node_t *node_evt;
    vsf_dlist_node_t *node_eda;
    vsf_eda_t *eda;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(pthis != NULL);

    evtq_orig = __vsf_set_cur_evtq(pthis);
    while (!vsf_dlist_is_empty(&pthis->rdy_list)) {
        orig = vsf_protect_int();
        node_eda = pthis->rdy_list.head;
        while (node_eda != NULL) {
            pthis->cur.eda = (vsf_eda_t *)__vsf_dlist_get_host(vsf_eda_t, rdy_node, node_eda);
            eda = pthis->cur.eda;
            vsf_slist_queue_dequeue(
                    vsf_evt_node_t, use_as__vsf_slist_node_t,
                    &eda->evt_list,
                    node_evt);
            vsf_unprotect_int(orig);

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
            pthis->cur.evt = node_evt->evt;
            pthis->cur.msg = (uintptr_t)node_evt->msg;
#else
            uintptr_t value = node_evt->evt_union.value;
            if (value & 1) {
                pthis->cur.evt = (vsf_evt_t)(value >> 1);
                pthis->cur.msg = NULL;
            } else {
                pthis->cur.evt = VSF_EVT_MESSAGE;
                pthis->cur.msg = value;
            }
#endif
            __vsf_os_free_evt_node(node_evt);
            if (!eda->state.bits.is_to_exit) {
                __vsf_dispatch_evt(eda, pthis->cur.evt);
            }
            pthis->cur.evt = VSF_EVT_INVALID;
            pthis->cur.msg = NULL;

            orig = vsf_protect_int();
            node_eda = eda->rdy_node.next;

            // remove current eda, and will enqueue again if more events pending
            // so the eda will always have an opportunity to run
            vsf_dlist_remove(
                    vsf_eda_t, rdy_node,
                    &pthis->rdy_list,
                    eda);
            if (NULL == eda->evt_list.head.next) {
                eda->state.bits.is_ready = false;
            } else {
                vsf_dlist_queue_enqueue(
                    vsf_eda_t, rdy_node,
                    &pthis->rdy_list,
                    eda);
            }
            pthis->cur.eda = NULL;

            if (eda->state.bits.is_new_prio) {
                eda->state.bits.is_new_prio = false;
                __vsf_eda_update_priotiry(eda, (vsf_prio_t)eda->new_priority);
                __vsf_os_evtq_set_priority(pthis, __vsf_os_evtq_get_prio(pthis));
            }
            if (eda->state.bits.is_to_exit) {
                vsf_unprotect_int(orig);
                vsf_evtq_on_eda_fini(eda);
                orig = vsf_protect_int();
            }
        }
        vsf_unprotect_int(orig);
    }
    __vsf_set_cur_evtq(evtq_orig);
    return VSF_ERR_NONE;
}

#endif

#endif
