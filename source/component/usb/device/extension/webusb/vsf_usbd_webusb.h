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

#ifndef __VSF_USBD_WEBUSB_H__
#define __VSF_USBD_WEBUSB_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_BOS_CAP_WEBUSB_LEN              USB_DESC_BOS_CAPABILITY_LEN(4)

#define USB_DT_WEBUSB_URL                   0x03
#define USB_REQ_WEBUSB_GET_URL              0x02

#define WEBUSB_URL_PREFIX_HTTP              0x00
#define WEBUSB_URL_PREFIX_HTTPS             0x01
#define WEBUSB_URL_PREFIX_NONE              0xFF

/*============================ MACROFIED FUNCTIONS ===========================*/

#define USB_WEBUSB_URL_DESC(__PREFIX, __URL_LEN, ...)                           \
                3 + (__URL_LEN),                                                \
                USB_DT_WEBUSB_URL,                                              \
                (__PREFIX),                                                     \
                __VA_ARGS__

#define __bos_desc_webusb(__vendor_code, __landing_page)                        \
            USB_DESC_BOS_CAPABILITY_HEADER(4, 5,                                \
                0x38, 0xB6, 0x08, 0x34,     /* uuid: winusb */                  \
                0xA9, 0x09, 0xA0, 0x47,                                         \
                0x8B, 0xFD, 0xA0, 0x76,                                         \
                0x88, 0x15, 0xB6, 0x65,                                         \
            )                                                                   \
            USB_DESC_WORD(0x0100),          /* bcdVersion */                    \
            (__vendor_code),                /* bVendorCode */                   \
            (__landing_page),               /* iLandingPage */

#define bos_desc_webusb(__vendor_code, __landing_page)                          \
        __bos_desc_webusb((__vendor_code), (__landing_page))

#define __usbd_webusb_url_desc(__name, __landing_page, __prefix, __url_len, __url)\
        struct {                                                                \
            uint8_t bLength;                                                    \
            uint8_t bDescriptorType;                                            \
            uint8_t bScheme;                                                    \
            char url_utf8[(__url_len) + 3];                                     \
        } PACKED const __##__name##_webusb_url##__landing_page##_desc = {       \
            .bLength            = 6 + (__url_len),                              \
            .bDescriptorType    = USB_DT_WEBUSB_URL,                            \
            .bScheme            = (__prefix),                                   \
            .url_utf8           = (__url),                                      \
        };                                                                      \

#define usbd_webusb_url_desc(__name, __landing_page, __prefix, __url_len, __url)\
        __usbd_webusb_url_desc(__name, __landing_page, (__prefix), (__url_len), (__url))

#define __usbd_webusb_url_desc_buffer(__name, __index)                          \
        &__##__name##_webusb_url##__index##_desc
#define __usbd_webusb_url_desc_len(__name, __index)                             \
        sizeof(__##__name##_webusb_url##__index##_desc)
#define usbd_webusb_url_desc_buffer(__name, __index)                            \
        __usbd_webusb_url_desc_buffer(__name, __index)
#define usbd_webusb_url_desc_len(__name, __index)                               \
        __usbd_webusb_url_desc_len(__name, __index)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_WEBUSB_H__
