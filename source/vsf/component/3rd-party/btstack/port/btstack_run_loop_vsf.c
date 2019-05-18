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

#include "btstack_linked_list.h"
#include "btstack_debug.h"

#include "btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct btstack_vsf_t {
    btstack_linked_list_t timers;
    vsf_callback_timer_t callback_timer;
};
typedef struct btstack_vsf_t btstack_vsf_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static btstack_vsf_t __btstack_vsf;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint32_t btstack_run_loop_vsf_get_time_ms(void)
{
    return vsf_systimer_tick_to_ms(vsf_timer_get_tick());
}

static void btstack_run_loop_vsf_set_timer(btstack_timer_source_t *ts, uint32_t timeout_in_ms)
{
    ts->timeout = btstack_run_loop_vsf_get_time_ms() + timeout_in_ms + 1;
}

static void btstack_run_loop_vsf_add_timer(btstack_timer_source_t *ts)
{
    btstack_linked_item_t *it;
    for (it = (btstack_linked_item_t *)&__btstack_vsf.timers; it->next; it = it->next) {
        if ((btstack_timer_source_t *)it->next == ts) {
//            vsf_trace(VSF_TRACE_ERROR, "btstack_run_loop_timer_add error: timer to add already in list!");
            return;
        }
        if (ts->timeout < ((btstack_timer_source_t *)it->next)->timeout) {
            break;
        }
    }
    ts->item.next = it->next;
    it->next = (btstack_linked_item_t *)ts;
}

static int btstack_run_loop_vsf_remove_timer(btstack_timer_source_t *ts)
{
    return btstack_linked_list_remove(&__btstack_vsf.timers, (btstack_linked_item_t *)ts);
}

static void btstack_run_loop_vsf_dump_timer(void)
{
    btstack_linked_item_t *it;
    int index = 0;

    for (it = (btstack_linked_item_t *)__btstack_vsf.timers; it; it = it->next) {
        btstack_timer_source_t *ts = (btstack_timer_source_t *)it;
        vsf_trace(VSF_TRACE_DEBUG, "timer %u, timeout %u\n", index++, (unsigned int)ts->timeout);
    }
}

static void btstack_run_loop_vsf_on_timer(vsf_callback_timer_t *timer)
{
    uint32_t now = btstack_run_loop_vsf_get_time_ms();
    while (__btstack_vsf.timers) {
        btstack_timer_source_t *ts = (btstack_timer_source_t *)__btstack_vsf.timers;
        if (ts->timeout > now) {
            break;
        }
        btstack_run_loop_vsf_remove_timer(ts);
        ts->process(ts);
    }
    vsf_callback_timer_add_ms(timer, 1);
}

static void btstack_run_loop_vsf_init(void)
{
    memset(&__btstack_vsf, 0, sizeof(__btstack_vsf));

    __btstack_vsf.callback_timer.on_timer = btstack_run_loop_vsf_on_timer;
    vsf_callback_timer_add_ms(&__btstack_vsf.callback_timer, 1);
}

static const btstack_run_loop_t btstack_run_loop_vsf =
{
    .init = btstack_run_loop_vsf_init,
    .add_data_source = NULL,
    .remove_data_source = NULL,
    .enable_data_source_callbacks = NULL,
    .disable_data_source_callbacks = NULL,
    .set_timer = btstack_run_loop_vsf_set_timer,
    .add_timer = btstack_run_loop_vsf_add_timer,
    .remove_timer = btstack_run_loop_vsf_remove_timer,
    .execute = NULL,
    .dump_timer = btstack_run_loop_vsf_dump_timer,
    .get_time_ms = btstack_run_loop_vsf_get_time_ms,
};

const btstack_run_loop_t * btstack_run_loop_vsf_get_instance(void)
{
    return &btstack_run_loop_vsf;
}

