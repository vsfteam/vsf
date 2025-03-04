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
#ifndef __VSF_LIBUSB_HCD_H___
#define __VSF_LIBUSB_HCD_H___

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_LIBUSB == ENABLED

#include "hal/driver/common/template/vsf_template_usb.h"
#include "component/usb/host/vsf_usbh.h"
#include "hal/vsf_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBH_CFG_ENABLE_ROOT_HUB == ENABLED
#   error "libusb_hcd does not support root hub"
#endif
#if VSF_USBH_USE_HUB == ENABLED
#   error "libusb_hcd does not support hub"
#endif
#if !defined(__WIN__) && !defined(__LINUX__) && !defined(__MACOS__)
#   error "libusb_hcd ONLY support win/linux/macos"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_libusb_hcd_param_t {
    vsf_arch_prio_t priority;
} vk_libusb_hcd_param_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_hcd_drv_t vk_libusb_hcd_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif // __VSF_LIBUSB_HCD_H___
