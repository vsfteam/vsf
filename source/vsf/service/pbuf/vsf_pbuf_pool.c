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
#if VSF_USE_PBUF == ENABLED
#include "./vsf_pbuf_pool.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
static vsf_pbuf_t * __vsf_stream_src_pbuf_free(void *obj_ptr, vsf_pbuf_t *block_ptr);

/*============================ GLOBAL VARIABLES ==============================*/
const i_pbuf_methods_t VSF_PBUF_ADAPTER_METHODS_STREAM_SRC = {
    .Free = &__vsf_stream_src_pbuf_free,
};
/*============================ IMPLEMENTATION ================================*/

static vsf_pbuf_t * __vsf_stream_src_pbuf_free(void *obj_ptr, vsf_pbuf_t *block_ptr)
{
    VSF_SERVICE_ASSERT(NULL != obj_ptr);

    vsf_pbuf_pool_free( (vsf_pbuf_pool_t *)obj_ptr, block_ptr);
    return NULL;
};
                                                                            
void vsf_pbuf_pool_init(vsf_pbuf_pool_t *this_ptr,  
                        uint32_t u32_item_size, 
                        uint_fast16_t u16_align, 
                        vsf_pool_cfg_t *pcfg)           
{                                                                               
    vsf_pool_init((vsf_pool_t *)this_ptr, u32_item_size, u16_align, pcfg);                                   
}                                                                               
                                                                            
bool vsf_pbuf_pool_add_buffer(  vsf_pbuf_pool_t *this_ptr, 
                                void *buffer_ptr, 
                                uint_fast32_t u32_size, 
                                uint_fast16_t u16_item_size)                
{                  
    if (   (NULL == buffer_ptr)
        || (0 == u32_size) 
        || (u16_item_size <sizeof(vsf_pbuf_t))) {
        return false;
    }
                                                                
    return vsf_pool_add_buffer_ex(  (vsf_pool_t *)this_ptr, (uintptr_t)buffer_ptr,              
                                    u32_size,                                      
                                    u16_item_size,               
                                    vsf_pbuf_pool_item_init_event_handler );                            
}                                                                               
                                                                                                                                          
                                                                            
vsf_pbuf_t *vsf_pbuf_pool_alloc(vsf_pbuf_pool_t *this_ptr)                             
{                  
    vsf_pbuf_t *pbuf = (vsf_pbuf_t *)vsf_pool_alloc((vsf_pool_t *)this_ptr);
    if (NULL != pbuf) {
        vsf_pbuf_size_reset(pbuf);
    }
    return pbuf;
}                                                                               
                                                                            
void vsf_pbuf_pool_free(vsf_pbuf_pool_t *this_ptr, vsf_pbuf_t *item_ptr)                 
{                                                                               
    vsf_pool_free((vsf_pool_t *)this_ptr, (uintptr_t)item_ptr);                         
}                                                                               
                                                                            
SECTION(".text." "vsf_pbuf" "_get_pool_item_count")                                
uint_fast32_t vsf_pbuf_get_pool_item_count(vsf_pbuf_pool_t *this_ptr)              
{                                                                               
    return vsf_pool_get_count((vsf_pool_t *)this_ptr);                             
}                                                                               
                                
                                       
SECTION(".text." "vsf_pbuf" "_pool_get_region")                                    
code_region_t *vsf_pbuf_pool_get_region(vsf_pbuf_pool_t *this_ptr)                 
{                                                                               
    return vsf_pool_get_region((vsf_pool_t *)this_ptr);                            
}                                                                               
                                                                            
SECTION(".text." "vsf_pbuf" "_pool_get_target")                                    
uintptr_t vsf_pbuf_pool_get_target(vsf_pbuf_pool_t *this_ptr)                          
{                                                                               
    return vsf_pool_get_tag((vsf_pool_t *)this_ptr);                               
}                                                                               
#endif                                                                            