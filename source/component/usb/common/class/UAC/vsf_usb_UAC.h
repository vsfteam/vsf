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

#ifndef __VSF_USB_UAC_H__
#define __VSF_USB_UAC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     ((VSF_USE_USB_DEVICE == ENABLED) && (VSF_USBD_USE_UAC == ENABLED))      \
    ||  ((VSF_USE_USB_HOST == ENABLED) && (VSF_USBH_USE_UAC == ENABLED))

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usb_uac_req_t {
    USB_UAC_REQ_CUR             = 1,
    USB_UAC_REQ_MIN             = 2,
    USB_UAC_REQ_MAX             = 3,
    USB_UAC_REQ_RES             = 4,
    USB_UAC_REQ_MEM             = 5,
    USB_UAC_REQ_SET             = 0,
    USB_UAC_REQ_GET             = 0x80,
    
    USB_UAC2_REQ_CUR            = 1,
    USB_UAC2_REQ_RANGE          = 2,
    USB_UAC2_REQ_MEM            = 3,
} usb_uac_req_t;

typedef enum usb_uac_terminal_types_t {
    // USB Terminal Types
    USB_UAC_UTT_UNDEFINED       = 0x0100,
    USB_UAC_UTT_STREAMING       = 0x0101,
    USB_UAC_UTT_VENDOR          = 0x01FF,
    // Input Terminal Types
    USB_UAC_ITT_UNDEFINED       = 0x0200,
    USB_UAC_ITT_MIC             = 0x0201,
    USB_UAC_ITT_DESKTOP_MIC     = 0x0202,
    USB_UAC_ITT_PERSONAL_MIC    = 0x0203,
    USB_UAC_ITT_MIC_ARRAY       = 0x0205,
    // Output Terminal Types
    USB_UAC_OTT_UNDEFINED       = 0x0300,
    USB_UAC_OTT_SPEAKER         = 0x0301,
    USB_UAC_OTT_DESKTOP_SPEAKER = 0x0304,
    USB_UAC_OTT_ROOM_SPEAKER    = 0x0305,
    // Bi-directional Terminal Types
    USB_UAC_BTT_UNDEFINED       = 0x0400,
    USB_UAC_BTT_HANDSET         = 0X0401,
    USB_UAC_BTT_HEADSET         = 0x0402,
    USB_UAC_BTT_SPEAKERPHONE    = 0x0403,
    USB_UAC_BTT_SPEAKERPHONE_ES = 0x0404,
    USB_UAC_BTT_SPEAKERPHONE_EC = 0x0405,
    // Telephony Terminal Types
    USB_UAC_TTT_UNDEFINED       = 0x0500,
    USB_UAC_TTT_PHONE_LINE      = 0x0501,
    USB_UAC_TTT_TELEPHONE       = 0x0502,
    // External Terminal Types
    // Embedded Function Terminal Types
} usb_uac_terminal_types_t;

typedef enum usb_uac_format_tag_t {
    USB_UAC_FORMAT_PCM          = 0x0001,
} usb_uac_format_tag_t;

typedef struct usb_uac_ac_interface_header_desc_t usb_uac_ac_interface_header_desc_t;
struct usb_uac_ac_interface_header_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdADC;
    uint16_t wTotalLength;
    uint8_t bInCollection;
    uint8_t baInterfaceNr[1];
} PACKED;

typedef struct usb_uac_as_interface_desc_t usb_uac_as_interface_desc_t;
struct usb_uac_as_interface_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bTerminalLink;
    uint8_t bDelay;
    uint16_t wFormatTag;
} PACKED;

typedef struct usb_uac_as_format_i_desc_t usb_uac_as_format_i_desc_t;
struct usb_uac_as_format_i_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatType;
    uint8_t bNrChannels;
    uint8_t bSubframeSize;
    uint8_t bBitResolution;
    uint8_t bSamFreqType;
    uint8_t tSamFreq[1][3];
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // (VSF_USE_USB_DEVICE && VSF_USBD_USE_UAC) || (VSF_USE_USB_HOST && VSF_USBH_USE_UAC)
#endif      // __VSFUSB_UAC_H__
