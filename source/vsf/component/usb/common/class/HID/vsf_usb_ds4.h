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

#ifndef __VSF_USB_DS4_H__
#define __VSF_USB_DS4_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usb_ds4_gamepad_in_report_t vsf_usb_ds4_gamepad_in_report_t;
struct vsf_usb_ds4_gamepad_in_report_t {
    uint8_t id;             // data 0
    uint8_t lx;             // data 1
    uint8_t ly;             // data 2
    uint8_t rx;             // data 3
    uint8_t ry;             // data 4
    uint8_t d_pad:4;        // data 5
    uint8_t square:1;
    uint8_t cross:1;
    uint8_t circle:1;
    uint8_t triangle:1;
    uint8_t l1:1;           // data 6
    uint8_t r1:1;
    uint8_t l2_click:1;
    uint8_t r2_click:1;
    uint8_t menu_left:1;    // share
    uint8_t menu_right:1;   // options
    uint8_t l3:1;
    uint8_t r3:1;
    uint8_t menu_main:1;    // data 7 ps
    uint8_t touch_click:1;
    uint8_t counter:6;
    uint8_t l2;             // data 8
    uint8_t r2;             // data 9
    uint8_t unknown1[2];    // data 10 - data 11
    uint8_t battery;        // data 12
    int16_t gyro_pitch;     // data 13 - data 14
    int16_t gyro_yaw;       // data 15 - data 16
    int16_t gyro_roll;      // data 17 - data 18
    int16_t acc_y_nega;     // data 19 - data 20    1G = 0x2000
    int16_t acc_z;          // data 21 - data 22
    int16_t acc_x_nega;     // data 23 - data 24
    uint8_t unknown2[5];    // data 25 - data 29
    uint8_t type;           // data 30              0: no audio, 0x60: audio
    uint8_t unknown3[2];    // data 31 - data 32
    uint8_t touch[19];      // data 33 - data 51
    uint8_t unknown4[12];   // data 52 - data 63
} PACKED;

typedef struct vsf_usb_ds4_gamepad_out_report_t vsf_usb_ds4_gamepad_out_report_t;
struct vsf_usb_ds4_gamepad_out_report_t {
    uint8_t id;
    uint8_t dummyFF;
    uint8_t dummy1[2];
    
    uint8_t rumble_r;       // 0 - 255
    uint8_t rumble_l;
    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;
    
    uint8_t flash_bright;   // 255 = 2.5 seconds
    uint8_t flash_dark;     // 255 = 2.5 seconds
    
    uint8_t dummy2[21];
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_DS4_H__

