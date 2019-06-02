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

struct eda_sub_demo_t {
    vsf_teda_t teda;
    vsf_teda_t teda_fsm;
};
typedef struct eda_sub_demo_t eda_sub_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static eda_sub_demo_t eda_sub_demo;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static fsm_rt_t eda_sub_demo_fsm_entry(void *pthis, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_teda_set_timer_ms(1);
        return fsm_rt_wait_for_evt;
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_DEBUG, "sub fsm return cpl\r\n");
        return fsm_rt_cpl;
    }
    return fsm_rt_yield;
}

static void eda_sub_demo_teda_sub_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    int cnt = eda->frame->state;
    switch (evt) {
    case VSF_EVT_YIELD:
        vsf_trace(VSF_TRACE_DEBUG, "get yield in sub eda\r\n");
        vsf_eda_return();
        break;
    case VSF_EVT_INIT:
    case VSF_EVT_TIMER:
        if (cnt > 0) {
            eda->frame->state = cnt - 1;
            vsf_trace(VSF_TRACE_DEBUG, "set 10ms timer in sub eda\r\n");
            vsf_teda_set_timer_ms(10);
        } else {
            vsf_trace(VSF_TRACE_DEBUG, "call sub fsm in sub eda\r\n");
            vsf_eda_call_fsm(eda_sub_demo_fsm_entry, NULL);
        }
        break;
    }
}

static void eda_sub_demo_teda_main_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_YIELD:
        vsf_trace(VSF_TRACE_DEBUG, "get yield in main eda\r\n");
    case VSF_EVT_INIT:
        vsf_trace(VSF_TRACE_DEBUG, "set 1000ms timer in main eda\r\n");
        vsf_teda_set_timer_ms(1000);
        break;
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_DEBUG, "call sub eda in main eda\r\n");
        vsf_eda_call_eda(eda_sub_demo_teda_sub_evthandler, (void *)(2 << 1));
        break;
    }
}

static fsm_rt_t eda_sub_demo_fsm_main_entry(void *pthis, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_YIELD:
        vsf_trace(VSF_TRACE_DEBUG, "get yield in main fsm\r\n");
        vsf_trace(VSF_TRACE_DEBUG, "main fsm return cpl\r\n");
        return fsm_rt_cpl;
    case VSF_EVT_INIT:
        vsf_trace(VSF_TRACE_DEBUG, "call sub fsm in main fsm\r\n");
        vsf_eda_call_fsm(eda_sub_demo_fsm_entry, NULL);
        return fsm_rt_wait_for_evt;
    }
    return fsm_rt_yield;
}

void eda_sub_demo_start(void)
{
    eda_sub_demo.teda.evthandler = eda_sub_demo_teda_main_evthandler;
    vsf_teda_init(&eda_sub_demo.teda, vsf_priority_0, false);

    {
        vsf_eda_fsm_cfg_t cfg = {
            .entry      = eda_sub_demo_fsm_main_entry,
            .priority   = vsf_priority_0,
            .target     = NULL,
        };
        vsf_eda_fsm_init(&eda_sub_demo.teda_fsm.use_as__vsf_eda_t, &cfg);
    }
}
