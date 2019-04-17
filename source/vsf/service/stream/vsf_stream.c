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

#if VSF_USE_SERVICE_STREAM == ENABLED

#define __VSF_STREAM_CLASS_IMPLEMENT
#include "./vsf_stream.h"

#include "./pbuf/vsf_pbuf.h"

/*============================ MACROS ========================================*/
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

const i_stream_writer_t  VSF_STREAM_WRITER = {
    .Init =         &vsf_stream_writer_init,
    .GetTX =        (vsf_stream_tx_t *(*) (vsf_stream_writer_t *))   
                        &vsf_stream_src_get_tx,
    .Block = {
        .New =      (vsf_pbuf_t *(*)(vsf_stream_writer_t *,int_fast32_t,int_fast32_t))
                        &vsf_stream_src_new_pbuf,
        .Send =         &vsf_stream_writer_send_pbuf,
    }, 

    .Stream = {
        .Write =        &vsf_stream_writer_write,
        .WriteByte =    &vsf_stream_writer_write_byte,
        .Flush =        &vsf_stream_writer_flush,
    },
};

extern const i_stream_usr_t VSF_STREAM_USR = {  
    .Init =             &vsf_stream_usr_init,
    .Block = {
        .Fetch =        &vsf_stream_usr_fetch_pbuf,
    },
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
implement_pbuf_pool(general_pbuf_pool_t)
#endif

void vsf_service_stream_init(void)
{
#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
    //! initialise pbuf pool
    init_pbuf_pool( general_pbuf_pool_t, 
                    &g_tGeneralPBUFPool,
                    0,   
                    GENERAL_PBUF_POOL_BLOCK_COUNT);
#endif
}

/*----------------------------------------------------------------------------*
 * STREAM SOURCE                                                              *
 *----------------------------------------------------------------------------*/

vsf_stream_tx_t *vsf_stream_src_get_tx(vsf_stream_src_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_src_t);
    ASSERT(NULL != ptThis);

    return this.ptTX;
}

vsf_err_t vsf_stream_src_init(  vsf_stream_src_t *ptObj, 
                                const vsf_stream_src_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_stream_src_t);
    vsf_err_t tResult = VSF_ERR_INVALID_PARAMETER;

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    ASSERT(     (NULL != ptThis)
            &&  (NULL != ptCFG));
    if ( NULL == ptCFG->ptTX ) {
        return VSF_ERR_INVALID_PTR;
    }
    ASSERT( NULL != ptCFG->ptTX->piMethod );
    ASSERT( NULL != ptCFG->ptTX->piMethod->DataDrainEvent.Register );
    ASSERT( NULL != ptCFG->ptTX->piMethod->Send );

    this.use_as__vsf_stream_src_cfg_t = *ptCFG;

    do {
        if (NULL == this.tRequestPBUFEvent.fnHandler) {
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
        tResult = VSF_ERR_NONE;
    } while(0);

    return tResult;
}

vsf_pbuf_t *vsf_stream_src_new_pbuf (      vsf_stream_src_t *ptObj,
                                           int_fast32_t nNoLessThan,
                                           int_fast32_t nBestSize)
{
    class_internal(ptObj, ptThis, vsf_stream_src_t);
    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    ASSERT(NULL != ptObj);
    do {
        if ( NULL == this.ptTX ) {
            break;
        }
        ASSERT( NULL != this.ptTX->piMethod );
        ASSERT( NULL != this.ptTX->piMethod->GetStatus );
        vsf_stream_status_t tStatus = this.ptTX->piMethod->GetStatus(this.ptTX);
        UNUSED_PARAM(tStatus);
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        if (!tStatus.IsOpen) {
            break;
        }
    #endif
    #if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
        //! check uplimit
        if (   (tStatus.u14Count >= this.hwpbufCountUpLimit)
            && (this.hwpbufCountUpLimit != 0)) {
            break;
        }
    #endif
        ASSERT(NULL != this.tRequestPBUFEvent.fnHandler);
        return this.tRequestPBUFEvent.fnHandler(this.tRequestPBUFEvent.pTarget, 
                                                nNoLessThan, 
                                                nBestSize,
    #if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
                                                this.hwpbufPoolReserve
    #else
                                                0
    #endif
                                                );
    } while(0);
    return NULL;
}

#if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
vsf_err_t vsf_stream_src_set_limitation(    vsf_stream_src_t *ptObj,
                                            uint_fast16_t hwpbufCountUpLimit,
                                            uint_fast16_t hwpbufPoolReserve)
{
    class_internal(ptObj, ptThis, vsf_stream_src_t);
    ASSERT(NULL != ptObj);
    
    this.hwpbufCountUpLimit = hwpbufCountUpLimit;
    this.hwpbufPoolReserve = hwpbufPoolReserve;
    return VSF_ERR_NONE;
}
#endif

vsf_err_t vsf_stream_src_send_pbuf (  vsf_stream_src_t *ptObj, 
                                            vsf_pbuf_t *ptOldBlock)
{
    class_internal(ptObj, ptThis, vsf_stream_src_t);

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    ASSERT(NULL != ptObj);
    if (NULL == this.ptTX) {
        return VSF_ERR_INVALID_PTR;
    }

    ASSERT(NULL != this.ptTX->piMethod);

    if (NULL == ptOldBlock) {
        return VSF_ERR_INVALID_PTR;
    }

    //! write stream
    this.ptTX->piMethod->Send(this.ptTX, ptOldBlock);
    
    return VSF_ERR_NONE;
}

vsf_err_t vsf_stream_writer_init(   vsf_stream_writer_t *ptObj, 
                                    const vsf_stream_src_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_stream_writer_t);
    ASSERT(NULL != ptObj);

    memset(ptObj, 0, sizeof(vsf_stream_writer_t));
    return vsf_stream_src_init(&this.use_as__vsf_stream_src_t, ptCFG);
}

static void __vsf_stream_writer_send_current_pbuf(vsf_stream_writer_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_writer_t);
    vsf_pbuf_t *ptPBUF = NULL;
    
    __SAFE_ATOM_CODE(
        if (0 != this.hwOffset) {
            ptPBUF = this.ptCurrent;
            this.ptCurrent = NULL;
        }
    );

    if (NULL != ptPBUF) {
        //! set size
        //vsf_pbuf_size_set(  ptPBUF, this.hwOffset);
        //! write byte fifo pbuf first
        vsf_stream_src_send_pbuf(   &this.use_as__vsf_stream_src_t, 
                                     ptPBUF);
    }
}

vsf_err_t vsf_stream_writer_send_pbuf ( vsf_stream_writer_t *ptObj, 
                                        vsf_pbuf_t *ptOldBlock)
{
    class_internal(ptObj, ptThis, vsf_stream_writer_t);

    ASSERT(NULL != ptObj);
    
    __vsf_stream_writer_send_current_pbuf(ptObj);

    return vsf_stream_src_send_pbuf(&this.use_as__vsf_stream_src_t, 
                                        ptOldBlock);
}

int_fast32_t vsf_stream_writer_write(   vsf_stream_writer_t *ptObj,
                                        uint8_t *pchBuffer,
                                        uint_fast16_t hwSize)
{
    class_internal(ptObj, ptThis, vsf_stream_writer_t);
    ASSERT(NULL != ptObj);
    uint_fast16_t hwWrittenSize = 0;
    

    __SAFE_ATOM_CODE(
        if (NULL == this.ptCurrent) {
            this.ptCurrent = vsf_stream_src_new_pbuf(
                                &this.use_as__vsf_stream_src_t, -1, -1);
            if (NULL != this.ptCurrent) {
                this.hwOffset = 0;
                this.hwBufferSize = vsf_pbuf_size_get(this.ptCurrent);
            } 
        }
    )
    do {
        if (NULL == pchBuffer || 0 == hwSize) {
            break; 
        }

        if (NULL == this.ptCurrent) {
            break;
        }
        
        __SAFE_ATOM_CODE(
            if (this.hwOffset < this.hwBufferSize) {
                hwWrittenSize = vsf_pbuf_buffer_write(  this.ptCurrent, 
                                                        pchBuffer,
                                                        hwSize,
                                                        this.hwOffset);
                this.hwOffset += hwWrittenSize;
                if (this.hwOffset == this.hwBufferSize) {
                    //! current buffer is full
                    __vsf_stream_writer_send_current_pbuf(ptObj);
                }
            }
        )

    } while(0);

    return hwWrittenSize;
}

bool vsf_stream_writer_write_byte(  vsf_stream_writer_t *ptObj, 
                                    uint_fast8_t chByte)
{
    return vsf_stream_writer_write(ptObj, (uint8_t *)&chByte, 1);
}

fsm_rt_t vsf_stream_writer_flush (vsf_stream_writer_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_writer_t);
    fsm_rt_t tReturn;
    ASSERT(NULL != ptObj);
    

    if (NULL != this.ptCurrent) {
        __vsf_stream_writer_send_current_pbuf(ptObj);
    }
    
    with_class(vsf_stream_src_t, &this.use_as__vsf_stream_src_t, {
        tReturn = _->ptTX->piMethod->GetStatus(_->ptTX).IsDataDrain ? 
            fsm_rt_cpl 
        :   fsm_rt_on_going;
    });

    return tReturn;
}

/*----------------------------------------------------------------------------*
 * STREAM USER                                                                *
 *----------------------------------------------------------------------------*/

vsf_err_t vsf_stream_usr_init(  vsf_stream_usr_t *ptObj, 
                                const vsf_stream_usr_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_stream_usr_t);
    vsf_err_t tResult = VSF_ERR_INVALID_PARAMETER;

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    ASSERT(     (NULL != ptThis)
            &&  (NULL != ptCFG));
    if ( NULL == ptCFG->ptRX ) {
        return VSF_ERR_INVALID_PTR;
    }

    ASSERT( NULL != ptCFG->ptRX->piMethod );
    ASSERT( NULL != ptCFG->ptRX->piMethod->DataReadyEvent.Register );
    ASSERT( NULL != ptCFG->ptRX->piMethod->Fetch );

    do {
        this.ptRX = ptCFG->ptRX;
        /*
        this.ptRX->piMethod->DataReadyEvent.Register(
                                                this.ptRX, 
                                                ptCFG->tDataReadyEventHandling);
        */
    } while(0);
 
    return tResult;
}


vsf_pbuf_t *vsf_stream_usr_fetch_pbuf ( vsf_stream_usr_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_usr_t);

    /*! \note check them seperately to avoid busfault, in order to give the right 
              diagnosis info
     */
    ASSERT(NULL != ptObj);
    if (NULL == this.ptRX) {
        return NULL;
    }
    ASSERT(NULL != this.ptRX->piMethod);
    ASSERT(NULL != this.ptRX->piMethod->Fetch);

    //! read stream
    return this.ptRX->piMethod->Fetch(this.ptRX);
}

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
void vsf_stream_usr_open ( vsf_stream_usr_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_usr_t);

    ASSERT(NULL != ptObj);
    if (NULL == this.ptRX) {
        return ;
    }
    ASSERT(NULL != this.ptRX->piMethod);
    ASSERT(NULL != this.ptRX->piMethod->Open);

    this.ptRX->piMethod->Open(this.ptRX);
}

void vsf_stream_usr_close ( vsf_stream_usr_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_usr_t);

    ASSERT(NULL != ptObj);
    if (NULL == this.ptRX) {
        return ;
    }
    ASSERT(NULL != this.ptRX->piMethod);
    ASSERT(NULL != this.ptRX->piMethod->Close);

    this.ptRX->piMethod->Close(this.ptRX);
}
#endif
#endif

/* EOF */
