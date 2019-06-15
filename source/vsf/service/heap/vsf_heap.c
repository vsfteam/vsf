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

#if VSF_USE_HEAP == ENABLED
#if defined(VSF_HEAP_CFG_ATOM_ACCESS_DEPENDENCY)
#   include VSF_HEAP_CFG_ATOM_ACCESS_DEPENDENCY
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



#ifndef VSF_HEAP_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_HEAP_CFG_PROTECT_LEVEL  none
 *!         
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_HEAP_CFG_PROTECT_LEVEL  scheduler
 *!         
 *!         NOTE: This macro should be defined in app_cfg.h or vsf_cfg.h
 */
#   define VSF_HEAP_CFG_PROTECT_LEVEL           interrupt
#endif

#define vsf_heap_protect                    vsf_protect(VSF_HEAP_CFG_PROTECT_LEVEL)
#define vsf_heap_unprotect                  vsf_unprotect(VSF_HEAP_CFG_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_mcb_get_size(__mcb)           ((__mcb)->linear.next << VSF_HEAP_CFG_MCB_ALIGN_BIT)
#define __vsf_mcb_get_prev_size(__mcb)      ((__mcb)->linear.prev << VSF_HEAP_CFG_MCB_ALIGN_BIT)

/*============================ TYPES =========================================*/

struct __vsf_heap_mcb_t {
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    uint32_t magic;
#endif
    struct {
        uint16_t next;
        uint16_t prev;
    } linear;

    vsf_dlist_node_t list;
};
typedef struct __vsf_heap_mcb_t __vsf_heap_mcb_t;

struct __vsf_heap_t {
    vsf_dlist_t freelist[VSF_HEAP_CFG_FREELIST_NUM];
    vsf_dlist_t terminator;
};
typedef struct __vsf_heap_t __vsf_heap_t;

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

static NO_INIT __vsf_heap_t __vsf_heap;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK vsf_dlist_t * vsf_heap_get_freelist(vsf_dlist_t *freelist, uint_fast8_t freelist_num, uint_fast32_t size)
{
	return &__vsf_heap.freelist[0];
}

// MUST NOT return NULL;
static vsf_dlist_t * __vsf_heap_get_freelist(uint_fast32_t size)
{
	return vsf_heap_get_freelist(&__vsf_heap.freelist[0], VSF_HEAP_CFG_FREELIST_NUM, size);
}

static void __vsf_heap_mcb_init(__vsf_heap_mcb_t *mcb)
{
    memset(mcb, 0, sizeof(*mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    mcb->magic = VSF_HEAP_MCB_MAGIC;
#endif
}

static uint_fast32_t __vsf_heap_calc_unaligned_size(void *buffer, uint_fast32_t alignment)
{
    uint_fast32_t unaligned_size = (uint_fast32_t)buffer & (alignment - 1);
    if (unaligned_size != 0) {
        unaligned_size = alignment - unaligned_size;
    }
    return unaligned_size;
}

static void __vsf_heap_mcb_set_next(__vsf_heap_mcb_t *mcb, __vsf_heap_mcb_t *mcb_next)
{
    uint_fast32_t margin_size = (uint32_t)mcb_next - (uint32_t)mcb;
    margin_size >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
    mcb->linear.next = mcb_next->linear.prev = margin_size;
}

static __vsf_heap_mcb_t * __vsf_heap_mcb_get_next(__vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    __vsf_heap_mcb_t *mcb_next = (__vsf_heap_mcb_t *)((uint32_t)mcb + __vsf_mcb_get_size(mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb_next->magic == VSF_HEAP_MCB_MAGIC);
#endif
    return mcb_next;
}

static __vsf_heap_mcb_t * __vsf_heap_mcb_get_prev(__vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    __vsf_heap_mcb_t *mcb_prev = (__vsf_heap_mcb_t *)((uint32_t)mcb - __vsf_mcb_get_prev_size(mcb));
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb_prev->magic == VSF_HEAP_MCB_MAGIC);
#endif
    return mcb_prev;
}

static bool __vsf_heap_mcb_is_allocated(__vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

    return !vsf_dlist_is_in(__vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_remove_from_freelist(__vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

    vsf_dlist_remove(__vsf_heap_mcb_t, list, freelist, mcb);
}

static void __vsf_heap_mcb_add_to_freelist(__vsf_heap_mcb_t *mcb)
{
    uint_fast32_t size = __vsf_mcb_get_size(mcb);
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size);

    vsf_dlist_add_to_head(__vsf_heap_mcb_t, list, freelist, mcb);
}

static __vsf_heap_mcb_t * __vsf_heap_get_mcb(uint8_t *buffer)
{
    uint_fast32_t addr = (uint32_t)buffer - sizeof(__vsf_heap_mcb_t);
    addr = (addr + (VSF_HEAP_CFG_MCB_ALIGN - 1)) & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
    return (__vsf_heap_mcb_t *)addr;
}

static void * __vsf_heap_mcb_malloc(__vsf_heap_mcb_t *mcb, uint_fast32_t size,
            uint_fast32_t alignment)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    uint_fast32_t buffer_size = __vsf_mcb_get_size(mcb);
    uint8_t *buffer = (uint8_t *)&mcb[1];
    uint_fast32_t unaligned_size;

    buffer_size -= sizeof(__vsf_heap_mcb_t);
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
        __vsf_heap_mcb_t *mcb_new;
        uint_fast32_t margin_size, temp_size;

        __vsf_heap_mcb_remove_from_freelist(mcb);
        mcb_new = __vsf_heap_get_mcb(buffer);
        margin_size = (uint32_t)mcb_new - (uint32_t)mcb;

        if (0 == margin_size) {
            temp_size = mcb->linear.prev;
        } else if (margin_size <= sizeof(__vsf_heap_mcb_t)) {
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

        uint_fast32_t addr = (uint_fast32_t)buffer + size;
        addr += VSF_HEAP_CFG_MCB_ALIGN - 1;
        addr &= ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
        margin_size = buffer_size - (addr - (uint_fast32_t)buffer);
        if(margin_size > sizeof(__vsf_heap_mcb_t)) {
            mcb = (__vsf_heap_mcb_t *)addr;
            __vsf_heap_mcb_init(mcb);

            margin_size = (uint32_t)mcb - (uint32_t)mcb_new;
            margin_size >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
            mcb->linear.next = mcb_new->linear.next - margin_size;
            mcb->linear.prev = mcb_new->linear.next = margin_size;
            __vsf_heap_mcb_add_to_freelist(mcb);

            margin_size = mcb->linear.next;
            mcb = __vsf_heap_mcb_get_next(mcb);
            mcb->linear.prev = margin_size;
        }
        return buffer;
    }

    return NULL;
}

static void __vsf_heap_mcb_free(__vsf_heap_mcb_t *mcb)
{
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
    ASSERT(mcb->magic == VSF_HEAP_MCB_MAGIC);
#endif
    __vsf_heap_mcb_t *mcb_tmp;
    vsf_protect_t state;

    if (mcb->linear.next != 0) {
        state = vsf_heap_protect();
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
        vsf_heap_unprotect(state);
    }

    if (mcb->linear.prev != 0) {
        state = vsf_heap_protect();
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
        vsf_heap_unprotect(state);
    }

    state = vsf_heap_protect();
        __vsf_heap_mcb_add_to_freelist(mcb);
    vsf_heap_unprotect(state);
}

static void * __vsf_heap_freelist_malloc(vsf_dlist_t *plist, uint_fast32_t size,
            uint_fast32_t alignment)
{
    __vsf_heap_mcb_t *mcb;
    void *buffer = NULL;

    vsf_protect_t state = vsf_heap_protect();
        vsf_dlist_peek_head(__vsf_heap_mcb_t, list, plist, mcb);
        while ((NULL == buffer) && (mcb != NULL)) {
            buffer = __vsf_heap_mcb_malloc(mcb, size, alignment);
            if (NULL == buffer) {
                vsf_dlist_peek_next(__vsf_heap_mcb_t, list, mcb, mcb);
            }
        }
    vsf_heap_unprotect(state);
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
    __vsf_heap_mcb_t *mcb;

    ASSERT(NULL != heap);

#if VSF_HEAP_CFG_ADD_MERGE_EN == ENABLED
    vsf_dlist_t      *freelist;
    uint8_t          counter = 0;
    __vsf_heap_mcb_t *mcb_new, *mcb_next, *mcb_prev;
/*********************************New features*********************************/
    NEXT_FREELIST:
    freelist = vsf_heap_get_freelist(&__vsf_heap.freelist[counter],
                                     VSF_HEAP_CFG_FREELIST_NUM,
                                     1);

    if(!vsf_dlist_is_empty(freelist)) {
        vsf_dlist_peek_head(__vsf_heap_mcb_t, list, freelist, mcb);
    
        while((0 != mcb->linear.prev) && (0 != __vsf_heap_mcb_get_next(mcb)->linear.next) 
                                      && (NULL != mcb->list.next)) {
            PEEK_NEXT:
            vsf_dlist_peek_next(__vsf_heap_mcb_t, list, mcb, mcb);
        }
            
        if(0 == mcb->linear.prev) {
            if(((uint8_t *)mcb - heap - size) <= sizeof(__vsf_heap_mcb_t)) {
                vsf_protect_t state = vsf_heap_protect();
                
                __vsf_heap_mcb_remove_from_freelist(mcb);
                unaligned_size = __vsf_heap_calc_unaligned_size(heap, VSF_HEAP_CFG_MCB_ALIGN);
                heap += unaligned_size;
                size = (uint8_t *)mcb - heap;
                size -= unaligned_size;
                mcb_new = (__vsf_heap_mcb_t *)heap;
                __vsf_heap_mcb_init(mcb_new);
                
                mcb_new->linear.next = (size >> VSF_HEAP_CFG_MCB_ALIGN_BIT)
                                     + mcb->linear.next;
                mcb_next = __vsf_heap_mcb_get_next(mcb);
                
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                mcb->magic = 0;
#endif
                mcb_next->linear.prev = mcb_new->linear.next;
                offset = mcb_new->linear.next << VSF_HEAP_CFG_MCB_ALIGN_BIT;
                vsf_dlist_add_to_tail(  __vsf_heap_mcb_t, list,
                                        __vsf_heap_get_freelist(offset),
                                        mcb_new);
                
                vsf_heap_unprotect(state);
            } else if (0 == __vsf_heap_mcb_get_next(mcb)->linear.next) {
                goto TAIL;
            } else if (NULL != mcb->list.next) {
                goto PEEK_NEXT;
            } else {
                goto First_Add;
            }
        } else if (0 == __vsf_heap_mcb_get_next(mcb)->linear.next) {
            TAIL:
            vsf_protect_t state = vsf_heap_protect();
            
            mcb = __vsf_heap_mcb_get_next(mcb);
            if((heap - (uint8_t *)mcb - sizeof(__vsf_heap_mcb_t)) <= sizeof(__vsf_heap_mcb_t)) {
                
                offset = (size + heap - (uint8_t *)mcb - sizeof(__vsf_heap_mcb_t)) 
                         & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
                mcb_new = (__vsf_heap_mcb_t *)((uint8_t *)mcb + offset);
                offset >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
                
                __vsf_heap_mcb_init(mcb_new);
                mcb_new->linear.prev = offset;
                mcb->linear.next = offset;
                
                mcb_prev = __vsf_heap_mcb_get_prev(mcb);
                if(__vsf_heap_mcb_is_allocated(mcb_prev)) {
                    offset <<= VSF_HEAP_CFG_MCB_ALIGN_BIT;
                    vsf_dlist_add_to_tail(  __vsf_heap_mcb_t, list,
                                            __vsf_heap_get_freelist(offset),
                                            mcb);
                } else {
                    __vsf_heap_mcb_remove_from_freelist(mcb_prev);
                    mcb_prev->linear.next += mcb->linear.next;
                    mcb_new->linear.prev = mcb_prev->linear.next;
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                mcb->magic = 0;
#endif
                    offset = mcb_prev->linear.next << VSF_HEAP_CFG_MCB_ALIGN_BIT;
                    vsf_dlist_add_to_tail(  __vsf_heap_mcb_t, list,
                                            __vsf_heap_get_freelist(offset),
                                            mcb_prev);
                }
                
                vsf_heap_unprotect(state);
            } else if (NULL != mcb->list.next) {
                goto PEEK_NEXT;
            } else {
                goto First_Add;
            }
        } else if (NULL == mcb->list.next) {
            if((VSF_HEAP_CFG_FREELIST_NUM > 1) && (counter < (VSF_HEAP_CFG_FREELIST_NUM - 1))) {
                counter++;
                goto NEXT_FREELIST;
            } else {
                goto First_Add;
            }
        } else {
            goto PEEK_NEXT;
        }
    } else {
/******************************************************************************/
        First_Add:
#endif
        unaligned_size = __vsf_heap_calc_unaligned_size(heap, VSF_HEAP_CFG_MCB_ALIGN);
        heap += unaligned_size;
        size = (size - unaligned_size) & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);
        offset = (size - sizeof(__vsf_heap_mcb_t))  & ~(VSF_HEAP_CFG_MCB_ALIGN - 1);

        ASSERT(     !(size >> (VSF_HEAP_CFG_MCB_ALIGN_BIT + 16))
                &&  (size > 2 * sizeof(__vsf_heap_mcb_t)));

        mcb = (__vsf_heap_mcb_t *)((uint8_t *)heap + offset);
        offset >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
        __vsf_heap_mcb_init(mcb);
        mcb->linear.prev = offset;

        mcb = (__vsf_heap_mcb_t *)heap;
        __vsf_heap_mcb_init(mcb);
        mcb->linear.next = offset;

        vsf_protect_t state = vsf_heap_protect();
            offset <<= VSF_HEAP_CFG_MCB_ALIGN_BIT;
            vsf_dlist_add_to_tail(  __vsf_heap_mcb_t, list,
                                    __vsf_heap_get_freelist(offset),
                                    mcb);
        vsf_heap_unprotect(state);
#if VSF_HEAP_CFG_ADD_MERGE_EN == ENABLED
    }
#endif
}

void vsf_heap_add_memory(vsf_mem_t mem)
{
    vsf_heap_add(mem.pchSrc, mem.nSize);
}

void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t alignment)
{
    vsf_dlist_t *freelist = __vsf_heap_get_freelist(size + sizeof(__vsf_heap_mcb_t));
    void *buffer;

    if (alignment) {
        ASSERT((alignment >= 4) && !(alignment & (alignment - 1)));
    }

    size = (size + 3) & ~3;
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

//Adjust the allocated memory size(Custom alignment)
void * vsf_heap_realloc_aligned(void *buffer, uint_fast32_t size, uint_fast32_t alignment)
{
    void             *new_buffer;           
    __vsf_heap_mcb_t *mcb, *mcb_new;
    uint_fast32_t    memory_size, margin_size, addr;
    
    ASSERT((buffer != NULL) && (alignment >= 4) && !(alignment & (alignment - 1)));
    
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    realloc:
    memory_size = __vsf_mcb_get_size(mcb);
    memory_size -= (uint_fast32_t)buffer - (uint_fast32_t)mcb;
    
    if(memory_size >= size) {
        addr = (uint_fast32_t)buffer + size;
        addr += alignment - 1;
        addr &= ~(alignment - 1);
        margin_size = memory_size - addr + (uint_fast32_t)buffer;
        if(margin_size > sizeof(__vsf_heap_mcb_t)) {
            mcb_new = (__vsf_heap_mcb_t *)addr;
            __vsf_heap_mcb_init(mcb_new);
            
            margin_size >>= VSF_HEAP_CFG_MCB_ALIGN_BIT;
            mcb_new->linear.next = margin_size;
            mcb_new->linear.prev = mcb->linear.next - margin_size;
            mcb->linear.next = mcb_new->linear.prev;
            
            mcb = __vsf_heap_mcb_get_next(mcb_new);
            if(__vsf_heap_mcb_is_allocated(mcb)) {
                mcb->linear.prev = mcb_new->linear.next;
            } else {
                mcb_new->linear.next += mcb->linear.next;
                
                #if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                    mcb->magic = 0;
                #endif
                    
                mcb = __vsf_heap_mcb_get_next(mcb_new);    
                mcb->linear.prev = mcb_new->linear.next;
            }
            __vsf_heap_mcb_add_to_freelist(mcb_new);
            return buffer;
        } else {
            return buffer;
        }
    } else {
        mcb_new = __vsf_heap_mcb_get_next(mcb);
        if(__vsf_heap_mcb_is_allocated(mcb_new)) {
            new_buffer = vsf_heap_malloc_aligned(size, alignment);
            memcpy(new_buffer, buffer, memory_size);
            vsf_heap_free(buffer);
            return new_buffer;
        } else {
            mcb->linear.next += mcb_new->linear.next;
            
            #if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
                mcb_new->magic = 0;
            #endif
                
            mcb_new = __vsf_heap_mcb_get_next(mcb);
            mcb_new->linear.prev = mcb->linear.next;
            goto realloc;
        }
    }
}

//Adjust the allocated memory size(4-byte alignment)
void * vsf_heap_realloc(void *buffer, uint_fast32_t size)
{
    return vsf_heap_realloc_aligned(buffer, size, 4);
}

void vsf_heap_free(void *buffer)
{
    __vsf_heap_mcb_t *mcb;

    ASSERT(buffer != NULL);
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    ASSERT( __vsf_heap_mcb_is_allocated(mcb)
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
        &&  mcb->magic == VSF_HEAP_MCB_MAGIC
#endif
    );
    __vsf_heap_mcb_free(mcb);
}

//           pos
// |--------------------------------|
// |   head   |   free   |   tail   |
// |--------------------------------|
//             ___size___
bool vsf_heap_partial_free(void *buffer, uint_fast32_t pos, uint_fast32_t size)
{
    __vsf_heap_mcb_t *mcb, *mcb_free, *mcb_tail, *mcb_next;
    uint8_t *buffer_tmp;
    uint_fast32_t all_size;
    uint_fast32_t unaligned_size;

    ASSERT(buffer != NULL);
    mcb = __vsf_heap_get_mcb((uint8_t *)buffer);
    all_size = __vsf_mcb_get_size(mcb);
    ASSERT( __vsf_heap_mcb_is_allocated(mcb)
        && (all_size >= (pos + size + sizeof(__vsf_heap_mcb_t)))
#if VSF_HEAP_CFG_MCB_MAGIC_EN == ENABLED
        &&  mcb->magic == VSF_HEAP_MCB_MAGIC
#endif
    );
    all_size -= sizeof(__vsf_heap_mcb_t);
    mcb_next = __vsf_heap_mcb_get_next(mcb);

    buffer_tmp = (uint8_t *)buffer + pos;
    unaligned_size = __vsf_heap_calc_unaligned_size(buffer_tmp, VSF_HEAP_CFG_MCB_ALIGN);
    if (size < unaligned_size) {
        return false;
    }
    size -= unaligned_size;
    pos += unaligned_size;
    buffer_tmp += unaligned_size;
    if (buffer_tmp != buffer) {
        // need to allocate new mcb for freed area
        mcb_free = (__vsf_heap_mcb_t *)buffer_tmp;
        __vsf_heap_mcb_init(mcb_free);
        __vsf_heap_mcb_set_next(mcb, mcb_free);
    } else {
        mcb_free = mcb;
    }

    if (all_size > pos + size) {
        // need to allocate new mcb for tail area
        buffer_tmp += size;
        mcb_tail = __vsf_heap_get_mcb(buffer_tmp);
        if ((uint32_t)mcb_tail <= ((uint32_t)mcb_free + sizeof(__vsf_heap_mcb_t))) {
            return false;
        }
        __vsf_heap_mcb_init(mcb_tail);
        __vsf_heap_mcb_set_next(mcb_free, mcb_tail);
    } else {
        mcb_tail = mcb_free;
    }
    __vsf_heap_mcb_set_next(mcb_tail, mcb_next);

    vsf_heap_free((void *)((uint32_t)mcb_free + sizeof(__vsf_heap_mcb_t)));
    return true;
}

#endif