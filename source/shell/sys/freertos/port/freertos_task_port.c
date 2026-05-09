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
 * Port implementation for "task.h" on VSF.
 *
 * FreeRTOS tasks are backed by vsf_thread workers:
 *  - xTaskCreate spawns a vsf_thread with a heap-allocated stack and a
 *    trampoline that invokes the user TaskFunction_t.
 *  - vTaskDelay routes to vsf_thread_delay, mapping 1 tick == 1 ms (see
 *    pdMS_TO_TICKS in FreeRTOS.h).
 *  - taskYIELD is vsf_thread_yield.
 *  - xTaskGetTickCount returns the systimer reading in milliseconds.
 *  - vTaskDelete(NULL) performs a clean self-exit via vsf_thread_exit.
 *
 * Priority handling: the MVP collapses any FreeRTOS priority to
 * VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO. A richer mapping can be introduced
 * without breaking callers.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_TASK == ENABLED

#define __VSF_FREERTOS_TASK_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "task.h"

#include "../vsf_freertos.h"
#include "kernel/vsf_kernel.h"

#if defined(VSF_USE_HEAP) && VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#else
#   error "VSF_FREERTOS_CFG_USE_TASK requires VSF_USE_HEAP"
#endif

#include <string.h>
#include <stddef.h>

/*============================ TYPES =========================================*/

// StaticTask_t is the task control block (see include/task.h). It is
// shared with the notify port, which relies on the same vsf_thread_t
// first-member invariant.

/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
static void __freertos_task_wrapper(vsf_thread_cb_t *cb)
{
    StaticTask_t *ft = (StaticTask_t *)
        vsf_container_of(cb, StaticTask_t, use_as__vsf_thread_cb_t);
    if (ft->entry != NULL) {
        ft->entry(ft->arg);
    }
    // FreeRTOS contract forbids returning from a task body; if it happens,
    // terminate the vsf_thread cleanly. The StaticTask_t backing
    // storage is intentionally leaked in the MVP (self-cleanup would race
    // with the scheduler that is still referencing our fields).
    vsf_thread_exit();
}
#else
static void __freertos_task_wrapper(vsf_thread_t *t)
{
    StaticTask_t *ft = (StaticTask_t *)
        vsf_container_of(t, StaticTask_t, use_as__vsf_thread_t);
    if (ft->entry != NULL) {
        ft->entry(ft->arg);
    }
    vsf_thread_exit();
}
#endif

static void __freertos_task_on_terminate(vsf_eda_t *eda)
{
    StaticTask_t *ft = (StaticTask_t *)eda;
    if (!ft->is_stack_static) {
        vsf_heap_free(ft->stack);
    }
    if (!ft->is_static) {
        vsf_heap_free(ft);
    }
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    vsf_thread_delay(xTicksToDelay);
}

void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime,
                     const TickType_t xTimeIncrement)
{
    if (pxPreviousWakeTime == NULL) {
        return;
    }
    TickType_t now  = xTaskGetTickCount();
    TickType_t next = *pxPreviousWakeTime + xTimeIncrement;
    *pxPreviousWakeTime = next;
    // Signed compare handles tick counter wrap.
    int32_t delta = (int32_t)(next - now);
    if (delta > 0) {
        vsf_thread_delay(delta);
    }
}

TickType_t xTaskGetTickCount(void)
{
    return vsf_systimer_get_tick();
}

TickType_t xTaskGetTickCountFromISR(void)
{
    return xTaskGetTickCount();
}

void vTaskYield(void)
{
    vsf_thread_yield();
}

// Normalise usStackDepth against the VSF kernel's page/guardian alignment
// rules. Returns the (possibly grown / rounded) byte count to hand to
// vsf_thread_start. For dynamic creates we're free to grow the caller's
// request; for static creates the caller owns the buffer so this must be
// used as a strict validator (see xTaskCreateStatic).
static uint32_t __frt_task_round_stack(uint32_t stack_bytes)
{
    if (stack_bytes < VSF_FREERTOS_CFG_MIN_STACK_BYTES) {
        stack_bytes = VSF_FREERTOS_CFG_MIN_STACK_BYTES;
    }
    uint32_t page = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE;
    uint32_t guard = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE;
    uint32_t min_bytes = page + guard;
    if (stack_bytes < min_bytes) {
        stack_bytes = min_bytes;
    }
    // round up to page boundary then 8-byte align.
    stack_bytes = (stack_bytes + (page - 1u)) & ~(page - 1u);
    stack_bytes = (stack_bytes + 7u) & ~7u;
    return stack_bytes;
}

// Wire up the pre-populated StaticTask_t fields and spawn the
// underlying vsf_thread worker. `ft->entry/arg/stack/stack_bytes` and
// ownership flags must already be set by the caller.
static vsf_err_t __frt_task_spawn(StaticTask_t *ft)
{
#if VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED
    vsf_eda_sem_init(&ft->notify_sem, 0, 1);
    ft->notify_value   = 0;
    ft->notify_pending = false;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    ft->entry             = (vsf_thread_entry_t *)__freertos_task_wrapper;
    ft->on_terminate      = __freertos_task_on_terminate;
    ft->stack             = ft->stack;
    return vsf_thread_start(&ft->use_as__vsf_thread_t, &ft->use_as__vsf_thread_cb_t,
                            VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO);
#else
    ft->thread.entry      = (vsf_thread_entry_t *)__freertos_task_wrapper;
    ft->on_terminate      = __freertos_task_on_terminate;
    ft->thread.stack      = ft->stack;
    ft->thread.stack_size = ft->stack_bytes;
    return vsf_thread_start(&ft->thread,
                            VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO);
#endif
}

VSF_CAL_WEAK(xTaskCreate)
BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                       const char * const pcName,
                       const uint32_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pxCreatedTask)
{
    (void)pcName;
    (void)uxPriority;

    if (pxTaskCode == NULL) {
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }

    uint32_t stack_bytes = __frt_task_round_stack(usStackDepth);

    StaticTask_t *ft = (StaticTask_t *)vsf_heap_malloc(sizeof(*ft));
    if (ft == NULL) {
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }
    void *stack = vsf_heap_malloc_aligned(stack_bytes, 8);
    if (stack == NULL) {
        vsf_heap_free(ft);
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }
    memset(ft, 0, sizeof(*ft));
    ft->freertos_entry  = (void (*)(void *))pxTaskCode;
    ft->arg             = pvParameters;
    ft->stack           = stack;
    ft->stack_size      = stack_bytes;
    ft->is_static       = false;
    ft->is_stack_static = false;

    vsf_err_t err = __frt_task_spawn(ft);
    if (err != VSF_ERR_NONE) {
        vsf_heap_free(stack);
        vsf_heap_free(ft);
        if (pxCreatedTask != NULL) { *pxCreatedTask = NULL; }
        return pdFAIL;
    }

    if (pxCreatedTask != NULL) { *pxCreatedTask = (TaskHandle_t)ft; }
    return pdPASS;
}

VSF_CAL_WEAK(xTaskCreateStatic)
TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                               const char * const pcName,
                               const uint32_t ulStackDepth,
                               void * const pvParameters,
                               UBaseType_t uxPriority,
                               StackType_t * const puxStackBuffer,
                               StaticTask_t * const pxTaskBuffer)
{
    (void)pcName;
    (void)uxPriority;

    if ((pxTaskCode == NULL) || (puxStackBuffer == NULL)
            || (pxTaskBuffer == NULL)) {
        return NULL;
    }
    // User-owned stack buffer: enforce the same page/guardian/alignment
    // floor as the dynamic path without secretly growing the caller's
    // allocation. ulStackDepth is in bytes (StackType_t == uint8_t).
    if (ulStackDepth < VSF_FREERTOS_CFG_MIN_STACK_BYTES) {
        return NULL;
    }
    uint32_t page = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE;
    uint32_t guard = (uint32_t)VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE;
    if (ulStackDepth < (page + guard)) {
        return NULL;
    }
    if ((ulStackDepth & (page - 1u)) != 0u) {
        return NULL;
    }
    if ((ulStackDepth & 7u) != 0u) {
        return NULL;
    }
    // 8-byte alignment of the stack buffer itself is the caller's
    // responsibility; StaticTask_t is uint64_t[]-backed so is aligned
    // by construction.
    if (((uintptr_t)puxStackBuffer & 7u) != 0u) {
        return NULL;
    }

    StaticTask_t *ft = (StaticTask_t *)pxTaskBuffer;
    memset(ft, 0, sizeof(*ft));
    ft->freertos_entry  = (void (*)(void *))pxTaskCode;
    ft->arg             = pvParameters;
    ft->stack           = (void *)puxStackBuffer;
    ft->stack_size      = ulStackDepth;
    ft->is_static       = true;
    ft->is_stack_static = true;

    vsf_err_t err = __frt_task_spawn(ft);
    if (err != VSF_ERR_NONE) {
        return NULL;
    }
    return (TaskHandle_t)ft;
}

VSF_CAL_WEAK(vTaskDelete)
void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    if (xTaskToDelete == NULL) {
        // Self-delete: terminate the vsf_thread cleanly. Backing storage
        // is leaked in MVP; caller-owned cleanup would race the scheduler.
        vsf_thread_exit();
        // unreachable
    }
    // Deleting another task is not safely supported in the MVP.
    (void)xTaskToDelete;
}

VSF_CAL_WEAK(xTaskGetCurrentTaskHandle)
TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    // The opaque handle we expose to FreeRTOS callers is the vsf_thread_t *
    // of the calling task. Callers typically only use it for NULL checks or
    // to round-trip through other FreeRTOS APIs.
    return (TaskHandle_t)vsf_thread_get_cur();
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_TASK
