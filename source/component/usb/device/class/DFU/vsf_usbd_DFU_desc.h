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

#ifndef __VSF_USBD_DFU_DESC_H__
#define __VSF_USBD_DFU_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_DESC_DFU(__IFS, __I_FUNC, __PROTOCOL, __DFU_ATTR, __DETACH_TIMEOUT, __TRANSFER_SIZE)\
            USB_DESC_IFS((__IFS), 0, 0, 0xFE, 0x01, (__PROTOCOL), (__I_FUNC))   \
            USB_DT_DFU_FUNCTIONAL_SIZE,                                         \
            USB_DT_DFU_FUNCTIONAL,                                              \
            (__DFU_ATTR),                                                       \
            USB_DESC_WORD(__DETACH_TIMEOUT),                                    \
            USB_DESC_WORD(__TRANSFER_SIZE),                                     \
            USB_DESC_WORD(0x0110),
#define USB_DESC_DFU_LEN                        (USB_DESC_IFS_LEN + USB_DT_DFU_FUNCTIONAL_SIZE)

#define USB_DESC_DFU_IAD(__IFS, __I_FUNC, __PROTOCOL, __DFU_ATTR, __DETACH_TIMEOUT, __TRANSFER_SIZE)\
            USB_DESC_IAD((__IFS), 1, 0xFE, 0x01, (__PROTOCOL), (__I_FUNC))      \
            USB_DESC_DFU((__IFS), (__I_FUNC), (__PROTOCOL), (__DFU_ATTR), (__DETACH_TIMEOUT), (__TRANSFER_SIZE))
#define USB_DESC_DFU_IAD_LEN                    (USB_DESC_IAD_LEN + USB_DESC_DFU_LEN)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USBD_DFU_DESC_H__
