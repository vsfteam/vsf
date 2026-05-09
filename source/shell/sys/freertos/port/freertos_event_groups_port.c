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
 * FreeRTOS event_groups.h port over vsf_bmpevt_t.
 *
 * Storage: a single heap block with a vsf_bmpevt_t and zero adapters. Set
 * / clear are forwarded to vsf_eda_bmpevt_set / reset; waits are layered on
 * vsf_thread_bmpevt_pend with a transient vsf_bmpevt_pender_t on the
 * caller stack. The pender->op selects AND vs OR matching, translating the
 * FreeRTOS "wait for all bits" flag 1:1.
 *
 * FreeRTOS returns different semantics from set vs clear:
 *   - xEventGroupSetBits   returns the group bits AFTER the set.
 *   - xEventGroupClearBits returns the group bits BEFORE the clear.
 *   - xEventGroupWaitBits  returns a snapshot AT wait completion.
 * We sample via a short protect_sched region to keep the read atomic with
 * respect to the kernel scheduler; ISR-side accesses are equally safe
 * because vsf_protect_sched already masks interrupts on this platform.
 *
 * Backing access to bmp.value requires __VSF_EDA_CLASS_INHERIT__ to expose
 * the private member of vsf_bmpevt_t.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_EVENT_GROUPS == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_FREERTOS_EVENT_GROUPS_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "event_groups.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "vsf_freertos event_groups port requires VSF_USE_HEAP == ENABLED"
#endif
#if !defined(VSF_USE_KERNEL) || VSF_USE_KERNEL != ENABLED
#   error "vsf_freertos event_groups port requires VSF_USE_KERNEL == ENABLED"
#endif

#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT != ENABLED
#   error "vsf_freertos event_groups port requires VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED"
#endif

#include <string.h>

/*============================ TYPES =========================================*/

// Control block = StaticEventGroup_t (vsf_class declared in event_groups.h).
// Heap allocations and caller-provided static buffers share the same layout;
// the is_static flag controls whether vEventGroupDelete releases storage.

/*============================ IMPLEMENTATION ================================*/

static void __frt_evg_init(StaticEventGroup_t *eg, bool is_static)
{
    memset(eg, 0, sizeof(*eg));
    // adapter_count == 0 -> adapters[] is never dereferenced by the kernel.
    vsf_eda_bmpevt_init(&eg->bmp, 0);
    eg->is_static = is_static;
}

static EventBits_t __frt_evg_sample(StaticEventGroup_t *eg)
{
    // Atomically read bmp.value; a short protect_sched region is enough
    // because the only writers (set/reset) run under the same protect.
    vsf_protect_t p = vsf_protect_sched();
        EventBits_t v = (EventBits_t)eg->bmp.value;
    vsf_unprotect_sched(p);
    return v;
}

EventGroupHandle_t xEventGroupCreate(void)
{
    StaticEventGroup_t *eg = (StaticEventGroup_t *)vsf_heap_malloc(sizeof(*eg));
    if (eg == NULL) {
        return NULL;
    }
    __frt_evg_init(eg, false);
    return (EventGroupHandle_t)eg;
}

EventGroupHandle_t xEventGroupCreateStatic(
        StaticEventGroup_t *pxEventGroupBuffer)
{
    if (pxEventGroupBuffer == NULL) {
        return NULL;
    }
    __frt_evg_init(pxEventGroupBuffer, true);
    return (EventGroupHandle_t)pxEventGroupBuffer;
}

void vEventGroupDelete(EventGroupHandle_t xEventGroup)
{
    if (xEventGroup == NULL) {
        return;
    }
    StaticEventGroup_t *eg = (StaticEventGroup_t *)xEventGroup;
    // Wake any pending waiters with a cancel before releasing storage.
    // Pass a full mask so the kernel walks the entire pending list.
    vsf_eda_bmpevt_cancel(&eg->bmp, 0xFFFFFFFFu);
    if (!eg->is_static) {
        vsf_heap_free(eg);
    }
}

EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup,
                               const EventBits_t uxBitsToSet)
{
    if (xEventGroup == NULL) {
        return 0;
    }
    StaticEventGroup_t *eg = (StaticEventGroup_t *)xEventGroup;
    // vsf_eda_bmpevt_set updates bmp.value and triggers a poll cycle that
    // releases any pender whose criteria is now satisfied.
    vsf_eda_bmpevt_set(&eg->bmp, (uint_fast32_t)uxBitsToSet);
    return __frt_evg_sample(eg);
}

BaseType_t xEventGroupSetBitsFromISR(EventGroupHandle_t xEventGroup,
                                     const EventBits_t uxBitsToSet,
                                     BaseType_t *pxHigherPriorityTaskWoken)
{
    if (xEventGroup == NULL) {
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdFALSE;
        }
        return pdFAIL;
    }
    // vsf_eda_bmpevt_set uses vsf_protect_sched internally, which masks
    // interrupts on this platform, so it is safe to call from an ISR.
    (void)xEventGroupSetBits(xEventGroup, uxBitsToSet);
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    return pdPASS;
}

EventBits_t xEventGroupClearBits(EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToClear)
{
    if (xEventGroup == NULL) {
        return 0;
    }
    StaticEventGroup_t *eg = (StaticEventGroup_t *)xEventGroup;
    // FreeRTOS contract: return the bits as they were BEFORE the clear.
    EventBits_t before = __frt_evg_sample(eg);
    vsf_eda_bmpevt_reset(&eg->bmp, (uint_fast32_t)uxBitsToClear);
    return before;
}

BaseType_t xEventGroupClearBitsFromISR(EventGroupHandle_t xEventGroup,
                                       const EventBits_t uxBitsToClear)
{
    if (xEventGroup == NULL) {
        return pdFAIL;
    }
    (void)xEventGroupClearBits(xEventGroup, uxBitsToClear);
    return pdPASS;
}

EventBits_t xEventGroupGetBits(EventGroupHandle_t xEventGroup)
{
    if (xEventGroup == NULL) {
        return 0;
    }
    return __frt_evg_sample((StaticEventGroup_t *)xEventGroup);
}

EventBits_t xEventGroupGetBitsFromISR(EventGroupHandle_t xEventGroup)
{
    return xEventGroupGetBits(xEventGroup);
}

EventBits_t xEventGroupWaitBits(EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToWaitFor,
                                const BaseType_t xClearOnExit,
                                const BaseType_t xWaitForAllBits,
                                TickType_t xTicksToWait)
{
    if ((xEventGroup == NULL) || (uxBitsToWaitFor == 0)) {
        return 0;
    }
    StaticEventGroup_t *eg = (StaticEventGroup_t *)xEventGroup;

    vsf_bmpevt_pender_t pender = { 0 };
    pender.mask = (uint32_t)uxBitsToWaitFor;
    pender.op   = (xWaitForAllBits == pdTRUE) ? VSF_BMPEVT_AND : VSF_BMPEVT_OR;

    // Fast-path: a timeout=0 pend acts as a non-blocking poll. If the
    // wait condition is already satisfied we never enqueue to the
    // pending_list, so neither TIMER nor SYNC_POLL races can intercept.
    // (vsf_eda_bmpevt_pend() with timeout==0 only walks __vsf_eda_bmpevt_poll
    // and returns NONE / NOT_READY.)
    vsf_err_t try_err = vsf_eda_bmpevt_pend(&eg->bmp, &pender, 0);
    vsf_sync_reason_t reason;
    if (try_err == VSF_ERR_NONE) {
        reason = VSF_SYNC_GET;
    } else if (xTicksToWait == 0) {
        reason = VSF_SYNC_TIMEOUT;
    } else {
        vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
                ? (vsf_timeout_tick_t)-1 : xTicksToWait;
        reason = vsf_thread_bmpevt_pend(&eg->bmp, &pender, timeout);
    }

    // Sample the bits at wait completion. FreeRTOS returns this snapshot
    // whether or not the wait condition was satisfied (on timeout the
    // caller inspects the returned bits against uxBitsToWaitFor).
    EventBits_t snap = __frt_evg_sample(eg);

    // Clear matched bits on a successful wait if the caller asked for it.
    if ((reason == VSF_SYNC_GET) && (xClearOnExit == pdTRUE)) {
        EventBits_t matched = snap & uxBitsToWaitFor;
        if (matched != 0) {
            vsf_eda_bmpevt_reset(&eg->bmp, (uint_fast32_t)matched);
        }
    }
    return snap;
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_EVENT_GROUPS
