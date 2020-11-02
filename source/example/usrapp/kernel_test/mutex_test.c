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

struct usrapp_mutex_eda_t {
    vsf_teda_t teda;
    enum {
        PENDING_MUTEX,
        DELAYING,
    } state;
    uint8_t i;
};
typedef struct usrapp_mutex_eda_t usrapp_mutex_eda_t;

struct usrapp_mutex_test_t {
    vsf_mutex_t mutex;
    usrapp_mutex_eda_t mutex_eda[VSF_OS_CFG_PRIORITY_NUM];
};
typedef struct usrapp_mutex_test_t usrapp_mutex_test_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT usrapp_mutex_test_t usrapp_mutex;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_mutex_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    usrapp_mutex_eda_t *mutex_eda = (usrapp_mutex_eda_t *)eda;

    switch (evt) {
    case VSF_EVT_INIT:
    pend_next:
        mutex_eda->state = PENDING_MUTEX;
        if (VSF_ERR_NONE == vsf_eda_mutex_enter(&usrapp_mutex.mutex)) {
        on_got_mutex:
            vsf_trace(VSF_TRACE_INFO, "%d: eda[%d] got mutex\r\n",
                        vsf_systimer_get_ms(),
                        mutex_eda->i);
            mutex_eda->state = DELAYING;
            vsf_teda_set_timer(vsf_systimer_us_to_tick((mutex_eda->i + 1) * 100));
        }
        break;
    case VSF_EVT_TIMER:
        if (DELAYING == mutex_eda->state) {
            vsf_trace(VSF_TRACE_INFO, "%d: eda[%d] release mutex\r\n",
                        vsf_systimer_get_ms(),
                        mutex_eda->i);
            vsf_eda_mutex_leave(&usrapp_mutex.mutex);
            goto pend_next;
        }
    case VSF_EVT_SYNC:
    case VSF_EVT_SYNC_CANCEL:
        {
            vsf_sync_reason_t reason = vsf_eda_sync_get_reason(&usrapp_mutex.mutex.use_as__vsf_sync_t, evt);
            switch (reason) {
            case VSF_SYNC_GET:
                goto on_got_mutex;
            case VSF_SYNC_CANCEL:
            case VSF_SYNC_FAIL:
                ASSERT(false);
                break;
            case VSF_SYNC_TIMEOUT:
                vsf_trace(VSF_TRACE_INFO, "%d: eda[%d] time out\r\n",
                            vsf_systimer_get_ms(),
                            mutex_eda->i);
                goto pend_next;
            case VSF_SYNC_PENDING:
                return;
            }
        }
    }
}

void usrapp_mutex_test_start(void)
{
    vsf_eda_mutex_init(&usrapp_mutex.mutex);

    for (uint_fast8_t i = 0; i < dimof(usrapp_mutex.mutex_eda); i++) {
        usrapp_mutex.mutex_eda[i].i = i;

        {
            const vsf_eda_cfg_t cfg = {
                .fn.evthandler  = usrapp_mutex_evthandler,
                .priority       = vsf_prio_0 + i,
            };
            vsf_teda_start(&usrapp_mutex.mutex_eda[i].teda, (vsf_eda_cfg_t *)&cfg);
        }
    }
}
