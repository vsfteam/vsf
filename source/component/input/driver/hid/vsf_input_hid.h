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

#ifndef __VSF_INPUT_HID_H__
#define __VSF_INPUT_HID_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_HID == ENABLED

#include "../../vsf_input_get_type.h"
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define HID_USAGE_PAGE_GENERIC                      0x01
#define HID_USAGE_PAGE_KEYBOARD                     0x07
#define HID_USAGE_PAGE_BUTTON                       0x09
#define HID_USAGE_PAGE_DIGITIZERS                   0x0D

// HID_USAGE_PAGE_GENERIC (Generic Desktop Page)
#define HID_USAGE_ID_X                              0x30
#define HID_USAGE_ID_Y                              0x31
#define HID_USAGE_ID_Z                              0x32
#define HID_USAGE_ID_Rx                             0x33
#define HID_USAGE_ID_Ry                             0x34
#define HID_USAGE_ID_Rz                             0x35
#define HID_USAGE_ID_Slider                         0x36
#define HID_USAGE_ID_Dial                           0x37
#define HID_USAGE_ID_Wheel                          0x38
#define HID_USAGE_ID_Hat                            0x39

// HID_USAGE_PAGE_DIGITIZERS (Digitizers Page)
#define HID_USAGE_ID_TOUCH_SCREEN                   0x04
#define HID_USAGE_ID_TIP_PRESSURE                   0x30
#define HID_USAGE_ID_TIP_SWITCH                     0x42
#define HID_USAGE_ID_TIP_ID                         0x51

#define HID_USAGE_ID_GENERIC_MIN                    HID_USAGE_ID_X
#define HID_USAGE_ID_GENERIC_MAX                    HID_USAGE_ID_Hat

#define HID_USAGE_ID_KEYBOARD_MIN                   0
#define HID_USAGE_ID_KEYBOARD_BITS                  224
#define HID_USAGE_ID_KEYBOARD_MAX                   255

#define HID_USAGE_ID_BUTTON_MIN                     1
#define HID_USAGE_ID_BUTTON_MAX                     32

#define HID_GENERIC_DESKTOP_UNDEFINEED              0
#define HID_GENERIC_DESKTOP_POINTER                 1
#define HID_GENERIC_DESKTOP_MOUSE                   2
#define HID_GENERIC_DESKTOP_JOYSTICK                4
#define HID_GENERIC_DESKTOP_GAMEPAD                 5
#define HID_GENERIC_DESKTOP_KEYBOARD                6
#define HID_GENERIC_DESKTOP_KEYPAD                  7
#define HID_GENERIC_DESKTOP_MULTIAXIS_CONTROLLER    8

/*============================ MACROFIED FUNCTIONS ===========================*/

#define hid_usage_is_const(__usage)                 ((__usage)->data_flag & 1)
#define hid_usage_is_data(__usage)                  !hid_usage_is_const(__usage)
#define hid_usage_is_var(__usage)                   ((__usage)->data_flag & 2)
#define hid_usage_is_array(__usage)                 !hid_usage_is_var(__usage)
#define hid_usage_is_rel(__usage)                   ((__usage)->data_flag & 4)
#define hid_usage_is_abs(__usage)                   !hid_usage_is_rel(__usage)

#define hid_get_generic_usage_id(__evt)             ((uint8_t)((__evt)->id >> 0))
#define hid_get_generic_usage_page(__evt)           ((uint8_t)((__evt)->id >> 8))
#define hid_get_usage_id(__evt)                     ((uint8_t)((__evt)->id >> 16))
#define hid_get_usage_page(__evt)                   ((uint8_t)((__evt)->id >> 24))

/*============================ TYPES =========================================*/

enum {
    VSF_INPUT_TYPE_HID = VSF_INPUT_USER_TYPE,
};

typedef struct vk_hid_usage_t {
    implement_ex(vsf_slist_node_t, usage_node)

    uint8_t generic_usage_page;
    uint8_t generic_usage_id;
    uint8_t data_flag;
    uint8_t usage_page;
    uint8_t usage_min;
    uint8_t usage_max;
    int32_t logical_min;
    int32_t logical_max;
    int32_t bit_offset;
    int32_t bit_length;
    int32_t report_size;
    int32_t report_count;
} vk_hid_usage_t;

typedef struct vk_hid_evt_t {
    implement(vk_input_evt_t)
    vk_hid_usage_t *usage;
} vk_hid_evt_t;

typedef struct vk_hid_report_t {
    implement_ex(vsf_slist_node_t, report_node)

    uint8_t type;
    int16_t id;
    uint16_t bitlen;

    uint8_t *value;
    vsf_slist_t usage_list;
} vk_hid_report_t;

typedef struct vk_input_hid_t {
    vsf_slist_t report_list;
    vk_input_timestamp_t timestamp;
    bool report_has_id;
} vk_input_hid_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast8_t vk_hid_parse_desc(vk_input_hid_t *dev, uint8_t *desc_buf, uint_fast32_t desc_len);
extern void vk_hid_process_input(vk_input_hid_t *dev, uint8_t *buf, uint_fast32_t len);
extern void vk_hid_new_dev(vk_input_hid_t *dev);
extern void vk_hid_free_dev(vk_input_hid_t *dev);

extern void vsf_hid_parse_touch_screen(vk_hid_evt_t *hid_evt);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT && VSF_INPUT_USE_HID
#endif      // __VSF_INPUT_HID_H__
