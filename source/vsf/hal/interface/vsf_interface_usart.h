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

#ifndef __HAL_DRIVER_USART_INTERFACE_H__
#define __HAL_DRIVER_USART_INTERFACE_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "vsf_interface_common.h"
#if VSF_HAL_CFG_USE_STREAM == ENABLED || VSF_HAL_CFG_USE_VSFSTREAM == ENABLED
#   indlude "service/stream/vsf_stream.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_usart_mode_t em_usart_mode_t;
/*
//! \name usart working mode 
//! @{
enum em_usart_mode_t {
    USART_NO_AUTO_BAUD      = 0x00,
    USART_AUTO_BAUD_MODE0   = 0x01,
    USART_AUTO_BAUD_MODE1   = 0x03,
    USART_AUTO_RESTART      = 0x04,
    USART_NO_AUTO_RESTART   = 0x00,

    USART_NO_PARITY         = 0x00,
    USART_EVEN_PARITY       = 0x18,
    USART_ODD_PARITY        = 0x08,
    USART_FORCE_1_PARITY    = 0x28,
    USART_FORCE_0_PARITY    = 0x38,

    USART_1_STOPBIT         = 0x00,
    USART_2_STOPBIT         = 0x40,

    USART_ENABLE_FIFO       = 0x00,
    USART_DISABLE_FIFO      = 0x80,

    USART_5_BIT_LENGTH      = 0x0000,
    USART_6_BIT_LENGTH      = 0x0100,
    USART_7_BIT_LENGTH      = 0x0200,
    USART_8_BIT_LENGTH      = 0x0300,
       
    USART_SYNC_MODE         = 0x0400,
    USART_ASYNC_MODE        = 0x0000
};
//! @}
*/

//! \name usart configuration
//! @{
typedef struct usart_cfg_t usart_cfg_t;
struct usart_cfg_t {
    uint32_t    wMode;
    uint32_t    wBaudrate;
    uint32_t    wReceiveTimeout;

#if VSF_HAL_CFG_USE_STREAM == ENABLED
    implement(vsf_stream_src_cfg_t)
    vsf_stream_pbuf_fifo_t *ptRXFIFO;
    vsf_stream_pbuf_fifo_t *ptTXFIFO;
#endif
};

//! @}

/* usart_status_t should implement peripheral_status_t */
typedef struct usart_status_t usart_status_t;

/* usart_capability_t should implement peripheral_capability_t */
typedef struct usart_capability_t usart_capability_t;



typedef void vsf_usart_evt_handler_t(   void *pTarget, 
                                        vsf_usart_t *,
                                        usart_status_t tStatus);

typedef struct vsf_usart_evt_type_t vsf_usart_evt_type_t;
struct vsf_usart_evt_type_t
{
    vsf_usart_evt_handler_t *fnHandler;
    void *pTarget;
};

enum em_usart_evt_t{
    VSF_USART_EVT_RX,
    VSF_USART_EVT_TX,
    VSF_USART_EVT_RCV_BLK_CPL,
    VSF_USART_EVT_SND_BLK_CPL,
};

//! the the target machine int
typedef unsigned int usart_evt_status_t;

//! \name class: usart_t
//! @{
def_interface(i_usart_t)
    union {
        implement(peripheral_t);
        struct {
            usart_status_t     (*Status)(void);
            usart_capability_t (*Capability)(void);
        }USART;
    };
    vsf_err_t       (*Init)(usart_cfg_t *ptCFG);

    //! data access
    implement(i_byte_pipe_t)
    implement_ex(vsf_async_block_access_t, Block)

    //! event
    struct {
        void (*Register)(vsf_usart_evt_type_t tType, vsf_usart_evt_t tEvent);
        usart_evt_status_t (*Enable)(usart_evt_status_t tEventMask);
        usart_evt_status_t (*Disable)(usart_evt_status_t tEventMask);
        void (*Resume)(usart_evt_status_t tEventStatus);
    }Event;

#if VSF_HAL_CFG_USE_STREAM == ENABLED || VSF_HAL_CFG_USE_VSFSTREAM == ENABLED
    /*! \note the stream interface is only implemented in lv1 */
    struct {
#if VSF_HAL_CFG_USE_VSFSTREAM == ENABLED
        struct {
            vsf_stream_t *  (*Get)(void);
        }RX;
        struct {
            vsf_stream_t *  (*Get)(void);
        }TX;
#endif
#if VSF_HAL_CFG_USE_STREAM == ENABLED
        struct {
            implement(vsf_stream_rx_t)
            void (*Register)(vsf_stream_tx_t *ptReceiver);
        }RX;
        struct {
            implement(vsf_stream_tx_t)
            void (*Register)(vsf_stream_rx_t *ptSender);
        }TX;
#endif
    }Stream;
#endif
    //! properties
    u32_property_t  Baudrate;

end_def_interface(i_usart_t)
//! @}

typedef struct vsf_usart_t vsf_usart_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern const i_usart_t VSF_USART[USART_COUNT];

/*============================ PROTOTYPES ====================================*/



#endif
