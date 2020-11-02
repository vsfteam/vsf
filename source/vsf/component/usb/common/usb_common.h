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

#ifndef __USB_COMMON_H__
#define __USB_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define DeviceRequest           ((USB_DIR_IN  | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8)
#define DeviceOutRequest        ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE) << 8)
#define InterfaceRequest        ((USB_DIR_IN  | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)
#define EndpointRequest         ((USB_DIR_IN  | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)
#define EndpointOutRequest      ((USB_DIR_OUT | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8)

#define ClearHubFeature         (0x2000 | USB_REQ_CLEAR_FEATURE)
#define ClearPortFeature        (0x2300 | USB_REQ_CLEAR_FEATURE)
#define GetHubDescriptor        (0xa000 | USB_REQ_GET_DESCRIPTOR)
#define GetHubStatus            (0xa000 | USB_REQ_GET_STATUS)
#define GetPortStatus           (0xa300 | USB_REQ_GET_STATUS)
#define SetHubFeature           (0x2000 | USB_REQ_SET_FEATURE)
#define SetPortFeature          (0x2300 | USB_REQ_SET_FEATURE)

#define USB_ENDPOINT_HALT       0   /* IN/OUT will STALL */

#define USB_DT_DEVICE_SIZE                  18
#define USB_DT_CONFIG_SIZE                  9
#define USB_DT_INTERFACE_SIZE               9
#define USB_DT_ENDPOINT_SIZE                7
#define USB_DT_ENDPOINT_AUDIO_SIZE          9
#define USB_DT_DEVICE_QUALIFIER_SIZE        10
#define USB_DT_INTERFACE_ASSOCIATION_SIZE   8
#define USB_DT_BOS_SIZE                     5

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum usb_pid_t {
    USB_PID_EXT                 = 0xf0,     /* USB 2.0 LPM ECN */
    USB_PID_OUT                 = 0xe1,
    USB_PID_ACK                 = 0xd2,
    USB_PID_DATA0               = 0xc3,
    USB_PID_PING                = 0xb4,     /* USB 2.0 */
    USB_PID_SOF                 = 0xa5,
    USB_PID_NYET                = 0x96,     /* USB 2.0 */
    USB_PID_DATA2               = 0x87,     /* USB 2.0 */
    USB_PID_SPLIT               = 0x78,     /* USB 2.0 */
    USB_PID_IN                  = 0x69,
    USB_PID_NAK                 = 0x5a,
    USB_PID_DATA1               = 0x4b,
    USB_PID_PREAMBLE            = 0x3c,     /* Token mode */
    USB_PID_ERR                 = 0x3c,     /* USB 2.0: handshake mode */
    USB_PID_SETUP               = 0x2d,
    USB_PID_STALL               = 0x1e,
    USB_PID_MDATA               = 0x0f,     /* USB 2.0 */
} usb_pid_t;

enum usb_dir_t {
    USB_DIR_MASK                = 0x80,
    USB_DIR_OUT                 = 0x00,     /* to device */
    USB_DIR_IN                  = 0x80,     /* to host */
};

typedef struct usb_ctrlrequest_t {
    uint8_t  bRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} PACKED usb_ctrlrequest_t;

enum usb_req_t {
    USB_REQ_GET_STATUS          = 0x00,
    USB_REQ_CLEAR_FEATURE       = 0x01,
    USB_REQ_SET_FEATURE         = 0x03,
    USB_REQ_SET_ADDRESS         = 0x05,
    USB_REQ_GET_DESCRIPTOR      = 0x06,
    USB_REQ_SET_DESCRIPTOR      = 0x07,
    USB_REQ_GET_CONFIGURATION   = 0x08,
    USB_REQ_SET_CONFIGURATION   = 0x09,
    USB_REQ_GET_INTERFACE       = 0x0A,
    USB_REQ_SET_INTERFACE       = 0x0B,
    USB_REQ_SYNCH_FRAME         = 0x0C,
};

enum usb_req_type_t {
    USB_TYPE_MASK               = (0x03 << 5),
    USB_TYPE_STANDARD           = (0x00 << 5),
    USB_TYPE_CLASS              = (0x01 << 5),
    USB_TYPE_VENDOR             = (0x02 << 5),
    USB_TYPE_RESERVED           = (0x03 << 5),
};

enum usb_req_recip_t {
    USB_RECIP_MASK              = 0x1F,
    USB_RECIP_DEVICE            = 0x00,
    USB_RECIP_INTERFACE         = 0x01,
    USB_RECIP_ENDPOINT          = 0x02,
    USB_RECIP_OTHER             = 0x03,
    /* From Wireless USB 1.0 */
    USB_RECIP_PORT              = 0x04,
    USB_RECIP_RPIPE             = 0x05,
};

enum usb_class_t {
    USB_CLASS_PER_INTERFACE     = 0,    /* for DeviceClass */
    USB_CLASS_AUDIO             = 1,
    USB_CLASS_COMM              = 2,
    USB_CLASS_HID               = 3,
    USB_CLASS_PHYSICAL          = 5,
    USB_CLASS_STILL_IMAGE       = 6,
    USB_CLASS_PRINTER           = 7,
    USB_CLASS_MASS_STORAGE      = 8,
    USB_CLASS_HUB               = 9,
    USB_CLASS_CDC_DATA          = 0x0a,
    USB_CLASS_CSCID             = 0x0b, /* chip+ smart card */
    USB_CLASS_CONTENT_SEC       = 0x0d, /* content security */
    USB_CLASS_VIDEO             = 0x0e,
    USB_CLASS_WIRELESS_CONTROLLER   = 0xe0,
    USB_CLASS_MISC              = 0xef,
    USB_CLASS_APP_SPEC          = 0xfe,
    USB_CLASS_VENDOR_SPEC       = 0xff,

    USB_SUBCLASS_VENDOR_SPEC    = 0xff,
};

enum usb_desc_type_t {
    USB_DT_DEVICE               = 0x01,
    USB_DT_CONFIG               = 0x02,
    USB_DT_STRING               = 0x03,
    USB_DT_INTERFACE            = 0x04,
    USB_DT_ENDPOINT             = 0x05,
    USB_DT_DEVICE_QUALIFIER     = 0x06,
    USB_DT_OTHER_SPEED_CONFIG   = 0x07,
    USB_DT_INTERFACE_POWER      = 0x08,
    USB_DT_INTERFACE_ASSOCIATION= 0x0b,
    USB_DT_BOS                  = 0x0f,
    USB_DT_DEVICE_CAPABILITY    = 0x10,
    USB_DT_SS_ENDPOINT_COMP     = 0x30,

    USB_DT_CS_DEVICE            = (USB_TYPE_CLASS | USB_DT_DEVICE),
    USB_DT_CS_CONFIG            = (USB_TYPE_CLASS | USB_DT_CONFIG),
    USB_DT_CS_STRING            = (USB_TYPE_CLASS | USB_DT_STRING),
    USB_DT_CS_INTERFACE         = (USB_TYPE_CLASS | USB_DT_INTERFACE),
    USB_DT_CS_ENDPOINT          = (USB_TYPE_CLASS | USB_DT_ENDPOINT),

    USB_DT_HID                  = (USB_TYPE_CLASS | 0x01),
    USB_DT_REPORT               = (USB_TYPE_CLASS | 0x02),
    USB_DT_PHYSICAL             = (USB_TYPE_CLASS | 0x03),
    USB_DT_HUB                  = (USB_TYPE_CLASS | 0x09),
};

typedef struct usb_descriptor_header_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
} PACKED usb_descriptor_header_t;

typedef struct usb_class_interface_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
} PACKED usb_class_interface_desc_t;

typedef struct usb_device_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} PACKED usb_device_desc_t;

typedef struct usb_config_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
} PACKED usb_config_desc_t;

enum usb_config_desc_att_t {
    USB_CONFIG_ATT_ONE          = (1 << 7),     /* must be set */
    USB_CONFIG_ATT_SELFPOWER    = (1 << 6),     /* self powered */
    USB_CONFIG_ATT_WAKEUP       = (1 << 5),     /* can wakeup */
    USB_CONFIG_ATT_BATTERY      = (1 << 4),     /* battery powered */
};

enum usb_feature_t {
    USB_DEVICE_SELF_POWERED     = 0,            /* (read only) */
    USB_DEVICE_REMOTE_WAKEUP    = 1,            /* dev may initiate wakeup */
};

typedef struct usb_interface_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
} PACKED usb_interface_desc_t;

typedef struct usb_endpoint_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;

    /* NOTE:  these two are _only_ in audio endpoints. */
    /* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
    uint8_t  bRefresh;
    uint8_t  bSynchAddress;
} PACKED usb_endpoint_desc_t;

typedef struct usb_interface_assoc_desc_t {
    uint8_t  bLength;
    uint8_t  bDescriptorType;

    uint8_t  bFirstInterface;
    uint8_t  bInterfaceCount;
    uint8_t  bFunctionClass;
    uint8_t  bFunctionSubClass;
    uint8_t  bFunctionProtocol;
    uint8_t  iFunction;
} PACKED usb_interface_assoc_desc_t;

typedef enum usb_device_speed_t {
    USB_SPEED_UNKNOWN = 0,          /* enumerating */
    USB_SPEED_LOW, USB_SPEED_FULL,  /* usb 1.1 */
    USB_SPEED_HIGH,                 /* usb 2.0 */
    USB_SPEED_VARIABLE,             /* wireless (usb 2.5) */
    USB_SPEED_SUPER,                /* usb 3.0 */
} usb_device_speed_t;

enum usb_endpoint_sync_t {
    USB_ENDPOINT_SYNCTYPE       = 0x0c,
    USB_ENDPOINT_SYNC_NONE      = (0 << 2),
    USB_ENDPOINT_SYNC_ASYNC     = (1 << 2),
    USB_ENDPOINT_SYNC_ADAPTIVE  = (2 << 2),
    USB_ENDPOINT_SYNC_SYNC      = (3 << 2),
};

enum usb_endpoint_xfer_t {
    USB_ENDPOINT_XFERTYPE_MASK  = 0x03,
    USB_ENDPOINT_XFER_CONTROL   = 0,
    USB_ENDPOINT_XFER_ISOC      = 1,
    USB_ENDPOINT_XFER_BULK      = 2,
    USB_ENDPOINT_XFER_INT       = 3,
    USB_ENDPOINT_MAX_ADJUSTABLE = 0x80,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif // __USB_COMMON_H__
