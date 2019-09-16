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

#ifndef __VSF_BITMAP_H__
#define __VSF_BITMAP_H__

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Declare the bitmap with bitsize
    // IMPORTANT: DANGER: RED: can not access bits larger than bitsize
    vsf_declare_bitmap(xxx_bitmap, 32);

    // 2. Defining pool variable
    static vsf_bitmap(xxxx_bitmap) usr_xxxx_bitmap;

    void user_example_task(void)
    {
        ......

        ......

        // 3. Set bit in bitmap
        vsf_bitmap_set(&usr_xxxx_bitmap, 10);
        // 4. Clear bit in bitmap
        vsf_bitmap_clear(&usr_xxxx_bitmap, 12);
        // 5. TODO: Find First Set/Zero bit in bitmap
        vsf_bitmap_ffs(&usr_xxxx_bitmap);
        vsf_bitmap_ffz(&usr_xxxx_bitmap);

        ......
    }
 */

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define __optimal_bit_sz        (sizeof(uint_fast8_t) * 8)
#define __optimal_bit_msk       (__optimal_bit_sz - 1)

#define __vsf_bitmap(__name)        __name##_bitmap_t

#define __vsf_declare_bitmap(__name, __bitsize)                                 \
typedef uint_fast8_t __vsf_bitmap(__name)[                                      \
            ((__bitsize) + __optimal_bit_sz - 1) / __optimal_bit_sz];



#define __vsf_bitmap_get(__pbitmap, __bit, __pbit_val)                          \
    do {                                                                        \
        *(&(__bit_val)) =   __pbitmap[(__bit) / __optimal_bit_sz]               \
                        &   (1 << ((__bit) & __optimal_bit_msk));               \
    } while (0)

#define __vsf_bitmap_set(__pbitmap, __bit)                                      \
    do {                                                                        \
        (__pbitmap)[(__bit) / __optimal_bit_sz] |=                              \
            (1 << ((__bit) & __optimal_bit_msk));                               \
    } while (0)

#define __vsf_bitmap_clear(__pbitmap, __bit)                                    \
    do {                                                                        \
        (__pbitmap)[(__bit) / __optimal_bit_sz] &=                              \
            ~(1 << ((__bit) & __optimal_bit_msk));                              \
    } while (0)

//! \name bitmap normal access
//! @{
#define vsf_bitmap(__name)          __vsf_bitmap(__name)

#define vsf_declare_bitmap(__name, __bitsize)                                   \
            __vsf_declare_bitmap(__name, __bitsize)

#define vsf_bitmap_get(__pbitmap, __bit, __pbit_val)                            \
            __vsf_bitmap_get((uint_fast8_t *)(__pbitmap), (__bit), (__pbit_val))

#define vsf_bitmap_set(__pbitmap, __bit)                                        \
            __vsf_bitmap_set((uint_fast8_t *)(__pbitmap), (__bit))

#define vsf_bitmap_clear(__pbitmap, __bit)                                      \
            __vsf_bitmap_clear((uint_fast8_t *)(__pbitmap), (__bit))

#define vsf_bitmap_reset(__pbitmap, __bitsize)                                  \
            __vsf_bitmap_reset((uint_fast8_t *)(__pbitmap), (__bitsize))

#define vsf_bitmap_ffz(__pbitmap, __bitsize)                                    \
            __vsf_bitmap_ffz((uint_fast8_t *)(__pbitmap), (__bitsize))
//! @}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifndef VSF_FFZ
extern int_fast16_t ffz(uint_fast8_t);
#endif
extern void __vsf_bitmap_reset(uint_fast8_t* pthis, int_fast16_t bitsize);
extern int_fast16_t __vsf_bitmap_ffz(uint_fast8_t* pbitmap,  int_fast16_t bitsize);

#endif
