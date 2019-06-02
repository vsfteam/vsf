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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define GENERATE_HEX(value)             TPASTE2(0x, value)

/*============================ TYPES =========================================*/

struct usbd_demo_const_t {
    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[75 + 66];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        uint8_t str_cdc[14];
        uint8_t str_cdc2[16];
        vsf_usbd_desc_t std_desc[7];
    } usbd;
};
typedef struct usbd_demo_const_t usbd_demo_const_t;

struct usbd_demo_t {
    struct {
        struct {
            vsf_usbd_CDCACM_t param[2];
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            struct {
                vsf_fifo_stream_t tx;
                vsf_fifo_stream_t rx;
                uint8_t tx_buffer[4 * 1024];
                uint8_t rx_buffer[4 * 1024];
            } stream[2];
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif
        } cdc;
        vsf_usbd_ifs_t ifs[4];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;
    } usbd;
};
typedef struct usbd_demo_t usbd_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usbd_demo_const_t usbd_demo_const = {
    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,     // bcdUSB
            0xEF,           // device class: IAD
            0x02,           // device sub class
            0x01,           // device protocol
            64,             // max packet size
            (GENERATE_HEX(APP_CFG_USBD_VID) >> 0) & 0xFF,
            (GENERATE_HEX(APP_CFG_USBD_VID) >> 8) & 0xFF,
                            // vendor
            (GENERATE_HEX(APP_CFG_USBD_PID) >> 0) & 0xFF,
            (GENERATE_HEX(APP_CFG_USBD_PID) >> 8) & 0xFF,
                            // product
            0x00, 0x02,     // bcdDevice
            1,              // manu facturer
            2,              // product
            0,              // serial number
            1,              // number of configuration
        },
        .config_desc            = {
            USB_DT_CONFIG_SIZE,
            USB_DT_CONFIG,
            (sizeof(usbd_demo_const.usbd.config_desc) >> 0) & 0xFF,
            (sizeof(usbd_demo_const.usbd.config_desc) >> 8) & 0xFF,
                            // wTotalLength
            0x04,           // bNumInterfaces: 4 interfaces
            0x01,           // bConfigurationValue: Configuration value
            0x00,           // iConfiguration: Index of string descriptor describing the configuration
            0x80,           // bmAttributes: bus powered
            0x64,           // MaxPower

            // IDA for CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            0,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x04,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x00,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x04,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            0,              // bMasterInterface: Communication class interface
            1,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x81,           // bEndpointAddress: (IN1)
            0x03,           // bmAttributes: Interrupt
            8, 0x00,        // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x01,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x04,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x82,           // bEndpointAddress: (IN2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x02,           // bEndpointAddress: (OUT2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            // IDA for another CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            2,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x05,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x02,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x05,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            2,              // bMasterInterface: Communication class interface
            3,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x83,           // bEndpointAddress: (IN3)
            0x03,           // bmAttributes: Interrupt
            8, 0x00,        // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x03,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x05,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x84,           // bEndpointAddress: (IN4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x04,           // bEndpointAddress: (OUT4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x00, 0x02,     // wMaxPacketSize:
#else
            0x40, 0x00,     // wMaxPacketSize:
#endif
            0x00,           // bInterval:
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
            'V', 0, 'S', 0, 'F', 0, 'A', 0, 'I', 0, 'O', 0,
        },
        .str_cdc                = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0,
        },
        .str_cdc2               = {
            16,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '2', 0,
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usbd_demo_const.usbd.dev_desc, sizeof(usbd_demo_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usbd_demo_const.usbd.config_desc, sizeof(usbd_demo_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usbd_demo_const.usbd.str_lanid, sizeof(usbd_demo_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usbd_demo_const.usbd.str_vendor, sizeof(usbd_demo_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usbd_demo_const.usbd.str_product, sizeof(usbd_demo_const.usbd.str_product)),
            VSF_USBD_DESC_STRING(0x0409, 4, usbd_demo_const.usbd.str_cdc, sizeof(usbd_demo_const.usbd.str_cdc)),
            VSF_USBD_DESC_STRING(0x0409, 5, usbd_demo_const.usbd.str_cdc2, sizeof(usbd_demo_const.usbd.str_cdc2)),
        },
    },
};

static usbd_demo_t usbd_demo = {
    .usbd                       = {
        .cdc.param[0]           = {
            .ep = {
                .notify         = 1,
                .out            = 2,
                .in             = 2,
            },
            .line_coding        = {
                .bitrate        = 115200,
                .stop           = 0,
                .parity         = 0,
                .datalen        = 8,
            },
        },
        .cdc.param[1]           = {
            .ep = {
                .notify         = 3,
                .out            = 4,
                .in             = 4,
            },
            .line_coding        = {
                .bitrate        = 115200,
                .stop           = 0,
                .parity         = 0,
                .datalen        = 8,
            },
        },
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
        .cdc.param[0].stream.tx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc.stream[0].tx,
        .cdc.param[0].stream.rx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc.stream[0].rx,
        .cdc.stream[0]          = {
            .tx.op              = &vsf_fifo_stream_op,
            .tx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc.stream[0].tx_buffer,
            .tx.size            = sizeof(usbd_demo.usbd.cdc.stream[0].tx_buffer),
            .rx.op              = &vsf_fifo_stream_op,
            .rx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc.stream[0].rx_buffer,
            .rx.size            = sizeof(usbd_demo.usbd.cdc.stream[0].rx_buffer),
        },
        .cdc.param[1].stream.tx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc.stream[1].tx,
        .cdc.param[1].stream.rx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc.stream[1].rx,
        .cdc.stream[1]          = {
            .tx.op              = &vsf_fifo_stream_op,
            .tx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc.stream[1].tx_buffer,
            .tx.size            = sizeof(usbd_demo.usbd.cdc.stream[1].tx_buffer),
            .rx.op              = &vsf_fifo_stream_op,
            .rx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc.stream[1].rx_buffer,
            .rx.size            = sizeof(usbd_demo.usbd.cdc.stream[1].rx_buffer),
        },
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif

        .ifs[0].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[0].class_param     = &usbd_demo.usbd.cdc.param[0],
        .ifs[1].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[1].class_param     = &usbd_demo.usbd.cdc.param[0],

        .ifs[2].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[2].class_param     = &usbd_demo.usbd.cdc.param[1],
        .ifs[3].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[3].class_param     = &usbd_demo.usbd.cdc.param[1],

        .config[0].num_of_ifs   = dimof(usbd_demo.usbd.ifs),
        .config[0].ifs          = usbd_demo.usbd.ifs,

        .dev.num_of_config      = dimof(usbd_demo.usbd.config),
        .dev.config             = usbd_demo.usbd.config,
        .dev.num_of_desc        = dimof(usbd_demo_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usbd_demo_const.usbd.std_desc,

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

void usbd_demo_start(void)
{
    vsf_usbd_init(&usbd_demo.usbd.dev);
    vsf_usbd_disconnect(&usbd_demo.usbd.dev);
}

void usbd_demo_connect(void)
{
    vsf_usbd_connect(&usbd_demo.usbd.dev);
}