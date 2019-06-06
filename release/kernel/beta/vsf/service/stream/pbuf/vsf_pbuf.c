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
#define this    (*ptThis)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static struct {
    const vsf_pbuf_adapter_t *padapters;
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
    class_internal(ptItem, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptThis);
    const vsf_pbuf_adapter_t *ptAdapter = NULL;
    
    do {
        uint_fast8_t chID = this.u8AdapterID;
        
        if (0xFF == chID) {
            break;
        } /*else if (VSF_PBUF_FREE_TO_ANY == chID) {
            chID = 1;
        }  */
        
        if (NULL == __vsf_pbuf_cb.padapters) {
            break;
        }
        if (chID >= __vsf_pbuf_cb.length) {
            break;
        }
        
        ptAdapter = &__vsf_pbuf_cb.padapters[chID];
        ASSERT(NULL != ptAdapter->piMethods);
    } while(0);
    
    return ptAdapter;
}

void vsf_adapter_register(const vsf_pbuf_adapter_t *ptAdaptors, uint_fast8_t chSize)
{
    //!< this function could only be called once.
    ASSERT(__vsf_pbuf_cb.padapters == NULL);   
    
    __vsf_pbuf_cb.padapters = ptAdaptors;
    __vsf_pbuf_cb.length = chSize;
}

const vsf_pbuf_adapter_t *vsf_pbuf_adapter_get(uint_fast8_t chID)
{
    const vsf_pbuf_adapter_t *ptAdapter = NULL;
    
    /*if (VSF_PBUF_FREE_TO_ANY == chID) {
        ptAdapter =  __vsf_pbuf_cb.padapters;
    } else */
    if (chID < __vsf_pbuf_cb.length) {
        ptAdapter =  &__vsf_pbuf_cb.padapters[chID];
    }

    return ptAdapter;
}

vsf_pbuf_t *vsf_pbuf_init(vsf_pbuf_t *pbuf, vsf_pbuf_cfg_t *pcfg)
{
    class_internal(pbuf, ptThis, vsf_pbuf_t);
    ASSERT(NULL != pbuf && NULL != pcfg);
    
    do {
        if (    (pcfg->u24BlockSize < sizeof(this)) 
            &&  (NULL == pcfg->pBuffer)) {
            break;
        }

        this.Capability = pcfg->Capability;

        if (NULL != pcfg->pBuffer) {
            this.pchBuffer = pcfg->pBuffer;
            this.u24Size = pcfg->u24BlockSize;
        } else {
            this.pchBuffer = ((uint8_t *)&this)+sizeof(this);
            this.u24Size = pcfg->u24BlockSize - sizeof(this);
            this.u24BlockSize = this.u24Size;
        }

        this.u8AdapterID = pcfg->AdapterID;
    } while(false);
    
    return pbuf;
}

void vsf_pbuf_size_reset(vsf_pbuf_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    
    ASSERT(NULL != ptThis);
    if (!this.isNoWrite) {      //!< readonly
        this.u24Size = this.u24BlockSize;
    }
}

void *vsf_pbuf_buffer_get(vsf_pbuf_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptThis);
#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
    if (this.isNoDirectAccess) {
        return NULL;
    }
#endif
    return this.pchBuffer;
}


int_fast32_t vsf_pbuf_buffer_write( vsf_pbuf_t *ptObj, 
                            const void *psrc, 
                            int_fast32_t nSize, 
                            uint_fast32_t offsite)
{
    ASSERT(NULL != ptObj && NULL != psrc);
    
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    int_fast32_t nWrittenSize = -1;
    do {
        if (0 == nSize) {
            break;
        } else if (this.isNoWrite) {
            break;
        }
        
        uint_fast32_t wMaxSize = this.u24BlockSize - offsite;
        nSize = min(nSize, wMaxSize);

#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
        if (this.isNoDirectAccess) {
            const vsf_pbuf_adapter_t *
                ptAdapter = __vsf_get_adapter_interface(ptObj);
            if (NULL == ptAdapter) {
                break;
            }
            if (NULL == ptAdapter->piMethods->Write) {
                break;
            }
            
            nSize = (*ptAdapter->piMethods->Write)(ptAdapter->ptTarget, 
                                                  this.pchBuffer,
                                                  psrc,
                                                  nSize,
                                                  offsite);
            if (nSize < 0) {
                break;
            }

        } else 
#endif
        {
            memcpy(((uint8_t *)vsf_pbuf_buffer_get(ptObj)) + offsite, psrc, nSize);
        }
        
        this.u24Size = nSize + offsite;
        nWrittenSize = nSize;
    } while(false);
    
    return nWrittenSize;
}

int_fast32_t vsf_pbuf_buffer_read( vsf_pbuf_t *ptObj, 
                                void *psrc, 
                                int_fast32_t nSize, 
                                uint_fast32_t wOffset)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptObj && NULL != psrc);
    //uint8_t *pbuffer = NULL;
    //vsf_mem_t tResult = {.nSize = -1,};
    int_fast32_t nReadSize = -1;
    do {
        if (0 == nSize) {
            break;
        } else if (this.isNoRead) {
            break;
        }
        
        uint_fast32_t max_size = this.u24BlockSize - wOffset;
#if VSF_PBUF_CFG_INDIRECT_RW_SUPPORT == ENABLED
        if (this.isNoDirectAccess) {
            nSize = min(nSize, max_size);
            
            const vsf_pbuf_adapter_t *
                ptAdapter = __vsf_get_adapter_interface(ptObj);
            if (NULL == ptAdapter) {
                break;
            }
            if (NULL == ptAdapter->piMethods->Read) {
                break;
            }
            
            nSize = (*ptAdapter->piMethods->Read)(   ptAdapter->ptTarget, 
                                                    this.pchBuffer,
                                                    psrc,
                                                    nSize,
                                                    wOffset);
            if (nSize < 0) {
                break;
            }
            //pbuffer = psrc;

        } else 
#endif
        {
        //if (nSize <= max_size) {
        //    pbuffer = (uint8_t *)vsf_pbuf_buffer_get(ptObj) + wOffset;
        //} else {
            nSize = min(nSize, max_size);
            memcpy(psrc, this.pchBuffer + wOffset, nSize);
            //pbuffer = psrc;
        //}
        }
        
        //tResult.pchBuffer = pbuffer;
        nReadSize = nSize;
        
    } while(false);
    
    return nReadSize;
}


void vsf_pbuf_size_set(vsf_pbuf_t *ptObj, int_fast32_t nSize)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptObj);
    
    if (this.isNoWrite) {            //! readonly
        return ;
    }
    
    this.u24Size = min(nSize, this.u24BlockSize);
}

int_fast32_t vsf_pbuf_capacity_get(vsf_pbuf_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptObj);
    
    return this.u24BlockSize;
}

int_fast32_t vsf_pbuf_size_get(vsf_pbuf_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptObj);

    return this.u24Size;
}

vsf_pbuf_capability_t vsf_pbuf_capability_get(vsf_pbuf_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptObj);

    return this.tFeature;
}
#if VSF_PBUF_CFG_SUPPORT_REF_COUNTING == ENABLED
void vsf_pbuf_ref_increase(vsf_pbuf_t *pbuf)
{
    class_internal(pbuf, ptThis, vsf_pbuf_t);
    ASSERT(NULL != ptThis);
    ASSERT(NULL != this.tFeature.u5REFCount < 32);

    this.tFeature.u5REFCount++;
}
#endif

vsf_pbuf_t * vsf_pbuf_free(vsf_pbuf_t *pbuf)
{
    class_internal(pbuf, ptThis, vsf_pbuf_t);
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
        uint_fast8_t chID = this.u8AdapterID;
        
        if (VSF_PBUF_NO_FREE == chID) {
            ptReturn = NULL;
            break;
        }
        const vsf_pbuf_adapter_t *padatper = __vsf_get_adapter_interface(pbuf);
        if (NULL == padatper) {
            break;
        }
        if (NULL != padatper->piMethods->Free) {
            ptReturn = (*padatper->piMethods->Free)(padatper->ptTarget, pbuf);
        }
    } while(false);
    
    return ptReturn;
}


void vsf_pbuf_pool_item_init_event_handler( void *ptTarget, 
                                            void *pbuf, 
                                            uint_fast32_t nSize)
{
    ASSERT(NULL != ptTarget);
    vsf_pbuf_cfg_t cfg = {
        NULL,                   //!< use the rest memory of the block as buffer
        nSize,                   //!< total block nSize
        false,                  //!< allow write
        false,                  //!< allow read
        false,                  //!< allow direct access
        ((vsf_pbuf_adapter_t *)ptTarget)->ID  //!< use default adapter
    };

    vsf_pbuf_init((vsf_pbuf_t *)pbuf, &cfg);
}

#endif
/* EOF */
