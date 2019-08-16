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

/*============================ MACROS ========================================*/

#define VSF_INPUT_ITEM(__id, __bitoffset, __bitlen, __is_signed)                \
            {                                                                   \
                .id         = (__id),                                           \
                .offset     = (__bitoffset),                                    \
                .bitlen     = (__bitlen),                                       \
                .is_signed  = (__is_signed),                                    \
            }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint32_t vsf_input_timestamp_t;

struct vsf_input_item_info_t {
    uint8_t id;
    uint8_t bitlen      : 7;
    uint8_t is_signed   : 1;
    uint16_t offset;
};
typedef struct vsf_input_item_info_t vsf_input_item_info_t;

union vsf_input_value_t {
    uint32_t bit    : 1;
    uint32_t valu32;
    uint16_t valu16;
    uint8_t valu8;
    int32_t val32;
    int16_t val16;
    int8_t val8;
};
typedef union vsf_input_value_t vsf_input_value_t;

enum vsf_input_type_t {
    VSF_INPUT_TYPE_UNKNOWN,
};
typedef enum vsf_input_type_t vsf_input_type_t;

struct vsf_input_evt_t {
    void *dev;
    uint32_t duration;          // duration in ms between pre and cur
    uint64_t id;
    vsf_input_value_t pre;
    vsf_input_value_t cur;
};
typedef struct vsf_input_evt_t vsf_input_evt_t;

struct vsf_input_parser_t {
    vsf_input_item_info_t *info;
    uint8_t num;

    vsf_input_value_t pre;
    vsf_input_value_t cur;
};
typedef struct vsf_input_parser_t vsf_input_parser_t;

/*============================ INCLUDES ======================================*/

#include "./protocol/vsf_input_gamepad.h"
#include "./protocol/vsf_input_sensor.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vsf_input_buf_get_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vsf_input_buf_set_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len, uint_fast32_t value);
extern void vsf_input_buf_clear(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vsf_input_buf_set(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);

extern vsf_input_item_info_t * vsf_input_parse(vsf_input_parser_t *parser, uint8_t *pre, uint8_t *cur);

extern void vsf_input_on_sensor(vsf_sensor_evt_t *sensor_evt);

extern void vsf_input_on_new_dev(vsf_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vsf_input_type_t type, void *dev);
extern void vsf_input_on_evt(vsf_input_type_t type, vsf_input_evt_t *evt);

// returns duration
extern uint_fast32_t vsf_input_update_timestamp(vsf_input_timestamp_t *timestamp);
#endif
/* EOF */