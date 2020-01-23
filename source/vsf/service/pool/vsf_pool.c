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

#define __VSF_POOL_CLASS_IMPLEMENT  

#include "service/vsf_service_cfg.h"

#if VSF_USE_POOL == ENABLED
#include "vsf_pool.h"  
#include <stdlib.h>

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wvla"
#endif

/*============================ MACROS ========================================*/

#undef  this
#define this    (*ptThis)

#if     defined(VSF_POOL_LOCK) && !defined(VSF_POOL_UNLOCK) 
#   define  VSF_POOL_UNLOCK()
#elif   !defined(VSF_POOL_LOCK) && defined(VSF_POOL_UNLOCK) 
#   define  VSF_POOL_LOCK()
#elif   !defined(VSF_POOL_LOCK) && !defined(VSF_POOL_UNLOCK) 
#   if defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 199901L
#       define VSF_POOL_LOCK()      CODE_REGION_START(this.ptRegion)
#       define VSF_POOL_UNLOCK()    CODE_REGION_END()
#   else
#       define VSF_POOL_LOCK()      CODE_REGION_SIMPLE_START(this.ptRegion)
#       define VSF_POOL_UNLOCK()    CODE_REGION_SIMPLE_END()
#   endif
#   define __VSF_POOL_USE_DEFAULT_ATOM_ACCESS
#endif

#if defined(VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY)
#       include VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY
#endif

#ifndef VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT
#   define VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT      ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name protected class __vsf_pool_node_t
//! @{
struct __vsf_pool_node_t {
    vsf_slist_node_t node;                  /* single list node for pool item */
};
typedef struct __vsf_pool_node_t __vsf_pool_node_t;
//! @}

/*============================ GLOBAL VARIABLES ==============================*/
const i_pool_t VSF_POOL = {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    &vsf_pool_init,
    {
        &vsf_pool_add_buffer_ex,
        &vsf_pool_add_buffer,
    },
    &vsf_pool_alloc,
    &vsf_pool_free,
    &vsf_pool_get_count,
#   if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    {
        &vsf_pool_get_tag,
        &vsf_pool_set_tag,
    }
#   endif
#else
    .Init =         &vsf_pool_init,
    .Buffer = {
        .AddEx =    &vsf_pool_add_buffer_ex,
        .Add =      &vsf_pool_add_buffer,
    },
    .Allocate =     &vsf_pool_alloc,
    .Free =         &vsf_pool_free,
    .Count =        &vsf_pool_get_count,
#   if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    .Tag = {
        .Get =      &vsf_pool_get_tag,
        .Set =      &vsf_pool_set_tag,
    }
#   endif
#endif
};


/*============================ LOCAL VARIABLES ===============================*/
#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED

static struct {
    vsf_slist_t tPoolList;
    uint16_t    hwPoolCount;
} s_tPoolStatisticChain = {0};

#endif

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN)                            \
    &&  defined(WEAK_VSF_HEAP_MALLOC_ALIGNED)
WEAK_VSF_HEAP_MALLOC_ALIGNED_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/


/*! \brief initialise target pool
 *! \param ptThis address of the target pool
 *! \param ptCFG configurations
 *! \return none
 */
void vsf_pool_init( vsf_pool_t *ptObj, 
                    uint32_t wItemSize, 
                    uint_fast16_t hwAlign, 
                    vsf_pool_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT(ptThis != NULL);

    memset(ptObj, 0, sizeof(vsf_pool_t));

#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    if (NULL == ptCFG) {
        this.ptRegion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
    } else {
        if (NULL == ptCFG->ptRegion) {
            this.ptRegion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
        } else {
            this.ptRegion = ptCFG->ptRegion;
        }
    }
#else
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    this.pTarget = ptCFG->pTarget;
#endif
#endif

#if VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED
    this.Statistic.wItemSize = wItemSize;
    if (0 == hwAlign) {
        hwAlign = sizeof(uint_fast8_t);
    } 
    this.Statistic.u15Align = hwAlign;
    this.fnItemInit = ptCFG->fnItemInit;
#endif

#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED
    this.Statistic.pchPoolName = ptCFG->pchPoolName;
    //! add pool to the pool list
    {
        s_tPoolStatisticChain.hwPoolCount++;
        
        vsf_slist_insert(class(vsf_pool_t),
                         Statistic.use_as__vsf_slist_node_t,
                         &(s_tPoolStatisticChain.tPoolList),
                         ptThis,
                        (   (this.Statistic.u15Align >= _->Statistic.u15Align) 
                        &&  (this.Statistic.wItemSize >= _->Statistic.wItemSize)));
                        
    }
#endif

    vsf_slist_init(&this.tFreeList);
}

/*! \brief add memory to pool 
 *! \param ptThis       address of the target pool
 *! \param pBuffer      address of the target memory
 *! \param wBufferSize  the size of the target memory
 *! \param wItemSize    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added 
 */
bool vsf_pool_add_buffer(   vsf_pool_t *ptThis, 
                            uintptr_t pBuffer, 
                            uint32_t wBufferSize,
                            uint32_t wItemSize)
{
    return vsf_pool_add_buffer_ex(ptThis, pBuffer, wBufferSize, wItemSize, NULL);
}

#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsf_plug_in_on_failed_to_feed_pool_on_heap)
bool vsf_plug_in_on_failed_to_feed_pool_on_heap(vsf_pool_t *ptObj)
{
    UNUSED_PARAM(ptObj);

    /*! \note return true will let the vsf_pool try again. Usually we can use 
     *        this function to print out the heap and pool usage info. You can 
     *        also use this function to allocate more resources to either pool 
     *        or heap and ask vsf pool to try again.
     */
    VSF_SERVICE_ASSERT(false);
    return false;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif


#ifndef WEAK_VSF_HEAP_MALLOC_ALIGNED
WEAK(vsf_heap_malloc_aligned)
void * vsf_heap_malloc_aligned(uint_fast32_t wSize, uint_fast32_t wAlign)
{
    UNUSED_PARAM(wSize);
    UNUSED_PARAM(wAlign);

    /*! \note if vsf_heap is enabled in your project, this function will be 
     *        replaced by the function with the same name in vsf_heap. Otherwise
     *        the posix_memalign will be used by default. You can also implement
     *        this function by yourself
     */     
    void *pMemory = NULL;
#if     defined(_POSIX_VERSION)                                                 \
    ||  (__IS_COMPILER_ARM_COMPILER_6 && !defined(__STRICT_ANSI__))
extern int posix_memalign(  void ** /*ret*/, 
                            size_t /*alignment*/, 
                            size_t /*size*/);

    posix_memalign(&pMemory, wSize, wAlign);
#endif
    return pMemory;
}
#endif
#endif

/*! \brief try to fetch a memory block from the target pool
 *! \param ptThis    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
uintptr_t vsf_pool_alloc(vsf_pool_t *ptObj)
{
    __vsf_pool_node_t *ptNode = NULL;
    class_internal(ptObj, ptThis, vsf_pool_t);
    

    VSF_SERVICE_ASSERT(ptThis != NULL);

    VSF_POOL_LOCK();
        /* verify it again for safe */
        if (!vsf_slist_is_empty(&this.tFreeList)) {
            vsf_slist_stack_pop(__vsf_pool_node_t, 
                                node,
                                &this.tFreeList, 
                                ptNode);
            this.hwFree--;
#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED
            this.hwUsed++;
#endif
        } 
#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
        else if (!this.Statistic.IsNoFeedOnHeap) {
            bool bRetry = false;
            do {
                //! feed on heap
#ifndef WEAK_VSF_HEAP_MALLOC_ALIGNED
                ptNode = (__vsf_pool_node_t *)vsf_heap_malloc_aligned(this.Statistic.wItemSize, this.Statistic.u15Align);
#else
                ptNode = (__vsf_pool_node_t *)WEAK_VSF_HEAP_MALLOC_ALIGNED(this.Statistic.wItemSize, this.Statistic.u15Align);
#endif
                if (NULL != ptNode) {
                #if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
                    if (this.fnItemInit != NULL) {
                        (*(this.fnItemInit))(this.pTarget, (uintptr_t)ptNode, this.Statistic.wItemSize);
                    }
                #else
                    if (this.fnItemInit != NULL) {
                        (*(this.fnItemInit))(NULL, ptNode, this.wItemSize);
                    }
                #endif
                    this.hwUsed++;
                } else {
                    bRetry = vsf_plug_in_on_failed_to_feed_pool_on_heap(ptObj);
                }
            } while(bRetry);
        }
#endif
    VSF_POOL_UNLOCK();

    return (uintptr_t)ptNode;
}

static void __vsf_pool_add_item(vsf_pool_t *ptObj, uintptr_t pitem)
{
    __vsf_pool_node_t *ptNode = (__vsf_pool_node_t *)pitem;
    class_internal(ptObj, ptThis, vsf_pool_t);
    

    VSF_POOL_LOCK();
        vsf_slist_stack_push(__vsf_pool_node_t, node, &this.tFreeList, ptNode);
        this.hwFree++;
    VSF_POOL_UNLOCK();
}

/*! \brief add memory to pool 
 *! \param ptThis           address of the target pool
 *! \param pBuffer          address of the target memory
 *! \param wBufferSize      the size of the target memory
 *! \param wItemSize        memory block size of the pool
 *! \param fnItemInit       block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added 
 */
bool vsf_pool_add_buffer_ex(    vsf_pool_t *ptObj, 
                                uintptr_t pBuffer, 
                                uint32_t wBufferSize,
                                uint32_t wItemSize, 
                                vsf_pool_item_init_evt_handler_t *fnItemInit)
{
    __vsf_pool_node_t *ptNode;
    class_internal(ptObj, ptThis, vsf_pool_t);
    

    VSF_SERVICE_ASSERT(     (ptThis != NULL) 
            			&&  (pBuffer != 0));

    /* Allowing multiple-layers of Pool management */
    if (    (wItemSize < sizeof(__vsf_pool_node_t))
        ||  (wBufferSize < wItemSize)) {
        return false;
    }
    
#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
    this.Statistic.IsNoFeedOnHeap = true;
#endif
    
    ptNode = (__vsf_pool_node_t *)pBuffer;
    do {
        __vsf_pool_add_item((vsf_pool_t *)ptThis, (uintptr_t)ptNode);

    #if VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT == ENABLED
    #   if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
        if (fnItemInit != NULL) {
            (*fnItemInit)(this.pTarget, (uintptr_t)ptNode, wItemSize);
        }
    #   else
        if (fnItemInit != NULL) {
            (*fnItemInit)(NULL, (uintptr_t)ptNode, wItemSize);
        }
    #   endif
    #endif
        ptNode = (__vsf_pool_node_t *)((uintptr_t)ptNode + wItemSize);
        wBufferSize -= wItemSize;
    } while (wBufferSize >= wItemSize);

    return true;
}


/*! \brief return a memory block to the target pool
 *! \param ptThis    address of the target pool
 *! \param pItem    target memory block
 *! \return none
 */
void vsf_pool_free(vsf_pool_t *ptObj, uintptr_t pItem)
{
    
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT((ptObj != NULL) && (pItem != 0));

    VSF_POOL_LOCK();
        __vsf_pool_add_item(ptObj, (uintptr_t)pItem);
        this.hwUsed--;
    VSF_POOL_UNLOCK();

}

SECTION("text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param ptThis    address of the target pool
 *! \return the number of memory blocks
 */
uint_fast16_t vsf_pool_get_count(vsf_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT(ptThis != NULL);

    return this.hwFree;
}

SECTION("text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param ptThis    address of the target pool
 *! \return the address of the object
 */
uintptr_t vsf_pool_get_tag(vsf_pool_t *ptObj)
{
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT(ptThis != NULL);

    return this.pTarget;
#else
    return NULL;
#endif
}

SECTION("text.vsf.utilities.vsf_pool_set_tag")
/*! \brief set the address of the object which is attached to the pool
 *! \param ptThis    address of the target pool
 *! \return the address of the object
 */
uintptr_t vsf_pool_set_tag(vsf_pool_t *ptObj, uintptr_t pTarget)
{
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT(ptThis != NULL);
    this.pTarget = pTarget;

    return this.pTarget;
#else
    return NULL;
#endif
}

SECTION("text.vsf.utilities.vsf_pool_get_region")
/*! \brief get the address of the code region used by this pool
 *! \param ptThis    address of the target pool
 *! \return the address of the code region
 */
code_region_t *vsf_pool_get_region(vsf_pool_t *ptObj)
{
#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    class_internal(ptObj, ptThis, vsf_pool_t);
    VSF_SERVICE_ASSERT(ptThis != NULL);

    return this.ptRegion;
#else
    return NULL;
#endif
}


#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#pragma clang diagnostic pop
#endif

#endif
