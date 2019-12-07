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

#ifndef __VSF_USBH_XB360_H__
#define __VSF_USBH_XB360_H__


/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_XB360 == ENABLED

#include "component/usb/common/class/HID/vsf_usb_xb360.h"
#if VSF_USE_INPUT_XB360 == ENABLED
#   include "component/input/driver/xb360/vsf_input_xb360.h"
#endif

#if     defined(VSF_USBH_XB360_IMPLEMENT)
#   undef VSF_USBH_XB360_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USE_USB_HOST_HID != ENABLED
#   error "XB360 driver need VSF_USE_USB_HOST_HID"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_usbh_xb360_t)

// xb360 controller is not HID class, but almost compatible with HID class
def_simple_class(vk_usbh_xb360_t) {

    implement(vk_usbh_hid_teda_t)
#if VSF_USE_INPUT_XB360 == ENABLED
    implement(vk_input_xb360_t)
#endif

    private_member(
        bool out_idle;
        vsf_usb_xb360_gamepad_out_report_t gamepad_out_buf;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_xb360_drv;

/*============================ PROTOTYPES ====================================*/

#endif

#endif
