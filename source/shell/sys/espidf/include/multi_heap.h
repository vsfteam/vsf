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
 * Clean-room re-implementation of ESP-IDF public API "multi_heap.h".
 *
 * Provides independent heap instances backed by VSF's per-instance
 * vsf_heap_t API (__vsf_heap_init / __vsf_heap_add_buffer / ...).
 *
 * Baseline: ESP-IDF v5.1 public API.
 */

#ifndef __VSF_ESPIDF_MULTI_HEAP_H__
#define __VSF_ESPIDF_MULTI_HEAP_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vsf_espidf_multi_heap *multi_heap_handle_t;

typedef struct {
    size_t total_free_bytes;
    size_t total_allocated_bytes;
    size_t largest_free_block;
    size_t minimum_free_bytes;
    size_t allocated_blocks;
    size_t free_blocks;
    size_t total_blocks;
} multi_heap_info_t;

void *multi_heap_aligned_alloc(multi_heap_handle_t heap, size_t size, size_t alignment);
void *multi_heap_malloc(multi_heap_handle_t heap, size_t size);
void  multi_heap_free(multi_heap_handle_t heap, void *p);
void *multi_heap_realloc(multi_heap_handle_t heap, void *p, size_t size);
size_t multi_heap_get_allocated_size(multi_heap_handle_t heap, void *p);

multi_heap_handle_t multi_heap_register(void *start, size_t size);
void multi_heap_set_lock(multi_heap_handle_t heap, void *lock);

void multi_heap_dump(multi_heap_handle_t heap);
bool multi_heap_check(multi_heap_handle_t heap, bool print_errors);

size_t multi_heap_free_size(multi_heap_handle_t heap);
size_t multi_heap_minimum_free_size(multi_heap_handle_t heap);

void multi_heap_get_info(multi_heap_handle_t heap, multi_heap_info_t *info);

void *multi_heap_aligned_alloc_offs(multi_heap_handle_t heap, size_t size,
                                    size_t alignment, size_t offset);
size_t multi_heap_reset_minimum_free_bytes(multi_heap_handle_t heap);
void multi_heap_restore_minimum_free_bytes(multi_heap_handle_t heap,
                                           const size_t new_minimum_free_bytes_value);

typedef bool (*multi_heap_walker_cb_t)(void *block_ptr, size_t block_size,
                                       int block_used, void *user_data);
void multi_heap_walk(multi_heap_handle_t heap, multi_heap_walker_cb_t walker_func,
                     void *user_data);

size_t multi_heap_get_full_block_size(multi_heap_handle_t heap, void *p);
void *multi_heap_find_containing_block(multi_heap_handle_t heap, void *ptr);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_MULTI_HEAP_H__
