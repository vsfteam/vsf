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

#include "./usrapp_usbd_common.h"

#if     VSF_USE_USB_DEVICE == ENABLED                                           \
    &&  (   VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED                               \
        ||  VSF_USBD_USE_DCD_DWCOTG == ENABLED                                  \
        ||  VSF_USBD_USE_DCD_USBIP == ENABLED)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const usrapp_usbd_common_const_t usrapp_usbd_common_const = {
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    .musb_fdrc_dcd_param    = {
        .op                 = &VSF_USB_DC0_IP,
    },
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    .dwcotg_dcd_param       = {
        .op                 = &VSF_USB_DC0_IP,
        .speed              = USRAPP_CFG_USBD_SPEED,
    },
#endif
};

usrapp_usbd_common_t usrapp_usbd_common = {
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    .usbip_dcd.param        = &usrapp_usbd_common_const.usbip_dcd_param,
#endif
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    .musb_fdrc_dcd.param    = &usrapp_usbd_common_const.musb_fdrc_dcd_param,
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    .dwcotg_dcd.param       = &usrapp_usbd_common_const.dwcotg_dcd_param,
#endif
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USBD_USE_DCD_USBIP == ENABLED
vsf_usb_dc_from_usbip_ip(0, usrapp_usbd_common.usbip_dcd, VSF_USB_DC0)
#elif VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
vsf_usb_dc_from_musbfdrc_ip(0, usrapp_usbd_common.musb_fdrc_dcd, VSF_USB_DC0)
#elif VSF_USBD_USE_DCD_DWCOTG == ENABLED
vsf_usb_dc_from_dwcotg_ip(0, usrapp_usbd_common.dwcotg_dcd, VSF_USB_DC0)
#endif


#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_[IP]
/* EOF */