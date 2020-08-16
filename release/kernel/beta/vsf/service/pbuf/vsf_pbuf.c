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
#define __VSF_PBUF_CLASS_IMPLEMENT
#include "service/vsf_service_cfg.h"
#if VSF_USE_PBUF == ENABLED

#include <string.h>

#include "vsf_pbuf.h"


/*============================ MACROS ========================================*/
#undef  this
#define this    (*this_ptr)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static struct {
    const vsf_pbuf_adapter_t *adapters_ptr;
    uint_fast8_t        length;
}__vsf_pbuf_cb = {NULL, 0};

/*============================ PROTOTYPES ====================================*/   




/*============================ GLOBAL VARIABLES ==============================*/
const i_pbuf_t VSF_PBUF = {
    .Init =             &vsf_pbuf_init,
#if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
    .REF = {
        .Increase =     &vsf_pbuf_ref_increase,
        .Decrease =     &vsf_pbuf_free,
    },
#endif
    .PoolHelper = {
        .Free =         &vsf_pbuf_free,
        .ItemInitEventHandler = {
            .Get =      &vsf_pbuf_pool_item_init_event_handler,
        },
    },
    .Adapter = {
        .Register =     &vsf_adapter_register,
        .Get =          &vsf_pbuf_adapter_get,
    },
    .Capability = {
        .Get =          &vsf_pbuf_capability_get,
    },
    .Size = {
        .Get =          &vsf_pbuf_size_get,
        .Set =          &vsf_pbuf_size_set,
        .Reset =        &vsf_pbuf_size_reset,
        .Capacity =     &vsf_pbuf_capacity_get,
    },
    .Buffer = {
        .Get =          &vsf_pbuf_buffer_get,
        .Write =        &vsf_pbuf_buffer_write,
        .Read =         &vsf_pbuf_buffer_read,
    },
};

/*============================ IMPLEMENTATION ================================*/


static const vsf_pbuf_adapter_t* __vsf_get_adapter_interface(vsf_pbuf_t *ptItem)
{
    class_internal(ptItem, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    const vsf_pbuf_adapter_t *adapter_ptr = NULL;
    
    do {
        uint_fast8_t u8_id = this.u8_adapter_id;
        
        if (0xFF == u8_id) {
            break;
        } /*else if (VSF_PBUF_FREE_TO_ANY == u8_id) {
            u8_id = 1;
        }  */
        
        if (NULL == __vsf_pbuf_cb.adapters_ptr) {
            break;
        }
        if (u8_id >= __vsf_pbuf_cb.length) {
            break;
        }
        
        adapter_ptr = &__vsf_pbuf_cb.adapters_ptr[u8_id];
        VSF_SERVICE_ASSERT(NULL != adapter_ptr->methods_ptr);
    } while(0);
    
    return adapter_ptr;
}

void vsf_adapter_register(const vsf_pbuf_adapter_t *adaptors_ptr, uint_fast8_t u8_size)
{
    //!< this function could only be called once.
    VSF_SERVICE_ASSERT(__vsf_pbuf_cb.adapters_ptr == NULL);   
    
    __vsf_pbuf_cb.adapters_ptr = adaptors_ptr;
    __vsf_pbuf_cb.length = u8_size;
}

const vsf_pbuf_adapter_t *vsf_pbuf_adapter_get(uint_fast8_t u8_id)
{
    const vsf_pbuf_adapter_t *adapter_ptr = NULL;
    
    /*if (VSF_PBUF_FREE_TO_ANY == u8_id) {
        adapter_ptr =  __vsf_pbuf_cb.adapters_ptr;
    } else */
    if (u8_id < __vsf_pbuf_cb.length) {
        adapter_ptr =  &__vsf_pbuf_cb.adapters_ptr[u8_id];
    }

    return adapter_ptr;
}

vsf_pbuf_t *vsf_pbuf_init(vsf_pbuf_t *pbuf, vsf_pbuf_cfg_t *pcfg)
{
    class_internal(pbuf, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != pbuf && NULL != pcfg);
    
    do {
        if (    (pcfg->u24_block_size < sizeof(this)) 
            &&  (NULL == pcfg->buffer_ptr)) {
            break;
        }

        this.Capability = pcfg->Capability;

        if (NULL != pcfg->buffer_ptr) {
            this.buffer_ptr = pcfg->buffer_ptr;
            this.u24_size = pcfg->u24_block_size;
        } else {
            this.buffer_ptr = ((uint8_t *)&this)+sizeof(this);
            this.u24_size = pcfg->u24_block_size - sizeof(this);
            this.u24_block_size = this.u24_size;
        }

        this.u8_adapter_id = pcfg->adapter_id;
    } while(false);
    
    return pbuf;
}

void vsf_pbuf_size_reset(vsf_pbuf_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    if (!this.is_no_write) {      //!< readonly
        this.u24_size = this.u24_block_size;
    }
}

void *vsf_pbuf_buffer_get(vsf_pbuf_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
    if (this.is_no_direct_access) {
        return NULL;
    }
#endif
    return this.buffer_ptr;
}


int_fast32_t vsf_pbuf_buffer_write( vsf_pbuf_t *obj_ptr, 
                            const void *psrc, 
                            int_fast32_t s32_size, 
                            uint_fast32_t offsite)
{
    VSF_SERVICE_ASSERT(NULL != obj_ptr && NULL != psrc);
    
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    int_fast32_t nWrittenSize = -1;
    do {
        if (0 == s32_size) {
            break;
        } else if (this.is_no_write) {
            break;
        }
        
        uint_fast32_t u32_max_size = this.u24_block_size - offsite;
        s32_size = min(s32_size, u32_max_size);

#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
        if (this.is_no_direct_access) {
            const vsf_pbuf_adapter_t *
                adapter_ptr = __vsf_get_adapter_interface(obj_ptr);
            if (NULL == adapter_ptr) {
                break;
            }
            if (NULL == adapter_ptr->methods_ptr->Write) {
                break;
            }
            
            s32_size = (*adapter_ptr->methods_ptr->Write)(adapter_ptr->target_ptr, 
                                                  this.buffer_ptr,
                                                  psrc,
                                                  s32_size,
                                                  offsite);
            if (s32_size < 0) {
                break;
            }

        } else 
#endif
        {
            memcpy(((uint8_t *)vsf_pbuf_buffer_get(obj_ptr)) + offsite, psrc, s32_size);
        }
        
        this.u24_size = s32_size + offsite;
        nWrittenSize = s32_size;
    } while(false);
    
    return nWrittenSize;
}

int_fast32_t vsf_pbuf_buffer_read( vsf_pbuf_t *obj_ptr, 
                                void *psrc, 
                                int_fast32_t s32_size, 
                                uint_fast32_t u32_offset)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr && NULL != psrc);
    //uint8_t *pbuffer = NULL;
    //vsf_mem_t result = {.s32_size = -1,};
    int_fast32_t nReadSize = -1;
    do {
        if (0 == s32_size) {
            break;
        } else if (this.is_no_read) {
            break;
        }
        
        uint_fast32_t max_size = this.u24_block_size - u32_offset;
#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
        if (this.is_no_direct_access) {
            s32_size = min(s32_size, max_size);
            
            const vsf_pbuf_adapter_t *
                adapter_ptr = __vsf_get_adapter_interface(obj_ptr);
            if (NULL == adapter_ptr) {
                break;
            }
            if (NULL == adapter_ptr->methods_ptr->Read) {
                break;
            }
            
            s32_size = (*adapter_ptr->methods_ptr->Read)(   adapter_ptr->target_ptr, 
                                                    this.buffer_ptr,
                                                    psrc,
                                                    s32_size,
                                                    u32_offset);
            if (s32_size < 0) {
                break;
            }
            //pbuffer = psrc;

        } else 
#endif
        {
        //if (s32_size <= max_size) {
        //    pbuffer = (uint8_t *)vsf_pbuf_buffer_get(obj_ptr) + u32_offset;
        //} else {
            s32_size = min(s32_size, max_size);
            memcpy(psrc, this.buffer_ptr + u32_offset, s32_size);
            //pbuffer = psrc;
        //}
        }
        
        //result.buffer_ptr = pbuffer;
        nReadSize = s32_size;
        
    } while(false);
    
    return nReadSize;
}


void vsf_pbuf_size_set(vsf_pbuf_t *obj_ptr, int_fast32_t s32_size)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    
    if (this.is_no_write) {            //! readonly
        return ;
    }
    
    this.u24_size = min(s32_size, this.u24_block_size);
}

int_fast32_t vsf_pbuf_capacity_get(vsf_pbuf_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    
    return this.u24_block_size;
}

int_fast32_t vsf_pbuf_size_get(vsf_pbuf_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);

    return this.u24_size;
}

vsf_pbuf_capability_t vsf_pbuf_capability_get(vsf_pbuf_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);

    return this.tFeature;
}
#if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
void vsf_pbuf_ref_increase(vsf_pbuf_t *pbuf)
{
    class_internal(pbuf, this_ptr, vsf_pbuf_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    VSF_SERVICE_ASSERT(NULL != this.tFeature.u5REFCount < 32);

    this.tFeature.u5REFCount++;
}
#endif

vsf_pbuf_t * vsf_pbuf_free(vsf_pbuf_t *pbuf)
{
    class_internal(pbuf, this_ptr, vsf_pbuf_t);
    vsf_pbuf_t *ptReturn = pbuf;
    do {
        if(NULL == pbuf) {      //!< allow free NULL item.
            break;
        }        
    #if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
        if (this.tFeature.u5REFCount) {
            this.tFeature.u5REFCount--;
            break;
        }
    #endif
        uint_fast8_t u8_id = this.u8_adapter_id;
        
        if (VSF_PBUF_NO_FREE == u8_id) {
            ptReturn = NULL;
            break;
        }
        const vsf_pbuf_adapter_t *padatper = __vsf_get_adapter_interface(pbuf);
        if (NULL == padatper) {
            break;
        }
        if (NULL != padatper->methods_ptr->Free) {
            ptReturn = (*padatper->methods_ptr->Free)(padatper->target_ptr, pbuf);
        }
    } while(false);
    
    return ptReturn;
}


void vsf_pbuf_pool_item_init_event_handler( uintptr_t target_ptr, 
                                            uintptr_t pbuf, 
                                            uint_fast32_t s32_size)
{
    VSF_SERVICE_ASSERT(NULL != target_ptr);
    vsf_pbuf_cfg_t cfg = {
        .buffer_ptr            = NULL,     //!< use the rest memory of the block as buffer
        .u24_block_size       = s32_size,    //!< total block s32_size
        .is_no_write          = false,    //!< allow write
        .is_no_read           = false,    //!< allow read
        .is_no_direct_access   = false,    //!< allow direct access
        .adapter_id          = ((vsf_pbuf_adapter_t *)target_ptr)->ID  //!< use default adapter
    };

    vsf_pbuf_init((vsf_pbuf_t *)pbuf, &cfg);
}

#endif
/* EOF */
