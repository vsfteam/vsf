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

#ifndef __VSF_SHELL_WRAPPER_H__
#define __VSF_SHELL_WRAPPER_H__

/*============================ INCLUDES ======================================*/

// TODO: is it secure to include compiler info here?
#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef COMPILER_WRAPPER
#   define VSF_SHELL_WRAPPER(__header, __api)       COMPILER_WRAPPER(__api)
#else
#   define __VSF_SHELL_API_NAME(__header, __api)    __ ## header ## _ ## __api
#   define VSF_SHELL_WRAPPER(__header, __api)       __VSF_SHELL_API_NAME(__header, __api)
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_SHELL_WRAPPER_H__
/* EOF */
