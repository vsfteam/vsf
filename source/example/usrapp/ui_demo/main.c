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
#include <stdio.h>
#include <stdarg.h>

/*============================ MACROS ========================================*/

#define SCREEN_WIDTH                    128
#define SCREEN_HEIGHT                   128
#define SCREEN_FPS                      10

//#define SCREEN_PIXEL_FORMAT             'R', 'G', 'B', 'P'
//#define SCREEN_PIXEL_BITLEN             16
//#define SCREEN_PIXEL_FORMAT             'N', 'V', '1', '2',
//#define SCREEN_PIXEL_BITLEN             12
//#define SCREEN_PIXEL_FORMAT             'Y', 'U', 'Y', '2'
//#define SCREEN_PIXEL_BITLEN             16

// RGB888
//#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7D, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
//#define SCREEN_PIXEL_BITLEN             24

// RGB555
//#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7C, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
//#define SCREEN_PIXEL_BITLEN             10

// RGB565
#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
#define SCREEN_PIXEL_BITLEN             16

#define SCREEN_FRAME_SIZE               (SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN / 8)
#define SCREEN_PACKET_SIZE              (256 * SCREEN_PIXEL_BITLEN / 8)

#define GENERATE_HEX(value)             TPASTE2(0x, value)

/*============================ MACROFIED FUNCTIONS ===========================*/

#define USB_DESC_WORD(__value)                                                  \
            ((__value) >> 0) & 0xFF, ((__value) >> 8) & 0xFF

#define USB_DESC_DWORD(__value)                                                 \
            ((__value) >> 0) & 0xFF, ((__value) >> 8) & 0xFF,                   \
            ((__value) >> 16) & 0xFF, ((__value) >> 24) & 0xFF

/*============================ TYPES =========================================*/

struct usrapp_const_t {
    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[167];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        vsf_usbd_desc_t std_desc[5];

        struct {
            struct {
                usb_UVC_ct_roi_t roi_def;
                vsf_usbd_UVC_control_info_t control_info[1];
            } camera;

            struct {
                usb_UVC_vs_t probe_commit;
                vsf_usbd_UVC_control_info_t control_info[2];
            } stream;
        } uvc;
    } usbd;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    struct {
        struct {
            struct {
                usb_UVC_ct_roi_t roi_cur;
                vsf_usbd_UVC_control_t control[1];
            } camera;
            struct {
                usb_UVC_vs_t probe_commit;
                vsf_usbd_UVC_control_t control[2];
            } stream;
            vsf_usbd_UVC_entity_t entity[3];
            vsf_usbd_UVC_t param;
            vsf_teda_t teda;
            uint8_t buffer[2 + SCREEN_PACKET_SIZE];
            uint32_t frame_pos;
            uint16_t cur_size;
            bool stream_started;
        } uvc;
        vsf_usbd_ifs_t ifs[2];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;
        bool is_connected;
    } usbd;

    vsf_callback_timer_t poll_timer;
    uint8_t heap[0x8000];
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,             // bcdUSB
            0xEF,                   // device class: IAD
            0x02,                   // device sub class
            0x01,                   // device protocol
            64,                     // max packet size
            USB_DESC_WORD(GENERATE_HEX(APP_CFG_USBD_VID)),
                                    // vendor
            USB_DESC_WORD(GENERATE_HEX(APP_CFG_USBD_PID)),
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
            USB_DESC_WORD(sizeof(usrapp_const.usbd.config_desc)),
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
            USB_DESC_DWORD(SYSTEM_FREQ),
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
            USB_DESC_WORD(1024),    // wMaxPacketSize: 1024
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
            VSF_USBD_DESC_DEVICE(0, usrapp_const.usbd.dev_desc, sizeof(usrapp_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usrapp_const.usbd.config_desc, sizeof(usrapp_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usrapp_const.usbd.str_lanid, sizeof(usrapp_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usrapp_const.usbd.str_vendor, sizeof(usrapp_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usrapp_const.usbd.str_product, sizeof(usrapp_const.usbd.str_product)),
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
                    .size       = sizeof(usb_UVC_ct_roi_t),
                    .def.buffer = (void *)&usrapp_const.usbd.uvc.camera.roi_def,
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
                .size           = sizeof(usb_UVC_vs_t),
                .def.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
            },
            [1]                 = {
                .selector       = 0x02,     // VS_COMMIT_CONTROL
                .size           = sizeof(usb_UVC_vs_t),
                .def.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&usrapp_const.usbd.uvc.stream.probe_commit,
            },
        },
    },
};

static void usrapp_uvc_evthandler(vsf_eda_t *eda, vsf_evt_t evt);

static usrapp_t usrapp = {
    .usbd                       = {
        // input terminal: Camera
        .uvc.camera.control     = {
            [0]                 = {
                .info           = &usrapp_const.usbd.uvc.camera.control_info[0],
                .cur.buffer     = (void *)&usrapp.usbd.uvc.camera.roi_cur,
            },
        },
        .uvc.stream.control     = {
            [0]                 = {
                .info           = &usrapp_const.usbd.uvc.stream.control_info[0],
                .cur.buffer     = (void *)&usrapp.usbd.uvc.stream.probe_commit,
            },
            [1]                 = {
                .info           = &usrapp_const.usbd.uvc.stream.control_info[1],
                .cur.buffer     = (void *)&usrapp.usbd.uvc.stream.probe_commit,
            },
        },
        .uvc.entity[0]          = {
            .id                 = 0,            // for video streaming
            .control_num        = dimof(usrapp.usbd.uvc.stream.control),
            .control            = usrapp.usbd.uvc.stream.control,
        },
        // input terminal: Camera
        .uvc.entity[1]          = {
            .id                 = 1,
            .control_num        = dimof(usrapp.usbd.uvc.camera.control),
            .control            = usrapp.usbd.uvc.camera.control,
        },
        // output terminal: USB
        .uvc.entity[2]          = {
            .id                 = 2,
        },
        .uvc.param              = {
            .ep_in              = 1,

            .entity_num         = dimof(usrapp.usbd.uvc.entity),
            .entity             = usrapp.usbd.uvc.entity,
        },
        .uvc.teda               = {
            .evthandler         = usrapp_uvc_evthandler,
            .on_terminate       = NULL,
        },

        .ifs[0].class_op        = &vsf_usbd_UVC_control_class,
        .ifs[0].class_param     = &usrapp.usbd.uvc.param,
        .ifs[1].class_op        = &vsf_usbd_UVC_stream_class,
        .ifs[1].class_param     = &usrapp.usbd.uvc.param,

        .config[0].num_of_ifs   = dimof(usrapp.usbd.ifs),
        .config[0].ifs          = usrapp.usbd.ifs,

        .dev.num_of_config      = dimof(usrapp.usbd.config),
        .dev.config             = usrapp.usbd.config,
        .dev.num_of_desc        = dimof(usrapp_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usrapp_const.usbd.std_desc,

        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.priority_int       = 0,
#if VSF_USBD_CFG_USE_EDA == ENABLED
        .dev.priority_eda       = vsf_priority_0,
#endif
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usrapp_uvc_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    uint8_t *buffer = usrapp.usbd.uvc.buffer;
    uint_fast32_t remain;
    static uint_fast8_t cur_color = 0x00;

    switch (evt) {
    case VSF_EVT_INIT:
        buffer[0] = 2;
        buffer[1] = 1;
        memset(&buffer[2], cur_color, SCREEN_PACKET_SIZE);
        // fall through
    case VSF_EVT_TIMER:
        buffer[1] ^= 1;
        usrapp.usbd.uvc.frame_pos = 0;
        usrapp.usbd.uvc.cur_size = 0;
        remain = SCREEN_FRAME_SIZE;
        // fall through
    case VSF_EVT_MESSAGE:
        if (usrapp.usbd.uvc.stream_started) {
            usrapp.usbd.uvc.frame_pos += usrapp.usbd.uvc.cur_size;

            remain = SCREEN_FRAME_SIZE - usrapp.usbd.uvc.frame_pos;
            if (!remain) {
                vsf_teda_set_timer_ms(1000 / SCREEN_FPS);
                cur_color++;
                memset(&buffer[2], cur_color, SCREEN_PACKET_SIZE);
            } else {
                usrapp.usbd.uvc.cur_size = min(SCREEN_PACKET_SIZE, remain);
                vsf_usbd_UVC_send_frame(&usrapp.usbd.uvc.param, buffer, usrapp.usbd.uvc.cur_size + 2);
            }
        }
        break;
    }
}

void vsf_usbd_UVC_stop_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
    if (usrapp.usbd.uvc.stream_started) {
        usrapp.usbd.uvc.stream_started = false;
    }
}

void vsf_usbd_UVC_start_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
    if (!usrapp.usbd.uvc.stream_started) {
        usrapp.usbd.uvc.stream_started = true;
        vsf_teda_init(&usrapp.usbd.uvc.teda, vsf_priority_0, false);
    }
}

void usrapp_on_timer(vsf_callback_timer_t *timer)
{
    if (!usrapp.usbd.is_connected) {
        usrapp.usbd.is_connected = true;
        vsf_usbd_connect(&usrapp.usbd.dev);
    } else {
        vsf_trace(VSF_TRACE_INFO, "heartbeat: [%d]" VSF_TRACE_CFG_LINEEND, vsf_timer_get_tick());
    }
//    vsf_callback_timer_add_ms(timer, 1000);
}

int main(void)
{
    vsf_trace_init(NULL);

    vsf_heap_init();
    vsf_heap_add(usrapp.heap, sizeof(usrapp.heap));

    usrapp.usbd.uvc.stream.probe_commit = usrapp_const.usbd.uvc.stream.probe_commit;
    vsf_usbd_init(&usrapp.usbd.dev);
    vsf_usbd_disconnect(&usrapp.usbd.dev);

    usrapp.poll_timer.on_timer = usrapp_on_timer;
    vsf_callback_timer_add_ms(&usrapp.poll_timer, 200);

    return 0;
}

/* EOF */
