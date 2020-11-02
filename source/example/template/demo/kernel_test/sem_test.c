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

#if APP_USE_KERNEL_TEST == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_sem_test_t {
    vsf_sem_t sem;
    vsf_teda_t eda_send[VSF_OS_CFG_PRIORITY_NUM];
    vsf_teda_t eda_recv[VSF_OS_CFG_PRIORITY_NUM];
} usrapp_sem_test_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT usrapp_sem_test_t __usrapp_sem;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __usrapp_sem_recv_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint_fast8_t idx = (vsf_teda_t *)eda - &__usrapp_sem.eda_recv[0];
    vsf_trace(VSF_TRACE_INFO, "eda_recv[%d]: %d\r\n", idx, evt);

    switch (evt) {
    case VSF_EVT_INIT:
    pend_next:
        if (VSF_ERR_NONE == vsf_eda_sem_pend(&__usrapp_sem.sem, vsf_systimer_us_to_tick(100))) {
        process_sem:
            vsf_trace(VSF_TRACE_INFO, "%d: eda_recv[%d] got sem\r\n", vsf_systimer_get_ms(), idx);
            goto pend_next;
        }
        break;
    case VSF_EVT_TIMER:
    case VSF_EVT_SYNC:
    case VSF_EVT_SYNC_CANCEL:
        {
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&__usrapp_sem.sem, evt);
            switch (reason) {
            case VSF_SYNC_GET:
                goto process_sem;
            case VSF_SYNC_CANCEL:
            case VSF_SYNC_FAIL:
                ASSERT(false);
                break;
            case VSF_SYNC_TIMEOUT:
                vsf_trace(VSF_TRACE_INFO, "%d: eda_recv[%d] time out\r\n", vsf_systimer_get_ms(), idx);
                goto pend_next;
            case VSF_SYNC_PENDING:
                return;
            }
        }
    }
}

static void __usrapp_sem_send_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint_fast8_t idx = (vsf_teda_t *)eda - &__usrapp_sem.eda_send[0];
    vsf_trace(VSF_TRACE_INFO, "eda_send[%d]: %d\r\n", idx, evt);

    switch (evt) {
    case VSF_EVT_INIT:
        // fall through
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_INFO, "%d: eda_send[%d] post sem\r\n", vsf_systimer_get_ms(), idx);
        vsf_eda_sem_post(&__usrapp_sem.sem);
        vsf_teda_set_timer(vsf_systimer_us_to_tick(10));
        break;
    }
}

void usrapp_sem_test_start(void)
{
    vsf_eda_sem_init(&__usrapp_sem.sem, 0);

    for (int i = 0; i < dimof(__usrapp_sem.eda_recv); i++) {
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __usrapp_sem_recv_evthandler,
            .priority       = vsf_prio_0 + i,
        };
        vsf_teda_start(&__usrapp_sem.eda_recv[i], (vsf_eda_cfg_t *)&cfg);
    }

    for (int i = 0; i < dimof(__usrapp_sem.eda_send); i++) {
        const vsf_eda_cfg_t cfg = {
            .fn.evthandler  = __usrapp_sem_send_evthandler,
            .priority       = vsf_prio_0 + i,
        };
        vsf_teda_start(&__usrapp_sem.eda_send[i], (vsf_eda_cfg_t *)&cfg);
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
int kernel_sem_test_main(int argc, char *argv[])
{
    usrapp_sem_test_start();
    return 0;
}
#endif

#endif
