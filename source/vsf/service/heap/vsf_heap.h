/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#ifndef __VSF_HEAP_H__
#define __VSF_HEAP_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USE_HEAP == ENABLED

#ifndef VSF_HEAP_SIZE
#   define VSF_HEAP_SIZE    (128 * 1024)
#endif

/*============================ TYPES =========================================*/

declare_interface(i_heap_t)

//! \name vsf heap interface
//! @{
def_interface(i_heap_t)
    void (*Init)            (void);
    void (*Add)             (uint8_t *heap, uint_fast32_t size);
    void (*AddMemory)       (vsf_mem_t mem);
    void *(*MallocAligned)  (uint_fast32_t size, uint_fast32_t alignment);
    void *(*Malloc)         (uint_fast32_t size);
    void *(*ReallocAligned) (void *buffer, uint_fast32_t size, uint_fast32_t alignment);
    void *(*Realloc)        (void *buffer, uint_fast32_t size);
    void (*Free)            (void *buffer);
end_def_interface(i_heap_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_heap_t VSF_HEAP;

/*============================ PROTOTYPES ====================================*/

extern void vsf_heap_init(void);

/*!\note: vsf_heap_add and vsf_heap_add_memory cannot add misaligned memory spaces.
 *!\when a user needs to add memory space, the space provided each time must be aligned
 *!\according to the alignment defined by VSF_HEAP_CFG_MCB_ALIGN_BIT(4 if undefined).
*/
extern void vsf_heap_add(uint8_t *heap, uint_fast32_t size);
extern void vsf_heap_add_memory(vsf_mem_t mem);
extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_heap_malloc(uint_fast32_t size);
extern void * vsf_heap_realloc_aligned(void *buffer, uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_heap_realloc(void *buffer, uint_fast32_t size);
extern void vsf_heap_free(void *buffer);

#endif

#ifdef __cplusplus
}
#endif

#endif
