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

/*============================ INCLUDES ======================================*/

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED && VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_CTYPE == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/simple_libc/ctype.h"
#else
#   include <ctype.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

const char __vsf_linux_ctype[1 + 256] = {
    0,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_S|CTYPE_B,CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,
    CTYPE_N,        CTYPE_N,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_C,
};

#endif      // VSF_USE_LINUX && VSF_LINUX_USE_SIMPLE_LIBC && VSF_LINUX_USE_SIMPLE_CTYPE
