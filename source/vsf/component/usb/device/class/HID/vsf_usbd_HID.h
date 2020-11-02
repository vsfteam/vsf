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

#ifndef __VSF_USBD_HID_H__
#define __VSF_USBD_HID_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_HID == ENABLED

#include "../../../common/class/HID/vsf_usb_HID.h"

#if     defined(__VSF_USBD_HID_CLASS_IMPLEMENT)
#   undef __VSF_USBD_HID_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBD_HID_CLASS_INHERIT)
#   undef __VSF_USBD_HID_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_USBD_DESC_HID_REPORT(__PTR, __SIZE)                                 \
    {USB_HID_DT_REPORT, 0, 0, (__SIZE), (uint8_t*)(__PTR)}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbd_hid_t)
dcl_simple_class(vk_usbd_hid_report_t)

def_simple_class(vk_usbd_hid_report_t) {

    public_member(
        usb_hid_report_type_t type;
        vsf_mem_t mem;
        uint8_t id;
        uint8_t idle;
    )

    protected_member(
        bool changed;
        uint8_t idle_cnt;
    )
};

def_simple_class(vk_usbd_hid_t) {

    public_member(
        uint8_t ep_out;
        uint8_t ep_in;
        uint8_t num_of_report;

        uint8_t has_report_id   : 1;
        uint8_t notify_eda      : 1;

        vsf_mem_t rx_buffer;
        vk_usbd_hid_report_t *reports;
        vk_usbd_desc_t *desc;

        union {
            vsf_err_t (*on_report)(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);
            vsf_eda_t *eda;
        };
    )

    private_member(
        uint8_t protocol;
        uint8_t cur_report;
        uint8_t cur_in_id;

        bool busy;

        vk_usbd_trans_t transact_in;
        vk_usbd_trans_t transact_out;
        vsf_teda_t teda;

        vk_usbd_dev_t *dev;
        vk_usbd_ifs_t *ifs;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_hid;

/*============================ PROTOTYPES ====================================*/

extern bool vk_usbh_hid_in_report_can_update(vk_usbd_hid_report_t *report);
extern void vk_usbd_hid_in_report_changed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);
extern void vk_usbh_hid_out_report_processed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_HID
#endif      // __VSF_USBD_HID_H__
