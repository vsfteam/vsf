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

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#if VSF_USE_HEAP == ENABLED

#define __VSF_HEAP_CLASS_IMPLEMENT
#include "vsf_heap.h"
#include "utilities/vsf_utilities.h"

// rule breaker, but need kernel configurations to determine VSF_HEAP_CFG_PROTECT_LEVEL
#include "kernel/vsf_kernel.h"

// need pool configurations to determine VSF_HEAP_CFG_PROTECT_LEVEL
#include "../pool/vsf_pool.h"

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HEAP_CFG_MCB_ALIGN_BIT
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT       2
#endif
#define VSF_HEAP_CFG_MCB_ALIGN              (1 << VSF_HEAP_CFG_MCB_ALIGN_BIT)
#ifndef VSF_HEAP_CFG_MCB_OFFSET_TYPE
#   define VSF_HEAP_CFG_MCB_OFFSET_TYPE     uint16_t
#endif

#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
#   define VSF_HEAP_MCB_MAGIC               0x1ea01ea0
#endif

#ifndef VSF_HEAP_CFG_FREELIST_NUM
#   define VSF_HEAP_CFG_FREELIST_NUM        1
#endif


// heap protection uses scheduler protection by default
#ifndef VSF_HEAP_CFG_PROTECT_LEVEL
#   if      VSF_USE_KERNEL == ENABLED                                           \
        &&  defined(__VSF_OS_CFG_EVTQ_LIST)                                     \
        &&  VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
#       ifndef __VSF_I_KNOW_POOL_CFG_FEED_ON_HEAP_WILL_INCREASE_INTERRUPT_LATENCY__
#           warning ******** evtq_list is used with VSF_POOL_CFG_FEED_ON_HEAP enabled,\
so set heap protection level to interrupt, and interrupt lantancy will be increased ********
#       endif
#       define VSF_HEAP_CFG_PROTECT_LEVEL   interrupt
#   else
#       define VSF_HEAP_CFG_PROTECT_LEVEL   scheduler
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_heap_protect                  vsf_protect(VSF_HEAP_CFG_PROTECT_LEVEL)
#define __vsf_heap_unprotect                vsf_unprotect(VSF_HEAP_CFG_PROTECT_LEVEL)

#define __vsf_heap_mcb_get_size(__mcb)      ((__mcb)->linear.next << VSF_HEAP_CFG_MCB_ALIGN_BIT)
#define __vsf_heap_mcb_get_prev_size(__mcb) ((__mcb)->linear.prev << VSF_HEAP_CFG_MCB_ALIGN_BIT)

/*============================ TYPES =========================================*/

typedef struct vsf_heap_mcb_t {
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    uint32_t magic;
#endif
    struct {
        VSF_HEAP_CFG_MCB_OFFSET_TYPE next;
        VSF_HEAP_CFG_MCB_OFFSET_TYPE prev;
    } linear;

    vsf_dlist_node_t list;
} vsf_heap_mcb_t;

#ifndef VSF_ARCH_PROVIDE_HEAP
typedef struct vsf_default_heap_t {
    implement(vsf_heap_t)
    // one more as terminator
    vsf_dlist_t freelist[VSF_HEAP_CFG_FREELIST_NUM + 1];
} vsf_default_heap_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const i_heap_t VSF_HEAP = {
#ifndef VSF_ARCH_PROVIDE_HEAP
    .Init           = &vsf_heap_init,
    .AddBuffer      = &vsf_heap_add_buffer,
    .AddMemory      = &vsf_heap_add_memory,
#endif
    .MallocAligned  = &vsf_heap_malloc_aligned_imp,
    .Malloc         = &vsf_heap_malloc_imp,
    .ReallocAligned = &vsf_heap_realloc_aligned_imp,
    .Realloc        = &vsf_heap_realloc_imp,
    .Free           = &vsf_heap_free_imp,
#if !defined(VSF_ARCH_PROVIDE_HEAP) && VSF_HEAP_CFG_STATISTICS == ENABLED
    .Statistics     = &vsf_heap_statistics,
#endif
};

/*============================ LOCAL VARIABLES ===============================*/

#ifndef VSF_ARCH_PROVIDE_HEAP
static NO_INIT vsf_default_heap_t __vsf_heap;
#endif

/*============================ PROTOTYPES ====================================*/

extern vsf_dlist_t * vsf_heap_get_freelist(vsf_dlist_t *freelist, uint_fast8_t freelist_num, uint_fast32_t size);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_heap_mcb_init(vsf_heap_mcb_t *mcb)
{
    memset(mcb, 0, sizeof(*mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    mcb->magic = VSF_HEAP_MCB_MAGIC;
#endif
}

static uint_fast32_t __vsf_heap_calc_unaligned_size(void *buffer, uint_fast32_t alignment)
{
    uint_fast32_t unaligned_size = (uintptr_t)buffer & (alignment - 1);
    if (unaligned_size != 0) {
        unaligned_size = alignment - unaligned_size;
    }
    return unaligned_size;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe177
#endif
// __vsf_heap_mcb_set_next is reserved for future use
static void __vsf_heap_mcb_set_next(vsf_heap_mcb_t *mcb, vsf_heap_mcb_t *mcb_next)
{
    uint_fast32_t margin_size = (uint8_t *)mcb_next - (uint8_t *)mcb;
    margin_size >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    mcb->linear.next = mcb_next->linear.prev = margin_size;
}

static vsf_heap_mcb_t * __vsf_heap_mcb_get_next(vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    vsf_heap_mcb_t *mcb_next = (vsf_heap_mcb_t *)((uint8_t *)mcb + __vsf_heap_mcb_get_size(mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb_next->magic == VSF_HEAP_MCB_MAGIC);
#endif
    return mcb_next;
}

static vsf_heap_mcb_t * __vsf_heap_mcb_get_prev(vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    vsf_heap_mcb_t *mcb_prev = (vsf_heap_mcb_t *)((uint8_t *)mcb - __vsf_heap_mcb_get_prev_size(mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb_prev->magic == VSF_HEAP_MCB_MAGIC);
#endif
    return mcb_prev;
}

static bool __vsf_heap_mcb_is_allocated(vsf_heap_t *heap, vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_heap_mcb_get_size(mcb);
    vsf_dlist_t *freelist = heap->get_freelist(size);

    return !vsf_dlist_is_in(vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_remove_from_freelist(vsf_heap_t *heap, vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_heap_mcb_get_size(mcb);
    vsf_dlist_t *freelist = heap->get_freelist(size);

    vsf_dlist_remove(vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_add_to_freelist(vsf_heap_t *heap, vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_heap_mcb_get_size(mcb);
    vsf_dlist_t *freelist = heap->get_freelist(size);

    vsf_dlist_add_to_head(vsf_heap_mcb_t, list, freelist, mcb);
}

static vsf_heap_mcb_t * __vsf_heap_get_mcb(uint8_t *buffer)
{
    uintptr_t addr = (uintptr_t)buffer - sizeof(vsf_heap_mcb_t);
    uint_fast32_t unaligned_size = __vsf_heap_calc_unaligned_size((void *)addr, VSF_HEAP_CFG_MCB_ALIGN);
    if (unaligned_size) {
        addr -= VSF_HEAP_CFG_MCB_ALIGN - unaligned_size;
    }
    return (vsf_heap_mcb_t *)addr;
}

static void __vsf_heap_mcb_truncate(vsf_heap_t *heap, vsf_heap_mcb_t *mcb, uintptr_t buf_end)
{
    uintptr_t margin_size = (uintptr_t)mcb + __vsf_heap_mcb_get_size(mcb);

    buf_end += VSF_HEAP_CFG_MCB_ALIGN - 1;
    buf_end &= ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
    margin_size = margin_size - buf_end;
    if(margin_size > sizeof(vsf_heap_mcb_t)) {
        vsf_heap_mcb_t *mcb_next = (vsf_heap_mcb_t *)buf_end;
        __vsf_heap_mcb_init(mcb_next);

        margin_size = (uint8_t *)mcb_next - (uint8_t *)mcb;
        margin_size >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
        mcb_next->linear.next = mcb->linear.next - margin_size;
        mcb_next->linear.prev = mcb->linear.next = margin_size;

        margin_size = mcb_next->linear.next;
        mcb = __vsf_heap_mcb_get_next(mcb_next);
        mcb->linear.prev = margin_size;

        __vsf_heap_mcb_add_to_freelist(heap, mcb_next);
    }
}

static void * __vsf_heap_mcb_malloc(vsf_heap_t *heap, vsf_heap_mcb_t *mcb,
            uint_fast32_t size, uint_fast32_t alignment)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    uint_fast32_t buffer_size = __vsf_heap_mcb_get_size(mcb);
    uint8_t *buffer = (uint8_t *)&mcb[1];
    uint_fast32_t unaligned_size;

    buffer_size -= sizeof(vsf_heap_mcb_t);
    if (alignment) {
        unaligned_size = __vsf_heap_calc_unaligned_size(buffer, alignment);
        if (unaligned_size) {
        fix_alignment:
            if (buffer_size < unaligned_size) {
                return NULL;
            }
            buffer += unaligned_size;
            buffer_size -= unaligned_size;
        }
    }

    if (buffer_size >= size) {
        vsf_heap_mcb_t *mcb_new;
        uint_fast32_t margin_size, temp_size;

        // IMPORTANT: DO NOT remove mcb and then add mcb if not suitable
        //  this may cause dead loop
        mcb_new = __vsf_heap_get_mcb(buffer);
        margin_size = (uint8_t *)mcb_new - (uint8_t *)mcb;

        if (0 == margin_size) {
            __vsf_heap_mcb_remove_from_freelist(heap, mcb);
            temp_size = mcb->linear.prev;
        } else if (margin_size <= sizeof(vsf_heap_mcb_t)) {
            unaligned_size = alignment;
            goto fix_alignment;
        } else {
            // split mcb
            temp_size = mcb->linear.next = margin_size >> VSF_HEAP_CFG_MCB_ALIGN_BIT;
        }

        __vsf_heap_mcb_init(mcb_new);
        mcb_new->linear.prev = temp_size;
        mcb_new->linear.next =
            ((buffer - (uint8_t *)mcb_new) + buffer_size) >> VSF_HEAP_CFG_MCB_ALIGN_BIT;
        mcb = __vsf_heap_mcb_get_next(mcb_new);
        mcb->linear.prev = mcb_new->linear.next;

        __vsf_heap_mcb_truncate(heap, mcb_new, (uintptr_t)buffer + size);
#if VSF_HEAP_CFG_STATISTICS == ENABLED
        heap->statistics.used_size += __vsf_heap_mcb_get_size(mcb_new);
#endif
        return buffer;
    }

    return NULL;
}

static void __vsf_heap_mcb_free(vsf_heap_t *heap, vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    vsf_heap_mcb_t *mcb_tmp;
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    uint_fast32_t mcb_size = __vsf_heap_mcb_get_size(mcb);
#endif
    vsf_protect_t state = __vsf_heap_protect();

    if (mcb->linear.next != 0) {
        mcb_tmp = __vsf_heap_mcb_get_next(mcb);
        if (!__vsf_heap_mcb_is_allocated(heap, mcb_tmp)) {
            __vsf_heap_mcb_remove_from_freelist(heap, mcb_tmp);
            mcb->linear.next += mcb_tmp->linear.next;
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
            mcb_tmp->magic = 0;
#endif

            mcb_tmp = __vsf_heap_mcb_get_next(mcb);
            mcb_tmp->linear.prev = mcb->linear.next;
        }
    }

    if (mcb->linear.prev != 0) {
        mcb_tmp = __vsf_heap_mcb_get_prev(mcb);
        if (!__vsf_heap_mcb_is_allocated(heap, mcb_tmp)) {
            __vsf_heap_mcb_remove_from_freelist(heap, mcb_tmp);
            mcb_tmp->linear.next += mcb->linear.next;
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
            mcb->magic = 0;
#endif
            mcb = mcb_tmp;

            mcb_tmp = __vsf_heap_mcb_get_next(mcb);
            mcb_tmp->linear.prev = mcb->linear.next;
        }
    }

    __vsf_heap_mcb_add_to_freelist(heap, mcb);
#if VSF_HEAP_CFG_STATISTICS == ENABLED
    heap->statistics.used_size -= mcb_size;
#endif
    __vsf_heap_unprotect(state);
}

static void * __vsf_heap_freelist_malloc(vsf_heap_t *heap, vsf_dlist_t *plist,
            uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_heap_mcb_t *mcb;
    void *buffer = NULL;

    vsf_protect_t state = __vsf_heap_protect();
        vsf_dlist_peek_head(vsf_heap_mcb_t, list, plist, mcb);
        while ((NULL == buffer) && (mcb != NULL)) {
            buffer = __vsf_heap_mcb_malloc(heap, mcb, size, alignment);
            if (NULL == buffer) {
                vsf_dlist_peek_next(vsf_heap_mcb_t, list, mcb, mcb);
            }
        }
    __vsf_heap_unprotect(state);
    return buffer;
}

void __vsf_heap_add_buffer(vsf_heap_t *heap, uint8_t *buffer, uint_fast32_t size)
{
    uint_fast32_t unaligned_size;
    uint_fast32_t offset;
    vsf_heap_mcb_t *mcb;
    vsf_protect_t state;

    VSF_SERVICE_ASSERT((NULL != heap) && (NULL != buffer) && (size > 0));

    unaligned_size = __vsf_heap_calc_unaligned_size(buffer, VSF_HEAP_CFG_MCB_ALIGN);

    state = __vsf_heap_protect();

    buffer += unaligned_size;
    size = (size - unaligned_size) & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
    offset = (size - sizeof(vsf_heap_mcb_t))  & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);

    VSF_SERVICE_ASSERT  (   (size > 2 * sizeof(vsf_heap_mcb_t))
                        &&  (   ((VSF_HEAP_CFG_MCB_ALIGN_BIT + sizeof(VSF_HEAP_CFG_MCB_OFFSET_TYPE) * 8) >= 32)
                            ||  !(size >> (VSF_HEAP_CFG_MCB_ALIGN_BIT + sizeof(VSF_HEAP_CFG_MCB_OFFSET_TYPE) * 8))
                            )
                        );

    mcb = (vsf_heap_mcb_t *)((uint8_t *)buffer + offset);
    __vsf_heap_mcb_init(mcb);
    offset >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    mcb->linear.prev = offset;

    mcb = (vsf_heap_mcb_t *)buffer;
    __vsf_heap_mcb_init(mcb);
    mcb->linear.next = offset;

    offset <<= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    vsf_dlist_add_to_tail(  vsf_heap_mcb_t, list,
                            heap->get_freelist(offset),
                            mcb);

#if VSF_HEAP_CFG_STATISTICS == ENABLED
    heap->statistics.all_size += size;
#endif
    __vsf_heap_unprotect(state);
}

void * __vsf_heap_malloc_aligned(vsf_heap_t *heap, uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_dlist_t *freelist = heap->get_freelist(size + sizeof(vsf_heap_mcb_t));
    void *buffer;

    if (alignment) {
        VSF_SERVICE_ASSERT(!(alignment & (alignment - 1)));
    }

    size = (size + 3) & ~(uint_fast32_t)3;
    while (!vsf_dlist_is_empty(freelist)) {
        buffer = __vsf_heap_freelist_malloc(heap, freelist, size, alignment);
        if (buffer != NULL) {
            return buffer;
        }
        freelist++;
    }
    vsf_trace_error("fail to allocate %d bytes with %d alignment" VSF_TRACE_CFG_LINEEND, size, alignment);
    return NULL;
}

void * __vsf_heap_realloc_aligned(vsf_heap_t *heap, void *buffer, uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_heap_mcb_t *mcb;
    uint_fast32_t memory_size, mcb_size;

    VSF_SERVICE_ASSERT((buffer != NULL) && (alignment >= 4) && !(alignment & (alignment - 1)));
    VSF_SERVICE_ASSERT(!((uintptr_t)buffer & (alignment - 1)));

    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif

    mcb_size = (uint_fast32_t)__vsf_heap_mcb_get_size(mcb);
    memory_size = mcb_size - ((uint8_t *)buffer - (uint8_t *)mcb);

    if (memory_size >= size) {
        vsf_protect_t state = __vsf_heap_protect();
        __vsf_heap_mcb_truncate(heap, mcb, (uintptr_t)buffer + size);
#if VSF_HEAP_CFG_STATISTICS == ENABLED
        heap->statistics.used_size -= mcb_size;
        heap->statistics.used_size += __vsf_heap_mcb_get_size(mcb);
#endif
        __vsf_heap_unprotect(state);
        return buffer;
    } else {
        vsf_heap_mcb_t *mcb_next = __vsf_heap_mcb_get_next(mcb);
        vsf_protect_t state = __vsf_heap_protect();

        uint_fast32_t next_size = __vsf_heap_mcb_get_size(mcb_next);
        if (__vsf_heap_mcb_is_allocated(heap, mcb_next) || ((memory_size + next_size) < size)) {
            __vsf_heap_unprotect(state);

            void *new_buffer = __vsf_heap_malloc_aligned(heap, size, alignment);
            if (new_buffer != NULL) {
                memcpy(new_buffer, buffer, vsf_min(memory_size, size));
            }
            __vsf_heap_mcb_free(heap, mcb);
            return new_buffer;
        } else {
            __vsf_heap_mcb_remove_from_freelist(heap, mcb_next);

            mcb->linear.next += mcb_next->linear.next;
            mcb_next = __vsf_heap_mcb_get_next(mcb_next);
            mcb_next->linear.prev = mcb->linear.next;

            __vsf_heap_mcb_truncate(heap, mcb, (uintptr_t)buffer + size);
#if VSF_HEAP_CFG_STATISTICS == ENABLED
        heap->statistics.used_size -= mcb_size;
        heap->statistics.used_size += __vsf_heap_mcb_get_size(mcb);
#endif
            __vsf_heap_unprotect(state);
            return buffer;
        }
    }
}

uint_fast32_t __vsf_heap_size(vsf_heap_t *heap, void *buffer)
{
    vsf_heap_mcb_t *mcb;

    VSF_SERVICE_ASSERT(buffer != NULL);
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    VSF_SERVICE_ASSERT( __vsf_heap_mcb_is_allocated(heap, mcb)
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
        &&  mcb->magic == VSF_HEAP_MCB_MAGIC
#endif
    );
    return __vsf_heap_mcb_get_size(mcb) - ((uint8_t *)buffer - (uint8_t *)mcb);
}

void __vsf_heap_free(vsf_heap_t *heap, void *buffer)
{
    vsf_heap_mcb_t *mcb;

    VSF_SERVICE_ASSERT(buffer != NULL);
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    VSF_SERVICE_ASSERT( __vsf_heap_mcb_is_allocated(heap, mcb)
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
        &&  mcb->magic == VSF_HEAP_MCB_MAGIC
#endif
    );
    __vsf_heap_mcb_free(heap, mcb);
}

#if VSF_HEAP_CFG_STATISTICS == ENABLED
void __vsf_heap_statistics(vsf_heap_t *heap, vsf_heap_statistics_t *statistics)
{
    if (statistics != NULL) {
        vsf_protect_t state = __vsf_heap_protect();
            *statistics = heap->statistics;
        __vsf_heap_unprotect(state);
    }
}
#endif

// default heap
#ifndef WEAK_VSF_HEAP_GET_FREELIST
// default freelist, single free list
WEAK(vsf_heap_get_freelist)
vsf_dlist_t * vsf_heap_get_freelist(vsf_dlist_t *freelist, uint_fast8_t freelist_num, uint_fast32_t size)
{
    VSF_UNUSED_PARAM(freelist);
    VSF_UNUSED_PARAM(freelist_num);
    VSF_UNUSED_PARAM(size);
    return freelist;
}
#endif

#ifndef VSF_ARCH_PROVIDE_HEAP
// MUST NOT return NULL;
static vsf_dlist_t * __vsf_heap_get_freelist(uint_fast32_t size)
{
    return vsf_heap_get_freelist(&__vsf_heap.freelist[0], VSF_HEAP_CFG_FREELIST_NUM, size);
}

void vsf_heap_init(void)
{
    memset(&__vsf_heap, 0, sizeof(__vsf_heap));
    for (uint_fast8_t i = 0; i < dimof(__vsf_heap.freelist); i++) {
        vsf_dlist_init(&__vsf_heap.freelist[i]);
    }
    __vsf_heap.get_freelist = __vsf_heap_get_freelist;
}

void vsf_heap_add_buffer(uint8_t *buffer, uint_fast32_t size)
{
    __vsf_heap_add_buffer(&__vsf_heap.use_as__vsf_heap_t, buffer, size);
}

void vsf_heap_add_memory(vsf_mem_t mem)
{
    vsf_heap_add_buffer(mem.buffer, (uint_fast32_t)mem.size);
}

void vsf_heap_statistics(vsf_heap_statistics_t *statistics)
{
    __vsf_heap_statistics(&__vsf_heap.use_as__vsf_heap_t, statistics);
}
#endif

void * vsf_heap_malloc_aligned_imp(uint_fast32_t size, uint_fast32_t alignment)
{
#ifdef VSF_ARCH_PROVIDE_HEAP
    void *buffer;
    if (alignment > vsf_arch_heap_alignment()) {
        VSF_SERVICE_ASSERT(false);
        buffer = NULL;
    } else {
        buffer = vsf_arch_heap_malloc(size);
    }
    if (NULL == buffer) {
        vsf_trace_error("fail to allocate %d bytes with %d alignment" VSF_TRACE_CFG_LINEEND, size, alignment);
    }
    return buffer;
#else
    return __vsf_heap_malloc_aligned(&__vsf_heap.use_as__vsf_heap_t, size, alignment);
#endif
}

void * vsf_heap_malloc_imp(uint_fast32_t size)
{
#ifdef VSF_ARCH_PROVIDE_HEAP
    void *buffer = vsf_arch_heap_malloc(size);
    if (NULL == buffer) {
        vsf_trace_error("fail to allocate %d bytes with %d alignment" VSF_TRACE_CFG_LINEEND, size, 0);
    }
    return buffer;
#else
    return vsf_heap_malloc_aligned(size, VSF_HEAP_ALIGN);
#endif
}

void * vsf_heap_realloc_aligned_imp(void *buffer, uint_fast32_t size, uint_fast32_t alignment)
{
    if (NULL == buffer) {
        if (size > 0) {
            return vsf_heap_malloc_aligned(size, alignment);
        }
        return NULL;
    } else if (0 == size) {
        if (buffer != NULL) {
            vsf_heap_free(buffer);
        }
        return NULL;
    }
#ifdef VSF_ARCH_PROVIDE_HEAP
    VSF_SERVICE_ASSERT(false);
    vsf_heap_free(buffer);
    return NULL;
#else
    return __vsf_heap_realloc_aligned(&__vsf_heap.use_as__vsf_heap_t, buffer, size, alignment);
#endif
}

//Adjust the allocated memory size(aligned to sizeof(uintalu_t))
void * vsf_heap_realloc_imp(void *buffer, uint_fast32_t size)
{
#ifdef VSF_ARCH_PROVIDE_HEAP
    if (NULL == buffer) {
        if (size > 0) {
            return vsf_heap_malloc(size);
        }
        return NULL;
    } else if (0 == size) {
        if (buffer != NULL) {
            vsf_heap_free(buffer);
        }
        return NULL;
    }

    void *new_buffer = vsf_arch_heap_realloc(buffer, size);
    if ((size > 0) && (NULL == new_buffer)) {
        vsf_trace_error("fail to reallocate %d bytes with %d alignment" VSF_TRACE_CFG_LINEEND, size, 0);
    }
    return new_buffer;
#else
    return vsf_heap_realloc_aligned(buffer, size, VSF_HEAP_ALIGN);
#endif
}

uint_fast32_t vsf_heap_size(uint8_t *buffer)
{
#ifdef VSF_ARCH_PROVIDE_HEAP
    return vsf_arch_heap_size(buffer);
#else
    return __vsf_heap_size(&__vsf_heap.use_as__vsf_heap_t, buffer);
#endif
}

void vsf_heap_free_imp(void *buffer)
{
    if (buffer != NULL) {
#ifdef VSF_ARCH_PROVIDE_HEAP
    return vsf_arch_heap_free(buffer);
#else
        __vsf_heap_free(&__vsf_heap.use_as__vsf_heap_t, buffer);
#endif
    }
}

void * vsf_heap_calloc(uint_fast32_t n, uint_fast32_t size)
{
    size *= n;
    void *buff = vsf_heap_malloc(size);
    if (buff != NULL) {
        memset(buff, 0, size);
    }
    return buff;
}

char * vsf_heap_strdup(const char *str)
{
    if (str != NULL) {
        int str_len = strlen(str);
        char *new_str = vsf_heap_malloc(str_len + 1);
        if (new_str != NULL) {
            memcpy(new_str, str, str_len);
            new_str[str_len] = '\0';
        }
        return new_str;
    }
    return NULL;
}

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif