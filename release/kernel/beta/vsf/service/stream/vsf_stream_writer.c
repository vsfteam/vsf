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

#define __VSF_STREAM_WRITER_CLASS_IMPLEMENT
#include "./vsf_stream_base.h"
#include "./vsf_stream_writer.h"

/*============================ MACROS ========================================*/
#undef  this
#define this        (*this_ptr)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const i_stream_writer_t  VSF_STREAM_WRITER = {
    .Init =         &vsf_stream_writer_init,
    .GetTX =        (vsf_stream_tx_t *(*) (vsf_stream_writer_t *))   
                        &vsf_stream_src_get_tx,
    .Block = {
        .New =      (vsf_pbuf_t *(*)(vsf_stream_writer_t *,int_fast32_t,int_fast32_t))
                        &vsf_stream_src_new_pbuf,
        .Send =         &vsf_stream_writer_send_pbuf,
    }, 


    .Write =            &vsf_stream_writer_write,
    .WriteByte =        &vsf_stream_writer_write_byte,
    .Flush =            &vsf_stream_writer_flush,
 
};


/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * STREAM WRITER                                                              *
 *----------------------------------------------------------------------------*/
vsf_err_t vsf_stream_writer_init(   vsf_stream_writer_t *obj_ptr, 
                                    const vsf_stream_src_cfg_t *cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_writer_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);

    memset(obj_ptr, 0, sizeof(vsf_stream_writer_t));
    return vsf_stream_src_init(&this.use_as__vsf_stream_src_t, cfg_ptr);
}

static void __vsf_stream_writer_send_current_pbuf(vsf_stream_writer_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_writer_t);
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

vsf_err_t vsf_stream_writer_send_pbuf ( vsf_stream_writer_t *obj_ptr, 
                                        vsf_pbuf_t *ptOldBlock)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_writer_t);

    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    
    __vsf_stream_writer_send_current_pbuf(obj_ptr);

    return vsf_stream_src_send_pbuf(&this.use_as__vsf_stream_src_t, 
                                        ptOldBlock);
}

int_fast32_t vsf_stream_writer_write(   vsf_stream_writer_t *obj_ptr,
                                        uint8_t *buffer_ptr,
                                        uint_fast16_t u16_size)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_writer_t);
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    int_fast32_t nWrittenSize = -1;
    

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
        if (NULL == buffer_ptr || 0 == u16_size) {
            break; 
        }

        __SAFE_ATOM_CODE(
            if (NULL != this.ptCurrent) {
                if (this.hwOffset < this.hwBufferSize) {
                    nWrittenSize = vsf_pbuf_buffer_write(  this.ptCurrent, 
                                                            buffer_ptr,
                                                            u16_size,
                                                            this.hwOffset);
                    if (nWrittenSize > 0) {
                        this.hwOffset += nWrittenSize;
                        if (this.hwOffset == this.hwBufferSize) {
                            //! current buffer is full
                            __vsf_stream_writer_send_current_pbuf(obj_ptr);
                        }
                    }
                }
            }
        )

    } while(0);

    return nWrittenSize;
}

bool vsf_stream_writer_write_byte(  vsf_stream_writer_t *obj_ptr, 
                                    uint_fast8_t chByte)
{
    return vsf_stream_writer_write(obj_ptr, (uint8_t *)&chByte, 1);
}

fsm_rt_t vsf_stream_writer_flush (vsf_stream_writer_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_writer_t);
    fsm_rt_t tReturn;
    VSF_SERVICE_ASSERT(NULL != obj_ptr);
    

    if (NULL != this.ptCurrent) {
        __vsf_stream_writer_send_current_pbuf(obj_ptr);
    }
    
    do {
        vsf_stream_tx_t *ptTX = vsf_stream_src_get_tx(&this.use_as__vsf_stream_src_t);
    
        tReturn = ptTX->piMethod->GetStatus(ptTX).IsDataDrain ? 
            fsm_rt_cpl 
        :   fsm_rt_on_going;
    
    } while(0);

    return tReturn;
}

#endif

/* EOF */
