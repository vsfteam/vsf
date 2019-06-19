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
#undef vsf_trace
#define vsf_trace(__LV, ...)       printf(__VA_ARGS__)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct eda_sub_demo_t {
    vsf_teda_t teda;
    vsf_teda_t teda_fsm;
};
typedef struct eda_sub_demo_t eda_sub_demo_t;

declare_vsf_task(vsf_task_a)
def_vsf_task(vsf_task_a)



/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static eda_sub_demo_t eda_sub_demo;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#undef this
#define this    (*ptThis)


static fsm_rt_t eda_sub_demo_fsm_b_entry(vsf_eda_frame_t *frame , vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
        vsf_teda_set_timer_ms(1);
        return fsm_rt_wait_for_evt;
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_DEBUG, "sub fsm B return cpl\r\n");
        return fsm_rt_cpl;
    }
    return fsm_rt_on_going;
}

implement_vsf_task(vsf_task_a)
{
    vsf_task_begin();
#define RESET_FSM()     do {vsf_task_state = 0;} while(0)

    enum {
        START = 0,
        DELAY,
        SUB_CALL,
    };
    
    switch(vsf_task_state) {
        case START:
            vsf_task_state = DELAY;
            //break;
        case DELAY:
            vsf_delay_ms(1) {
                vsf_trace(VSF_TRACE_DEBUG, "call sub fsm B\r\n");
                vsf_task_state = SUB_CALL;
            }
            break;
        case SUB_CALL:
            if (fsm_rt_cpl == vsf_eda_call_fsm(eda_sub_demo_fsm_b_entry, NULL)) {
                vsf_trace(VSF_TRACE_DEBUG, "sub fsm A return cpl\r\n");
                RESET_FSM();
                return fsm_rt_cpl;
            } 
            break;
    }
    
    vsf_task_end();
}

static void eda_sub_demo_teda_sub_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable(  )
        )
        vsf_task(vsf_task_a)  task_cb;
        int cnt;
    )
    
    this.cnt = eda->frame->state;
    switch (evt) {
    case VSF_EVT_RETURN:
        vsf_trace(VSF_TRACE_DEBUG, "get return from sub eda\r\n");
        /*! \note IMPORTANT 
         *!       You must call vsf_call_task()/vsf_eda_call_fsm() again to read 
         *        the return value, even you didn't really need the value.
         *!       This read will reset interal status to make sure any 
         *!       vsf_call_task()/vsf_eda_call_fsm call working correctly
         */
        fsm_rt_t ret = vsf_eda_call_task(vsf_task_a, &this.task_cb);            //! DUMMY CALL
        UNUSED_PARAM(ret);
        vsf_eda_return();
        break;
    case VSF_EVT_INIT:
    case VSF_EVT_TIMER:
        if (this.cnt > 0) {
            eda->frame->state = this.cnt - 1;
            vsf_trace(VSF_TRACE_DEBUG, "set 10ms timer in sub eda\r\n");
            vsf_teda_set_timer_ms(10);
        } else {
            vsf_trace(VSF_TRACE_DEBUG, "call sub fsm in sub eda\r\n");
            vsf_eda_call_task(vsf_task_a, &this.task_cb);
        }
        break;
    }
}

static void eda_sub_demo_teda_main_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_RETURN:
        vsf_trace(VSF_TRACE_DEBUG, "get return in main eda\r\n");
    case VSF_EVT_INIT:
        vsf_trace(VSF_TRACE_DEBUG, "set 1000ms timer in main eda\r\n");
        vsf_teda_set_timer_ms(1000);
        break;
    case VSF_EVT_TIMER:
        vsf_trace(VSF_TRACE_DEBUG, "call sub eda in main eda\r\n");
        vsf_eda_call_eda(eda_sub_demo_teda_sub_evthandler);
        break;
    }
}
/*
static fsm_rt_t eda_sub_demo_fsm_main_entry(void *pthis, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_RETURN:
        vsf_trace(VSF_TRACE_DEBUG, "get yield in main fsm\r\n");
        vsf_trace(VSF_TRACE_DEBUG, "main fsm return cpl\r\n");
        return fsm_rt_cpl;
    case VSF_EVT_INIT:
        vsf_trace(VSF_TRACE_DEBUG, "call sub fsm in main fsm\r\n");
        vsf_eda_call_fsm(eda_sub_demo_fsm_a_entry, NULL);
        return fsm_rt_wait_for_evt;
    }
    return fsm_rt_yield;
}
*/
void eda_sub_demo_start(void)
{
    eda_sub_demo.teda.evthandler = eda_sub_demo_teda_main_evthandler;
    vsf_teda_init(&eda_sub_demo.teda, vsf_priority_0, false);
/*
    {
        vsf_eda_cfg_t cfg = {
            .fsm_entry  = eda_sub_demo_fsm_main_entry,
            .priority   = vsf_priority_0,
            .target     = NULL,
            .is_fsm     = true,
        };
        vsf_eda_init_ex(&eda_sub_demo.teda_fsm.use_as__vsf_eda_t, &cfg);
    }
*/
}

#if 0
int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )
    
    vsf_stdio_init();
    
    eda_sub_demo_start();
    
#if VSF_OS_CFG_RUN_MAIN_AS_THREAD == ENABLED
    while(1) {
        //printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
#else
    return 0;
#endif
}

#endif
