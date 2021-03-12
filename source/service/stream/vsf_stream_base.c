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

#if VSF_USE_STREAM == ENABLED

#define __VSF_STREAM_BASE_CLASS_IMPLEMENT
#include "./vsf_stream.h"

#include "./pbuf/vsf_pbuf.h"

/*============================ MACROS ========================================*/
#undef  this
#define this    (*this_ptr)

#ifndef GENERAL_PBUF_POLL_PRIV_USER_COUNT
#   define GENERAL_PBUF_POLL_PRIV_USER_COUNT    0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
NO_INIT vsf_pool(general_pbuf_pool_t) g_tGeneralPBUFPool;
#endif

const i_stream_src_t VSF_STREAM_SRC = {
    .Init =             &vsf_stream_src_init,
    .GetTX =            &vsf_stream_src_get_tx,
    .Block = {
        .New =          &vsf_stream_src_new_pbuf,
        .Send =         &vsf_stream_src_send_pbuf,
    },
};


const i_stream_usr_t VSF_STREAM_USR = {  
    .Init =             &vsf_stream_usr_init,
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    .Open =             &vsf_stream_usr_open,
    .Close =            &vsf_stream_usr_close,
#endif
    .Block = {
        .Fetch =        &vsf_stream_usr_fetch_pbuf,
    },
};


/*============================ IMPLEMENTATION ================================*/

#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
implement_pbuf_pool(general_pbuf_pool_t)
#endif

/*----------------------------------------------------------------------------*
 * STREAM SOURCE                                                              *
 *----------------------------------------------------------------------------*/

vsf_stream_tx_t *vsf_stream_src_get_tx(vsf_stream_src_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_src_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    return this.ptTX;
}

vsf_err_t vsf_stream_src_init(  vsf_stream_src_t *obj_ptr, 
                                const vsf_stream_src_cfg_t *cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_src_t);
    vsf_err_t result = VSF_ERR_INVALID_PARAMETER;

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    VSF_SERVICE_ASSERT(     (NULL != this_ptr)
            &&  (NULL != cfg_ptr));
    if ( NULL == cfg_ptr->ptTX ) {
        return VSF_ERR_INVALID_PTR;
    }
    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptTX->piMethod );
    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptTX->piMethod->DataDrainEvent.Register );
    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptTX->piMethod->Send );

    this.use_as__vsf_stream_src_cfg_t = *cfg_ptr;

    do {
        if (NULL == this.tRequestPBUFEvent.handler_fn) {
    #if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
            //! using general pbuf pool
            this.tRequestPBUFEvent = vsf_pbuf_pool_req_pbuf_evt( general_pbuf_pool_t, 
                                                                &g_tGeneralPBUFPool);
    #   if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
            if (0 == this.hwpbufPoolReserve) {
                /*! \note non-privileged user of general pbuf pool will reserve 
                 *        specific count of pbuf for priviliged (0xFF) users 
                 *        by default (hwpbufPoolReserve equals 0x00)
                 */
                this.hwpbufPoolReserve = GENERAL_PBUF_POLL_PRIV_USER_COUNT;
            } else if (0xFF == this.hwpbufPoolReserve) {
                /*! \note privileged users(0xFF) should set hwpbufPoolReserve to
                 *        0xFF when using general pbuf pool
                 */
                this.hwpbufPoolReserve = 0;
            }
    #   endif
    #else
            break;
    #endif
        }
        result = VSF_ERR_NONE;
    } while(0);

    return result;
}

vsf_pbuf_t *vsf_stream_src_new_pbuf (      vsf_stream_src_t *obj_ptr,
                                           int_fast32_t nNoLessThan,
                                           int_fast32_t nBestSize)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_src_t);
    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    vsf_pbuf_t *pbuf = NULL;
    do {
        if ( NULL == this.ptTX ) {
            break;
        }
        VSF_SERVICE_ASSERT( NULL != this.ptTX->piMethod );
        VSF_SERVICE_ASSERT( NULL != this.ptTX->piMethod->GetStatus );
        vsf_stream_status_t Status = this.ptTX->piMethod->GetStatus(this.ptTX);
        UNUSED_PARAM(Status);
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        if (!Status.IsOpen) {
            break;
        }
    #endif
    #if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
        //! check uplimit
        if (   (Status.u14Count >= this.hwpbufCountUpLimit)
            && (this.hwpbufCountUpLimit != 0)) {
            break;
        }
    #endif
        VSF_SERVICE_ASSERT(NULL != this.tRequestPBUFEvent.handler_fn);
        pbuf =  this.tRequestPBUFEvent.handler_fn(this.tRequestPBUFEvent.target_ptr, 
                                                nNoLessThan, 
                                                nBestSize,
    #if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
                                                this.hwpbufPoolReserve
    #else
                                                0
    #endif
                                                );
        if (NULL != pbuf) {
            vsf_pbuf_size_reset(pbuf);
        }
    } while(0);
    return pbuf;
}

#if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
vsf_err_t vsf_stream_src_set_limitation(    vsf_stream_src_t *obj_ptr,
                                            uint_fast16_t hwpbufCountUpLimit,
                                            uint_fast16_t hwpbufPoolReserve)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_src_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    
    this.hwpbufCountUpLimit = hwpbufCountUpLimit;
    this.hwpbufPoolReserve = hwpbufPoolReserve;
    return VSF_ERR_NONE;
}
#endif

vsf_err_t vsf_stream_src_send_pbuf (vsf_stream_src_t *obj_ptr, 
                                    vsf_pbuf_t *ptOldBlock)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_src_t);

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    if (NULL == this.ptTX) {
        return VSF_ERR_INVALID_PTR;
    }

    VSF_SERVICE_ASSERT(NULL != this.ptTX->piMethod);

    if (NULL == ptOldBlock) {
        return VSF_ERR_INVALID_PTR;
    }

    //! write stream
    return this.ptTX->piMethod->Send(this.ptTX, ptOldBlock);
}

/*----------------------------------------------------------------------------*
 * STREAM USER                                                                *
 *----------------------------------------------------------------------------*/

vsf_err_t vsf_stream_usr_init(  vsf_stream_usr_t *obj_ptr, 
                                const vsf_stream_usr_cfg_t *cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_usr_t);
    vsf_err_t result = VSF_ERR_INVALID_PARAMETER;

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    VSF_SERVICE_ASSERT(     (NULL != this_ptr)
            &&  (NULL != cfg_ptr));
    if ( NULL == cfg_ptr->ptRX ) {
        return VSF_ERR_INVALID_PTR;
    }

    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptRX->piMethod );
    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptRX->piMethod->DataReadyEvent.Register );
    VSF_SERVICE_ASSERT( NULL != cfg_ptr->ptRX->piMethod->Fetch );

    do {
        this.ptRX = cfg_ptr->ptRX;
        /*
        this.ptRX->piMethod->DataReadyEvent.Register(
                                                this.ptRX, 
                                                cfg_ptr->tDataReadyEventHandling);
        */
    } while(0);
 
    return result;
}


vsf_pbuf_t *vsf_stream_usr_fetch_pbuf ( vsf_stream_usr_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_usr_t);

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    if (NULL == this.ptRX) {
        return NULL;
    }
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod);
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod->Fetch);

    //! read stream
    return this.ptRX->piMethod->Fetch(this.ptRX);
}

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
void vsf_stream_usr_open ( vsf_stream_usr_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_usr_t);

    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    if (NULL == this.ptRX) {
        return ;
    }
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod);
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod->Open);

    this.ptRX->piMethod->Open(this.ptRX);
}

void vsf_stream_usr_close ( vsf_stream_usr_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_usr_t);

    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    if (NULL == this.ptRX) {
        return ;
    }
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod);
    VSF_SERVICE_ASSERT(NULL != this.ptRX->piMethod->Close);

    this.ptRX->piMethod->Close(this.ptRX);
}

#endif
#endif

/* EOF */
