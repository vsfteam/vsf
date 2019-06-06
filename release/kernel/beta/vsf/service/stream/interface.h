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

#ifndef __VSF_SERVICE_STREAM_INTERFACE_H__
#define __VSF_SERVICE_STREAM_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#include "./pbuf/vsf_pbuf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_stream_tx_t vsf_stream_tx_t;
typedef struct vsf_stream_rx_t vsf_stream_rx_t;
typedef union vsf_stream_status_t vsf_stream_status_t;

typedef void vsf_stream_dat_rdy_evt_handler_t(  void *pTarget, 
                                                vsf_stream_rx_t *,
                                                vsf_stream_status_t tStatus);

typedef struct vsf_stream_dat_rdy_evt_t vsf_stream_dat_rdy_evt_t;
struct vsf_stream_dat_rdy_evt_t
{
    vsf_stream_dat_rdy_evt_handler_t *fnHandler;
    void *pTarget;
};

typedef void vsf_stream_dat_drn_evt_handler_t(  void *pTarget, 
                                                vsf_stream_tx_t *,
                                                vsf_stream_status_t tStatus);

typedef struct vsf_stream_dat_drn_evt_t vsf_stream_dat_drn_evt_t;
struct vsf_stream_dat_drn_evt_t
{
    vsf_stream_dat_drn_evt_handler_t *fnHandler;
    void *pTarget;
};

union vsf_stream_status_t {
    struct {
        uint32_t u14Count                   : 12;
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        uint32_t IsOpen                     : 1;
    #else
        uint32_t                            : 1;
    #endif
        uint32_t IsDataFull                 : 1;
        uint32_t IsDataReady                : 1;
        uint32_t IsDataDrain                : 1;
        uint32_t u8DataReadyThreshold       : 8;
        uint32_t u8DataDrainThreshold       : 8;
    };
    uint32_t wStatus;
};

def_interface(i_stream_pbuf_rx_t)
    vsf_pbuf_t * (*Fetch)(vsf_stream_rx_t *);
    struct {
        vsf_err_t (*Register)(vsf_stream_rx_t *, vsf_stream_dat_rdy_evt_t);
    } DataReadyEvent;
    vsf_stream_status_t (*GetStatus)(vsf_stream_rx_t *);
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    void  (*Open)(vsf_stream_rx_t *ptObj);
    void  (*Close)(vsf_stream_rx_t *ptObj);
#endif
end_def_interface(i_stream_pbuf_rx_t)

def_interface(i_stream_pbuf_tx_t)
    vsf_err_t (*Send)(vsf_stream_tx_t *ptObj, vsf_pbuf_t *pblock);
    struct {
        vsf_err_t (*Register)(vsf_stream_tx_t *, vsf_stream_dat_drn_evt_t);
    } DataDrainEvent;
    vsf_stream_status_t (*GetStatus)(vsf_stream_tx_t *);
end_def_interface(i_stream_pbuf_tx_t)

def_interface(i_stream_pbuf_t)
    i_stream_pbuf_rx_t  RX;
    i_stream_pbuf_tx_t  TX;
end_def_interface(i_stream_pbuf_t)



struct vsf_stream_tx_t {
    const i_stream_pbuf_tx_t *piMethod;
};

struct vsf_stream_rx_t {
    const i_stream_pbuf_rx_t *piMethod;
};


/*============================ GLOBAL VARIABLES ==============================*/


/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
