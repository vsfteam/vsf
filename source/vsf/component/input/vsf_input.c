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

#include "./vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED

#include "./vsf_input.h"
#include "kernel/vsf_kernel.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_INPUT_ON_EVT_EXTERN)                                   \
    &&  defined(WEAK_VSF_INPUT_ON_EVT)
WEAK_VSF_INPUT_ON_EVT_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

void vsf_input_buf_set(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len)
{
    uint_fast8_t bitlen, bitpos, bytepos, mask, result_len = 0;

    while (result_len < len) {
        bytepos = offset >> 3;
        bitpos = offset & 7;
        bitlen = min(len - result_len, 8 - bitpos);
        mask = (1 << bitlen) - 1;

        buf[bytepos] |= ((~0UL >> result_len) & mask) << bitpos;

        offset += bitlen;
        result_len += bitlen;
    }
}

void vsf_input_buf_clear(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len)
{
    uint_fast8_t bitlen, bitpos, bytepos, mask, result_len = 0;

    while (result_len < len) {
        bytepos = offset >> 3;
        bitpos = offset & 7;
        bitlen = min(len - result_len, 8 - bitpos);
        mask = (1 << bitlen) - 1;

        buf[bytepos] &= ~(((~0UL >> result_len) & mask) << bitpos);

        offset += bitlen;
        result_len += bitlen;
    }
}

uint_fast32_t vsf_input_buf_get_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len)
{
    uint_fast8_t bitlen, bitpos, bytepos, mask, result_len = 0;
    uint_fast32_t result = 0;

    while (result_len < len) {
        bytepos = offset >> 3;
        bitpos = offset & 7;
        bitlen = min(len - result_len, 8 - bitpos);
        mask = (1 << bitlen) - 1;

        result |= ((buf[bytepos] >> bitpos) & mask) << result_len;

        offset += bitlen;
        result_len += bitlen;
    }
    return result;
}

void vsf_input_buf_set_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len, uint_fast32_t value)
{
    uint_fast8_t bitlen, bitpos, bytepos, mask, result_len = 0;

    while (result_len < len) {
        bytepos = offset >> 3;
        bitpos = offset & 7;
        bitlen = min(len - result_len, 8 - bitpos);
        mask = (1 << bitlen) - 1;

        buf[bytepos] &= ~(((~0UL >> result_len) & mask) << bitpos);
        buf[bytepos] |= ((value >> result_len) & mask) << bitpos;

        offset += bitlen;
        result_len += bitlen;
    }
}

vsf_input_item_info_t * vsf_input_parse(vsf_input_parser_t *parser, uint8_t *pre, uint8_t *cur)
{
    vsf_input_item_info_t *info = &parser->info[parser->num - 1];
    uint_fast32_t pre_value, cur_value;

    while (parser->num-- > 0) {
        cur_value = vsf_input_buf_get_value(cur, info->offset, info->bitlen);
        pre_value = vsf_input_buf_get_value(pre, info->offset, info->bitlen);

        if (cur_value != pre_value) {
            parser->cur.valu32 = cur_value;
            parser->pre.valu32 = pre_value;
            return info;
        }
        info--;
    }
    return NULL;
}

#ifndef WEAK_VSF_INPUT_ON_SENSOR
WEAK(vsf_input_on_sensor)
void vsf_input_on_sensor(vsf_sensor_evt_t *sensor_evt)
{
#ifndef WEAK_VSF_INPUT_ON_EVT
    vsf_input_on_evt(VSF_INPUT_TYPE_SENSOR, &sensor_evt->use_as__vsf_input_evt_t);
#else
    WEAK_VSF_INPUT_ON_EVT(VSF_INPUT_TYPE_SENSOR, &sensor_evt->use_as__vsf_input_evt_t);
#endif
}
#endif

#ifndef WEAK_VSF_INPUT_ON_TOUCHSCREEN
WEAK(vsf_input_on_touchscreen)
void vsf_input_on_touchscreen(vsf_touchscreen_evt_t *ts_evt)
{
#ifndef WEAK_VSF_INPUT_ON_EVT
    vsf_input_on_evt(VSF_INPUT_TYPE_TOUCHSCREEN, &ts_evt->use_as__vsf_input_evt_t);
#else
    WEAK_VSF_INPUT_ON_EVT(VSF_INPUT_TYPE_TOUCHSCREEN, &ts_evt->use_as__vsf_input_evt_t);
#endif
}
#endif

#ifndef WEAK_VSF_INPUT_ON_GAMEPAD
WEAK(vsf_input_on_gamepad)
void vsf_input_on_gamepad(vsf_gamepad_evt_t *gamepad_evt)
{
#ifndef WEAK_VSF_INPUT_ON_EVT
    vsf_input_on_evt(VSF_INPUT_TYPE_GAMEPAD, &gamepad_evt->use_as__vsf_input_evt_t);
#else
    WEAK_VSF_INPUT_ON_EVT(VSF_INPUT_TYPE_GAMEPAD, &gamepad_evt->use_as__vsf_input_evt_t);
#endif
}
#endif

#ifndef WEAK_VSF_INPUT_ON_KEYBOARD
WEAK(vsf_input_on_keyboard)
void vsf_input_on_keyboard(vsf_keyboard_evt_t *keyboard_evt)
{
#ifndef WEAK_VSF_INPUT_ON_EVT
    vsf_input_on_evt(VSF_INPUT_TYPE_KEYBOARD, &keyboard_evt->use_as__vsf_input_evt_t);
#else
    WEAK_VSF_INPUT_ON_EVT(VSF_INPUT_TYPE_KEYBOARD, &keyboard_evt->use_as__vsf_input_evt_t);
#endif
}
#endif

#ifndef WEAK_VSF_INPUT_ON_NEW_DEV
WEAK(vsf_input_on_new_dev)
void vsf_input_on_new_dev(vsf_input_type_t type, void *dev)
{
}
#endif

#ifndef WEAK_VSF_INPUT_ON_FREE_DEV
WEAK(vsf_input_on_free_dev)
void vsf_input_on_free_dev(vsf_input_type_t type, void *dev)
{
}
#endif

#ifndef WEAK_VSF_INPUT_ON_EVT
WEAK(vsf_input_on_evt)
void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *evt)
{
}
#endif

uint_fast32_t vsf_input_update_timestamp(vsf_input_timestamp_t *timestamp)
{
    vsf_input_timestamp_t cur = vsf_timer_get_tick();
    uint_fast32_t duration = *timestamp > 0 ? vsf_timer_get_duration(*timestamp, cur) : 0;
    *timestamp = cur;
    return duration;
}

#endif      // VSF_USE_INPUT
