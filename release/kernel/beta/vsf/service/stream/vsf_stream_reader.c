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

#define __VSF_STREAM_READER_CLASS_IMPLEMENT
#include "./vsf_stream_base.h"
#include "./vsf_stream_reader.h"

/*============================ MACROS ========================================*/
#undef  this
#define this        (*ptThis)


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
const i_stream_reader_t  VSF_STREAM_READER = {
    .Init =             &vsf_stream_reader_init,
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    .Open =         (void(*)(vsf_stream_reader_t *))
                        &vsf_stream_usr_open,
    .Close =        (void(*)(vsf_stream_reader_t *))
                        &vsf_stream_usr_close,
#endif
    .Block = {
        .Fetch =        &vsf_stream_reader_fetch_pbuf,
    },
    .Read =             &vsf_stream_reader_read,
    .ReadByte =         &vsf_stream_reader_read_byte,

};

/*============================ IMPLEMENTATION ================================*/

/*----------------------------------------------------------------------------*
 * STREAM READER                                                              *
 *----------------------------------------------------------------------------*/

 
vsf_err_t vsf_stream_reader_init(   vsf_stream_reader_t *ptObj, 
                                    const vsf_stream_reader_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_stream_reader_t);
    ASSERT(NULL != ptObj);

    memset(ptObj, 0, sizeof(vsf_stream_reader_t));
    return vsf_stream_usr_init(&this.use_as__vsf_stream_usr_t, ptCFG);
}

vsf_pbuf_t *vsf_stream_reader_fetch_pbuf ( vsf_stream_reader_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_reader_t);
    vsf_pbuf_t *pbuf;
    ASSERT(NULL != ptObj);
    
    __SAFE_ATOM_CODE(
        pbuf = vsf_stream_usr_fetch_pbuf(&this.use_as__vsf_stream_usr_t);
        if (NULL == this.ptCurrent) {
            this.ptCurrent = vsf_stream_usr_fetch_pbuf(&this.use_as__vsf_stream_usr_t);
        } else {
            vsf_pbuf_t *ptTemp = pbuf;
            pbuf = this.ptCurrent;
            this.ptCurrent = ptTemp;
        }

        if (NULL != this.ptCurrent) {
            this.hwOffset = 0;
            this.hwBufferSize = 0;
        } 
    );

    return pbuf;
}
 

int_fast32_t vsf_stream_reader_read(vsf_stream_reader_t *ptObj,
                                    uint8_t *pchBuffer,
                                    uint_fast16_t hwSize)
{
    class_internal(ptObj, ptThis, vsf_stream_reader_t);
    vsf_pbuf_t *pbuf;
    int_fast32_t nReadSize = -1;
    ASSERT(NULL != ptObj);
    
    do {
        if (NULL == pchBuffer || 0 == hwSize) {
            break;
        }

        __SAFE_ATOM_CODE(
            if (NULL == this.ptCurrent) {
                this.ptCurrent = vsf_stream_usr_fetch_pbuf(&this.use_as__vsf_stream_usr_t);
                this.hwOffset = 0;
                this.hwBufferSize = 0;
            }
        )

        __SAFE_ATOM_CODE(
            if (NULL != this.ptCurrent) {
                if (this.hwOffset < this.hwBufferSize) {
                    nReadSize = vsf_pbuf_buffer_read(   this.ptCurrent, 
                                                        pchBuffer,
                                                        hwSize,
                                                        this.hwOffset);
                    
                    if (nReadSize > 0) {
                        this.hwOffset += nReadSize;
                        if (this.hwOffset == this.hwBufferSize) {
                            //! current buffer is full
                            vsf_pbuf_free(this.ptCurrent);                      //! free
                            this.ptCurrent = NULL;
                        }
                    }
                }
            }
        )
    
    } while(0);   

    return nReadSize;
}

bool vsf_stream_reader_read_byte(   vsf_stream_reader_t *ptObj, 
                                    uint8_t *pchByte)
{
    return vsf_stream_reader_read(ptObj, pchByte, 1);
}

#endif

/* EOF */
