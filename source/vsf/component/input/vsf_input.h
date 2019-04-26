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

struct vsf_input_parser_t {
    vsf_input_item_info_t *info;
    uint8_t num;

    vsf_input_value_t pre;
    vsf_input_value_t cur;
};
typedef struct vsf_input_parser_t vsf_input_parser_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast32_t vsf_input_buf_get_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vsf_input_buf_set_value(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len, uint_fast32_t value);
extern void vsf_input_buf_clear(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);
extern void vsf_input_buf_set(uint8_t *buf, uint_fast8_t offset, uint_fast8_t len);

extern vsf_input_item_info_t * vsf_input_parse(vsf_input_parser_t *parser, uint8_t *pre, uint8_t *cur);

#endif
/* EOF */