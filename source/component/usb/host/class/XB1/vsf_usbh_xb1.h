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

#ifndef __VSF_USBH_XB1_H__
#define __VSF_USBH_XB1_H__


/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_XB1 == ENABLED

#include "component/usb/common/class/XB1/vsf_usb_xb1.h"
#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
#   include "component/input/vsf_input.h"
#endif

#if     defined(__VSF_USBH_XB1_CLASS_IMPLEMENT)
#   undef __VSF_USBH_XB1_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBH_USE_HID != ENABLED
#   error "XB1 driver need VSF_USBH_USE_HID"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
enum {
    VSF_INPUT_TYPE_XB1 = VSF_INPUT_USER_TYPE,
};

typedef struct vk_input_xb1_t {
    vsf_usb_xb1_gamepad_in_report_t data;
    vk_input_timestamp_t timestamp;
} vk_input_xb1_t;
#endif

dcl_simple_class(vk_usbh_xb1_t)

// xb1 controller is not HID class, but almost compatible with HID class
def_simple_class(vk_usbh_xb1_t) {

    implement(vk_usbh_hid_teda_t)
#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
    implement(vk_input_xb1_t)
#endif

    private_member(
        uint8_t out_idle    : 1;
        uint8_t home_got    : 1;
        uint8_t home_seq;
        uint8_t shake_id;
        vsf_usb_xb1_gamepad_out_report_t gamepad_out_buf;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
extern const vk_input_item_info_t vk_xb1_gamepad_item_info[GAMEPAD_ID_NUM];
#endif

extern const vk_usbh_class_drv_t vk_usbh_xb1_drv;

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
extern void vk_xb1_process_input(vk_input_xb1_t *dev, vsf_usb_xb1_gamepad_in_report_t *data);
extern void vk_xb1_new_dev(vk_input_xb1_t *dev);
extern void vk_xb1_free_dev(vk_input_xb1_t *dev);

#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
