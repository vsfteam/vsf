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

#ifndef __VSF_PBUF_QUEUE_H__
#define __VSF_PBUF_QUEUE_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_SERVICE_STREAM == ENABLED

#include "../interface.h"

#include "../pbuf/vsf_pbuf.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__
   
#if     defined(__VSF_PBUF_QUEUE_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_PBUF_QUEUE_CLASS_IMPLEMENT
#elif   defined(__VSF_PBUF_QUEUE_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_PBUF_QUEUE_CLASS_INHERIT
#endif   

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(vsf_stream_fifo_t)

typedef struct {
    vsf_stream_dat_rdy_evt_t        tDataReadyEventHandling;
    vsf_stream_dat_drn_evt_t        tDataDrainEventHandling;
    struct {
        uint32_t                    : 16;
        uint32_t                    chDataReadyThreshold : 8;
        uint32_t                    chDataDrainThreshold : 8;
    };
#if !defined(VSF_PBUF_QUEUE_CFG_ATOM_ACCESS)
    code_region_t *pregion;         /*!< protection region defined by user */
#endif
}vsf_stream_fifo_cfg_t;


def_class(vsf_stream_fifo_t,
    which(
        vsf_stream_tx_t TX;
        vsf_stream_rx_t RX;
    ),
    private_member(
        implement(vsf_slist_queue_t)
        union {
            vsf_stream_fifo_cfg_t tCFG;
            struct {
                vsf_stream_dat_rdy_evt_t    tDataReadyEventHandling;
                vsf_stream_dat_drn_evt_t    tDataDrainEventHandling;

                vsf_stream_status_t tStatus;

        #if !defined(VSF_PBUF_QUEUE_CFG_ATOM_ACCESS)
                code_region_t *pregion;     /*!< protection region defined by user */
        #endif
            };
        };
    )
)

end_def_class(vsf_stream_fifo_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern 
vsf_err_t vsf_stream_fifo_init( vsf_stream_fifo_t *ptObj, 
                                vsf_stream_fifo_cfg_t *ptCFG);

extern 
vsf_err_t vsf_pbuf_queue_enqueue(vsf_stream_fifo_t *ptObj, vsf_pbuf_t *pblock);

extern 
vsf_pbuf_t * vsf_pbuf_queue_dequeue(vsf_stream_fifo_t *ptObj);

SECTION(".text.vsf.service.stream.pbuf_queue.vsf_pbuf_queue_peek")
extern 
vsf_pbuf_t * vsf_pbuf_queue_peek(vsf_stream_fifo_t *ptObj);

extern
vsf_err_t vsf_pbuf_queue_data_available_event_register(
    vsf_stream_fifo_t *ptObj, vsf_stream_dat_rdy_evt_t tEventHandling);

extern 
vsf_err_t vsf_pbuf_queue_data_drain_event_register(
    vsf_stream_fifo_t *ptObj, vsf_stream_dat_drn_evt_t tEventHandling);

#endif
#endif
/* EOF */
