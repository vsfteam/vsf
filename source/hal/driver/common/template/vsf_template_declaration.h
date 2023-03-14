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
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or DEClied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

// for example:
//  #define __VSF_HAL_TEMPLATE_PREFIX           vsf_hw
//  #define __VSF_HAL_TEMPLATE_UPCASE_PREFIX    VSF_HW
//  #define __VSF_HAL_TEMPLATE_NAME             adc
//  #define __VSF_HAL_TEMPLATE_UPCASE_NAME      ADC

#define __VSF_HAL_TEMPLATE_PREFIX               VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_DEC_PREFIX)
#define __VSF_HAL_TEMPLATE_EXTERN_OP            VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _CFG_DEC_EXTERN_OP)


#if defined(__VSF_HAL_TEMPLATE_PREFIX)
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API               VSF_HAL_TEMPLATE_API_EXTERN

// expand to:
//  typedef vsf_xxx_adc_t vsf_xxx_adc_t;
typedef struct VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)         \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t);

// expand to:
//  extern vsf_err_t vsf_hw_adc_init(vsf_hw_adc_t *adc_ptr, vsf_adc_cfg_t *cfg_ptr);
//  ...
VSF_MCONNECT(VSF, __VSF_HAL_TEMPLATE_UPCASE_NAME, _APIS) (__VSF_HAL_TEMPLATE_PREFIX)

#if __VSF_HAL_TEMPLATE_EXTERN_OP == ENABLED
extern const VSF_MCONNECT(vsf, __VSF_HAL_TEMPLATE_NAME, _op_t)
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _op);
#endif
#endif

#if defined(__VSF_HAL_TEMPLATE_PREFIX) && defined(__VSF_HAL_TEMPLATE_UPCASE_PREFIX)

// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_COUNT VSF_HW_ADC_COUNT
#   if !defined(__VSF_HAL_TEMPLATE_DEC_COUNT) && VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#       define __VSF_HAL_TEMPLATE_DEC_COUNT  VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _COUNT)
#   endif

#   if !defined(__VSF_HAL_TEMPLATE_DEC_MASK) &&VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_MASK     VSF_HW_ADC_MASK
#       define __VSF_HAL_TEMPLATE_DEC_MASK   VSF_MCONNECT(__VSF_HAL_TEMPLATE_UPCASE_PREFIX, __VSF_HAL_TEMPLATE_UPCASE_NAME, _MASK)
#   endif

#   ifndef __VSF_HAL_TEMPLATE_DEC_COUNT
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 0)
#           define __VSF_HAL_TEMPLATE_DEC_MASK0 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK0 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 1)
#           define __VSF_HAL_TEMPLATE_DEC_MASK1 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK1 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 2)
#           define __VSF_HAL_TEMPLATE_DEC_MASK2 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK2 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 3)
#           define __VSF_HAL_TEMPLATE_DEC_MASK3 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK3 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 4)
#           define __VSF_HAL_TEMPLATE_DEC_MASK4 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK4 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 5)
#           define __VSF_HAL_TEMPLATE_DEC_MASK5 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK5 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 6)
#           define __VSF_HAL_TEMPLATE_DEC_MASK6 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK6 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 7)
#           define __VSF_HAL_TEMPLATE_DEC_MASK7 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK7 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 8)
#           define __VSF_HAL_TEMPLATE_DEC_MASK8 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK8 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 9)
#           define __VSF_HAL_TEMPLATE_DEC_MASK9 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK9 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 10)
#           define __VSF_HAL_TEMPLATE_DEC_MASK10 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK10 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 11)
#           define __VSF_HAL_TEMPLATE_DEC_MASK11 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK11 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 12)
#           define __VSF_HAL_TEMPLATE_DEC_MASK12 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK12 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 13)
#           define __VSF_HAL_TEMPLATE_DEC_MASK13 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK13 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 14)
#           define __VSF_HAL_TEMPLATE_DEC_MASK14 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK14 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 15)
#           define __VSF_HAL_TEMPLATE_DEC_MASK15 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK15 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 16)
#           define __VSF_HAL_TEMPLATE_DEC_MASK16 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK16 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 17)
#           define __VSF_HAL_TEMPLATE_DEC_MASK17 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK17 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 18)
#           define __VSF_HAL_TEMPLATE_DEC_MASK18 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK18 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 19)
#           define __VSF_HAL_TEMPLATE_DEC_MASK19 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK19 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 20)
#           define __VSF_HAL_TEMPLATE_DEC_MASK20 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK20 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 21)
#           define __VSF_HAL_TEMPLATE_DEC_MASK21 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK21 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 22)
#           define __VSF_HAL_TEMPLATE_DEC_MASK22 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK22 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 23)
#           define __VSF_HAL_TEMPLATE_DEC_MASK23 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK23 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 24)
#           define __VSF_HAL_TEMPLATE_DEC_MASK24 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK24 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 25)
#           define __VSF_HAL_TEMPLATE_DEC_MASK25 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK25 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 26)
#           define __VSF_HAL_TEMPLATE_DEC_MASK26 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK26 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 27)
#           define __VSF_HAL_TEMPLATE_DEC_MASK27 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK27 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 28)
#           define __VSF_HAL_TEMPLATE_DEC_MASK28 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK28 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 29)
#           define __VSF_HAL_TEMPLATE_DEC_MASK29 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK29 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 30)
#           define __VSF_HAL_TEMPLATE_DEC_MASK30 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK30 0
#       endif
#       if __VSF_HAL_TEMPLATE_DEC_MASK & (1 << 31)
#           define __VSF_HAL_TEMPLATE_DEC_MASK31 1
#       else
#           define __VSF_HAL_TEMPLATE_DEC_MASK31 0
#       endif
#       define __VSF_HAL_TEMPLATE_DEC_COUNT (                                   \
            __VSF_HAL_TEMPLATE_DEC_MASK0 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK1 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK2 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK3 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK4 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK5 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK6 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK7 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK8 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK9 +                                      \
            __VSF_HAL_TEMPLATE_DEC_MASK10 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK11 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK12 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK13 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK14 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK15 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK16 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK17 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK18 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK19 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK20 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK21 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK22 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK23 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK24 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK25 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK26 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK27 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK28 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK29 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK30 +                                     \
            __VSF_HAL_TEMPLATE_DEC_MASK31                                       \
    )
#   endif

#   ifndef __VSF_HAL_TEMPLATE_DEC_MASK
#       define __VSF_HAL_TEMPLATE_DEC_MASK   ((1ull << __VSF_HAL_TEMPLATE_DEC_COUNT) - 1)
#   endif


#   ifndef __VSF_HAL_TEMPLATE_DEC_LV0
#       ifndef __VSF_HAL_TEMPLATE_TYPE_PREFIX
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_LV0  vsf_hw_usart_t vsf_hw_usart_0;
#           define __VSF_HAL_TEMPLATE_DEC_LV0(__COUNT, __DONT_CARE)                                 \
                extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)         \
                    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __COUNT);
#else
// expand to:
//  #define __VSF_HAL_TEMPLATE_DEC_LV0  vsf_hw_usart_t __vsf_hw_usart_0; vsf_fifo2req_usart_t vsf_hw_usart_0;
#       define __VSF_HAL_TEMPLATE_DEC_LV0(__COUNT, __DONT_CARE)                                     \
            extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)             \
                VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __COUNT);      \
            extern VSF_MCONNECT(vsf_, __VSF_HAL_TEMPLATE_TYPE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t)  \
                VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, __COUNT);
#       endif
#   endif

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

#if !defined(__VSF_HAL_TEMPLATE_CFG_REMOVE_ARRAY) && (__VSF_HAL_TEMPLATE_DEC_MASK != 0)
#   ifndef __VSF_HAL_TEMPLATE_TYPE_PREFIX
extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const                        \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_DEC_COUNT];
#   else
extern VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const                        \
    VSF_MCONNECT(__, __VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_DEC_COUNT];
extern VSF_MCONNECT(vsf_, __VSF_HAL_TEMPLATE_TYPE_PREFIX, __VSF_HAL_TEMPLATE_NAME, _t) * const             \
    VSF_MCONNECT(__VSF_HAL_TEMPLATE_PREFIX, __VSF_HAL_TEMPLATE_NAME, s)[__VSF_HAL_TEMPLATE_DEC_COUNT];
#endif
#endif

#undef __VSF_HAL_TEMPLATE_PREFIX
#undef __VSF_HAL_TEMPLATE_TYPE_PREFIX
#undef __VSF_HAL_TEMPLATE_UPCASE_PREFIX
#undef __VSF_HAL_TEMPLATE_EXTERN_OP
#undef __VSF_HAL_TEMPLATE_NAME
#undef __VSF_HAL_TEMPLATE_UPCASE_NAME
#undef __VSF_HAL_TEMPLATE_APIS
#undef __VSF_HAL_TEMPLATE_DEC_COUNT
#undef __VSF_HAL_TEMPLATE_DEC_MASK
#undef __VSF_HAL_TEMPLATE_DEC_MASK0
#undef __VSF_HAL_TEMPLATE_DEC_MASK1
#undef __VSF_HAL_TEMPLATE_DEC_MASK2
#undef __VSF_HAL_TEMPLATE_DEC_MASK3
#undef __VSF_HAL_TEMPLATE_DEC_MASK4
#undef __VSF_HAL_TEMPLATE_DEC_MASK5
#undef __VSF_HAL_TEMPLATE_DEC_MASK6
#undef __VSF_HAL_TEMPLATE_DEC_MASK7
#undef __VSF_HAL_TEMPLATE_DEC_MASK8
#undef __VSF_HAL_TEMPLATE_DEC_MASK9
#undef __VSF_HAL_TEMPLATE_DEC_MASK10
#undef __VSF_HAL_TEMPLATE_DEC_MASK11
#undef __VSF_HAL_TEMPLATE_DEC_MASK12
#undef __VSF_HAL_TEMPLATE_DEC_MASK13
#undef __VSF_HAL_TEMPLATE_DEC_MASK14
#undef __VSF_HAL_TEMPLATE_DEC_MASK15
#undef __VSF_HAL_TEMPLATE_DEC_MASK16
#undef __VSF_HAL_TEMPLATE_DEC_MASK17
#undef __VSF_HAL_TEMPLATE_DEC_MASK18
#undef __VSF_HAL_TEMPLATE_DEC_MASK19
#undef __VSF_HAL_TEMPLATE_DEC_MASK20
#undef __VSF_HAL_TEMPLATE_DEC_MASK21
#undef __VSF_HAL_TEMPLATE_DEC_MASK22
#undef __VSF_HAL_TEMPLATE_DEC_MASK23
#undef __VSF_HAL_TEMPLATE_DEC_MASK24
#undef __VSF_HAL_TEMPLATE_DEC_MASK25
#undef __VSF_HAL_TEMPLATE_DEC_MASK26
#undef __VSF_HAL_TEMPLATE_DEC_MASK27
#undef __VSF_HAL_TEMPLATE_DEC_MASK28
#undef __VSF_HAL_TEMPLATE_DEC_MASK29
#undef __VSF_HAL_TEMPLATE_DEC_MASK30
#undef __VSF_HAL_TEMPLATE_DEC_MASK31
#undef __VSF_HAL_TEMPLATE_DEC_LV0
