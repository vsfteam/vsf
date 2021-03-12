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

#ifndef __VSF_USB_CDCACM_H__
#define __VSF_USB_CDCACM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_cdcacm_line_coding_t usb_cdcacm_line_coding_t;
struct usb_cdcacm_line_coding_t {
    uint32_t bitrate;
    uint8_t stop;
    uint8_t parity;
    uint8_t datalen;
} PACKED;

#define USB_CDCACM_CONTROLLINE_RTS          0x02
#define USB_CDCACM_CONTROLLINE_DTR          0x01
#define USB_CDCACM_CONTROLLINE_MASK         0x03

typedef enum usb_cdcacm_req_t {
    USB_CDCACM_REQ_SET_LINE_CODING          = 0x20,
    USB_CDCACM_REQ_GET_LINE_CODING          = 0x21,
    USB_CDCACM_REQ_SET_CONTROL_LINE_STATE   = 0x22,
    USB_CDCACM_REQ_SEND_BREAK               = 0x23,
} usb_cdcacm_req_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // __VSF_USB_CDCACM_H__
