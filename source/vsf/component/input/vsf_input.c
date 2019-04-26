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
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
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
