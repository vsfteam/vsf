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

// Clean-room FreeRTOS queue.h shim for VSF.
//
// Exposes the FreeRTOS queue primitive subset required by ESP-IDF v5.x and
// typical middleware / networking stacks. The backing store is a plain
// ring buffer with a vsf_eda_queue_t attached for blocking semantics.
//
// Subset covered (MVP):
//   xQueueCreate / vQueueDelete
//   xQueueSend  / xQueueSendToBack  (aliases)
//   xQueueSendFromISR
//   xQueueReceive
//   xQueueReceiveFromISR
//   uxQueueMessagesWaiting
//   uxQueueSpacesAvailable
//   xQueueReset
//
// Not covered yet (add on demand):
//   xQueueSendToFront / xQueuePeek.

#ifndef __VSF_FREERTOS_QUEUE_H__
#define __VSF_FREERTOS_QUEUE_H__

#include "FreeRTOS.h"

#if defined(__VSF_FREERTOS_QUEUE_CLASS_IMPLEMENT)
#   undef __VSF_FREERTOS_QUEUE_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

// Forward-declared vsf_class. QueueHandle_t is a StaticQueue_t *; the
// inline ring cursors, state flags and embedded vsf_eda_queue_t are all
// private to the implementation (see port/freertos_queue_port.c).
vsf_dcl_class(StaticQueue_t)
typedef StaticQueue_t * QueueHandle_t;

#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
#ifndef __VSF_FREERTOS_QUEUESET_TYPES_DEFINED__
#   define __VSF_FREERTOS_QUEUESET_TYPES_DEFINED__
typedef void * QueueSetHandle_t;
typedef void * QueueSetMemberHandle_t;
#endif
#endif

vsf_class(StaticQueue_t) {
    private_member(
        // vsf_eda_queue_t MUST stay first in the layout: the kernel
        // walks container_of via the op vtable, which relies on an
        // offset-0 embedding. `implement()` produces an anonymous
        // union giving the implementation file transparent access.
        implement(vsf_eda_queue_t)

        uint16_t    head;           // write cursor (next enqueue slot)
        uint16_t    tail;           // read cursor  (next dequeue slot)
        uint16_t    node_num;       // ring capacity in slots
        uint16_t    node_size;      // bytes per slot
        bool        is_static;      // control block is user-owned
        bool        is_storage_static; // item storage is user-owned

        uint8_t *   node_buffer;    // ring storage (heap or user-supplied)

#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
        QueueSetHandle_t pxQueueSetContainer;   // NULL if not in a set
#endif
    )
};

/*============================ PROTOTYPES ====================================*/

// Allocate a queue that stores uxQueueLength items of uxItemSize bytes each.
// Returns NULL on failure (invalid args or heap exhaustion).
extern QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength,
                                  UBaseType_t uxItemSize);

// Zero-heap create. The caller provides BOTH the item storage buffer
// (uxQueueLength * uxItemSize bytes, any alignment) and the state
// buffer. Neither is freed by vQueueDelete.
extern QueueHandle_t xQueueCreateStatic(UBaseType_t uxQueueLength,
                                        UBaseType_t uxItemSize,
                                        uint8_t *pucQueueStorage,
                                        StaticQueue_t *pxQueueBuffer);

// Free a queue. Any pending senders/receivers are woken with failure.
extern void          vQueueDelete(QueueHandle_t xQueue);

// Copy one item into the queue (tail). If the queue is full and
// xTicksToWait > 0 the caller is blocked up to that many ticks.
// Returns pdTRUE on success, pdFALSE on timeout / invalid args.
extern BaseType_t    xQueueSend(QueueHandle_t xQueue,
                                const void *pvItemToQueue,
                                TickType_t xTicksToWait);

// Alias required by FreeRTOS API.
#define xQueueSendToBack(q, item, ticks)  xQueueSend((q), (item), (ticks))

// ISR-context enqueue. pxHigherPriorityTaskWoken may be NULL; when non-NULL
// it is set to pdFALSE on success / pdTRUE when a waiter was released.
extern BaseType_t    xQueueSendFromISR(QueueHandle_t xQueue,
                                       const void *pvItemToQueue,
                                       BaseType_t *pxHigherPriorityTaskWoken);

// Copy one item out of the queue (head). Blocks up to xTicksToWait ticks
// if the queue is empty. Returns pdTRUE on success.
extern BaseType_t    xQueueReceive(QueueHandle_t xQueue,
                                   void *pvBuffer,
                                   TickType_t xTicksToWait);

// ISR-context dequeue. Semantics mirror xQueueSendFromISR.
extern BaseType_t    xQueueReceiveFromISR(QueueHandle_t xQueue,
                                          void *pvBuffer,
                                          BaseType_t *pxHigherPriorityTaskWoken);

// Number of items currently stored in the queue.
extern UBaseType_t   uxQueueMessagesWaiting(QueueHandle_t xQueue);

// Number of free slots in the queue.
extern UBaseType_t   uxQueueSpacesAvailable(QueueHandle_t xQueue);

// Discard all pending items. Returns pdPASS.
extern BaseType_t    xQueueReset(QueueHandle_t xQueue);

#if VSF_FREERTOS_CFG_USE_QUEUESET == ENABLED
// ── QueueSet (queue of member pointers) ─────────────────────────────────

extern QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength);

extern BaseType_t xQueueAddToSet(QueueSetMemberHandle_t xQueueOrSemaphore,
                                 QueueSetHandle_t xQueueSet);

extern BaseType_t xQueueRemoveFromSet(QueueSetMemberHandle_t xQueueOrSemaphore,
                                      QueueSetHandle_t xQueueSet);

extern QueueSetMemberHandle_t xQueueSelectFromSet(QueueSetHandle_t xQueueSet,
                                                  TickType_t xTicksToWait);

extern QueueSetMemberHandle_t xQueueSelectFromSetFromISR(
                                      QueueSetHandle_t xQueueSet);
#endif

#ifdef __cplusplus
}
#endif

#endif      // __VSF_FREERTOS_QUEUE_H__
