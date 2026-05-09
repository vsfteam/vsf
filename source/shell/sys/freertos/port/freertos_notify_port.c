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
 * FreeRTOS task notification port over vsf_sem.
 *
 * The MVP implements a single-slot model: each task owns a
 *   (vsf_sem_t notify_sem, uint32_t notify_value, bool notify_pending)
 * triple living inside StaticTask_t (see include/task.h).
 *
 *   - xTaskNotify updates notify_value according to eNotifyAction and
 *     posts notify_sem exactly once per "transition into pending". The
 *     semaphore is created with max=1, so back-to-back notifies merge
 *     into a single wake-up, matching FreeRTOS merge-on-arrival semantics.
 *   - ulTaskNotifyTake / xTaskNotifyWait block on notify_sem, then drain
 *     notify_value according to the FreeRTOS contract (zero / decrement /
 *     mask-clear on exit).
 *
 * All mutations of notify_value / notify_pending are performed inside
 * vsf_protect_sched to serialise concurrent notifiers (task + ISR).
 *
 * NOTE: xTaskNotifyWait / ulTaskNotifyTake target the CALLING task (the
 * FreeRTOS contract only allows a task to wait on its own notification
 * slot). We retrieve that task via vsf_thread_get_cur() which, thanks to
 * the "vsf_thread_t is first member" invariant on StaticTask_t, is
 * cast-compatible with the TaskHandle_t stored in xTaskCreate.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if     VSF_USE_FREERTOS == ENABLED \
    &&  VSF_FREERTOS_CFG_USE_TASK == ENABLED \
    &&  VSF_FREERTOS_CFG_USE_NOTIFY == ENABLED

#define __VSF_FREERTOS_TASK_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "task.h"

#include "../vsf_freertos.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL PROTOTYPES ==============================*/

// Central update routine: apply eAction to (value, pending) in a scheduler
// critical section. Returns true iff the caller should post notify_sem
// (i.e. the task was not already pending -- so we do not exceed sem max=1).
// Also returns pdFAIL through *paccept for the "WithoutOverwrite" case when
// a prior notification is still pending and would be clobbered.
static bool __frt_notify_apply(StaticTask_t       *ft,
                               uint32_t            value,
                               eNotifyAction       action,
                               BaseType_t         *paccept);

/*============================ IMPLEMENTATION ================================*/

static bool __frt_notify_apply(StaticTask_t       *ft,
                               uint32_t            value,
                               eNotifyAction       action,
                               BaseType_t         *paccept)
{
    bool need_post = false;
    BaseType_t accept = pdPASS;

    vsf_protect_t orig = vsf_protect_sched();
        bool was_pending = ft->notify_pending;
        switch (action) {
        case eNoAction:
            // No value change; still promotes a wake-up because FreeRTOS
            // treats eNoAction as an unblock-only signal.
            break;
        case eSetBits:
            ft->notify_value |= value;
            break;
        case eIncrement:
            ft->notify_value += 1u;
            break;
        case eSetValueWithOverwrite:
            ft->notify_value = value;
            break;
        case eSetValueWithoutOverwrite:
            if (was_pending) {
                // Target already has an unclaimed notification -- refuse.
                accept = pdFAIL;
            } else {
                ft->notify_value = value;
            }
            break;
        default:
            accept = pdFAIL;
            break;
        }

        if (accept == pdPASS) {
            ft->notify_pending = true;
            // Only transitioning "not pending -> pending" must post the
            // sem; a second notify while one is already pending must not,
            // otherwise the subsequent Take would return twice for one
            // logical notification.
            need_post = !was_pending;
        }
    vsf_unprotect_sched(orig);

    if (paccept != NULL) {
        *paccept = accept;
    }
    return need_post;
}

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify,
                       uint32_t     ulValue,
                       eNotifyAction eAction)
{
    if (xTaskToNotify == NULL) {
        return pdFAIL;
    }
    StaticTask_t *ft = (StaticTask_t *)xTaskToNotify;

    BaseType_t accept = pdPASS;
    bool need_post = __frt_notify_apply(ft, ulValue, eAction, &accept);
    if (accept != pdPASS) {
        return pdFAIL;
    }
    if (need_post) {
        vsf_eda_sem_post(&ft->notify_sem);
    }
    return pdPASS;
}

BaseType_t xTaskNotifyFromISR(TaskHandle_t xTaskToNotify,
                              uint32_t     ulValue,
                              eNotifyAction eAction,
                              BaseType_t  *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if (xTaskToNotify == NULL) {
        return pdFAIL;
    }
    StaticTask_t *ft = (StaticTask_t *)xTaskToNotify;

    BaseType_t accept = pdPASS;
    bool need_post = __frt_notify_apply(ft, ulValue, eAction, &accept);
    if (accept != pdPASS) {
        return pdFAIL;
    }
    if (need_post) {
        vsf_err_t err = vsf_eda_sem_post_isr(&ft->notify_sem);
        if (pxHigherPriorityTaskWoken != NULL) {
            // Best-effort hint; the VSF scheduler handles the real yield.
            *pxHigherPriorityTaskWoken = (err == VSF_ERR_NONE) ? pdTRUE : pdFALSE;
        }
    }
    return pdPASS;
}

BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify)
{
    return xTaskNotify(xTaskToNotify, 0u, eIncrement);
}

void vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify,
                            BaseType_t *pxHigherPriorityTaskWoken)
{
    (void)xTaskNotifyFromISR(xTaskToNotify, 0u, eIncrement,
                             pxHigherPriorityTaskWoken);
}

uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit,
                          TickType_t xTicksToWait)
{
    // Only the calling task can wait on its own notification slot.
    // Map vsf_thread_t * back to StaticTask_t * via the
    // "first member" invariant.
    StaticTask_t *ft = (StaticTask_t *)vsf_thread_get_cur();
    if (ft == NULL) {
        return 0u;
    }

    vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
            ? (vsf_timeout_tick_t)-1 : xTicksToWait;

    vsf_sync_reason_t reason = vsf_thread_sem_pend(&ft->notify_sem, timeout);
    if (reason != VSF_SYNC_GET) {
        return 0u;     // timeout / cancel -> FreeRTOS returns 0
    }

    uint32_t returned;
    bool     republish = false;
    vsf_protect_t orig = vsf_protect_sched();
        returned = ft->notify_value;
        if (xClearCountOnExit == pdTRUE) {
            ft->notify_value   = 0u;
            ft->notify_pending = false;
        } else {
            // Counting-sem semantics: decrement by one, keep "pending"
            // state (and republish the sem) while the counter stays > 0.
            if (ft->notify_value > 0u) {
                ft->notify_value -= 1u;
            }
            if (ft->notify_value > 0u) {
                republish = true;            // more to claim
            } else {
                ft->notify_pending = false;
            }
        }
    vsf_unprotect_sched(orig);

    if (republish) {
        // Re-arm the sem so the next ulTaskNotifyTake returns immediately
        // without blocking, preserving the counting-semaphore contract.
        vsf_eda_sem_post(&ft->notify_sem);
    }
    return returned;
}

BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry,
                           uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue,
                           TickType_t xTicksToWait)
{
    StaticTask_t *ft = (StaticTask_t *)vsf_thread_get_cur();
    if (ft == NULL) {
        if (pulNotificationValue != NULL) { *pulNotificationValue = 0u; }
        return pdFAIL;
    }

    // Entry-clear mask: applied before blocking.
    vsf_protect_t orig = vsf_protect_sched();
        ft->notify_value &= ~ulBitsToClearOnEntry;
    vsf_unprotect_sched(orig);

    vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
            ? (vsf_timeout_tick_t)-1 : xTicksToWait;

    vsf_sync_reason_t reason = vsf_thread_sem_pend(&ft->notify_sem, timeout);
    if (reason != VSF_SYNC_GET) {
        if (pulNotificationValue != NULL) {
            // On timeout FreeRTOS returns the current value anyway.
            orig = vsf_protect_sched();
                *pulNotificationValue = ft->notify_value;
            vsf_unprotect_sched(orig);
        }
        return pdFAIL;
    }

    uint32_t snap;
    orig = vsf_protect_sched();
        snap = ft->notify_value;
        ft->notify_value  &= ~ulBitsToClearOnExit;
        ft->notify_pending = false;
    vsf_unprotect_sched(orig);

    if (pulNotificationValue != NULL) {
        *pulNotificationValue = snap;
    }
    return pdPASS;
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_TASK && VSF_FREERTOS_CFG_USE_NOTIFY
