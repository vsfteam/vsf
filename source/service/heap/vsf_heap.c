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

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"
#include "vsf_heap.h"
#include "utilities/vsf_utilities.h"
#include "hal/arch/vsf_arch.h"

// rule breaker, but need kernel configurations to determine VSF_HEAP_CFG_PROTECT_LEVEL
#include "kernel/vsf_kernel.h"

#if VSF_USE_HEAP == ENABLED

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
        &&  (   !defined(VSF_OS_CFG_EVTQ_POOL_SIZE)                             \
            ||  VSF_OS_CFG_EVTQ_POOL_SIZE <= 0)
#       warning ******** evtq_list is used without VSF_OS_CFG_EVTQ_POOL_SIZE configured, so heap protection level is set to interrupt ********
#       define VSF_HEAP_CFG_PROTECT_LEVEL   interrupt
#   else
#       define VSF_HEAP_CFG_PROTECT_LEVEL   scheduler
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_heap_protect                  vsf_protect(VSF_HEAP_CFG_PROTECT_LEVEL)
#define __vsf_heap_unprotect                vsf_unprotect(VSF_HEAP_CFG_PROTECT_LEVEL)

#define __vsf_mcb_get_size(__mcb)           ((__mcb)->linear.next << VSF_HEAP_CFG_MCB_ALIGN_BIT)
#define __vsf_mcb_get_prev_size(__mcb)      ((__mcb)->linear.prev << VSF_HEAP_CFG_MCB_ALIGN_BIT)

/*============================ TYPES =========================================*/

typedef struct vsf_heap_mcb_t {
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    uint32_t magic;
#endif
    struct {
        uint16_t next;
        uint16_t prev;
    } linear;

    vsf_dlist_node_t list;
} vsf_heap_mcb_t;

typedef struct vsf_heap_t {
    // one more as terminator
    vsf_dlist_t freelist[VSF_HEAP_CFG_FREELIST_NUM + 1];
} vsf_heap_t;

/*============================ GLOBAL VARIABLES ==============================*/

const i_heap_t VSF_HEAP = {
    .Init           = &vsf_heap_init,
    .Add            = &vsf_heap_add,
    .MallocAligned  = &vsf_heap_malloc_aligned,
    .Malloc         = &vsf_heap_malloc,
    .ReallocAligned = &vsf_heap_realloc_aligned,
    .Realloc        = &vsf_heap_realloc,
    .Free           = &vsf_heap_free
};

/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_heap_t __vsf_heap;

/*============================ PROTOTYPES ====================================*/

extern vsf_dlist_t * vsf_heap_get_freelist(vsf_dlist_t *freelist, uint_fast8_t freelist_num, uint_fast32_t size);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_HEAP_GET_FREELIST
WEAK(vsf_heap_get_freelist)
vsf_dlist_t * vsf_heap_get_freelist(vsf_dlist_t *freelist, uint_fast8_t freelist_num, uint_fast32_t size)
{
    UNUSED_PARAM(freelist);
    UNUSED_PARAM(freelist_num);
    UNUSED_PARAM(size);
    return freelist;
}
#endif

// MUST NOT return NULL;
static vsf_dlist_t * __vsf_heap_get_freelist(uint_fast32_t size)
{
    return vsf_heap_get_freelist(&__vsf_heap.freelist[0], VSF_HEAP_CFG_FREELIST_NUM, size);
}

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
    vsf_heap_mcb_t *mcb_next = (vsf_heap_mcb_t *)((uint8_t *)mcb + __vsf_mcb_get_size(mcb));
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
    vsf_heap_mcb_t *mcb_prev = (vsf_heap_mcb_t *)((uint8_t *)mcb - __vsf_mcb_get_prev_size(mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb_prev->magic == VSF_HEAP_MCB_MAGIC);
#endif
    return mcb_prev;
}

static bool __vsf_heap_mcb_is_allocated(vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

    return !vsf_dlist_is_in(vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_remove_from_freelist(vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

    vsf_dlist_remove(vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_add_to_freelist(vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

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

static void __vsf_heap_mcb_truncate(vsf_heap_mcb_t *mcb, uintptr_t buf_end)
{
    uintptr_t margin_size = (uintptr_t)mcb + __vsf_mcb_get_size(mcb);

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

        __vsf_heap_mcb_add_to_freelist(mcb_next);
    }
}

static void * __vsf_heap_mcb_malloc(vsf_heap_mcb_t *mcb, uint_fast32_t size,
            uint_fast32_t alignment)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    uint_fast32_t buffer_size = __vsf_mcb_get_size(mcb);
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

        __vsf_heap_mcb_remove_from_freelist(mcb);
        mcb_new = __vsf_heap_get_mcb(buffer);
        margin_size = (uint8_t *)mcb_new - (uint8_t *)mcb;

        if (0 == margin_size) {
            temp_size = mcb->linear.prev;
        } else if (margin_size <= sizeof(vsf_heap_mcb_t)) {
            __vsf_heap_mcb_add_to_freelist(mcb);
            unaligned_size = alignment;
            goto fix_alignment;
        } else {
            // split mcb
            temp_size = mcb->linear.next = margin_size >> VSF_HEAP_CFG_MCB_ALIGN_BIT;
            __vsf_heap_mcb_add_to_freelist(mcb);
        }

        __vsf_heap_mcb_init(mcb_new);
        mcb_new->linear.prev = temp_size;
        mcb_new->linear.next =
            ((buffer - (uint8_t *)mcb_new) + buffer_size) >> VSF_HEAP_CFG_MCB_ALIGN_BIT;
        mcb = __vsf_heap_mcb_get_next(mcb_new);
        mcb->linear.prev = mcb_new->linear.next;

        __vsf_heap_mcb_truncate(mcb_new, (uintptr_t)buffer + size);
        return buffer;
    }

    return NULL;
}

static void __vsf_heap_mcb_free(vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    vsf_heap_mcb_t *mcb_tmp;
    vsf_protect_t state;

    if (mcb->linear.next != 0) {
        state = __vsf_heap_protect();
            mcb_tmp = __vsf_heap_mcb_get_next(mcb);
            if (!__vsf_heap_mcb_is_allocated(mcb_tmp)) {
                __vsf_heap_mcb_remove_from_freelist(mcb_tmp);
                mcb->linear.next += mcb_tmp->linear.next;
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                mcb_tmp->magic = 0;
#endif

                mcb_tmp = __vsf_heap_mcb_get_next(mcb);
                mcb_tmp->linear.prev = mcb->linear.next;
            }
        __vsf_heap_unprotect(state);
    }

    if (mcb->linear.prev != 0) {
        state = __vsf_heap_protect();
            mcb_tmp = __vsf_heap_mcb_get_prev(mcb);
            if (!__vsf_heap_mcb_is_allocated(mcb_tmp)) {
                __vsf_heap_mcb_remove_from_freelist(mcb_tmp);
                mcb_tmp->linear.next += mcb->linear.next;
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                mcb->magic = 0;
#endif
                mcb = mcb_tmp;

                mcb_tmp = __vsf_heap_mcb_get_next(mcb);
                mcb_tmp->linear.prev = mcb->linear.next;
            }
        __vsf_heap_unprotect(state);
    }

    state = __vsf_heap_protect();
        __vsf_heap_mcb_add_to_freelist(mcb);
    __vsf_heap_unprotect(state);
}

static void * __vsf_heap_freelist_malloc(vsf_dlist_t *plist, uint_fast32_t size,
            uint_fast32_t alignment)
{
    vsf_heap_mcb_t *mcb;
    void *buffer = NULL;

    vsf_protect_t state = __vsf_heap_protect();
        vsf_dlist_peek_head(vsf_heap_mcb_t, list, plist, mcb);
        while ((NULL == buffer) && (mcb != NULL)) {
            buffer = __vsf_heap_mcb_malloc(mcb, size, alignment);
            if (NULL == buffer) {
                vsf_dlist_peek_next(vsf_heap_mcb_t, list, mcb, mcb);
            }
        }
    __vsf_heap_unprotect(state);
    return buffer;
}

void vsf_heap_init(void)
{
    memset(&__vsf_heap, 0, sizeof(__vsf_heap));
}

void vsf_heap_add(uint8_t *heap, uint_fast32_t size)
{
    uint_fast32_t    unaligned_size;
    uint_fast32_t    offset;
    vsf_heap_mcb_t *mcb;
    vsf_protect_t    state;

    VSF_SERVICE_ASSERT(NULL != heap);

    unaligned_size = __vsf_heap_calc_unaligned_size(heap, VSF_HEAP_CFG_MCB_ALIGN);

    state = __vsf_heap_protect();

    heap += unaligned_size;
    size = (size - unaligned_size) & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
    offset = (size - sizeof(vsf_heap_mcb_t))  & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);

    VSF_SERVICE_ASSERT(     !(size >> (VSF_HEAP_CFG_MCB_ALIGN_BIT + 16))
                        &&  (size > 2 * sizeof(vsf_heap_mcb_t)));

    mcb = (vsf_heap_mcb_t *)((uint8_t *)heap + offset);
    __vsf_heap_mcb_init(mcb);
    offset >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    mcb->linear.prev = offset;

    mcb = (vsf_heap_mcb_t *)heap;
    __vsf_heap_mcb_init(mcb);
    mcb->linear.next = offset;

    offset <<= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    vsf_dlist_add_to_tail(  vsf_heap_mcb_t, list,
                            __vsf_heap_get_freelist(offset),
                            mcb);

    __vsf_heap_unprotect(state);
}

void vsf_heap_add_memory(vsf_mem_t mem)
{
    vsf_heap_add(mem.buffer, (uint_fast32_t)mem.size);
}

void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size + sizeof(vsf_heap_mcb_t));
    void *buffer;

    if (alignment) {
        VSF_SERVICE_ASSERT(!(alignment & (alignment - 1)));
    }

    size = (size + 3) & ~(uint_fast32_t)3;
    while (!vsf_dlist_is_empty(freelist)) {
        buffer = __vsf_heap_freelist_malloc(freelist, size, alignment);
        if (buffer != NULL) {
            return buffer;
        }
        freelist++;
    }
    return NULL;
}

void * vsf_heap_malloc(uint_fast32_t size)
{
    return vsf_heap_malloc_aligned(size, 0);
}

void * vsf_heap_realloc_aligned(void *buffer, uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_heap_mcb_t *mcb;
    uint_fast32_t memory_size;

    VSF_SERVICE_ASSERT((buffer != NULL) && (alignment >= 4) && !(alignment & (alignment - 1)));
    VSF_SERVICE_ASSERT(!((uintptr_t)buffer & (alignment - 1)));

    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    VSF_SERVICE_ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif

    memory_size = (uint_fast32_t)__vsf_mcb_get_size(mcb);
    memory_size -= (uint8_t *)buffer - (uint8_t *)mcb;

    if (memory_size >= size) {
        vsf_protect_t state = __vsf_heap_protect();
        __vsf_heap_mcb_truncate(mcb, (uintptr_t)buffer + size);
        __vsf_heap_unprotect(state);
        return buffer;
    } else {
        vsf_heap_mcb_t *mcb_next = __vsf_heap_mcb_get_next(mcb);
        vsf_protect_t state = __vsf_heap_protect();

        uint_fast32_t next_size = __vsf_mcb_get_size(mcb_next);
        if (__vsf_heap_mcb_is_allocated(mcb_next) || ((memory_size + next_size) < size)) {
            __vsf_heap_unprotect(state);

            void *new_buffer = vsf_heap_malloc_aligned(size, alignment);
            if (new_buffer != NULL) {
                memcpy(new_buffer, buffer, memory_size);
            }
            __vsf_heap_mcb_free(mcb);
            return new_buffer;
        } else {
            __vsf_heap_mcb_remove_from_freelist(mcb_next);

            mcb->linear.next += mcb_next->linear.next;
            mcb_next = __vsf_heap_mcb_get_next(mcb_next);
            mcb_next->linear.prev = mcb->linear.next;

            __vsf_heap_mcb_truncate(mcb, (uintptr_t)buffer + size);
            __vsf_heap_unprotect(state);
            return buffer;
        }
    }
}

//Adjust the allocated memory size(aligned to sizeof(uintalu_t))
void * vsf_heap_realloc(void *buffer, uint_fast32_t size)
{
    return vsf_heap_realloc_aligned(buffer, size, sizeof(uintalu_t));
}

void vsf_heap_free(void *buffer)
{
    vsf_heap_mcb_t *mcb;

    VSF_SERVICE_ASSERT(buffer != NULL);
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    VSF_SERVICE_ASSERT( __vsf_heap_mcb_is_allocated(mcb)
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
        &&  mcb->magic == VSF_HEAP_MCB_MAGIC
#endif
    );
    __vsf_heap_mcb_free(mcb);
}

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif