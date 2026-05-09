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
 * FreeRTOS queue.h port over vsf_eda_queue + an inline ring buffer.
 *
 * Storage layout: a single heap block that holds the queue object followed
 * by the item ring buffer (flexible array member). Producers and consumers
 * share the ring, with vsf_eda_queue_t carrying the blocking semantics
 * (wait/notify). memcpy is used for item transfer so the shim is oblivious
 * to item alignment as long as the caller respects uxItemSize.
 *
 * Blocking strategy mirrors the AIC8800 rtos_al.c queue port:
 *   - fast path: vsf_eda_queue_send/recv returns immediately on success.
 *   - slow path: fall back to vsf_thread_wait + get_reason loop, which
 *     transparently consumes VSF_SYNC_PENDING wake-ups and exits on
 *     VSF_SYNC_GET / VSF_SYNC_TIMEOUT / VSF_SYNC_CANCEL.
 *
 * All storage is claimed on xQueueCreate via vsf_heap_malloc. Allowing the
 * stdlib fallback would hide real heap pressure from the bring-up target.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_QUEUE == ENABLED

#include <string.h>

#define __VSF_FREERTOS_QUEUE_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "queue.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "vsf_freertos queue port requires VSF_USE_HEAP == ENABLED"
#endif
#if !defined(VSF_USE_KERNEL) || VSF_USE_KERNEL != ENABLED
#   error "vsf_freertos queue port requires VSF_USE_KERNEL == ENABLED"
#endif

#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

#if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE != ENABLED
#   error "vsf_freertos queue port requires VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED"
#endif
#if VSF_EDA_QUEUE_CFG_SUPPORT_ISR != ENABLED
#   error "vsf_freertos queue port requires VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED"
#endif

/*============================ TYPES =========================================*/

// Control block = StaticQueue_t (vsf_class declared in queue.h). Heap
// and caller-provided static buffers share the same layout; the two
// is_*_static flags control who owns the storage at vQueueDelete time.

/*============================ PROTOTYPES ====================================*/

static bool __frt_queue_enqueue(vsf_eda_queue_t *base, void *item);
static bool __frt_queue_dequeue(vsf_eda_queue_t *base, void **item);

/*============================ IMPLEMENTATION ================================*/

static bool __frt_queue_enqueue(vsf_eda_queue_t *base, void *item)
{
    // vsf_eda_queue_t is the first embedded member; offset-0 upcast back.
    StaticQueue_t *q = (StaticQueue_t *)base;
    memcpy(&q->node_buffer[q->head * q->node_size], item, q->node_size);
    if (++q->head >= q->node_num) {
        q->head = 0;
    }
    return true;
}

static bool __frt_queue_dequeue(vsf_eda_queue_t *base, void **item)
{
    StaticQueue_t *q = (StaticQueue_t *)base;
    // vsf_eda_queue passes a `void **` slot; the ring stores raw item
    // bytes so we memcpy into *caller-provided* destination which itself
    // lives at `item`. Mirrors rtos_al.c semantics.
    memcpy(item, &q->node_buffer[q->tail * q->node_size], q->node_size);
    if (++q->tail >= q->node_num) {
        q->tail = 0;
    }
    return true;
}

QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize)
{
    if ((uxQueueLength == 0) || (uxItemSize == 0)) {
        return NULL;
    }
    if ((uxQueueLength > UINT16_MAX) || (uxItemSize > UINT16_MAX)) {
        return NULL;
    }

    StaticQueue_t *q = (StaticQueue_t *)vsf_heap_malloc(sizeof(*q));
    if (q == NULL) {
        return NULL;
    }
    uint8_t *storage = (uint8_t *)vsf_heap_malloc(
            (size_t)uxQueueLength * (size_t)uxItemSize);
    if (storage == NULL) {
        vsf_heap_free(q);
        return NULL;
    }
    memset(q, 0, sizeof(*q));
    q->node_buffer = storage;
    q->node_num    = (uint16_t)uxQueueLength;
    q->node_size   = (uint16_t)uxItemSize;
    q->op.enqueue  = __frt_queue_enqueue;
    q->op.dequeue  = __frt_queue_dequeue;
    vsf_eda_queue_init(&q->use_as__vsf_eda_queue_t, (uint_fast16_t)uxQueueLength);
    return (QueueHandle_t)q;
}

QueueHandle_t xQueueCreateStatic(UBaseType_t uxQueueLength,
                                 UBaseType_t uxItemSize,
                                 uint8_t *pucQueueStorage,
                                 StaticQueue_t *pxQueueBuffer)
{
    if ((uxQueueLength == 0) || (uxItemSize == 0)
            || (pucQueueStorage == NULL) || (pxQueueBuffer == NULL)) {
        return NULL;
    }
    if ((uxQueueLength > UINT16_MAX) || (uxItemSize > UINT16_MAX)) {
        return NULL;
    }
    StaticQueue_t *q = pxQueueBuffer;
    memset(q, 0, sizeof(*q));
    q->node_buffer       = pucQueueStorage;
    q->node_num          = (uint16_t)uxQueueLength;
    q->node_size         = (uint16_t)uxItemSize;
    q->is_static         = true;
    q->is_storage_static = true;
    q->op.enqueue        = __frt_queue_enqueue;
    q->op.dequeue        = __frt_queue_dequeue;
    vsf_eda_queue_init(&q->use_as__vsf_eda_queue_t, (uint_fast16_t)uxQueueLength);
    return (QueueHandle_t)q;
}

void vQueueDelete(QueueHandle_t xQueue)
{
    if (xQueue == NULL) {
        return;
    }
    StaticQueue_t *q = xQueue;
    // Wake any blocked waiters with a cancel before freeing storage.
    vsf_eda_queue_cancel(&q->use_as__vsf_eda_queue_t);
    if ((q->node_buffer != NULL) && !q->is_storage_static) {
        vsf_heap_free(q->node_buffer);
    }
    if (!q->is_static) {
        vsf_heap_free(q);
    }
}

BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue,
                      TickType_t xTicksToWait)
{
    if ((xQueue == NULL) || (pvItemToQueue == NULL)) {
        return pdFAIL;
    }

    vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
            ? (vsf_timeout_tick_t)-1 : xTicksToWait;

    // Fast path: if a slot is already free we land here.
    if (VSF_ERR_NONE == vsf_eda_queue_send(&xQueue->use_as__vsf_eda_queue_t,
                                            (void *)pvItemToQueue, timeout)) {
#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
        if (xQueue->pxQueueSetContainer != NULL) {
            xQueueSend((QueueHandle_t)xQueue->pxQueueSetContainer,
                       &xQueue, 0);
        }
#endif
        return pdTRUE;
    }

    // Nothing more to do when the caller asked for non-blocking behaviour.
    if (xTicksToWait == 0) {
        return pdFAIL;
    }

    // Slow path: drain PENDING evts until we either get the slot, time out,
    // or the queue gets cancelled from under us.
    vsf_sync_reason_t reason;
    for (;;) {
        reason = vsf_eda_queue_send_get_reason(
                        &xQueue->use_as__vsf_eda_queue_t,
                        vsf_thread_wait(),
                        (void *)pvItemToQueue);
        if (reason != VSF_SYNC_PENDING) {
            break;
        }
    }
    if (reason == VSF_SYNC_GET) {
#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
        if (xQueue->pxQueueSetContainer != NULL) {
            xQueueSend((QueueHandle_t)xQueue->pxQueueSetContainer,
                       &xQueue, 0);
        }
#endif
        return pdTRUE;
    }
    return pdFAIL;
}

// FreeRTOS contract: xQueueSendFromISR MUST be called from a real ISR
// context. Under the hood vsf_eda_queue_send_isr enqueues the item and
// posts a VSF_KERNEL_EVT_QUEUE_SEND_NOTIFY event to the VSF system task;
// the system task is what flips the item to "readable" and wakes any
// pending receiver. Calling this API from a task context is a semantic
// misuse - the event still gets posted, but you have broken the
// "producer interrupts consumer" assumption that the ISR API is built
// around (and pxHigherPriorityTaskWoken loses its meaning, since there
// is no portYIELD_FROM_ISR on the return path).
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue,
                             BaseType_t *pxHigherPriorityTaskWoken)
{
    if ((xQueue == NULL) || (pvItemToQueue == NULL)) {
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdFALSE;
        }
        return pdFAIL;
    }
    vsf_err_t err = vsf_eda_queue_send_isr(&xQueue->use_as__vsf_eda_queue_t,
                                            (void *)pvItemToQueue);
    BaseType_t woken = (err == VSF_ERR_NONE) ? pdTRUE : pdFALSE;
#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
    if ((err == VSF_ERR_NONE) && (xQueue->pxQueueSetContainer != NULL)) {
        BaseType_t woken2 = pdFALSE;
        (void)xQueueSendFromISR((QueueHandle_t)xQueue->pxQueueSetContainer,
                                &xQueue, &woken2);
        if (woken2 == pdTRUE) { woken = pdTRUE; }
    }
#endif
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = woken;
    }
    return (err == VSF_ERR_NONE) ? pdTRUE : pdFAIL;
}

BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer,
                         TickType_t xTicksToWait)
{
    if ((xQueue == NULL) || (pvBuffer == NULL)) {
        return pdFAIL;
    }

    vsf_timeout_tick_t timeout = (xTicksToWait == portMAX_DELAY)
            ? (vsf_timeout_tick_t)-1 : xTicksToWait;

    // dequeue callback treats its arg as the caller buffer; pass pvBuffer
    // directly. The `void **` parameter shape is inherited from the eda
    // queue contract.
    if (VSF_ERR_NONE == vsf_eda_queue_recv(&xQueue->use_as__vsf_eda_queue_t,
                                           (void **)pvBuffer, timeout)) {
        return pdTRUE;
    }

    if (xTicksToWait == 0) {
        return pdFAIL;
    }

    vsf_sync_reason_t reason;
    for (;;) {
        reason = vsf_eda_queue_recv_get_reason(
                        &xQueue->use_as__vsf_eda_queue_t,
                        vsf_thread_wait(),
                        (void **)pvBuffer);
        if (reason != VSF_SYNC_PENDING) {
            break;
        }
    }
    return (reason == VSF_SYNC_GET) ? pdTRUE : pdFAIL;
}

// FreeRTOS contract: xQueueReceiveFromISR MUST be called from a real
// ISR context. It consumes the "readable" slot that the system task has
// previously published on behalf of an earlier xQueueSendFromISR, so
// the producer-side notify event must have been processed before this
// can return pdTRUE. Calling this from a task is therefore a semantic
// misuse for the same reason as xQueueSendFromISR.
BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void *pvBuffer,
                                BaseType_t *pxHigherPriorityTaskWoken)
{
    if ((xQueue == NULL) || (pvBuffer == NULL)) {
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdFALSE;
        }
        return pdFAIL;
    }
    vsf_err_t err = vsf_eda_queue_recv_isr(&xQueue->use_as__vsf_eda_queue_t,
                                           (void **)pvBuffer);
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = (err == VSF_ERR_NONE) ? pdTRUE : pdFALSE;
    }
    return (err == VSF_ERR_NONE) ? pdTRUE : pdFAIL;
}

UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue)
{
    if (xQueue == NULL) {
        return 0;
    }
    return (UBaseType_t)vsf_eda_queue_get_cnt(&xQueue->use_as__vsf_eda_queue_t);
}

UBaseType_t uxQueueSpacesAvailable(QueueHandle_t xQueue)
{
    if (xQueue == NULL) {
        return 0;
    }
    uint_fast16_t used = vsf_eda_queue_get_cnt(&xQueue->use_as__vsf_eda_queue_t);
    return (UBaseType_t)((xQueue->node_num > used) ? (xQueue->node_num - used) : 0);
}

BaseType_t xQueueReset(QueueHandle_t xQueue)
{
    if (xQueue == NULL) {
        return pdFAIL;
    }
    // Cancel first to release any blocked senders/receivers with a safe
    // reason, then re-init the backing queue state. The ring cursors are
    // reset so the queue acts fresh.
    vsf_eda_queue_cancel(&xQueue->use_as__vsf_eda_queue_t);
    xQueue->head = 0;
    xQueue->tail = 0;
    vsf_eda_queue_init(&xQueue->use_as__vsf_eda_queue_t,
                       (uint_fast16_t)xQueue->node_num);
    return pdPASS;
}

#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED

QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength)
{
    return xQueueCreate(uxEventQueueLength, (UBaseType_t)sizeof(void *));
}

BaseType_t xQueueAddToSet(QueueSetMemberHandle_t xQueueOrSemaphore,
                          QueueSetHandle_t xQueueSet)
{
    if ((xQueueOrSemaphore == NULL) || (xQueueSet == NULL)) {
        return pdFALSE;
    }
    if ((QueueSetHandle_t)xQueueOrSemaphore == xQueueSet) {
        return pdFALSE;
    }
    StaticQueue_t *pxQueue = (StaticQueue_t *)xQueueOrSemaphore;
    if (pxQueue->pxQueueSetContainer != NULL) {
        return pdFALSE;
    }
    pxQueue->pxQueueSetContainer = xQueueSet;
    return pdTRUE;
}

BaseType_t xQueueRemoveFromSet(QueueSetMemberHandle_t xQueueOrSemaphore,
                               QueueSetHandle_t xQueueSet)
{
    if ((xQueueOrSemaphore == NULL) || (xQueueSet == NULL)) {
        return pdFALSE;
    }
    StaticQueue_t *pxQueue = (StaticQueue_t *)xQueueOrSemaphore;
    if (pxQueue->pxQueueSetContainer != xQueueSet) {
        return pdFALSE;
    }
    pxQueue->pxQueueSetContainer = NULL;
    return pdTRUE;
}

QueueSetMemberHandle_t xQueueSelectFromSet(QueueSetHandle_t xQueueSet,
                                           TickType_t xTicksToWait)
{
    QueueSetMemberHandle_t xReturn = NULL;
    (void)xQueueReceive((QueueHandle_t)xQueueSet, &xReturn, xTicksToWait);
    return xReturn;
}

QueueSetMemberHandle_t xQueueSelectFromSetFromISR(QueueSetHandle_t xQueueSet)
{
    QueueSetMemberHandle_t xReturn = NULL;
    (void)xQueueReceiveFromISR((QueueHandle_t)xQueueSet, &xReturn, NULL);
    return xReturn;
}

#endif      // VSF_FREERTOS_CFG_USE_QUEUESET

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_QUEUE
