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

#ifndef __VSF_USBD_HID_DESC_H__
#define __VSF_USBD_HID_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_HID_SUBCLASS_NONE                   0
#define USB_HID_SUBCLASS_BOOT                   1

#define USB_HID_PROTOCOL_NONE                   0
#define USB_HID_PROTOCOL_KEYBOARD               1
#define USB_HID_PROTOCOL_MOUSE                  2

#define USB_DT_HID_SIZE                         9
#define USB_DESC_HID(__IFS, __I_FUNC, __SUBCLASS, __PROTOCOL, __VERSION_BCD, __COUNTRY_CODE, __REPORT_DESC_LEN,\
                    __EP_IN, __EP_IN_SIZE, __EP_IN_INTERVAL,                    \
                    __EP_OUT, __EP_OUT_SIZE, __EP_OUT_INTERVAL)                 \
            USB_DESC_IFS((__IFS), 0, 2, USB_CLASS_HID, (__SUBCLASS), (__PROTOCOL), (__I_FUNC))\
            USB_DT_HID_SIZE,                                                    \
            USB_DT_HID,                                                         \
            USB_DESC_WORD(__VERSION_BCD),       /* bcdHID: HID Class Specification release in BCD */\
            (__COUNTRY_CODE),                   /* bCountryCode */              \
            (1),                                /* bNumDescriptor: number of class descriptors */\
            USB_DT_REPORT,                      /* bDescriptorType */           \
            USB_DESC_WORD(__REPORT_DESC_LEN),   /* wDescriptorLen */            \
            USB_DESC_EP(USB_DIR_IN | (__EP_IN), USB_ENDPOINT_XFER_INT, 64, __EP_IN_INTERVAL)\
            USB_DESC_EP(USB_DIR_OUT | (__EP_OUT), USB_ENDPOINT_XFER_INT, 64, __EP_OUT_INTERVAL)
#define USB_DESC_HID_LEN                                                        \
            (   USB_DESC_IFS_LEN + USB_DT_HID_SIZE + 2 * USB_DESC_EP_LEN)

#define USB_DESC_HID_IAD(__IFS, __I_FUNC, __SUBCLASS, __PROTOCOL, __VERSION_BCD, __COUNTRY_CODE, __REPORT_DESC_LEN,\
                        __EP_IN, __EP_IN_SIZE, __EP_IN_INTERVAL,                \
                        __EP_OUT, __EP_OUT_SIZE, __EP_OUT_INTERVAL)             \
            USB_DESC_IAD((__IFS), 1, USB_CLASS_HID, (__SUBCLASS), (__PROTOCOL), (__I_FUNC))\
            USB_DESC_HID((__IFS), (__I_FUNC), (__SUBCLASS), (__PROTOCOL), (__VERSION_BCD), (__COUNTRY_CODE), (__REPORT_DESC_LEN),\
                        (__EP_IN), (__EP_IN_SIZE), (__EP_IN_INTERVAL),          \
                        (__EP_OUT), (__EP_OUT_SIZE), (__EP_OUT_INTERVAL))
#define USB_DESC_HID_IAD_LEN                                                    \
            (USB_DT_INTERFACE_SIZE + USB_DESC_HID_LEN)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USBD_HID_DESC_H__
