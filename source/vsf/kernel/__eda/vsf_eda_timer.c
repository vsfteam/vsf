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

#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_KERNEL == ENABLED && defined(__EDA_GADGET__)

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if     VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   if      VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS     \
        &&  !defined(VSF_SYSTIMER_CFG_IMPL_MODE)
extern bool vsf_systimer_is_due(vsf_systimer_cnt_t due);
extern vsf_err_t vsf_systimer_start(void);
extern vsf_systimer_cnt_t vsf_systimer_get(void);
extern uint_fast32_t vsf_systimer_tick_to_ms(vsf_systimer_cnt_t tick);
extern uint_fast32_t vsf_systimer_tick_to_us(vsf_systimer_cnt_t tick);
extern vsf_systimer_cnt_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern vsf_systimer_cnt_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
#   endif

SECTION(".text.vsf.kernel.teda")
static void __vsf_teda_timer_enqueue(vsf_teda_t *this_ptr, vsf_timer_tick_t due);

SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
extern vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS

static void __vsf_timer_wakeup(void)
{
    if (!__vsf_eda.timer.processing) {
        __vsf_eda.timer.processing = true;
#if defined(__VSF_KERNEL_TASK_TEDA)
        if (vsf_eda_post_evt(&__vsf_eda.teda.use_as__vsf_eda_t, VSF_EVT_TIMER)) {
#elif defined(__VSF_KERNEL_TASK_EDA)
        if (vsf_eda_post_evt(&__vsf_eda.eda, VSF_EVT_TIMER)) {
#endif
            __vsf_eda.timer.processing = false;
        }
    }
}

#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
static void __vsf_timer_update(bool force)
{
    vsf_teda_t *teda;

    vsf_timq_peek(&__vsf_eda.timer.timq, teda);
    if (NULL == teda) {
        vsf_systimer_set_idle();
    } else if (force || (teda->due != __vsf_eda.timer.pre_tick)) {
        __vsf_eda.timer.pre_tick = teda->due;
        if (!vsf_systimer_set(teda->due)) {
            __vsf_timer_wakeup();
        }
    }
}
#else
static void __vsf_timer_update(bool force)
{

}
#endif

SECTION(".text.vsf.kernel.teda")
static bool __vsf_timer_is_due(vsf_systimer_cnt_t due)
{
    return vsf_systimer_is_due(due);
}

SECTION(".text.vsf.kernel.teda")
static void __vsf_timer_start(void)
{
    vsf_systimer_prio_set(__vsf_eda.timer.arch_prio);

    vsf_systimer_start();
}

// DO NOT add section on vsf_systimer_evthandler, it's a weak function in arch
void vsf_systimer_evthandler(vsf_systimer_cnt_t tick)
{
    UNUSED_PARAM(tick);
    __vsf_timer_wakeup();
}

SECTION(".text.vsf.kernel.teda")
static vsf_err_t __vsf_teda_set_timer_imp(vsf_teda_t *this_ptr, vsf_timer_tick_t due)
{
    __vsf_teda_timer_enqueue(this_ptr, due);
    __vsf_timer_update(false);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.teda")
vsf_timer_tick_t vsf_systimer_get_tick(void)
{
    return vsf_systimer_get();
}
#else       // VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
SECTION(".text.vsf.kernel.teda")
static void __vsf_timer_start(void)
{
    // in tick timer mode, user will initialize timer and
}

SECTION(".text.vsf.kernel.teda")
static bool __vsf_timer_is_due(vsf_systimer_cnt_t due)
{
    return ((vsf_systimer_cnt_signed_t)(vsf_systimer_get_tick() - due) >= 0);
}

SECTION(".text.vsf.kernel.teda")
void vsf_timer_on_tick(void)
{
    vsf_protect_t orig = vsf_protect_int();
        __vsf_eda.timer.cur_tick++;
    vsf_unprotect_int(orig);
    vsf_eda_post_evt(&__vsf_eda.teda.use_as__vsf_eda_t, VSF_EVT_TIMER);
}

SECTION(".text.vsf.kernel.teda")
static vsf_err_t __vsf_teda_set_timer_imp(vsf_teda_t *this_ptr, vsf_timer_tick_t due)
{
    __vsf_teda_timer_enqueue(this_ptr, due);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.teda")
vsf_timer_tick_t vsf_systimer_get_tick(void)
{
    vsf_timer_tick_t tick;
    vsf_protect_t orig = vsf_protect_int();
        tick = __vsf_eda.timer.cur_tick;
    vsf_unprotect_int(orig);
    return tick;
}
#endif      // VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS

SECTION(".text.vsf.kernel.teda")
static void __vsf_timer_init(void)
{
#if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
    __vsf_eda.timer.processing = false;
#endif
    vsf_timq_init(&__vsf_eda.timer.timq);
#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq);
    vsf_callback_timq_init(&__vsf_eda.timer.callback_timq_done);
#endif
}

SECTION(".text.vsf.kernel.teda")
static void __vsf_teda_timer_enqueue(vsf_teda_t *this_ptr, vsf_timer_tick_t due)
{
    VSF_KERNEL_ASSERT((this_ptr != NULL) && !this_ptr->use_as__vsf_eda_t.state.bits.is_timed);
    this_ptr->due = due;

    vsf_timq_insert(&__vsf_eda.timer.timq, this_ptr);
    this_ptr->use_as__vsf_eda_t.state.bits.is_timed = true;
#if     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED                      \
    &&  (   VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                       \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED)
    this_ptr->use_as__vsf_eda_t.state.bits.is_evt_incoming = true;
#endif
}

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_init")
void vsf_callback_timer_init(vsf_callback_timer_t *timer)
{
    timer->due = 0;
}

SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick)
{
    vsf_protect_t lock_status;
    VSF_KERNEL_ASSERT(timer != NULL);

    lock_status = vsf_protect_sched();
        if (timer->due != 0) {
            vsf_kernel_err_report(VSF_KERNEL_ERR_INVALID_USAGE);
            return VSF_ERR_FAIL;
        }

        timer->due = tick + vsf_systimer_get_tick();
        vsf_callback_timq_insert(&__vsf_eda.timer.callback_timq, timer);

        if (NULL == timer->timer_node.prev) {
            __vsf_teda_cancel_timer(&__vsf_eda.teda);

            vsf_callback_timq_peek(&__vsf_eda.timer.callback_timq, timer);
            __vsf_teda_set_timer_imp(&__vsf_eda.teda, timer->due);
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

#if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
SECTION(".text.vsf.kernel.vsf_callback_timer_add_ms")
vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms)
{
    vsf_systimer_cnt_t tick = vsf_systimer_ms_to_tick(ms);
    return vsf_callback_timer_add(timer, (uint_fast32_t)tick);
}

SECTION(".text.vsf.kernel.vsf_callback_timer_add_us")
vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us)
{
    vsf_systimer_cnt_t tick = vsf_systimer_us_to_tick(us);
    return vsf_callback_timer_add(timer, (uint_fast32_t)tick);
}
#endif

SECTION(".text.vsf.kernel.vsf_callback_timer_remove")
vsf_err_t vsf_callback_timer_remove(vsf_callback_timer_t *timer)
{
    vsf_protect_t lock_status;
    VSF_KERNEL_ASSERT(timer != NULL);

    lock_status = vsf_protect_sched();
        if (timer->due != 0) {
            timer->due = 0;
            vsf_callback_timq_remove(&__vsf_eda.timer.callback_timq, timer);
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

#endif

SECTION(".text.vsf.kernel.vsf_timer_get_duration")
uint_fast32_t vsf_timer_get_duration(vsf_timer_tick_t from_time, vsf_timer_tick_t to_time)
{
    if (to_time >= from_time) {
        return (uint_fast32_t)(to_time - from_time);
    } else {
        return (uint_fast32_t)(to_time + 0xFFFFFFFF - from_time);
    }
}

SECTION(".text.vsf.kernel.vsf_timer_get_elapsed")
uint_fast32_t vsf_timer_get_elapsed(vsf_timer_tick_t from_time)
{
    return vsf_timer_get_duration(from_time, vsf_systimer_get_tick());
}

SECTION(".text.vsf.kernel.teda")
vsf_err_t vsf_teda_init(vsf_teda_t *this_ptr, vsf_prio_t priority, bool is_stack_owner)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    return vsf_eda_init(&this_ptr->use_as__vsf_eda_t, priority, is_stack_owner);
}

SECTION(".text.vsf.kernel.vsf_teda_start")
vsf_err_t vsf_teda_start(vsf_teda_t *this_ptr, vsf_eda_cfg_t *cfg)
{
    VSF_KERNEL_ASSERT(this_ptr != NULL);
    return vsf_eda_start(&(this_ptr->use_as__vsf_eda_t), cfg);
}

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ex")
vsf_err_t vsf_teda_set_timer_ex(vsf_teda_t *this_ptr, uint_fast32_t tick)
{
    vsf_protect_t origlevel;
    vsf_err_t err;

    if (0 == tick) {
        VSF_KERNEL_ASSERT(false);
        return VSF_ERR_NOT_AVAILABLE;
    }
    origlevel = vsf_protect_sched();
        err = __vsf_teda_set_timer_imp(
                this_ptr, vsf_systimer_get_tick() + tick);
    vsf_unprotect_sched(origlevel);
    return err;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.vsf_teda_set_timer")
vsf_err_t vsf_teda_set_timer(uint_fast32_t tick)
{
    return vsf_teda_set_timer_ex((vsf_teda_t *)vsf_eda_get_cur(), tick);
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
SECTION(".text.vsf.kernel.vsf_teda_set_timer_ms")
vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms)
{
    vsf_systimer_cnt_t tick = vsf_systimer_ms_to_tick(ms);
    return vsf_teda_set_timer((uint_fast32_t)tick);
}

SECTION(".text.vsf.kernel.vsf_teda_set_timer_us")
vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us)
{
    vsf_systimer_cnt_t tick = vsf_systimer_us_to_tick(us);
    return vsf_teda_set_timer((uint_fast32_t)tick);
}
#endif

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr)
{
    vsf_protect_t lock_status;
    this_ptr = (vsf_teda_t *)__vsf_eda_get_valid_eda((vsf_eda_t *)this_ptr);

    VSF_KERNEL_ASSERT(this_ptr != NULL);

    lock_status = vsf_protect_sched();
        if (this_ptr->use_as__vsf_eda_t.state.bits.is_timed) {
            vsf_timq_remove(&__vsf_eda.timer.timq, this_ptr);
            this_ptr->use_as__vsf_eda_t.state.bits.is_timed = false;
        }
    vsf_unprotect_sched(lock_status);
    return VSF_ERR_NONE;
}

SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
vsf_err_t vsf_teda_cancel_timer(void)
{
    __vsf_teda_cancel_timer(NULL);
    vsf_evtq_clean_evt(VSF_EVT_TIMER);
    return VSF_ERR_NONE;
}

#if __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif