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

#ifndef __VSF_USBD_HID_H__
#define __VSF_USBD_HID_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_HID == ENABLED

#include "../../../common/class/HID/vsf_usb_HID.h"
#include "./vsf_usbd_HID_desc.h"

#if     defined(__VSF_USBD_HID_CLASS_IMPLEMENT)
#   undef __VSF_USBD_HID_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBD_HID_CLASS_INHERIT__)
#   undef __VSF_USBD_HID_CLASS_INHERIT__
#   define __VSF_CLASS_INHERIT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_HID_PARAM(__IN_EP, __OUT_EP,                                        \
                    __REPORT_NUM, __REPORTS, __HAS_REPORT_ID,                   \
                    __DESC_NUM, __DESC,                                         \
                    __RX_BUFF, __RX_BUFF_SIZE,                                  \
                    __NOTIFY_EDA, __NOTIFIER)                                   \
            .ep_out             = (__OUT_EP),                                   \
            .ep_in              = (__IN_EP),                                    \
            .num_of_report      = (__REPORT_NUM),                               \
            .reports            = (__REPORTS),                                  \
            .has_report_id      = (__HAS_REPORT_ID),                            \
            .desc_num           = (__DESC_NUM),                                 \
            .desc               = (vk_usbd_desc_t *)(__DESC),                   \
            .rx_buffer.buffer   = (__RX_BUFF),                                  \
            .rx_buffer.size     = (__RX_BUFF_SIZE),                             \
            .notify_eda         = (__NOTIFY_EDA),                               \
            .notifier           = (__NOTIFIER),

#define USB_HID_IFS_NUM             1
#define USB_HID_IFS(__HID_PARAM)    USB_IFS(&vk_usbd_hid, &(__HID_PARAM))


#define __usbd_hid_desc(__name, __ifs, __i_func, __subclass, __protocol,        \
                    __version_bcd, __country_code, __report_desc_len,           \
                    __ep_in, __ep_in_size, __ep_in_interval,                    \
                    __ep_out, __ep_out_size, __ep_out_interval)                 \
            USB_DESC_HID((__ifs), 4 + (__i_func), (__subclass), (__protocol),   \
                    (__version_bcd), (__country_code), (__report_desc_len),     \
                    (__ep_in), (__ep_in_size), (__ep_in_interval),              \
                    (__ep_out), (__ep_out_size), (__ep_out_interval)            \
            )
#define __usbd_hid_desc_iad(__name, __ifs, __i_func, __subclass, __protocol,    \
                    __version_bcd, __country_code, __report_desc_len,           \
                    __ep_in, __ep_in_size, __ep_in_interval,                    \
                    __ep_out, __ep_out_size, __ep_out_interval)                 \
            USB_DESC_HID_IAD((__ifs), 4 + (__i_func), (__subclass), (__protocol),\
                    (__version_bcd), (__country_code), (__report_desc_len),     \
                    (__ep_in), (__ep_in_size), (__ep_in_interval),              \
                    (__ep_out), (__ep_out_size), (__ep_out_interval)            \
            )

#define __usbd_hid_func(__name, __func_id,                                      \
                    __in_ep, __out_ep, __out_ep_size,                           \
                    __report_num, __reports, __has_report_id,                   \
                    __report_desc, __report_desc_len,                           \
                    __notify_eda, __notifier)                                   \
            uint8_t __##__name##_HID##_rx_buffer[__out_ep_size];                \
            const vk_usbd_desc_t __##__name##_HID##__desc[1] = {                \
                VSF_USBD_DESC_HID_REPORT((__report_desc), (__report_desc_len)), \
            };                                                                  \
            vk_usbd_hid_t __##__name##_HID##__func_id = {                       \
                USB_HID_PARAM((__in_ep), (__out_ep),                            \
                    (__report_num), (__reports), (__has_report_id),             \
                    dimof(__##__name##_HID##__desc), (__##__name##_HID##__desc),\
                    __##__name##_HID##_rx_buffer, __out_ep_size,                \
                    __notify_eda, __notifier)                                   \
            };

#define __usbd_hid_ifs(__name, __func_id)                                       \
            USB_HID_IFS(__##__name##_HID##__func_id)

#define usbd_hid_desc(__name, __ifs, __i_func,                                  \
                    __subclass, __protocol,                                     \
                    __version_bcd, __country_code, __report_desc_len,           \
                    __ep_in, __ep_in_size, __ep_in_interval,                    \
                    __ep_out, __ep_out_size, __ep_out_interval)                 \
            __usbd_hid_desc(__name, (__ifs), (__i_func),                        \
                    (__subclass), (__protocol),                                 \
                    (__version_bcd), (__country_code), (__report_desc_len),     \
                    (__ep_in), (__ep_in_size), (__ep_in_interval),              \
                    (__ep_out), (__ep_out_size), (__ep_out_interval))
#define usbd_hid_desc_iad(__name, __ifs, __i_func,                              \
                    __subclass, __protocol,                                     \
                    __version_bcd, __country_code, __report_desc_len,           \
                    __ep_in, __ep_in_size, __ep_in_interval,                    \
                    __ep_out, __ep_out_size, __ep_out_interval)                 \
            __usbd_hid_desc_iad(__name, (__ifs), (__i_func),                    \
                    (__subclass), (__protocol),                                 \
                    (__version_bcd), (__country_code), (__report_desc_len),     \
                    (__ep_in), (__ep_in_size), (__ep_in_interval),              \
                    (__ep_out), (__ep_out_size), (__ep_out_interval))
#define usbd_hid_desc_table(__name, __func_id)                                  \
            usbd_func_str_desc_table(__name, __func_id)

#define usbd_hid_func(__name, __func_id,                                        \
                    __in_ep, __out_ep, __out_ep_size,                           \
                    __report_num, __reports, __has_report_id,                   \
                    __report_desc, __report_desc_size,                          \
                    __notify_eda, __notifier)                                   \
            __usbd_hid_func(__name, __func_id,                                  \
                    (__in_ep), (__out_ep), (__out_ep_size),                     \
                    (__report_num), (__reports), (__has_report_id),             \
                    (__report_desc), (__report_desc_size),                      \
                    (__notify_eda), (__notifier))
#define usbd_hid_ifs(__name, __func_id)                                         \
            __usbd_hid_ifs(__name, __func_id)

#define VSF_USBD_DESC_HID_REPORT(__PTR, __SIZE)                                 \
    {USB_HID_DT_REPORT, 0, 0, (__SIZE), (uint8_t*)(__PTR)}
#define VSF_USBD_HID_REPORT(__TYPE, __ID, __BUFFER, __SIZE, __IDLE)             \
    {(__TYPE), {(__BUFFER), (__SIZE)}, (__ID), (__IDLE)}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_usbd_hid_report_t) {

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

vsf_class(vk_usbd_hid_t) {

    public_member(
        uint8_t ep_out;
        uint8_t ep_in;
        uint8_t num_of_report;

        uint8_t has_report_id   : 1;
        uint8_t notify_eda      : 1;
        uint8_t desc_num        : 6;

        vsf_mem_t rx_buffer;
        vk_usbd_hid_report_t *reports;
        vk_usbd_desc_t *desc;

        union {
            vsf_err_t (*on_report)(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);
            vsf_eda_t *eda;
            void *notifier;
        };
    )

    private_member(
        uint8_t protocol;
        uint8_t cur_report;
        uint8_t cur_in_id;

        uint8_t busy : 1;
        uint8_t report_from_control : 1;

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

extern bool vk_usbd_hid_in_report_can_update(vk_usbd_hid_report_t *report);
extern void vk_usbd_hid_in_report_changed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);
extern void vk_usbd_hid_out_report_processed(vk_usbd_hid_t *hid, vk_usbd_hid_report_t *report);

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./vsf_usbd_hidmsc.h"

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_HID
#endif      // __VSF_USBD_HID_H__
