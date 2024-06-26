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

#ifndef __VSF_USB_HUB_H__
#define __VSF_USB_HUB_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef USB_MAXCHILDREN
#   define USB_MAXCHILDREN          31
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usb_hub_desc_t usb_hub_desc_t;
struct usb_hub_desc_t {
    uint8_t  bDescLength;
    uint8_t  bDescriptorType;
    uint8_t  bNbrPorts;
    uint16_t wHubCharacteristics;
    uint8_t  bPwrOn2PwrGood;
    uint8_t  bHubContrCurrent;

    /* 2.0 and 3.0 hubs differ here */
    union {
        struct {
            /* add 1 bit for hub status change; round to bytes */
            uint8_t  DeviceRemovable[(USB_MAXCHILDREN + 1 + 7) / 8];
            uint8_t  PortPwrCtrlMask[(USB_MAXCHILDREN + 1 + 7) / 8];
        } VSF_CAL_PACKED hs;

        struct {
            uint8_t bHubHdrDecLat;
            uint16_t wHubDelay;
            uint16_t DeviceRemovable;
        } VSF_CAL_PACKED ss;
    } VSF_CAL_PACKED u;
} VSF_CAL_PACKED;

typedef struct usb_port_status_t usb_port_status_t;
struct usb_port_status_t {
    uint16_t wPortStatus;
    uint16_t wPortChange;
} VSF_CAL_PACKED;

enum usb_port_feature_t {
    USB_PORT_FEAT_CONNECTION        = 0,
    USB_PORT_FEAT_ENABLE            = 1,
    USB_PORT_FEAT_SUSPEND           = 2,    /* L2 suspend */
    USB_PORT_FEAT_OVER_CURRENT      = 3,
    USB_PORT_FEAT_RESET             = 4,
    USB_PORT_FEAT_L1                = 5,    /* L1 suspend */
    USB_PORT_FEAT_POWER             = 8,
    USB_PORT_FEAT_LOWSPEED          = 9,    /* Should never be used */
    USB_PORT_FEAT_C_CONNECTION      = 16,
    USB_PORT_FEAT_C_ENABLE          = 17,
    USB_PORT_FEAT_C_SUSPEND         = 18,
    USB_PORT_FEAT_C_OVER_CURRENT    = 19,
    USB_PORT_FEAT_C_RESET           = 20,
    USB_PORT_FEAT_TEST              = 21,
    USB_PORT_FEAT_INDICATOR         = 22,
    USB_PORT_FEAT_C_PORT_L1         = 23,
};

enum usb_port_state_t {
    USB_PORT_STAT_CONNECTION        = 0x0001,
    USB_PORT_STAT_ENABLE            = 0x0002,
    USB_PORT_STAT_SUSPEND           = 0x0004,
    USB_PORT_STAT_OVERCURRENT       = 0x0008,
    USB_PORT_STAT_RESET             = 0x0010,
    USB_PORT_STAT_L1                = 0x0020,
    USB_PORT_STAT_POWER             = 0x0100,
    USB_PORT_STAT_LOW_SPEED         = 0x0200,
    USB_PORT_STAT_HIGH_SPEED        = 0x0400,
    USB_PORT_STAT_TEST              = 0x0800,
    USB_PORT_STAT_INDICATOR         = 0x1000,
};

enum usb_port_change_t {
    USB_PORT_STAT_C_CONNECTION      = 0x0001,
    USB_PORT_STAT_C_ENABLE          = 0x0002,
    USB_PORT_STAT_C_SUSPEND         = 0x0004,
    USB_PORT_STAT_C_OVERCURRENT     = 0x0008,
    USB_PORT_STAT_C_RESET           = 0x0010,
    USB_PORT_STAT_C_L1              = 0x0020,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_HUB_H__
