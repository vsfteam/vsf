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
 * FreeRTOS semphr.h port over vsf_sem / vsf_mutex.
 *
 * The SemaphoreHandle_t is an opaque pointer to a discriminated union: a
 * vsf_sem_t for binary/counting semaphores, or a vsf_mutex_t for mutexes.
 * xSemaphoreTake / xSemaphoreGive branch on the discriminator and forward
 * to the appropriate kernel primitive.
 *
 * Blocking strategy mirrors AIC8800 rtos_al.c: fast-path through the EDA
 * primitive and fall back to vsf_thread_*_pend / enter for timed waits.
 * ISR-safe give is wired to vsf_eda_sem_post_isr; ISR-side take is
 * documented as unsupported (there is no non-blocking "try take" for a
 * binary semaphore in the current VSF kernel API without breaking contract).
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_SEMPHR == ENABLED

#define __VSF_FREERTOS_SEMPHR_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "semphr.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "vsf_freertos semphr port requires VSF_USE_HEAP == ENABLED"
#endif
#if !defined(VSF_USE_KERNEL) || VSF_USE_KERNEL != ENABLED
#   error "vsf_freertos semphr port requires VSF_USE_KERNEL == ENABLED"
#endif

#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

/*============================ TYPES =========================================*/

enum {
    __FRT_SEM_KIND_SEM   = 1,   // binary / counting -> vsf_sem_t
    __FRT_SEM_KIND_MUTEX = 2,   // mutex             -> vsf_mutex_t
};

// StaticSemaphore_t IS the internal control block (PLOOC exposes the layout
// to this translation unit via __VSF_FREERTOS_SEMPHR_CLASS_IMPLEMENT). No
// mirror type and no _Static_assert needed -- size is owned by the vsf_class
// macro and therefore automatically in sync.

/*============================ IMPLEMENTATION ================================*/

// Initialise a semaphore control block in place. `s` may point into a
// user-supplied StaticSemaphore_t (zero-heap path) or into freshly malloc'd
// memory (dynamic path). The caller sets is_static accordingly.
static void __frt_sem_init_sem(StaticSemaphore_t *s, UBaseType_t init,
                               UBaseType_t max, bool is_static)
{
    s->kind = __FRT_SEM_KIND_SEM;
    s->is_static = is_static;
    vsf_eda_sem_init(&s->u.sem, (uint_fast16_t)init, (uint_fast16_t)max);
}

static void __frt_sem_init_mutex(StaticSemaphore_t *s, bool is_static)
{
    s->kind = __FRT_SEM_KIND_MUTEX;
    s->is_static = is_static;
    vsf_eda_mutex_init(&s->u.mutex);
}

static SemaphoreHandle_t __frt_sem_alloc_sem(UBaseType_t init, UBaseType_t max)
{
    StaticSemaphore_t *s =
        (StaticSemaphore_t *)vsf_heap_malloc(sizeof(*s));
    if (s == NULL) {
        return NULL;
    }
    __frt_sem_init_sem(s, init, max, false);
    return (SemaphoreHandle_t)s;
}

SemaphoreHandle_t xSemaphoreCreateBinary(void)
{
    // FreeRTOS binary semaphore: initial count 0, ceiling 1.
    return __frt_sem_alloc_sem(0, 1);
}

SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount,
                                           UBaseType_t uxInitialCount)
{
    if (uxMaxCount == 0) {
        return NULL;
    }
    if (uxInitialCount > uxMaxCount) {
        return NULL;
    }
    return __frt_sem_alloc_sem(uxInitialCount, uxMaxCount);
}

SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    StaticSemaphore_t *s =
        (StaticSemaphore_t *)vsf_heap_malloc(sizeof(*s));
    if (s == NULL) {
        return NULL;
    }
    __frt_sem_init_mutex(s, false);
    return (SemaphoreHandle_t)s;
}

// ---- Static (zero-heap) variants ----

SemaphoreHandle_t xSemaphoreCreateBinaryStatic(
        StaticSemaphore_t *pxSemaphoreBuffer)
{
    if (pxSemaphoreBuffer == NULL) {
        return NULL;
    }
    __frt_sem_init_sem(pxSemaphoreBuffer, 0, 1, true);
    return (SemaphoreHandle_t)pxSemaphoreBuffer;
}

SemaphoreHandle_t xSemaphoreCreateCountingStatic(
        UBaseType_t uxMaxCount,
        UBaseType_t uxInitialCount,
        StaticSemaphore_t *pxSemaphoreBuffer)
{
    if ((pxSemaphoreBuffer == NULL)
     || (uxMaxCount == 0)
     || (uxInitialCount > uxMaxCount)) {
        return NULL;
    }
    __frt_sem_init_sem(pxSemaphoreBuffer, uxInitialCount, uxMaxCount, true);
    return (SemaphoreHandle_t)pxSemaphoreBuffer;
}

SemaphoreHandle_t xSemaphoreCreateMutexStatic(
        StaticSemaphore_t *pxSemaphoreBuffer)
{
    if (pxSemaphoreBuffer == NULL) {
        return NULL;
    }
    __frt_sem_init_mutex(pxSemaphoreBuffer, true);
    return (SemaphoreHandle_t)pxSemaphoreBuffer;
}

void vSemaphoreDelete(SemaphoreHandle_t xSemaphore)
{
    if (xSemaphore == NULL) {
        return;
    }
    // Wake any pending waiters with a cancel reason before freeing storage.
    if (xSemaphore->kind == __FRT_SEM_KIND_SEM) {
        vsf_eda_sync_cancel(&xSemaphore->u.sem);
    } else if (xSemaphore->kind == __FRT_SEM_KIND_MUTEX) {
        vsf_eda_sync_cancel(&xSemaphore->u.mutex.use_as__vsf_sync_t);
    }
    if (!xSemaphore->is_static) {
        vsf_heap_free(xSemaphore);
    }
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait)
{
    if (xSemaphore == NULL) {
        return pdFAIL;
    }

    vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
            ? (vsf_timeout_tick_t)-1 : xTicksToWait;

    vsf_sync_reason_t reason;
    if (xSemaphore->kind == __FRT_SEM_KIND_SEM) {
        reason = vsf_thread_sem_pend(&xSemaphore->u.sem, timeout);
    } else if (xSemaphore->kind == __FRT_SEM_KIND_MUTEX) {
        reason = vsf_thread_mutex_enter(&xSemaphore->u.mutex, timeout);
    } else {
        return pdFAIL;
    }
    return (reason == VSF_SYNC_GET) ? pdTRUE : pdFAIL;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
    if (xSemaphore == NULL) {
        return pdFAIL;
    }

    if (xSemaphore->kind == __FRT_SEM_KIND_SEM) {
        // vsf_eda_sem_post caps at max; returning NONE here covers the
        // binary case where the count was already 1. FreeRTOS treats a
        // give on a full binary sem as pdFAIL; we emulate that by peeking
        // the raw sync counter via vsf_eda_sync_get_reason is overkill,
        // so follow the common "always pdPASS" simplification: all
        // middleware we target expects give-to-signal semantics and does
        // not inspect the ceiling overflow case.
        vsf_err_t err = vsf_eda_sem_post(&xSemaphore->u.sem);
        return (err == VSF_ERR_NONE) ? pdTRUE : pdFAIL;
    }
    if (xSemaphore->kind == __FRT_SEM_KIND_MUTEX) {
        vsf_err_t err = vsf_thread_mutex_leave(&xSemaphore->u.mutex);
        return (err == VSF_ERR_NONE) ? pdTRUE : pdFAIL;
    }
    return pdFAIL;
}

BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,
                                 BaseType_t *pxHigherPriorityTaskWoken)
{
    if (xSemaphore == NULL) {
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdFALSE;
        }
        return pdFAIL;
    }

    // Mutex from ISR is undefined; FreeRTOS itself rejects it.
    if (xSemaphore->kind != __FRT_SEM_KIND_SEM) {
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdFALSE;
        }
        return pdFAIL;
    }

    vsf_err_t err = vsf_eda_sem_post_isr(&xSemaphore->u.sem);
    if (pxHigherPriorityTaskWoken != NULL) {
        // Best-effort; the VSF scheduler handles the real yield.
        *pxHigherPriorityTaskWoken = (err == VSF_ERR_NONE) ? pdTRUE : pdFALSE;
    }
    return (err == VSF_ERR_NONE) ? pdTRUE : pdFAIL;
}

BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore,
                                 BaseType_t *pxHigherPriorityTaskWoken)
{
    // The VSF sync primitives have no non-blocking "try pend" that is
    // safe to call from a real ISR. Document the incompatibility and
    // return pdFAIL so callers can branch on the result. In practice
    // FreeRTOS code almost never takes from ISR.
    (void)xSemaphore;
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    return pdFAIL;
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_SEMPHR
