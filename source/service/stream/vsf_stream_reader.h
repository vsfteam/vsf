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

#ifndef __VSF_SERVICE_STREAM_READER_H__
#define __VSF_SERVICE_STREAM_READER_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#include "./pbuf/vsf_pbuf.h"
#include "./pbuf/vsf_pbuf_pool.h"

#if VSF_USE_STREAM == ENABLED
#include "./interface.h"
#include "../pool/vsf_pool.h"
#include "./pbuf_queue/vsf_pbuf_queue.h"
#include "./vsf_stream_base.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_STREAM_READER_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_STREAM_READER_CLASS_IMPLEMENT
#elif   defined(__VSF_STREAM_READER_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_STREAM_READER_CLASS_INHERIT__
#endif   

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*----------------------------------------------------------------------------*
 * STREAM READER                                                              *
 *----------------------------------------------------------------------------*/
declare_class(vsf_stream_reader_t);

def_class(vsf_stream_reader_t,
    which(
        implement(vsf_stream_usr_t)
    ),
    private_member(
        vsf_pbuf_t *current_ptr;
        uint16_t    hwOffset;
        uint16_t    hwBufferSize;
    )
)
end_def_class(vsf_stream_reader_t)

typedef vsf_stream_usr_cfg_t vsf_stream_reader_cfg_t;

def_interface(i_stream_reader_t)
    vsf_err_t           (*Init)    (vsf_stream_reader_t *obj_ptr, 
                                    const vsf_stream_reader_cfg_t *cfg_ptr);
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    void                (*Open)(    vsf_stream_reader_t *obj_ptr);
    void                (*Close)(   vsf_stream_reader_t *obj_ptr);
#endif
    struct {
        vsf_pbuf_t *    (*Fetch)   (vsf_stream_reader_t *obj_ptr);
    }Block;

    bool            (*ReadByte)(vsf_stream_reader_t *obj_ptr, 
                                uint8_t *byte_ptr);
    int_fast32_t    (*Read)    (vsf_stream_reader_t *obj_ptr,
                                uint8_t *buffer_ptr,
                                uint_fast16_t u16_size);
end_def_interface(i_stream_reader_t)

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_stream_reader_t  VSF_STREAM_READER;
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * STREAM READER                                                              *
 *----------------------------------------------------------------------------*/

extern 
vsf_err_t vsf_stream_reader_init(   vsf_stream_reader_t *obj_ptr, 
                                    const vsf_stream_reader_cfg_t *cfg_ptr);

extern 
vsf_pbuf_t *vsf_stream_reader_fetch_pbuf ( vsf_stream_reader_t *obj_ptr);

extern 
int_fast32_t vsf_stream_reader_read(vsf_stream_reader_t *obj_ptr,
                                    uint8_t *buffer_ptr,
                                    uint_fast16_t u16_size);

extern
bool vsf_stream_reader_read_byte(   vsf_stream_reader_t *obj_ptr, 
                                    uint8_t *byte_ptr);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
