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




/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

// for example:
//  #define __VSF_HAL_TEMPLATE_NAME              _adc
//  #define __VSF_HAL_TEMPLATE_UPCASE_NAME       _ADC

// expand to:
//  #define __VSF_HAL_TEMPLATE_PREFIX            VSF_ADC_CFG_IMP_PREFIX
//  #define __VSF_HAL_TEMPLATE_UPCASE_PREFIX     VSF_ADC_CFG_IMP_UPCASE_PREFIX
#define __VSF_HAL_TEMPLATE_PREFIX                VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMP_PREFIX)
#define __VSF_HAL_TEMPLATE_UPCASE_PREFIX         VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMP_UPCASE_PREFIX)
#define __VSF_HAL_TEMPLATE_EXTERN_OP             VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMP_EXTERN_OP)

// #if VSF_HW_ADC_CFG_MULTI_CLASS == ENABLED
#if VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_MULTI_CLASS) == ENABLED
#   if __VSF_HAL_TEMPLATE_EXTERN_OP == DISABLED
#       define __VSF_HAL_OP_VAR     VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _op)
#       define __VSF_HAL_OP_ATR     static
#   else
#       define __VSF_HAL_OP_VAR     VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _op)
#       define __VSF_HAL_OP_ATR
#   endif
#   define __VSF_HAL_OP             .VSF_MCONNECT(vsf, __VSF_HAL_TEMPLATE_NAME).op = & __VSF_HAL_OP_VAR,

#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_OP

__VSF_HAL_OP_ATR const VSF_MCONNECT(vsf, __VSF_HAL_TEMPLATE_NAME, _op_t) __VSF_HAL_OP_VAR = {
    VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _APIS) (__VSF_HAL_TEMPLATE_PREFIX)
};

#else
#   define __VSF_HAL_OP
#endif

// expand to:
//  #define __VSF_HAL_TEMPLATE_IMP_COUNT VSF_HW_ADC_COUNT
#if !defined(__VSF_HAL_TEMPLATE_IMP_COUNT) && VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#   define __VSF_HAL_TEMPLATE_IMP_COUNT  VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#endif

#if !defined(__VSF_HAL_TEMPLATE_IMP_MASK) && VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
// expand to:
//  #define __VSF_HAL_TEMPLATE_IMP_MASK     VSF_HW_ADC_MASK
#   define __VSF_HAL_TEMPLATE_IMP_MASK   VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
#endif

#ifndef __VSF_HAL_TEMPLATE_IMP_COUNT
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
#        define __VSF_HAL_TEMPLATE_IMP_MASK0 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK0 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
#        define __VSF_HAL_TEMPLATE_IMP_MASK1 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK1 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
#        define __VSF_HAL_TEMPLATE_IMP_MASK2 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK2 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
#        define __VSF_HAL_TEMPLATE_IMP_MASK3 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK3 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
#        define __VSF_HAL_TEMPLATE_IMP_MASK4 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK4 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
#        define __VSF_HAL_TEMPLATE_IMP_MASK5 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK5 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
#        define __VSF_HAL_TEMPLATE_IMP_MASK6 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK6 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
#        define __VSF_HAL_TEMPLATE_IMP_MASK7 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK7 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
#        define __VSF_HAL_TEMPLATE_IMP_MASK8 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK8 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
#        define __VSF_HAL_TEMPLATE_IMP_MASK9 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK9 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
#        define __VSF_HAL_TEMPLATE_IMP_MASK10 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK10 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
#        define __VSF_HAL_TEMPLATE_IMP_MASK11 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK11 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
#        define __VSF_HAL_TEMPLATE_IMP_MASK12 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK12 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
#        define __VSF_HAL_TEMPLATE_IMP_MASK13 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK13 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
#        define __VSF_HAL_TEMPLATE_IMP_MASK14 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK14 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
#        define __VSF_HAL_TEMPLATE_IMP_MASK15 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK15 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
#        define __VSF_HAL_TEMPLATE_IMP_MASK16 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK16 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
#        define __VSF_HAL_TEMPLATE_IMP_MASK17 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK17 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
#        define __VSF_HAL_TEMPLATE_IMP_MASK18 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK18 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
#        define __VSF_HAL_TEMPLATE_IMP_MASK19 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK19 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
#        define __VSF_HAL_TEMPLATE_IMP_MASK20 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK20 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
#        define __VSF_HAL_TEMPLATE_IMP_MASK21 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK21 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
#        define __VSF_HAL_TEMPLATE_IMP_MASK22 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK22 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
#        define __VSF_HAL_TEMPLATE_IMP_MASK23 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK23 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
#        define __VSF_HAL_TEMPLATE_IMP_MASK24 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK24 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
#        define __VSF_HAL_TEMPLATE_IMP_MASK25 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK25 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
#        define __VSF_HAL_TEMPLATE_IMP_MASK26 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK26 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
#        define __VSF_HAL_TEMPLATE_IMP_MASK27 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK27 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
#        define __VSF_HAL_TEMPLATE_IMP_MASK28 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK28 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
#        define __VSF_HAL_TEMPLATE_IMP_MASK29 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK29 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
#        define __VSF_HAL_TEMPLATE_IMP_MASK30 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK30 0
#    endif
#    if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
#        define __VSF_HAL_TEMPLATE_IMP_MASK31 1
#    else
#        define __VSF_HAL_TEMPLATE_IMP_MASK31 0
#    endif
#    define __VSF_HAL_TEMPLATE_IMP_COUNT (                                   \
         __VSF_HAL_TEMPLATE_IMP_MASK0 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK1 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK2 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK3 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK4 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK5 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK6 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK7 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK8 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK9 +                                      \
         __VSF_HAL_TEMPLATE_IMP_MASK10 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK11 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK12 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK13 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK14 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK15 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK16 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK17 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK18 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK19 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK20 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK21 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK22 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK23 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK24 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK25 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK26 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK27 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK28 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK29 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK30 +                                     \
         __VSF_HAL_TEMPLATE_IMP_MASK31                                       \
    )
#   endif

#ifndef __VSF_HAL_TEMPLATE_IMP_MASK
#    define __VSF_HAL_TEMPLATE_IMP_MASK   ((1ull << __VSF_HAL_TEMPLATE_IMP_COUNT) - 1)
#endif

#define __VSF_HAL_TEMPLATE_IMP_LV0   VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMP_LV0)
#define __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(__INDEX, __NULL) __INDEX,

#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
     __VSF_HAL_TEMPLATE_IMP_LV0(0, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
     __VSF_HAL_TEMPLATE_IMP_LV0(1, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
     __VSF_HAL_TEMPLATE_IMP_LV0(2, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
     __VSF_HAL_TEMPLATE_IMP_LV0(3, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
     __VSF_HAL_TEMPLATE_IMP_LV0(4, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
     __VSF_HAL_TEMPLATE_IMP_LV0(5, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
     __VSF_HAL_TEMPLATE_IMP_LV0(6, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
     __VSF_HAL_TEMPLATE_IMP_LV0(7, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
     __VSF_HAL_TEMPLATE_IMP_LV0(8, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
     __VSF_HAL_TEMPLATE_IMP_LV0(9, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
     __VSF_HAL_TEMPLATE_IMP_LV0(10, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
     __VSF_HAL_TEMPLATE_IMP_LV0(11, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
     __VSF_HAL_TEMPLATE_IMP_LV0(12, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
     __VSF_HAL_TEMPLATE_IMP_LV0(13, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
     __VSF_HAL_TEMPLATE_IMP_LV0(14, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
     __VSF_HAL_TEMPLATE_IMP_LV0(15, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
     __VSF_HAL_TEMPLATE_IMP_LV0(16, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
     __VSF_HAL_TEMPLATE_IMP_LV0(17, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
     __VSF_HAL_TEMPLATE_IMP_LV0(18, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
     __VSF_HAL_TEMPLATE_IMP_LV0(19, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
     __VSF_HAL_TEMPLATE_IMP_LV0(20, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
     __VSF_HAL_TEMPLATE_IMP_LV0(21, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
     __VSF_HAL_TEMPLATE_IMP_LV0(22, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
     __VSF_HAL_TEMPLATE_IMP_LV0(23, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
     __VSF_HAL_TEMPLATE_IMP_LV0(24, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
     __VSF_HAL_TEMPLATE_IMP_LV0(25, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
     __VSF_HAL_TEMPLATE_IMP_LV0(26, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
     __VSF_HAL_TEMPLATE_IMP_LV0(27, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
     __VSF_HAL_TEMPLATE_IMP_LV0(28, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
     __VSF_HAL_TEMPLATE_IMP_LV0(29, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
     __VSF_HAL_TEMPLATE_IMP_LV0(30, __VSF_HAL_OP)
#endif
#if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
     __VSF_HAL_TEMPLATE_IMP_LV0(31, __VSF_HAL_OP)
#endif

#if !defined(__VSF_HAL_TEMPLATE_CFG_REMOVE_ARRAY) && (__VSF_HAL_TEMPLATE_IMP_MASK != 0)

#   ifndef __VSF_HAL_TEMPLATE_TYPE_PREFIX
#define __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(__INDEX, __NULL)                             \
                &VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __INDEX),

VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const                \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_IMP_COUNT] = {

#else
#define __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(__INDEX, __NULL)                             \
                &VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __INDEX),

VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const                \
    VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_IMP_COUNT] = {

#   endif

#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(0, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(1, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(2, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(3, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(4, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(5, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(6, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(7, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(8, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(9, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(10, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(11, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(12, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(13, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(14, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(15, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(16, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(17, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(18, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(19, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(20, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(21, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(22, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(23, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(24, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(25, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(26, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(27, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(28, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(29, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(30, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
        __VSF_HAL_TEMPLATE_INSTANCE_ARRAY_ITEM(31, __VSF_HAL_OP)
#   endif
};

#   ifdef __VSF_HAL_TEMPLATE_TYPE_PREFIX
#define __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(__INDEX, __NULL)           \
                &VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __INDEX),
VSF_MCONNECT(vsf_, __VSF_HAL_TEMPLATE_TYPE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, , __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_IMP_COUNT] = {
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(0, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(1, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(2, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(3, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(4, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(5, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(6, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(7, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(8, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(9, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(10, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(11, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(12, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(13, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(14, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(15, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(16, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(17, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(18, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(19, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(20, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(21, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(22, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(23, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(24, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(25, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(26, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(27, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(28, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(29, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(30, __VSF_HAL_OP)
#       endif
#       if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
            __VSF_HAL_TEMPLATE_OTHER_INSTANCE_ARRAY_ITEM(31, __VSF_HAL_OP)
#       endif
};
#   endif

const uint8_t VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _indexs)[__VSF_HAL_TEMPLATE_IMP_COUNT] = {
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(0, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(1, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(2, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(3, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(4, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(5, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(6, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(7, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(8, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(9, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(10, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(11, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(12, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(13, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(14, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(15, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(16, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(17, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(18, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(19, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(20, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(21, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(22, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(23, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(24, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(25, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(26, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(27, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(28, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(29, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(30, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
        __VSF_HAL_TEMPLATE_INDEX_ARRAY_ITEM(31, __VSF_HAL_OP)
#   endif
};
#endif

#undef __VSF_HAL_TEMPLATE_PREFIX
#undef __VSF_HAL_TEMPLATE_TYPE_PREFIX
#undef __VSF_HAL_TEMPLATE_UPCASE_PREFIX
#undef __VSF_HAL_TEMPLATE_EXTERN_OP
#undef __VSF_HAL_TEMPLATE_NAME
#undef __VSF_HAL_TEMPLATE_UPCASE_NAME
#undef __VSF_HAL_TEMPLATE_APIS
#undef __VSF_HAL_TEMPLATE_IMP_COUNT
#undef __VSF_HAL_TEMPLATE_IMP_MASK
#undef __VSF_HAL_TEMPLATE_IMP_MASK0
#undef __VSF_HAL_TEMPLATE_IMP_MASK1
#undef __VSF_HAL_TEMPLATE_IMP_MASK2
#undef __VSF_HAL_TEMPLATE_IMP_MASK3
#undef __VSF_HAL_TEMPLATE_IMP_MASK4
#undef __VSF_HAL_TEMPLATE_IMP_MASK5
#undef __VSF_HAL_TEMPLATE_IMP_MASK6
#undef __VSF_HAL_TEMPLATE_IMP_MASK7
#undef __VSF_HAL_TEMPLATE_IMP_MASK8
#undef __VSF_HAL_TEMPLATE_IMP_MASK9
#undef __VSF_HAL_TEMPLATE_IMP_MASK10
#undef __VSF_HAL_TEMPLATE_IMP_MASK11
#undef __VSF_HAL_TEMPLATE_IMP_MASK12
#undef __VSF_HAL_TEMPLATE_IMP_MASK13
#undef __VSF_HAL_TEMPLATE_IMP_MASK14
#undef __VSF_HAL_TEMPLATE_IMP_MASK15
#undef __VSF_HAL_TEMPLATE_IMP_MASK16
#undef __VSF_HAL_TEMPLATE_IMP_MASK17
#undef __VSF_HAL_TEMPLATE_IMP_MASK18
#undef __VSF_HAL_TEMPLATE_IMP_MASK19
#undef __VSF_HAL_TEMPLATE_IMP_MASK20
#undef __VSF_HAL_TEMPLATE_IMP_MASK21
#undef __VSF_HAL_TEMPLATE_IMP_MASK22
#undef __VSF_HAL_TEMPLATE_IMP_MASK23
#undef __VSF_HAL_TEMPLATE_IMP_MASK24
#undef __VSF_HAL_TEMPLATE_IMP_MASK25
#undef __VSF_HAL_TEMPLATE_IMP_MASK26
#undef __VSF_HAL_TEMPLATE_IMP_MASK27
#undef __VSF_HAL_TEMPLATE_IMP_MASK28
#undef __VSF_HAL_TEMPLATE_IMP_MASK29
#undef __VSF_HAL_TEMPLATE_IMP_MASK30
#undef __VSF_HAL_TEMPLATE_IMP_MASK31
#undef __VSF_HAL_TEMPLATE_IMP_LV0
#undef __VSF_HAL_OP_VAR
#undef __VSF_HAL_OP
