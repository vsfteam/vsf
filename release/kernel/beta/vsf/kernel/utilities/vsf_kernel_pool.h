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

#ifndef __VSF_KERNEL_POOL_H__
#define __VSF_KERNEL_POOL_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_kernel_cfg.h"

/*! \note: Include header files required by __class_xxxxxx */
#include "../vsf_os.h"
#include "../vsf_eda.h"
#include "./__class_kernel_pool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*! \brief initialise target pool
 *! \param pthis address of the target pool
 *! \return none
 */
extern void vsf_kernel_pool_init(vsf_kernel_pool_t *pobj);

/*! \brief add memory to pool 
 *! \param pthis            address of the target pool
 *! \param pbuffer          address of the target memory
 *! \param buffer_size      the size of the target memory
 *! \param item_size        memory block size of the pool
 *! \param fnhandler_init   block initialisation handler
 *! \retval false           the buffer size is too small or invalid parameters
 *! \retval true            buffer is added 
 */
extern bool 
vsf_kernel_pool_add_buffer_ex(  vsf_kernel_pool_t *pobj, 
                                void *pbuffer, 
                                uint32_t buffer_size,
                                uint32_t item_size, 
                                vsf_pool_item_init_evt_handler_t *fnhandler_init);
                                
/*! \brief add memory to pool 
 *! \param pthis        address of the target pool
 *! \param pbuffer      address of the target memory
 *! \param buffer_size  the size of the target memory
 *! \param item_size    memory block size of the pool
 *! \retval false       the buffer size is too small or invalid parameters
 *! \retval true        buffer is added 
 */
extern bool vsf_kernel_pool_add_buffer( vsf_kernel_pool_t *pobj, 
                                        void *pbuffer, 
                                        uint32_t buffer_size,
                                        uint32_t item_size);

/*! \brief try to fetch a memory block from the target kernel pool
 *! \param pthis    address of the target pool
 *! \param time_out ms to wait, 0 means wait forever...
 *! \retval NULL    the pool is empty
 *! \retval !NULL   address of the allocated memory block
 */
extern void *vsf_kernel_pool_alloc(vsf_kernel_pool_t *pobj, uint32_t time_out);


/*! \brief return a memory block to the target pool
 *! \param pthis    address of the target pool
 *! \param pitem    target memory block
 *! \return none
 */
extern void vsf_kernel_pool_free(vsf_pool_t *pobj, void *pitem);

#endif