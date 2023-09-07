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

#ifndef __VSF_PYTHON_H__
#define __VSF_PYTHON_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if (VSF_USE_STDPYTHON == ENABLED) || (VSF_USE_PIKAPYTHON == ENABLED) || (VSF_USE_MICROPYTHON == ENABLED)
#   define VSF_USE_PYTHON       ENABLED
#endif

#if     VSF_USE_STDPYTHON == ENABLED
#   include "./al/stdpython/vsf_stdpython.h"
#elif   VSF_USE_PIKAPYTHON == ENABLED
#   include "./al/pikapython/vsf_pikapython.h"
#elif   VSF_USE_MICROPYTHON == ENABLED
#   include "./al/micropython/vsf_micropython.h"
#else
#   error please enable one of VSF_USE_PIKAPYTHON or VSF_USE_MICROPYTHON
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
