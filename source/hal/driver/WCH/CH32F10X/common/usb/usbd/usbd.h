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

#ifndef __HAL_DRIVER_WCH_CH32f10x_USBD_H__
#define __HAL_DRIVER_WCH_CH32f10x_USBD_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../../__device.h"

//! include the infrastructure
//#include "../../io/io.h"
//#include "../../pm/pm.h"

#include "hal/interface/vsf_interface_usb.h"

/*============================ MACROS ========================================*/

#undef USB_DC_HAS_CONST
#if     !defined(USB_DC_EP_NUM) && !defined(USB_DC_HP_IRQN)                     \
    &&  !defined(USB_DC_HP_IRQN) && !defined(USB_DC_REG_BASE)                   \
    &&  !defined(USB_DC_PMA_BASE)
#   define USB_DC_HAS_CONST
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#ifdef USB_DC_HAS_CONST
struct ch32f10x_usbd_const_t {
    // information from device.h
#   ifndef USB_DC_EP_NUM
    uint8_t ep_num;
#   endif
#   ifndef USB_DC_LP_IRQN
    IRQn_Type lp_irq;
#   endif
#   ifndef USB_DC_REG_BASE
    void *reg;
#   endif
#   ifndef USB_DC_PMA_BASE
    void *pma;
#   endif
};
typedef struct ch32f10x_usbd_const_t ch32f10x_usbd_const_t;
#endif

struct ch32f10x_usbd_t {
    struct {
        usb_dc_evt_handler_t evt_handler;
        void *param;
    } callback;
#ifdef USB_DC_HAS_CONST
    const ch32f10x_usbd_const_t *param;
#endif

    uint16_t ep_buf_ptr;
    bool is_status_in;
    bool is_status_out;
    union {
#ifndef USB_DC_EP_NUM
        struct {
            uint8_t ep_size_in[16];
            uint8_t ep_size_out[16];
        };
        uint8_t ep_size[32];
#else
        struct {
            uint8_t ep_size_in[USB_DC_EP_NUM];
            uint8_t ep_size_out[USB_DC_EP_NUM];
        };
        uint8_t ep_size[2 * USB_DC_EP_NUM];
#endif
    };
};
typedef struct ch32f10x_usbd_t ch32f10x_usbd_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t ch32f10x_usbd_init(ch32f10x_usbd_t *usbd, usb_dc_cfg_t *cfg);
extern void ch32f10x_usbd_fini(ch32f10x_usbd_t *usbd);
extern void ch32f10x_usbd_reset(ch32f10x_usbd_t *usbd, usb_dc_cfg_t *cfg);

extern void ch32f10x_usbd_connect(ch32f10x_usbd_t *usbd);
extern void ch32f10x_usbd_disconnect(ch32f10x_usbd_t *usbd);
extern void ch32f10x_usbd_wakeup(ch32f10x_usbd_t *usbd);

extern void ch32f10x_usbd_set_address(ch32f10x_usbd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t ch32f10x_usbd_get_address(ch32f10x_usbd_t *usbd);

extern uint_fast16_t ch32f10x_usbd_get_frame_number(ch32f10x_usbd_t *usbd);
extern uint_fast8_t ch32f10x_usbd_get_mframe_number(ch32f10x_usbd_t *usbd);

extern void ch32f10x_usbd_get_setup(ch32f10x_usbd_t *usbd, uint8_t *buffer);
extern void ch32f10x_usbd_status_stage(ch32f10x_usbd_t *usbd, bool is_in);

extern uint_fast8_t ch32f10x_usbd_ep_get_feature(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t ch32f10x_usbd_ep_add(ch32f10x_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t ch32f10x_usbd_ep_get_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t ch32f10x_usbd_ep_set_stall(ch32f10x_usbd_t *usbd, uint_fast8_t ep);
extern bool ch32f10x_usbd_ep_is_stalled(ch32f10x_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t ch32f10x_usbd_ep_clear_stall(ch32f10x_usbd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t ch32f10x_usbd_ep_get_data_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t ch32f10x_usbd_ep_transaction_read_buffer(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t ch32f10x_usbd_ep_transaction_enable_out(ch32f10x_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t ch32f10x_usbd_ep_transaction_set_data_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t ch32f10x_usbd_ep_transaction_write_buffer(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t ch32f10x_usbd_ep_transfer_recv(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t ch32f10x_usbd_ep_transfer_send(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void ch32f10x_usbd_irq(ch32f10x_usbd_t *usbd);

#endif
/* EOF */
