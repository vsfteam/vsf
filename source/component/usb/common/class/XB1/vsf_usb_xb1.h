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

#ifndef __VSF_USB_XB1_H__
#define __VSF_USB_XB1_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usb_xb1_gamepad_in_report_t vsf_usb_xb1_gamepad_in_report_t;
struct vsf_usb_xb1_gamepad_in_report_t {
    uint8_t data0;          //0x20
    uint8_t data1;          //0x00
    uint8_t number;         //0x01-0xff
    uint8_t data3;          //0x0e
    uint8_t :1;             //data4.bit0
    uint8_t menu_main:1;    //data4.bit1 dummy_menu_main
    uint8_t menu_right:1;   //data4.bit2 menu
    uint8_t menu_left:1;    //data4.bit3 view
    uint8_t a:1;            //data4.bit4
    uint8_t b:1;            //data4.bit5
    uint8_t x:1;            //data4.bit6
    uint8_t y:1;            //data4.bit7
    uint8_t up:1;           //data5.bit0
    uint8_t down:1;         //data5.bit1
    uint8_t left:1;         //data5.bit2
    uint8_t right:1;        //data5.bit3
    uint8_t lb:1;           //data5.bit4
    uint8_t rb:1;           //data5.bit5
    uint8_t ls:1;           //data5.bit6
    uint8_t rs:1;           //data5.bit7
    int16_t lt;             //data6,data7
    int16_t rt;             //data8,data9
    int16_t lx;             //data10,data11
    int16_t ly;             //data12,data13
    int16_t rx;             //data14,data15
    int16_t ry;             //data16,data17
} PACKED;

typedef struct vsf_usb_xb1_gamepad_out_report_t vsf_usb_xb1_gamepad_out_report_t;
struct vsf_usb_xb1_gamepad_out_report_t {
    uint8_t buffer[36];
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_XB1_H__

