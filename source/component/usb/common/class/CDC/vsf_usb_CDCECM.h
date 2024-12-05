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

#ifndef __VSF_USB_CDCECM_H__
#define __VSF_USB_CDCECM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_CDCECM_NOTIFICATION_NETWORK_CONNECTION          0x00
#define USB_CDCECM_NOTIFICATION_RESPONSE_AVAILABLE          0x01
#define USB_CDCECM_NOTIFICATION_CONNECTION_SPEED_CHANGE     0x2A

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_cdcecm_notification_speed_change_t usb_cdcecm_notification_speed_change_t;
struct usb_cdcecm_notification_speed_change_t {
    usb_ctrlrequest_t;
    uint32_t down;
    uint32_t up;
} VSF_CAL_PACKED;

typedef usb_ctrlrequest_t usb_cdcecm_notification_network_connection_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USB_CDCNCM_H__
