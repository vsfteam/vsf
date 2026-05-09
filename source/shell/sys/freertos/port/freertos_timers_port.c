/*****************************************************************************
 *   Copyright(C)2009-2026 by VSF Team                                       *
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

/*
 * FreeRTOS timers.h port over vsf_callback_timer + a daemon task.
 *
 * Layout:
 *   TimerHandle_t -> opaque pointer to __vsf_frt_timer_t (inherits
 *                    vsf_callback_timer_t as first field so the kernel's
 *                    container_of via upcast works transparently).
 *
 * Dispatch path:
 *   1. xTimerStart/Reset/ChangePeriod call vsf_callback_timer_add.
 *   2. When the tick fires, __frt_timer_on_cb runs in kernel/ISR context
 *      and posts the timer pointer to __frt_timer_q via the shim's own
 *      xQueueSendFromISR helper -- safe thanks to vsf_eda_queue_send_isr.
 *   3. __frt_timer_daemon (a vsf_thread spawned via xTaskCreate on first
 *      create) loops on xQueueReceive, invokes pxCallbackFunction in task
 *      context, then re-arms auto-reload timers that are still active.
 *
 * Inspiration: AIC8800 rtos_al.c (rtos_timer_*), which uses the same
 * vsf_callback_timer -> dedicated task pattern to bridge ISR-context
 * ticks to task-context user callbacks.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_TIMERS == ENABLED

#define __VSF_FREERTOS_TIMERS_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "vsf_freertos timers port requires VSF_USE_HEAP == ENABLED"
#endif
#if !defined(VSF_USE_KERNEL) || VSF_USE_KERNEL != ENABLED
#   error "vsf_freertos timers port requires VSF_USE_KERNEL == ENABLED"
#endif

#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

#if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER != ENABLED
#   error "vsf_freertos timers port requires VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED"
#endif
#if VSF_FREERTOS_CFG_USE_QUEUE != ENABLED
#   error "vsf_freertos timers port requires VSF_FREERTOS_CFG_USE_QUEUE == ENABLED (timer daemon queue)"
#endif
#if VSF_FREERTOS_CFG_USE_TASK != ENABLED
#   error "vsf_freertos timers port requires VSF_FREERTOS_CFG_USE_TASK == ENABLED (timer daemon task)"
#endif

#include <string.h>
#include <stddef.h>

/*============================ MACROS ========================================*/

// Queue depth chosen to absorb short bursts without dropping ticks on a
// busy system. Each slot holds one pointer-sized word (Timer * address).
#ifndef VSF_FREERTOS_CFG_TIMER_Q_DEPTH
#   define VSF_FREERTOS_CFG_TIMER_Q_DEPTH       16
#endif

// Daemon task stack -- just big enough to host the user callback body.
#ifndef VSF_FREERTOS_CFG_TIMER_TASK_STACK
#   define VSF_FREERTOS_CFG_TIMER_TASK_STACK    4096
#endif

/*============================ TYPES =========================================*/

// Control block = StaticTimer_t (vsf_class declared in timers.h).
// Heap and caller-provided static buffers share the same layout; is_static
// controls whether xTimerDelete releases the backing storage.

/*============================ LOCAL VARIABLES ===============================*/

static QueueHandle_t            __frt_timer_q;
static TaskHandle_t             __frt_timer_task;
static bool                     __frt_timer_inited;

/*============================ LOCAL PROTOTYPES ==============================*/

static void __frt_timer_on_cb(vsf_callback_timer_t *cb_timer);
static void __frt_timer_daemon(void *arg);
static bool __frt_timer_ensure_service(void);

/*============================ IMPLEMENTATION ================================*/

static void __frt_timer_on_cb(vsf_callback_timer_t *cb_timer)
{
    // on_timer runs in kernel / ISR-ish context: must not block. Hand
    // the timer pointer off to the daemon task for user-level dispatch.
    // cb_timer is the first member of StaticTimer_t so the cast is an
    // offset-0 upcast.
    StaticTimer_t *t = (StaticTimer_t *)cb_timer;
    if ((t == NULL) || (__frt_timer_q == NULL)) {
        return;
    }
    BaseType_t woken = pdFALSE;
    (void)xQueueSendFromISR(__frt_timer_q, &t, &woken);
}

static void __frt_timer_daemon(void *arg)
{
    (void)arg;
    for (;;) {
        StaticTimer_t *t = NULL;
        if (xQueueReceive(__frt_timer_q, &t, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        if (t == NULL) { continue; }

        // One-shot timers stop being "active" the moment they fire.
        // Auto-reload timers stay active until the user stops them.
        if (!t->auto_reload) {
            t->is_active = false;
        }

        if (t->pxCallbackFunction != NULL) {
            t->pxCallbackFunction((TimerHandle_t)t);
        }

        // Re-arm only if the user has not disarmed us during the callback.
        if (t->auto_reload && t->is_active) {
            vsf_systimer_tick_t tk = t->period_ticks;
            vsf_callback_timer_add(&t->cb_timer, tk);
        }
    }
}

static bool __frt_timer_ensure_service(void)
{
    // Fast path: already initialised. The only writer runs below under
    // a protect_sched critical section so a simple load is enough.
    if (__frt_timer_inited) {
        return true;
    }

    vsf_protect_t orig = vsf_protect_sched();
        bool need_init = !__frt_timer_inited;
        if (need_init) {
            __frt_timer_inited = true;
        }
    vsf_unprotect_sched(orig);

    if (!need_init) {
        return true;
    }

    // Spawn the queue + daemon exactly once. On failure we roll the flag
    // back so a later xTimerCreate can retry.
    __frt_timer_q = xQueueCreate(VSF_FREERTOS_CFG_TIMER_Q_DEPTH,
                                 sizeof(StaticTimer_t *));
    if (__frt_timer_q == NULL) {
        goto fail;
    }

    __frt_timer_task = NULL;
    BaseType_t rc = xTaskCreate(__frt_timer_daemon,
                                "tmrsvc",
                                VSF_FREERTOS_CFG_TIMER_TASK_STACK,
                                NULL, 0, &__frt_timer_task);
    if (rc != pdPASS) {
        vQueueDelete(__frt_timer_q);
        __frt_timer_q = NULL;
        goto fail;
    }
    return true;

fail:
    orig = vsf_protect_sched();
        __frt_timer_inited = false;
    vsf_unprotect_sched(orig);
    return false;
}

/*============================ API ===========================================*/

static void __frt_timer_init(StaticTimer_t *t,
                             const char * const pcTimerName,
                             TickType_t xTimerPeriodInTicks,
                             UBaseType_t uxAutoReload,
                             void *pvTimerID,
                             TimerCallbackFunction_t pxCallbackFunction,
                             bool is_static)
{
    memset(t, 0, sizeof(*t));
    t->cb_timer.on_timer     = __frt_timer_on_cb;
    t->period_ticks          = xTimerPeriodInTicks;
    t->auto_reload           = uxAutoReload;
    t->pvTimerID             = pvTimerID;
    t->pxCallbackFunction    = pxCallbackFunction;
    t->name                  = pcTimerName;
    t->is_active             = false;
    t->is_static             = is_static;
    vsf_callback_timer_init(&t->cb_timer);
}

TimerHandle_t xTimerCreate(const char * const pcTimerName,
                           const TickType_t xTimerPeriodInTicks,
                           const UBaseType_t uxAutoReload,
                           void * const pvTimerID,
                           TimerCallbackFunction_t pxCallbackFunction)
{
    if ((pxCallbackFunction == NULL) || (xTimerPeriodInTicks == 0)) {
        return NULL;
    }
    if (!__frt_timer_ensure_service()) {
        return NULL;
    }

    StaticTimer_t *t = (StaticTimer_t *)vsf_heap_malloc(sizeof(*t));
    if (t == NULL) {
        return NULL;
    }
    __frt_timer_init(t, pcTimerName, xTimerPeriodInTicks, uxAutoReload,
                     pvTimerID, pxCallbackFunction, false);
    return (TimerHandle_t)t;
}

TimerHandle_t xTimerCreateStatic(const char * const pcTimerName,
                                 const TickType_t xTimerPeriodInTicks,
                                 const UBaseType_t uxAutoReload,
                                 void * const pvTimerID,
                                 TimerCallbackFunction_t pxCallbackFunction,
                                 StaticTimer_t *pxTimerBuffer)
{
    if ((pxTimerBuffer == NULL) || (pxCallbackFunction == NULL)
            || (xTimerPeriodInTicks == 0)) {
        return NULL;
    }
    if (!__frt_timer_ensure_service()) {
        return NULL;
    }
    __frt_timer_init(pxTimerBuffer, pcTimerName, xTimerPeriodInTicks, uxAutoReload,
                     pvTimerID, pxCallbackFunction, true);
    return (TimerHandle_t)pxTimerBuffer;
}

BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    (void)xTicksToWait;
    if (xTimer == NULL) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    // FreeRTOS "start" is idempotent: if already running we still re-arm
    // from the current tick -- caller who wants "reset" semantics calls
    // xTimerReset explicitly (which goes through the same path).
    vsf_callback_timer_remove(&t->cb_timer);
    t->is_active = true;
    vsf_callback_timer_add(&t->cb_timer, t->period_ticks);
    return pdPASS;
}

BaseType_t xTimerStartFromISR(TimerHandle_t xTimer,
                              BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if (xTimer == NULL) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    vsf_callback_timer_remove_isr(&t->cb_timer);
    t->is_active = true;
    vsf_callback_timer_add_isr(&t->cb_timer, t->period_ticks);
    return pdPASS;
}

BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    (void)xTicksToWait;
    if (xTimer == NULL) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    t->is_active = false;
    vsf_callback_timer_remove(&t->cb_timer);
    return pdPASS;
}

BaseType_t xTimerStopFromISR(TimerHandle_t xTimer,
                             BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if (xTimer == NULL) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    t->is_active = false;
    vsf_callback_timer_remove_isr(&t->cb_timer);
    return pdPASS;
}

BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    // FreeRTOS contract: semantically "stop then start from now".
    return xTimerStart(xTimer, xTicksToWait);
}

BaseType_t xTimerResetFromISR(TimerHandle_t xTimer,
                              BaseType_t *pxHigherPriorityTaskWoken)
{
    return xTimerStartFromISR(xTimer, pxHigherPriorityTaskWoken);
}

BaseType_t xTimerChangePeriod(TimerHandle_t xTimer,
                              TickType_t xNewPeriod,
                              TickType_t xTicksToWait)
{
    (void)xTicksToWait;
    if ((xTimer == NULL) || (xNewPeriod == 0)) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    vsf_callback_timer_remove(&t->cb_timer);
    t->period_ticks = xNewPeriod;
    t->is_active = true;
    vsf_callback_timer_add(&t->cb_timer, xNewPeriod);
    return pdPASS;
}

BaseType_t xTimerChangePeriodFromISR(TimerHandle_t xTimer,
                                     TickType_t xNewPeriod,
                                     BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((xTimer == NULL) || (xNewPeriod == 0)) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    vsf_callback_timer_remove_isr(&t->cb_timer);
    t->period_ticks = xNewPeriod;
    t->is_active = true;
    vsf_callback_timer_add_isr(&t->cb_timer, xNewPeriod);
    return pdPASS;
}

BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait)
{
    (void)xTicksToWait;
    if (xTimer == NULL) { return pdFAIL; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    // Race window note: a callback may still be queued in __frt_timer_q
    // when we get here. Removing from the kernel callback-timer list
    // stops new posts; the user is responsible for not deleting a timer
    // whose callback is still executing (same contract as FreeRTOS).
    t->is_active = false;
    vsf_callback_timer_remove(&t->cb_timer);
    if (!t->is_static) {
        vsf_heap_free(t);
    }
    return pdPASS;
}

BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer)
{
    if (xTimer == NULL) { return pdFALSE; }
    StaticTimer_t *t = (StaticTimer_t *)xTimer;
    return t->is_active ? pdTRUE : pdFALSE;
}

TickType_t xTimerGetPeriod(TimerHandle_t xTimer)
{
    if (xTimer == NULL) { return 0; }
    return ((StaticTimer_t *)xTimer)->period_ticks;
}

void *pvTimerGetTimerID(const TimerHandle_t xTimer)
{
    if (xTimer == NULL) { return NULL; }
    return ((StaticTimer_t *)xTimer)->pvTimerID;
}

void vTimerSetTimerID(TimerHandle_t xTimer, void *pvNewID)
{
    if (xTimer == NULL) { return; }
    ((StaticTimer_t *)xTimer)->pvTimerID = pvNewID;
}

const char *pcTimerGetName(TimerHandle_t xTimer)
{
    if (xTimer == NULL) { return NULL; }
    return ((StaticTimer_t *)xTimer)->name;
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_TIMERS
