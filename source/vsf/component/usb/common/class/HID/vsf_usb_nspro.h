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

#ifndef __VSF_USB_NSPRO_H__
#define __VSF_USB_NSPRO_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usb_nspro_gamepad_in_report_common_t vsf_usb_nspro_gamepad_in_report_common_t;
struct vsf_usb_nspro_gamepad_in_report_common_t {
    uint8_t id;         // 0x21/0x30/0x31
    uint8_t tick;

    // power_state
    //  1: switch/usb powered
    uint8_t power_state : 1;
    // con_info
    //  
    uint8_t con_info : 3;
    uint8_t charging : 1;
    // battery_level:
    //  0: empty
    //  1: critical
    //  2: low
    //  3: medium
    //  4: full
    uint8_t battery_level : 3;

    // button status, 3-5
    uint8_t rl : 1;
    uint8_t ru : 1;
    uint8_t rd : 1;
    uint8_t rr : 1;
    uint8_t : 2;
    uint8_t rb : 1;
    uint8_t rt : 1;
    
    uint8_t minus : 1;
    uint8_t plus : 1;
    uint8_t rs : 1;
    uint8_t ls : 1;
    uint8_t home : 1;
    uint8_t capture : 1;
    uint8_t : 1;
    uint8_t keep_1: 1;
    
    uint8_t ld : 1;
    uint8_t lu : 1;
    uint8_t lr : 1;
    uint8_t ll : 1;
    uint8_t : 2;
    uint8_t lb : 1;
    uint8_t lt : 1;

    // left stick, 6 - 8
    uint8_t lx_l8;
    uint8_t lx_h4:4;
    uint8_t ly_l4:4;
    uint8_t ly_h8;

    // right stick, 9 - 11
    uint8_t rx_l8;
    uint8_t rx_h4:4;
    uint8_t ry_l4:4;
    uint8_t ry_h8;

    // Vibrator input report. Decides if next vibration pattern should be sent. 12
    uint8_t vivrator;
} PACKED;

typedef struct vsf_usb_nspro_gamepad_in_report_full_t vsf_usb_nspro_gamepad_in_report_full_t;
struct vsf_usb_nspro_gamepad_in_report_full_t {
    implement(vsf_usb_nspro_gamepad_in_report_common_t)

    struct {
        uint16_t accx;        // 1G = 0x1000
        uint16_t accy;
        uint16_t accz;
        uint16_t roll;
        uint16_t pitch;
        uint16_t yaw;
    } PACKED gyro_acc[3];
} PACKED;

typedef struct vsf_usb_nspro_gamepad_in_report_t vsf_usb_nspro_gamepad_in_report_t;
struct vsf_usb_nspro_gamepad_in_report_t {
    implement(vsf_usb_nspro_gamepad_in_report_full_t)
    uint8_t zero[15];   // fill data to make 64 bytes report
} PACKED;

typedef struct vsf_usb_nspro_gamepad_out_report_t vsf_usb_nspro_gamepad_out_report_t;
struct vsf_usb_nspro_gamepad_out_report_t {
    uint8_t buffer[64];
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_NSPRO_H__

