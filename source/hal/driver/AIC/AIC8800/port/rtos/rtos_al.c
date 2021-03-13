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

#define __VSF_EDA_CLASS_INHERIT__
#include "vsf.h"

#include "rtos_al.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

#if VSF_KERNEL_TIMER_CFG_ISR != ENABLED
#   error "VSF_KERNEL_TIMER_CFG_ISR is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_vsf_thread_ex(vsf_rtos_thread_t)
def_vsf_thread_ex(vsf_rtos_thread_t,
    def_params(
        void            *arg;
        rtos_task_fct   func;
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
    TimerHandle_t xTimer = (TimerHandle_t)timer;
    if (xTimer->pxCallbackFunction != NULL) {
        xTimer->pxCallbackFunction(xTimer);
    }
    if (xTimer->uxAutoReload) {
        rtos_timer_restart(xTimer, 0, false);
    }
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
    }
    return xTimer;
}

int rtos_timer_delete(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    vsf_callback_timer_remove(&xTimer->use_as__vsf_callback_timer_t);
    vsf_heap_free(xTimer);
    return 0;
}

int rtos_timer_start(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr)
{
    if (isr) {
        vsf_callback_timer_add_isr(&xTimer->use_as__vsf_callback_timer_t, (uint_fast32_t)vsf_systimer_ms_to_tick(xTicksToWait));
        return 0;
    }

    vsf_callback_timer_add_ms(&xTimer->use_as__vsf_callback_timer_t, xTimer->xTimerPeriodInTicks);
    return 0;
}

int rtos_timer_restart(TimerHandle_t xTimer, TickType_t xTicksToWait, bool isr)
{
    return rtos_timer_start(xTimer, xTicksToWait, isr);
}

int rtos_timer_stop(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
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
    this_ptr->func(this_ptr->arg);
}

int rtos_task_create(   rtos_task_fct func,
                        const char * const name,
                        enum rtos_task_id task_id,
                        const uint16_t stack_depth,
                        void * const param,
                        rtos_prio prio,
                        rtos_task_handle * const task_handle)
{
    // default alignment is ok for cortex-m4
    vsf_rtos_thread_t *thread = vsf_heap_malloc(sizeof(vsf_rtos_thread_t) + (stack_depth << 2));
    if (NULL == thread) {
        return -1;
    }

    thread->on_terminate = __vsf_rtos_thread_on_terminate;

    thread->arg = param;
    thread->func = func;

    init_vsf_thread_ex( vsf_rtos_thread_t,
                        thread,
                        prio,
                        (uint64_t *)&thread[1], // no need to align to 64-bit
                        (stack_depth << 2));

    return 0;
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

int rtos_semaphore_wait(rtos_semaphore semaphore, int timeout)
{
    vsf_sync_reason_t reason = vsf_thread_sem_pend(semaphore, vsf_systimer_ms_to_tick(timeout));
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
    vsf_sync_reason_t reason = vsf_thread_mutex_enter(mutex, timeout);
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
        q->node_num = nb_elt;
        q->node_size = elt_size;

        q->op.dequeue = __rtos_queue_dequeue;
        q->op.enqueue = __rtos_queue_enqueue;
        q->region = (vsf_protect_region_t *)&vsf_protect_region_int;
        vsf_eda_queue_init(&q->use_as__vsf_eda_queue_t, nb_elt);
        *queue = q;
        return 0;
    }
    return -1;
}

void rtos_queue_delete(rtos_queue queue)
{
    vsf_heap_free(queue);
}

int rtos_queue_write(rtos_queue queue, void *msg, int timeout, bool isr)
{
    if (isr) {
        vsf_err_t err;
        VSF_ASSERT(0 == timeout);

        err = vsf_eda_queue_send_isr(&queue->use_as__vsf_eda_queue_t, msg);
        return (VSF_ERR_NONE == err) ? 0 : -1;
    }

    if (VSF_ERR_NONE == vsf_eda_queue_send(&queue->use_as__vsf_eda_queue_t,
                                msg, vsf_systimer_ms_to_tick(timeout))) {
        return 0;
    }

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

int rtos_queue_read(rtos_queue queue, void *msg, int timeout, bool isr)
{
    if (isr) {
        vsf_err_t err;
        VSF_ASSERT(0 == timeout);

        err = vsf_eda_queue_recv_isr(&queue->use_as__vsf_eda_queue_t, msg);
        return (VSF_ERR_NONE == err) ? 0 : -1;
    }

    if (VSF_ERR_NONE == vsf_eda_queue_recv(&queue->use_as__vsf_eda_queue_t,
                                msg, vsf_systimer_ms_to_tick(timeout))) {
        return 0;
    }

    vsf_sync_reason_t reason;
    while (1) {
        reason = vsf_eda_queue_recv_get_reason(&queue->use_as__vsf_eda_queue_t,
                                vsf_thread_wait(), msg);
        if (VSF_SYNC_PENDING == reason) {
            continue;
        }
        break;
    }
    return (VSF_SYNC_GET == reason) ? 0 : -1;
}

bool rtos_queue_is_empty(rtos_queue queue)
{
    return 0 == vsf_eda_queue_get_cnt(&queue->use_as__vsf_eda_queue_t);
}

int rtos_init(void)
{
    return 0;
}

/* EOF */
