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

#if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_XB360 == ENABLED
// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

const vk_input_item_info_t vk_xb360_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           31, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         28, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         30, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        29, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             24, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             25, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             22, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             23, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      21, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     20, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      26, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LX,             48, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LY,             64, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RX,             80, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RY,             96, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LT,             32, 8,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RT,             40, 8,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_UP,           16, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_DOWN,         17, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_LEFT,         18, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_RIGHT,        19, 1,  false),
};

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN)                               \
    &&  defined(WEAK_VSF_INPUT_ON_GAMEPAD)
WEAK_VSF_INPUT_ON_GAMEPAD_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_NEW_DEV_EXTERN)                               \
    &&  defined(WEAK_VSF_INPUT_ON_NEW_DEV)
WEAK_VSF_INPUT_ON_NEW_DEV_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_FREE_DEV_EXTERN)                              \
    &&  defined(WEAK_VSF_INPUT_ON_FREE_DEV)
WEAK_VSF_INPUT_ON_FREE_DEV_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_XB360_ON_NEW_DEV
WEAK(vk_xb360_on_new_dev)
void vk_xb360_on_new_dev(vk_input_xb360_t *dev)
{
#   ifndef WEAK_VSF_INPUT_ON_NEW_DEV
    vk_input_on_new_dev(VSF_INPUT_TYPE_XB360, dev);
#   else
    WEAK_VSF_INPUT_ON_NEW_DEV(VSF_INPUT_TYPE_XB360, dev);
#   endif
}
#endif

#ifndef WEAK_VSF_XB360_ON_FREE_DEV
WEAK(vk_xb360_on_free_dev)
void vk_xb360_on_free_dev(vk_input_xb360_t *dev)
{
#   ifndef WEAK_VSF_INPUT_ON_FREE_DEV
    vk_input_on_free_dev(VSF_INPUT_TYPE_XB360, dev);
#   else
    WEAK_VSF_INPUT_ON_FREE_DEV(VSF_INPUT_TYPE_XB360, dev);
#   endif
}
#endif

#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
WEAK(vk_xb360_on_report_input)
void vk_xb360_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
#   ifndef WEAK_VSF_INPUT_ON_GAMEPAD
    vk_input_on_gamepad(gamepad_evt);
#   else
    WEAK_VSF_INPUT_ON_GAMEPAD(gamepad_evt);
#   endif
}
#endif

void vk_xb360_new_dev(vk_input_xb360_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
#ifndef WEAK_VSF_XB360_ON_NEW_DEV
    vk_xb360_on_new_dev(dev);
#else
    WEAK_VSF_XB360_ON_NEW_DEV(dev);
#endif
}

void vk_xb360_free_dev(vk_input_xb360_t *dev)
{
#ifndef WEAK_VSF_XB360_ON_FREE_DEV
    vk_xb360_on_free_dev(dev);
#else
    WEAK_VSF_XB360_ON_FREE_DEV(dev);
#endif
}

void vk_xb360_process_input(vk_input_xb360_t *dev, vsf_usb_xb360_gamepad_in_report_t *data)
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
    } parser;

    parser.gamepad.evt.duration     = vk_input_update_timestamp(&dev->timestamp);
    parser.gamepad.evt.dev          = dev;

    parser.gamepad.event_sent       = false;
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_xb360_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_xb360_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
            vk_xb360_on_report_input(&parser.gamepad.evt);
#else
            WEAK_VSF_XB360_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
        vk_xb360_on_report_input(&parser.gamepad.evt);
#else
        WEAK_VSF_XB360_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
    }

    dev->data = *data;
}

#endif      // VSF_USE_INPUT && VSF_USE_INPUT_XB360
