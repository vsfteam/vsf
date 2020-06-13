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

#ifndef __VSF_VM_H__
#define __VSF_VM_H__

/*============================ INCLUDES ======================================*/
#include "./vsf_vm_cfg.h"

#if VSFVM_CFG_RUNTIME_EN == ENABLED
#   include "./runtime/vsfvm_runtime.h"
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
#   include "./compiler/vsfvm_compiler.h"
#   include "./compiler/lexer/dart/vsfvm_lexer_dart.h"
#   include "./compiler/vsfvm_snapshot.h"
#endif

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)
#   include "./extension/std/vsfvm_ext_std.h"
#   include "./common/vsfvm_objdump.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */