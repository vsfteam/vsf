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

SECTION(".text.vsf.kernel.eda")
extern void vsf_eda_on_terminate(vsf_eda_t *this_ptr);

SECTION(".text.vsf.kernel.eda")
extern void __vsf_dispatch_evt(vsf_eda_t *this_ptr, vsf_evt_t evt);

extern vsf_evtq_t *__vsf_os_evtq_get(vsf_prio_t priority);
extern vsf_err_t __vsf_os_evtq_activate(vsf_evtq_t *this_ptr);
extern vsf_err_t __vsf_os_evtq_init(vsf_evtq_t *this_ptr);

/*============================ IMPLEMENTATION ================================*/

void vsf_evtq_on_eda_init(vsf_eda_t *this_ptr)
{
    this_ptr->evt_cnt = 0;
}

static bool __vsf_eda_terminate(vsf_eda_t *this_ptr)
{
    bool terminate;

    VSF_KERNEL_ASSERT(this_ptr != NULL);

    terminate = !this_ptr->evt_cnt;
    if (terminate) {
        vsf_eda_on_terminate(this_ptr);
    }
    return terminate;
}


void vsf_evtq_on_eda_fini(vsf_eda_t *this_ptr)
{
    if (!__vsf_eda_terminate((vsf_eda_t *)this_ptr)) {
        this_ptr->state.bits.is_to_exit = true;
    }
}

vsf_err_t vsf_evtq_init(vsf_evtq_t *this_ptr)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    this_ptr->cur.eda = NULL;
    this_ptr->cur.evt = VSF_EVT_INVALID;
    this_ptr->cur.msg = (uintptr_t)NULL;
    this_ptr->head = 0;
    this_ptr->tail = 0;
    return __vsf_os_evtq_init(this_ptr);
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
    mask = (1 << evtq->bitsize) - 1;

    orig = vsf_protect_int();

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    if (eda->evt_cnt && eda->state.bits.is_limitted && !force) {
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
    eda->evt_cnt++;
    vsf_unprotect_int(orig);

    return __vsf_os_evtq_activate(evtq);
}

vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *this_ptr, vsf_evt_t evt, bool force)
{
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    return __vsf_evtq_post(this_ptr, evt, NULL, force);
#else
    return __vsf_evtq_post(this_ptr, (uintptr_t)((evt << 1) | 1), force);
#endif
}

vsf_err_t vsf_evtq_post_evt(vsf_eda_t *this_ptr, vsf_evt_t evt)
{
    return vsf_evtq_post_evt_ex(this_ptr, evt, false);
}

vsf_err_t vsf_evtq_post_msg(vsf_eda_t *this_ptr, void *msg)
{
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    return __vsf_evtq_post(this_ptr, VSF_EVT_MESSAGE, msg, false);
#else
    return __vsf_evtq_post(this_ptr, (uintptr_t)msg, false);
#endif
}

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
vsf_err_t vsf_evtq_post_evt_msg(vsf_eda_t *this_ptr, vsf_evt_t evt, void *msg)
{
    return __vsf_evtq_post(this_ptr, evt, msg, false);
}
#endif

bool vsf_evtq_is_empty(vsf_evtq_t *this_ptr)
{
    return this_ptr->head == this_ptr->tail;
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
        if ((node->eda == eda) && (node_evt == evt)) {
            node->eda = NULL;
        }
        head_idx = (head_idx + 1) & (size - 1);
    }
}

vsf_err_t vsf_evtq_poll(vsf_evtq_t *this_ptr)
{
    vsf_evt_node_t *node;
    vsf_eda_t *eda;
    uint_fast8_t size;
    vsf_protect_t orig;

    VSF_KERNEL_ASSERT(this_ptr != NULL);
    size = 1 << this_ptr->bitsize;

    while (!vsf_evtq_is_empty(this_ptr)) {
        node = &this_ptr->node[this_ptr->head];
        this_ptr->head = (this_ptr->head + 1) & (size - 1);
        eda = node->eda;

        if (eda != NULL) {
            if (!eda->state.bits.is_to_exit) {
                orig = vsf_protect_int();
                    this_ptr->cur.eda = eda;

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
                    this_ptr->cur.evt = node->evt;
                    this_ptr->cur.msg = (uintptr_t)node->msg;
#else
                {
                    uintptr_t value = node->evt_union.value;
                    if (value & 1) {
                        this_ptr->cur.evt = (vsf_evt_t)(value >> 1);
                        this_ptr->cur.msg = NULL;
                    } else {
                        this_ptr->cur.evt = VSF_EVT_MESSAGE;
                        this_ptr->cur.msg = value;
                    }
                }
#endif
                vsf_unprotect_int(orig);

                __vsf_dispatch_evt(eda, this_ptr->cur.evt);
            }

            orig = vsf_protect_int();
                this_ptr->cur.eda = NULL;
                this_ptr->cur.evt = VSF_EVT_INVALID;
                this_ptr->cur.msg = (uintptr_t)NULL;
                eda->evt_cnt--;
            vsf_unprotect_int(orig);

            if (eda->state.bits.is_to_exit) {
                __vsf_eda_terminate(eda);
            }
        }
    }
    return VSF_ERR_NONE;
}
#endif
#endif
