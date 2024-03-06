/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "utilities/vsf_utilities_cfg.h"
#include "../compiler/compiler.h"
#include "vsf_bitmap.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int_fast8_t __vsf_arch_ffz(uintalu_t);
extern int_fast8_t __vsf_arch_ffs(uintalu_t);

/*============================ IMPLEMENTATION ================================*/

void __vsf_bitmap_range_assign(uintalu_t *bitmap_ptr, int_fast32_t begin_bit, int_fast32_t end_bit, int_fast32_t value)
{
    int_fast32_t bits = end_bit + 1 - begin_bit, maxbits_per_round = __optimal_bit_sz;
    int_fast32_t offset = begin_bit & __optimal_bit_msk, curbits;
    uintalu_t value_mask = value ? (uintalu_t)0 - 1 : 0, cur_mask;
    uintalu_t *ptr = &bitmap_ptr[begin_bit / maxbits_per_round];

    while (bits > 0) {
        curbits = vsf_min(bits, maxbits_per_round - offset);
        cur_mask = value_mask >> (maxbits_per_round - curbits);
        cur_mask <<= offset;

        *ptr &= ~cur_mask;
        *ptr++ |= value_mask & cur_mask;
        bits -= curbits;
        offset = 0;
    }
}

int_fast32_t __vsf_bitmap_ffz(uintalu_t *bitmap_ptr, int_fast32_t bit_size)
{
    int_fast32_t word_size =    (bit_size + (int_fast32_t)__optimal_bit_sz - 1)
                            /   (int_fast32_t)__optimal_bit_sz;
    int_fast32_t index = 0, i = 0, temp;

    for (; i < word_size; i++) {
        temp = __vsf_arch_ffz(bitmap_ptr[i]);
        if (temp >= 0) {
            index += temp;
            return index >= bit_size ? -1 : index;
        }
        index += __optimal_bit_sz;
    }
    return -1;
}

int_fast32_t __vsf_bitmap_ffs(uintalu_t *bitmap_ptr, int_fast32_t bit_size)
{
    int_fast32_t word_size =    (bit_size + (int_fast32_t)__optimal_bit_sz - 1)
                            /   (int_fast32_t)__optimal_bit_sz;
    int_fast32_t index = 0, i = 0, temp;

    for (; i < word_size; i++) {
        temp = __vsf_arch_ffs(bitmap_ptr[i]);
        if (temp >= 0) {
            index += temp;
            return index >= bit_size ? -1 : index;
        }
        index += __optimal_bit_sz;
    }
    return -1;
}

