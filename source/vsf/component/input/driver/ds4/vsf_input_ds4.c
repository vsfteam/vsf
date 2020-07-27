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

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_DS4 == ENABLED
// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

const vk_input_item_info_t vk_ds4u_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           47, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         45, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         44, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        46, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             48, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             49, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             54, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             55, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      52, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     53, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      56, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  SPECIAL,        57, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LX,     8,  8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LY,     16, 8,  false, true),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RX,     24, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RY,     32, 8,  false, true),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LT,     64, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RT,     72, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  DPAD,           40, 4,  false),
};

const vk_sensor_item_info_t vk_ds4u_sensor_item_info[6] = {
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_PITCH, 16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_YAW,   16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_ROLL,  16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Y,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Z,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_X,     16),
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_sensor(vk_sensor_evt_t *sensor_evt);

extern void vsf_ds4u_on_new_dev(vk_input_ds4u_t *dev);
extern void vsf_ds4u_on_free_dev(vk_input_ds4u_t *dev);
extern void vsf_ds4u_on_sensor(vk_sensor_evt_t *sensor_evt);
extern void vsf_ds4u_on_report_input(vk_gamepad_evt_t *gamepad_evt);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_DS4U_ON_NEW_DEV
WEAK(vsf_ds4u_on_new_dev)
void vsf_ds4u_on_new_dev(vk_input_ds4u_t *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_DS4, dev);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_FREE_DEV
WEAK(vsf_ds4u_on_free_dev)
void vsf_ds4u_on_free_dev(vk_input_ds4u_t *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_DS4, dev);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_REPORT_INPUT
WEAK(vsf_ds4u_on_report_input)
void vsf_ds4u_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_SENSOR
WEAK(vsf_ds4u_on_sensor)
void vsf_ds4u_on_sensor(vk_sensor_evt_t *sensor_evt)
{
    vsf_input_on_sensor(sensor_evt);
}
#endif

void vk_ds4u_new_dev(vk_input_ds4u_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
    vsf_ds4u_on_new_dev(dev);
}

void vk_ds4u_free_dev(vk_input_ds4u_t *dev)
{
    vsf_ds4u_on_free_dev(dev);
}

void vk_ds4u_process_input(vk_input_ds4u_t *dev, vsf_usb_ds4_gamepad_in_report_t *data)
{
    union {
        struct {
            vk_gamepad_evt_t evt;
            vk_input_item_info_t *info;
            vk_input_parser_t parser;
            bool event_sent;
        } gamepad;
        struct {
            vk_sensor_evt_t evt;
        } sensor;
        struct {
            vk_touchscreen_evt_t evt;
        } touch;
    } parser;

    parser.gamepad.evt.duration     = vk_input_update_timestamp(&dev->timestamp);
    parser.gamepad.evt.dev          = dev;

    parser.gamepad.event_sent       = false;
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_ds4u_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_ds4u_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
            vsf_ds4u_on_report_input(&parser.gamepad.evt);
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
        vsf_ds4u_on_report_input(&parser.gamepad.evt);
    }

    // sensor
    parser.sensor.evt.desc.item_info    = (vk_sensor_item_info_t *)vk_ds4u_sensor_item_info;
    parser.sensor.evt.desc.item_num     = dimof(vk_ds4u_sensor_item_info);
    parser.sensor.evt.data              = (uint8_t *)&data->gyro_pitch;
    vsf_ds4u_on_sensor(&parser.sensor.evt);

    // touch
    if (data->touch[0] > 0) {
        uint_fast16_t x, y;

        // TODO: get the real resolution
        parser.touch.evt.info.width     = 1920;
        parser.touch.evt.info.height    = 942;

        if ((dev->data.touch[2] != data->touch[2]) || !(data->touch[2] & 0x80)) {
            x = data->touch[3] + ((data->touch[4] & 0x0F) << 8);
            y = ((data->touch[4] & 0xF0) >> 4) + (data->touch[5] << 4);
            vsf_input_touchscreen_set(&parser.touch.evt, 0, !(data->touch[2] & 0x80), 0, x, y);
            vsf_input_on_touchscreen(&parser.touch.evt);
        }

        if ((dev->data.touch[6] != data->touch[6]) || !(data->touch[6] & 0x80)) {
            x = data->touch[7] + ((data->touch[8] & 0x0F) << 8);
            y = ((data->touch[8] & 0xF0) >> 4) + (data->touch[9] << 4);
            vsf_input_touchscreen_set(&parser.touch.evt, 1, !(data->touch[6] & 0x80), 0, x, y);
            vsf_input_on_touchscreen(&parser.touch.evt);
        }
    }

    dev->data = *data;
}

#endif      // VSF_USE_INPUT && VSF_USE_INPUT_DS4
