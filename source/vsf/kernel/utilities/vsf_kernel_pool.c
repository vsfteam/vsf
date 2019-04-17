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
#include "../vsf_kernel_cfg.h"

/*! \note: Include header files required by __class_xxxxxx */
#include "vsf.h"

/*! \note make this module possible to access protected member of vsf_pool_t */
#include "utilities/template/vsf_pool.h"        
#define __VSF_CLASS_INHERIT
#include "utilities/template/__class_pool.h"       

/*! \note implement class vsf_kernel_pool_t */
#include "vsf_kernel_pool.h"
#define __VSF_CLASS_IMPLEMENT
#include "./__class_kernel_pool.h"       


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_KERNEL_POOL_CFG_ATOM_ACCESS
/*! \note   By default, the driver tries to make all APIs thread-safe, in the  
 *!         case when you want to disable it, please use following macro to 
 *!         disable it:
 *!
 *!         #define VSF_KERNEL_POOL_CFG_ATOM_ACCESS()           \
 *!             for (int TPASTE2(___, __LINE__) =1;TPASTE2(___, __LINE__)--;)
 *!                 
 *!         
 *!         NOTE: This macro should be defined in app_cfg.h or vsf_cfg.h
 */
#   define VSF_KERNEL_POOL_CFG_ATOM_ACCESS()    SAFE_ATOM_CODE()
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/*! \brief initialise target pool
 *! \param pthis address of the target pool
 *! \return none
 */
void vsf_kernel_pool_init(vsf_kernel_pool_t *pobj)
{
    class_internal(pobj, pthis, vsf_kernel_pool_t);
    ASSERT(pthis != NULL);

    vsf_sem_init(&this.available_sem, 0);
    
    do {
        vsf_pool_cfg_t cfg = {
            .ptarget = pthis,
            .pregion = (code_region_t *)&VSF_SCHED_SAFE_CODE_REGION,
        };
        vsf_pool_init(&this.use_as__vsf_pool_t, &cfg);
    } while(0);
}

/*! \brief add memory to pool 
 *! \param pthis        address of the target pool
 *! \param pbuffer      address of the target memory
 *! \param buffer_size  the size of the target memory
 *! \param item_size    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added 
 */
bool vsf_kernel_pool_add_buffer(   vsf_kernel_pool_t *pobj, 
                            void *pbuffer, 
                            uint32_t buffer_size,
                            uint32_t item_size)
{
    class_internal(pobj, pthis, vsf_kernel_pool_t);
    bool result = false;
    
    ASSERT(     (pthis != NULL) 
            &&  (pbuffer != NULL));
    
    VSF_KERNEL_POOL_CFG_ATOM_ACCESS(){
        result = vsf_pool_add_buffer(&this.use_as__vsf_pool_t, 
                                    pbuffer, 
                                    buffer_size, 
                                    item_size);
        if (result) {
//TODO: Fix this            this.available_sem.cur += buffer_size / item_size;
        }
    }
    return result;
}

/*! \brief add memory to pool 
 *! \param pthis            address of the target pool
 *! \param pbuffer          address of the target memory
 *! \param buffer_size      the size of the target memory
 *! \param item_size        memory block size of the pool
 *! \param fnhandler_init   block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added 
 */
bool 
vsf_kernel_pool_add_buffer_ex(  vsf_kernel_pool_t *pobj, 
                                void *pbuffer, 
                                uint32_t buffer_size,
                                uint32_t item_size, 
                                vsf_pool_item_init_evt_handler_t *fnhandler_init)
{
    class_internal(pobj, pthis, vsf_kernel_pool_t);
    bool result = false;
    
    ASSERT(     (pthis != NULL) 
            &&  (pbuffer != NULL));
    
    VSF_KERNEL_POOL_CFG_ATOM_ACCESS(){
        result = vsf_pool_add_buffer_ex(&this.use_as__vsf_pool_t, 
                                    pbuffer, 
                                    buffer_size, 
                                    item_size,
                                    fnhandler_init);
        if (result) {
//TODO: Fix this            this.available_sem.cur += buffer_size / item_size;
        }
    }
    return result;
}

/*! \brief try to fetch a memory block from the target kernel pool
 *! \param pthis    address of the target pool
 *! \param time_out ms to wait, 0 means wait forever...
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
void *vsf_kernel_pool_alloc(vsf_kernel_pool_t *pobj, uint32_t time_out)
{
    class_internal(pobj, pthis, vsf_kernel_pool_t);
    
    void *pnode = NULL;

    ASSERT(pthis != NULL);

    if (VSF_SYNC_GET == vsf_thread_sem_pend(&(this.available_sem), time_out)) {
        pnode = vsf_pool_alloc(&(this.use_as__vsf_pool_t));
    }

    return (void *)pnode;
}

/*! \brief return a memory block to the target pool
 *! \param pthis    address of the target pool
 *! \param pitem    target memory block
 *! \return none
 */
void vsf_kernel_pool_free(vsf_pool_t *pobj, void *pitem)
{
    class_internal(pobj, pthis, vsf_kernel_pool_t);
    ASSERT(pthis != NULL);
    
    if (NULL == pitem) {
        return ;
    }
    
    //VSF_KERNEL_POOL_CFG_ATOM_ACCESS(
        vsf_pool_free(&(this.use_as__vsf_pool_t), pitem);
        vsf_sync_increase(&(this.available_sem));                              
    //)
}
