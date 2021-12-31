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

#ifndef __VSF_USB_MSC_H__
#define __VSF_USB_MSC_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define USB_MSC_CBW_SIGNATURE           0x43425355
#define USB_MSC_CSW_SIGNATURE           0x53425355

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_msc_cbw_t usb_msc_cbw_t;
struct usb_msc_cbw_t {
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
} PACKED;

typedef struct usb_msc_csw_t usb_msc_csw_t;
struct usb_msc_csw_t {
    uint32_t dCSWSignature;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t dCSWStatus;
} PACKED;

typedef enum usb_msc_req_t {
    USB_MSC_REQ_GET_MAX_LUN = 0xFE,
    USB_MSC_REQ_RESET       = 0xFF,
} usb_msc_req_t;

typedef enum usb_msc_csw_status_t {
    USB_MSC_CSW_OK          = 0,
    USB_MSC_CSW_FAIL        = 1,
    USB_MSC_CSW_PHASE_ERROR = 2,
} usb_msc_csw_status_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_MSC_H__
