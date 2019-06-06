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

/*============================ MACROS ========================================*/
#ifndef VSF_USE_HEAP
#   define VSF_USE_HEAP     ENABLED     //!< enable vsf_heap_t by default
#endif

#if VSF_USE_HEAP == ENABLED
#if 0
/*! \brief free a target memory which belongs to a bigger memory chunk previouly 
 *!        allocated from the heap
 */
#define vsf_heap_free_ex(                                                       \
        __ADDR,     /* the adress of target memory */                           \
        __OFFSET,   /* the offset of the target mem from the original memory */ \
        __SIZE)     /* the size of the target mem */                            \
        vsf_heap_partial_free((uint8_t *)(__ADDR) - (__OFFSET),                 \
                                (__OFFSET),                                     \
                                (__SIZE))
#endif
/*============================ TYPES =========================================*/

//! \name vsf heap interface
//! @{
def_interface(i_heap_t)
    void (*Init)            (void);
    void (*Add)             (uint8_t *heap, uint_fast32_t size);
    void (*AddMemory)       (vsf_mem_t mem);
    void *(*MallocAligned)  (uint_fast32_t size, uint_fast32_t alignment);
    void *(*Malloc)         (uint_fast32_t size);
    void (*Free)            (void *buffer);
end_def_interface(i_heap_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_heap_t VSF_HEAP;

/*============================ PROTOTYPES ====================================*/

extern void vsf_heap_init(void);
extern void vsf_heap_add(uint8_t *heap, uint_fast32_t size);
extern void vsf_heap_add_memory(vsf_mem_t mem);
extern void *vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
extern void *vsf_heap_malloc(uint_fast32_t size);
extern void vsf_heap_free(void *buffer);

/*! \brief partially free a target memory
 *! \param buffer the address of the target memory chunk
 *! \param pos the start position of the desired to be partially freed part
 *! \param size the size of the partially freed part.
 *! \retval true the free access is sucessful
 *! \retval false the free access failed.
 */
extern bool vsf_heap_partial_free(  void *buffer,       
                                    uint_fast32_t pos, 
                                    uint_fast32_t size);

#endif
#endif
