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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_USBD_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_USBD_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_usb_dcd_t f1cx00s_usb_dcd_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern f1cx00s_usb_dcd_t USB_DC0;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t f1cx00s_usbd_init(f1cx00s_usb_dcd_t *usbd, usb_dc_cfg_t *cfg);
extern void f1cx00s_usbd_fini(f1cx00s_usb_dcd_t *usbd);
extern void f1cx00s_usbd_reset(f1cx00s_usb_dcd_t *usbd, usb_dc_cfg_t *cfg);

extern void f1cx00s_usbd_connect(f1cx00s_usb_dcd_t *usbd);
extern void f1cx00s_usbd_disconnect(f1cx00s_usb_dcd_t *usbd);
extern void f1cx00s_usbd_wakeup(f1cx00s_usb_dcd_t *usbd);

extern void f1cx00s_usbd_set_address(f1cx00s_usb_dcd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t f1cx00s_usbd_get_address(f1cx00s_usb_dcd_t *usbd);

extern uint_fast16_t f1cx00s_usbd_get_frame_number(f1cx00s_usb_dcd_t *usbd);
extern uint_fast8_t f1cx00s_usbd_get_mframe_number(f1cx00s_usb_dcd_t *usbd);

extern void f1cx00s_usbd_get_setup(f1cx00s_usb_dcd_t *usbd, uint8_t *buffer);
extern void f1cx00s_usbd_status_stage(f1cx00s_usb_dcd_t *usbd, bool is_in);

extern uint_fast8_t f1cx00s_usbd_ep_get_feature(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t f1cx00s_usbd_ep_add(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t f1cx00s_usbd_ep_get_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t f1cx00s_usbd_ep_set_stall(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);
extern bool f1cx00s_usbd_ep_is_stalled(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t f1cx00s_usbd_ep_clear_stall(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t f1cx00s_usbd_ep_get_data_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);

extern vsf_err_t f1cx00s_usbd_ep_transaction_read_buffer(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t f1cx00s_usbd_ep_transaction_enable_out(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep);
extern vsf_err_t f1cx00s_usbd_ep_transaction_set_data_size(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t f1cx00s_usbd_ep_transaction_write_buffer(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t f1cx00s_usbd_ep_transfer_recv(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t f1cx00s_usbd_ep_transfer_send(f1cx00s_usb_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void f1cx00s_usbd_irq(f1cx00s_usb_dcd_t *usbd);

#endif
/* EOF */
