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

#ifndef __VSF_USBH_HID_H__
#define __VSF_USBH_HID_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HID == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_USBH_HID_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBH_HID_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vk_usbh_hid_send_report(__hid, __buffer, __size)                       \
            __vk_usbh_hid_send_report_imp(                                     \
                (vk_usbh_hid_teda_t *)(__hid), (__buffer), (__size))

#define vk_usbh_hid_recv_report(__hid, __buffer, __size)                       \
            __vk_usbh_hid_recv_report_imp(                                     \
                (vk_usbh_hid_teda_t *)(__hid), (__buffer), (__size))

#define vk_usbh_hid_get_rx_report(__hid)                                       \
            __vk_usbh_hid_get_rx_report_imp((vk_usbh_hid_teda_t *)(__hid))
#define vk_usbh_hid_get_tx_report(__hid)                                       \
            __vk_usbh_hid_get_tx_report_imp((vk_usbh_hid_teda_t *)(__hid))

#define vk_usbh_hid_set_idle(__hid, __id, __duration)                          \
            __vk_usbh_hid_set_idle_imp(                                        \
                (vk_usbh_hid_base_t *)(__hid), (__id), (__duration))

#define vk_usbh_hid_send_report_req(__hid, __type_id, __report, __report_len)  \
            __vk_usbh_hid_send_report_req_imp(                                 \
                (vk_usbh_hid_base_t *)(__hid), (__type_id), (__report), (__report_len))

#define vk_usbh_hid_recv_report_req(__hid, __type_id, __report, __report_len)  \
            __vk_usbh_hid_recv_report_req_imp(                                 \
                (vk_usbh_hid_base_t *)(__hid), (__type_id), (__report), (__report_len))

/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbh_hid_base_t)
dcl_simple_class(vk_usbh_hid_teda_t)

def_simple_class(vk_usbh_hid_base_t) {

    public_member(
        union {
            vsf_eda_evthandler_t user_evthandler;
            vk_usbh_ep0_t *ep0;
        };
    )

    protected_member(
        vk_usbh_t *usbh;
        vk_usbh_dev_t *dev;
        vk_usbh_ifs_t *ifs;
    )

    private_member(
        vk_usbh_urb_t urb_in;
        vk_usbh_urb_t urb_out;

        uint16_t desc_len;
    )
};

def_simple_class(vk_usbh_hid_teda_t) {
    public_member(
        implement(vk_usbh_hid_base_t)
        implement(vsf_teda_t)
    )
};

// user callback will need this
typedef struct vk_usbh_hid_input_t vk_usbh_hid_input_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_hid_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef __VSF_USBH_HID_CLASS_INHERIT__
extern void * vk_usbh_hid_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs, uint_fast32_t obj_size, bool has_hid_desc);
extern void vk_usbh_hid_disconnect(vk_usbh_hid_teda_t *hid);

extern uint8_t * __vk_usbh_hid_get_tx_report_imp(vk_usbh_hid_teda_t *hid);
extern uint8_t * __vk_usbh_hid_get_rx_report_imp(vk_usbh_hid_teda_t *hid);

extern vsf_err_t __vk_usbh_hid_recv_report_imp(vk_usbh_hid_teda_t *hid, uint8_t *buffer, int_fast32_t size);
extern vsf_err_t __vk_usbh_hid_recv_report_req_imp(vk_usbh_hid_base_t *hid, uint_fast16_t type_id, uint8_t *report, uint_fast16_t report_len);
extern vsf_err_t __vk_usbh_hid_send_report_req_imp(vk_usbh_hid_base_t *hid, uint_fast16_t type_id, uint8_t *report, uint_fast16_t report_len);

extern vsf_err_t __vk_usbh_hid_set_idle_imp(vk_usbh_hid_base_t *hid, uint_fast8_t id, uint_fast8_t duration);
#endif

// user can call vk_usbh_hid_send_report in callbacks
extern vsf_err_t __vk_usbh_hid_send_report_imp(vk_usbh_hid_teda_t *hid, uint8_t *buffer, int_fast32_t size);

#ifdef __cplusplus
}
#endif

#undef __VSF_USBH_HID_CLASS_IMPLEMENT
#undef __VSF_USBH_HID_CLASS_INHERIT__

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_HID
#endif      // __VSF_USBH_HID_H__
