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

#ifndef __VSF_USB_DFU_H__
#define __VSF_USB_DFU_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define USB_DFU_PROTOCOL_APP                1
#define USB_DFU_PROTOCOL_DFU                2

#define USB_DT_DFU_FUNCTIONAL_SIZE          9

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
    USB_DT_DFU_FUNCTIONAL   = (USB_TYPE_CLASS | 0x01),
} usb_dfu_desc_t;

typedef enum usb_dfu_state_t {
    DFU_appIDLE             = 0,
    DFU_appDETACH           = 1,
    DFU_dfuIDLE             = 2,
    DFU_dfuDNLOAD_SYNC      = 3,
    DFU_dfuDNBUSY           = 4,
    DFU_dfuDNLOAD_IDLE      = 5,
    DFU_dfuMANIFEST_SYNC    = 6,
    DFU_dfuMANIFEST         = 7,
    DFU_dfuMANIFEST_WAIT_RESET  = 8,
    DFU_dfuUPLOAD_IDLE      = 9,
    DFU_dfuERROR            = 10,
} usb_dfu_state_t;

enum {
    DFU_OK                      = 0,
    DFU_errTARGET               = 1,
    DFU_errFILE                 = 2,
    DFU_errWRITE                = 3,
    DFU_errERASE                = 4,
    DFU_errCHECK_ERASED         = 5,
    DFU_errPROG                 = 6,
    DFU_erVERIFY                = 7,
    DFU_errADDRESS              = 8,
    DFU_errNOTDONE              = 9,
    DFU_errFIRMWARE             = 10,
    DFU_errVENDOR               = 11,
    DFU_errUSBR                 = 12,
    DFU_errPOR                  = 13,
    DFU_errUNKNOWN              = 14,
    DFU_errSTALLEDPKT           = 15,
};

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
    } VSF_CAL_PACKED;
    uint16_t wDetachTimeOut;
    uint16_t wTransferSize;
    uint16_t bcdDFUVersion;
} VSF_CAL_PACKED;

typedef struct usb_dfu_status_t usb_dfu_status_t;
struct usb_dfu_status_t {
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
} VSF_CAL_PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_DFU_H__
