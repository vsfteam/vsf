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

#ifndef __VSF_HID_H__
#define __VSF_HID_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_input_cfg.h"

#if VSF_INPUT_CFG_HID_EN == ENABLED

#include "../vsf_input_get_type.h"

/*============================ MACROS ========================================*/

#define HID_USAGE_PAGE_GENERIC      0x01
#define HID_USAGE_PAGE_KEYBOARD     0x07
#define HID_USAGE_PAGE_BUTTON       0x09

// HID_USAGE_PAGE_GENERIC (Generic Desktop Page)
#define HID_USAGE_ID_X              0x30
#define HID_USAGE_ID_Y              0x31
#define HID_USAGE_ID_Z              0x32
#define HID_USAGE_ID_Rx             0x33
#define HID_USAGE_ID_Ry             0x34
#define HID_USAGE_ID_Rz             0x35
#define HID_USAGE_ID_Slider         0x36
#define HID_USAGE_ID_Dial           0x37
#define HID_USAGE_ID_Wheel          0x38
#define HID_USAGE_ID_Hat            0x39

#define HID_USAGE_ID_GENERIC_MIN    HID_USAGE_ID_X
#define HID_USAGE_ID_GENERIC_MAX    HID_USAGE_ID_Hat

#define HID_USAGE_ID_KEYBOARD_MIN   0
#define HID_USAGE_ID_KEYBOARD_BITS  224
#define HID_USAGE_ID_KEYBOARD_MAX   255

#define HID_USAGE_ID_BUTTON_MIN     1
#define HID_USAGE_ID_BUTTON_MAX     32

#define HID_GENERIC_DESKTOP_UNDEFINEED              0
#define HID_GENERIC_DESKTOP_POINTER                 1
#define HID_GENERIC_DESKTOP_MOUSE                   2
#define HID_GENERIC_DESKTOP_JOYSTICK                4
#define HID_GENERIC_DESKTOP_GAMEPAD                 5
#define HID_GENERIC_DESKTOP_KEYBOARD                6
#define HID_GENERIC_DESKTOP_KEYPAD                  7
#define HID_GENERIC_DESKTOP_MULTIAXIS_CONTROLLER    8

/*============================ MACROFIED FUNCTIONS ===========================*/

#define HID_USAGE_IS_CONST(__usage) ((__usage)->data_flag & 1)
#define HID_USAGE_IS_DATA(__usage)  !HID_USAGE_IS_CONST(__usage)
#define HID_USAGE_IS_VAR(__usage)   ((__usage)->data_flag & 2)
#define HID_USAGE_IS_ARRAY(__usage) !HID_USAGE_IS_VAR(__usage)
#define HID_USAGE_IS_REL(__usage)   ((__usage)->data_flag & 4)
#define HID_USAGE_IS_ABS(__usage)   !HID_USAGE_IS_REL(__usage)

#define HID_GET_GENERIC_USAGE(__id) ((__id >> 0) & 0xFFFF)
#define HID_GET_USAGE_PAGE(__id)    ((__id >> 16) & 0xFFFF)
#define HID_GET_USAGE_ID(__id)      ((__id >> 32) & 0xFFFF)

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_HID = VSF_INPUT_USER_TYPE,
};

struct vsf_hid_usage_t {
    implement_ex(vsf_slist_node_t, usage_node)

    uint16_t usage_page;
    uint8_t usage_min;
    uint8_t usage_max;
    int32_t logical_min;
    int32_t logical_max;
    int32_t bit_offset;
    int32_t bit_length;
    int32_t report_size;
    int32_t report_count;
    uint32_t data_flag;
};
typedef struct vsf_hid_usage_t vsf_hid_usage_t;

struct vsf_hid_event_t {
    implement(vsf_input_evt_t)
    vsf_hid_usage_t *usage;
};
typedef struct vsf_hid_event_t vsf_hid_event_t;

struct vsf_hid_report_t {
    implement_ex(vsf_slist_node_t, report_node)

    uint8_t type;
    int16_t id;
    uint16_t bitlen;
    uint16_t generic_usage;

    uint8_t *value;
    vsf_slist_t usage_list;
};
typedef struct vsf_hid_report_t vsf_hid_report_t;

struct vsf_input_hid_t {
    vsf_slist_t report_list;
    vsf_input_timestamp_t timestamp;
    bool report_has_id;
};
typedef struct vsf_input_hid_t vsf_input_hid_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vsf_hid_parse_desc(vsf_input_hid_t *dev, uint8_t *desc_buf, uint_fast32_t desc_len);
extern void vsf_hid_process_input(vsf_input_hid_t *dev, uint8_t *buf, uint_fast32_t len);
extern void vsf_hid_new_dev(vsf_input_hid_t *dev);
extern void vsf_hid_free_dev(vsf_input_hid_t *dev);

#endif      // VSF_INPUT_CFG_HID_EN
#endif      // __VSF_HID_H__
