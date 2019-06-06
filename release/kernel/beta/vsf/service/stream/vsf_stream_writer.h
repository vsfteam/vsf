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

#ifndef __VSF_SERVICE_STREAM_WRITER_H__
#define __VSF_SERVICE_STREAM_WRITER_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#include "./pbuf/vsf_pbuf.h"
#include "./pbuf/vsf_pbuf_pool.h"

#if VSF_USE_SERVICE_STREAM == ENABLED
#include "./interface.h"
#include "../pool/vsf_pool.h"
#include "./pbuf_queue/vsf_pbuf_queue.h"
#include "./vsf_stream_base.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_STREAM_WRITER_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_STREAM_WRITER_CLASS_IMPLEMENT
#elif   defined(__VSF_STREAM_WRITER_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_STREAM_WRITER_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*----------------------------------------------------------------------------*
 * STREAM WRITER                                                              *
 *----------------------------------------------------------------------------*/

declare_class(vsf_stream_writer_t);

def_class(vsf_stream_writer_t,
    which(
        implement(vsf_stream_src_t)
    ),
    private_member(
        vsf_pbuf_t *ptCurrent;
        uint16_t    hwOffset;
        uint16_t    hwBufferSize;
    )
)
end_def_class(vsf_stream_writer_t)

def_interface(i_stream_writer_t)
    vsf_err_t           (*Init)     (   vsf_stream_writer_t *ptObj, 
                                        const vsf_stream_src_cfg_t *ptCFG);
    vsf_stream_tx_t     *(*GetTX)   (   vsf_stream_writer_t *ptObj);

    struct {
        vsf_pbuf_t *    (*New)      (   vsf_stream_writer_t *ptObj,
                                        int_fast32_t nNoLessThan,
                                        int_fast32_t nBestSize);
        vsf_err_t       (*Send)     (   vsf_stream_writer_t *ptObj,
                                        vsf_pbuf_t *ptOldBlock);
    }Block;


    bool                (*WriteByte)(   vsf_stream_writer_t *ptObj, 
                                        uint_fast8_t chByte);
    int_fast32_t        (*Write)    (   vsf_stream_writer_t *ptObj,
                                        uint8_t *pchBuffer,
                                        uint_fast16_t hwSize);
    fsm_rt_t            (*Flush)    (   vsf_stream_writer_t *ptObj);

end_def_interface(i_stream_writer_t)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_stream_writer_t  VSF_STREAM_WRITER;
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * STREAM WRITER                                                              *
 *----------------------------------------------------------------------------*/
extern
vsf_err_t vsf_stream_writer_init(   vsf_stream_writer_t *ptObj, 
                                    const vsf_stream_src_cfg_t *ptCFG);

extern
vsf_err_t vsf_stream_writer_send_pbuf ( vsf_stream_writer_t *ptObj, 
                                        vsf_pbuf_t *ptOldBlock);

extern 
int_fast32_t vsf_stream_writer_write(   vsf_stream_writer_t *ptObj,
                                        uint8_t *pchBuffer,
                                        uint_fast16_t hwSize);

extern
bool vsf_stream_writer_write_byte(  vsf_stream_writer_t *ptObj, 
                                    uint_fast8_t chByte);

extern
fsm_rt_t vsf_stream_writer_flush (vsf_stream_writer_t *ptObj);

#endif
#endif
/* EOF */
