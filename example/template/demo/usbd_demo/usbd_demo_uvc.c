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

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_USB_DEVICE == ENABLED && APP_USE_USBD_DEMO == ENABLED && APP_USE_USBD_UVC_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_TIMER must be enabled for uvc
#endif

#define SCREEN_WIDTH                    256
#define SCREEN_HEIGHT                   256
#define SCREEN_FPS                      30

#define SCREEN_PIXEL_BITLEN             16
#define SCREEN_FRAME_SIZE               (SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN / 8)

// RGB565
#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usbd_uvc_const_t {
    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[167];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        vk_usbd_desc_t std_desc[5];

        struct {
            struct {
                usb_uvc_ct_roi_t roi_def;
                vk_usbd_uvc_control_info_t control_info[1];
            } camera;

            struct {
                usb_uvc_vs_t probe_commit;
                vk_usbd_uvc_control_info_t control_info[2];
            } stream;
        } uvc;
    } usbd;
} usbd_uvc_const_t;

typedef struct usbd_uvc_t {
    struct {
        struct {
            struct {
                usb_uvc_ct_roi_t roi_cur;
                vk_usbd_uvc_control_t control[1];
            } camera;
            struct {
                usb_uvc_vs_t probe_commit;
                vk_usbd_uvc_control_t control[2];
            } stream;
            vk_usbd_uvc_entity_t entity[3];
            vk_usbd_uvc_t param;
            vsf_teda_t teda;
            uint32_t frame_pos;
            uint16_t cur_size;
            bool stream_started;
            vsf_eda_t *eda_host;
        } uvc;
        vk_usbd_ifs_t ifs[2];
        vk_usbd_cfg_t config[1];
        vk_usbd_dev_t dev;
    } usbd;
} usbd_uvc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usbd_uvc_const_t __user_usbd_uvc_const = {
    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,             // bcdUSB
            0xEF,                   // device class: IAD
            0x02,                   // device sub class
            0x01,                   // device protocol
            64,                     // max packet size
            USB_DESC_WORD(APP_CFG_USBD_VID),
                                    // vendor
            USB_DESC_WORD(APP_CFG_USBD_PID),
                                    // product
            USB_DESC_WORD(0x2000),  // bcdDevice
            1,                      // manu facturer
            2,                      // product
            0,                      // serial number
            1,                      // number of configuration
        },
        .config_desc            = {
            USB_DT_CONFIG_SIZE,
            USB_DT_CONFIG,
            USB_DESC_WORD(sizeof(__user_usbd_uvc_const.usbd.config_desc)),
                                    // wTotalLength
            0x02,                   // bNumInterfaces: 2 interfaces
            0x01,                   // bConfigurationValue: Configuration value
            0x00,                   // iConfiguration: Index of string descriptor describing the configuration
            0x80,                   // bmAttributes: bus powered
            0xFA,                   // MaxPower

            // IDA for UVC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            0,                      // bFirstInterface
            2,                      // bInterfaceCount
            0x0E,                   // bFunctionClass: CC_VIDEO
            0x03,                   // bFunctionSubClass: SC_VIDEO_INTERFACE_COLLECTION
            0x00,                   // bFunctionProtocol: PC_PROTOCOL_UNDEFINED
            0x02,                   // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x00,                   // bInterfaceNumber: Number of Interface
            0x00,                   // bAlternateSetting: Alternate setting
            0,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x01,                   // bInterfaceSubClass: SC_VIDEOCONTROL
            0x00,                   // nInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x02,                   // iInterface:

            // Class-specific VC Interface Descriptor
            13,                     // bLength: Endpoint Descriptor size
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubtype: VC_HEADER
            USB_DESC_WORD(0x0150),  // bcdUVC: 1.5
            USB_DESC_WORD(39),      // wTotalLength
            USB_DESC_DWORD(100 * 1000 * 1000),
                                    // dwClockFrequency: deprecated, MUST not be 0
            0x01,                   // bInCollection: Number of streaming interfaces
            0x01,                   // baInterfaceNr[1]:

            // Input Terminal Descriptor
            17,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubtype: VC_INPUT_TERMINAL
            1,                      // bTerminalID
            USB_DESC_WORD(0x0201),  // wTerminalType: ITT_CAMERA
            0x00,                   // bAssocTerminal
            0x00,                   // iTerminal
            USB_DESC_WORD(0),       // wObjectiveFocalLengthMin
            USB_DESC_WORD(0),       // wObjectiveFocalLengthMax
            USB_DESC_WORD(0),       // wOcularFocalLength
            0x02,                   // bControlSize
            USB_DESC_WORD(0),       // bmControls

            // Output Terminal Descriptor
            9,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x03,                   // bDescriptorSubtype: VC_OUTPUT_TERMINAL
            2,                      // bTerminalID
            USB_DESC_WORD(0x0101),  // wTerminalType: TT_STREAMING
            0x00,                   // bAssocTerminal
            1,                      // bSourceID
            0x00,                   // iTerminal

            // Standard VS Interrupt Descriptor for 0-bandwidth alt
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x01,                   // bInterfaceNumber
            0x00,                   // bAlternateSetting
            0,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x02,                   // bInterfaceSubClass: SC_VIDEOSTREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Class-specific VS Header Descriptor
            14,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubType: VS_INPUT_HEADER
            1,                      // bNumFormats
            USB_DESC_WORD(77),      // wTotalLength
            0x81,                   // bEndpointAddress
            0x00,                   // bmInfo
            2,                      // bTerminalLink
            0x00,                   // bStillCaptureMethod
            0x00,                   // bTriggerSupport
            0x00,                   // bTriggerUsage
            0x01,                   // bControlSize
            0x00,                   // bmaControls

            // Class-specific VS Format Descriptor
            27,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x04,                   // bDescriptorSubType: CS_FORMAT_UNCOMPRESSED
            0x01,                   // bFormatIndex
            1,                      // bNumFrameDescriptors
#ifndef SCREEN_PIXEL_FORMAT_GUID
            SCREEN_PIXEL_FORMAT,
            0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
#else
            SCREEN_PIXEL_FORMAT_GUID,
#endif
                                    // guidFormat
            SCREEN_PIXEL_BITLEN,    // bBitsPerPixel
            0x01,                   // bDefaultFrameIndex
            0x00,                   // bAspectRationX
            0x00,                   // bAspectRatioY
            0x00,                   // bmInterfaceFlags
            0x00,                   // bCopyProtect

            // Class-specific VS Frame Descriptor
            30,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x05,                   // bDescriptorSubType: VS_FRAME_UNCOMPRESSED
            0x01,                   // bFrameIndex
            0x00,                   // bmCapabilities
            USB_DESC_WORD(SCREEN_WIDTH),
                                    // wWidth
            USB_DESC_WORD(SCREEN_HEIGHT),
                                    // wHeight
            USB_DESC_DWORD(SCREEN_FPS * SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN),
                                    // dwMinBitRate
            USB_DESC_DWORD(SCREEN_FPS * SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN),
                                    // dwMaxBitRate
            USB_DESC_DWORD(SCREEN_FRAME_SIZE),
                                    // dwMaxVideoFrameBufferSize
            USB_DESC_DWORD(10000000 / SCREEN_FPS),
                                    // dwDefaultFrameInterval
            0x01,                   // bFrameIntervalType: Continuous frame interval
            USB_DESC_DWORD(10000000 / SCREEN_FPS),
                                    // dwFrameInterval

            // Class-specific Color Matching Descriptor
            6,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x0D,                   // bDescriptorSubType: VS_COLORFORMAT
            0x01,                   // bColorPrimaries
            0x01,                   // bTransferCharacteristics
            0x04,                   // bMatrixCoefficients

            // Standard VS Interrupt Descriptor
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x01,                   // bInterfaceNumber
            0x01,                   // bAlternateSetting
            1,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x02,                   // bInterfaceSubClass: SC_VIDEOSTREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            7,                      // bLength
            0x05,                   // bDescriptorType: ENDPOINT
            0x81,                   // bEndpointAddress: IN1
            0x05,                   // bmAttributes: Iso
            USB_DESC_WORD(1024),    // wMaxPacketSize: 1K
            0x01,                   // bInterval
        },
        .str_lanid              = {
            4,
            USB_DT_STRING,
            0x09,
            0x04,
        },
        .str_vendor             = {
            20,
            USB_DT_STRING,
            'S', 0, 'i', 0, 'm', 0, 'o', 0, 'n', 0, 'Q', 0, 'i', 0, 'a', 0,
            'n', 0,
        },
        .str_product            = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'U', 0, 'V', 0, 'C', 0,
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(__user_usbd_uvc_const.usbd.dev_desc, sizeof(__user_usbd_uvc_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, __user_usbd_uvc_const.usbd.config_desc, sizeof(__user_usbd_uvc_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, __user_usbd_uvc_const.usbd.str_lanid, sizeof(__user_usbd_uvc_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, __user_usbd_uvc_const.usbd.str_vendor, sizeof(__user_usbd_uvc_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, __user_usbd_uvc_const.usbd.str_product, sizeof(__user_usbd_uvc_const.usbd.str_product)),
        },

        // camera_control
        .uvc.camera             = {
            .roi_def            = {
                .wROI_Top       = SCREEN_HEIGHT - 1,
                .wROI_Left      = 0,
                .wROI_Bottom    = 0,
                .wROI_Right     = SCREEN_WIDTH - 1,
                .bmAutoControls = 0x0000,
            },
            .control_info       = {
                [0]             = {
                    .selector   = 0x14,     // CT_REGION_OF_INTEREST_CONTROL
                    .size       = sizeof(usb_uvc_ct_roi_t),
                    .def.buffer = (void *)&__user_usbd_uvc_const.usbd.uvc.camera.roi_def,
                },
            },
        },
        .uvc.stream.probe_commit= {
            .bFormatIndex       = 1,
            .bFrameIndex        = 1,
            .dwFrameInterval    = 10000000 / SCREEN_FPS,
            .dwMaxVideoFrameSize= SCREEN_FRAME_SIZE,
        },
        .uvc.stream.control_info= {
            [0]                 = {
                .selector       = 0x01,     // VS_PROBE_CONTROL
                .size           = sizeof(usb_uvc_vs_t),
                .def.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
            },
            [1]                 = {
                .selector       = 0x02,     // VS_COMMIT_CONTROL
                .size           = sizeof(usb_uvc_vs_t),
                .def.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&__user_usbd_uvc_const.usbd.uvc.stream.probe_commit,
            },
        },
    },
};

static usbd_uvc_t __user_usbd_uvc = {
    .usbd                       = {
        // input terminal: Camera
        .uvc.camera.control     = {
            [0]                 = {
                .info           = &__user_usbd_uvc_const.usbd.uvc.camera.control_info[0],
                .cur.buffer     = (void *)&__user_usbd_uvc.usbd.uvc.camera.roi_cur,
            },
        },
        .uvc.stream.control     = {
            [0]                 = {
                .info           = &__user_usbd_uvc_const.usbd.uvc.stream.control_info[0],
                .cur.buffer     = (void *)&__user_usbd_uvc.usbd.uvc.stream.probe_commit,
            },
            [1]                 = {
                .info           = &__user_usbd_uvc_const.usbd.uvc.stream.control_info[1],
                .cur.buffer     = (void *)&__user_usbd_uvc.usbd.uvc.stream.probe_commit,
            },
        },
        .uvc.entity[0]          = {
            .id                 = 0,            // for video streaming
            .control_num        = dimof(__user_usbd_uvc.usbd.uvc.stream.control),
            .control            = __user_usbd_uvc.usbd.uvc.stream.control,
        },
        // input terminal: Camera
        .uvc.entity[1]          = {
            .id                 = 1,
            .control_num        = dimof(__user_usbd_uvc.usbd.uvc.camera.control),
            .control            = __user_usbd_uvc.usbd.uvc.camera.control,
        },
        // output terminal: USB
        .uvc.entity[2]          = {
            .id                 = 2,
        },
        .uvc.param              = {
            .ep_in              = 1,

            .entity_num         = dimof(__user_usbd_uvc.usbd.uvc.entity),
            .entity             = __user_usbd_uvc.usbd.uvc.entity,
        },

        .ifs[0].class_op        = &vk_usbd_uvc_control_class,
        .ifs[0].class_param     = &__user_usbd_uvc.usbd.uvc.param,
        .ifs[1].class_op        = &vk_usbd_uvc_stream_class,
        .ifs[1].class_param     = &__user_usbd_uvc.usbd.uvc.param,

        .config[0].num_of_ifs   = dimof(__user_usbd_uvc.usbd.ifs),
        .config[0].ifs          = __user_usbd_uvc.usbd.ifs,

        .dev.num_of_config      = dimof(__user_usbd_uvc.usbd.config),
        .dev.config             = __user_usbd_uvc.usbd.config,
        .dev.num_of_desc        = dimof(__user_usbd_uvc_const.usbd.std_desc),
        .dev.desc               = (vk_usbd_desc_t *)__user_usbd_uvc_const.usbd.std_desc,
        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.drv                = &VSF_USB_DC0,
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if APP_USE_LINUX_DEMO == ENABLED
int usbd_uvc_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    vk_usbd_init(&__user_usbd_uvc.usbd.dev);
    vk_usbd_connect(&__user_usbd_uvc.usbd.dev);
    return 0;
}

#endif
