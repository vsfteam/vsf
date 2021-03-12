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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "vsf.h"

#if VSF_USE_USB_DEVICE == ENABLED && APP_USE_USBD_DEMO == ENABLED && APP_USE_USBD_UAC_DEMO == ENABLED

#include "../common/usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usbd_uac_const_t {
    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[193];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        vk_usbd_desc_t std_desc[5];

        struct {
            struct {
                vk_usbd_uac_control_info_t control_info[2];
            } line_out;

            struct {
                vk_usbd_uac_control_info_t control_info[2];
            } line_in;
        } uac;
    } usbd;
} usbd_uac_const_t;

typedef struct usbd_uac_t {
    struct {
        struct {
            struct {
                vk_usbd_uac_control_t control[2];
            } line_out;
            struct {
                vk_usbd_uac_control_t control[2];
            } line_in;
            vk_usbd_uac_entity_t entity[6];

            // param followed by streams
            vk_usbd_uac_ac_t ac_param;
            vk_usbd_uac_as_t as_param[2];
        } uac;
        vk_usbd_ifs_t ifs[3];
        vk_usbd_cfg_t config[1];
        vk_usbd_dev_t dev;
    } usbd;
} usbd_uac_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

describe_mem_stream(__user_usbd_uac_rx_stream, 192)
describe_mem_stream(__user_usbd_uac_tx_stream, 96)

static const usbd_uac_const_t __user_usbd_uac_const = {
    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,             // bcdUSB
            0x00,                   // device class:
            0x00,                   // device sub class
            0x00,                   // device protocol
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
            USB_DESC_WORD(sizeof(__user_usbd_uac_const.usbd.config_desc)),
                                    // wTotalLength
            0x03,                   // bNumInterfaces: 3 interfaces
            0x01,                   // bConfigurationValue: Configuration value
            0x00,                   // iConfiguration: Index of string descriptor describing the configuration
            0x80,                   // bmAttributes: bus powered
            0xFA,                   // MaxPower

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x00,                   // bInterfaceNumber: Number of Interface
            0x00,                   // bAlternateSetting: Alternate setting
            0,                      // bNumEndpoints
            0x01,                   // bInterfaceClass: CC_AUDIO
            0x01,                   // bInterfaceSubClass: AUDIO_CONTROL
            0x00,                   // nInterfaceProtocol
            0x00,                   // iInterface:

            // Class-specific AC Interface Descriptor
            10,                     // bLength: Endpoint Descriptor size
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubtype: AC_HEADER
            USB_DESC_WORD(0x0100),  // bcdADC: 1.0
            USB_DESC_WORD(71),      // wTotalLength
            2,                      // bInCollection: 2 streams
            0x01,                   // baInterfaceNr[1]: interface 1
            0x02,                   // baInterfaceNr[2]: interface 2

            // Input Terminal Descriptor
            12,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubtype: AC_INPUT_TERMINAL
            1,                      // bTerminalID
            USB_DESC_WORD(0x0101),  // wTerminalType: ITT_STREAMING
            0,                      // bAssocTerminal
            0x02,                   // bNrChannels
            USB_DESC_WORD(0x0003),  // wChannelConfig: left + right
            0,                      // iChannelNames
            0,                      // iTerminal

            // Feature Unit Descriptor
            10,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x06,                   // bDescriptorSubtype: AC_FEATURE_UNIT
            2,                      // bUnitID
            1,                      // bSourceID
            1,                      // bControlSize
            0x03,                   // bmaControls[0]: Mute + Volume
            0x00,                   // bmaControls[1]
            0x00,                   // bmaControls[2]
            0x00,                   // iFeature

            // Output Terminal Descriptor
            9,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x03,                   // bDescriptorSubType: AC_OUTPUT_TERMINAL
            3,                      // bTerminalID
            USB_DESC_WORD(0x0103),  // wTerminalType:
            0,                      // bAssocTerminal
            2,                      // bSourceID
            0,                      // iTerminal

            // Intput Terminal Descriptor
            12,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubtype: AC_INPUT_TERMINAL
            4,                      // bTerminalID
            USB_DESC_WORD(0x0102),  // wTerminalType:
            0,                      // bAssocTerminal
            1,                      // bNrChannels
            USB_DESC_WORD(0x0001),  // wChannelConfig: left
            0,                      // iChannelNames
            0,                      // iTerminal

            // Feature Unit Descriptor
            9,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x06,                   // bDescriptorSubtype: AC_FEATURE_UNIT
            5,                      // bUnitID
            4,                      // bSourceID
            1,                      // bControlSize
            0x03,                   // bmaControls[0]: Mute + Volume
            0x00,                   // bmaControls[1]
            0x00,                   // iFeature

            // Output Terminal Descriptor
            9,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x03,                   // bDescriptorSubType: AC_OUTPUT_TERMINAL
            6,                      // bTerminalID
            USB_DESC_WORD(0x0101),  // wTerminalType:
            0,                      // bAssocTerminal
            5,                      // bSourceID
            0,                      // iTerminal

            // Standard AS Interrupt Descriptor for 0-bandwidth alt
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x01,                   // bInterfaceNumber
            0x00,                   // bAlternateSetting
            0,                      // bNumEndpoints
            0x01,                   // bInterfaceClass: CC_AUDIO
            0x02,                   // bInterfaceSubClass: AUDIO_STREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Standard AS Interrupt Descriptor
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x01,                   // bInterfaceNumber
            0x01,                   // bAlternateSetting
            1,                      // bNumEndpoints
            0x01,                   // bInterfaceClass: CC_AUDIO
            0x02,                   // bInterfaceSubClass: AUDIO_STREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Class-specific AS General Descriptor
            7,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubType: AS_GENERAL
            6,                      // bTerminalLink
            0,                      // bDelay
            USB_DESC_WORD(0x0001),  // wFormatTag: PCM

            // Class-specific AS Format Type Descriptor
            11,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubType: AS_FORMAT_TYPE
            0x01,                   // bFormatType: FORMAT_TYPE_I
            1,                      // bNrChannels
            2,                      // bSubframeSize
            16,                     // bBitResolution
            0x01,                   // bSamFreqType
            0x80, 0xBB, 0x00,       // tSamFreq[1]: 48K

            // Standard Audio Endpoint Descriptor
            USB_DT_ENDPOINT_AUDIO_SIZE,
                                    // bLength
            USB_DT_ENDPOINT,        // bDescriptorType
            0x83,                   // bEndpointAddress: IN3
            0x01,                   // bmAttributes
            USB_DESC_WORD(96),      // wMaxPacketSize
            1,                      // bInterval
            0,                      // bRefresh
            0,                      // bSynchAddress

            // Class-specific Endpoint Descriptor
            7,                      // bLength
            0x25,                   // bDescriptorType: CS_ENDPOINT
            0x01,                   // bDescriptorSubType: AUDIO_EP_GENERAL
            0x80,                   // bmAttributes: MaxPacketsOnly
            0,                      // bLockDelayUnits
            USB_DESC_WORD(0),       // wLockDelay

            // Standard AS Interrupt Descriptor for 0-bandwidth alt
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x02,                   // bInterfaceNumber
            0x00,                   // bAlternateSetting
            0,                      // bNumEndpoints
            0x01,                   // bInterfaceClass: CC_AUDIO
            0x02,                   // bInterfaceSubClass: AUDIO_STREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Standard AS Interrupt Descriptor
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x02,                   // bInterfaceNumber
            0x01,                   // bAlternateSetting
            1,                      // bNumEndpoints
            0x01,                   // bInterfaceClass: CC_AUDIO
            0x02,                   // bInterfaceSubClass: AUDIO_STREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Class-specific AS General Descriptor
            7,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubType: AS_GENERAL
            1,                      // bTerminalLink
            0,                      // bDelay
            USB_DESC_WORD(0x0001),  // wFormatTag: PCM

            // Class-specific AS Format Type Descriptor
            11,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubType: AS_FORMAT_TYPE
            0x01,                   // bFormatType: FORMAT_TYPE_I
            2,                      // bNrChannels
            2,                      // bSubframeSize
            16,                     // bBitResolution
            0x01,                   // bSamFreqType
            0x80, 0xBB, 0x00,       // tSamFreq[1]: 48K

            // Standard Audio Endpoint Descriptor
            USB_DT_ENDPOINT_AUDIO_SIZE,
                                    // bLength
            USB_DT_ENDPOINT,        // bDescriptorType
            0x04,                   // bEndpointAddress: OUT4
            0x01,                   // bmAttributes
            USB_DESC_WORD(192),     // wMaxPacketSize
            1,                      // bInterval
            0,                      // bRefresh
            0,                      // bSynchAddress

            // Class-specific Endpoint Descriptor
            7,                      // bLength
            0x25,                   // bDescriptorType: CS_ENDPOINT
            0x01,                   // bDescriptorSubType: AUDIO_EP_GENERAL
            0x80,                   // bmAttributes: MaxPacketsOnly
            0,                      // bLockDelayUnits
            USB_DESC_WORD(0),       // wLockDelay
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
            'V', 0, 'S', 0, 'F', 0, 'U', 0, 'A', 0, 'C', 0,
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(__user_usbd_uac_const.usbd.dev_desc, sizeof(__user_usbd_uac_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, __user_usbd_uac_const.usbd.config_desc, sizeof(__user_usbd_uac_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, __user_usbd_uac_const.usbd.str_lanid, sizeof(__user_usbd_uac_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, __user_usbd_uac_const.usbd.str_vendor, sizeof(__user_usbd_uac_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, __user_usbd_uac_const.usbd.str_product, sizeof(__user_usbd_uac_const.usbd.str_product)),
        },

        // line in control
        .uac.line_in            = {
            .control_info       = {
                [0]             = {
                    .selector   = 0x01,     // Mute
                    .size       = 1,
                },
                [1]             = {
                    .selector   = 0x02,     // Volume
                    .size       = 2,
                    .res.uval16 = 0x0001,
                    .min.uval16 = 0x0000,
                    .max.uval16 = 0xFFFF,
                },
            },
        },
        .uac.line_out           = {
            .control_info       = {
                [0]             = {
                    .selector   = 0x01,     // Mute
                    .size       = 1,
                },
                [1]             = {
                    .selector   = 0x02,     // Volume
                    .size       = 2,
                    .res.uval16 = 0x0001,
                    .min.uval16 = 0x0000,
                    .max.uval16 = 0xFFFF,
                },
            },
        },
    },
};

static usbd_uac_t __user_usbd_uac = {
    .usbd                       = {
        .uac.line_in.control    = {
            [0]                 = {
                .info           = &__user_usbd_uac_const.usbd.uac.line_in.control_info[0],
                .cur.uval8      = 0x00,
            },
            [1]                 = {
                .info           = &__user_usbd_uac_const.usbd.uac.line_in.control_info[1],
                .cur.uval16     = 0x8000,
            },
        },
        .uac.line_out.control   = {
            [0]                 = {
                .info           = &__user_usbd_uac_const.usbd.uac.line_out.control_info[0],
                .cur.uval8      = 0x00,
            },
            [1]                 = {
                .info           = &__user_usbd_uac_const.usbd.uac.line_out.control_info[1],
                .cur.uval16     = 0x8000,
            },
        },

        // Input Terminal: USB
        .uac.entity[0]          = {
            .id                 = 1,
        },
        // Feature Unit
        .uac.entity[1]          = {
            .id                 = 2,
            .control_num        = dimof(__user_usbd_uac.usbd.uac.line_out.control),
            .control            = __user_usbd_uac.usbd.uac.line_out.control,
        },
        // Output Terminal:
        .uac.entity[2]          = {
            .id                 = 3,
        },
        // Input Terminal
        .uac.entity[3]          = {
            .id                 = 4,
        },
        // Feature Unit
        .uac.entity[4]          = {
            .id                 = 5,
            .control_num        = dimof(__user_usbd_uac.usbd.uac.line_in.control),
            .control            = __user_usbd_uac.usbd.uac.line_in.control,
        },
        // Output Terminal: USB
        .uac.entity[5]          = {
            .id                 = 6,
        },

        .uac.ac_param           = {
            .stream_num         = 2,
            .entity_num         = dimof(__user_usbd_uac.usbd.uac.entity),
            .entity             = __user_usbd_uac.usbd.uac.entity,
        },
        .uac.as_param[0]        = {
            .ep                 = 0x83,
            .packet_size        = 96,
            .uac_ac             = &__user_usbd_uac.usbd.uac.ac_param,
            .stream             = &__user_usbd_uac_tx_stream.use_as__vsf_stream_t,
        },
        .uac.as_param[1]        = {
            .ep                 = 0x04,
            .packet_size        = 192,
            .uac_ac             = &__user_usbd_uac.usbd.uac.ac_param,
            .stream             = &__user_usbd_uac_rx_stream.use_as__vsf_stream_t,
        },

        .ifs[0].class_op        = &vk_usbd_uac_control_class,
        .ifs[0].class_param     = &__user_usbd_uac.usbd.uac.ac_param,
        .ifs[1].class_op        = &vk_usbd_uac_stream_class,
        .ifs[1].class_param     = &__user_usbd_uac.usbd.uac.as_param[0],
        .ifs[2].class_op        = &vk_usbd_uac_stream_class,
        .ifs[2].class_param     = &__user_usbd_uac.usbd.uac.as_param[1],

        .config[0].num_of_ifs   = dimof(__user_usbd_uac.usbd.ifs),
        .config[0].ifs          = __user_usbd_uac.usbd.ifs,

        .dev.num_of_config      = dimof(__user_usbd_uac.usbd.config),
        .dev.config             = __user_usbd_uac.usbd.config,
        .dev.num_of_desc        = dimof(__user_usbd_uac_const.usbd.std_desc),
        .dev.desc               = (vk_usbd_desc_t *)__user_usbd_uac_const.usbd.std_desc,
        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.drv                = &VSF_USB_DC0,
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __usrapp_usbd_uac_on_stream(void *param, vsf_stream_evt_t evt)
{
    vsf_stream_t *stream = param;
    switch (evt) {
    case VSF_STREAM_ON_TX:
        vsf_stream_write(stream, NULL, vsf_stream_get_free_size(stream));
        break;
    case VSF_STREAM_ON_RX:
        vsf_stream_read(stream, NULL, vsf_stream_get_buff_size(stream));
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
int usbd_uac_main(int argc, char *argv[])
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

    vsf_stream_init(&__user_usbd_uac_rx_stream.use_as__vsf_stream_t);
    __user_usbd_uac_rx_stream.rx.param = &__user_usbd_uac_rx_stream;
    __user_usbd_uac_rx_stream.rx.evthandler = __usrapp_usbd_uac_on_stream;
    vsf_stream_connect_rx(&__user_usbd_uac_rx_stream.use_as__vsf_stream_t);

    vsf_stream_init(&__user_usbd_uac_tx_stream.use_as__vsf_stream_t);
    __user_usbd_uac_tx_stream.tx.param = &__user_usbd_uac_tx_stream;
    __user_usbd_uac_tx_stream.tx.evthandler = __usrapp_usbd_uac_on_stream;
    vsf_stream_connect_tx(&__user_usbd_uac_tx_stream.use_as__vsf_stream_t);

    vk_usbd_init(&__user_usbd_uac.usbd.dev);
    vk_usbd_connect(&__user_usbd_uac.usbd.dev);
    return 0;
}

#endif
