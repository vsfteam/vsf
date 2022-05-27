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
//  #define __VSF_HAL_TEMPLATE_PREFIX            VSF_ADC_CFG_PREFIX
//  #define __VSF_HAL_TEMPLATE_UPCASE_PREFIX     VSF_ADC_CFG_UPPERCASE_PREFIX
#define __VSF_HAL_TEMPLATE_PREFIX                VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_PREFIX)
#define __VSF_HAL_TEMPLATE_UPCASE_PREFIX         VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_UPPERCASE_PREFIX)

// #if VSF_ADC_CFG_IMPLEMENT_OP == ENABLED
#if VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMPLEMENT_OP) == ENABLED
#   define __VSF_HAL_OP_VAR     VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _op)
#   define __VSF_HAL_OP         .VSF_MCONNECT(vsf, __VSF_HAL_TEMPLATE_NAME).op = & __VSF_HAL_OP_VAR,

#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_OP

static const VSF_MCONNECT(vsf, __VSF_HAL_TEMPLATE_NAME, _op_t) __VSF_HAL_OP_VAR = {
    VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _APIS) (__VSF_HAL_TEMPLATE_PREFIX)
};

#else
#   define __VSF_HAL_OP
#endif

// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_COUNT VSF_HW_ADC_COUNT
#if VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#    define __VSF_HAL_TEMPLATE_DEC_COUNT  VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#endif

#if VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_COUNT     VSF_HW_ADC_MASK
#   define __VSF_HAL_TEMPLATE_IMP_MASK   VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
#else
#   define __VSF_HAL_TEMPLATE_IMP_MASK   ((1ul << __VSF_HAL_TEMPLATE_DEC_COUNT) - 1)
#endif

#   define __VSF_HAL_TEMPLATE_IMP_LV0   VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_IMP_LV0)

#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 0)
        __VSF_HAL_TEMPLATE_IMP_LV0(0, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 1)
        __VSF_HAL_TEMPLATE_IMP_LV0(1, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 2)
        __VSF_HAL_TEMPLATE_IMP_LV0(2, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 3)
        __VSF_HAL_TEMPLATE_IMP_LV0(3, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 4)
        __VSF_HAL_TEMPLATE_IMP_LV0(4, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 5)
        __VSF_HAL_TEMPLATE_IMP_LV0(5, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 6)
        __VSF_HAL_TEMPLATE_IMP_LV0(6, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 7)
        __VSF_HAL_TEMPLATE_IMP_LV0(7, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 8)
        __VSF_HAL_TEMPLATE_IMP_LV0(8, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 9)
        __VSF_HAL_TEMPLATE_IMP_LV0(9, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 10)
        __VSF_HAL_TEMPLATE_IMP_LV0(10, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 11)
        __VSF_HAL_TEMPLATE_IMP_LV0(11, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 12)
        __VSF_HAL_TEMPLATE_IMP_LV0(12, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 13)
        __VSF_HAL_TEMPLATE_IMP_LV0(13, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 14)
        __VSF_HAL_TEMPLATE_IMP_LV0(14, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 15)
        __VSF_HAL_TEMPLATE_IMP_LV0(15, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 16)
        __VSF_HAL_TEMPLATE_IMP_LV0(16, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 17)
        __VSF_HAL_TEMPLATE_IMP_LV0(17, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 18)
        __VSF_HAL_TEMPLATE_IMP_LV0(18, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 19)
        __VSF_HAL_TEMPLATE_IMP_LV0(19, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 20)
        __VSF_HAL_TEMPLATE_IMP_LV0(20, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 21)
        __VSF_HAL_TEMPLATE_IMP_LV0(21, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 22)
        __VSF_HAL_TEMPLATE_IMP_LV0(22, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 23)
        __VSF_HAL_TEMPLATE_IMP_LV0(23, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 24)
        __VSF_HAL_TEMPLATE_IMP_LV0(24, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 25)
        __VSF_HAL_TEMPLATE_IMP_LV0(25, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 26)
        __VSF_HAL_TEMPLATE_IMP_LV0(26, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 27)
        __VSF_HAL_TEMPLATE_IMP_LV0(27, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 28)
        __VSF_HAL_TEMPLATE_IMP_LV0(28, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 29)
        __VSF_HAL_TEMPLATE_IMP_LV0(29, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 30)
        __VSF_HAL_TEMPLATE_IMP_LV0(30, __VSF_HAL_OP)
#   endif
#   if __VSF_HAL_TEMPLATE_IMP_MASK & (1 << 31)
        __VSF_HAL_TEMPLATE_IMP_LV0(31, __VSF_HAL_OP)
#   endif

#undef __VSF_HAL_TEMPLATE_PREFIX
#undef __VSF_HAL_TEMPLATE_UPCASE_PREFIX
#undef __VSF_HAL_TEMPLATE_NAME
#undef __VSF_HAL_TEMPLATE_UPCASE_NAME
#undef __VSF_HAL_TEMPLATE_APIS
#undef __VSF_HAL_TEMPLATE_DEC_COUNT
#undef __VSF_HAL_TEMPLATE_IMP_MASK
#undef __VSF_HAL_TEMPLATE_IMP_LV0
#undef __VSF_HAL_OP_VAR
#undef __VSF_HAL_OP
