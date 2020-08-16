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

#ifndef __VSF_INPUT_H__
#define __VSF_INPUT_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_input_cfg.h"
#include "utilities/vsf_utilities.h"

#if VSF_USE_INPUT == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_INPUT_ITEM_EX(__ITEM, __BITOFFSET, __BITLEN, __IS_SIGNED, __CONFIG) \
            {                                                                   \
                .item       = (__ITEM),                                         \
                .offset     = (__BITOFFSET),                                    \
                .bitlen     = (__BITLEN),                                       \
                .is_signed  = (__IS_SIGNED),                                    \
                .config     = (__CONFIG),                                       \
            }

#define VSF_INPUT_ITEM(__ITEM, __BITOFFSET, __BITLEN, __IS_SIGNED)              \
            VSF_INPUT_ITEM_EX((__ITEM), (__BITOFFSET), (__BITLEN), (__IS_SIGNED), false)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint32_t vk_input_timestamp_t;

typedef struct vk_input_item_info_t {
    uint32_t item       : 8;
    uint32_t bitlen     : 7;
    uint32_t is_signed  : 1;
    uint32_t offset     : 14;
    uint32_t config     : 1;
    uint32_t endian     : 1;
} vk_input_item_info_t;

typedef union vk_input_value_t {
    uint32_t bit    : 1;
    uint64_t valu64;
    uint32_t valu32;
    uint16_t valu16;
    uint8_t valu8;
    int32_t val32;
    int16_t val16;
    int8_t val8;
} vk_input_value_t;

typedef enum vk_input_type_t {
    VSF_INPUT_TYPE_UNKNOWN,
} vk_input_type_t;

typedef struct vk_input_evt_t {
    void *dev;
    uint32_t duration;          // duration in ms between pre and cur
    uint64_t id;
    vk_input_value_t pre;
    vk_input_value_t cur;
} vk_input_evt_t;

typedef struct vk_input_parser_t {
    vk_input_item_info_t *info;
    uint8_t num;

    vk_input_value_t pre;
    vk_input_value_t cur;
} vk_input_parser_t;

#if VSF_INPUT_CFG_REGISTRATION_MECHANISM == ENABLED
typedef void (*vk_input_on_evt_t)(vk_input_type_t type, vk_input_evt_t *evt);
typedef struct vk_input_notifier_t {
    vsf_slist_node_t notifier_node;
    vk_input_on_evt_t on_evt;
    uint8_t mask;
} vk_input_notifier_t;
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./protocol/vsf_input_gamepad.h"
#include "./protocol/vsf_input_sensor.h"
#include "./protocol/vsf_input_touchscreen.h"
#include "./protocol/vsf_input_keyboard.h"
#include "./protocol/vsf_input_mouse.h"

#include "./driver/hid/vsf_input_hid.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vk_input_buf_get_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vk_input_buf_set_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len, uint_fast32_t value);
extern void vk_input_buf_clear(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vk_input_buf_set(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);

extern vk_input_item_info_t * vk_input_parse(vk_input_parser_t *parser, uint8_t *pre, uint8_t *cur);

#if VSF_INPUT_CFG_REGISTRATION_MECHANISM == ENABLED
extern void vk_input_notifier_register(vk_input_notifier_t *notifier);
extern void vk_input_notifier_unregister(vk_input_notifier_t *notifier);
#endif

extern void vsf_input_on_sensor(vk_sensor_evt_t *sensor_evt);
extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_input_on_keyboard(vk_keyboard_evt_t *keyboard_evt);

extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_evt(vk_input_type_t type, vk_input_evt_t *evt);

// returns duration
extern uint_fast32_t vk_input_update_timestamp(vk_input_timestamp_t *timestamp);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_INPUT
#endif      // __VSF_INPUT_H__
/* EOF */