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
#include "vsf_pool.h"  
#include <stdlib.h>

/*============================ MACROS ========================================*/

#undef  this
#define this    (*ptThis)

#ifndef VSF_POOL_CFG_ATOM_ACCESS

#   define __VSF_POOL_USE_DEFAULT_ATOM_ACCESS
/*! \note   By default, the driver tries to make all APIs thread-safe, in the  
 *!         case when you want to disable it, please use following macro to 
 *!         disable it:
 *!
 *!         #define VSF_POOL_CFG_ATOM_ACCESS(...)   __VA_ARGS__
 *!                 
 *!         
 *!         NOTE: This macro should be defined in app_cfg.h or vsf_cfg.h
 */
#   define VSF_POOL_CFG_ATOM_ACCESS(...)                                        \
        code_region_simple(this.ptRegion, __VA_ARGS__)

#elif defined(VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY)
#       include VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY
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
    .Init =         &vsf_pool_init,
    .Buffer = {
        .AddEx =    &vsf_pool_add_buffer_ex,
        .Add =      &vsf_pool_add_buffer,
    },
    .Allocate =     &vsf_pool_alloc,
    .Free =         &vsf_pool_free,
    .Count =        &vsf_pool_get_count,
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    .Tag = {
        .Get =      &vsf_pool_get_tag,
        .Set =      &vsf_pool_set_tag,
    }
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
    ASSERT(ptThis != NULL);

    memset(ptObj, 0, sizeof(vsf_pool_t));

#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    if (NULL == ptCFG) {
        this.ptRegion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
    } else {
        this.pTarget = ptCFG->pTarget;
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
    this.wItemSize = wItemSize;
    if (0 == hwAlign) {
        hwAlign = sizeof(uint_fast8_t);
    } 
    this.u15Align = hwAlign;
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
                            void *pBuffer, 
                            uint32_t wBufferSize,
                            uint32_t wItemSize)
{
    return vsf_pool_add_buffer_ex(ptThis, pBuffer, wBufferSize, wItemSize, NULL);
}

#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED
WEAK bool vsf_plug_in_on_failed_to_feed_pool_on_heap(vsf_pool_t *ptObj)
{
    /*! \note return true will let the vsf_pool try again. Usually we can use 
     *        this function to print out the heap and pool usage info. You can 
     *        also use this function to allocate more resources to either pool 
     *        or heap and ask vsf pool to try again.
     */
    return false;
}

WEAK void * vsf_heap_malloc_aligned(uint_fast32_t wSize, uint_fast32_t wAlign)
{
    /*! \note if vsf_heap is enabled in your project, this function will be 
     *        replaced by the function with the same name in vsf_heap. Otherwise
     *        the posix_memalign will be used by default. You can also implement
     *        this function by yourself
     */     
    void *pMemory = NULL;
    posix_memalign(&pMemory, wSize, wAlign);
    return pMemory;
}

#endif

/*! \brief try to fetch a memory block from the target pool
 *! \param ptThis    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
void *vsf_pool_alloc(vsf_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *ptNode = NULL;

    ASSERT(ptThis != NULL);


    VSF_POOL_CFG_ATOM_ACCESS(
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
                ptNode = vsf_heap_malloc_aligned(this.wItemSize, this.Statistic.u15Align);
                if (NULL != ptNode) {
                #if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
                    if (this.fnItemInit != NULL) {
                        (*(this.fnItemInit))(this.pTarget, ptNode, this.wItemSize);
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
    )

    return (void *)ptNode;
}

static void __vsf_pool_add_item(vsf_pool_t *ptObj, void *pitem)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *ptNode = (__vsf_pool_node_t *)pitem;

    VSF_POOL_CFG_ATOM_ACCESS(
        vsf_slist_stack_push(__vsf_pool_node_t, node, &this.tFreeList, ptNode);
        this.hwFree++;
    )
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
                                void *pBuffer, 
                                uint32_t wBufferSize,
                                uint32_t wItemSize, 
                                vsf_pool_item_init_evt_handler_t *fnItemInit)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *ptNode;

    ASSERT(     (ptThis != NULL) 
            &&  (pBuffer != NULL));

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
        __vsf_pool_add_item((vsf_pool_t *)ptThis, ptNode);

    #if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
        if (fnItemInit != NULL) {
            (*fnItemInit)(this.pTarget, ptNode, wItemSize);
        }
    #else
        if (fnItemInit != NULL) {
            (*fnItemInit)(NULL, ptNode, wItemSize);
        }
    #endif
        ptNode = (__vsf_pool_node_t *)((uint8_t *)ptNode + wItemSize);
        wBufferSize -= wItemSize;
    } while (wBufferSize >= wItemSize);

    return true;
}


/*! \brief return a memory block to the target pool
 *! \param ptThis    address of the target pool
 *! \param pItem    target memory block
 *! \return none
 */
void vsf_pool_free(vsf_pool_t *ptObj, void *pItem)
{
    
    class_internal(ptObj, ptThis, vsf_pool_t);
    ASSERT((ptObj != NULL) && (pItem != NULL));

    VSF_POOL_CFG_ATOM_ACCESS(
        __vsf_pool_add_item(ptObj, pItem);
        this.hwUsed--;
    )

}

SECTION("text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param ptThis    address of the target pool
 *! \return the number of memory blocks
 */
uint_fast16_t vsf_pool_get_count(vsf_pool_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

    return this.hwFree;
}

SECTION("text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param ptThis    address of the target pool
 *! \return the address of the object
 */
void *vsf_pool_get_tag(vsf_pool_t *ptObj)
{
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    class_internal(ptObj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

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
void *vsf_pool_set_tag(vsf_pool_t *ptObj, void *pTarget)
{
#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    class_internal(ptObj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);
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
    ASSERT(ptThis != NULL);

    return this.ptRegion;
#else
    return NULL;
#endif
}
