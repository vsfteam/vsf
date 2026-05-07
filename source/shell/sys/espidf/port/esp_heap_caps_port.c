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
 * esp_heap_caps.h -> vsf_heap bridge.
 *
 * VSF exposes a single flat managed heap (service/heap/vsf_heap). The
 * ESP-IDF caps mask is therefore accepted and validated (MALLOC_CAP_INVALID
 * fails) but otherwise ignored: every request is served from the same
 * underlying pool. This matches ESP-IDF behaviour on hosts / single-pool
 * internal-RAM-only chips.
 *
 * Statistics query surface today:
 *   heap_caps_get_total_size()        = vsf_heap_statistics.all_size
 *   heap_caps_get_free_size()         = all_size - used_size
 *   heap_caps_get_largest_free_block()= vsf_heap_statistics.largest_free_block
 *                                       (exact: lazily scanned on demand)
 *   heap_caps_get_minimum_free_size() = all_size - max_used_size
 *                                       (historical low-water of free size)
 *
 * Block counts reported by heap_caps_get_info() are left at 0 because the
 * underlying VSF API does not surface them. Callers that want these values
 * should drive a dedicated vsf_heap walk themselves.
 */

/*============================ INCLUDES ======================================*/

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED

#include <string.h>

#include "esp_heap_caps.h"

#include "../vsf_espidf.h"

#if !defined(VSF_USE_HEAP) || VSF_USE_HEAP != ENABLED
#   error "esp_heap_caps port requires VSF_USE_HEAP == ENABLED"
#endif

#include "service/heap/vsf_heap.h"

/*============================ MACROS ========================================*/

/* Flags that we can honour in this flat-pool backend. */
#define __CAPS_SUPPORTED                                                       \
    (MALLOC_CAP_EXEC | MALLOC_CAP_32BIT | MALLOC_CAP_8BIT | MALLOC_CAP_DMA |   \
     MALLOC_CAP_SPIRAM | MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT |            \
     MALLOC_CAP_IRAM_8BIT | MALLOC_CAP_RETENTION | MALLOC_CAP_RTCRAM |         \
     MALLOC_CAP_TCM |                                                          \
     MALLOC_CAP_PID2 | MALLOC_CAP_PID3 | MALLOC_CAP_PID4 | MALLOC_CAP_PID5 |   \
     MALLOC_CAP_PID6 | MALLOC_CAP_PID7)

/*============================ PROTOTYPES ====================================*/

static bool __caps_is_valid(uint32_t caps);

/*============================ IMPLEMENTATION ================================*/

static bool __caps_is_valid(uint32_t caps)
{
    if (caps & MALLOC_CAP_INVALID) {
        return false;
    }
    /* Unknown bits outside the documented set fail per IDF semantics. */
    if (caps & ~__CAPS_SUPPORTED) {
        return false;
    }
    return true;
}

void *heap_caps_malloc(size_t size, uint32_t caps)
{
    if (!__caps_is_valid(caps) || (size == 0)) {
        return NULL;
    }
    vsf_heap_t *(*cb)(uint32_t) = vsf_espidf_get_caps_to_heap();
    if (cb != NULL) {
        vsf_heap_t *heap = cb(caps);
        if (heap != NULL) {
            return __vsf_heap_malloc_aligned(heap, size, VSF_HEAP_ALIGN);
        }
    }
    return vsf_heap_malloc(size);
}

void heap_caps_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    vsf_heap_free(ptr);
}

void *heap_caps_realloc(void *ptr, size_t size, uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return NULL;
    }
    if (size == 0) {
        if (ptr != NULL) {
            vsf_heap_free(ptr);
        }
        return NULL;
    }
    return vsf_heap_realloc(ptr, size);
}

void *heap_caps_calloc(size_t n, size_t size, uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return NULL;
    }
    size_t total = n * size;
    if ((n != 0) && ((total / n) != size)) {
        /* size_t overflow. */
        return NULL;
    }
    if (total == 0) {
        return NULL;
    }
    void *p = vsf_heap_malloc(total);
    if (p != NULL) {
        memset(p, 0, total);
    }
    return p;
}

void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
    if (!__caps_is_valid(caps) || (size == 0)) {
        return NULL;
    }
    /* ESP-IDF requires alignment to be a power of two. */
    if ((alignment == 0) || (alignment & (alignment - 1))) {
        return NULL;
    }
    return vsf_heap_malloc_aligned(size, alignment);
}

void heap_caps_aligned_free(void *ptr)
{
    /* vsf_heap_free accepts any pointer returned by the heap. */
    if (ptr == NULL) {
        return;
    }
    vsf_heap_free(ptr);
}

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size,
                                uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return NULL;
    }
    if ((alignment == 0) || (alignment & (alignment - 1))) {
        return NULL;
    }
    size_t total = n * size;
    if ((n != 0) && ((total / n) != size)) {
        return NULL;
    }
    if (total == 0) {
        return NULL;
    }
    void *p = vsf_heap_malloc_aligned(total, alignment);
    if (p != NULL) {
        memset(p, 0, total);
    }
    return p;
}

/* ---- Statistics ---------------------------------------------------------- */

size_t heap_caps_get_total_size(uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return 0;
    }
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t stat = { 0 };
    vsf_heap_statistics(&stat);
    return (size_t)stat.all_size;
#else
    return 0;
#endif
}

size_t heap_caps_get_free_size(uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return 0;
    }
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t stat = { 0 };
    vsf_heap_statistics(&stat);
    return (stat.all_size >= stat.used_size)
            ? (size_t)(stat.all_size - stat.used_size)
            : 0;
#else
    return 0;
#endif
}

size_t heap_caps_get_minimum_free_size(uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return 0;
    }
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t stat = { 0 };
    vsf_heap_statistics(&stat);
    return (stat.all_size >= stat.max_used_size)
            ? (size_t)(stat.all_size - stat.max_used_size)
            : 0;
#else
    return 0;
#endif
}

size_t heap_caps_get_largest_free_block(uint32_t caps)
{
    if (!__caps_is_valid(caps)) {
        return 0;
    }
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t stat = { 0 };
    vsf_heap_statistics(&stat);
    return (size_t)stat.largest_free_block;
#else
    return 0;
#endif
}

void heap_caps_get_info(multi_heap_info_t *info, uint32_t caps)
{
    if (info == NULL) {
        return;
    }
    memset(info, 0, sizeof(*info));
    if (!__caps_is_valid(caps)) {
        return;
    }
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t stat = { 0 };
    vsf_heap_statistics(&stat);
    info->total_free_bytes       = (stat.all_size >= stat.used_size)
                                   ? (size_t)(stat.all_size - stat.used_size)
                                   : 0;
    info->total_allocated_bytes  = (size_t)stat.used_size;
    info->largest_free_block     = (size_t)stat.largest_free_block;
    info->minimum_free_bytes     = (stat.all_size >= stat.max_used_size)
                                   ? (size_t)(stat.all_size - stat.max_used_size)
                                   : 0;
    /* block counts are not tracked at this layer. */
#endif
}

/* ---- Integrity checks ---------------------------------------------------- */

bool heap_caps_check_integrity_all(bool print_errors)
{
    (void)print_errors;
    /* vsf_heap has internal sanity protections; no explicit walker API. */
    return true;
}

bool heap_caps_check_integrity(uint32_t caps, bool print_errors)
{
    (void)print_errors;
    return __caps_is_valid(caps);
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_HEAP_CAPS
