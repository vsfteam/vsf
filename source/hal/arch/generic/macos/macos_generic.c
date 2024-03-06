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

#define __VSF_ARCH_MACOS_IMPLEMENT
#include "hal/arch/vsf_arch_abstraction.h"
#include "hal/arch/__vsf_arch_interface.h"

#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <setjmp.h>

#include <mach/clock.h>
#include <mach/mach.h>
#include <dispatch/dispatch.h>

/*============================ MACROS ========================================*/

#if VSF_ARCH_PRI_NUM != 1 || VSF_ARCH_SWI_NUM != 0
#   error "macos support parameter error!"
#endif

#ifndef VSF_ARCH_CFG_THREAD_NUM
#   define VSF_ARCH_CFG_THREAD_NUM          32
#endif

#ifndef VSF_ARCH_CFG_IRQ_REQUEST_NUM
#   define VSF_ARCH_CFG_IRQ_REQUEST_NUM     (VSF_ARCH_CFG_THREAD_NUM + 32)
#endif
#if VSF_ARCH_CFG_IRQ_REQUEST_NUM <= VSF_ARCH_CFG_THREAD_NUM
#   error VSF_ARCH_CFG_IRQ_REQUEST_NUM MUST be larger than VSF_ARCH_CFG_THREAD_NUM,\
        because every thread has a start_request
#endif

#ifndef VSF_ARCH_CFG_TRACE_FUNC
#   define VSF_ARCH_CFG_TRACE_FUNC          printf
#endif
#ifndef VSF_ARCH_CFG_IRQ_TRACE_EN
#   define VSF_ARCH_CFG_IRQ_TRACE_EN        DISABLED
#endif
#ifndef VSF_ARCH_CFG_REQUEST_TRACE_EN
#   define VSF_ARCH_CFG_REQUEST_TRACE_EN    DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_ARCH_CFG_IRQ_TRACE_EN == ENABLED
#   define vsf_arch_irq_trace(...)          VSF_ARCH_CFG_TRACE_FUNC(__VA_ARGS__)
#else
#   define vsf_arch_irq_trace(...)
#endif
#if VSF_ARCH_CFG_REQUEST_TRACE_EN == ENABLED
#   define vsf_arch_request_trace(__request, __event)                           \
    do {                                                                        \
        int idx = __vsf_arch_get_thread_idx((__request)->arch_thread);          \
        if (idx >= 0) {                                                         \
            VSF_ARCH_CFG_TRACE_FUNC("irq_request%d %p " __event "\n", idx, (__request));\
        } else {                                                                \
            VSF_ARCH_CFG_TRACE_FUNC("irq_request %p " __event "\n", (__request));\
        }                                                                       \
    } while (false)
#else
#   define vsf_arch_request_trace(...)
#endif

#define __vsf_arch_crit_init(__crit)        pthread_mutex_init(&(__crit), NULL)
#define __vsf_arch_crit_enter(__crit)       pthread_mutex_lock(&(__crit))
#define __vsf_arch_crit_leave(__crit)       pthread_mutex_unlock(&(__crit))

/*============================ TYPES =========================================*/

typedef pthread_mutex_t vsf_arch_crit_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry);

/*============================ INCLUDES ======================================*/

#define __VSF_ARCH_WITHOUT_THREAD_SUSPEND_TEMPLATE__
#include "hal/arch/common/arch_without_thread_suspend/vsf_arch_without_thread_suspend_template.inc"

/*============================ TYPES =========================================*/

typedef struct vsf_arch_systimer_ctx_t {
    implement(vsf_arch_irq_thread_t);
    vsf_arch_irq_request_t due_request;
    dispatch_queue_t queue;
    dispatch_source_t timer;
} vsf_arch_systimer_ctx_t;

dcl_vsf_bitmap(vsf_arch_thread_bitmap, VSF_ARCH_CFG_THREAD_NUM)

typedef struct vsf_arch_thread_t {
    pthread_t pthread;
    vsf_arch_irq_request_t start_request;

    void *param;
} vsf_arch_thread_t;

dcl_vsf_bitmap(vsf_arch_irq_request_bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM)

typedef struct vsf_arch_irq_request_priv_t {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} vsf_arch_irq_request_priv_t;

typedef struct vsf_arch_t {
    struct {
        vsf_arch_thread_t pool[VSF_ARCH_CFG_THREAD_NUM];
        vsf_bitmap(vsf_arch_thread_bitmap) bitmap;
    } thread;
    struct {
        vsf_arch_irq_request_priv_t pool[VSF_ARCH_CFG_IRQ_REQUEST_NUM];
        vsf_bitmap(vsf_arch_irq_request_bitmap) bitmap;
    } irq_request;

    vsf_arch_systimer_ctx_t systimer;
} vsf_arch_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static VSF_CAL_NO_INIT vsf_arch_t __vsf_arch;

/*============================ IMPLEMENTATION ================================*/

static int __vsf_arch_get_thread_idx(vsf_arch_thread_t *thread)
{
    int idx = thread - __vsf_arch.thread.pool;
    if ((idx >= 0) && (idx <= dimof(__vsf_arch.thread.pool))) {
        return idx;
    }
    return -1;
}

/*----------------------------------------------------------------------------*
 * infrastructure                                                             *
 *----------------------------------------------------------------------------*/

void __vsf_arch_irq_request_init(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(!request->is_inited);

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        request->id = vsf_bitmap_ffz(&__vsf_arch.irq_request.bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM);
        if (request->id >= 0) {
            vsf_bitmap_set(&__vsf_arch.irq_request.bitmap, request->id);
        }
    __vsf_arch_crit_leave(__vsf_arch_common.lock);
    VSF_ARCH_ASSERT(request->id >= 0);

    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    request->is_triggered = false;
    pthread_mutex_init(mutex, NULL);
    pthread_cond_init(cond, NULL);
    request->is_inited = true;
}

void __vsf_arch_irq_request_fini(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    pthread_cond_destroy(cond);
    pthread_mutex_destroy(mutex);

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        vsf_bitmap_clear(&__vsf_arch.irq_request.bitmap, request->id);
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    request->is_inited = false;
    request->is_triggered = false;
}

void __vsf_arch_irq_request_pend(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    vsf_arch_request_trace(request, "pend");
    pthread_mutex_lock(mutex);
        while (!request->is_triggered) {
            vsf_arch_request_trace(request, "wait");
            pthread_cond_wait(cond, mutex);
        }
        request->is_triggered = false;
    pthread_mutex_unlock(mutex);
    vsf_arch_request_trace(request, "got");

    vsf_arch_irq_thread_t *irq_thread = request->irq_thread_to_exit;
    if (irq_thread != NULL) {
        request->irq_thread_to_exit = NULL;
        __vsf_arch_irq_exit(irq_thread);
    }
}

void __vsf_arch_irq_request_send(vsf_arch_irq_request_t *request)
{
    VSF_HAL_ASSERT(request->is_inited);
    pthread_cond_t *cond = &__vsf_arch.irq_request.pool[request->id].cond;
    pthread_mutex_t *mutex = &__vsf_arch.irq_request.pool[request->id].mutex;

    vsf_arch_request_trace(request, "send");
    pthread_mutex_lock(mutex);
        request->is_triggered = true;
        pthread_cond_signal(cond);
        vsf_arch_request_trace(request, "signal");
    pthread_mutex_unlock(mutex);
    vsf_arch_request_trace(request, "sent");
}

static void * __vsf_arch_irq_entry(void *arg)
{
    vsf_arch_thread_t *thread = arg;
    int idx = __vsf_arch_get_thread_idx(thread);
    jmp_buf exit;

    thread->start_request.arch_thread = thread;

    while (true) {
        __vsf_arch_irq_request_pend(&thread->start_request);

        vsf_arch_irq_thread_t *irq_thread = thread->param;
        vsf_arch_irq_trace("irq_thread_start %s\n", irq_thread->name);

        irq_thread->exit = (void *)&exit;
        if (!setjmp(exit)) {
            if (irq_thread->entry != NULL) {
                irq_thread->entry(irq_thread);
            }
        }
        irq_thread->exit = NULL;

        if (irq_thread->is_to_restart) {
            irq_thread->is_to_restart = false;
        } else {
            __vsf_arch_crit_enter(__vsf_arch_common.lock);
                vsf_bitmap_clear(&__vsf_arch.thread.bitmap, idx);
            __vsf_arch_crit_leave(__vsf_arch_common.lock);
        }
    }

    pthread_detach(pthread_self());
    return NULL;
}

static vsf_err_t __vsf_arch_create_irq_thread(vsf_arch_irq_thread_t *irq_thread, vsf_arch_irq_entry_t entry)
{
    vsf_arch_thread_t *thread;
    int idx;

    __vsf_arch_crit_enter(__vsf_arch_common.lock);
        idx = vsf_bitmap_ffz(&__vsf_arch.thread.bitmap, VSF_ARCH_CFG_THREAD_NUM);
        if (idx >= 0) {
            vsf_bitmap_set(&__vsf_arch.thread.bitmap, idx);
        }
    __vsf_arch_crit_leave(__vsf_arch_common.lock);

    if (idx >= 0) {
        thread = &__vsf_arch.thread.pool[idx];
        thread->param = irq_thread;
        irq_thread->entry = entry;
        irq_thread->arch_thread = thread;
        vsf_arch_irq_trace("irq_thread_init %s\n", irq_thread->name);
        __vsf_arch_irq_request_send(&thread->start_request);
        return VSF_ERR_NONE;
    }
    VSF_ARCH_ASSERT(false);
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
}

void __vsf_arch_irq_exit(vsf_arch_irq_thread_t *irq_thread)
{
    VSF_ARCH_ASSERT((irq_thread != NULL) && (irq_thread->exit != NULL));
    longjmp(*(jmp_buf *)(irq_thread->exit), -1);
}

vsf_err_t __vsf_arch_irq_restart(vsf_arch_irq_thread_t *irq_thread,
                    vsf_arch_irq_request_t *request_pending)
{
    vsf_arch_thread_t *thread = irq_thread->arch_thread;

    irq_thread->is_to_restart = true;
    request_pending->irq_thread_to_exit = irq_thread;
    __vsf_arch_irq_request_send(request_pending);
    __vsf_arch_irq_request_send(&thread->start_request);
    return VSF_ERR_NONE;
}

void __vsf_arch_irq_sleep(uint32_t ms)
{
    usleep(ms * 1000);
}

/*----------------------------------------------------------------------------*
 * Systimer Timer Implementation                                              *
 *----------------------------------------------------------------------------*/

#if VSF_SYSTIMER_CFG_IMPL_MODE == VSF_SYSTIMER_IMPL_REQUEST_RESPONSE

static void __vsf_systimer_thread(void *arg)
{
    vsf_arch_systimer_ctx_t *ctx = arg;

    __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t);

    while (1) {
        __vsf_arch_irq_request_pend(&__vsf_arch.systimer.due_request);

        __vsf_arch_irq_start(&ctx->use_as__vsf_arch_irq_thread_t);
            vsf_systimer_tick_t tick = vsf_systimer_get();
            vsf_systimer_timeout_evt_handler(tick);
        __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false);
    }
}

/*! \brief initialise SysTick to generate a system timer
 *! \param frequency the target tick frequency in Hz
 *! \return initialization result in vsf_err_t
 */
vsf_err_t vsf_systimer_init(void)
{
    vsf_arch_systimer_ctx_t *ctx = &__vsf_arch.systimer;

    ctx->queue = dispatch_queue_create("timer_queue", 0);
    ctx->timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, ctx->queue);
    dispatch_set_context(ctx->timer, ctx);
    dispatch_source_set_event_handler(ctx->timer, ^{
        dispatch_source_cancel(ctx->timer);
        __vsf_arch_irq_request_send(&ctx->due_request);
    });
    dispatch_source_set_cancel_handler(ctx->timer, ^{
        dispatch_release(ctx->timer);
        dispatch_release(ctx->queue);
        ctx->timer = NULL;
        ctx->queue = NULL;
    });
    __vsf_arch_irq_request_init(&__vsf_arch.systimer.due_request);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_systimer_start(void)
{
    __vsf_arch_irq_init(&__vsf_arch.systimer.use_as__vsf_arch_irq_thread_t,
                "timer", __vsf_systimer_thread, vsf_arch_prio_0);
    __vsf_arch.systimer.due_request.arch_thread = __vsf_arch.systimer.arch_thread;
    return VSF_ERR_NONE;
}

void vsf_systimer_set_idle(void)
{
}

vsf_systimer_tick_t vsf_systimer_get(void)
{
    clock_serv_t clock_serv;
    mach_timespec_t ts;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clock_serv);
    clock_get_time(clock_serv, &ts);
    mach_port_deallocate(mach_task_self(), clock_serv);
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

bool vsf_systimer_set(vsf_systimer_tick_t due)
{
    vsf_arch_systimer_ctx_t *ctx = &__vsf_arch.systimer;
    vsf_systimer_tick_t now = vsf_systimer_get();

    dispatch_source_cancel(ctx->timer);
    if (now >= due) {
        __vsf_arch_irq_request_send(&ctx->due_request);
    } else {
        vsf_systimer_tick_t interval = due - now;
        dispatch_source_set_timer(ctx->timer, dispatch_time(DISPATCH_TIME_NOW, 0), interval, 0);
        dispatch_resume(ctx->timer);
    }
    return true;
}

bool vsf_systimer_is_due(vsf_systimer_tick_t due)
{
    return (vsf_systimer_get() >= due);
}

vsf_systimer_tick_t vsf_systimer_us_to_tick(uint_fast32_t time_us)
{
    return (vsf_systimer_tick_t)time_us;
}

vsf_systimer_tick_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms)
{
    return (vsf_systimer_tick_t)(time_ms * 1000);
}

vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick)
{
    return tick;
}

vsf_systimer_tick_t vsf_systimer_tick_to_ms(vsf_systimer_tick_t tick)
{
    return tick / 1000;
}

void vsf_systimer_prio_set(vsf_arch_prio_t priority)
{

}

#endif

/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_arch_low_level_init(void)
{
    memset(&__vsf_arch, 0, sizeof(__vsf_arch));
    strcpy((char *)__vsf_arch_common.por_thread.name, "por");

    // create thread pool
    vsf_bitmap_clear(&__vsf_arch.irq_request.bitmap, VSF_ARCH_CFG_IRQ_REQUEST_NUM);
    vsf_bitmap_clear(&__vsf_arch.thread.bitmap, VSF_ARCH_CFG_THREAD_NUM);

    // __vsf_arch_low_level_init MUST be called before using __vsf_arch_common.lock
    __vsf_arch_low_level_init();

    for (int i = 0; i < dimof(__vsf_arch.thread.pool); i++) {
        __vsf_arch_irq_request_init(&__vsf_arch.thread.pool[i].start_request);
        if (0 != pthread_create(&__vsf_arch.thread.pool[i].pthread, NULL, __vsf_arch_irq_entry, &__vsf_arch.thread.pool[i])) {
            VSF_HAL_ASSERT(false);
            return false;
        }
    }
    return true;
}

/* EOF */
