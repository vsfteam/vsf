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

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

#   define usrapp_eda_t             vsf_teda_t
#   define usrapp_eda_init          vsf_teda_start
#   define usrapp_cur_tick()        vsf_systimer_get_ms()

#   define usrapp_sem_pend_timeout  vsf_systimer_us_to_tick(100)
#   define usrapp_sem_post_delay()  vsf_teda_set_timer(vsf_systimer_us_to_tick(10))

#else

#   define usrapp_eda_t             vsf_eda_t
#   define usrapp_eda_init          vsf_eda_start
#   define usrapp_cur_tick()        0

#   define usrapp_sem_pend_timeout  -1
#   define usrapp_sem_post_delay()  vsf_eda_yield()

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_sem_test_t {
    vsf_sem_t sem;
    usrapp_eda_t eda_send[VSF_OS_CFG_PRIORITY_NUM];
    usrapp_eda_t eda_recv[VSF_OS_CFG_PRIORITY_NUM];
};
typedef struct usrapp_sem_test_t usrapp_sem_test_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT usrapp_sem_test_t usrapp_sem;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_sem_recv_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint_fast8_t idx = (usrapp_eda_t *)eda - &usrapp_sem.eda_recv[0];
    vsf_trace(VSF_TRACE_INFO, "eda_recv[%d]: %d\r\n", idx, evt);

    switch (evt) {
    case VSF_EVT_INIT:
    pend_next:
        if (VSF_ERR_NONE == vsf_eda_sem_pend(&usrapp_sem.sem, usrapp_sem_pend_timeout)) {
        process_sem:
            vsf_trace(VSF_TRACE_INFO, "%d: eda_recv[%d] got sem\r\n",
                        usrapp_cur_tick(), idx);
            goto pend_next;
        }
        break;
    case VSF_EVT_TIMER:
    case VSF_EVT_SYNC:
    case VSF_EVT_SYNC_CANCEL:
        {
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&usrapp_sem.sem, evt);
            switch (reason) {
            case VSF_SYNC_GET:
                goto process_sem;
            case VSF_SYNC_CANCEL:
            case VSF_SYNC_FAIL:
                ASSERT(false);
                break;
            case VSF_SYNC_TIMEOUT:
                vsf_trace(VSF_TRACE_INFO, "%d: eda_recv[%d] time out\r\n",
                            usrapp_cur_tick(), idx);
                goto pend_next;
            case VSF_SYNC_PENDING:
                return;
            }
        }
    }
}

static void usrapp_sem_send_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint_fast8_t idx = (usrapp_eda_t *)eda - &usrapp_sem.eda_send[0];
    vsf_trace(VSF_TRACE_INFO, "eda_send[%d]: %d\r\n", idx, evt);

    switch (evt) {
    case VSF_EVT_INIT:
        // fall through
    case VSF_EVT_TIMER:
    case VSF_EVT_YIELD:
        vsf_trace(VSF_TRACE_INFO, "%d: eda_send[%d] post sem\r\n",
                        usrapp_cur_tick(), idx);
        vsf_eda_sem_post(&usrapp_sem.sem);
        usrapp_sem_post_delay();
        break;
    }
}

void usrapp_sem_test_start(void)
{
    vsf_eda_cfg_t cfg = { 0 };
    int i;

    vsf_eda_sem_init(&usrapp_sem.sem, 0);

    cfg.fn.evthandler = usrapp_sem_recv_evthandler;
    for (i = 0; i < dimof(usrapp_sem.eda_recv); i++) {
        cfg.priority = (vsf_prio_t)((int)vsf_prio_0 + i);
        usrapp_eda_init(&usrapp_sem.eda_recv[i], (vsf_eda_cfg_t *)&cfg);
    }

    cfg.fn.evthandler = usrapp_sem_send_evthandler;
    for (i = 0; i < dimof(usrapp_sem.eda_send); i++) {
        cfg.priority = (vsf_prio_t)((int)vsf_prio_0 + i);
        usrapp_eda_init(&usrapp_sem.eda_send[i], (vsf_eda_cfg_t *)&cfg);
    }
}
