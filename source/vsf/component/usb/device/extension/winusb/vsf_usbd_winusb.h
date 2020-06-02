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

#ifndef __VSF_USBD_WINUSB_H__
#define __VSF_USBD_WINUSB_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_BOS_CAP_WINUSB_LEN                                                  \
            USB_DESC_BOS_CAPABILITY_LEN(8)

#define MSOS20_FEATURE_WINUSB_SIZE                                              \
            (   USB_DT_MSOS20_SUBSET_HEADER_CONFIGURATION_SIZE +                \
                USB_DT_MSOS20_FEATURE_COMPATIBLE_ID_SIZE +                      \
                USB_DESC_MSOS20_FEATURE_REG_PROPERTY_LEN(                       \
                    MSOS20_REG_PROPERTY_NAME_LEN___DEVICE_INTERFACE_GUIDS___,   \
                    MSOS20_REG_PROPERTY_DATA_LEN___DEVICE_INTERFACE_GUIDS___    \
                )                                                               \
            )

#define MSOS20_REQ_DISCRIPTOR_INDEX                                 7
#define MSOS20_REQ_SET_ALT_ENUMERATION                              8

#define USB_DT_MSOS20_SET_HEADER_SIZE                               10
#define USB_DT_MSOS20_SUBSET_HEADER_CONFIGURATION_SIZE              8
#define USB_DT_MSOS20_FEATURE_COMPATIBLE_ID_SIZE                    20

#define MSOS20_REG_PROPERTY_NAME_LEN___DEVICE_INTERFACE_GUIDS___    42
#define MSOS20_REG_PROPERTY_DATA_LEN___DEVICE_INTERFACE_GUIDS___    80
#define MSOS20_REG_PROPERTY_NAME___DEVICE_INTERFACE_GUIDS___                    \
            'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,                                \
            'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,              \
            'G',0,'U',0,'I',0,'D',0,'s',0,0,0,

#define MSOS20_REG_PROPERTY_TYPE_REG_MULTI_SZ                       7

#define USB_DT_MSOS20_SET_HEADER                                    0x00
#define USB_DT_MSOS20_SUBSET_HEADER_CONFIGURATION                   0x01
#define USB_DT_MSOS20_SUBSET_HEADER_FUNCTION                        0x02
#define USB_DT_MSOS20_FEATURE_COMPATIBLE_ID                         0x03
#define USB_DT_MSOS20_FEATURE_REG_PROPERTY                          0x04
#define USB_DT_MSOS20_FEATURE_MIN_RESUME_TIME                       0x05
#define USB_DT_MSOS20_FEATURE_MODEL_ID                              0x06
#define USB_DT_MSOS20_FEATURE_CCGP_DEVICE                           0x07
#define USB_DT_MSOS20_FEATURE_VENDOR_REVISION                       0x08

/*============================ MACROFIED FUNCTIONS ===========================*/

#define USB_DESC_MSOS20_SET_HEADER(__TOTAL_LEN)                                 \
            USB_DESC_WORD(USB_DT_MSOS20_SET_HEADER_SIZE),                       \
            USB_DESC_WORD(USB_DT_MSOS20_SET_HEADER),                            \
            0x00, 0x00, 0x03, 0x06,         /* dwWindowsVersion: Win8.1+ */     \
            USB_DESC_WORD(__TOTAL_LEN),

#define USB_DESC_MSOS20_SUBSET_FUNCTION(__FIRST_IFS, __LEN, ...)                \
            USB_DESC_WORD(USB_DT_MSOS20_SUBSET_HEADER_CONFIGURATION_SIZE),      \
            USB_DESC_WORD(USB_DT_MSOS20_SUBSET_HEADER_FUNCTION),                \
            (__FIRST_IFS),                                                      \
            0,                                                                  \
            USB_DESC_WORD(USB_DT_MSOS20_SUBSET_HEADER_CONFIGURATION_SIZE + (__LEN)),\
            __VA_ARGS__

#define USB_DESC_MSOS20_FEATURE_COMPATIBLE_ID(...)                              \
            USB_DESC_WORD(USB_DT_MSOS20_FEATURE_COMPATIBLE_ID_SIZE),            \
            USB_DESC_WORD(USB_DT_MSOS20_FEATURE_COMPATIBLE_ID),                 \
            __VA_ARGS__

#define USB_DESC_MSOS20_FEATURE_REG_PROPERTY_LEN(__NAME_LEN, __DATA_LEN)        \
            (10 + (__NAME_LEN) + (__DATA_LEN))
#define USB_DESC_MSOS20_FEATURE_REG_PROPERTY(__NAME_LEN, __NAME, __DATA_LEN, ...)\
            USB_DESC_WORD(USB_DESC_MSOS20_FEATURE_REG_PROPERTY_LEN((__NAME_LEN), (__DATA_LEN))),\
            USB_DESC_WORD(USB_DT_MSOS20_FEATURE_REG_PROPERTY),                  \
            USB_DESC_WORD(MSOS20_REG_PROPERTY_TYPE_REG_MULTI_SZ),               \
            USB_DESC_WORD(__NAME_LEN),                                          \
            __NAME                                                              \
            USB_DESC_WORD(__DATA_LEN),                                          \
            __VA_ARGS__

#define USB_DESC_BOS_CAPABILITY_MSOS20(__len)                                   \
            USB_DESC_BOS_CAPABILITY_HEADER(4 + (__len), 5,                      \
                0xDF, 0x60, 0xDD, 0xD8,     /* uuid */                          \
                0x89, 0x45, 0xC7, 0x4C,                                         \
                0x9C, 0xD2, 0x65, 0x9D,                                         \
                0x9E, 0x64, 0x8A, 0x9F,                                         \
            )                                                                   \
            0x00, 0x00, 0x03, 0x06,         /* dwWindowsVersion: Win8.1+ */

#define __usbd_msos20_desc(__name, __desc_size, ...)                            \
        enum {                                                                  \
            __##__name##_msos20_desc_len = USB_DT_MSOS20_SET_HEADER_SIZE + (__desc_size),\
        };                                                                      \
        const uint8_t __##__name##_msos20_desc[__##__name##_msos20_desc_len] = {\
            USB_DESC_MSOS20_SET_HEADER(__##__name##_msos20_desc_len)            \
            __VA_ARGS__                                                         \
        };

#define usbd_msos20_desc(__name, __desc_size, ...)                              \
        __usbd_msos20_desc(__name, __desc_size, __VA_ARGS__)

#define __usbd_msos20_desc_buffer(__name)   __##__name##_msos20_desc
#define __usbd_msos20_desc_len(__name)      __##__name##_msos20_desc_len
#define usbd_msos20_desc_buffer(__name)     __usbd_msos20_desc_buffer(__name)
#define usbd_msos20_desc_len(__name)        __usbd_msos20_desc_len(__name)

#define __msos20_feature_winusb(__first_ifs, ...)                               \
            USB_DESC_MSOS20_SUBSET_FUNCTION(__first_ifs,                        \
                USB_DT_MSOS20_FEATURE_COMPATIBLE_ID_SIZE +                      \
                    USB_DESC_MSOS20_FEATURE_REG_PROPERTY_LEN(                   \
                        MSOS20_REG_PROPERTY_NAME_LEN___DEVICE_INTERFACE_GUIDS___,\
                        MSOS20_REG_PROPERTY_DATA_LEN___DEVICE_INTERFACE_GUIDS___\
                    ),                                                          \
                USB_DESC_MSOS20_FEATURE_COMPATIBLE_ID(                          \
                    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,                         \
                    0, 0, 0, 0, 0, 0, 0, 0,                                     \
                )                                                               \
                USB_DESC_MSOS20_FEATURE_REG_PROPERTY(                           \
                    MSOS20_REG_PROPERTY_NAME_LEN___DEVICE_INTERFACE_GUIDS___,   \
                    MSOS20_REG_PROPERTY_NAME___DEVICE_INTERFACE_GUIDS___,       \
                    MSOS20_REG_PROPERTY_DATA_LEN___DEVICE_INTERFACE_GUIDS___,   \
                    __VA_ARGS__                                                 \
                )                                                               \
                0,0,0,0,                                                        \
            )

// msos20_feature_winusb(__fisrst_ifs, __guid)
#define msos20_feature_winusb(__first_ifs, ...)                                 \
        __msos20_feature_winusb((__first_ifs), __VA_ARGS__)

#define __bos_desc_winusb(__vendor_code, __alt_enum_code, __desc_len)           \
            USB_DESC_BOS_CAPABILITY_MSOS20(4)                                   \
            USB_DESC_WORD(__desc_len),      /* wMSOSDescriptorSetTotalLength */ \
            (__vendor_code),                /* bMS_VendorCode */                \
            (__alt_enum_code),              /* bAltEnumCode */

#define bos_desc_winusb(__vendor_code, __alt_enum_code, __desc_len)             \
        __bos_desc_winusb((__vendor_code), (__alt_enum_code), (__desc_len))

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_WINUSB_H__
