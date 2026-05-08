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
 * multi_heap.h -> VSF vsf_heap_t instance bridge.
 *
 * Uses PLOOC implement(vsf_heap_t) to create per-instance heaps with
 * private freelist storage, mirroring the pattern of vsf_heap_init().
 */

#define __VSF_HEAP_CLASS_IMPLEMENT

#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_HEAP_CAPS == ENABLED

#include "multi_heap.h"
#include "service/heap/vsf_heap.h"

#include <string.h>

struct vsf_espidf_multi_heap {
    implement(vsf_heap_t)
    vsf_dlist_t   __freelist[20];
};

multi_heap_handle_t multi_heap_register(void *start, size_t size)
{
    struct vsf_espidf_multi_heap *mh;

    if ((start == NULL) || (size == 0)) {
        return NULL;
    }

    mh = (struct vsf_espidf_multi_heap *)vsf_heap_malloc(sizeof(*mh));
    if (mh == NULL) {
        return NULL;
    }
    memset(mh, 0, sizeof(*mh));

    mh->freelist     = &mh->__freelist[0];
    mh->freelist_num = dimof(mh->__freelist);
    __vsf_heap_init(&mh->use_as__vsf_heap_t);

    __vsf_heap_add_buffer(&mh->use_as__vsf_heap_t, (uint8_t *)start, (uint_fast32_t)size);
    return (multi_heap_handle_t)mh;
}

void *multi_heap_malloc(multi_heap_handle_t heap, size_t size)
{
    if (heap == NULL) { return NULL; }
    return __vsf_heap_malloc_aligned(&heap->use_as__vsf_heap_t,
                                      (uint_fast32_t)size, VSF_HEAP_ALIGN);
}

void *multi_heap_aligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment)
{
    if (heap == NULL) { return NULL; }
    return __vsf_heap_malloc_aligned(&heap->use_as__vsf_heap_t,
                                      (uint_fast32_t)size, (uint_fast32_t)alignment);
}

void *multi_heap_aligned_alloc_offs(multi_heap_handle_t heap, size_t size,
                                     size_t alignment, size_t offset)
{
    if ((heap == NULL) || (offset != 0)) { return NULL; }
    return __vsf_heap_malloc_aligned(&heap->use_as__vsf_heap_t,
                                      (uint_fast32_t)size, (uint_fast32_t)alignment);
}

void multi_heap_free(multi_heap_handle_t heap, void *p)
{
    if ((heap == NULL) || (p == NULL)) { return; }
    __vsf_heap_free(&heap->use_as__vsf_heap_t, p);
}

void *multi_heap_realloc(multi_heap_handle_t heap, void *p, size_t size)
{
    if (heap == NULL) { return NULL; }
    return __vsf_heap_realloc_aligned(&heap->use_as__vsf_heap_t, p,
                                       (uint_fast32_t)size, VSF_HEAP_ALIGN);
}

size_t multi_heap_get_allocated_size(multi_heap_handle_t heap, void *p)
{
    if ((heap == NULL) || (p == NULL)) { return 0; }
    return (size_t)__vsf_heap_size(&heap->use_as__vsf_heap_t, p);
}

void multi_heap_set_lock(multi_heap_handle_t heap, void *lock)
{
    (void)heap;
    (void)lock;
}

void multi_heap_dump(multi_heap_handle_t heap)
{
    if (heap == NULL) { return; }
#if VSF_HEAP_CFG_TRACE_LEAKAGE == ENABLED
    __vsf_heap_dump(&heap->use_as__vsf_heap_t, false);
#endif
}

bool multi_heap_check(multi_heap_handle_t heap, bool print_errors)
{
    (void)print_errors;
    return (heap != NULL);
}

size_t multi_heap_free_size(multi_heap_handle_t heap)
{
    multi_heap_info_t info;
    if (heap == NULL) { return 0; }
    multi_heap_get_info(heap, &info);
    return info.total_free_bytes;
}

size_t multi_heap_minimum_free_size(multi_heap_handle_t heap)
{
    multi_heap_info_t info;
    if (heap == NULL) { return 0; }
    multi_heap_get_info(heap, &info);
    return info.minimum_free_bytes;
}

void multi_heap_get_info(multi_heap_handle_t heap, multi_heap_info_t *info)
{
    if ((heap == NULL) || (info == NULL)) { return; }
    memset(info, 0, sizeof(*info));
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    {
        vsf_heap_statistics_t stats;
        __vsf_heap_statistics(&heap->use_as__vsf_heap_t, &stats);
        info->total_free_bytes      = (size_t)(stats.all_size - stats.used_size);
        info->total_allocated_bytes = (size_t)stats.used_size;
        info->largest_free_block    = (size_t)stats.largest_free_block;
        info->minimum_free_bytes    = stats.all_size - stats.max_used_size;
    }
#endif
}

size_t multi_heap_reset_minimum_free_bytes(multi_heap_handle_t heap)
{
    multi_heap_info_t info;
    if (heap == NULL) { return 0; }
    multi_heap_get_info(heap, &info);
    return info.minimum_free_bytes;
}

void multi_heap_restore_minimum_free_bytes(multi_heap_handle_t heap,
                                            const size_t new_minimum_free_bytes_value)
{
    (void)heap;
    (void)new_minimum_free_bytes_value;
}

void multi_heap_walk(multi_heap_handle_t heap, multi_heap_walker_cb_t walker_func,
                     void *user_data)
{
    (void)heap;
    (void)walker_func;
    (void)user_data;
}

size_t multi_heap_get_full_block_size(multi_heap_handle_t heap, void *p)
{
    return multi_heap_get_allocated_size(heap, p);
}

void *multi_heap_find_containing_block(multi_heap_handle_t heap, void *ptr)
{
    (void)heap;
    (void)ptr;
    return NULL;
}

#endif      // VSF_USE_ESPIDF && VSF_ESPIDF_CFG_USE_HEAP_CAPS
