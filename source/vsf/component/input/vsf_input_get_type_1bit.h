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
/*============================ MACROS ========================================*/

#ifndef VSF_INPUT_USER_TYPE_maker
#   define VSF_INPUT_USER_TYPE_make(__bit7, __bit6, __bit5, __bit4, __bit3, __bit2, __bit1, __bit0)\
            __CONNECT9(0b, __bit7, __bit6, __bit5, __bit4, __bit3, __bit2, __bit1, __bit0)
#endif

#ifndef VSF_INPUT_USER_TYPE
#   define VSF_INPUT_USER_TYPE_BIT0     1
#   define VSF_INPUT_USER_TYPE_BIT1     0
#   define VSF_INPUT_USER_TYPE_BIT2     0
#   define VSF_INPUT_USER_TYPE_BIT3     0
#   define VSF_INPUT_USER_TYPE_BIT4     0
#   define VSF_INPUT_USER_TYPE_BIT5     0
#   define VSF_INPUT_USER_TYPE_BIT6     0
#   define VSF_INPUT_USER_TYPE_BIT7     0
#else

#   if VSF_INPUT_USER_TYPE_BIT0 == 0
#       undef VSF_INPUT_USER_TYPE_BIT0
#       define VSF_INPUT_USER_TYPE_BIT0 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT0
#       define VSF_INPUT_USER_TYPE_BIT0 0

#   if VSF_INPUT_USER_TYPE_BIT1 == 0
#       undef VSF_INPUT_USER_TYPE_BIT1
#       define VSF_INPUT_USER_TYPE_BIT1 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT1
#       define VSF_INPUT_USER_TYPE_BIT1 0

#   if VSF_INPUT_USER_TYPE_BIT2 == 0
#       undef VSF_INPUT_USER_TYPE_BIT2
#       define VSF_INPUT_USER_TYPE_BIT2 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT2
#       define VSF_INPUT_USER_TYPE_BIT2 0

#   if VSF_INPUT_USER_TYPE_BIT3 == 0
#       undef VSF_INPUT_USER_TYPE_BIT3
#       define VSF_INPUT_USER_TYPE_BIT3 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT3
#       define VSF_INPUT_USER_TYPE_BIT3 0

#   if VSF_INPUT_USER_TYPE_BIT4 == 0
#       undef VSF_INPUT_USER_TYPE_BIT4
#       define VSF_INPUT_USER_TYPE_BIT4 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT4
#       define VSF_INPUT_USER_TYPE_BIT4 0

#   if VSF_INPUT_USER_TYPE_BIT5 == 0
#       undef VSF_INPUT_USER_TYPE_BIT5
#       define VSF_INPUT_USER_TYPE_BIT5 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT5
#       define VSF_INPUT_USER_TYPE_BIT5 0

#   if VSF_INPUT_USER_TYPE_BIT6 == 0
#       undef VSF_INPUT_USER_TYPE_BIT6
#       define VSF_INPUT_USER_TYPE_BIT6 1
#   else
#       undef VSF_INPUT_USER_TYPE_BIT6
#       define VSF_INPUT_USER_TYPE_BIT6 0

#   if VSF_INPUT_USER_TYPE_BIT7 == 0
#       undef VSF_INPUT_USER_TYPE_BIT7
#       define VSF_INPUT_USER_TYPE_BIT7 1
#   else
#       error "bitlen too long!!"

#   endif
#   endif
#   endif
#   endif
#   endif
#   endif
#   endif
#   endif
#endif

#undef VSF_INPUT_USER_TYPE
#define VSF_INPUT_USER_TYPE         VSF_INPUT_USER_TYPE_make(                   \
                VSF_INPUT_USER_TYPE_BIT7, VSF_INPUT_USER_TYPE_BIT6,             \
                VSF_INPUT_USER_TYPE_BIT5, VSF_INPUT_USER_TYPE_BIT4,             \
                VSF_INPUT_USER_TYPE_BIT3, VSF_INPUT_USER_TYPE_BIT2,             \
                VSF_INPUT_USER_TYPE_BIT1, VSF_INPUT_USER_TYPE_BIT0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
