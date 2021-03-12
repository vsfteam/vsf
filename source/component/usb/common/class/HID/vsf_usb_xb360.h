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

#ifndef __VSF_USB_XB360_H__
#define __VSF_USB_XB360_H__

/*============================ INCLUDES ======================================*/

#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usb_xb360_gamepad_in_report_t vsf_usb_xb360_gamepad_in_report_t;
struct vsf_usb_xb360_gamepad_in_report_t {
    uint8_t type;           // data0
    uint8_t length;         // data1
    uint8_t up:1;           // data2
    uint8_t down:1;
    uint8_t left:1;
    uint8_t right:1;
    uint8_t menu_right:1;   // start
    uint8_t menu_left:1;    // back
    uint8_t ls:1;
    uint8_t rs:1;
    uint8_t lb:1;           // data3
    uint8_t rb:1;
    uint8_t menu_main:1;    // xbox
    uint8_t dummy1:1;
    uint8_t a:1;
    uint8_t b:1;
    uint8_t x:1;
    uint8_t y:1;
    uint8_t lt;             // data4
    uint8_t rt;             // data5
    int16_t lx;             // data6 - data7
    int16_t ly;             // data8 - data9
    int16_t rx;             // data10 - data11
    int16_t ry;             // data12 - data13
    uint8_t dummy2[6];      // data14 - data19
} PACKED;

typedef struct vsf_usb_xb360_gamepad_out_report_t vsf_usb_xb360_gamepad_out_report_t;
struct vsf_usb_xb360_gamepad_out_report_t {
    uint8_t buffer[8];
} PACKED;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif    // __VSF_USB_XB360_H__

