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
#define __VSF_PBUF_QUEUE_CLASS_IMPLEMENT
#include "service/vsf_service_cfg.h"

#if VSF_USE_SERVICE_STREAM == ENABLED
#include "./vsf_pbuf_queue.h"


/*============================ MACROS ========================================*/
#undef  this
#define this    (*ptThis)

#ifndef VSF_PBUF_QUEUE_CFG_RAISE_DAVL_EVT_ONCE 
#   if  VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
#       define VSF_PBUF_QUEUE_CFG_RAISE_DAVL_EVT_ONCE   ENABLED
#   else
#       define VSF_PBUF_QUEUE_CFG_RAISE_DAVL_EVT_ONCE   DISABLED
#   endif
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_PBUF_QUEUE_CFG_ATOM_ACCESS

#   define __VSF_PBUF_QUEUE_USE_DEFAULT_ATOM_ACCESS
/*! \note   By default, the driver tries to make all APIs thread-safe, in the  
 *!         case when you want to disable it, please use following macro to 
 *!         disable it:
 *!
 *!         #define VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(...)   __VA_ARGS__
 *!                 
 *!         
 *!         NOTE: This macro should be defined in app_cfg.h or vsf_cfg.h
 */
#   define VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(...)                                  \
        code_region_simple(this.pregion, __VA_ARGS__)

#elif defined(VSF_PBUF_QUEUE_CFG_ATOM_ACCESS_DEPENDENCY)
#       include VSF_PBUF_QUEUE_CFG_ATOM_ACCESS_DEPENDENCY
#endif

/*============================ TYPES =========================================*/

/*============================ PROTOTYPES ====================================*/
static vsf_pbuf_t * __vsf_stream_pbuf_rx_fecch(vsf_stream_rx_t *pObj);

static vsf_err_t __vsf_stream_pbuf_rx_register_notification(
    vsf_stream_rx_t *pObj, vsf_stream_dat_rdy_evt_t tEventHandling);

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
static 
void  __vsf_stream_pbuf_rx_open(vsf_stream_rx_t *ptObj);

static 
void  __vsf_stream_pbuf_rx_close(vsf_stream_rx_t *ptObj);
#endif
static 
vsf_stream_status_t __vsf_stream_pbuf_rx_get_status(vsf_stream_rx_t *ptObj);

static vsf_err_t __vsf_stream_pbuf_tx_send(
    vsf_stream_tx_t *pObj, vsf_pbuf_t *pbuf);

static vsf_err_t __vsf_stream_pbuf_tx_register_notification(
    vsf_stream_tx_t *pObj, vsf_stream_dat_drn_evt_t tEventHandling);

static 
vsf_stream_status_t __vsf_stream_pbuf_tx_get_status(vsf_stream_tx_t *ptObj);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static const i_stream_pbuf_rx_t c_iStreamFIFORX = {
    .Fetch = __vsf_stream_pbuf_rx_fecch,
    .DataReadyEvent = {
        .Register = __vsf_stream_pbuf_rx_register_notification,
    },
    .GetStatus = &__vsf_stream_pbuf_rx_get_status,
#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
    .Open = &__vsf_stream_pbuf_rx_open,
    .Close = &__vsf_stream_pbuf_rx_close,
#endif
};

static const i_stream_pbuf_tx_t c_iStreamFIFOTX = {
    .Send = __vsf_stream_pbuf_tx_send,
    .DataDrainEvent = {
        .Register = __vsf_stream_pbuf_tx_register_notification,
    },
    .GetStatus = &__vsf_stream_pbuf_tx_get_status,
};

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_stream_fifo_init( vsf_stream_fifo_t *ptObj, 
                                vsf_stream_fifo_cfg_t *ptCFG)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);

    memset(ptObj, 0, sizeof(vsf_stream_fifo_t));

    if (NULL != ptCFG) {
        this.tCFG = (*ptCFG);
    } 

#if defined(__VSF_PBUF_QUEUE_USE_DEFAULT_ATOM_ACCESS)
    if (NULL == this.pregion) {
        this.pregion = (code_region_t *)&DEFAULT_CODE_REGION_ATOM_CODE;
    }
#endif
    
    this.RX.piMethod = &c_iStreamFIFORX;
    this.TX.piMethod = &c_iStreamFIFOTX;

    return VSF_ERR_NONE;
}

/*----------------------------------------------------------------------------*
 * pbuf queue implementation                                                  *
 *----------------------------------------------------------------------------*/

vsf_err_t vsf_pbuf_queue_enqueue(vsf_stream_fifo_t *ptObj, vsf_pbuf_t *pblock)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    vsf_err_t tResult = VSF_ERR_NONE;
    
    do {
        bool bFirstDataReady = false;
        if (NULL == pblock) {
            tResult = VSF_ERR_INVALID_PTR;
            break;
        }
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        if (!this.tStatus.IsOpen) {
            return VSF_ERR_NOT_AVAILABLE;
        }
    #endif

        VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
            vsf_slist_queue_enqueue(vsf_pbuf_t, 
                                    use_as__vsf_slist_node_t,
                                    &this.use_as__vsf_slist_queue_t,
                                    pblock);
            this.tStatus.u14Count++;
            if (this.tStatus.u14Count > this.tStatus.u8DataDrainThreshold) {
                this.tStatus.IsDataDrain = false;
            }
    #if VSF_PBUF_QUEUE_CFG_RAISE_DAVL_EVT_ONCE == ENABLED
            if (this.tStatus.u14Count == (this.tStatus.u8DataReadyThreshold + 1)) {
    #else
            if (this.tStatus.u14Count >= (this.tStatus.u8DataReadyThreshold + 1)) {
    #endif
                this.tStatus.IsDataReady = true;
                bFirstDataReady = true;
            }
        )

        //! raise data ready event
        if (    bFirstDataReady 
            &&  (NULL != this.tDataReadyEventHandling.fnHandler)) {
            this.tDataReadyEventHandling.fnHandler(
                this.tDataReadyEventHandling.pTarget, 
                &this.RX,
                this.tStatus);
        }

    } while(0);

    return tResult;
}

SECTION(".text.vsf.service.stream.pbuf_queue.vsf_pbuf_queue_peek")
vsf_pbuf_t * vsf_pbuf_queue_peek(vsf_stream_fifo_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    vsf_pbuf_t *ptBuff = NULL;
    ASSERT(NULL != ptThis);
    
    vsf_slist_queue_peek(   vsf_pbuf_t, 
                            use_as__vsf_slist_node_t,
                            &this.use_as__vsf_slist_queue_t,
                            ptBuff);

    return ptBuff;
}

vsf_pbuf_t * vsf_pbuf_queue_dequeue(vsf_stream_fifo_t *ptObj)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    vsf_pbuf_t *ptBuff = NULL;
    bool bDataDrain = false;

    VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
        do {
            vsf_slist_queue_dequeue(vsf_pbuf_t, 
                                    use_as__vsf_slist_node_t,
                                    &this.use_as__vsf_slist_queue_t,
                                    ptBuff);
            if (NULL == ptBuff) {
                break;
            }

            this.tStatus.u14Count--;
            if (!this.tStatus.u14Count) {
                this.tStatus.IsDataReady = false;
            } 
            if (this.tStatus.u14Count <= this.tStatus.u8DataDrainThreshold) {
                this.tStatus.IsDataDrain = true;
                bDataDrain = true;
            }
        } while(0);
    )

    //! raise data drain event
    if (    bDataDrain 
        &&  (NULL != this.tDataDrainEventHandling.fnHandler)) {
        this.tDataDrainEventHandling.fnHandler(
            this.tDataDrainEventHandling.pTarget, 
            &this.TX,
            this.tStatus);
    }

    return ptBuff;
}

vsf_err_t vsf_pbuf_queue_data_available_event_register(
    vsf_stream_fifo_t *ptObj, vsf_stream_dat_rdy_evt_t tEventHandling)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    this.tDataReadyEventHandling = tEventHandling;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pbuf_queue_data_drain_event_register(
    vsf_stream_fifo_t *ptObj, vsf_stream_dat_drn_evt_t tEventHandling)
{
    class_internal(ptObj, ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    this.tDataDrainEventHandling = tEventHandling;
    return VSF_ERR_NONE;
}

/*----------------------------------------------------------------------------*
 * Interface Wrapper                                                          *
 *----------------------------------------------------------------------------*/

static 
vsf_pbuf_t * __vsf_stream_pbuf_rx_fecch(vsf_stream_rx_t *ptObj)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, RX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    
    return vsf_pbuf_queue_dequeue((vsf_stream_fifo_t *)ptThis);
}

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
static 
void  __vsf_stream_pbuf_rx_open(vsf_stream_rx_t *ptObj)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, RX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
        this.tStatus.IsOpen = true;
    )
}

static 
void  __vsf_stream_pbuf_rx_close(vsf_stream_rx_t *ptObj)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, RX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
        this.tStatus.IsOpen = false;
    )
}
#endif

static 
vsf_stream_status_t __vsf_stream_pbuf_rx_get_status(vsf_stream_rx_t *ptObj)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, RX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);

    return this.tStatus;
}

static vsf_err_t __vsf_stream_pbuf_rx_register_notification(
    vsf_stream_rx_t *ptObj, vsf_stream_dat_rdy_evt_t tEventHandling)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, RX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);

    return vsf_pbuf_queue_data_available_event_register( 
        (vsf_stream_fifo_t *)ptThis, tEventHandling);
}

static vsf_err_t __vsf_stream_pbuf_tx_send(vsf_stream_tx_t *ptObj, vsf_pbuf_t *pbuf)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, TX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);
    
    return vsf_pbuf_queue_enqueue((vsf_stream_fifo_t *)ptThis, pbuf);
}

static vsf_err_t __vsf_stream_pbuf_tx_register_notification(
    vsf_stream_tx_t *ptObj, vsf_stream_dat_drn_evt_t tEventHandling)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, TX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);

    return vsf_pbuf_queue_data_drain_event_register( (vsf_stream_fifo_t *)ptThis, 
                                                        tEventHandling);
}

static 
vsf_stream_status_t __vsf_stream_pbuf_tx_get_status(vsf_stream_tx_t *ptObj)
{
    class_internal( container_of(ptObj, vsf_stream_fifo_t, TX), 
                    ptThis, vsf_stream_fifo_t);
    ASSERT(NULL != ptThis);

    return this.tStatus;
}

#endif


/* EOF */
