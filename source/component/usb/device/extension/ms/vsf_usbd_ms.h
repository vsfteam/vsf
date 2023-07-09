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

#ifndef __VSF_USBD_MSEXT_H__
#define __VSF_USBD_MSEXT_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_MSOS10_GENRE                    1
#define USB_MSOS10_EXT_COMPATID_INDEX       4
#define USB_MSOS10_EXT_PROPERTIES_INDEX     5

#define USB_MSOS10_EXT_COMPATID_HEADER_LEN  16
#define USB_MSOS10_EXT_COMPATID_FUNC_LEN    24

#define __usbd_msos10_compatid_func_desc(__first_ifs, ...)                      \
            (__first_ifs),                  /* bFirstInterfaceNumber */         \
            1,                              /* Reserved */                      \
            __VA_ARGS__

#define __usbd_msos10_compatid_desc(__name, __section_cnt, ...)                 \
        const uint8_t __##__name##_msos10_compatid_desc[                        \
                USB_MSOS10_EXT_COMPATID_HEADER_LEN + ((__section_cnt) * USB_MSOS10_EXT_COMPATID_FUNC_LEN)] = {\
            USB_DESC_DWORD(40),             /* dwLength */                      \
            USB_DESC_WORD(0x0100),          /* bcdVersion: 1.0 */               \
            USB_DESC_WORD(4),               /* wIndex: Extended compat ID descriptor index */\
            (__section_cnt),                /* bCount: Number of function sections */\
            0, 0, 0, 0, 0, 0, 0,            /* Reserved */                      \
            __VA_ARGS__                                                         \
        };

#define usbd_msos10_compatid_desc(__name, __section_cnt, ...)                   \
        __usbd_msos10_compatid_desc(__name, (__section_cnt), __VA_ARGS__)

#define __usbd_msos10_compatid_desc_buffer(__name)                              \
        &__##__name##_msos10_compatid_desc
#define __usbd_msos10_compatid_desc_len(__name)                                 \
        sizeof(__##__name##_msos10_compatid_desc)
#define usbd_msos10_compatid_desc_buffer(__name)                                \
        __usbd_msos10_compatid_desc_buffer(__name)
#define usbd_msos10_compatid_desc_len(__name)                                   \
        __usbd_msos10_compatid_desc_len(__name)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_MSEXT_H__
