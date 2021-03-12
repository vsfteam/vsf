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

#ifndef __VSF_USBH_BTHCI_H__
#define __VSF_USBH_BTHCI_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_BTHCI == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_bthci_drv;

/*============================ PROTOTYPES ====================================*/

// type: 0x01: command_data, 0x02: acl, 0x03: sco, 0x04: event
vsf_err_t vk_usbh_bthci_send(void *dev, uint8_t type, uint8_t *packet, uint16_t size);
bool vk_usbh_bthci_can_send(void *dev, uint8_t type);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_BTHCI
#endif      // __VSF_USBH_BTHCI_H__
