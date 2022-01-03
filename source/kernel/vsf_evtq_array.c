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

#if VSF_USE_KERNEL == ENABLED

#include "./vsf_kernel_common.h"

#define __VSF_EDA_CLASS_IMPLEMENT
#include "./vsf_eda.h"
#include "./vsf_evtq.h"

#include "./vsf_os.h"

#ifdef __VSF_OS_CFG_EVTQ_ARRAY

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_evtq_t *__vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *pthis);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *pthis);

/*============================ IMPLEMENTATION ================================*/

void vsf_evtq_on_eda_init(vsf_eda_t *pthis)
{
    pthis->evt_cnt = 0;
}

static bool __vsf_eda_terminate(vsf_eda_t *pthis)
{
    bool terminate;

    VSF_KERNEL_ASSERT(pthis != NULL);

    terminate = !pthis->evt_cnt;
    if (terminate) {
        __vsf_eda_on_terminate(pthis);
    }
    return terminate;
}


void vsf_evtq_on_eda_fini(vsf_eda_t *pthis)
{
    if (!__vsf_eda_terminate((vsf_eda_t *)pthis)) {
        pthis->flag.state.is_to_exit = true;
    }
}

vsf_err_t vsf_evtq_init(vsf_evtq_t *pthis)
{
    VSF_KERNEL_ASSERT(pthis != NULL);
    pthis->cur.eda = NULL;
    pthis->cur.evt = VSF_EVT_INVALID;
    pthis->cur.msg = (uintptr_t)NULL;
#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED
    pthis->cur.is_timing = false;
#endif
    pthis->head = 0;
    pthis->tail = 0;
    return __vsf_os_evtq_init(pthis);
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, vsf_evt_t evt, void *msg, bool force)
#else
static vsf_err_t __vsf_evtq_post(vsf_eda_t *eda, uintptr_t value, bool force)
#endif
{
    vsf_evtq_t *evtq;
    uint_fast8_t tail, tail_next, mask;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(eda != NULL);
    evtq = __vsf_os_evtq_get((vsf_prio_t)eda->priority);
    VSF_KERNEL_ASSERT(evtq != NULL);
    mask = (1 << evtq->bitsize) - 1;

    orig = vsf_protect_int();

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    if (eda->evt_cnt && eda->flag.state.is_limitted && !force) {
        vsf_unprotect_int(orig);
        return VSF_ERR_FAIL;
    }
#endif

    tail = evtq->tail;
    tail_next = (tail + 1) & mask;
    if (tail_next == evtq->head) {
        vsf_unprotect_int(orig);
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    evtq->tail = tail_next;
    evtq->node[tail].eda = eda;
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    evtq->node[tail].evt = evt;
    evtq->node[tail].msg = msg;
#else
    evtq->node[tail].evt_union.value = value;
#endif

#if VSF_KERNEL_CFG_TRACE == ENABLED
    if (!eda->evt_cnt) {
        vsf_kernel_trace_eda_ready(eda);
    }
#endif

    eda->evt_cnt++;
    vsf_unprotect_int(orig);

    return __vsf_os_evtq_activate(evtq);
}

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

bool vsf_evtq_is_empty(vsf_evtq_t *pthis)
{
    return pthis->head == pthis->tail;
}

void vsf_evtq_clean_evt(vsf_evt_t evt)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_KERNEL_ASSERT(eda != NULL);
    vsf_evtq_t *evtq = __vsf_os_evtq_get((vsf_prio_t)eda->priority);
    uint_fast8_t size = 1 << evtq->bitsize;
    uint_fast8_t head_idx, tail_idx;
    vsf_evt_node_t *node;
    vsf_evt_t node_evt;

    vsf_protect_t orig = vsf_protect_int();
        head_idx = evtq->head;
        tail_idx = evtq->tail;
    vsf_unprotect_int(orig);

    while (head_idx != tail_idx) {
        node = &evtq->node[head_idx];
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
        node_evt = node->evt;
#else
        {
            uintptr_t value = node->evt_union.value;
            if (value & 1) {
                node_evt = (vsf_evt_t)(value >> 1);
            } else {
                node_evt = VSF_EVT_MESSAGE;
            }
        }
#endif
        if ((node->eda == eda) && ((VSF_EVT_NONE == evt) || (node_evt == evt))) {
            node->eda = NULL;
        }
        head_idx = (head_idx + 1) & (size - 1);
    }
}

vsf_err_t vsf_evtq_poll(vsf_evtq_t *pthis)
{
    vsf_evt_node_t *node;
    vsf_eda_t *eda;
    uint_fast8_t size;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(pthis != NULL);
    size = 1 << pthis->bitsize;

    while (!vsf_evtq_is_empty(pthis)) {
        node = &pthis->node[pthis->head];
        pthis->head = (pthis->head + 1) & (size - 1);
        eda = node->eda;

        if (eda != NULL) {
            if (!eda->flag.state.is_to_exit) {
                orig = vsf_protect_int();
                    pthis->cur.eda = eda;

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
                    pthis->cur.evt = node->evt;
                    pthis->cur.msg = (uintptr_t)node->msg;
#else
                {
                    uintptr_t value = node->evt_union.value;
                    if (value & 1) {
                        pthis->cur.evt = (vsf_evt_t)(value >> 1);
                        pthis->cur.msg = NULL;
                    } else {
                        pthis->cur.evt = VSF_EVT_MESSAGE;
                        pthis->cur.msg = value;
                    }
                }
#endif
                vsf_unprotect_int(orig);

                __vsf_dispatch_evt(eda, pthis->cur.evt);
            }

            orig = vsf_protect_int();
                pthis->cur.eda = NULL;
                pthis->cur.evt = VSF_EVT_INVALID;
                pthis->cur.msg = (uintptr_t)NULL;
                eda->evt_cnt--;
#if VSF_KERNEL_CFG_TRACE == ENABLED
                if (!eda->evt_cnt) {
                    vsf_kernel_trace_eda_idle(eda);
                }
#endif
            vsf_unprotect_int(orig);

            if (eda->flag.state.is_to_exit) {
                __vsf_eda_terminate(eda);
            }
        }
    }
    return VSF_ERR_NONE;
}
#endif
#endif
