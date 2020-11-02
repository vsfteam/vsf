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

#if VSF_USE_STREAM == ENABLED
#include "./vsf_pbuf_queue.h"


/*============================ MACROS ========================================*/
#undef  this
#define this    (*this_ptr)

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
static vsf_pbuf_t * __vsf_stream_pbuf_rx_fecch(vsf_stream_rx_t *obj_ptr);

static vsf_err_t __vsf_stream_pbuf_rx_register_notification(
    vsf_stream_rx_t *obj_ptr, vsf_stream_dat_rdy_evt_t tEventHandling);

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
static 
void  __vsf_stream_pbuf_rx_open(vsf_stream_rx_t *obj_ptr);

static 
void  __vsf_stream_pbuf_rx_close(vsf_stream_rx_t *obj_ptr);
#endif
static 
vsf_stream_status_t __vsf_stream_pbuf_rx_get_status(vsf_stream_rx_t *obj_ptr);

static vsf_err_t __vsf_stream_pbuf_tx_send(
    vsf_stream_tx_t *obj_ptr, vsf_pbuf_t *pbuf);

static vsf_err_t __vsf_stream_pbuf_tx_register_notification(
    vsf_stream_tx_t *obj_ptr, vsf_stream_dat_drn_evt_t tEventHandling);

static 
vsf_stream_status_t __vsf_stream_pbuf_tx_get_status(vsf_stream_tx_t *obj_ptr);

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

vsf_err_t vsf_stream_fifo_init( vsf_stream_fifo_t *obj_ptr, 
                                vsf_stream_fifo_cfg_t *cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    memset(obj_ptr, 0, sizeof(vsf_stream_fifo_t));

    if (NULL != cfg_ptr) {
        this.cfg = (*cfg_ptr);
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

vsf_err_t vsf_pbuf_queue_enqueue(vsf_stream_fifo_t *obj_ptr, vsf_pbuf_t *pblock)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    vsf_err_t result = VSF_ERR_NONE;
    
    do {
        bool bFirstDataReady = false;
        if (NULL == pblock) {
            result = VSF_ERR_INVALID_PTR;
            break;
        }
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        if (!this.Status.IsOpen) {
            return VSF_ERR_NOT_AVAILABLE;
        }
    #endif

        VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
            vsf_slist_queue_enqueue(vsf_pbuf_t, 
                                    use_as__vsf_slist_node_t,
                                    &this.use_as__vsf_slist_queue_t,
                                    pblock);
            this.Status.u14Count++;
            if (this.Status.u14Count > this.Status.u8DataDrainThreshold) {
                this.Status.IsDataDrain = false;
            }
    #if VSF_PBUF_QUEUE_CFG_RAISE_DAVL_EVT_ONCE == ENABLED
            if (this.Status.u14Count == (this.Status.u8DataReadyThreshold + 1)) {
    #else
            if (this.Status.u14Count >= (this.Status.u8DataReadyThreshold + 1)) {
    #endif
                this.Status.IsDataReady = true;
                bFirstDataReady = true;
            }
        )

        //! raise data ready event
        if (    bFirstDataReady 
            &&  (NULL != this.tDataReadyEventHandling.handler_fn)) {
            this.tDataReadyEventHandling.handler_fn(
                this.tDataReadyEventHandling.target_ptr, 
                &this.RX,
                this.Status);
        }

    } while(0);

    return result;
}

SECTION(".text.vsf.service.stream.pbuf_queue.vsf_pbuf_queue_peek")
vsf_pbuf_t * vsf_pbuf_queue_peek(vsf_stream_fifo_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    vsf_pbuf_t *ptBuff = NULL;
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    
    vsf_slist_queue_peek(   vsf_pbuf_t, 
                            use_as__vsf_slist_node_t,
                            &this.use_as__vsf_slist_queue_t,
                            ptBuff);

    return ptBuff;
}

vsf_pbuf_t * vsf_pbuf_queue_dequeue(vsf_stream_fifo_t *obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
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

            this.Status.u14Count--;
            if (!this.Status.u14Count) {
                this.Status.IsDataReady = false;
            } 
            if (this.Status.u14Count <= this.Status.u8DataDrainThreshold) {
                this.Status.IsDataDrain = true;
                bDataDrain = true;
            }
        } while(0);
    )

    //! raise data drain event
    if (    bDataDrain 
        &&  (NULL != this.tDataDrainEventHandling.handler_fn)) {
        this.tDataDrainEventHandling.handler_fn(
            this.tDataDrainEventHandling.target_ptr, 
            &this.TX,
            this.Status);
    }

    return ptBuff;
}

vsf_err_t vsf_pbuf_queue_data_available_event_register(
    vsf_stream_fifo_t *obj_ptr, vsf_stream_dat_rdy_evt_t tEventHandling)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    this.tDataReadyEventHandling = tEventHandling;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_pbuf_queue_data_drain_event_register(
    vsf_stream_fifo_t *obj_ptr, vsf_stream_dat_drn_evt_t tEventHandling)
{
    class_internal(obj_ptr, this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    this.tDataDrainEventHandling = tEventHandling;
    return VSF_ERR_NONE;
}

/*----------------------------------------------------------------------------*
 * Interface Wrapper                                                          *
 *----------------------------------------------------------------------------*/

static 
vsf_pbuf_t * __vsf_stream_pbuf_rx_fecch(vsf_stream_rx_t *obj_ptr)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, RX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    
    return vsf_pbuf_queue_dequeue((vsf_stream_fifo_t *)this_ptr);
}

#if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
static 
void  __vsf_stream_pbuf_rx_open(vsf_stream_rx_t *obj_ptr)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, RX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
        this.Status.IsOpen = true;
    )
}

static 
void  __vsf_stream_pbuf_rx_close(vsf_stream_rx_t *obj_ptr)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, RX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    VSF_PBUF_QUEUE_CFG_ATOM_ACCESS(
        this.Status.IsOpen = false;
    )
}
#endif

static 
vsf_stream_status_t __vsf_stream_pbuf_rx_get_status(vsf_stream_rx_t *obj_ptr)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, RX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    return this.Status;
}

static vsf_err_t __vsf_stream_pbuf_rx_register_notification(
    vsf_stream_rx_t *obj_ptr, vsf_stream_dat_rdy_evt_t tEventHandling)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, RX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    return vsf_pbuf_queue_data_available_event_register( 
        (vsf_stream_fifo_t *)this_ptr, tEventHandling);
}

static vsf_err_t __vsf_stream_pbuf_tx_send(vsf_stream_tx_t *obj_ptr, vsf_pbuf_t *pbuf)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, TX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);
    
    return vsf_pbuf_queue_enqueue((vsf_stream_fifo_t *)this_ptr, pbuf);
}

static vsf_err_t __vsf_stream_pbuf_tx_register_notification(
    vsf_stream_tx_t *obj_ptr, vsf_stream_dat_drn_evt_t tEventHandling)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, TX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    return vsf_pbuf_queue_data_drain_event_register( (vsf_stream_fifo_t *)this_ptr, 
                                                        tEventHandling);
}

static 
vsf_stream_status_t __vsf_stream_pbuf_tx_get_status(vsf_stream_tx_t *obj_ptr)
{
    class_internal( container_of(obj_ptr, vsf_stream_fifo_t, TX), 
                    this_ptr, vsf_stream_fifo_t);
    VSF_SERVICE_ASSERT(NULL != this_ptr);

    return this.Status;
}

#endif


/* EOF */
