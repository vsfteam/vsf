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

#ifndef __VSF_USB_DFU_H__
#define __VSF_USB_DFU_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usb_dfu_req_t {
    USB_DFUREQ_DETACH       = 0x00,
    USB_DFUREQ_DNLOAD       = 0x01,
    USB_DFUREQ_UPLOAD       = 0x02,
    USB_DFUREQ_GETSTATUS    = 0x03,
    USB_DFUREQ_CLRSTATUS    = 0x04,
    USB_DFUREQ_GETSTATE     = 0x05,
    USB_DFUREQ_ABORT        = 0x06,
} usb_dfu_req_t;

typedef enum usb_dfu_desc_t {
    USB_DFU_FUNCTIONAL_DESC = 0x21,
} usb_dfu_desc_t;

typedef struct usb_dfu_functional_descriptor_t usb_dfu_functional_descriptor_t;
struct usb_dfu_functional_descriptor_t {
    uint8_t bLength;
    uint8_t bDescriptorType;

    union {
        uint8_t bmAttributes;
        struct {
            uint8_t bitCanDnload                : 1;
            uint8_t bitCanUpload                : 1;
            uint8_t bitManifestationTolerant    : 1;
            uint8_t bitWillDetach               : 1;
        };
    } PACKED;
    uint16_t wDetachTimeOut;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} PACKED;

typedef struct usb_dfu_status_t usb_dfu_status_t;
struct usb_dfu_status_t {
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_DFU_H__
