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

#ifndef __VSF_USBH_NSPRO_H__
#define __VSF_USBH_NSPRO_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_NSPRO == ENABLED

#include "component/usb/common/class/HID/vsf_usb_nspro.h"
#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
#   include "component/input/vsf_input.h"
#endif

#if     defined(__VSF_USBH_NSPRO_CLASS_IMPLEMENT)
#   undef __VSF_USBH_NSPRO_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBH_USE_HID != ENABLED
#   error "NSPRO driver need VSF_USBH_USE_HID"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vk_usbh_nspro_state_t {
    VSF_USBH_NSPRO_GET_INFO,
    VSF_USBH_NSPRO_HANDSHAKE,
    VSF_USBH_NSPRO_RUNNING,
} vk_usbh_nspro_state_t;

typedef enum vk_usbh_nspro_type_t {
    NSPRO_LEFT      = 0x01,
    NSPRO_RIGHT     = 0x02,
    NSPRO_BOTH      = 0x03,
} vk_usbh_nspro_type_t;

vsf_class(vk_usbh_nspro_t) {
    implement(vk_usbh_hid_teda_t)

    private_member(
        // gamepad_out_buf should be aligned
        vsf_usb_nspro_gamepad_out_report_t gamepad_out_buf;
        bool out_idle;
        vk_usbh_nspro_state_t start_state;
        uint8_t mac[6];
        vk_usbh_nspro_type_t type;

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
        vk_input_timestamp_t timestamp;
        // data should be aligned
        vsf_usb_nspro_gamepad_in_report_t data;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
extern const vk_input_item_info_t vk_nspro_usb_gamepad_item_info[GAMEPAD_ID_NUM];
#endif

extern const vk_usbh_class_drv_t vk_usbh_nspro_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_NSPRO
#endif      // __VSF_USBH_NSPRO_H__
