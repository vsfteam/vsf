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

#ifndef __VSF_USB_CDC_H__
#define __VSF_USB_CDC_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usb_cdc_req_t {
    USB_CDCREQ_SEND_ENCAPSULATED_COMMAND    = 0x00,
    USB_CDCREQ_GET_ENCAPSULATED_RESPONSE    = 0x01,
    USB_CDCREQ_SET_COMM_FEATURE             = 0x02,
    USB_CDCREQ_GET_COMM_FEATURE             = 0x03,
    USB_CDCREQ_CLEAR_COMM_FEATURE           = 0x04,
} usb_cdc_req_t;

typedef struct usb_cdc_union_descriptor_t usb_cdc_union_descriptor_t;
struct usb_cdc_union_descriptor_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;

    uint8_t bControlInterface;
    uint8_t bSubordinateInterface[1];
} PACKED;

typedef struct usb_cdc_ecm_descriptor_t usb_cdc_ecm_descriptor_t;
struct usb_cdc_ecm_descriptor_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubType;

    uint8_t iMACAddress;
    uint8_t bmEthernetStatistics[4];
    uint16_t wMaxSegmentSize;
    uint16_t wNumberMCFilters;
    uint8_t bNumberPowerFilters;
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_CDC_H__
