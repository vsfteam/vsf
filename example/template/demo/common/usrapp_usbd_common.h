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

#ifndef __USRAPP_USBD_COMMON_H__
#define __USRAPP_USBD_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if     VSF_USE_USB_DEVICE == ENABLED                                           \
    &&  (   VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED                               \
        ||  VSF_USBD_USE_DCD_DWCOTG == ENABLED                                  \
        ||  VSF_USBD_USE_DCD_USBIP == ENABLED)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_usbd_common_const_t {
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    vk_usbip_dcd_param_t usbip_dcd_param;
#endif
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    vk_musb_fdrc_dcd_param_t musb_fdrc_dcd_param;
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    vk_dwcotg_dcd_param_t dwcotg_dcd_param;
#endif
} usrapp_usbd_common_const_t;

typedef struct usrapp_usbd_common_t {
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    vk_usbip_dcd_t usbip_dcd;
#endif
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    vk_musb_fdrc_dcd_t musb_fdrc_dcd;
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    vk_dwcotg_dcd_t dwcotg_dcd;
#endif
} usrapp_usbd_common_t;

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED                                   \
    ||  VSF_USBD_USE_DCD_DWCOTG == ENABLED                                      \
    ||  VSF_USBD_USE_DCD_USBIP == ENABLED
extern const i_usb_dc_t VSF_USB_DC0;
#endif

extern const usrapp_usbd_common_const_t usrapp_usbd_common_const;
extern usrapp_usbd_common_t usrapp_usbd_common;
extern const i_usb_dc_t VSF_USB_DC0;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_[IP]
#endif      // __USRAPP_USBD_COMMON_H__
/* EOF */