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

#ifdef VSF_CFG_EVTQ_LIST

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

extern vsf_evtq_t * __vsf_os_evtq_get(vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_set_priority(vsf_evtq_t *pthis, vsf_priority_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);
extern vsf_priority_t __vsf_os_evtq_get_prio(vsf_evtq_t *pthis);

extern vsf_evt_node_t * __vsf_os_alloc_evt_node(void);
extern void __vsf_os_free_evt_node(vsf_evt_node_t *node);

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsf_eda_update_priotiry(vsf_eda_t *pthis, vsf_priority_t priority)
{

    if (pthis->is_ready) {
        vsf_evtq_t *evtq = __vsf_os_evtq_get((vsf_priority_t)pthis->cur_priority);
        vsf_dlist_remove(
                vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);

        evtq = __vsf_os_evtq_get(priority);
        vsf_dlist_queue_enqueue(vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_eda_set_priority(vsf_eda_t *pthis, vsf_priority_t priority)
{
    vsf_evtq_t *evtq;

    ASSERT(pthis != NULL);

    vsf_interrupt_safe(){
        evtq = __vsf_os_evtq_get((vsf_priority_t)pthis->cur_priority);
        ASSERT(evtq != NULL);
        if (pthis->cur_priority != priority) {
            if ((evtq->cur.eda != pthis) && !pthis->is_new_prio) {
                __vsf_eda_update_priotiry(pthis, priority);
                pthis->cur_priority = priority;
            } else {
                pthis->is_new_prio = true;
                pthis->new_priority = priority;
                if (priority > pthis->cur_priority) {
                    __vsf_os_evtq_set_priority(evtq, priority);
                }
            }
        }
    }
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
    istate_t orig;

    orig = vsf_disable_interrupt();
    evtq = __vsf_os_evtq_get((vsf_priority_t)pthis->cur_priority);
    if (evtq->cur.eda == pthis) {
        pthis->is_to_exit = true;
        vsf_set_interrupt(orig);
        return;
    } else if (pthis->is_ready) {
        vsf_dlist_remove(vsf_eda_t, rdy_node,
                &evtq->rdy_list,
                pthis);
    }
    vsf_set_interrupt(orig);

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
    ASSERT(pthis != NULL);
    pthis->cur.eda = NULL;
    pthis->cur.evt = VSF_EVT_INVALID;
    pthis->cur.msg = NULL;
    vsf_dlist_init(&pthis->rdy_list);
    return __vsf_os_evtq_init(pthis);
}

#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, vsf_evt_t evt, void *msg, bool force)
#else
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, uint_fast32_t value, bool force)
#endif
{
    vsf_evt_node_t *node;
    vsf_evtq_t *evtq;
    vsf_err_t err;
    istate_t orig;

    ASSERT(eda != NULL);

    node = __vsf_os_alloc_evt_node();
    if (NULL == node) {
        ASSERT(false);
//        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    vsf_slist_init_node(vsf_evtq_node_t, use_as__vsf_slist_node_t, node);
#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
    node->evt = evt;
    node->msg = msg;
#else
    node->value = value;
#endif

    orig = vsf_disable_interrupt();
    if (eda->is_limitted && eda->is_ready && !force) {
        vsf_set_interrupt(orig);
        __vsf_os_free_evt_node(node);
        return VSF_ERR_FAIL;
    }

    vsf_slist_queue_enqueue(vsf_evt_node_t, use_as__vsf_slist_node_t, &eda->evt_list, node);
    evtq = __vsf_os_evtq_get((vsf_priority_t)eda->cur_priority);
    if (!eda->is_ready) {
        eda->is_ready = true;
        vsf_dlist_queue_enqueue(vsf_eda_t, rdy_node,
                    &evtq->rdy_list,
                    eda);
    }
    err = __vsf_os_evtq_activate(evtq);
    vsf_set_interrupt(orig);

    return err;
}

vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *pthis, vsf_evt_t evt, bool force)
{
#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
    return __vsf_evtq_post(pthis, evt, NULL, force);
#else
    return __vsf_evtq_post(pthis, (uint32_t)((evt << 1) | 1), force);
#endif
}

vsf_err_t vsf_evtq_post_evt(vsf_eda_t *pthis, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(pthis, evt, false);
}

vsf_err_t vsf_evtq_post_msg(vsf_eda_t *pthis, void *msg)
{
#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
    return __vsf_evtq_post(pthis, VSF_EVT_MESSAGE, msg, false);
#else
    return __vsf_evtq_post(pthis, (uint32_t)msg, false);
#endif
}

vsf_err_t vsf_evtq_poll(vsf_evtq_t *pthis)
{
    vsf_evtq_t *evtq_orig;
    vsf_evt_node_t *node_evt;
    vsf_dlist_node_t *node_eda;
    vsf_eda_t *eda;
    istate_t orig;

    ASSERT(pthis != NULL);

    evtq_orig = __vsf_set_cur_evtq(pthis);
    while (!vsf_dlist_is_empty(&pthis->rdy_list)) {
        orig = vsf_disable_interrupt();
        node_eda = pthis->rdy_list.head;
        while (node_eda != NULL) {
            pthis->cur.eda = (vsf_eda_t *)__vsf_dlist_get_host(vsf_eda_t, rdy_node, node_eda);
            eda = pthis->cur.eda;
            vsf_slist_queue_dequeue(
                    vsf_evt_node_t, use_as__vsf_slist_node_t,
                    &eda->evt_list,
                    node_evt);
            vsf_set_interrupt(orig);

#if VSF_CFG_EVT_MESSAGE_EN == ENABLED
            pthis->evt_cur = node->evt;
            pthis->msg_cur = node->msg;
#else
            uint_fast32_t value = node_evt->value;
            if (value & 1) {
                pthis->cur.evt = (vsf_evt_t)(value >> 1);
                pthis->cur.msg = NULL;
            } else {
                pthis->cur.evt = VSF_EVT_MESSAGE;
                pthis->cur.msg = (void *)value;
            }
#endif
            __vsf_os_free_evt_node(node_evt);
            if (!eda->is_to_exit) {
                __vsf_dispatch_evt(eda, pthis->cur.evt);
            }
            pthis->cur.evt = VSF_EVT_INVALID;
            pthis->cur.msg = NULL;

            orig = vsf_disable_interrupt();
            node_eda = eda->rdy_node.next;
            if (NULL == eda->evt_list.head.next) {
                eda->is_ready = false;
                vsf_dlist_remove(
                    vsf_eda_t, rdy_node,
                    &pthis->rdy_list,
                    eda);
            }
            pthis->cur.eda = NULL;

            if (!eda->is_to_exit) {
                if (eda->is_new_prio) {
                    bool is_prio_rise = eda->new_priority > eda->cur_priority;
                    __vsf_eda_update_priotiry(eda, (vsf_priority_t)eda->new_priority);
                    eda->cur_priority = eda->new_priority;
                    eda->is_new_prio = false;
                    if (is_prio_rise) {
                        __vsf_os_evtq_set_priority(pthis, (vsf_priority_t)eda->priority);
                    }
                }
            } else {
                vsf_set_interrupt(orig);
                vsf_evtq_on_eda_fini(eda);
                orig = vsf_disable_interrupt();
            }
        }
        vsf_set_interrupt(orig);
    }
    __vsf_set_cur_evtq(evtq_orig);
    return VSF_ERR_NONE;
}

#endif
