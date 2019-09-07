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

#ifndef __VSF_USBH_DS4_H__
#define __VSF_USBH_DS4_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_DS4 == ENABLED

#include "component/usb/common/class/HID/vsf_usb_ds4.h"
#if VSF_USE_INPUT_DS4 == ENABLED
#   include "component/input/driver/ds4/vsf_input_ds4.h"
#endif

#if     defined(VSF_USBH_DS4_IMPLEMENT)
#   undef VSF_USBH_DS4_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USE_USB_HOST_HID != ENABLED
#   error "DS4 driver need VSF_USE_USB_HOST_HID"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbh_ds4_t)

def_simple_class(vsf_usbh_ds4_t) {
	implement(vsf_usbh_hid_teda_t)
#if VSF_USE_INPUT_DS4 == ENABLED
    implement(vsf_input_ds4u_t)
#endif

    private_member(
        bool out_idle;
        vsf_usb_ds4_gamepad_out_report_t gamepad_out_buf;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbh_class_drv_t vsf_usbh_ds4_drv;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_DS4
#endif      // __VSF_USBH_DS4_H__
