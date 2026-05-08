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
 * Clean-room re-implementation of ESP-IDF public API "esp_heap_caps.h".
 *
 * This file is authored from the ESP-IDF v5.x public API reference only.
 * No code is copied or derived from the ESP-IDF source tree. Flag values
 * and function signatures are kept compatible so that unmodified ESP-IDF
 * example applications can compile and link against this header.
 *
 * VSF backend: all capabilities are served from the single managed heap
 * (service/heap/vsf_heap). The caps bitmask is therefore accepted but
 * largely ignored -- allocations succeed whenever the underlying heap
 * can satisfy the request. This mirrors host / single-pool IDF builds.
 */

#ifndef __VSF_ESPIDF_ESP_HEAP_CAPS_H__
#define __VSF_ESPIDF_ESP_HEAP_CAPS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "esp_err.h"
#include "multi_heap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/* Bit values fixed by ESP-IDF public API contract. */
#define MALLOC_CAP_EXEC                 (1u << 0)
#define MALLOC_CAP_32BIT                (1u << 1)
#define MALLOC_CAP_8BIT                 (1u << 2)
#define MALLOC_CAP_DMA                  (1u << 3)
#define MALLOC_CAP_PID2                 (1u << 4)
#define MALLOC_CAP_PID3                 (1u << 5)
#define MALLOC_CAP_PID4                 (1u << 6)
#define MALLOC_CAP_PID5                 (1u << 7)
#define MALLOC_CAP_PID6                 (1u << 8)
#define MALLOC_CAP_PID7                 (1u << 9)
#define MALLOC_CAP_SPIRAM               (1u << 10)
#define MALLOC_CAP_INTERNAL             (1u << 11)
#define MALLOC_CAP_DEFAULT              (1u << 12)
#define MALLOC_CAP_IRAM_8BIT            (1u << 13)
#define MALLOC_CAP_RETENTION            (1u << 14)
#define MALLOC_CAP_RTCRAM               (1u << 16)
#define MALLOC_CAP_TCM                  (1u << 17)
#define MALLOC_CAP_INVALID              (1u << 31)

/*============================ PROTOTYPES ====================================*/

/*
 * Caveat: the VSF backend is a single flat heap, so the caps argument is
 * accepted and validated (MALLOC_CAP_INVALID fails) but otherwise ignored.
 * All queries report figures for the whole heap regardless of caps.
 */

extern void    *heap_caps_malloc(size_t size, uint32_t caps);
extern void     heap_caps_free(void *ptr);
extern void    *heap_caps_realloc(void *ptr, size_t size, uint32_t caps);
extern void    *heap_caps_calloc(size_t n, size_t size, uint32_t caps);

extern void    *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps);
extern void     heap_caps_aligned_free(void *ptr);
extern void    *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size, uint32_t caps);

extern size_t   heap_caps_get_total_size(uint32_t caps);
extern size_t   heap_caps_get_free_size(uint32_t caps);
extern size_t   heap_caps_get_minimum_free_size(uint32_t caps);
extern size_t   heap_caps_get_largest_free_block(uint32_t caps);

extern void     heap_caps_get_info(multi_heap_info_t *info, uint32_t caps);

extern bool     heap_caps_check_integrity_all(bool print_errors);
extern bool     heap_caps_check_integrity(uint32_t caps, bool print_errors);

#ifdef __cplusplus
}
#endif

#endif      // __VSF_ESPIDF_ESP_HEAP_CAPS_H__
