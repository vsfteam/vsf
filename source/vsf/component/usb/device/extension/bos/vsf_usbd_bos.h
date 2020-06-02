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

#ifndef __VSF_USBD_BOS_H__
#define __VSF_USBD_BOS_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_DESC_BOS_CAPABILITY_LEN(__LEN)  (20 + (__LEN))

#define USB_DESC_BOS_CAPABILITY_HEADER(__LEN, __TYPE, ...)                      \
            20 + (__LEN),                   /* bLength */                       \
            USB_DT_DEVICE_CAPABILITY,       /* bDescriptorType */               \
            (__TYPE),                       /* bDevCapabilityType */            \
            0x00,                           /* bReserved */                     \
            __VA_ARGS__

#define bos_desc_msos20(__len)              __bos_desc_msos20(__len)

#define __usbd_bos_desc(__name, __cap_num, __len, ...)                          \
        const uint8_t __##__name##_bos_desc[USB_DT_BOS_SIZE + (__len)] = {      \
            USB_DT_BOS_SIZE,                                                    \
            USB_DT_BOS,                                                         \
            USB_DESC_WORD(USB_DT_BOS_SIZE + (__len)),                           \
            (__cap_num),                                                        \
            __VA_ARGS__                                                         \
        };

#define __usbd_bos_desc_table(__name)                                           \
        {USB_DT_BOS, 0, 0, sizeof(__##__name##_bos_desc), (uint8_t*)(__##__name##_bos_desc)}

#define usbd_bos_desc(__name, __cap_num, __len, ...)                            \
        __usbd_bos_desc(__name, __cap_num, __len, __VA_ARGS__)
#define usbd_bos_desc_table(__name)                                             \
        __usbd_bos_desc_table(__name)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // VSF_USE_USB_DEVICE
#endif    // __VSF_USBD_BOS_H__
