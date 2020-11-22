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

#ifdef __VSF_ARCH_WITHOUT_THREAD_SUSPEND_TEMPLATE__
#undef __VSF_ARCH_WITHOUT_THREAD_SUSPEND_TEMPLATE__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_arch_common_t {
    vsf_arch_irq_thread_t *cur_thread;
    vsf_arch_irq_thread_t por_thread;    // power on reset
    vsf_arch_irq_request_t wakeup_request;

    vsf_arch_crit_t lock;
    vsf_arch_crit_t irq_lock;

    uint32_t irq_ready_cnt;
    vsf_gint_state_t gint_state;
    bool irq_end_from_por;
} vsf_arch_common_t;

#if VSF_ARCH_BG_TRACE_EN == ENABLED
typedef struct vsf_arch_bg_trace_event_t {
    uint32_t idx;
    enum {
        VSF_ARCH_TRACE_LOCK,
        VSF_ARCH_TRACE_IRQ_START,
        VSF_ARCH_TRACE_IRQ_ENTER,
        VSF_ARCH_TRACE_IRQ_END,
        VSF_ARCH_TRACE_GINT,
    } event;
    vsf_arch_irq_thread_t *thread;
    int info;
} vsf_arch_bg_trace_event_t;

typedef struct vsf_arch_bg_trace_t {
    vsf_arch_bg_trace_event_t events[1024];
    uint32_t pos;
    uint32_t idx;
} vsf_arch_bg_trace_t;
#endif

typedef struct vsf_arch_t vsf_arch_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_ARCH_BG_TRACE_EN == ENABLED
static vsf_arch_bg_trace_t __vsf_arch_bg_trace;
#endif

static vsf_arch_common_t __vsf_arch_common;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_ARCH_BG_TRACE_EN == ENABLED
static vsf_arch_bg_trace_event_t * __vsf_arch_bg_trace_get_event(void)
{
    vsf_arch_bg_trace_event_t *event = &__vsf_arch_bg_trace.events[__vsf_arch_bg_trace.pos++];
    if (__vsf_arch_bg_trace.pos >= dimof(__vsf_arch_bg_trace.events)) {
        __vsf_arch_bg_trace.pos = 0;
    }
    event->idx = __vsf_arch_bg_trace.idx++;
    return event;
}

static void __vsf_arch_bg_trace_event(int event, vsf_arch_irq_thread_t *irq_thread, int info)
{
    vsf_arch_bg_trace_event_t *trace_event = __vsf_arch_bg_trace_get_event();
    trace_event->event = event;
    trace_event->thread = irq_thread;
    trace_event->info = info;
}
#endif

static bool __vsf_arch_low_level_init(void)
{
    __vsf_arch_crit_init(__vsf_arch_common.lock);
    __vsf_arch_crit_init(__vsf_arch_common.irq_lock);
    __vsf_arch_common.gint_state = true;
    __vsf_arch_irq_request_init(&__vsf_arch_common.wakeup_request);
    __vsf_arch_irq_start(&__vsf_arch_common.por_thread);
    return true;
}

void __vsf_arch_irq_start(vsf_arch_irq_thread_t *irq_thread)
{
    __vsf_arch_crit_enter(__vsf_arch_common.lock);
#if VSF_ARCH_BG_TRACE_EN == ENABLED
        __vsf_arch_bg_trace_event(VSF_ARCH_TRACE_IRQ_START, irq_thread, __vsf_arch_common.irq_ready_cnt);
#endif
        __vsf_arch_common.irq_ready_cnt++;
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    __vsf_arch_crit_enter(__vsf_arch_common.irq_lock);
#if VSF_ARCH_BG_TRACE_EN == ENABLED
    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        __vsf_arch_bg_trace_event(VSF_ARCH_TRACE_IRQ_ENTER, irq_thread, __vsf_arch_common.irq_ready_cnt);
    __vsf_arch_crit_leave(__vsf_arch_common.lock);
#endif
    __vsf_arch_common.cur_thread = irq_thread;
}

void __vsf_arch_irq_end(vsf_arch_irq_thread_t *irq_thread, bool is_terminate)
{
    bool is_to_wakeup;

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        VSF_HAL_ASSERT(__vsf_arch_common.irq_ready_cnt > 0);
#if VSF_ARCH_BG_TRACE_EN == ENABLED
        __vsf_arch_bg_trace_event(VSF_ARCH_TRACE_IRQ_END, irq_thread, __vsf_arch_common.irq_ready_cnt);
#endif
        is_to_wakeup = (0 == --__vsf_arch_common.irq_ready_cnt) && !__vsf_arch_common.irq_end_from_por;
        __vsf_arch_common.irq_end_from_por = false;
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    if (is_to_wakeup) {
        __vsf_arch_common.cur_thread = &__vsf_arch_common.por_thread;
        __vsf_arch_crit_leave(__vsf_arch_common.irq_lock);
        __vsf_arch_irq_request_send(&__vsf_arch_common.wakeup_request);
    } else {
        __vsf_arch_common.cur_thread = NULL;
        __vsf_arch_crit_leave(__vsf_arch_common.irq_lock);
    }
}

void __vsf_arch_irq_fini(vsf_arch_irq_thread_t *irq_thread)
{

}

void __vsf_arch_irq_init(vsf_arch_irq_thread_t *irq_thread, char *name,
    vsf_arch_irq_entry_t entry, vsf_arch_prio_t priority)
{
    VSF_HAL_ASSERT(strlen(name) < sizeof(irq_thread->name) - 1);
    strcpy((char *)irq_thread->name, name);

    if (VSF_ERR_NONE != __vsf_arch_create_irq_thread(irq_thread, entry)) {
        VSF_HAL_ASSERT(false);
    }
}

void __vsf_arch_irq_set_background(vsf_arch_irq_thread_t *irq_thread)
{
}


/*----------------------------------------------------------------------------*
 * priority and interrupt                                                     *
 *----------------------------------------------------------------------------*/

vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority)
{
    return vsf_arch_prio_0;
}


vsf_gint_state_t vsf_get_interrupt(void)
{
    return __vsf_arch_common.gint_state;
}

void vsf_set_interrupt(vsf_gint_state_t level)
{
    if (__vsf_arch_common.gint_state != level) {
        __vsf_arch_common.gint_state = level;
#if VSF_ARCH_BG_TRACE_EN == ENABLED
        __vsf_arch_crit_enter(__vsf_arch_common.lock);
            __vsf_arch_bg_trace_event(VSF_ARCH_TRACE_GINT, __vsf_arch_common.cur_thread, __vsf_arch_common.gint_state);
        __vsf_arch_crit_leave(__vsf_arch_common.lock);
#endif
    }
}

vsf_gint_state_t vsf_disable_interrupt(void)
{
    vsf_gint_state_t orig = __vsf_arch_common.gint_state;
    if (orig != false) {
        __vsf_arch_common.gint_state = false;
#if VSF_ARCH_BG_TRACE_EN == ENABLED
        __vsf_arch_crit_enter(__vsf_arch_common.lock);
            __vsf_arch_bg_trace_event(VSF_ARCH_TRACE_GINT, __vsf_arch_common.cur_thread, __vsf_arch_common.gint_state);
        __vsf_arch_crit_leave(__vsf_arch_common.lock);
#endif
    }
    return orig;
}

void vsf_enable_interrupt(void)
{
    vsf_set_interrupt(true);
}

/*----------------------------------------------------------------------------*
 * Others: sleep, reset and etc.                                              *
 *----------------------------------------------------------------------------*/

void vsf_arch_sleep(uint_fast32_t mode)
{
    // vsf_arch_sleep can be called with interrupt disabled
//    VSF_HAL_ASSERT(__vsf_arch_common.gint_state);
    __vsf_arch_common.irq_end_from_por = true;
    __vsf_arch_irq_end(&__vsf_arch_common.por_thread, true);
    __vsf_arch_irq_request_pend(&__vsf_arch_common.wakeup_request);
    __vsf_arch_irq_start(&__vsf_arch_common.por_thread);
}

#endif
/* EOF */
