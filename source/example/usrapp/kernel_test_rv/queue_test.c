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

#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_msg_t {
    implement(vsf_slist_node_t);
    int idx;
};
typedef struct usrapp_msg_t usrapp_msg_t;

declare_vsf_pool(usrapp_msg_pool)
def_vsf_pool(usrapp_msg_pool, usrapp_msg_t)

struct usrapp_msgq_test_t {
    __vsf_pool(usrapp_msg_pool) pool;
    vsf_msg_queue_t msgq;
    vsf_teda_t eda_send[VSF_OS_CFG_PRIORITY_NUM];
    vsf_eda_t eda_recv;
};
typedef struct usrapp_msgq_test_t usrapp_msgq_test_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT usrapp_msgq_test_t usrapp_msgq;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_pool(usrapp_msg_pool, usrapp_msg_t);

static void usrapp_msgq_recv_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    usrapp_msg_t *node;

    switch (evt) {
    case VSF_EVT_INIT:
    recv_next:
        if (VSF_ERR_NONE == vsf_eda_msg_queue_recv(&usrapp_msgq.msgq, &node, -1)) {
        process_node:
            ASSERT(node != NULL);
            vsf_trace(VSF_TRACE_INFO, "%d: get msg from %d\r\n", vsf_systimer_get_ms(), node->idx);
            VSF_POOL_FREE(usrapp_msg_pool, &usrapp_msgq.pool, node);
            goto recv_next;
        }
        break;
    case VSF_EVT_SYNC:
    case VSF_EVT_SYNC_CANCEL:
    case VSF_EVT_TIMER:
        {
            vsf_sync_reason_t reason = vsf_eda_msg_queue_recv_get_reason(&usrapp_msgq.msgq, evt, &node);
            switch (reason) {
            case VSF_SYNC_GET:
                goto process_node;
                // fall through
            case VSF_SYNC_CANCEL:
            case VSF_SYNC_FAIL:
            case VSF_SYNC_TIMEOUT:
                goto recv_next;
            case VSF_SYNC_PENDING:
                break;
            }
        }
        break;
    }
}

static void usrapp_msgq_send_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint_fast8_t idx = (vsf_teda_t *)eda - &usrapp_msgq.eda_send[0];

    switch (evt) {
    case VSF_EVT_TIMER:
        {
            usrapp_msg_t *node = VSF_POOL_ALLOC(usrapp_msg_pool, &usrapp_msgq.pool);
            if (node != NULL) {
                node->idx = idx;
                vsf_trace(VSF_TRACE_INFO, "%d: post msg from %d\r\n", vsf_systimer_get_ms(), idx);
                if (VSF_ERR_NONE != vsf_eda_msg_queue_send(&usrapp_msgq.msgq, node, 0)) {
                    VSF_POOL_FREE(usrapp_msg_pool, &usrapp_msgq.pool, node);
                    vsf_trace(VSF_TRACE_INFO, "%d: fail to post msg from %d\r\n", vsf_systimer_get_ms(), idx);
                }
            }
        }
        // fall through
    case VSF_EVT_INIT:
        vsf_teda_set_timer(vsf_systimer_us_to_tick((idx + 1) * 100));
        break;
    }
}

void usrapp_msgq_test_start(void)
{
    VSF_POOL_PREPARE(usrapp_msg_pool, &usrapp_msgq.pool,
        .target_ptr = NULL,
        .region_ptr = (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
    );
    vsf_eda_msg_queue_init(&usrapp_msgq.msgq, 10);

    {
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = usrapp_msgq_recv_evthandler,
            .priority       = vsf_prio_0,     // TODO: test other priority
        };
        vsf_eda_start(&usrapp_msgq.eda_recv, (vsf_eda_cfg_t *)&cfg);
    }

    for (int i = 0; i < dimof(usrapp_msgq.eda_send); i++) {
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = usrapp_msgq_send_evthandler,
            .priority       = vsf_prio_0 + i,
        };
        vsf_teda_start(&usrapp_msgq.eda_send[i], (vsf_eda_cfg_t *)&cfg);
    }
}
