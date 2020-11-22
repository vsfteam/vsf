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
#include "hal/arch/vsf_arch.h"

#if defined(VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY)
#   include VSF_POOL_CFG_ATOM_ACCESS_DEPENDENCY
#endif

#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wvla"
#endif

/*============================ MACROS ========================================*/

#undef  this
#define this    (*this_ptr)

#if     defined(VSF_POOL_LOCK) && !defined(VSF_POOL_UNLOCK)
#   define  VSF_POOL_UNLOCK()
#elif   !defined(VSF_POOL_LOCK) && defined(VSF_POOL_UNLOCK)
#   define  VSF_POOL_LOCK()
#elif   !defined(VSF_POOL_LOCK) && !defined(VSF_POOL_UNLOCK)
#   if defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 199901L
#       define VSF_POOL_LOCK()      CODE_REGION_START(this.region_ptr)
#       define VSF_POOL_UNLOCK()    CODE_REGION_END()
#   else
#       define VSF_POOL_LOCK()      CODE_REGION_SIMPLE_START(this.region_ptr)
#       define VSF_POOL_UNLOCK()    CODE_REGION_SIMPLE_END()
#   endif
#   define __VSF_POOL_USE_DEFAULT_ATOM_ACCESS
#endif

#ifndef VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT
#   define VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT      ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
//! \name protected class __vsf_pool_node_t
//! @{
typedef struct __vsf_pool_node_t {
    vsf_slist_node_t node;                  /* single list node for pool item */
} __vsf_pool_node_t;
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
    vsf_slist_t pool_list;
    uint16_t    pool_cnt;
} __pool_statistic_chain = {0};

#endif

/*============================ PROTOTYPES ====================================*/

extern void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t align);

/*============================ IMPLEMENTATION ================================*/


/*! \brief initialise target pool
 *! \param this_ptr address of the target pool
 *! \param cfg_ptr configurations
 *! \return none
 */
void vsf_pool_init( vsf_pool_t *obj_ptr,
                    uint32_t item_size,
                    uint_fast16_t align,
                    vsf_pool_cfg_t *cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT(this_ptr != NULL);

    memset(obj_ptr, 0, sizeof(vsf_pool_t));

#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    if (NULL == cfg_ptr) {
        this.region_ptr = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
    } else {
        if (NULL == cfg_ptr->region_ptr) {
            this.region_ptr = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
        } else {
            this.region_ptr = cfg_ptr->region_ptr;
        }
    }
#else
#   if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
    this.target_ptr = cfg_ptr->target_ptr;
#   endif
#endif

#if VSF_POOL_CFG_FEED_ON_HEAP   == ENABLED
    this.statistic.item_size = item_size;
    if (0 == align) {
        align = sizeof(uint_fast8_t);
    }
    this.statistic.u15_align = align;
    this.item_init_fn = cfg_ptr->item_init_fn;
#endif

#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED
    this.statistic.pool_name = cfg_ptr->pool_name_str;
    //! add pool to the pool list
    {
        __pool_statistic_chain.pool_cnt++;

        vsf_slist_insert(class(vsf_pool_t),
                         statistic.use_as__vsf_slist_node_t,
                         &(__pool_statistic_chain.pool_list),
                         this_ptr,
                        (   (this.statistic.u15_align >= _->statistic.u15_align)
                        &&  (this.statistic.item_size >= _->statistic.item_size)));

    }
#endif

    vsf_slist_init(&this.free_list);
}

/*! \brief add memory to pool
 *! \param this_ptr       address of the target pool
 *! \param buffer_ptr      address of the target memory
 *! \param buffer_size  the size of the target memory
 *! \param item_size    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added
 */
bool vsf_pool_add_buffer(   vsf_pool_t *this_ptr,
                            uintptr_t buffer_ptr,
                            uint32_t buffer_size,
                            uint32_t item_size)
{
    return vsf_pool_add_buffer_ex(this_ptr, buffer_ptr, buffer_size, item_size, NULL);
}


#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsf_plug_in_on_failed_to_feed_pool_on_heap)
bool vsf_plug_in_on_failed_to_feed_pool_on_heap(vsf_pool_t *obj_ptr)
{
    UNUSED_PARAM(obj_ptr);

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
void * vsf_heap_malloc_aligned(uint_fast32_t size, uint_fast32_t align)
{
    UNUSED_PARAM(size);
    UNUSED_PARAM(align);

    /*! \note if vsf_heap is enabled in your project, this function will be
     *        replaced by the function with the same name in vsf_heap. Otherwise
     *        the posix_memalign will be used by default. You can also implement
     *        this function by yourself
     */
    void *memory_ptr = NULL;
#if     defined(_POSIX_VERSION)                                                 \
    ||  (__IS_COMPILER_ARM_COMPILER_6 && !defined(__STRICT_ANSI__))
extern int posix_memalign(  void ** /*ret*/,
                            size_t /*alignment*/,
                            size_t /*size*/);

    posix_memalign(&memory_ptr, size, align);
#endif
    return memory_ptr;
}
#endif

/*! \brief try to fetch a memory block from the target pool
 *! \param this_ptr    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
uintptr_t vsf_pool_alloc(vsf_pool_t *obj_ptr)
{
    __vsf_pool_node_t *node_ptr = NULL;
    class_internal(obj_ptr, this_ptr, vsf_pool_t);


    VSF_SERVICE_ASSERT(this_ptr != NULL);

    VSF_POOL_LOCK();
        /* verify it again for safe */
        if (!vsf_slist_is_empty(&this.free_list)) {
            vsf_slist_stack_pop(__vsf_pool_node_t,
                                node,
                                &this.free_list,
                                node_ptr);
            this.free_cnt--;
#if VSF_POOL_CFG_STATISTIC_MODE == ENABLED
            this.used_cnt++;
#endif
        }
#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
        else if (!this.statistic.is_no_feed_on_heap) {
            bool retry = false;
            do {
                //! feed on heap
                node_ptr = (__vsf_pool_node_t *)vsf_heap_malloc_aligned(this.statistic.item_size, this.statistic.u15_align);
                if (NULL != node_ptr) {
                #if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
                    if (this.item_init_fn != NULL) {
                        (*(this.item_init_fn))(this.target_ptr, (uintptr_t)node_ptr, this.statistic.item_size);
                    }
                #else
                    if (this.item_init_fn != NULL) {
                        (*(this.item_init_fn))(NULL, (uintptr_t)node_ptr, this.item_size);
                    }
                #endif
                    this.used_cnt++;
                } else {
                    retry = vsf_plug_in_on_failed_to_feed_pool_on_heap(obj_ptr);
                }
            } while(retry);
        }
#endif
    VSF_POOL_UNLOCK();

    return (uintptr_t)node_ptr;
}

static void __vsf_pool_add_item(vsf_pool_t *obj_ptr, uintptr_t pitem)
{
    __vsf_pool_node_t *node_ptr = (__vsf_pool_node_t *)pitem;
    class_internal(obj_ptr, this_ptr, vsf_pool_t);


    VSF_POOL_LOCK();
        vsf_slist_stack_push(__vsf_pool_node_t, node, &this.free_list, node_ptr);
        this.free_cnt++;
    VSF_POOL_UNLOCK();
}

/*! \brief add memory to pool
 *! \param this_ptr           address of the target pool
 *! \param buffer_ptr          address of the target memory
 *! \param buffer_size      the size of the target memory
 *! \param item_size        memory block size of the pool
 *! \param item_init_fn       block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added
 */
bool vsf_pool_add_buffer_ex(    vsf_pool_t *obj_ptr,
                                uintptr_t buffer_ptr,
                                uint32_t buffer_size,
                                uint32_t item_size,
                                vsf_pool_item_init_evt_handler_t *item_init_fn)
{
    __vsf_pool_node_t *node_ptr;
    class_internal(obj_ptr, this_ptr, vsf_pool_t);


    VSF_SERVICE_ASSERT(     (this_ptr != NULL)
            			&&  (buffer_ptr != 0));

    /* Allowing multiple-layers of Pool management */
    if (    (item_size < sizeof(__vsf_pool_node_t))
        ||  (buffer_size < item_size)) {
        return false;
    }

#if VSF_POOL_CFG_FEED_ON_HEAP == ENABLED
    this.statistic.is_no_feed_on_heap = true;
#endif

    node_ptr = (__vsf_pool_node_t *)buffer_ptr;
    do {
        __vsf_pool_add_item((vsf_pool_t *)this_ptr, (uintptr_t)node_ptr);

    #if VSF_POOL_CFG_SUPPORT_USER_ITEM_INIT == ENABLED
    #   if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
        if (item_init_fn != NULL) {
            (*item_init_fn)(this.target_ptr, (uintptr_t)node_ptr, item_size);
        }
    #   else
        if (item_init_fn != NULL) {
            (*item_init_fn)(NULL, (uintptr_t)node_ptr, item_size);
        }
    #   endif
    #endif
        node_ptr = (__vsf_pool_node_t *)((uintptr_t)node_ptr + item_size);
        buffer_size -= item_size;
    } while (buffer_size >= item_size);

    return true;
}


/*! \brief return a memory block to the target pool
 *! \param this_ptr    address of the target pool
 *! \param pItem    target memory block
 *! \return none
 */
void vsf_pool_free(vsf_pool_t *obj_ptr, uintptr_t pItem)
{

    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT((obj_ptr != NULL) && (pItem != 0));

    VSF_POOL_LOCK();
        __vsf_pool_add_item(obj_ptr, (uintptr_t)pItem);
        this.used_cnt--;
    VSF_POOL_UNLOCK();

}

SECTION(".text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param this_ptr    address of the target pool
 *! \return the number of memory blocks
 */
uint_fast16_t vsf_pool_get_count(vsf_pool_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT(this_ptr != NULL);

    return this.free_cnt;
}

#if VSF_POOL_CFG_SUPPORT_USER_OBJECT == ENABLED
SECTION(".text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param this_ptr    address of the target pool
 *! \return the address of the object
 */
uintptr_t vsf_pool_get_tag(vsf_pool_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT(this_ptr != NULL);

    return this.target_ptr;
}

SECTION(".text.vsf.utilities.vsf_pool_set_tag")
/*! \brief set the address of the object which is attached to the pool
 *! \param this_ptr    address of the target pool
 *! \return the address of the object
 */
uintptr_t vsf_pool_set_tag(vsf_pool_t *obj_ptr, uintptr_t target_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT(this_ptr != NULL);
    this.target_ptr = target_ptr;

    return this.target_ptr;
}
#endif

SECTION(".text.vsf.utilities.vsf_pool_get_region")
/*! \brief get the address of the code region used by this pool
 *! \param this_ptr    address of the target pool
 *! \return the address of the code region
 */
code_region_t *vsf_pool_get_region(vsf_pool_t *obj_ptr)
{
#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    class_internal(obj_ptr, this_ptr, vsf_pool_t);
    VSF_SERVICE_ASSERT(this_ptr != NULL);

    return this.region_ptr;
#else
    return NULL;
#endif
}


#if __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#pragma clang diagnostic pop
#endif

#endif
