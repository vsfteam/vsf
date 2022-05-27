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

// for example:
//  #define __VSF_HAL_TEMPLATE_PREFIX            vsf_hw
//  #define __VSF_HAL_TEMPLATE_UPCASE_PREFIX     VSF_HW
//  #define __VSF_HAL_TEMPLATE_NAME              adc
//  #define __VSF_HAL_TEMPLATE_UPCASE_NAME       ADC

#define __VSF_HAL_TEMPLATE_PREFIX                VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_API_DECLARATION_PREFIX)

#if defined(__VSF_HAL_TEMPLATE_PREFIX)
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API  VSF_HAL_TEMPLATE_API_EXTERN

// expand to:
//  typedef vsf_xxx_adc_t vsf_xxx_adc_t;
typedef struct VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)         \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t);

// expand to:
//  extern vsf_err_t vsf_hw_adc_init(vsf_hw_adc_t *adc_ptr, adc_cfg_t *cfg_ptr);
//  ...
VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _APIS) (__VSF_HAL_TEMPLATE_PREFIX)
#endif

#if defined(__VSF_HAL_TEMPLATE_PREFIX) && defined(__VSF_HAL_TEMPLATE_UPCASE_PREFIX)
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_COUNT VSF_HW_ADC_COUNT
#   if VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#       define __VSF_HAL_TEMPLATE_DEC_COUNT  VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#   endif

#   if VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_COUNT     VSF_HW_ADC_MASK
#       define __VSF_HAL_TEMPLATE_DEC_MASK   VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
#   else
#       define __VSF_HAL_TEMPLATE_DEC_MASK   ((1ul << __VSF_HAL_TEMPLATE_DEC_COUNT) - 1)
#   endif

// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_LV0  vsf_hw_adc_t vsf_hw_adc_0;
#   define __VSF_HAL_TEMPLATE_DEC_LV0(__COUNT, __DONT_CARE)                                 \
        extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)      \
            VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __COUNT);

#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 0)
        __VSF_HAL_TEMPLATE_DEC_LV0(0, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 1)
        __VSF_HAL_TEMPLATE_DEC_LV0(1, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 2)
        __VSF_HAL_TEMPLATE_DEC_LV0(2, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 3)
        __VSF_HAL_TEMPLATE_DEC_LV0(3, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 4)
        __VSF_HAL_TEMPLATE_DEC_LV0(4, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 5)
        __VSF_HAL_TEMPLATE_DEC_LV0(5, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 6)
        __VSF_HAL_TEMPLATE_DEC_LV0(6, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 7)
        __VSF_HAL_TEMPLATE_DEC_LV0(7, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 8)
        __VSF_HAL_TEMPLATE_DEC_LV0(8, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 9)
        __VSF_HAL_TEMPLATE_DEC_LV0(9, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 10)
        __VSF_HAL_TEMPLATE_DEC_LV0(10, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 11)
        __VSF_HAL_TEMPLATE_DEC_LV0(11, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 12)
        __VSF_HAL_TEMPLATE_DEC_LV0(12, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 13)
        __VSF_HAL_TEMPLATE_DEC_LV0(13, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 14)
        __VSF_HAL_TEMPLATE_DEC_LV0(14, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 15)
        __VSF_HAL_TEMPLATE_DEC_LV0(15, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 16)
        __VSF_HAL_TEMPLATE_DEC_LV0(16, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 17)
        __VSF_HAL_TEMPLATE_DEC_LV0(17, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 18)
        __VSF_HAL_TEMPLATE_DEC_LV0(18, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 19)
        __VSF_HAL_TEMPLATE_DEC_LV0(19, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 20)
        __VSF_HAL_TEMPLATE_DEC_LV0(20, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 21)
        __VSF_HAL_TEMPLATE_DEC_LV0(21, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 22)
        __VSF_HAL_TEMPLATE_DEC_LV0(22, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 23)
        __VSF_HAL_TEMPLATE_DEC_LV0(23, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 24)
        __VSF_HAL_TEMPLATE_DEC_LV0(24, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 25)
        __VSF_HAL_TEMPLATE_DEC_LV0(25, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 26)
        __VSF_HAL_TEMPLATE_DEC_LV0(26, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 27)
        __VSF_HAL_TEMPLATE_DEC_LV0(27, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 28)
        __VSF_HAL_TEMPLATE_DEC_LV0(28, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 29)
        __VSF_HAL_TEMPLATE_DEC_LV0(29, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 30)
        __VSF_HAL_TEMPLATE_DEC_LV0(30, NULL)
#   endif
#   if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 31)
        __VSF_HAL_TEMPLATE_DEC_LV0(31, NULL)
#   endif
#endif

#undef __VSF_HAL_TEMPLATE_PREFIX
#undef __VSF_HAL_TEMPLATE_UPCASE_PREFIX
#undef __VSF_HAL_TEMPLATE_NAME
#undef __VSF_HAL_TEMPLATE_UPCASE_NAME
#undef __VSF_HAL_TEMPLATE_APIS
#undef __VSF_HAL_TEMPLATE_DEC_COUNT
#undef __VSF_HAL_TEMPLATE_DEC_MASK
#undef __VSF_HAL_TEMPLATE_DEC_LV0
