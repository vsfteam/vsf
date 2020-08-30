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

    // 1. Declare the bitmap with bit_size
    // IMPORTANT: DANGER: RED: can not access bits larger than bit_size
    dcl_vsf_bitmap(xxx_bitmap, 32);

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

#include "../compiler/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
#define __vsf_bitmap(__name)        __name##_bitmap_t

#define __vsf_declare_bitmap_ex(__name, __bit_size)                             \
    typedef uintalu_t                                                           \
                __name[((__bit_size) + __optimal_bit_sz - 1) / __optimal_bit_sz];

#define __vsf_declare_bitmap(__name, __bit_size)                                \
    __vsf_declare_bitmap_ex(__vsf_bitmap(__name), __bit_size)



#define __vsf_bitmap_get(__bitmap_ptr, __bit, __pbit_val)                       \
    do {                                                                        \
        *(&(__bit_val)) =   __bitmap_ptr[(__bit) / __optimal_bit_sz]            \
                        &   ((uintalu_t)1 << ((__bit) & __optimal_bit_msk));    \
    } while (0)

#define __vsf_bitmap_set(__bitmap_ptr, __bit)                                   \
    do {                                                                        \
        (__bitmap_ptr)[(__bit) / __optimal_bit_sz] |=                           \
            ((uintalu_t)1 << ((__bit) & __optimal_bit_msk));                    \
    } while (0)

#define __vsf_bitmap_clear(__bitmap_ptr, __bit)                                 \
    do {                                                                        \
        (__bitmap_ptr)[(__bit) / __optimal_bit_sz] &=                           \
            ~((uintalu_t)1 << ((__bit) & __optimal_bit_msk));                   \
    } while (0)

//! \name bitmap normal access
//! @{
#define vsf_bitmap(__name)          __vsf_bitmap(__name)

#define vsf_declare_bitmap(__name, __bit_size)                                  \
            __vsf_declare_bitmap(__name, __bit_size)

#define dcl_vsf_bitmap(__name, __bit_size)                                      \
            vsf_declare_bitmap(__name, __bit_size)

#define declare_vsf_bitmap(__name, __bit_size)                                  \
            vsf_declare_bitmap(__name, __bit_size)

#define vsf_bitmap_get(__bitmap_ptr, __bit, __pbit_val)                         \
            __vsf_bitmap_get((uintalu_t *)(__bitmap_ptr), (__bit), (__pbit_val))

#define vsf_bitmap_set(__bitmap_ptr, __bit)                                     \
            __vsf_bitmap_set((uintalu_t *)(__bitmap_ptr), (__bit))

#define vsf_bitmap_clear(__bitmap_ptr, __bit)                                   \
            __vsf_bitmap_clear((uintalu_t *)(__bitmap_ptr), (__bit))

#define vsf_bitmap_reset(__bitmap_ptr, __bit_size)                              \
            __vsf_bitmap_reset((uintalu_t *)(__bitmap_ptr), (__bit_size))

#define vsf_bitmap_ffz(__bitmap_ptr, __bit_size)                                \
            __vsf_bitmap_ffz((uintalu_t *)(__bitmap_ptr), (__bit_size))
//! @}

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_bitmap_reset(uintalu_t * this_ptr, int_fast16_t bit_size);
extern int_fast16_t __vsf_bitmap_ffz(   uintalu_t * bitmap_ptr,  
                                        int_fast16_t bit_size);

#ifdef __cplusplus
}
#endif

#endif
