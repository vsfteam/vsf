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

#define __VSF_EDA_CLASS_INHERIT__
#include "vsf.h"

#include "rtos.h"
#include "rtos_al.h"

// library from vendor depends on this header,
//  and tv_sec in timeval structure MUST BE 64-bit,
//  this will be asserted in initializing code
#include <sys/time.h>

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE != ENABLED
#   error VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE is required for rtos_queue
#endif
#if VSF_EDA_QUEUE_CFG_SUPPORT_ISR != ENABLED
#   error VSF_EDA_QUEUE_CFG_SUPPORT_ISR is required
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required
#endif

#if VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR != ENABLED
#   error VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR is required
#endif

#ifndef AIC8800_OSAL_CFG_TIMER_TASK_STACK_DEPTH
#   define AIC8800_OSAL_CFG_TIMER_TASK_STACK_DEPTH      512
#endif

#ifndef AIC8800_OSAL_CFG_TIMER_TASK_STACK_PRIORITY
#   define AIC8800_OSAL_CFG_TIMER_TASK_STACK_PRIORITY   2
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef AIC8800_OSAL_CFG_TRACE
#   define rtos_trace(...)                  vsf_trace_debug(__VA_ARGS__)
#   define rtos_trace_buffer(...)           vsf_trace_buffer(VSF_TRACE_DEBUG, __VA_ARGS__)
#else
#   define rtos_trace(...)
#   define rtos_trace_buffer(...)
#endif

#ifdef AIC8800_OSAL_CFG_TRACE_TASK
#   define rtos_trace_task(...)             rtos_trace(__VA_ARGS__)
#else
#   define rtos_trace_task(...)
#endif

#ifdef AIC8800_OSAL_CFG_TRACE_QUEUE
#   define rtos_trace_queue(...)            rtos_trace(__VA_ARGS__)
#   define rtos_trace_queue_buffer(...)     rtos_trace_buffer(__VA_ARGS__)
#else
#   define rtos_trace_queue(...)
#   define rtos_trace_queue_buffer(...)
#endif

#ifdef AIC8800_OSAL_CFG_TRACE_NOTIFY
#   define rtos_trace_notify(...)           rtos_trace(__VA_ARGS__)
#else
#   define rtos_trace_notify(...)
#endif

#ifdef AIC8800_OSAL_CFG_TRACE_TIMER
#   define rtos_trace_timer(...)            rtos_trace(__VA_ARGS__)
#else
#   define rtos_trace_timer(...)
#endif

/*============================ TYPES =========================================*/

dcl_vsf_thread_ex(vsf_rtos_thread_t)
def_vsf_thread_ex(vsf_rtos_thread_t,
    def_params(
        void            *arg;
        rtos_task_fct   func;
        char            name[16];
    )
)

typedef struct vsf_rtos_queue_t {
    implement(vsf_eda_queue_t)

    uint16_t head;
    uint16_t tail;
    uint16_t node_num;
    uint16_t node_size;

    uint8_t node_buffer[];
} vsf_rtos_queue_t;

typedef struct rtos_timer_t {
    implement(vsf_callback_timer_t)
    void * pvTimerID;
    TickType_t xTimerPeriodInTicks;
    UBaseType_t uxAutoReload;
    TimerCallbackFunction_t pxCallbackFunction;
} rtos_timer_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT rtos_task_handle __rtos_timer_task;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// heap
void * rtos_malloc(uint32_t size)
{
    return vsf_heap_malloc(size);
}

void rtos_free(void *ptr)
{
    vsf_heap_free(ptr);
}

// timer
uint32_t rtos_now(bool isr)
{
    return vsf_systimer_get_ms();
}

static void __rtos_timer_on_timer(vsf_callback_timer_t * timer)
{
    VSF_ASSERT(__rtos_timer_task != NULL);
    vsf_eda_post_msg((vsf_eda_t *)__rtos_timer_task, timer);
}

TimerHandle_t rtos_timer_create(
                        const char * const pcTimerName,
                        const TickType_t xTimerPeriodInTicks,
                        const UBaseType_t uxAutoReload,
                        void * const pvTimerID,
                        TimerCallbackFunction_t pxCallbackFunction)
{
    TimerHandle_t xTimer = vsf_heap_malloc(sizeof(rtos_timer_t));
    if (xTimer != NULL) {
        xTimer->xTimerPeriodInTicks = xTimerPeriodInTicks;
        xTimer->uxAutoReload = uxAutoReload;
        xTimer->pvTimerID = pvTimerID;
        xTimer->pxCallbackFunction = pxCallbackFunction;
        xTimer->on_timer = __rtos_timer_on_timer;
        vsf_callback_timer_init(&xTimer->use_as__vsf_callback_timer_t);
        rtos_trace_timer("%s: %p %d %s\r\n", __FUNCTION__, xTimer, xTimerPeriodInTicks, uxAutoReload ? "true" : "false");
    }
    return xTimer;
}

int rtos_timer_delete(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    rtos_trace_timer("%s: %p\r\n", __FUNCTION__, xTimer);
    rtos_timer_stop(xTimer, 0);
    vsf_heap_free(xTimer);
    return 0;
}

int rtos_timer_start(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr)
{
    vsf_systimer_tick_t tick = vsf_systimer_ms_to_tick(xTimer->xTimerPeriodInTicks);
    rtos_trace_timer("%s: %p %d\r\n", __FUNCTION__, xTimer, xTicksToWait);
    if (isr) {
        vsf_callback_timer_add_isr(&xTimer->use_as__vsf_callback_timer_t, tick);
    } else {
        vsf_callback_timer_add(&xTimer->use_as__vsf_callback_timer_t, tick);
    }
    return 0;
}

int rtos_timer_restart(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr)
{
    rtos_trace_timer("%s: %p %d\r\n", __FUNCTION__, xTimer, xTicksToWait);
    rtos_timer_stop(xTimer, xTicksToWait);
    return rtos_timer_start(xTimer, xTicksToWait, isr);
}

int rtos_timer_stop(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    rtos_trace_timer("%s: %p %d\r\n", __FUNCTION__, xTimer, xTicksToWait);
    // vsf_callback_timer_remove is identical to vsf_callback_timer_remove_isr
    vsf_callback_timer_remove(&xTimer->use_as__vsf_callback_timer_t);
    return 0;
}

void * rtos_timer_get_pvTimerID(TimerHandle_t xTimer)
{
    return xTimer->pvTimerID;
}

// thread
static void __vsf_rtos_thread_on_terminate(vsf_eda_t *eda)
{
    vsf_heap_free(eda);
}

implement_vsf_thread_ex(vsf_rtos_thread_t)
{
    vsf_this.func(vsf_this.arg);
}

int rtos_task_create(   rtos_task_fct func,
                        const char * const name,
                        enum rtos_task_id task_id,
                        const uint16_t stack_depth,
                        void * const param,
                        rtos_prio prio,
                        rtos_task_handle * const task_handle)
{
    // fix priority dymanically, because the library from the vendor use hardcoded priorities
    // priority promotion MUST be supported, which is required by mutex if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY is enabled
#if     VSF_OS_CFG_ADD_EVTQ_TO_IDLE != ENABLED                                  \
    ||  VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY != ENABLED
    rtos_prio real_prio = prio - 1;
#else
    rtos_prio real_prio = prio;
#endif

#ifdef AIC8800_OSAL_CFG_PRIORITY_BASE
    real_prio += AIC8800_OSAL_CFG_PRIORITY_BASE;
    VSF_ASSERT(real_prio <= vsf_prio_highest);
#endif

    // patch
    // 1. IPC cntrl task will stackoverflow if using some protocols, eg: mdns
    if (!strcmp(name, "IPC cntrl task")) {
        *(uint16_t *)&stack_depth <<= 1;
    }

    // default alignment is ok for cortex-m4
    vsf_rtos_thread_t *thread = vsf_heap_malloc(sizeof(vsf_rtos_thread_t) + (stack_depth << 2));
    if (NULL == thread) {
        rtos_trace_task("%s: %s fail" VSF_TRACE_CFG_LINEEND, __FUNCTION__, name);
        return -1;
    }

    thread->on_terminate = __vsf_rtos_thread_on_terminate;

    thread->arg = param;
    thread->func = func;
    strncpy(thread->name, name, sizeof(thread->name) - 1);
    thread->name[sizeof(thread->name) - 1] = '\0';

    rtos_trace_task("%s: %s(%p) vsf_prio_%d stack(%p:%d)" VSF_TRACE_CFG_LINEEND,
                        __FUNCTION__, name, thread, real_prio,
                        &thread[1], (stack_depth << 2));
    init_vsf_thread_ex( vsf_rtos_thread_t,
                        thread,
                        real_prio,
                        (uint64_t *)&thread[1], // no need to align to 64-bit
                        (stack_depth << 2));

#   if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_kernel_trace_eda_info(&thread->use_as__vsf_eda_t, (char *)name, &thread[1], stack_depth);
#   endif

    if (task_handle) {
        *task_handle = thread;
    }
    return 0;
}

rtos_task_handle rtos_get_task_handle(void)
{
    vsf_thread_t *thread = vsf_thread_get_cur();
    return container_of(thread, vsf_rtos_thread_t, use_as__vsf_thread_t);
}

char * rtos_get_task_name(void)
{
    rtos_task_handle cur_task_handle = rtos_get_task_handle();
    return cur_task_handle->name;
}

void rtos_task_delete(rtos_task_handle task_handle)
{
    rtos_task_handle cur_task_handle = rtos_get_task_handle();
    if (!task_handle) {
        task_handle = cur_task_handle;
    }

    rtos_trace_task("%s: %s(%p)" VSF_TRACE_CFG_LINEEND, __FUNCTION__, task_handle->name, task_handle);
    if (task_handle == cur_task_handle) {
        vsf_thread_exit();
    } else {
        vsf_eda_fini(&task_handle->use_as__vsf_eda_t);
    }
}

void rtos_task_suspend(int duration)
{
    if (duration <= 0) {
        return;
    }
    vsf_thread_delay_ms(duration);
}

uint32_t rtos_protect(void)
{
    return vsf_protect_sched();
}

void rtos_unprotect(uint32_t protect)
{
    vsf_unprotect_sched(protect);
}

// notification
int rtos_task_init_notification(rtos_task_handle task)
{
    rtos_trace_notify("%s: %s %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, task->name, task);
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

uint32_t rtos_task_wait_notification(int timeout)
{
    // rtos_task_wait maybe called in user thread, not rtos_task
    vsf_eda_t *eda = vsf_eda_get_cur();
    uint32_t ret;

    vsf_protect_t orig = vsf_protect_int();
        if (0 == timeout) {
            ret = eda->flag.feature.user_bits;
            vsf_unprotect_int(orig);

            rtos_trace_notify("%s: %08X" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ret);
            return ret;
        }

        eda->flag.state.is_sync_got = false;
        eda->flag.state.is_limitted = true;
        eda->flag.feature.user_bits |= 1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1);

        // private kernel API, can only be used here, so declear here
        extern vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, vsf_systimer_tick_t timeout);
        vsf_timeout_tick_t timeout_tick = (timeout > 0) ? vsf_systimer_ms_to_tick(timeout) : -1;
        __vsf_eda_set_timeout(eda, timeout_tick);
    vsf_unprotect_int(orig);

    while (1) {
        vsf_evt_t evt = vsf_thread_wait();

        orig = vsf_protect_int();
        if ((evt != VSF_EVT_SYNC) && eda->flag.state.is_sync_got) {
            vsf_unprotect_int(orig);
            continue;
        }
        break;
    }

    // private kernel API, can only be used here, so declear here
    extern vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr);
    // -Wcast-align by gcc
    __vsf_teda_cancel_timer((vsf_teda_t *)eda);
    ret = eda->flag.feature.user_bits & ~(1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1));
    if (eda->flag.state.is_sync_got) {
        eda->flag.state.is_sync_got = false;
        eda->flag.feature.user_bits = 0;
    } else {
        eda->flag.feature.user_bits = ret;
    }
    eda->flag.state.is_limitted = false;
    vsf_unprotect_int(orig);

    rtos_trace_notify("%s: %08X" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ret);
    return ret;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

void rtos_task_notify(rtos_task_handle task_handle, uint32_t value, bool isr)
{
    VSF_ASSERT(!(value & ~((1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1)) - 1)));
    VSF_ASSERT(task_handle != NULL);
    uint32_t value_orig;
    vsf_protect_t orig = vsf_protect_int();
        value_orig = task_handle->flag.feature.user_bits;
        task_handle->flag.feature.user_bits = value;
//        task_handle->flag.feature.user_bits &= ~(1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1));
        if (!(value_orig & (1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1)))) {
            vsf_unprotect_int(orig);
            return;
        }
        task_handle->flag.state.is_sync_got = true;
    vsf_unprotect_int(orig);
    rtos_trace_notify("%s: %s(%p) %08X" VSF_TRACE_CFG_LINEEND, __FUNCTION__, task_handle->name,
                            task_handle, value);
    vsf_eda_post_evt(&task_handle->use_as__vsf_eda_t, VSF_EVT_SYNC);
}

void rtos_task_notify_setbits(rtos_task_handle task_handle, uint32_t value, bool isr)
{
    VSF_ASSERT(!(value & ~((1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1)) - 1)));
    VSF_ASSERT(task_handle != NULL);

#ifdef AIC8800_OSAL_CFG_TRACE_NOTIFY
    uint32_t notification;
#endif
    uint32_t value_orig;
    vsf_protect_t orig = vsf_protect_int();
        value_orig = task_handle->flag.feature.user_bits;
        task_handle->flag.feature.user_bits |= value;
        task_handle->flag.feature.user_bits &= ~(1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1));
#ifdef AIC8800_OSAL_CFG_TRACE_NOTIFY
        notification = task_handle->flag.feature.user_bits;
#endif
        if (!(value_orig & (1 << (VSF_KERNEL_CFG_EDA_USER_BITLEN - 1)))) {
            vsf_unprotect_int(orig);
            return;
        }
        task_handle->flag.state.is_sync_got = true;
    vsf_unprotect_int(orig);
    rtos_trace_notify("%s: %p %08X" VSF_TRACE_CFG_LINEEND, __FUNCTION__, task_handle, notification);
    vsf_eda_post_evt(&task_handle->use_as__vsf_eda_t, VSF_EVT_SYNC);
}

// semaphore
int rtos_semaphore_create(rtos_semaphore *semaphore, int max_count, int init_count)
{
    vsf_sem_t *s = vsf_heap_malloc(sizeof(vsf_sem_t));
    if (s != NULL) {
        vsf_eda_sem_init(s, init_count, max_count);
        *semaphore = s;
        return 0;
    }
    return -1;
}

void rtos_semaphore_delete(rtos_semaphore semaphore)
{
    vsf_heap_free(semaphore);
}

int rtos_semaphore_get_count(rtos_semaphore semaphore)
{
    int result;
    vsf_protect_t orig = vsf_protect_sched();
        result = semaphore->cur_union.bits.cur;
    vsf_unprotect_sched(orig);
    return result;
}

int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout)
{
    vsf_timeout_tick_t timeout_tick = (timeout >= 0) ? vsf_systimer_ms_to_tick(timeout) : -1;
    vsf_sync_reason_t reason = vsf_thread_sem_pend(semaphore, timeout_tick);
    return (VSF_SYNC_GET == reason) ? 0 : -1;
}

int rtos_semaphore_signal(rtos_semaphore semaphore, bool isr)
{
    vsf_err_t err = isr ? vsf_eda_sem_post_isr(semaphore) : vsf_eda_sem_post(semaphore);
    return (VSF_ERR_NONE == err) ? 0 : -1;
}

// mutex
int rtos_mutex_create(rtos_mutex *mutex)
{
    vsf_mutex_t *m = vsf_heap_malloc(sizeof(vsf_mutex_t));
    if (m != NULL) {
        vsf_eda_mutex_init(m);
        *mutex = m;
        return 0;
    }
    return -1;
}

void rtos_mutex_delete(rtos_mutex mutex)
{
    vsf_heap_free(mutex);
}

int rtos_mutex_lock(rtos_mutex mutex, int timeout)
{
    vsf_timeout_tick_t timeout_tick = (timeout >= 0) ? vsf_systimer_ms_to_tick(timeout) : -1;
    vsf_sync_reason_t reason = vsf_thread_mutex_enter(mutex, timeout_tick);
    return (VSF_SYNC_GET == reason) ? 0 : -1;
}

int rtos_mutex_unlock(rtos_mutex mutex)
{
    vsf_err_t err = vsf_thread_mutex_leave(mutex);
    return (VSF_ERR_NONE == err) ? 0 : -1;
}

// queue
static bool __rtos_queue_enqueue(vsf_eda_queue_t *queue, void *node)
{
    rtos_queue q = (rtos_queue)queue;
    memcpy(&q->node_buffer[q->head * q->node_size], node, q->node_size);
    if (++q->head >= q->node_num) {
        q->head = 0;
    }
    return true;
}

static bool __rtos_queue_dequeue(vsf_eda_queue_t *queue, void **node)
{
    rtos_queue q = (rtos_queue)queue;
    memcpy(node, &q->node_buffer[q->tail * q->node_size], q->node_size);
    if (++q->tail >= q->node_num) {
        q->tail = 0;
    }
    return true;
}

int rtos_queue_create(int elt_size, int nb_elt, rtos_queue *queue)
{
    rtos_queue q = vsf_heap_malloc(sizeof(vsf_rtos_queue_t) + nb_elt * elt_size);
    if (q != NULL) {
        q->head = q->tail = 0;
        if (nb_elt <= 0) {
            nb_elt = 1;
        }
        q->node_num = nb_elt;
        q->node_size = elt_size;

        q->op.dequeue = __rtos_queue_dequeue;
        q->op.enqueue = __rtos_queue_enqueue;
#if VSF_EDA_QUEUE_CFG_REGION == ENABLED
        q->region = (vsf_protect_region_t *)&vsf_protect_region_int;
#endif
        vsf_eda_queue_init(&q->use_as__vsf_eda_queue_t, nb_elt);
        *queue = q;
        rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, q);
        return 0;
    }
    return -1;
}

void rtos_queue_delete(rtos_queue queue)
{
    rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, queue);
    vsf_eda_queue_cancel(&queue->use_as__vsf_eda_queue_t);
    // CANNOT free now, is there is task notified and not claimed
    if (queue->rx_notified != 0) {
        queue->node_size = 0;
    } else {
        vsf_heap_free(queue);
    }
}

int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr)
{
    rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, queue);
    rtos_trace_queue_buffer(msg, queue->node_size);
    if (isr) {
        vsf_err_t err;
        VSF_ASSERT(0 == timeout);

        err = vsf_eda_queue_send_isr(&queue->use_as__vsf_eda_queue_t, msg);
        return (VSF_ERR_NONE == err) ? 0 : -1;
    }

    vsf_timeout_tick_t timeout_tick = (timeout >= 0) ? vsf_systimer_ms_to_tick(timeout) : -1;
    if (VSF_ERR_NONE == vsf_eda_queue_send(&queue->use_as__vsf_eda_queue_t,
                                msg, timeout_tick)) {
        return 0;
    }

    if (timeout != 0) {
        vsf_sync_reason_t reason;
        while (1) {
            reason = vsf_eda_queue_send_get_reason(&queue->use_as__vsf_eda_queue_t,
                                vsf_thread_wait(), msg);
            if (VSF_SYNC_PENDING == reason) {
                continue;
            }
            break;
        }
        return (VSF_SYNC_GET == reason) ? 0 : -1;
    }
    return -1;
}

int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr)
{
    if (isr) {
        vsf_err_t err;
        VSF_ASSERT(0 == timeout);

        err = vsf_eda_queue_recv_isr(&queue->use_as__vsf_eda_queue_t, msg);
        if (VSF_ERR_NONE == err) {
            rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, queue);
            rtos_trace_queue_buffer(msg, queue->node_size);
        }
        return (VSF_ERR_NONE == err) ? 0 : -1;
    }

    vsf_timeout_tick_t timeout_tick = (timeout >= 0) ? vsf_systimer_ms_to_tick(timeout) : -1;
    if (VSF_ERR_NONE == vsf_eda_queue_recv(&queue->use_as__vsf_eda_queue_t,
                                msg, timeout_tick)) {
        rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, queue);
        rtos_trace_queue_buffer(msg, queue->node_size);
        return 0;
    }

    if (timeout != 0) {
        vsf_sync_reason_t reason;
        while (1) {
            reason = vsf_eda_queue_recv_get_reason(&queue->use_as__vsf_eda_queue_t,
                                vsf_thread_wait(), msg);
            if (0 == queue->node_size) {
                vsf_heap_free(queue);
                return -1;
            }
            if (VSF_SYNC_PENDING == reason) {
                continue;
            }
            if (VSF_SYNC_GET == reason) {
                rtos_trace_queue("%s: %p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, queue);
                rtos_trace_queue_buffer(msg, queue->node_size);
            }
            break;
        }
        return (VSF_SYNC_GET == reason) ? 0 : -1;
    }
    return -1;
}

bool rtos_queue_is_empty(rtos_queue queue)
{
    return 0 == vsf_eda_queue_get_cnt(&queue->use_as__vsf_eda_queue_t);
}

static void __rtos_timer_thread(void *param)
{
    TimerHandle_t xTimer;

    while (1) {
        vsf_thread_wfe(VSF_EVT_MESSAGE);

        xTimer = vsf_eda_get_cur_msg();
        VSF_ASSERT(xTimer != NULL);

        rtos_trace_timer("%s: %p on_timer\r\n", __FUNCTION__, xTimer);
        if (xTimer->pxCallbackFunction != NULL) {
            xTimer->pxCallbackFunction(xTimer);
        }
        if (xTimer->uxAutoReload) {
            rtos_timer_start(xTimer, 0, false);
        }
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

int rtos_init(void)
{
// IMPORTANT: use the latest lib from vendor, there is no timeval dependency
// wifi library from vendor depends on <sys/timer.h>,
//  and tv_sec in timeval structure MUST BE 64-bit,
//  assert if current environment is OK
//    volatile struct timeval time;
//    if (    (8 != sizeof(time.tv_sec))
//        ||  (4 != sizeof(time.tv_usec))) {
//        VSF_ASSERT(false);
//        return -1;
//    }

    return rtos_task_create(__rtos_timer_thread, "rtos_timer", 0,
                AIC8800_OSAL_CFG_TIMER_TASK_STACK_DEPTH, NULL,
                AIC8800_OSAL_CFG_TIMER_TASK_STACK_PRIORITY, &__rtos_timer_task);
}

void rtos_data_save(void)
{
    // TODO: implement
#if 0
    backup_xTickCount = rtos_now(false);
    sleep_data_save();
#if PLF_WIFI_STACK
    #ifndef CFG_HOSTIF
    fhost_data_save();
    lwip_data_save();
    wpas_data_save();
    #endif
#endif
#endif
}

// ugly code, but really, vendor SDK is dependent on this even it has a working rtos_al
void vTaskStepTick(TickType_t xTicksToJump)
{
}

typedef struct {
    int priority;
    int stack_size;
} rtos_task_cfg_st;
rtos_task_cfg_st get_task_cfg(uint8_t task_id)
{
    rtos_task_cfg_st cfg = {0, 0};

    switch (task_id) {
        case IPC_CNTRL_TASK:
            cfg.priority   = TASK_PRIORITY_WIFI_IPC;
            cfg.stack_size = TASK_STACK_SIZE_WIFI_IPC;
            break;
        case SUPPLICANT_TASK:
            cfg.priority   = TASK_PRIORITY_WIFI_WPA;
            cfg.stack_size = TASK_STACK_SIZE_WIFI_WPA;
            break;
        case CONTROL_TASK:
            cfg.priority   = TASK_PRIORITY_WIFI_CNTRL;
            cfg.stack_size = TASK_STACK_SIZE_WIFI_CNTRL;
            break;
        default:
            break;
    }

    return cfg;
}

/* EOF */
