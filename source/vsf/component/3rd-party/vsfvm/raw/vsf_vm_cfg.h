/*****************************************************************************
 *   Copyright(C)2009-2020 by SimonQian                                      *
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



#ifndef __VSF_VM_CFG_H__
#define __VSF_VM_CFG_H__

/*============================ MACROS ========================================*/
/*============================ INCLUDES ======================================*/
#include "component/vsf_component_cfg.h"

/*============================ MACROS ========================================*/

#ifndef VSFVM_ASSERT
#   define VSFVM_ASSERT                     VSF_COMPONENT_ASSERT
#endif

#ifndef VSFVM_CFG_RUNTIME_EN
#   define VSFVM_CFG_RUNTIME_EN             ENABLED
#endif

#ifndef VSFVM_CFG_COMPILER_EN
#   define VSFVM_CFG_COMPILER_EN            ENABLED
#endif

#ifndef VSFVM_CFG_PRIORITY
#   define VSFVM_CFG_PRIORITY               vsf_prio_0
#endif

#ifndef VSFVM_LEXER_DEBUG_EN
#   define VSFVM_LEXER_DEBUG_EN             DISABLED
#endif

#ifndef VSFVM_PARSER_DEBUG_EN
#   define VSFVM_PARSER_DEBUG_EN            DISABLED
#endif

#ifndef VSFVM_COMPILER_DEBUG_EN
#   define VSFVM_COMPILER_DEBUG_EN          DISABLED
#endif

#ifndef VSFVM_RUNTIME_DEBUG_EN
#   define VSFVM_RUNTIME_DEBUG_EN           DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */