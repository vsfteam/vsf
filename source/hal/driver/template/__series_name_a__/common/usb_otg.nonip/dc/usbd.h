/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#ifndef __HAL_DRIVER_${SERIES}_USBD_H__
#define __HAL_DRIVER_${SERIES}_USBD_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USBD == ENABLED

#include "../usb.h"

/*============================ MACROS ========================================*/

#define vsf_hw_usbd_t                       vsf_hw_usb_t

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_hw_usbd_init(vsf_hw_usbd_t *usbd, usb_dc_cfg_t *cfg);
extern void vsf_hw_usbd_fini(vsf_hw_usbd_t *usbd);
extern void vsf_hw_usbd_reset(vsf_hw_usbd_t *usbd, usb_dc_cfg_t *cfg);

extern void vsf_hw_usbd_connect(vsf_hw_usbd_t *usbd);
extern void vsf_hw_usbd_disconnect(vsf_hw_usbd_t *usbd);
extern void vsf_hw_usbd_wakeup(vsf_hw_usbd_t *usbd);

extern void vsf_hw_usbd_set_address(vsf_hw_usbd_t *usbd, uint_fast8_t addr);
extern uint_fast8_t vsf_hw_usbd_get_address(vsf_hw_usbd_t *usbd);

extern uint_fast16_t vsf_hw_usbd_get_frame_number(vsf_hw_usbd_t *usbd);
extern uint_fast8_t vsf_hw_usbd_get_mframe_number(vsf_hw_usbd_t *usbd);

extern void vsf_hw_usbd_get_setup(vsf_hw_usbd_t *usbd, uint8_t *buffer);
extern void vsf_hw_usbd_status_stage(vsf_hw_usbd_t *usbd, bool is_in);

extern uint_fast8_t vsf_hw_usbd_ep_get_feature(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint_fast8_t feature);
extern vsf_err_t vsf_hw_usbd_ep_add(vsf_hw_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);
extern uint_fast16_t vsf_hw_usbd_ep_get_size(vsf_hw_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_hw_usbd_ep_set_stall(vsf_hw_usbd_t *usbd, uint_fast8_t ep);
extern bool vsf_hw_usbd_ep_is_stalled(vsf_hw_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_hw_usbd_ep_clear_stall(vsf_hw_usbd_t *usbd, uint_fast8_t ep);

extern uint_fast32_t vsf_hw_usbd_ep_get_data_size(vsf_hw_usbd_t *usbd, uint_fast8_t ep);

extern vsf_err_t vsf_hw_usbd_ep_transaction_read_buffer(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern vsf_err_t vsf_hw_usbd_ep_transaction_enable_out(vsf_hw_usbd_t *usbd, uint_fast8_t ep);
extern vsf_err_t vsf_hw_usbd_ep_transaction_set_data_size(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size);
extern vsf_err_t vsf_hw_usbd_ep_transaction_write_buffer(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

extern vsf_err_t vsf_hw_usbd_ep_transfer_recv(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size);
extern vsf_err_t vsf_hw_usbd_ep_transfer_send(vsf_hw_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp);

extern void vsf_hw_usbd_irq(vsf_hw_usbd_t *usbd);

#endif      // VSF_HAL_USE_USBD
#endif      // __HAL_DRIVER_${SERIES}_USBD_H__
/* EOF */
