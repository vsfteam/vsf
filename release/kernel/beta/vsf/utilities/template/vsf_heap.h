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
/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

//! \name vsf heap interface
//! @{
def_interface(i_heap_t)
    void (*Init)            (void);
    void (*Add)             (uint8_t *pheap, uint_fast32_t size);
    void *(*MallocAligned)  (uint_fast32_t size, uint_fast32_t alignment);
    void *(*Malloc)         (uint_fast32_t size);
    void (*Free)            (void *pbuffer);
end_def_interface(i_pool_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_heap_t VSF_HEAP;

/*============================ PROTOTYPES ====================================*/

extern void vsf_heap_init(void);
extern void vsf_heap_add(uint8_t *pheap, uint_fast32_t size);
extern void *vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment);
extern void *vsf_heap_malloc(uint_fast32_t size);
extern void vsf_heap_free(void *pbuffer);

#endif
