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
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsf_usb_nspro_gamepad_in_report_t {
    uint8_t id;         // 0x30
    uint8_t tick;
    uint8_t const_0x40; // 0x40

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
    
    uint8_t lx_l8;
    uint8_t lx_h4:4;
    uint8_t ly_l4:4;
    uint8_t ly_h8;
    
    uint8_t rx_l8;
    uint8_t rx_h4:4;
    uint8_t ry_l4:4;
    uint8_t ry_h8;
    
    uint8_t sixaxis_include_0x0a;

    struct 
    {
        uint8_t accx_l8;        // 1G = 0x1000
        uint8_t accx_h8;
        uint8_t accy_l8;
        uint8_t accy_h8;
        uint8_t accz_l8;
        uint8_t accz_h8;
        uint8_t roll_l8;
        uint8_t roll_h8;
        uint8_t pitch_l8;
        uint8_t pitch_h8;
        uint8_t yaw_l8;
        uint8_t yaw_h8;
    } gyro_acc[3];

    uint8_t zeros[15];
} PACKED;
typedef struct vsf_usb_nspro_gamepad_in_report_t vsf_usb_nspro_gamepad_in_report_t;

struct vsf_usb_nspro_gamepad_out_report_t {
    uint8_t buffer[64];
} PACKED;
typedef struct vsf_usb_nspro_gamepad_out_report_t vsf_usb_nspro_gamepad_out_report_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif    // __VSF_USB_NSPRO_H__

