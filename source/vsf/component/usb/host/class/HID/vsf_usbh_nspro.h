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

#ifndef __VSF_USBH_NSPRO_H__
#define __VSF_USBH_NSPRO_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_NSPRO == ENABLED

#include "component/usb/common/class/HID/vsf_usb_nspro.h"
#if VSF_USE_INPUT_NSPRO == ENABLED
#   include "component/input/driver/nspro/vsf_input_nspro.h"
#endif

#if     defined(VSF_USBH_NSPRO_IMPLEMENT)
#   undef VSF_USBH_NSPRO_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USE_USB_HOST_HID != ENABLED
#   error "NSPRO driver need VSF_USE_USB_HOST_HID"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_usbh_nspro_t)

enum vk_usbh_nspro_state_t {
    VSF_USBH_NSPRO_GET_INFO,
    VSF_USBH_NSPRO_HANDSHAKE,
    VSF_USBH_NSPRO_RUNNING,
};
typedef enum vk_usbh_nspro_state_t vk_usbh_nspro_state_t;

enum vk_usbh_nspro_type_t {
    NSPRO_LEFT      = 0x01,
    NSPRO_RIGHT     = 0x02,
    NSPRO_BOTH      = 0x03,
};
typedef enum vk_usbh_nspro_type_t vk_usbh_nspro_type_t;

def_simple_class(vk_usbh_nspro_t) {
	implement(vk_usbh_hid_teda_t)
#if VSF_USE_INPUT_NSPRO == ENABLED
    implement(vk_input_nspro_t)
#endif

    private_member(
        bool out_idle;
        vk_usbh_nspro_state_t start_state;
        uint8_t mac[6];
        vk_usbh_nspro_type_t type;
        vsf_usb_nspro_gamepad_out_report_t gamepad_out_buf;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_nspro_drv;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_NSPRO
#endif      // __VSF_USBH_NSPRO_H__
