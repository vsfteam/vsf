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
#include "tpaste.h"
/*============================ MACROS ========================================*/

#ifndef MFUNC_IN_U8_DEC_VALUE
#   error Please define the MFUNC_IN_U8_DEC_VALUE as the input value. It must be \
a compile-time constant value.
#endif

#undef __MFUNC_OUT_DEC_DIGIT_TEMP0
#undef __MFUNC_OUT_DEC_DIGIT_TEMP1
#undef __MFUNC_OUT_DEC_DIGIT_TEMP2
#undef __MFUNC_OUT_DEC_STR_TEMP
#undef MFUNC_OUT_DEC_STR


#if     (MFUNC_IN_U8_DEC_VALUE % 10) == 0
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       0
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 1
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       1
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 2
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       2
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 3
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       3
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 4
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       4
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 5
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       5
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 6
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       6
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 7
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       7
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 8
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       8
#elif   (MFUNC_IN_U8_DEC_VALUE % 10) == 9
#   define __MFUNC_OUT_DEC_DIGIT_TEMP0       9
#endif


#if     ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 0
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       0
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 1
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       1
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 2
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       2
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 3
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       3
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 4
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       4
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 5
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       5
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 6
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       6
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 7
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       7
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 8
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       8
#elif   ((MFUNC_IN_U8_DEC_VALUE/10) % 10) == 9
#   define __MFUNC_OUT_DEC_DIGIT_TEMP1       9
#endif

#if     ((MFUNC_IN_U8_DEC_VALUE/100) % 10) == 0
#   define __MFUNC_OUT_DEC_DIGIT_TEMP2       0
#elif   ((MFUNC_IN_U8_DEC_VALUE/100) % 10) == 1
#   define __MFUNC_OUT_DEC_DIGIT_TEMP2       1
#elif   ((MFUNC_IN_U8_DEC_VALUE/100) % 10) == 2
#   define __MFUNC_OUT_DEC_DIGIT_TEMP2       2
#endif


#if __MFUNC_OUT_DEC_DIGIT_TEMP2 == 0 
#   if __MFUNC_OUT_DEC_DIGIT_TEMP1 == 0
#       define MFUNC_OUT_DEC_STR        __MFUNC_OUT_DEC_DIGIT_TEMP0
#   else
#       define MFUNC_OUT_DEC_STR        ATPASTE2(   __MFUNC_OUT_DEC_DIGIT_TEMP1,\
                                                    __MFUNC_OUT_DEC_DIGIT_TEMP0)
#   endif
#else
#   define MFUNC_OUT_DEC_STR            ATPASTE3(   __MFUNC_OUT_DEC_DIGIT_TEMP2,\
                                                    __MFUNC_OUT_DEC_DIGIT_TEMP1,\
                                                    __MFUNC_OUT_DEC_DIGIT_TEMP0)
#endif


#undef MFUNC_IN_U8_DEC_VALUE


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
