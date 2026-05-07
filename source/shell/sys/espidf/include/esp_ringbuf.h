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
 * Clean-room re-implementation of ESP-IDF public API "freertos/ringbuf.h".
 *
 * Authored from the ESP-IDF v5.x public API reference only. No ESP-IDF /
 * FreeRTOS source code or data is copied.
 *
 * All three ring buffer types are fully implemented: BYTEBUF (byte stream,
 * zero-copy), NOSPLIT (item-oriented with dummy padding at wrap), and
 * ALLOWSPLIT (item-oriented with split items at wrap). Blocking support
 * (ticks_to_wait) is provided when VSF_USE_KERNEL is enabled; without the
 * kernel the port operates poll-only.
 *
 * Because VSF's ESP-IDF port does not include a full FreeRTOS layer, this
 * header exposes a minimal subset of the FreeRTOS value types that the
 * ring buffer API signature references. The guards allow code that does
 * include real FreeRTOS headers to compile side-by-side.
 */

#ifndef __VSF_ESPIDF_ESP_RINGBUF_H__
#define __VSF_ESPIDF_ESP_RINGBUF_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================ TYPES =========================================*/

#ifndef __VSF_ESPIDF_FREERTOS_TYPES_DEFINED__
#define __VSF_ESPIDF_FREERTOS_TYPES_DEFINED__
typedef long            BaseType_t;     /*!< FreeRTOS portable signed word.   */
typedef uint32_t        TickType_t;     /*!< FreeRTOS tick counter type.      */
#   ifndef pdTRUE
#       define pdTRUE       ((BaseType_t)1)
#   endif
#   ifndef pdFALSE
#       define pdFALSE      ((BaseType_t)0)
#   endif
#   ifndef pdPASS
#       define pdPASS       pdTRUE
#   endif
#   ifndef pdFAIL
#       define pdFAIL       pdFALSE
#   endif
#   ifndef portMAX_DELAY
#       define portMAX_DELAY    ((TickType_t)0xFFFFFFFFu)
#   endif
#endif      // __VSF_ESPIDF_FREERTOS_TYPES_DEFINED__

typedef enum {
    RINGBUF_TYPE_NOSPLIT    = 0,    /*!< Item-oriented, no split at wrap     */
    RINGBUF_TYPE_ALLOWSPLIT = 1,    /*!< Item-oriented, may wrap             */
    RINGBUF_TYPE_BYTEBUF    = 2,    /*!< Pure byte stream                    */
} RingbufferType_t;

struct __vsf_espidf_ringbuf;
typedef struct __vsf_espidf_ringbuf * RingbufHandle_t;

/*============================ PROTOTYPES ====================================*/

/* Allocate a ring buffer of the requested byte capacity. Returns NULL on
 * allocation failure. The returned handle must be released via
 * vRingbufferDelete(). */
RingbufHandle_t xRingbufferCreate(size_t buffer_size, RingbufferType_t type);

/* Release a previously created ring buffer. Passing NULL is a no-op. */
void vRingbufferDelete(RingbufHandle_t handle);

/* Enqueue `data_size` bytes (or a single item for NOSPLIT/ALLOWSPLIT).
 * Returns pdTRUE on success, pdFALSE if there is insufficient space or the
 * item exceeds xRingbufferGetMaxItemSize(). When VSF_USE_KERNEL is enabled
 * the caller blocks up to ticks_to_wait when the buffer is full; without
 * the kernel the port operates poll-only (ticks_to_wait is ignored). */
BaseType_t xRingbufferSend(RingbufHandle_t handle, const void *data,
                            size_t data_size, TickType_t ticks_to_wait);

/* Receive the next item (NOSPLIT/ALLOWSPLIT) or all available contiguous
 * bytes (BYTEBUF). Returns a zero-copy pointer into the ring buffer; the
 * caller must release it with vRingbufferReturnItem(). On success sets
 * *item_size to the payload length. Returns NULL if no data is available.
 * When VSF_USE_KERNEL is enabled the caller blocks up to ticks_to_wait
 * when the buffer is empty. */
void * xRingbufferReceive(RingbufHandle_t handle, size_t *item_size,
                            TickType_t ticks_to_wait);

/* Same as xRingbufferReceive() but for BYTEBUF caps the returned byte
 * count at wanted_size. */
void * xRingbufferReceiveUpTo(RingbufHandle_t handle, size_t *item_size,
                                TickType_t ticks_to_wait, size_t wanted_size);

/* Release a pointer previously returned by xRingbufferReceive() or
 * xRingbufferReceiveSplit(). For NOSPLIT/ALLOWSPLIT buffers this marks
 * the item as free within the ring; for BYTEBUF it advances the free
 * pointer to the read pointer. */
void vRingbufferReturnItem(RingbufHandle_t handle, void *item);

/* Retrieve a possibly-split item from an allow-split ring buffer. Only
 * valid for RINGBUF_TYPE_ALLOWSPLIT handles. On success, *ppvHeadItem
 * and *ppvTailItem point into the ring buffer (zero-copy) and
 * *pxHeadItemSize / *pxTailItemSize are set accordingly. If the item is
 * not split, *ppvTailItem is NULL. Each non-NULL pointer must be returned
 * individually via vRingbufferReturnItem(). */
BaseType_t xRingbufferReceiveSplit(RingbufHandle_t handle,
                                   void **ppvHeadItem,
                                   void **ppvTailItem,
                                   size_t *pxHeadItemSize,
                                   size_t *pxTailItemSize,
                                   TickType_t ticks_to_wait);

/* Bytes currently free for writing. */
size_t xRingbufferGetCurFreeSize(RingbufHandle_t handle);

/* Bytes currently queued for reading. */
size_t xRingbufferGetCurFilledSize(RingbufHandle_t handle);

/* Maximum item size that can currently be sent. For BYTEBUF this equals
 * the free byte count; for NOSPLIT/ALLOWSPLIT it accounts for per-item
 * header overhead and contiguous space constraints. */
size_t xRingbufferGetMaxItemSize(RingbufHandle_t handle);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_RINGBUF_H__
