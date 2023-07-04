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

#ifndef __VSF_MUSB_FDRC_COMMON_H__
#define __VSF_MUSB_FDRC_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     (   (VSF_USE_USB_DEVICE == ENABLED)                                     \
        && (VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED))                             \
    ||  (   (VSF_USE_USB_HOST == ENABLED)                                       \
        && (VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED))

#include "hal/vsf_hal.h"
#include "./vsf_musb_fdrc_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast8_t vk_musb_fdrc_set_ep(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep);

extern void vk_musb_fdrc_set_mask(volatile uint8_t *reg, uint_fast8_t ep);
extern void vk_musb_fdrc_clear_mask(volatile uint8_t *reg, uint_fast8_t ep);
extern uint16_t vk_musb_fdrc_get_mask(volatile uint8_t *reg);

extern void vk_musb_fdrc_interrupt_init(vk_musb_fdrc_reg_t *reg);
extern void vk_musb_fdrc_fifo_init(vk_musb_fdrc_reg_t *reg);
extern uint_fast16_t vk_musb_fdrc_rx_fifo_size(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep);
extern uint16_t vk_musb_fdrc_set_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, usb_ep_type_t type, uint16_t pos, uint_fast16_t size, uint_fast8_t size_msk);
extern void vk_musb_fdrc_read_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern void vk_musb_fdrc_write_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);

#ifdef __cplusplus
}
#endif

#endif
#endif      // __VSF_MUSB_FDRC_COMMON_H__
/* EOF */
