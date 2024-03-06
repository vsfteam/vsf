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

#ifndef __VSF_USB_UVC_H__
#define __VSF_USB_UVC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     ((VSF_USE_USB_DEVICE == ENABLED) && (VSF_USBD_USE_UVC == ENABLED))      \
    ||  ((VSF_USE_USB_HOST == ENABLED) && (VSF_USBH_USE_UVC == ENABLED))

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usb_uvc_req_t {
    USB_UVC_REQ_CUR     = 1,
    USB_UVC_REQ_MIN     = 2,
    USB_UVC_REQ_MAX     = 3,
    USB_UVC_REQ_RES     = 4,
    USB_UVC_REQ_LEN     = 5,
    USB_UVC_REQ_INFO    = 6,
    USB_UVC_REQ_DEF     = 7,
    USB_UVC_REQ_ALL     = 0x10,
    USB_UVC_REQ_SET     = 0x00,
    USB_UVC_REQ_GET     = 0x80,
} usb_uvc_req_t;

// descriptors

typedef struct usb_uvc_desc_header_t usb_uvc_desc_header_t;
struct usb_uvc_desc_header_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
};

typedef struct usb_uvc_vc_interface_header_desc_t usb_uvc_vc_interface_header_desc_t;
struct usb_uvc_vc_interface_header_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdVDC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t bInCollection;
    uint8_t baInterfaceNr[1];
} VSF_CAL_PACKED;

typedef struct usb_uvc_vs_interface_header_desc_t usb_uvc_vs_interface_header_desc_t;
struct usb_uvc_vs_interface_header_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bNumFormats;
    uint16_t wTotalLength;
    uint8_t bEndpointAddress;
    uint8_t bmInfo;
    uint8_t bTerminalLink;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
    uint8_t bControlSize;
    uint8_t bmControls[0];
} VSF_CAL_PACKED;

typedef struct usb_uvc_format_desc_t usb_uvc_format_desc_t;
struct usb_uvc_format_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFormatIndex;
    uint8_t bNumFrameDescriptors;
    union {
        struct {
            uint32_t guidFormat;
            uint8_t bBitsPerPixel;
            uint8_t bDefaultFrameIndex;
            uint8_t bAspectRatioX;
            uint8_t bAspectRatioY;
            uint8_t bmInterlaceFlags;
            uint8_t bCopyProtect;
        } VSF_CAL_PACKED uncompressed;
        struct {
            uint8_t bmFlags;
            uint8_t bDefaultFrameIndex;
            uint8_t bAspectRatioX;
            uint8_t bAspectRatioY;
            uint8_t bmInterlaceFlags;
            uint8_t bCopyProtect;
        } VSF_CAL_PACKED mjpeg;
    } VSF_CAL_PACKED;
} VSF_CAL_PACKED;

typedef struct usb_uvc_frame_desc_t usb_uvc_frame_desc_t;
struct usb_uvc_frame_desc_t {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bFrameIndex;
    uint8_t bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t bFrameIntervalType;
    uint32_t dwFrameInterval[0];
} VSF_CAL_PACKED;

// Stream Controls
typedef struct usb_uvc_vs_t usb_uvc_vs_t;
struct usb_uvc_vs_t {
    uint16_t bmHint;
    uint8_t bFormatIndex;
    uint8_t bFrameIndex;
    uint32_t dwFrameInterval;
    uint16_t wKeyFrameRate;
    uint16_t wPFrameRate;
    uint16_t wCompQuality;
    uint16_t wCompWindowSize;
    uint16_t wDelay;
    uint32_t dwMaxVideoFrameSize;
    uint32_t dwMaxPayloadTransferSize;
    uint32_t dwClockFrequency;
    uint8_t bmFramingInfo;
    uint8_t bPreferedVersion;
    uint8_t bMinVersion;
    uint8_t bMaxVersion;
    uint8_t bUsage;
    uint8_t bBitDepthLuma;
    uint8_t bmSettings;
    uint8_t bMaxNumberOfFramesPlus1;
    uint16_t bmRateControlModes;
    uint64_t bmLayoutPerStream;
} VSF_CAL_PACKED;

// Camera Terminal
typedef struct usb_uvc_ct_roi_t usb_uvc_ct_roi_t;
struct usb_uvc_ct_roi_t {
    uint16_t wROI_Top;
    uint16_t wROI_Left;
    uint16_t wROI_Bottom;
    uint16_t wROI_Right;
    uint16_t bmAutoControls;
} VSF_CAL_PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // (VSF_USE_USB_DEVICE && VSF_USBD_USE_UVC) || (VSF_USE_USB_HOST && VSF_USBH_USE_UVC)
#endif      // __VSFUSB_UVC_H__
