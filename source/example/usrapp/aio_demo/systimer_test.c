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

struct usrapp_t {
    vsf_callback_timer_t poll_timer;
    vsf_callback_timer_t one_tick_timer;
    vsf_teda_t one_tick_teda;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_t usrapp;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_one_tick_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_TIMER:
    case VSF_EVT_INIT:
        vsf_teda_set_timer(1);
        break;
    }
}

static void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    vsf_trace(VSF_TRACE_DEBUG, "heartbeat: [%lld]" VSF_TRACE_CFG_LINEEND, vsf_timer_get_tick());
    vsf_callback_timer_add_ms(timer, 1000);
}

static void one_tick_timer_callback(vsf_callback_timer_t *timer)
{
    vsf_callback_timer_add(timer, 1);
}

int main(void)
{
    vsf_trace_init(NULL);

    usrapp.poll_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 1000);

//    usrapp.one_tick_teda.evthandler = usrapp_one_tick_evthandler;
//    usrapp.one_tick_teda.on_terminate = NULL;
//    vsf_teda_init(&usrapp.one_tick_teda, vsf_priority_0, false);

    usrapp.one_tick_timer.on_timer = one_tick_timer_callback;
    vsf_callback_timer_add(&usrapp.one_tick_timer, 1);

    return 0;
}

/* EOF */
