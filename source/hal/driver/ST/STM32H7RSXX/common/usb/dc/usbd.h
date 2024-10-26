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

#ifndef __VSF_HAL_DRIVER_ST_STM32H7RSXX_USBD_H__
#define __VSF_HAL_DRIVER_ST_STM32H7RSXX_USBD_H__

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

extern vsf_err_t vsf_hw_usbd_init(vsf_hw_usb_t *dc, usb_dc_ip_cfg_t *cfg);
extern void vsf_hw_usbd_fini(vsf_hw_usb_t *dc);
extern void vsf_hw_usbd_get_info(vsf_hw_usb_t *dc, usb_dc_ip_info_t *info);
extern void vsf_hw_usbd_connect(vsf_hw_usb_t *dc);
extern void vsf_hw_usbd_disconnect(vsf_hw_usb_t *dc);
extern void vsf_hw_usbd_irq(vsf_hw_usb_t *dc);

#endif
#endif
/* EOF */
