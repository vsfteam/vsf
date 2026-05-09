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
 * FreeRTOS stream_buffer.h / message_buffer.h port.
 *
 * Implementation outline:
 *   - One byte-oriented ring buffer per handle, guarded by vsf_protect_sched.
 *   - Two vsf_sem_t: data_sem (posted when bytes become available)
 *                    space_sem (posted when space becomes available).
 *     Counting semaphores with a large ceiling act as level-triggered
 *     wakeups: each producer/consumer re-checks the ring state after a
 *     pend, so extra "spurious" credits are harmless.
 *   - Send waits in a loop until enough space is free for the full
 *     payload, then copies atomically under the lock. Timeout decrements
 *     each iteration using vsf_systimer_get_tick().
 *   - Receive waits until at least trigger_level bytes are available or
 *     the buffer is full, then copies min(avail, user_len) bytes out.
 *   - Message buffers reuse the same control block with a 4-byte header
 *     prepended to every payload; send is all-or-nothing, receive never
 *     splits a message.
 *
 * Concurrency contract: single reader + single writer per handle. This
 * matches the upstream FreeRTOS documented safe usage; the shim does
 * not guarantee correctness under multi-reader / multi-writer fan-in.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_freertos_cfg.h"

#if VSF_USE_FREERTOS == ENABLED && VSF_FREERTOS_CFG_USE_STREAM_BUFFER == ENABLED

#define __VSF_FREERTOS_STREAM_BUFFER_CLASS_IMPLEMENT
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "message_buffer.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "vsf_freertos stream_buffer port requires VSF_USE_HEAP == ENABLED"
#endif
#if !defined(VSF_USE_KERNEL) || VSF_USE_KERNEL != ENABLED
#   error "vsf_freertos stream_buffer port requires VSF_USE_KERNEL == ENABLED"
#endif

#include "service/heap/vsf_heap.h"
#include "kernel/vsf_kernel.h"

#include <string.h>
#include <stddef.h>

/*============================ MACROS ========================================*/

// vsf_eda_sem_init ceiling: a max count high enough that spurious posts
// on a saturated semaphore are functionally equivalent to "coalesced".
#define __FRT_SB_SEM_MAX        0x7FFF

#define __FRT_SB_HEADER_SIZE    (sizeof(uint32_t))

/*============================ TYPES =========================================*/

// Control block = StaticStreamBuffer_t (vsf_class declared in stream_buffer.h).
// MessageBufferHandle_t and StaticMessageBuffer_t alias the stream-buffer
// types; the `is_message` flag selects framing at runtime.

/*============================ LOCAL HELPERS =================================*/

// Copy n bytes into the ring at head (caller holds the lock).
static void __frt_sb_push(StaticStreamBuffer_t *sb, const uint8_t *src, size_t n)
{
    size_t first = sb->capacity - sb->head;
    if (first > n) { first = n; }
    memcpy(sb->buf + sb->head, src, first);
    if (n > first) {
        memcpy(sb->buf, src + first, n - first);
    }
    sb->head   = (sb->head + n) % sb->capacity;
    sb->count += n;
}

// Copy n bytes out of the ring at tail (caller holds the lock). If
// `dst` is NULL the bytes are simply discarded (used by peek + skip).
static void __frt_sb_pop(StaticStreamBuffer_t *sb, uint8_t *dst, size_t n)
{
    size_t first = sb->capacity - sb->tail;
    if (first > n) { first = n; }
    if (dst != NULL) {
        memcpy(dst, sb->buf + sb->tail, first);
        if (n > first) {
            memcpy(dst + first, sb->buf, n - first);
        }
    }
    sb->tail   = (sb->tail + n) % sb->capacity;
    sb->count -= n;
}

// Peek n bytes without advancing (caller holds the lock). Used to read
// a message header before deciding whether to consume it.
static void __frt_sb_peek(const StaticStreamBuffer_t *sb, size_t offset,
                          uint8_t *dst, size_t n)
{
    size_t pos = (sb->tail + offset) % sb->capacity;
    size_t first = sb->capacity - pos;
    if (first > n) { first = n; }
    memcpy(dst, sb->buf + pos, first);
    if (n > first) {
        memcpy(dst + first, sb->buf, n - first);
    }
}

static uint32_t __frt_sb_read_header(const StaticStreamBuffer_t *sb)
{
    uint8_t hdr[__FRT_SB_HEADER_SIZE];
    __frt_sb_peek(sb, 0, hdr, __FRT_SB_HEADER_SIZE);
    return ((uint32_t)hdr[0])
         | ((uint32_t)hdr[1] << 8)
         | ((uint32_t)hdr[2] << 16)
         | ((uint32_t)hdr[3] << 24);
}

static void __frt_sb_write_header(uint8_t *dst, uint32_t len)
{
    dst[0] = (uint8_t)(len       & 0xFF);
    dst[1] = (uint8_t)((len >> 8)  & 0xFF);
    dst[2] = (uint8_t)((len >> 16) & 0xFF);
    dst[3] = (uint8_t)((len >> 24) & 0xFF);
}

// Snapshot of "remaining timeout" bookkeeping. FreeRTOS ticks map 1:1 to
// ms in this shim (see pdMS_TO_TICKS), so the stored deadline uses the
// same unit as xTaskGetTickCount / vsf_systimer_get_tick.
typedef struct {
    bool            infinite;
    TickType_t      deadline;
} __frt_deadline_t;

static void __frt_deadline_init(__frt_deadline_t *d, TickType_t ticks)
{
    if (ticks == portMAX_DELAY) {
        d->infinite = true;
        d->deadline = 0;
    } else {
        d->infinite = false;
        d->deadline = vsf_systimer_get_tick() + (uint32_t)ticks;
    }
}

// Returns remaining time to wait on the kernel primitive, or 0 if we
// are already past the deadline (caller should do one final non-blocking
// check and return).
static vsf_timeout_tick_t __frt_deadline_remaining(const __frt_deadline_t *d)
{
    if (d->infinite) {
        return (vsf_timeout_tick_t)-1;
    }
    TickType_t now = vsf_systimer_get_tick();
    if (d->deadline <= now) {
        return 0;
    }
    return d->deadline - now;
}

/*============================ CORE: STREAM BUFFER ===========================*/

static void __frt_sb_init(StaticStreamBuffer_t *sb, uint8_t *buf, size_t size,
                          size_t trigger, bool is_message,
                          bool is_static, bool is_buf_static)
{
    memset(sb, 0, sizeof(*sb));
    sb->buf           = buf;
    sb->capacity      = size;
    sb->trigger_level = trigger;
    sb->is_message    = is_message;
    sb->is_static     = is_static;
    sb->is_buf_static = is_buf_static;
    vsf_eda_sem_init(&sb->data_sem,  0, __FRT_SB_SEM_MAX);
    vsf_eda_sem_init(&sb->space_sem, 0, __FRT_SB_SEM_MAX);
}

static StreamBufferHandle_t __frt_sb_create(size_t size, size_t trigger,
                                            bool is_message)
{
    if (size == 0) { return NULL; }
    // Stream buffers use trigger==0 as "one byte suffices".
    if (trigger == 0) { trigger = 1; }
    if (trigger > size) { trigger = size; }

    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)vsf_heap_malloc(sizeof(*sb));
    if (sb == NULL) { return NULL; }

    uint8_t *buf = (uint8_t *)vsf_heap_malloc(size);
    if (buf == NULL) {
        vsf_heap_free(sb);
        return NULL;
    }
    __frt_sb_init(sb, buf, size, trigger, is_message, false, false);
    return (StreamBufferHandle_t)sb;
}

static StreamBufferHandle_t __frt_sb_create_static(
        size_t size, size_t trigger, bool is_message,
        uint8_t *storage, void *pxStaticBuffer)
{
    if ((pxStaticBuffer == NULL) || (storage == NULL) || (size == 0)) {
        return NULL;
    }
    if (trigger == 0) { trigger = 1; }
    if (trigger > size) { trigger = size; }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)pxStaticBuffer;
    __frt_sb_init(sb, storage, size, trigger, is_message, true, true);
    return (StreamBufferHandle_t)sb;
}

StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes,
                                         size_t xTriggerLevelBytes)
{
    return __frt_sb_create(xBufferSizeBytes, xTriggerLevelBytes, false);
}

StreamBufferHandle_t xStreamBufferCreateStatic(
        size_t xBufferSizeBytes,
        size_t xTriggerLevelBytes,
        uint8_t *pucStreamBufferStorageArea,
        StaticStreamBuffer_t *pxStaticStreamBuffer)
{
    return __frt_sb_create_static(xBufferSizeBytes, xTriggerLevelBytes,
                                  false, pucStreamBufferStorageArea,
                                  pxStaticStreamBuffer);
}

void vStreamBufferDelete(StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return; }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;
    // Cancel any pending senders/receivers before releasing storage.
    // Storage ownership is tracked per-field so a Static instance never
    // hands user-supplied memory back to the heap allocator.
    vsf_eda_sync_cancel(&sb->data_sem);
    vsf_eda_sync_cancel(&sb->space_sem);
    if ((sb->buf != NULL) && !sb->is_buf_static) {
        vsf_heap_free(sb->buf);
    }
    if (!sb->is_static) {
        vsf_heap_free(sb);
    }
}

size_t xStreamBufferSend(StreamBufferHandle_t xStreamBuffer,
                         const void *pvTxData,
                         size_t xDataLengthBytes,
                         TickType_t xTicksToWait)
{
    if ((xStreamBuffer == NULL) || (pvTxData == NULL)
            || (xDataLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;

    __frt_deadline_t dl;
    __frt_deadline_init(&dl, xTicksToWait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_sched();
            size_t free_space = sb->capacity - sb->count;
            size_t n;
            if (free_space >= xDataLengthBytes) {
                n = xDataLengthBytes;
            } else if (free_space > 0 && xTicksToWait == 0) {
                // Stream semantics: non-blocking caller takes what fits.
                n = free_space;
            } else {
                n = 0;
            }
            if (n > 0) {
                bool crossed = (sb->count < sb->trigger_level)
                            && ((sb->count + n) >= sb->trigger_level);
                __frt_sb_push(sb, (const uint8_t *)pvTxData, n);
                vsf_unprotect_sched(orig);
                if (crossed) {
                    vsf_eda_sem_post(&sb->data_sem);
                }
                return n;
            }
        vsf_unprotect_sched(orig);

        // Need to wait for space.
        vsf_timeout_tick_t rem = __frt_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            return 0;
        }
        (void)vsf_thread_sem_pend(&sb->space_sem, rem);
        // Spurious wakeup handled by the loop re-check.
    }
}

size_t xStreamBufferSendFromISR(StreamBufferHandle_t xStreamBuffer,
                                const void *pvTxData,
                                size_t xDataLengthBytes,
                                BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((xStreamBuffer == NULL) || (pvTxData == NULL)
            || (xDataLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;

    vsf_protect_t orig = vsf_protect_sched();
        size_t free_space = sb->capacity - sb->count;
        size_t n = (free_space < xDataLengthBytes) ? free_space : xDataLengthBytes;
        bool crossed = (n > 0)
                    && (sb->count < sb->trigger_level)
                    && ((sb->count + n) >= sb->trigger_level);
        if (n > 0) {
            __frt_sb_push(sb, (const uint8_t *)pvTxData, n);
        }
    vsf_unprotect_sched(orig);

    if (crossed) {
        vsf_eda_sem_post_isr(&sb->data_sem);
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdTRUE;
        }
    }
    return n;
}

size_t xStreamBufferReceive(StreamBufferHandle_t xStreamBuffer,
                            void *pvRxData,
                            size_t xBufferLengthBytes,
                            TickType_t xTicksToWait)
{
    if ((xStreamBuffer == NULL) || (pvRxData == NULL)
            || (xBufferLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;

    __frt_deadline_t dl;
    __frt_deadline_init(&dl, xTicksToWait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_sched();
            bool ready = (sb->count >= sb->trigger_level)
                      || (sb->count == sb->capacity)
                      || (sb->count > 0 && xTicksToWait == 0);
            size_t n = 0;
            size_t old_count = sb->count;
            if (ready && sb->count > 0) {
                if (sb->is_message) {
                    // Read head message length header.
                    uint32_t msg_len = __frt_sb_read_header(sb);
                    if (msg_len > xBufferLengthBytes) {
                        // Caller buffer too small -- don't consume.
                        vsf_unprotect_sched(orig);
                        return 0;
                    }
                    // Consume header + payload.
                    __frt_sb_pop(sb, NULL, __FRT_SB_HEADER_SIZE);
                    __frt_sb_pop(sb, (uint8_t *)pvRxData, msg_len);
                    n = msg_len;
                } else {
                    size_t avail = sb->count;
                    n = (avail < xBufferLengthBytes) ? avail : xBufferLengthBytes;
                    __frt_sb_pop(sb, (uint8_t *)pvRxData, n);
                }
            }
        vsf_unprotect_sched(orig);

        if (n > 0) {
            if (old_count == sb->capacity
                    || (sb->capacity - (old_count - n)) > 0) {
                // Space freed; notify any waiting sender.
                vsf_eda_sem_post(&sb->space_sem);
            }
            return n;
        }

        vsf_timeout_tick_t rem = __frt_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            return 0;
        }
        (void)vsf_thread_sem_pend(&sb->data_sem, rem);
    }
}

size_t xStreamBufferReceiveFromISR(StreamBufferHandle_t xStreamBuffer,
                                   void *pvRxData,
                                   size_t xBufferLengthBytes,
                                   BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((xStreamBuffer == NULL) || (pvRxData == NULL)
            || (xBufferLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;

    vsf_protect_t orig = vsf_protect_sched();
        size_t n = 0;
        if (sb->count > 0) {
            if (sb->is_message) {
                uint32_t msg_len = __frt_sb_read_header(sb);
                if (msg_len <= xBufferLengthBytes) {
                    __frt_sb_pop(sb, NULL, __FRT_SB_HEADER_SIZE);
                    __frt_sb_pop(sb, (uint8_t *)pvRxData, msg_len);
                    n = msg_len;
                }
            } else {
                n = (sb->count < xBufferLengthBytes) ? sb->count : xBufferLengthBytes;
                __frt_sb_pop(sb, (uint8_t *)pvRxData, n);
            }
        }
    vsf_unprotect_sched(orig);

    if (n > 0) {
        vsf_eda_sem_post_isr(&sb->space_sem);
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdTRUE;
        }
    }
    return n;
}

BaseType_t xStreamBufferIsEmpty(const StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return pdTRUE; }
    return ((StaticStreamBuffer_t *)xStreamBuffer)->count == 0 ? pdTRUE : pdFALSE;
}

BaseType_t xStreamBufferIsFull(const StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return pdFALSE; }
    const StaticStreamBuffer_t *sb = (const StaticStreamBuffer_t *)xStreamBuffer;
    return sb->count == sb->capacity ? pdTRUE : pdFALSE;
}

size_t xStreamBufferBytesAvailable(const StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return 0; }
    return ((const StaticStreamBuffer_t *)xStreamBuffer)->count;
}

size_t xStreamBufferSpacesAvailable(const StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return 0; }
    const StaticStreamBuffer_t *sb = (const StaticStreamBuffer_t *)xStreamBuffer;
    return sb->capacity - sb->count;
}

BaseType_t xStreamBufferSetTriggerLevel(StreamBufferHandle_t xStreamBuffer,
                                        size_t xTriggerLevelBytes)
{
    if (xStreamBuffer == NULL) { return pdFAIL; }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;
    if (xTriggerLevelBytes == 0) { xTriggerLevelBytes = 1; }
    if (xTriggerLevelBytes > sb->capacity) { return pdFAIL; }
    vsf_protect_t orig = vsf_protect_sched();
        sb->trigger_level = xTriggerLevelBytes;
    vsf_unprotect_sched(orig);
    return pdPASS;
}

BaseType_t xStreamBufferReset(StreamBufferHandle_t xStreamBuffer)
{
    if (xStreamBuffer == NULL) { return pdFAIL; }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xStreamBuffer;
    vsf_protect_t orig = vsf_protect_sched();
        sb->head = sb->tail = sb->count = 0;
    vsf_unprotect_sched(orig);
    // Wake any waiting sender -- space just became available.
    vsf_eda_sem_post(&sb->space_sem);
    return pdPASS;
}

/*============================ CORE: MESSAGE BUFFER ==========================*/

MessageBufferHandle_t xMessageBufferCreate(size_t xBufferSizeBytes)
{
    // Must be able to hold at least a single byte message + header.
    if (xBufferSizeBytes <= __FRT_SB_HEADER_SIZE) { return NULL; }
    return (MessageBufferHandle_t)
        __frt_sb_create(xBufferSizeBytes, 1, true);
}

MessageBufferHandle_t xMessageBufferCreateStatic(
        size_t xBufferSizeBytes,
        uint8_t *pucMessageBufferStorageArea,
        StaticMessageBuffer_t *pxStaticMessageBuffer)
{
    if (xBufferSizeBytes <= __FRT_SB_HEADER_SIZE) { return NULL; }
    return (MessageBufferHandle_t)
        __frt_sb_create_static(xBufferSizeBytes, 1, true,
                               pucMessageBufferStorageArea,
                               pxStaticMessageBuffer);
}

void vMessageBufferDelete(MessageBufferHandle_t xMessageBuffer)
{
    vStreamBufferDelete((StreamBufferHandle_t)xMessageBuffer);
}

size_t xMessageBufferSend(MessageBufferHandle_t xMessageBuffer,
                          const void *pvTxData,
                          size_t xDataLengthBytes,
                          TickType_t xTicksToWait)
{
    if ((xMessageBuffer == NULL) || (pvTxData == NULL)
            || (xDataLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xMessageBuffer;
    size_t needed = __FRT_SB_HEADER_SIZE + xDataLengthBytes;
    if (needed > sb->capacity) {
        return 0;
    }

    __frt_deadline_t dl;
    __frt_deadline_init(&dl, xTicksToWait);

    uint8_t hdr[__FRT_SB_HEADER_SIZE];
    __frt_sb_write_header(hdr, (uint32_t)xDataLengthBytes);

    for (;;) {
        vsf_protect_t orig = vsf_protect_sched();
            size_t free_space = sb->capacity - sb->count;
            bool ok = (free_space >= needed);
            bool crossed = false;
            if (ok) {
                crossed = (sb->count < sb->trigger_level)
                       && ((sb->count + needed) >= sb->trigger_level);
                __frt_sb_push(sb, hdr, __FRT_SB_HEADER_SIZE);
                __frt_sb_push(sb, (const uint8_t *)pvTxData, xDataLengthBytes);
            }
        vsf_unprotect_sched(orig);
        if (ok) {
            if (crossed) {
                vsf_eda_sem_post(&sb->data_sem);
            }
            return xDataLengthBytes;
        }

        vsf_timeout_tick_t rem = __frt_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            return 0;
        }
        (void)vsf_thread_sem_pend(&sb->space_sem, rem);
    }
}

size_t xMessageBufferSendFromISR(MessageBufferHandle_t xMessageBuffer,
                                 const void *pvTxData,
                                 size_t xDataLengthBytes,
                                 BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((xMessageBuffer == NULL) || (pvTxData == NULL)
            || (xDataLengthBytes == 0)) {
        return 0;
    }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xMessageBuffer;
    size_t needed = __FRT_SB_HEADER_SIZE + xDataLengthBytes;

    uint8_t hdr[__FRT_SB_HEADER_SIZE];
    __frt_sb_write_header(hdr, (uint32_t)xDataLengthBytes);

    vsf_protect_t orig = vsf_protect_sched();
        size_t free_space = sb->capacity - sb->count;
        bool ok = (free_space >= needed);
        bool crossed = false;
        if (ok) {
            crossed = (sb->count < sb->trigger_level)
                   && ((sb->count + needed) >= sb->trigger_level);
            __frt_sb_push(sb, hdr, __FRT_SB_HEADER_SIZE);
            __frt_sb_push(sb, (const uint8_t *)pvTxData, xDataLengthBytes);
        }
    vsf_unprotect_sched(orig);

    if (ok && crossed) {
        vsf_eda_sem_post_isr(&sb->data_sem);
        if (pxHigherPriorityTaskWoken != NULL) {
            *pxHigherPriorityTaskWoken = pdTRUE;
        }
    }
    return ok ? xDataLengthBytes : 0;
}

size_t xMessageBufferReceive(MessageBufferHandle_t xMessageBuffer,
                             void *pvRxData,
                             size_t xBufferLengthBytes,
                             TickType_t xTicksToWait)
{
    return xStreamBufferReceive((StreamBufferHandle_t)xMessageBuffer,
                                pvRxData, xBufferLengthBytes, xTicksToWait);
}

size_t xMessageBufferReceiveFromISR(MessageBufferHandle_t xMessageBuffer,
                                    void *pvRxData,
                                    size_t xBufferLengthBytes,
                                    BaseType_t *pxHigherPriorityTaskWoken)
{
    return xStreamBufferReceiveFromISR((StreamBufferHandle_t)xMessageBuffer,
                                       pvRxData, xBufferLengthBytes,
                                       pxHigherPriorityTaskWoken);
}

BaseType_t xMessageBufferIsEmpty(const MessageBufferHandle_t xMessageBuffer)
{
    return xStreamBufferIsEmpty((const StreamBufferHandle_t)xMessageBuffer);
}

BaseType_t xMessageBufferIsFull(const MessageBufferHandle_t xMessageBuffer)
{
    return xStreamBufferIsFull((const StreamBufferHandle_t)xMessageBuffer);
}

size_t xMessageBufferSpaceAvailable(const MessageBufferHandle_t xMessageBuffer)
{
    return xStreamBufferSpacesAvailable((const StreamBufferHandle_t)xMessageBuffer);
}

size_t xMessageBufferNextLengthBytes(MessageBufferHandle_t xMessageBuffer)
{
    if (xMessageBuffer == NULL) { return 0; }
    StaticStreamBuffer_t *sb = (StaticStreamBuffer_t *)xMessageBuffer;
    if (!sb->is_message) { return 0; }
    size_t len = 0;
    vsf_protect_t orig = vsf_protect_sched();
        if (sb->count >= __FRT_SB_HEADER_SIZE) {
            len = (size_t)__frt_sb_read_header(sb);
        }
    vsf_unprotect_sched(orig);
    return len;
}

BaseType_t xMessageBufferReset(MessageBufferHandle_t xMessageBuffer)
{
    return xStreamBufferReset((StreamBufferHandle_t)xMessageBuffer);
}

#endif      // VSF_USE_FREERTOS && VSF_FREERTOS_CFG_USE_STREAM_BUFFER
