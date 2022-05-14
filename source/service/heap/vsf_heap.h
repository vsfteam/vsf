/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
#if VSF_USE_HEAP == ENABLED

#include "hal/arch/vsf_arch.h"
#include "utilities/vsf_utilities.h"

#if VSF_HEAP_CFG_TRACE == ENABLED
#   include "service/trace/vsf_trace.h"
#endif

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_HEAP_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#   undef __VSF_HEAP_CLASS_IMPLEMENT
#elif   defined(__VSF_HEAP_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#   undef __VSF_HEAP_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if !defined(VSF_HEAP_SIZE) && !defined(VSF_ARCH_PROVIDE_HEAP)
#   warning VSF_HEAP_SIZE is not defined, use 128K as default
#   define VSF_HEAP_SIZE    (128 * 1024)
#endif

#ifndef VSF_HEAP_ALIGN
#   define VSF_HEAP_ALIGN   (2 * sizeof(uintalu_t))
#endif

#if VSF_HEAP_CFG_TRACE == ENABLED
#   define vsf_heap_malloc_aligned(...)                                         \
    ({                                                                          \
        void * ptr = vsf_heap_malloc_aligned_imp(__VA_ARGS__);                  \
        vsf_trace_debug("%s: malloc_align 0x%p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ptr);\
        ptr;                                                                    \
    })
#   define vsf_heap_malloc(...)                                                 \
    ({                                                                          \
        void * ptr = vsf_heap_malloc_imp(__VA_ARGS__);                          \
        vsf_trace_debug("%s: malloc 0x%p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ptr);\
        ptr;                                                                    \
    })
#   define vsf_heap_realloc_aligned(...)                                        \
    ({                                                                          \
        void * ptr = vsf_heap_realloc_aligned_imp(__VA_ARGS__);                 \
        vsf_trace_debug("%s: realloc_align 0x%p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ptr);\
        ptr;                                                                    \
    })
#   define vsf_heap_realloc(...)                                                \
    ({                                                                          \
        void * ptr = vsf_heap_realloc_imp(__VA_ARGS__);                         \
        vsf_trace_debug("%s: realloc 0x%p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, ptr);\
        ptr;                                                                    \
    })
#   define vsf_heap_free(__ptr)                                                 \
    ({                                                                          \
        vsf_trace_debug("%s: free 0x%p" VSF_TRACE_CFG_LINEEND, __FUNCTION__, (__ptr));\
        vsf_heap_free_imp(__ptr);                                               \
    })
#else
#   define vsf_heap_malloc_aligned          vsf_heap_malloc_aligned_imp
#   define vsf_heap_malloc                  vsf_heap_malloc_imp
#   define vsf_heap_realloc_aligned         vsf_heap_realloc_aligned_imp
#   define vsf_heap_realloc                 vsf_heap_realloc_imp
#   define vsf_heap_free                    vsf_heap_free_imp
#endif

#ifndef VSF_HEAP_CFG_STATISTICS
#   define VSF_HEAP_CFG_STATISTICS          ENABLED
#endif

/*============================ TYPES =========================================*/

#if VSF_HEAP_CFG_STATISTICS == ENABLED
typedef struct vsf_heap_statistics_t {
    uint32_t all_size;
    uint32_t used_size;
} vsf_heap_statistics_t;
#endif

vsf_class(vsf_heap_t) {
    protected_member(
        vsf_dlist_t * (*get_freelist)(uint_fast32_t size);
    )
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    private_member(
        vsf_heap_statistics_t statistics;
    )
#endif
};

declare_interface(i_heap_t)

//! \name vsf heap interface
//! @{
def_interface(i_heap_t)
#ifndef VSF_ARCH_PROVIDE_HEAP
    void (*Init)            (void);
    void (*AddBuffer)       (uint8_t *buffer, uint_fast32_t size);
    void (*AddMemory)       (vsf_mem_t mem);
#endif
    void *(*MallocAligned)  (uint_fast32_t size, uint_fast32_t alignment);
    void *(*Malloc)         (uint_fast32_t size);
    void *(*ReallocAligned) (void *buffer, uint_fast32_t size, uint_fast32_t alignment);
    void *(*Realloc)        (void *buffer, uint_fast32_t size);
    void (*Free)            (void *buffer);
#if !defined(VSF_ARCH_PROVIDE_HEAP) && VSF_HEAP_CFG_STATISTICS == ENABLED
    void (*Statistics)      (vsf_heap_statistics_t *statistics);
#endif
end_def_interface(i_heap_t)
//! @}

/*============================ GLOBAL VARIABLES ==============================*/

extern const i_heap_t VSF_HEAP;

/*============================ PROTOTYPES ====================================*/

// heap class
extern void __vsf_heap_add_buffer(vsf_heap_t *heap, uint8_t *buffer, uint_fast32_t size);
extern void * __vsf_heap_malloc_aligned(vsf_heap_t *heap, uint_fast32_t size, uint_fast32_t alignment);
extern void * __vsf_heap_realloc_aligned(vsf_heap_t *heap, void *buffer, uint_fast32_t size, uint_fast32_t alignment);
extern void __vsf_heap_free(vsf_heap_t *heap, void *buffer);
extern uint_fast32_t __vsf_heap_size(vsf_heap_t *heap, void *buffer);
#if VSF_HEAP_CFG_STATISTICS == ENABLED
extern void __vsf_heap_statistics(vsf_heap_t *heap, vsf_heap_statistics_t *statistics);
#endif

#ifndef VSF_ARCH_PROVIDE_HEAP
// default heap
extern void vsf_heap_init(void);
/*!\note: vsf_heap_add_buffer and vsf_heap_add_memory cannot add misaligned memory spaces.
 *!\when a user needs to add memory space, the space provided each time must be aligned
 *!\according to the alignment defined by VSF_HEAP_CFG_MCB_ALIGN_BIT(2 if undefined).
*/
extern void vsf_heap_add_buffer(uint8_t *buffer, uint_fast32_t size);
extern void vsf_heap_add_memory(vsf_mem_t mem);
extern void vsf_heap_statistics(vsf_heap_statistics_t *statistics);
#endif
extern uint_fast32_t vsf_heap_size(uint8_t *buffer);
extern void * vsf_heap_malloc_aligned_imp(uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_heap_malloc_imp(uint_fast32_t size);
extern void * vsf_heap_realloc_aligned_imp(void *buffer, uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_heap_realloc_imp(void *buffer, uint_fast32_t size);
extern void vsf_heap_free_imp(void *buffer);

extern void * vsf_heap_calloc(uint_fast32_t n, uint_fast32_t size);
extern char * vsf_heap_strdup(const char *str);

#endif

#ifdef __cplusplus
}
#endif

#endif
