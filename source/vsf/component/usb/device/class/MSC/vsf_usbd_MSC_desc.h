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

#ifndef __VSF_USBD_MSC_DESC_H__
#define __VSF_USBD_MSC_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// __INT_EP_INTERVAL:   HS 0x10, otherwise 0xFF
#define USB_DESC_MSC_IAD(__IFS, __SUB_CLASS, __PROTOCOL, __I_FUNC)\
            USB_DESC_IAD((__IFS), 1, USB_CLASS_MASS_STORAGE, __SUB_CLASS, __PROTOCOL, (__I_FUNC))\
            USB_DESC_IFS((__IFS), 0, 2, USB_CLASS_MASS_STORAGE, __SUB_CLASS, __PROTOCOL, (__I_FUNC))

#define USB_DESC_MSCBOT_IAD(__IFS, __I_FUNC, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE)\
            USB_DESC_MSC_IAD((__IFS), 0x06, 0x50, (__I_FUNC))\
            USB_DESC_EP(USB_DIR_IN | (__BULK_IN_EP), USB_ENDPOINT_XFER_BULK, __BULK_EP_SIZE, 0x00)\
            USB_DESC_EP(USB_DIR_OUT | (__BULK_OUT_EP), USB_ENDPOINT_XFER_BULK, __BULK_EP_SIZE, 0x00)

#define USB_DESC_MSCBOT_IAD_LEN                                                 \
            (   USB_DT_INTERFACE_ASSOCIATION_SIZE +                             \
                USB_DT_INTERFACE_SIZE +                                         \
                2 * USB_DT_ENDPOINT_SIZE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USBD_MSC_DESC_H__
