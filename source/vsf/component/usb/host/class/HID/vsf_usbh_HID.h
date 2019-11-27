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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HID == ENABLED

#if     defined(VSF_USBH_HID_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBH_HID_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usbh_hid_send_report(__hid, __buffer, __size)                       \
            __vsf_usbh_hid_send_report_imp(                                     \
                (vsf_usbh_hid_eda_t *)(__hid), (__buffer), (__size))

#define vsf_usbh_hid_recv_report(__hid, __buffer, __size)                       \
            __vsf_usbh_hid_recv_report_imp(                                     \
                (vsf_usbh_hid_eda_t *)(__hid), (__buffer), (__size))

#define vsf_usbh_hid_get_rx_report(__hid)                                       \
            __vsf_usbh_hid_get_rx_report_imp((vsf_usbh_hid_eda_t *)(__hid))
#define vsf_usbh_hid_get_tx_report(__hid)                                       \
            __vsf_usbh_hid_get_tx_report_imp((vsf_usbh_hid_eda_t *)(__hid))

#define vsf_usbh_hid_set_idle(__hid, __id, __duration)                          \
            __vsf_usbh_hid_set_idle_imp(                                        \
                (vsf_usbh_hid_base_t *)(__hid), (__id), (__duration))

#define vsf_usbh_hid_send_report_req(__hid, __type_id, __report, __report_len)  \
            __vsf_usbh_hid_send_report_req_imp(                                 \
                (vsf_usbh_hid_base_t *)(__hid), (__type_id), (__report), (__report_len))

#define vsf_usbh_hid_recv_report_req(__hid, __type_id, __report, __report_len)  \
            __vsf_usbh_hid_recv_report_req_imp(                                 \
                (vsf_usbh_hid_base_t *)(__hid), (__type_id), (__report), (__report_len))

/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbh_hid_base_t)
declare_simple_class(vsf_usbh_hid_eda_t)
declare_simple_class(vsf_usbh_hid_teda_t)

def_simple_class(vsf_usbh_hid_base_t) {

    public_member(
        union {
            vsf_eda_evthandler_t user_evthandler;
            vsf_usbh_ep0_t *ep0;
        };
    )

    protected_member(
        vsf_usbh_t *usbh;
        vsf_usbh_dev_t *dev;
        vsf_usbh_ifs_t *ifs;
    )

    private_member(
        vsf_usbh_urb_t urb_in;
        vsf_usbh_urb_t urb_out;

        uint16_t desc_len;
    )
};

def_simple_class(vsf_usbh_hid_eda_t) {
    implement(vsf_usbh_hid_base_t)
    implement(vsf_eda_t)
};

def_simple_class(vsf_usbh_hid_teda_t) {
    implement(vsf_usbh_hid_base_t)
    implement(vsf_teda_t)
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbh_class_drv_t vsf_usbh_hid_drv;

/*============================ PROTOTYPES ====================================*/

#ifdef VSF_USBH_HID_INHERIT
extern void * vsf_usbh_hid_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
            vsf_usbh_ifs_parser_t *parser_ifs, uint_fast32_t obj_size, bool has_hid_desc);
extern void vsf_usbh_hid_disconnect(vsf_usbh_hid_eda_t *hid);

extern uint8_t * __vsf_usbh_hid_get_tx_report_imp(vsf_usbh_hid_eda_t *hid);
extern uint8_t * __vsf_usbh_hid_get_rx_report_imp(vsf_usbh_hid_eda_t *hid);

extern vsf_err_t __vsf_usbh_hid_send_report_imp(vsf_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size);
extern vsf_err_t __vsf_usbh_hid_recv_report_imp(vsf_usbh_hid_eda_t *hid, uint8_t *buffer, int_fast32_t size);
extern vsf_err_t __vsf_usbh_hid_recv_report_req_imp(vsf_usbh_hid_base_t *hid, uint_fast16_t type_id, uint8_t *report, uint_fast16_t report_len);
extern vsf_err_t __vsf_usbh_hid_send_report_req_imp(vsf_usbh_hid_base_t *hid, uint_fast16_t type_id, uint8_t *report, uint_fast16_t report_len);

extern vsf_err_t __vsf_usbh_hid_set_idle_imp(vsf_usbh_hid_base_t *hid, uint_fast8_t id, uint_fast8_t duration);
#endif

#undef VSF_USBH_HID_IMPLEMENT
#undef VSF_USBH_HID_IMHERIT

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_HID
#endif      // __VSF_USBH_HID_H__
