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

// Clean-room FreeRTOS task.h shim for VSF.
//
// Scope of the initial MVP:
//  - vTaskDelay / vTaskDelayUntil      (cooperative sleep via vsf_thread)
//  - xTaskGetTickCount                 (vsf_systimer_get_tick)
//  - taskYIELD                         (vsf_thread_yield)
//  - xTaskCreate / xTaskCreateStatic   (spawn a vsf_thread worker)
//  - vTaskDelete                       (self-termination / best-effort)
//
// Suspend/Resume/Notify and the full task API surface are deferred; they
// will be layered on top of the same vsf_thread worker model.

#ifndef __VSF_FREERTOS_TASK_H__
#define __VSF_FREERTOS_TASK_H__

#include "FreeRTOS.h"

#if defined(__VSF_FREERTOS_TASK_CLASS_IMPLEMENT)
#   undef __VSF_FREERTOS_TASK_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

vsf_dcl_class(StaticTask_t)
typedef StaticTask_t *          TaskHandle_t;
typedef void (*TaskFunction_t)(void *);

#if VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED
#   define __VSF_FREERTOS_TASK_NOTIFY_FIELDS                                   \
        vsf_sem_t           notify_sem;                                        \
        uint32_t            notify_value;                                      \
        bool                notify_pending;
#else
#   define __VSF_FREERTOS_TASK_NOTIFY_FIELDS
#endif

// Task control block exposed to both the task port and the notify port.
// The vsf_thread_t MUST remain the FIRST member: xTaskGetCurrentTaskHandle
// returns (TaskHandle_t)vsf_thread_get_cur() and the notify port casts
// that back to StaticTask_t * -- the offset-0 upcast is what makes this
// round-trip sound.
vsf_class(StaticTask_t) {
    private_member(
        implement(vsf_thread_t)
        implement(vsf_thread_cb_t)
        void              (*entry)(void *);
        void *              arg;
        // Ownership flags for zero-heap xTaskCreateStatic: when set, the
        // corresponding storage was supplied by the caller and must NOT
        // be released to the heap.
        bool                is_static;
        bool                is_stack_static;
        __VSF_FREERTOS_TASK_NOTIFY_FIELDS
    )
};

/*============================ API ===========================================*/

// Delay the calling task by xTicksToDelay ticks. In this shim, 1 tick equals
// 1 ms (see pdMS_TO_TICKS); the underlying sleep uses vsf_thread_delay.
// MUST be called from a vsf_thread context.
extern void vTaskDelay(const TickType_t xTicksToDelay);

// Absolute-deadline variant. pxPreviousWakeTime is updated on return.
extern void vTaskDelayUntil(TickType_t * const pxPreviousWakeTime,
                            const TickType_t xTimeIncrement);

// Returns the current kernel tick count. Mapped to vsf_systimer milliseconds
// to keep pdMS_TO_TICKS(ms) semantics consistent.
extern TickType_t xTaskGetTickCount(void);
extern TickType_t xTaskGetTickCountFromISR(void);

// Yield the CPU to any equal-priority ready task.
#define taskYIELD()             vTaskYield()
extern void vTaskYield(void);

// Creates a FreeRTOS task backed by a vsf_thread worker. pvParameters is
// passed to pxTaskCode. usStackDepth is interpreted as bytes (see StackType_t
// typedef). uxPriority is mapped to VSF_FREERTOS_CFG_DEFAULT_VSF_PRIO in the
// MVP; board overrides can refine the mapping.
// Returns pdPASS on success, pdFAIL on error.
extern BaseType_t xTaskCreate(TaskFunction_t pxTaskCode,
                              const char * const pcName,
                              const uint32_t usStackDepth,
                              void * const pvParameters,
                              UBaseType_t uxPriority,
                              TaskHandle_t * const pxCreatedTask);

// Deletes a task. xTaskToDelete == NULL means self-delete.
// NOTE: self-delete from inside the task body is the only supported form in
// the MVP; deleting another task is best-effort and may leak the stack.
extern void vTaskDelete(TaskHandle_t xTaskToDelete);

// Static allocation variant. The caller supplies BOTH the stack buffer
// (ulStackDepth bytes -- the shim defines StackType_t as uint8_t so the
// unit is always bytes) and the task control block storage. Neither is
// ever passed to the heap; vTaskDelete(NULL) self-exits the thread but
// the caller-owned storage is NOT freed.
//
// Requirements on the caller:
//   - puxStackBuffer must be 8-byte aligned (StackType_t is uint8_t so
//     this is the caller's responsibility on declaration).
//   - ulStackDepth must cover the VSF page + guardian size minimum.
// Returns the task handle on success, NULL on argument errors.
extern TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                                      const char * const pcName,
                                      const uint32_t ulStackDepth,
                                      void * const pvParameters,
                                      UBaseType_t uxPriority,
                                      StackType_t * const puxStackBuffer,
                                      StaticTask_t * const pxTaskBuffer);

// Returns the handle of the calling task, or NULL if called from outside a
// vsf_thread context.
extern TaskHandle_t xTaskGetCurrentTaskHandle(void);

/*============================ SCHEDULER / CRITICAL ==========================*/

// Disable / re-enable preemptive task switching. Unlike the critical
// section pair, these do NOT mask interrupts; they only prevent the
// scheduler from picking a different task until the matching resume.
// Nesting is permitted; only the outermost pair actually engages the
// underlying vsf_sched_lock.
//
// xTaskResumeAll returns pdTRUE if a context switch was forced on the
// way out and pdFALSE otherwise. The shim always returns pdFALSE --
// callers that drive their own yield based on the return value will
// still work because the very next cooperative yield point will pick
// up any newly-ready tasks.
extern void       vTaskSuspendAll(void);
extern BaseType_t xTaskResumeAll(void);

// Scheduler-level critical section. Nestable. The FromISR variants
// return / accept the saved scheduler state so that the caller can
// keep it on the local stack -- matching FreeRTOS semantics on
// architectures where the state is a pushed PRIMASK.
extern void        vTaskEnterCritical(void);
extern void        vTaskExitCritical(void);
extern UBaseType_t vTaskEnterCriticalFromISR(void);
extern void        vTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptState);

/*============================ NOTIFICATIONS =================================*/

// FreeRTOS task notifications. The shim implements a single-slot model:
// every task owns a (sem, value, pending) triple and xTaskNotify updates
// value according to eNotifyAction before signalling the sem.

typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite,
} eNotifyAction;

// Update a task's notification value and wake it. Returns pdPASS, or
// pdFAIL when eSetValueWithoutOverwrite is used on a task that already
// has a pending unclaimed notification.
extern BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify,
                              uint32_t ulValue,
                              eNotifyAction eAction);

// ISR-context variant. pxHigherPriorityTaskWoken is informational.
extern BaseType_t xTaskNotifyFromISR(TaskHandle_t xTaskToNotify,
                                     uint32_t ulValue,
                                     eNotifyAction eAction,
                                     BaseType_t *pxHigherPriorityTaskWoken);

// Convenience shortcuts: xTaskNotify(task, 0, eIncrement).
extern BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
extern void       vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify,
                                         BaseType_t *pxHigherPriorityTaskWoken);

// Block the calling task until a notification arrives. Returns the value
// of the notification at the point the wait completed; if
// xClearCountOnExit == pdTRUE the value is zeroed on successful exit,
// otherwise it is decremented by one (counting semaphore semantics).
extern uint32_t   ulTaskNotifyTake(BaseType_t xClearCountOnExit,
                                   TickType_t xTicksToWait);

// Block with bitmask semantics. On entry the bits in
// ulBitsToClearOnEntry are cleared; on successful return the bits in
// ulBitsToClearOnExit are cleared. pulNotificationValue receives the
// value observed at wait completion (may be NULL).
// Returns pdPASS on success, pdFAIL on timeout / invalid context.
extern BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry,
                                  uint32_t ulBitsToClearOnExit,
                                  uint32_t *pulNotificationValue,
                                  TickType_t xTicksToWait);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_FREERTOS_TASK_H__
