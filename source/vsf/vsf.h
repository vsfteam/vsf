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


//! \note Top Level Configuration 

#ifndef __VSF_H__
#define __VSF_H__

/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"
#include "service/vsf_service.h"
#include "hal/vsf_hal.h"

#include "kernel/vsf_kernel.h"

#include "component/av/vsfav.h"

//#include "component/fs/vsf_fs.h"

#include "component/input/vsf_input.h"
#include "component/input/hid/vsf_input_hid.h"

#include "component/usb/host/vsf_usbh.h"
#include "component/usb/host/hcd/ohci/vsf_ohci.h"
#include "component/usb/host/class/HUB/vsf_usbh_HUB.h"
#include "component/usb/host/class/libusb/vsf_usbh_libusb.h"
#include "component/usb/host/class/CDC/vsf_usbh_CDC.h"
#include "component/usb/host/class/CDC/vsf_usbh_CDCECM.h"
#include "component/usb/host/class/BTHCI/vsf_usbh_BTHCI.h"
#include "component/usb/host/class/HID/vsf_usbh_HID.h"

#include "component/usb/device/vsf_usbd.h"
#include "component/usb/device/class/CDC/vsf_usbd_CDC.h"
#include "component/usb/device/class/CDC/vsf_usbd_CDCACM.h"
#include "component/usb/device/class/HID/vsf_usbd_HID.h"
#include "component/usb/device/class/UVC/vsf_usbd_UVC.h"

#include "component/tcpip/netdrv/vsf_netdrv.h"
#include "component/tcpip/vsfip/vsfip.h"
#include "component/tcpip/vsfip/netif/eth/vsfip_eth.h"
#include "component/tcpip/vsfip/proto/dhcp/vsfip_dhcpd.h"
#include "component/tcpip/vsfip/proto/dhcp/vsfip_dhcpc.h"

#ifdef VSF_CFG_USER_HEADER
#   include VSF_CFG_USER_HEADER
#endif

/*============================ MACROS ========================================*/

#ifndef vsf_log_info
#   define vsf_log_info(...)
#endif

#ifndef vsf_log_warning
#   define vsf_log_warning(...)
#endif

#ifndef vsf_log_debug
#   define vsf_log_debug(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
