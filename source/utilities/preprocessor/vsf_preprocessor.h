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

#ifndef __VSF_PREPROCESSOR_H__
#define __VSF_PREPROCESSOR_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_utilities_cfg.h"

#include "./vsf_connect_macro.h"
#include "./vsf_foreach_macro.h"
#include "./vsf_repeat_macro.h"

/*============================ MACROS ========================================*/

// generate safe variable name used in macros
#define VSF_MACRO_SAFE_NAME(__NAME)     VSF_MCONNECT3(__, __LINE__, __NAME)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_PREPROCESSOR_H__
/* EOF */