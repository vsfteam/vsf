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
#include <stdio.h>
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_t {
    vsf_sem_t sem;
    vsf_eda_t eda_send;
    vsf_eda_t eda_recv;
};
typedef struct usrapp_t usrapp_t;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_send_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_YIELD:
    case VSF_EVT_INIT:
        vsf_eda_sem_post(&usrapp.sem);
        vsf_eda_yield();
        break;
    }
}

static void usrapp_recv_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_YIELD:
    case VSF_EVT_INIT:
        if (VSF_ERR_NONE == vsf_eda_sem_pend(&usrapp.sem, -1)) {
        got_sem:
            vsf_eda_yield();
        }
        break;
    case VSF_EVT_SYNC:
        goto got_sem;
    }
}

int main(void)
{
#if VSF_USE_TRACE
    vsf_trace_init((vsf_stream_t *)&VSF_DEBUG_STREAM_TX);
    vsf_stdio_init();
    printf("hello world\r\n");
#endif

    vsf_sem_init(&usrapp.sem, 0);

    vsf_eda_set_evthandler(&usrapp.eda_recv, usrapp_recv_evthandler);
    vsf_eda_init(&usrapp.eda_recv, vsf_prio_0, false);

    vsf_eda_set_evthandler(&usrapp.eda_send, usrapp_send_evthandler);
    vsf_eda_init(&usrapp.eda_send, vsf_prio_0, false);
    return 0;
}

/* EOF */
