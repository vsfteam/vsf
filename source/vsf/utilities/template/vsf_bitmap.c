/****************************************************************************
*  Copyright 2018 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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
#include "vsf_cfg.h"
#include "vsf_bitmap.h"

/*============================ MACROS ========================================*/


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



/*! \brief reset a bitmap
 *! \param pthis    address of the target bitmap
 *! \param biesize  the size of the bitmap in bits
 */
void __vsf_bitmap_reset(uint_fast8_t *pthis, int_fast16_t bitsize)
{
    memset(pthis, 0, (bitsize + 7) >> 3);
}

WEAK(msb)
int_fast16_t msb(uint_fast8_t a)
{
    int_fast16_t c = -1;
    while (a > 0) {
        c++;
        a >>= 1;
    }
    return c;
}

#ifndef VSF_FFZ
WEAK(ffz)
int_fast16_t ffz(uint_fast8_t a)
{
    a = ~a;
    return msb(a & -(int_fast8_t)a);
}
#endif

int_fast16_t __vsf_bitmap_ffz(uint_fast8_t *pbitmap, int_fast16_t bitsize)
{
    int_fast16_t dwordsize = (bitsize + __optimal_bit_sz - 1) / __optimal_bit_sz, i;
    int_fast16_t index;

    for (i = 0; i < dwordsize; i++) {
        index = ffz(pbitmap[i]);
        if (index >= 0) {
            return index;
        }
    }
    return -1;
}
