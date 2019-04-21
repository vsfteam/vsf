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

/*============================ MACROS ========================================*/

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
        code_region_simple(this.pregion, __VA_ARGS__)

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
    .Tag = {
        .Get =      &vsf_pool_get_tag,
        .Set =      &vsf_pool_set_tag,
    }
};


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*! \brief initialise target pool
 *! \param ptThis address of the target pool
 *! \param pcfg configurations
 *! \return none
 */
void vsf_pool_init(vsf_pool_t *pobj, vsf_pool_cfg_t *pcfg)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

    memset(pobj, 0, sizeof(vsf_pool_t));

#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    if (NULL == pcfg) {
        this.pregion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
    } else {
        this.ptag = pcfg->ptarget;
        if (NULL == pcfg->pregion) {
            this.pregion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
        } else {
            this.pregion = pcfg->pregion;
        }
    }
#else
    this.ptag = pcfg->ptarget;
#endif

    vsf_slist_init(&this.free_list);
}

/*! \brief add memory to pool 
 *! \param ptThis        address of the target pool
 *! \param pbuffer      address of the target memory
 *! \param buffer_size  the size of the target memory
 *! \param item_size    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added 
 */
bool vsf_pool_add_buffer(   vsf_pool_t *ptThis, 
                            void *pbuffer, 
                            uint32_t buffer_size,
                            uint32_t item_size)
{
    return vsf_pool_add_buffer_ex(ptThis, pbuffer, buffer_size, item_size, NULL);
}


/*! \brief try to fetch a memory block from the target pool
 *! \param ptThis    address of the target pool
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
void *vsf_pool_alloc(vsf_pool_t *pobj)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *pnode = NULL;

    ASSERT(ptThis != NULL);

    VSF_POOL_CFG_ATOM_ACCESS(
        /* verify it again for safe */
        if (!vsf_slist_is_empty(&this.free_list)) {
            vsf_slist_stack_pop(__vsf_pool_node_t, 
                                node,
                                &this.free_list, 
                                pnode);
            this.count--;
        }       
    )

    return (void *)pnode;
}

static void __vsf_pool_add_item(vsf_pool_t *pobj, void *pitem)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *pnode = (__vsf_pool_node_t *)pitem;

    VSF_POOL_CFG_ATOM_ACCESS(
        vsf_slist_stack_push(__vsf_pool_node_t, node, &this.free_list, pnode);
        this.count++;
    )
}

/*! \brief add memory to pool 
 *! \param ptThis            address of the target pool
 *! \param pbuffer          address of the target memory
 *! \param buffer_size      the size of the target memory
 *! \param item_size        memory block size of the pool
 *! \param fnhandler_init   block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added 
 */
bool vsf_pool_add_buffer_ex(    vsf_pool_t *pobj, 
                                void *pbuffer, 
                                uint32_t buffer_size,
                                uint32_t item_size, 
                                vsf_pool_item_init_evt_handler_t *fnhandler_init)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    __vsf_pool_node_t *pnode;

    ASSERT(     (ptThis != NULL) 
            &&  (pbuffer != NULL));

    /* Allowing multiple-layers of Pool management */
    if (    (item_size < sizeof(__vsf_pool_node_t))
        ||  (buffer_size < item_size)) {
        return false;
    }

    pnode = (__vsf_pool_node_t *)pbuffer;
    do {
        __vsf_pool_add_item((vsf_pool_t *)ptThis, pnode);

        if (fnhandler_init != NULL) {
            (*fnhandler_init)(this.ptag, pnode, item_size);
        }
        pnode = (__vsf_pool_node_t *)((uint8_t *)pnode + item_size);
        buffer_size -= item_size;
    } while (buffer_size >= item_size);

    return true;
}


/*! \brief return a memory block to the target pool
 *! \param ptThis    address of the target pool
 *! \param pitem    target memory block
 *! \return none
 */
void vsf_pool_free(vsf_pool_t *ptThis, void *pitem)
{
    ASSERT((ptThis != NULL) && (pitem != NULL));

    __vsf_pool_add_item(ptThis, pitem);
}

SECTION("text.vsf.utilities.vsf_pool_get_count")
/*! \brief get the number of memory blocks available in the target pool
 *! \param ptThis    address of the target pool
 *! \return the number of memory blocks
 */
uint_fast16_t vsf_pool_get_count(vsf_pool_t *pobj)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

    return this.count;
}

SECTION("text.vsf.utilities.vsf_pool_get_tag")
/*! \brief get the address of the object which is attached to the pool
 *! \param ptThis    address of the target pool
 *! \return the address of the object
 */
void *vsf_pool_get_tag(vsf_pool_t *pobj)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

    return this.ptag;
}

SECTION("text.vsf.utilities.vsf_pool_set_tag")
/*! \brief set the address of the object which is attached to the pool
 *! \param ptThis    address of the target pool
 *! \return the address of the object
 */
void *vsf_pool_set_tag(vsf_pool_t *pobj, void *ptag)
{
    class_internal(pobj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);
    this.ptag = ptag;

    return this.ptag;
}

SECTION("text.vsf.utilities.vsf_pool_get_region")
/*! \brief get the address of the code region used by this pool
 *! \param ptThis    address of the target pool
 *! \return the address of the code region
 */
code_region_t *vsf_pool_get_region(vsf_pool_t *pobj)
{
#if defined(__VSF_POOL_USE_DEFAULT_ATOM_ACCESS)
    class_internal(pobj, ptThis, vsf_pool_t);
    ASSERT(ptThis != NULL);

    return this.pregion;
#else
    return NULL;
#endif
}
