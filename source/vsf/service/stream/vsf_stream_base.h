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

#ifndef __VSF_SERVICE_STREAM_BASE_H__
#define __VSF_SERVICE_STREAM_BASE_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#include "./pbuf/vsf_pbuf.h"
#include "./pbuf/vsf_pbuf_pool.h"

#if VSF_USE_SERVICE_STREAM == ENABLED
#include "./interface.h"
#include "../pool/vsf_pool.h"
#include "./pbuf_queue/vsf_pbuf_queue.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_STREAM_BASE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_STREAM_BASE_CLASS_IMPLEMENT
#elif   defined(__VSF_STREAM_BASE_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_STREAM_BASE_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef GENERAL_PBUF_POOL_BLOCK_SIZE
#   define GENERAL_PBUF_POOL_BLOCK_SIZE        64
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_pbuf_pool(general_pbuf_pool_t)
def_pbuf_pool(general_pbuf_pool_t, GENERAL_PBUF_POOL_BLOCK_SIZE)

/*----------------------------------------------------------------------------*
 * STREAM SOURCE                                                              *
 *----------------------------------------------------------------------------*/
declare_class(vsf_stream_src_t);

typedef struct {
    vsf_stream_tx_t             *ptTX;
    req_pbuf_evt_t              tRequestPBUFEvent;
#if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
    uint16_t                    hwpbufCountUpLimit;
    uint16_t                    hwpbufPoolReserve;
#endif
}vsf_stream_src_cfg_t;

def_class(vsf_stream_src_t,,
    protected_member(
        implement(vsf_stream_src_cfg_t);
/*
        vsf_stream_tx_t         *ptTX;
        req_pbuf_evt_t          tRequestPBUFEvent;
        uint8_t                 chpbufCountUpLimit;
        uint8_t                 chpbufPoolReserve;
*/
    )
)
end_def_class(vsf_stream_src_t)

def_interface(i_stream_src_t)
    vsf_err_t           (*Init) (   vsf_stream_src_t *ptObj, 
                                    const vsf_stream_src_cfg_t *ptCFG);
    vsf_stream_tx_t *   (*GetTX)(   vsf_stream_src_t *ptObj);

    struct {
        vsf_pbuf_t *    (*New)  (   vsf_stream_src_t *ptObj,
                                    int_fast32_t nNoLessThan,
                                    int_fast32_t nBestSize);
        vsf_err_t       (*Send) (   vsf_stream_src_t *ptObj,vsf_pbuf_t *ptOldBlock);
    }Block;

end_def_interface(i_stream_src_t)

/*----------------------------------------------------------------------------*
 * STREAM USER                                                                *
 *----------------------------------------------------------------------------*/
declare_class(vsf_stream_usr_t);

def_class(vsf_stream_usr_t,,
    protected_member(
        vsf_stream_rx_t         *ptRX;
    )
)
end_def_class(vsf_stream_usr_t)

typedef struct {
    vsf_stream_rx_t             *ptRX;
}vsf_stream_usr_cfg_t;

def_interface(i_stream_usr_t)
    vsf_err_t       (*Init) (   vsf_stream_usr_t *ptObj, 
                                const vsf_stream_usr_cfg_t *ptCFG);
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    void            (*Open)(    vsf_stream_usr_t *ptObj);
    void            (*Close)(   vsf_stream_usr_t *ptObj);
#endif
    struct {
        vsf_pbuf_t *(*Fetch)(   vsf_stream_usr_t *ptObj);
    }Block;

end_def_interface(i_stream_usr_t)

/*============================ GLOBAL VARIABLES ==============================*/
#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
extern vsf_pool(general_pbuf_pool_t) g_tGeneralPBUFPool;
#endif

extern const i_stream_src_t     VSF_STREAM_SRC;
extern const i_stream_usr_t     VSF_STREAM_USR;
/*============================ PROTOTYPES ====================================*/

/*----------------------------------------------------------------------------*
 * STREAM SOURCE                                                              *
 *----------------------------------------------------------------------------*/
extern
vsf_err_t vsf_stream_src_init(  vsf_stream_src_t *ptObj, 
                                const vsf_stream_src_cfg_t *ptCFG);

extern
vsf_pbuf_t *vsf_stream_src_new_pbuf (vsf_stream_src_t *ptObj,
                                           int_fast32_t nNoLessThan,
                                           int_fast32_t nBestSize);

extern
vsf_err_t vsf_stream_src_send_pbuf (vsf_stream_src_t *ptObj, 
                                    vsf_pbuf_t *ptOldBlock);

extern 
vsf_stream_tx_t *vsf_stream_src_get_tx(vsf_stream_src_t *ptObj);

#if VSF_STREAM_CFG_SUPPORT_RESOURCE_LIMITATION == ENABLED
extern 
vsf_err_t vsf_stream_src_set_limitation(    vsf_stream_src_t *ptObj,
                                            uint_fast16_t hwpbufCountUpLimit,
                                            uint_fast16_t hwpbufPoolReserve);
#endif

/*----------------------------------------------------------------------------*
 * STREAM USER                                                                *
 *----------------------------------------------------------------------------*/

extern 
vsf_err_t vsf_stream_usr_init(  vsf_stream_usr_t *ptObj, 
                                const vsf_stream_usr_cfg_t *ptCFG);

extern 
vsf_pbuf_t *vsf_stream_usr_fetch_pbuf ( vsf_stream_usr_t *ptObj);

extern 
void vsf_stream_usr_open ( vsf_stream_usr_t *ptObj);

extern 
void vsf_stream_usr_close ( vsf_stream_usr_t *ptObj);


/*============================ INCLUDES ======================================*/
#include "./vsf_stream_writer.h"
#include "./vsf_stream_reader.h"
#endif
#endif
/* EOF */
