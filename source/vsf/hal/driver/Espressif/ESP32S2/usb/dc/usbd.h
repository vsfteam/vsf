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

#ifndef __OSA_HAL_DRIVER_ESPRESSIF_ESP32S2_USBD_H__
#define __OSA_HAL_DRIVER_ESPRESSIF_ESP32S2_USBD_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "../usb.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t esp32s2_usbd_init(esp32s2_usb_t *dc, usb_dc_ip_cfg_t *cfg);
extern void esp32s2_usbd_fini(esp32s2_usb_t *dc);
extern void esp32s2_usbd_get_info(esp32s2_usb_t *dc, usb_dc_ip_info_t *info);
extern void esp32s2_usbd_connect(esp32s2_usb_t *dc);
extern void esp32s2_usbd_disconnect(esp32s2_usb_t *dc);
extern void esp32s2_usbd_irq(esp32s2_usb_t *dc);

#endif
#endif
/* EOF */
