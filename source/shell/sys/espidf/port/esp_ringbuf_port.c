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
 * Port implementation for "esp_ringbuf.h" on VSF.
 *
 * Implements all three ring buffer types with the same four-pointer
 * storage model and embedded ItemHeader_t as the upstream ESP-IDF
 * ringbuf.c reference:
 *
 *   RINGBUF_TYPE_BYTEBUF    – byte stream, zero-copy return,
 *                              single outstanding read at a time.
 *   RINGBUF_TYPE_NOSPLIT    – item-oriented, never splits items at
 *                              wrap boundary (uses dummy item padding).
 *   RINGBUF_TYPE_ALLOWSPLIT – item-oriented, may split items at wrap
 *                              boundary (rbITEM_SPLIT_FLAG).
 *
 * Blocking (ticks_to_wait) is supported when VSF_USE_KERNEL is enabled:
 * two counting semaphores (data_sem / space_sem) provide level-triggered
 * wakeups with deadline tracking. Without the kernel the port remains
 * poll-only and the ticks parameter is ignored.
 *
 * Concurrency: critical sections use vsf_protect_int() / vsf_unprotect_int()
 * which is sufficient for timer-callback vs main-thread scenarios on the
 * current single-core host targets.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_RINGBUF == ENABLED

#include "esp_ringbuf.h"

#include "../vsf_espidf.h"
#include "kernel/vsf_kernel.h"
#include "service/trace/vsf_trace.h"
#if defined(VSF_USE_HEAP) && VSF_USE_HEAP == ENABLED
#   include "service/heap/vsf_heap.h"
#else
#   error "VSF_ESPIDF_CFG_USE_RINGBUF requires VSF_USE_HEAP"
#endif

#include <string.h>
#include <inttypes.h>
#include <stdio.h>

/*============================ MACROS ========================================*/

#define rbALIGN_MASK            0x03u
#define rbALIGN_SIZE(x)         (((x) + rbALIGN_MASK) & ~rbALIGN_MASK)
#define rbCHECK_ALIGNED(p)      ((((size_t)(p)) & rbALIGN_MASK) == 0)

#define rbHEADER_SIZE           sizeof(ItemHeader_t)

/* Ring buffer flags (uxRingbufferFlags) */
#define rbALLOW_SPLIT_FLAG      ((uint32_t)1)
#define rbBYTE_BUFFER_FLAG      ((uint32_t)2)
#define rbBUFFER_FULL_FLAG      ((uint32_t)4)

/* Per-item flags (uxItemFlags) */
#define rbITEM_FREE_FLAG        ((uint32_t)1)
#define rbITEM_DUMMY_DATA_FLAG  ((uint32_t)2)
#define rbITEM_SPLIT_FLAG       ((uint32_t)4)
#define rbITEM_WRITTEN_FLAG     ((uint32_t)8)

#if VSF_USE_KERNEL == ENABLED
#   define __RB_SEM_MAX         0x7FFFu
#   define __rb_wake_data(rb)   vsf_eda_sem_post(&(rb)->data_sem)
#   define __rb_wake_space(rb)  vsf_eda_sem_post(&(rb)->space_sem)
#   define __rb_wake_data_isr(rb, w)   do { \
        vsf_eda_sem_post_isr(&(rb)->data_sem); \
        if ((w) != NULL) *(w) = pdTRUE; \
    } while (0)
#   define __rb_wake_space_isr(rb, w)  do { \
        vsf_eda_sem_post_isr(&(rb)->space_sem); \
        if ((w) != NULL) *(w) = pdTRUE; \
    } while (0)
#   define __rb_cancel(rb)      do {                         \
        vsf_eda_sync_cancel(&(rb)->data_sem);                \
        vsf_eda_sync_cancel(&(rb)->space_sem);               \
    } while (0)
#else
#   define __rb_wake_data(rb)       ((void)0)
#   define __rb_wake_space(rb)      ((void)0)
#   define __rb_wake_data_isr(rb,w) ((void)0)
#   define __rb_wake_space_isr(rb,w)((void)0)
#   define __rb_cancel(rb)          ((void)0)
#endif

/*============================ TYPES =========================================*/

typedef struct {
    size_t          xItemLen;
    uint32_t        uxItemFlags;
} ItemHeader_t;

struct __vsf_espidf_ringbuf {
    uint8_t *       pucHead;
    uint8_t *       pucTail;
    uint8_t *       pucAcquire;
    uint8_t *       pucWrite;
    uint8_t *       pucRead;
    uint8_t *       pucFree;
    size_t          xSize;
    size_t          xMaxItemSize;
    size_t          xItemsWaiting;
    uint32_t        uxRingbufferFlags;
#if VSF_USE_KERNEL == ENABLED
    vsf_sem_t       data_sem;
    vsf_sem_t       space_sem;
#endif
    bool            is_static;
#if VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED
    vsf_heap_t *    heap;
#endif
};

/* StaticRingbuffer_t must be at least as large as the control block. */
#ifdef __STDC_VERSION__
#  if __STDC_VERSION__ >= 201112L
     _Static_assert(sizeof(StaticRingbuffer_t) >= sizeof(struct __vsf_espidf_ringbuf),
                    "StaticRingbuffer_t too small for platform");
#  endif
#endif

/*============================ LOCAL HELPERS =================================*/

/* ---------- free-space & capacity queries (caller holds lock) ------------- */

static size_t prvGetFreeSize(struct __vsf_espidf_ringbuf *rb)
{
    if (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    ptrdiff_t free = rb->pucFree - rb->pucAcquire;
    if (free <= 0) {
        free += rb->xSize;
    }
    return (size_t)free;
}

/* ---------- fit checks (caller holds lock) -------------------------------- */

static bool prvCheckItemFitsByteBuf(struct __vsf_espidf_ringbuf *rb,
                                    size_t xItemSize)
{
    return prvGetFreeSize(rb) >= xItemSize;
}

static bool prvCheckItemFitsDefault(struct __vsf_espidf_ringbuf *rb,
                                    size_t xItemSize)
{
    size_t xTotalItemSize = rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE;

    if (rb->pucAcquire == rb->pucFree) {
        return (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? false : true;
    }
    if (rb->pucFree > rb->pucAcquire) {
        return xTotalItemSize <= (size_t)(rb->pucFree - rb->pucAcquire);
    }
    if (xTotalItemSize <= (size_t)(rb->pucTail - rb->pucAcquire)) {
        return true;
    }
    if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        return (xTotalItemSize + rbHEADER_SIZE)
               <= (size_t)((rb->pucFree - rb->pucHead)
                         + (rb->pucTail - rb->pucAcquire));
    }
    return xTotalItemSize <= (size_t)(rb->pucFree - rb->pucHead);
}

/* ---------- item-availability check (caller holds lock) ------------------- */

static bool prvCheckItemAvail(struct __vsf_espidf_ringbuf *rb)
{
    if ((rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG)
            && rb->pucRead != rb->pucFree) {
        return false;
    }
    if ((rb->xItemsWaiting > 0)
            && ((rb->pucRead != rb->pucWrite)
                || (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG))) {
        if ((rb->uxRingbufferFlags
                & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG)) == 0) {
            ItemHeader_t *pxHeader = (ItemHeader_t *)rb->pucRead;
            if ((pxHeader->uxItemFlags & rbITEM_WRITTEN_FLAG) == 0) {
                return false;
            }
        }
        return true;
    }
    return false;
}

/* ---------- max-item-size helpers (caller holds lock) --------------------- */

static size_t prvGetCurMaxSizeNoSplit(struct __vsf_espidf_ringbuf *rb)
{
    ptrdiff_t free;
    if (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    if (rb->pucAcquire < rb->pucFree) {
        free = rb->pucFree - rb->pucAcquire;
    } else {
        ptrdiff_t s1 = rb->pucTail - rb->pucAcquire;
        ptrdiff_t s2 = rb->pucFree - rb->pucHead;
        free = (s1 > s2) ? s1 : s2;
    }
    free -= rbHEADER_SIZE;
    if (free < 0) return 0;
    if ((size_t)free > rb->xMaxItemSize) return rb->xMaxItemSize;
    return (size_t)free;
}

static size_t prvGetCurMaxSizeAllowSplit(struct __vsf_espidf_ringbuf *rb)
{
    ptrdiff_t free;
    if (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    if (rb->pucAcquire == rb->pucHead && rb->pucFree == rb->pucHead) {
        free = (ptrdiff_t)(rb->xSize) - rbHEADER_SIZE;
    } else if (rb->pucAcquire < rb->pucFree) {
        free = (rb->pucFree - rb->pucAcquire) - rbHEADER_SIZE;
    } else {
        free = (rb->pucFree - rb->pucHead)
             + (rb->pucTail - rb->pucAcquire)
             - (rbHEADER_SIZE * 2);
    }
    if (free < 0) return 0;
    if ((size_t)free > rb->xMaxItemSize) return rb->xMaxItemSize;
    return (size_t)free;
}

static size_t prvGetCurMaxSizeByteBuf(struct __vsf_espidf_ringbuf *rb)
{
    if (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    ptrdiff_t free = rb->pucFree - rb->pucAcquire;
    if (free <= 0) free += rb->xSize;
    return (size_t)free;
}

/* ---------- copy-to-buffer (caller holds lock) ---------------------------- */

static void prvCopyItemByteBuf(struct __vsf_espidf_ringbuf *rb,
                               const uint8_t *pucItem, size_t xItemSize)
{
    size_t xRemLen = (size_t)(rb->pucTail - rb->pucAcquire);
    if (xRemLen < xItemSize) {
        memcpy(rb->pucAcquire, pucItem, xRemLen);
        rb->xItemsWaiting += xRemLen;
        pucItem += xRemLen;
        xItemSize -= xRemLen;
        rb->pucAcquire = rb->pucHead;
    }
    memcpy(rb->pucAcquire, pucItem, xItemSize);
    rb->xItemsWaiting += xItemSize;
    rb->pucAcquire += xItemSize;
    if (rb->pucAcquire == rb->pucTail) {
        rb->pucAcquire = rb->pucHead;
    }
    if (rb->pucAcquire == rb->pucFree) {
        rb->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
    rb->pucWrite = rb->pucAcquire;
}

static void * prvAcquireItemNoSplit(struct __vsf_espidf_ringbuf *rb,
                                      size_t xItemSize)
{
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);
    size_t xRemLen = (size_t)(rb->pucTail - rb->pucAcquire);

    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        ItemHeader_t *pxDummy = (ItemHeader_t *)rb->pucAcquire;
        pxDummy->uxItemFlags = rbITEM_DUMMY_DATA_FLAG;
        pxDummy->xItemLen = 0;
        rb->pucAcquire = rb->pucHead;
    }

    ItemHeader_t *pxHeader = (ItemHeader_t *)rb->pucAcquire;
    pxHeader->xItemLen = xItemSize;
    pxHeader->uxItemFlags = 0;
    uint8_t *item_addr = rb->pucAcquire + rbHEADER_SIZE;
    rb->pucAcquire += rbHEADER_SIZE + xAlignedItemSize;

    if ((size_t)(rb->pucTail - rb->pucAcquire) < rbHEADER_SIZE) {
        rb->pucAcquire = rb->pucHead;
    }
    if (rb->pucAcquire == rb->pucFree) {
        rb->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
    return (void *)item_addr;
}

static void prvSendItemDoneNoSplit(struct __vsf_espidf_ringbuf *rb,
                                    uint8_t *pucItem)
{
    ItemHeader_t *pxCurHeader = (ItemHeader_t *)(pucItem - rbHEADER_SIZE);
    pxCurHeader->uxItemFlags &= ~rbITEM_SPLIT_FLAG;
    pxCurHeader->uxItemFlags |= rbITEM_WRITTEN_FLAG;

    rb->xItemsWaiting++;

    pxCurHeader = (ItemHeader_t *)rb->pucWrite;
    while (((pxCurHeader->uxItemFlags & rbITEM_WRITTEN_FLAG)
            || (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG))
            && (rb->pucWrite != rb->pucAcquire)) {
        if (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
            pxCurHeader->uxItemFlags |= rbITEM_WRITTEN_FLAG;
            rb->pucWrite = rb->pucHead;
        } else {
            size_t step = rbALIGN_SIZE(pxCurHeader->xItemLen);
            rb->pucWrite += step + rbHEADER_SIZE;
        }
        if ((size_t)(rb->pucTail - rb->pucWrite) < rbHEADER_SIZE) {
            rb->pucWrite = rb->pucHead;
        }
        if (rb->pucWrite == rb->pucAcquire) {
            break;
        }
        pxCurHeader = (ItemHeader_t *)rb->pucWrite;
    }
}

static void prvCopyItemNoSplit(struct __vsf_espidf_ringbuf *rb,
                               const uint8_t *pucItem, size_t xItemSize)
{
    uint8_t *item_addr = (uint8_t *)prvAcquireItemNoSplit(rb, xItemSize);
    memcpy(item_addr, pucItem, xItemSize);
    prvSendItemDoneNoSplit(rb, item_addr);
}

static void prvCopyItemAllowSplit(struct __vsf_espidf_ringbuf *rb,
                                  const uint8_t *pucItem, size_t xItemSize)
{
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);
    size_t xRemLen = (size_t)(rb->pucTail - rb->pucAcquire);

    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        ItemHeader_t *pxFirstHeader = (ItemHeader_t *)rb->pucAcquire;
        pxFirstHeader->uxItemFlags = 0;
        pxFirstHeader->xItemLen = xRemLen - rbHEADER_SIZE;
        rb->pucAcquire += rbHEADER_SIZE;
        xRemLen -= rbHEADER_SIZE;
        if (xRemLen > 0) {
            memcpy(rb->pucAcquire, pucItem, xRemLen);
            rb->xItemsWaiting++;
            pucItem += xRemLen;
            xItemSize -= xRemLen;
            xAlignedItemSize -= xRemLen;
            pxFirstHeader->uxItemFlags |= rbITEM_SPLIT_FLAG;
        } else {
            pxFirstHeader->uxItemFlags |= rbITEM_DUMMY_DATA_FLAG;
        }
        rb->pucAcquire = rb->pucHead;
    }

    ItemHeader_t *pxSecondHeader = (ItemHeader_t *)rb->pucAcquire;
    pxSecondHeader->xItemLen = xItemSize;
    pxSecondHeader->uxItemFlags = 0;
    rb->pucAcquire += rbHEADER_SIZE;
    memcpy(rb->pucAcquire, pucItem, xItemSize);
    rb->xItemsWaiting++;
    rb->pucAcquire += xAlignedItemSize;

    if ((size_t)(rb->pucTail - rb->pucAcquire) < rbHEADER_SIZE) {
        rb->pucAcquire = rb->pucHead;
    }
    if (rb->pucAcquire == rb->pucFree) {
        rb->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
    rb->pucWrite = rb->pucAcquire;
}

/* ---------- get-from-buffer (caller holds lock) --------------------------- */

static void * prvGetItemDefault(struct __vsf_espidf_ringbuf *rb,
                                bool *pxIsSplit, size_t *pxItemSize)
{
    ItemHeader_t *pxHeader = (ItemHeader_t *)rb->pucRead;
    uint8_t *pcReturn;

    if (pxHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
        rb->pucRead = rb->pucHead;
        pxHeader = (ItemHeader_t *)rb->pucRead;
    }
    pcReturn = rb->pucRead + rbHEADER_SIZE;
    *pxItemSize = pxHeader->xItemLen;
    rb->xItemsWaiting--;
    *pxIsSplit = (pxHeader->uxItemFlags & rbITEM_SPLIT_FLAG) ? true : false;

    rb->pucRead += rbHEADER_SIZE + rbALIGN_SIZE(pxHeader->xItemLen);
    if ((size_t)(rb->pucTail - rb->pucRead) < rbHEADER_SIZE) {
        rb->pucRead = rb->pucHead;
    }
    return (void *)pcReturn;
}

static void * prvGetItemByteBuf(struct __vsf_espidf_ringbuf *rb,
                                size_t xMaxSize, size_t *pxItemSize)
{
    uint8_t *ret = rb->pucRead;

    if ((rb->pucRead > rb->pucWrite)
            || (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG)) {
        size_t contig = (size_t)(rb->pucTail - rb->pucRead);
        if (xMaxSize == 0 || contig <= xMaxSize) {
            *pxItemSize = contig;
            rb->xItemsWaiting -= contig;
            rb->pucRead = rb->pucHead;
        } else {
            *pxItemSize = xMaxSize;
            rb->xItemsWaiting -= xMaxSize;
            rb->pucRead += xMaxSize;
        }
    } else {
        size_t contig = (size_t)(rb->pucWrite - rb->pucRead);
        if (xMaxSize == 0 || contig <= xMaxSize) {
            *pxItemSize = contig;
            rb->xItemsWaiting -= contig;
            rb->pucRead = rb->pucWrite;
        } else {
            *pxItemSize = xMaxSize;
            rb->xItemsWaiting -= xMaxSize;
            rb->pucRead += xMaxSize;
        }
    }
    return (void *)ret;
}

/* ---------- return-to-buffer (caller holds lock) -------------------------- */

static void prvReturnItemDefault(struct __vsf_espidf_ringbuf *rb,
                                 uint8_t *pucItem)
{
    ItemHeader_t *pxCurHeader = (ItemHeader_t *)(pucItem - rbHEADER_SIZE);
    pxCurHeader->uxItemFlags &= ~rbITEM_SPLIT_FLAG;
    pxCurHeader->uxItemFlags |= rbITEM_FREE_FLAG;

    bool allow_advance =
        ((rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? true : false)
        || ((rb->xItemsWaiting == 0) ? true : false);
    bool freed_any = false;

    pxCurHeader = (ItemHeader_t *)rb->pucFree;
    size_t limit = rb->xSize / rbHEADER_SIZE + 4;
    while (limit-- && ((pxCurHeader->uxItemFlags
                & (rbITEM_FREE_FLAG | rbITEM_DUMMY_DATA_FLAG)) != 0)
            && ((rb->pucFree != rb->pucRead) || allow_advance)) {

        allow_advance = false;
        freed_any = true;

        if (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
            pxCurHeader->uxItemFlags |= rbITEM_FREE_FLAG;
            rb->pucFree = rb->pucHead;
        } else {
            size_t step = rbALIGN_SIZE(pxCurHeader->xItemLen);
            rb->pucFree += step + rbHEADER_SIZE;
        }
        if ((size_t)(rb->pucTail - rb->pucFree) < rbHEADER_SIZE) {
            rb->pucFree = rb->pucHead;
        }
        pxCurHeader = (ItemHeader_t *)rb->pucFree;
    }

    if ((rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) && freed_any) {
        rb->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
    }
}

static void prvReturnItemByteBuf(struct __vsf_espidf_ringbuf *rb,
                                 uint8_t *pucItem)
{
    (void)pucItem;
    rb->pucFree = rb->pucRead;
    if (rb->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        rb->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
    }
}

/* ---------- blocking helpers (kernel available only) ---------------------- */

#if VSF_USE_KERNEL == ENABLED

static void __rb_sem_init(struct __vsf_espidf_ringbuf *rb)
{
    vsf_eda_sem_init(&rb->data_sem,  0, __RB_SEM_MAX);
    vsf_eda_sem_init(&rb->space_sem, 0, __RB_SEM_MAX);
}

typedef struct {
    bool            infinite;
    uint32_t        deadline_ms;
} __rb_deadline_t;

static void __rb_deadline_init(__rb_deadline_t *d, TickType_t ticks)
{
    if (ticks == portMAX_DELAY) {
        d->infinite = true;
        d->deadline_ms = 0;
    } else {
        d->infinite = false;
        d->deadline_ms = vsf_systimer_get_ms() + (uint32_t)ticks;
    }
}

static vsf_timeout_tick_t __rb_deadline_remaining(const __rb_deadline_t *d)
{
    if (d->infinite) {
        return (vsf_timeout_tick_t)-1;
    }
    uint32_t now = vsf_systimer_get_ms();
    if ((int32_t)(d->deadline_ms - now) <= 0) {
        return 0;
    }
    return vsf_systimer_ms_to_tick(d->deadline_ms - now);
}

#endif /* VSF_USE_KERNEL */

/*============================ INITIALIZATION ================================*/

static void prvInitializeNewRingbuffer(size_t xBufferSize,
                                       RingbufferType_t xBufferType,
                                       struct __vsf_espidf_ringbuf *rb,
                                       uint8_t *pucStorage)
{
    rb->pucHead = pucStorage;
    rb->pucTail = pucStorage + xBufferSize;
    rb->pucFree = pucStorage;
    rb->pucRead = pucStorage;
    rb->pucWrite = pucStorage;
    rb->pucAcquire = pucStorage;
    rb->xSize = xBufferSize;
    rb->xItemsWaiting = 0;
    rb->uxRingbufferFlags = 0;
    rb->is_static = false;

    if (xBufferType == RINGBUF_TYPE_NOSPLIT) {
        rb->xMaxItemSize = rbALIGN_SIZE(rb->xSize / 2) - rbHEADER_SIZE;
    } else if (xBufferType == RINGBUF_TYPE_ALLOWSPLIT) {
        rb->uxRingbufferFlags |= rbALLOW_SPLIT_FLAG;
        rb->xMaxItemSize = rb->xSize - (rbHEADER_SIZE * 2);
    } else {
        rb->uxRingbufferFlags |= rbBYTE_BUFFER_FLAG;
        rb->xMaxItemSize = rb->xSize;
    }

#if VSF_USE_KERNEL == ENABLED
    __rb_sem_init(rb);
#endif
}

/*============================ PUBLIC API ====================================*/

RingbufHandle_t xRingbufferCreate(size_t buffer_size, RingbufferType_t type)
{
    if (buffer_size == 0) {
        return NULL;
    }
    if (type != RINGBUF_TYPE_BYTEBUF) {
        buffer_size = rbALIGN_SIZE(buffer_size);
    }

    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)vsf_heap_malloc(sizeof(*rb));
    if (rb == NULL) {
        return NULL;
    }
    memset(rb, 0, sizeof(*rb));
    uint8_t *storage = (uint8_t *)vsf_heap_malloc(buffer_size);
    if (storage == NULL) {
        vsf_heap_free(rb);
        return NULL;
    }
    memset(storage, 0, buffer_size);
    prvInitializeNewRingbuffer(buffer_size, type, rb, storage);
    return (RingbufHandle_t)rb;
}

void vRingbufferDelete(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    __rb_cancel(rb);

    if (!rb->is_static) {
        if (rb->pucHead != NULL) {
#if VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED
            if (rb->heap != NULL) {
                __vsf_heap_free(rb->heap, rb->pucHead);
            } else
#endif
            {
                vsf_heap_free(rb->pucHead);
            }
        }
#if VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED
        if (rb->heap != NULL) {
            __vsf_heap_free(rb->heap, rb);
        } else
#endif
        {
            vsf_heap_free(rb);
        }
    }
}

BaseType_t xRingbufferSend(RingbufHandle_t handle, const void *data,
                            size_t data_size, TickType_t ticks_to_wait)
{
    if (handle == NULL) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    bool is_bytebuf = (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) != 0;
    if (is_bytebuf && data_size == 0) {
        return pdTRUE;
    }
    if ((data == NULL) || (data_size == 0)) {
        return pdFALSE;
    }
    if (data_size > rb->xMaxItemSize) {
        return pdFALSE;
    }

#if VSF_USE_KERNEL == ENABLED
    __rb_deadline_t dl;
    __rb_deadline_init(&dl, ticks_to_wait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_int();
        bool fits;
        if (is_bytebuf) {
            fits = prvCheckItemFitsByteBuf(rb, data_size);
        } else {
            fits = prvCheckItemFitsDefault(rb, data_size);
        }
        if (fits) {
            if (is_bytebuf) {
                prvCopyItemByteBuf(rb, (const uint8_t *)data, data_size);
            } else if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
                prvCopyItemAllowSplit(rb, (const uint8_t *)data, data_size);
            } else {
                prvCopyItemNoSplit(rb, (const uint8_t *)data, data_size);
            }
            vsf_unprotect_int(orig);
            __rb_wake_data(rb);
            return pdTRUE;
        }
        vsf_unprotect_int(orig);

        vsf_timeout_tick_t rem = __rb_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            break;
        }
        (void)vsf_thread_sem_pend(&rb->space_sem, rem);
    }
    return pdFALSE;
#else
    (void)ticks_to_wait;

    vsf_protect_t orig = vsf_protect_int();
    bool fits;
    if (is_bytebuf) {
        fits = prvCheckItemFitsByteBuf(rb, data_size);
    } else {
        fits = prvCheckItemFitsDefault(rb, data_size);
    }
    if (!fits) {
        vsf_unprotect_int(orig);
        return pdFALSE;
    }
    if (is_bytebuf) {
        prvCopyItemByteBuf(rb, (const uint8_t *)data, data_size);
    } else if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        prvCopyItemAllowSplit(rb, (const uint8_t *)data, data_size);
    } else {
        prvCopyItemNoSplit(rb, (const uint8_t *)data, data_size);
    }
    vsf_unprotect_int(orig);
    return pdTRUE;
#endif
}

void * xRingbufferReceive(RingbufHandle_t handle, size_t *item_size,
                            TickType_t ticks_to_wait)
{
    if ((handle == NULL) || (item_size == NULL)) {
        return NULL;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        vsf_trace_error("xRingbufferReceive called on ALLOWSPLIT handle;"
                        " use xRingbufferReceiveSplit"
                        VSF_TRACE_CFG_LINEEND);
        return NULL;
    }

    bool is_bytebuf = (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) != 0;

#if VSF_USE_KERNEL == ENABLED
    __rb_deadline_t dl;
    __rb_deadline_init(&dl, ticks_to_wait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_int();
        if (prvCheckItemAvail(rb)) {
            void *ret;
            if (is_bytebuf) {
                ret = prvGetItemByteBuf(rb, 0, item_size);
            } else {
                bool split;
                ret = prvGetItemDefault(rb, &split, item_size);
                (void)split;
            }
            vsf_unprotect_int(orig);
            return ret;
        }
        vsf_unprotect_int(orig);

        vsf_timeout_tick_t rem = __rb_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            break;
        }
        (void)vsf_thread_sem_pend(&rb->data_sem, rem);
    }
    return NULL;
#else
    (void)ticks_to_wait;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return NULL;
    }
    void *ret;
    if (is_bytebuf) {
        ret = prvGetItemByteBuf(rb, 0, item_size);
    } else {
        bool split;
        ret = prvGetItemDefault(rb, &split, item_size);
        (void)split;
    }
    vsf_unprotect_int(orig);
    return ret;
#endif
}

void * xRingbufferReceiveUpTo(RingbufHandle_t handle, size_t *item_size,
                                TickType_t ticks_to_wait, size_t wanted_size)
{
    if ((handle == NULL) || (item_size == NULL) || (wanted_size == 0)) {
        return NULL;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

#if VSF_USE_KERNEL == ENABLED
    __rb_deadline_t dl;
    __rb_deadline_init(&dl, ticks_to_wait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_int();
        if (prvCheckItemAvail(rb)) {
            void *ret = prvGetItemByteBuf(rb, wanted_size, item_size);
            vsf_unprotect_int(orig);
            return ret;
        }
        vsf_unprotect_int(orig);

        vsf_timeout_tick_t rem = __rb_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            break;
        }
        (void)vsf_thread_sem_pend(&rb->data_sem, rem);
    }
    return NULL;
#else
    (void)ticks_to_wait;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return NULL;
    }
    void *ret = prvGetItemByteBuf(rb, wanted_size, item_size);
    vsf_unprotect_int(orig);
    return ret;
#endif
}

void vRingbufferReturnItem(RingbufHandle_t handle, void *item)
{
    if ((handle == NULL) || (item == NULL)) {
        return;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    if (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
        prvReturnItemByteBuf(rb, (uint8_t *)item);
    } else {
        prvReturnItemDefault(rb, (uint8_t *)item);
    }
    vsf_unprotect_int(orig);

    __rb_wake_space(rb);
}

size_t xRingbufferGetCurFreeSize(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return 0;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    size_t free;
    if (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
        free = prvGetCurMaxSizeByteBuf(rb);
    } else if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        free = prvGetCurMaxSizeAllowSplit(rb);
    } else {
        free = prvGetCurMaxSizeNoSplit(rb);
    }
    vsf_unprotect_int(orig);
    return free;
}

size_t xRingbufferGetCurFilledSize(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return 0;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    size_t filled = rb->xItemsWaiting;
    vsf_unprotect_int(orig);
    return filled;
}

size_t xRingbufferGetMaxItemSize(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return 0;
    }
    return ((struct __vsf_espidf_ringbuf *)handle)->xMaxItemSize;
}

BaseType_t xRingbufferReceiveSplit(RingbufHandle_t handle,
                                    void **ppvHeadItem,
                                    void **ppvTailItem,
                                    size_t *pxHeadItemSize,
                                    size_t *pxTailItemSize,
                                    TickType_t ticks_to_wait)
{
    if ((handle == NULL) || (ppvHeadItem == NULL) || (ppvTailItem == NULL)
            || (pxHeadItemSize == NULL) || (pxTailItemSize == NULL)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    *ppvHeadItem = NULL;
    *ppvTailItem = NULL;

#if VSF_USE_KERNEL == ENABLED
    __rb_deadline_t dl;
    __rb_deadline_init(&dl, ticks_to_wait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_int();
        if (prvCheckItemAvail(rb)) {
            bool is_split = false;
            *ppvHeadItem = prvGetItemDefault(rb, &is_split, pxHeadItemSize);
            if (is_split) {
                bool no_split;
                *ppvTailItem = prvGetItemDefault(rb, &no_split,
                                                 pxTailItemSize);
                (void)no_split;
            } else {
                *ppvTailItem = NULL;
                *pxTailItemSize = 0;
            }
            vsf_unprotect_int(orig);
            return pdTRUE;
        }
        vsf_unprotect_int(orig);

        vsf_timeout_tick_t rem = __rb_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            break;
        }
        (void)vsf_thread_sem_pend(&rb->data_sem, rem);
    }
    return pdFALSE;
#else
    (void)ticks_to_wait;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return pdFALSE;
    }
    bool is_split = false;
    *ppvHeadItem = prvGetItemDefault(rb, &is_split, pxHeadItemSize);
    if (is_split) {
        bool no_split;
        *ppvTailItem = prvGetItemDefault(rb, &no_split, pxTailItemSize);
        (void)no_split;
    } else {
        *ppvTailItem = NULL;
        *pxTailItemSize = 0;
    }
    vsf_unprotect_int(orig);
    return pdTRUE;
#endif
}

RingbufHandle_t xRingbufferCreateNoSplit(size_t xItemSize, size_t xItemNum)
{
    return xRingbufferCreate(
        (rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE) * xItemNum,
        RINGBUF_TYPE_NOSPLIT);
}

/* ---------- ISR variants (no blocking) ---------------------------------- */

BaseType_t xRingbufferSendFromISR(RingbufHandle_t handle,
                                   const void *data, size_t data_size,
                                   BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((handle == NULL) || (data == NULL) || (data_size == 0)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    bool is_bytebuf = (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) != 0;
    if (is_bytebuf && data_size == 0) {
        return pdTRUE;
    }
    if (data_size > rb->xMaxItemSize) {
        return pdFALSE;
    }

    vsf_protect_t orig = vsf_protect_int();
    bool fits;
    if (is_bytebuf) {
        fits = prvCheckItemFitsByteBuf(rb, data_size);
    } else {
        fits = prvCheckItemFitsDefault(rb, data_size);
    }
    if (!fits) {
        vsf_unprotect_int(orig);
        return pdFALSE;
    }
    if (is_bytebuf) {
        prvCopyItemByteBuf(rb, (const uint8_t *)data, data_size);
    } else if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        prvCopyItemAllowSplit(rb, (const uint8_t *)data, data_size);
    } else {
        prvCopyItemNoSplit(rb, (const uint8_t *)data, data_size);
    }
    vsf_unprotect_int(orig);

    __rb_wake_data_isr(rb, pxHigherPriorityTaskWoken);
    return pdTRUE;
}

void * xRingbufferReceiveFromISR(RingbufHandle_t handle,
                                  size_t *item_size,
                                  BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((handle == NULL) || (item_size == NULL)) {
        return NULL;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    if (rb->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        return NULL;
    }
    bool is_bytebuf = (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) != 0;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return NULL;
    }
    void *ret;
    if (is_bytebuf) {
        ret = prvGetItemByteBuf(rb, 0, item_size);
    } else {
        bool split;
        ret = prvGetItemDefault(rb, &split, item_size);
        (void)split;
    }
    vsf_unprotect_int(orig);

    __rb_wake_space_isr(rb, pxHigherPriorityTaskWoken);
    return ret;
}

void * xRingbufferReceiveUpToFromISR(RingbufHandle_t handle,
                                      size_t *item_size, size_t wanted_size,
                                      BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((handle == NULL) || (item_size == NULL) || (wanted_size == 0)) {
        return NULL;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return NULL;
    }
    void *ret = prvGetItemByteBuf(rb, wanted_size, item_size);
    vsf_unprotect_int(orig);

    __rb_wake_space_isr(rb, pxHigherPriorityTaskWoken);
    return ret;
}

BaseType_t xRingbufferReceiveSplitFromISR(RingbufHandle_t handle,
                                           void **ppvHeadItem,
                                           void **ppvTailItem,
                                           size_t *pxHeadItemSize,
                                           size_t *pxTailItemSize,
                                           BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((handle == NULL) || (ppvHeadItem == NULL) || (ppvTailItem == NULL)
            || (pxHeadItemSize == NULL) || (pxTailItemSize == NULL)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    *ppvHeadItem = NULL;
    *ppvTailItem = NULL;

    vsf_protect_t orig = vsf_protect_int();
    if (!prvCheckItemAvail(rb)) {
        vsf_unprotect_int(orig);
        return pdFALSE;
    }
    bool is_split = false;
    *ppvHeadItem = prvGetItemDefault(rb, &is_split, pxHeadItemSize);
    if (is_split) {
        bool no_split;
        *ppvTailItem = prvGetItemDefault(rb, &no_split, pxTailItemSize);
        (void)no_split;
    } else {
        *ppvTailItem = NULL;
        *pxTailItemSize = 0;
    }
    vsf_unprotect_int(orig);

    __rb_wake_space_isr(rb, pxHigherPriorityTaskWoken);
    return pdTRUE;
}

void vRingbufferReturnItemFromISR(RingbufHandle_t handle, void *item,
                                   BaseType_t *pxHigherPriorityTaskWoken)
{
    if (pxHigherPriorityTaskWoken != NULL) {
        *pxHigherPriorityTaskWoken = pdFALSE;
    }
    if ((handle == NULL) || (item == NULL)) {
        return;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    if (rb->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
        prvReturnItemByteBuf(rb, (uint8_t *)item);
    } else {
        prvReturnItemDefault(rb, (uint8_t *)item);
    }
    vsf_unprotect_int(orig);

    __rb_wake_space_isr(rb, pxHigherPriorityTaskWoken);
}

/* ---------- Two-phase send (NOSPLIT only) ------------------------------- */

BaseType_t xRingbufferSendAcquire(RingbufHandle_t handle, void **ppvItem,
                                   size_t xItemSize, TickType_t ticks_to_wait)
{
    if ((handle == NULL) || (ppvItem == NULL)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    if ((rb->uxRingbufferFlags & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG))
            != 0) {
        return pdFALSE;
    }
    if (xItemSize > rb->xMaxItemSize) {
        return pdFALSE;
    }

    *ppvItem = NULL;

#if VSF_USE_KERNEL == ENABLED
    __rb_deadline_t dl;
    __rb_deadline_init(&dl, ticks_to_wait);

    for (;;) {
        vsf_protect_t orig = vsf_protect_int();
        if (prvCheckItemFitsDefault(rb, xItemSize)) {
            *ppvItem = prvAcquireItemNoSplit(rb, xItemSize);
            vsf_unprotect_int(orig);
            return pdTRUE;
        }
        vsf_unprotect_int(orig);

        vsf_timeout_tick_t rem = __rb_deadline_remaining(&dl);
        if (rem == 0 && !dl.infinite) {
            break;
        }
        (void)vsf_thread_sem_pend(&rb->space_sem, rem);
    }
    return pdFALSE;
#else
    (void)ticks_to_wait;

    vsf_protect_t orig = vsf_protect_int();
    if (prvCheckItemFitsDefault(rb, xItemSize)) {
        *ppvItem = prvAcquireItemNoSplit(rb, xItemSize);
        vsf_unprotect_int(orig);
        return pdTRUE;
    }
    vsf_unprotect_int(orig);
    return pdFALSE;
#endif
}

BaseType_t xRingbufferSendComplete(RingbufHandle_t handle, void *pvItem)
{
    if ((handle == NULL) || (pvItem == NULL)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    if ((rb->uxRingbufferFlags & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG))
            != 0) {
        return pdFALSE;
    }

    vsf_protect_t orig = vsf_protect_int();
    prvSendItemDoneNoSplit(rb, (uint8_t *)pvItem);
    vsf_unprotect_int(orig);

    __rb_wake_data(rb);
    return pdTRUE;
}

/* ---------- Debug helpers ----------------------------------------------- */

esp_err_t vRingbufferReset(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    if ((rb->pucAcquire != rb->pucWrite)
            || (rb->pucRead != rb->pucFree)) {
        vsf_unprotect_int(orig);
        return ESP_ERR_INVALID_STATE;
    }

    rb->pucAcquire = rb->pucHead;
    rb->pucWrite   = rb->pucHead;
    rb->pucRead    = rb->pucHead;
    rb->pucFree    = rb->pucHead;
    rb->xItemsWaiting = 0;
    rb->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
    vsf_unprotect_int(orig);

    __rb_wake_space(rb);
    return ESP_OK;
}

void vRingbufferGetInfo(RingbufHandle_t handle,
                         UBaseType_t *uxFree, UBaseType_t *uxRead,
                         UBaseType_t *uxWrite, UBaseType_t *uxAcquire,
                         UBaseType_t *uxItemsWaiting)
{
    if (handle == NULL) {
        return;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    vsf_protect_t orig = vsf_protect_int();
    if (uxFree != NULL) {
        *uxFree = (UBaseType_t)(rb->pucFree - rb->pucHead);
    }
    if (uxRead != NULL) {
        *uxRead = (UBaseType_t)(rb->pucRead - rb->pucHead);
    }
    if (uxWrite != NULL) {
        *uxWrite = (UBaseType_t)(rb->pucWrite - rb->pucHead);
    }
    if (uxAcquire != NULL) {
        *uxAcquire = (UBaseType_t)(rb->pucAcquire - rb->pucHead);
    }
    if (uxItemsWaiting != NULL) {
        *uxItemsWaiting = (UBaseType_t)(rb->xItemsWaiting);
    }
    vsf_unprotect_int(orig);
}

void xRingbufferPrintInfo(RingbufHandle_t handle)
{
    if (handle == NULL) {
        return;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;

    uint32_t flags = rb->uxRingbufferFlags;
    printf("RingBuffer Size: %" PRId32 ", FreeSize: %" PRId32 "\n"
           "  Read: %" PRId32 ", Free: %" PRId32 ", Write: %" PRId32
           ", Acquire: %" PRId32 ", Waiting: %" PRId32
           ", Flags: 0x%" PRIx32 " [",
           (int32_t)rb->xSize,
           (int32_t)prvGetFreeSize(rb),
           (int32_t)(rb->pucRead - rb->pucHead),
           (int32_t)(rb->pucFree - rb->pucHead),
           (int32_t)(rb->pucWrite - rb->pucHead),
           (int32_t)(rb->pucAcquire - rb->pucHead),
           (int32_t)rb->xItemsWaiting,
           flags);

    if (flags) {
        if (flags & rbALLOW_SPLIT_FLAG)  printf(" [ALLOW_SPLIT]");
        if (flags & rbBYTE_BUFFER_FLAG)  printf(" [BYTE_BUFFER]");
        if (flags & rbBUFFER_FULL_FLAG)  printf(" [FULL]");
    }
    printf(" ]\n  Items:\n");

    uint8_t *ptr = rb->pucHead;
    size_t max_steps = (rb->xSize / rbHEADER_SIZE) + 4;
    for (size_t steps = 0;
         ptr + rbHEADER_SIZE <= rb->pucTail && steps < max_steps;
         ++steps) {
        unsigned offset = (unsigned)(ptr - rb->pucHead);
        ItemHeader_t *hdr = (ItemHeader_t *)ptr;
        unsigned item_flags = hdr->uxItemFlags;
        unsigned item_len   = (unsigned)hdr->xItemLen;
        printf("  [%4u] Size: %u Flags: 0x%x [", offset, item_len, item_flags);

        if (item_len > rb->xMaxItemSize
                && !(item_flags & rbITEM_DUMMY_DATA_FLAG)) {
            printf(" [INVALID HEADER or UNUSED SPACE]\n");
            break;
        }
        if (item_flags) {
            if (item_flags & rbITEM_FREE_FLAG)    printf(" [FREE]");
            if (item_flags & rbITEM_DUMMY_DATA_FLAG) printf(" [DUMMY]");
            if (item_flags & rbITEM_SPLIT_FLAG)   printf(" [SPLIT]");
            if (item_flags & rbITEM_WRITTEN_FLAG) printf(" [WRITTEN]");
        }
        printf(" ]\n");

        if (item_flags & rbITEM_DUMMY_DATA_FLAG) {
            ptr = rb->pucHead;
        } else {
            size_t step = rbHEADER_SIZE + rbALIGN_SIZE(item_len);
            if (step == 0) break;
            ptr += step;
        }
        if (ptr == rb->pucHead) break;
    }
}

/* ---------- Static / caps allocation ------------------------------------ */

RingbufHandle_t xRingbufferCreateStatic(size_t buffer_size,
                                         RingbufferType_t type,
                                         uint8_t *pucStorage,
                                         StaticRingbuffer_t *pxStatic)
{
    if ((buffer_size == 0) || (pucStorage == NULL) || (pxStatic == NULL)) {
        return NULL;
    }
    if (type != RINGBUF_TYPE_BYTEBUF) {
        buffer_size = rbALIGN_SIZE(buffer_size);
    }

    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)pxStatic;
    memset(rb, 0, sizeof(*rb));
    memset(pucStorage, 0, buffer_size);
    prvInitializeNewRingbuffer(buffer_size, type, rb, pucStorage);
    rb->is_static = true;
    return (RingbufHandle_t)rb;
}

BaseType_t xRingbufferGetStaticBuffer(RingbufHandle_t handle,
                                       uint8_t **ppucStorage,
                                       StaticRingbuffer_t **ppxStatic)
{
    if ((handle == NULL) || (ppucStorage == NULL) || (ppxStatic == NULL)) {
        return pdFALSE;
    }
    struct __vsf_espidf_ringbuf *rb =
        (struct __vsf_espidf_ringbuf *)handle;
    if (!rb->is_static) {
        return pdFALSE;
    }
    *ppucStorage = rb->pucHead;
    *ppxStatic   = (StaticRingbuffer_t *)rb;
    return pdTRUE;
}

RingbufHandle_t xRingbufferCreateWithCaps(size_t buffer_size,
                                           RingbufferType_t type,
                                           UBaseType_t uxMemoryCaps)
{
    if (buffer_size == 0) {
        return NULL;
    }
    if (type != RINGBUF_TYPE_BYTEBUF) {
        buffer_size = rbALIGN_SIZE(buffer_size);
    }

    vsf_heap_t *heap = NULL;
#if VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED
    vsf_heap_t *(*cb)(uint32_t) = vsf_espidf_get_caps_to_heap();
    if (cb != NULL) {
        heap = cb((uint32_t)uxMemoryCaps);
    }
#endif

    struct __vsf_espidf_ringbuf *rb;
    uint8_t *storage;
    if (heap != NULL) {
        rb      = (struct __vsf_espidf_ringbuf *)
                  __vsf_heap_malloc_aligned(heap, sizeof(*rb),
                                            VSF_HEAP_ALIGN);
        storage = (uint8_t *)
                  __vsf_heap_malloc_aligned(heap, buffer_size,
                                            VSF_HEAP_ALIGN);
    } else {
        rb      = (struct __vsf_espidf_ringbuf *)
                  vsf_heap_malloc(sizeof(*rb));
        storage = (uint8_t *)vsf_heap_malloc(buffer_size);
    }

    if ((rb == NULL) || (storage == NULL)) {
        if (storage != NULL) {
            if (heap) __vsf_heap_free(heap, storage);
            else      vsf_heap_free(storage);
        }
        if (rb != NULL) {
            if (heap) __vsf_heap_free(heap, rb);
            else      vsf_heap_free(rb);
        }
        return NULL;
    }
    memset(storage, 0, buffer_size);
    prvInitializeNewRingbuffer(buffer_size, type, rb, storage);
#if VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED
    rb->heap = heap;
#endif
    return (RingbufHandle_t)rb;
}

void vRingbufferDeleteWithCaps(RingbufHandle_t handle)
{
    // Same as vRingbufferDelete — the stored heap pointer handles caps
    // cleanup automatically.
    vRingbufferDelete(handle);
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_RINGBUF
