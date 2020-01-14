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

#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED && APP_CFG_USE_USBH_DEMO == ENABLED

#include "../usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if APP_CFG_USE_LINUX_DEMO == ENABLED
int usbh_main(int argc, char *argv[])
{
#else
int main(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_trace_init(NULL);
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

#if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
    vk_ohci_init();
#endif
    vk_usbh_init(&usrapp_common.usbh.host);
#if VSF_USE_USB_HOST_LIBUSB == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.libusb);
#endif
#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.ecm);
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.bthci);
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.hid);
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.ds4);
#endif
#if VSF_USE_USB_HOST_NSPRO == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.nspro);
#endif
#if VSF_USE_USB_HOST_XB360 == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.xb360);
#endif
#if VSF_USE_USB_HOST_XB1 == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.xb1);
#endif
#if VSF_USE_USB_HOST_HUB == ENABLED
    vk_usbh_register_class(&usrapp_common.usbh.host, &usrapp_common.usbh.hub);
#endif
    return 0;
}

#endif
