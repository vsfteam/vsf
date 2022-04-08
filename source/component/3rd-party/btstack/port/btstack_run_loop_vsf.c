/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "component/vsf_component_cfg.h"

#if VSF_USE_BTSTACK == ENABLED

#include "service/vsf_service.h"
#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"

#include "btstack_linked_list.h"
#include "btstack_debug.h"

#include "btstack_run_loop_vsf.h"

/*============================ MACROS ========================================*/

#ifndef VSF_BTSTACK_CFG_PRIORITY
#   define VSF_BTSTACK_CFG_PRIORITY         vsf_prio_inherit
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct btstack_vsf_t {
    btstack_linked_list_t timers;
    vsf_teda_t task;
} btstack_vsf_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static uint32_t __btstack_run_loop_vsf_get_time_ms(void);
static void __btstack_run_loop_vsf_set_timer(btstack_timer_source_t *ts, uint32_t timeout_in_ms);
static void __btstack_run_loop_vsf_add_timer(btstack_timer_source_t *ts);
static bool __btstack_run_loop_vsf_remove_timer(btstack_timer_source_t *ts);
static void __btstack_run_loop_vsf_dump_timer(void);
static void __btstack_run_loop_vsf_init(void);

/*============================ LOCAL VARIABLES ===============================*/

static btstack_vsf_t __btstack_vsf;

static const btstack_run_loop_t __btstack_run_loop_vsf = {
    .init = __btstack_run_loop_vsf_init,
    .add_data_source = NULL,
    .remove_data_source = NULL,
    .enable_data_source_callbacks = NULL,
    .disable_data_source_callbacks = NULL,
    .set_timer = __btstack_run_loop_vsf_set_timer,
    .add_timer = __btstack_run_loop_vsf_add_timer,
    .remove_timer = __btstack_run_loop_vsf_remove_timer,
    .execute = NULL,
    .dump_timer = __btstack_run_loop_vsf_dump_timer,
    .get_time_ms = __btstack_run_loop_vsf_get_time_ms,
};

/*============================ IMPLEMENTATION ================================*/

#ifdef ENABLE_BTSTACK_ASSERT
WEAK(btstack_assert_failed)
void btstack_assert_failed(const char * file, uint16_t line_nr)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_error("Assert: file %s, line %u\n", file, line_nr);
#endif
    VSF_ASSERT(false);
}
#endif

static uint32_t __btstack_run_loop_vsf_get_time_ms(void)
{
    return vsf_systimer_get_ms();
}

static void __btstack_run_loop_vsf_set_timer(btstack_timer_source_t *ts, uint32_t timeout_in_ms)
{
    ts->timeout = __btstack_run_loop_vsf_get_time_ms() + timeout_in_ms + 1;
}

static void __btstaci_run_loop_vsf_update_timer(btstack_timer_source_t *ts)
{
    if (NULL == ts) {
        ts = (btstack_timer_source_t *)btstack_linked_list_get_first_item(&__btstack_vsf.timers);
        if (NULL == ts) {
            return;
        }
    }

    if (ts == (btstack_timer_source_t *)btstack_linked_list_get_first_item(&__btstack_vsf.timers)) {
        uint32_t now = __btstack_run_loop_vsf_get_time_ms();

        __vsf_teda_cancel_timer(&__btstack_vsf.task);
        if (ts->timeout > now) {
            uint32_t duration = ts->timeout - now;
            vsf_teda_set_timer_ex(&__btstack_vsf.task, vsf_systimer_ms_to_tick(duration));
        } else {
            vsf_eda_post_evt(&__btstack_vsf.task.use_as__vsf_eda_t, VSF_EVT_TIMER);
        }
    }
}

static void __btstack_run_loop_vsf_add_timer(btstack_timer_source_t *ts)
{
    btstack_linked_item_t *it;
    for (it = (btstack_linked_item_t *)&__btstack_vsf.timers; it->next; it = it->next) {
        if ((btstack_timer_source_t *)it->next == ts) {
//            vsf_trace_error("btstack_run_loop_timer_add error: timer to add already in list!");
            return;
        }
        if (ts->timeout < ((btstack_timer_source_t *)it->next)->timeout) {
            break;
        }
    }
    ts->item.next = it->next;
    it->next = (btstack_linked_item_t *)ts;

    __btstaci_run_loop_vsf_update_timer(ts);
}

static bool __btstack_run_loop_vsf_remove_timer(btstack_timer_source_t *ts)
{
    bool is_first = ts == (btstack_timer_source_t *)btstack_linked_list_get_first_item(&__btstack_vsf.timers);
    bool ret = btstack_linked_list_remove(&__btstack_vsf.timers, (btstack_linked_item_t *)ts);
    if (is_first) {
        __btstaci_run_loop_vsf_update_timer(NULL);
    }
    return ret;
}

static void __btstack_run_loop_vsf_dump_timer(void)
{
    btstack_linked_item_t *it;
    int index = 0;

    for (it = (btstack_linked_item_t *)__btstack_vsf.timers; it; it = it->next) {
        btstack_timer_source_t *ts = (btstack_timer_source_t *)it;
        vsf_trace_debug("timer %u, timeout %u\n", index++, (unsigned int)ts->timeout);
    }
}

static void __btstack_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_TIMER: {
            uint32_t now = __btstack_run_loop_vsf_get_time_ms();
            while (__btstack_vsf.timers) {
                btstack_timer_source_t *ts = (btstack_timer_source_t *)__btstack_vsf.timers;
                if (ts->timeout > now) {
                    break;
                }
                __btstack_run_loop_vsf_remove_timer(ts);
                ts->process(ts);
            }

            __btstaci_run_loop_vsf_update_timer(NULL);
        }
        break;
    }
}

static void __btstack_run_loop_vsf_init(void)
{
    memset(&__btstack_vsf, 0, sizeof(__btstack_vsf));

    __btstack_vsf.task.fn.evthandler = __btstack_evthandler;
    vsf_teda_init(&__btstack_vsf.task, VSF_BTSTACK_CFG_PRIORITY);
}

const btstack_run_loop_t * btstack_run_loop_vsf_get_instance(void)
{
    return &__btstack_run_loop_vsf;
}

#endif      // VSF_USE_BTSTACK
