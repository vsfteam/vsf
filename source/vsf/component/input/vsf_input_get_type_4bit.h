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
#   define VSF_INPUT_USER_TYPE_make(__x, __y)       __CONNECT3(0x, __x, __y)
#endif

#ifndef VSF_INPUT_USER_TYPE
#   define VSF_INPUT_USER_TYPE_L4       1
#   define VSF_INPUT_USER_TYPE_H4       0
#else

#   if VSF_INPUT_USER_TYPE_L4 == 0
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   1
#   elif VSF_INPUT_USER_TYPE_L4 == 1
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   2
#   elif VSF_INPUT_USER_TYPE_L4 == 2
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   3
#   elif VSF_INPUT_USER_TYPE_L4 == 3
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   4
#   elif VSF_INPUT_USER_TYPE_L4 == 4
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   5
#   elif VSF_INPUT_USER_TYPE_L4 == 5
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   6
#   elif VSF_INPUT_USER_TYPE_L4 == 6
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   7
#   elif VSF_INPUT_USER_TYPE_L4 == 7
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   8
#   elif VSF_INPUT_USER_TYPE_L4 == 8
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   9
#   elif VSF_INPUT_USER_TYPE_L4 == 9
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   A
#   elif VSF_INPUT_USER_TYPE_L4 == A
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   B
#   elif VSF_INPUT_USER_TYPE_L4 == B
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   C
#   elif VSF_INPUT_USER_TYPE_L4 == C
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   D
#   elif VSF_INPUT_USER_TYPE_L4 == D
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   E
#   elif VSF_INPUT_USER_TYPE_L4 == E
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   F
#   elif VSF_INPUT_USER_TYPE_L4 == F
#       undef VSF_INPUT_USER_TYPE_L4
#       define VSF_INPUT_USER_TYPE_L4   0

#   if VSF_INPUT_USER_TYPE_H4 == 0
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   1
#   elif VSF_INPUT_USER_TYPE_H4 == 1
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   2
#   elif VSF_INPUT_USER_TYPE_H4 == 2
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   3
#   elif VSF_INPUT_USER_TYPE_H4 == 3
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   4
#   elif VSF_INPUT_USER_TYPE_H4 == 4
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   5
#   elif VSF_INPUT_USER_TYPE_H4 == 5
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   6
#   elif VSF_INPUT_USER_TYPE_H4 == 6
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   7
#   elif VSF_INPUT_USER_TYPE_H4 == 7
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   8
#   elif VSF_INPUT_USER_TYPE_H4 == 8
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   9
#   elif VSF_INPUT_USER_TYPE_H4 == 9
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   A
#   elif VSF_INPUT_USER_TYPE_H4 == A
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   B
#   elif VSF_INPUT_USER_TYPE_H4 == B
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   C
#   elif VSF_INPUT_USER_TYPE_H4 == C
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   D
#   elif VSF_INPUT_USER_TYPE_H4 == D
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   E
#   elif VSF_INPUT_USER_TYPE_H4 == E
#       undef VSF_INPUT_USER_TYPE_H4
#       define VSF_INPUT_USER_TYPE_H4   F
#   elif VSF_INPUT_USER_TYPE_H4 == F
#       error "only support 8-bit counter"
#   endif
#   endif
#endif

#undef VSF_INPUT_USER_TYPE
#define VSF_INPUT_USER_TYPE         VSF_INPUT_USER_TYPE_make(VSF_INPUT_USER_TYPE_H4, VSF_INPUT_USER_TYPE_L4)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
