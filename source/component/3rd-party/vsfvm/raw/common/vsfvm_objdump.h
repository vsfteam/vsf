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

#ifndef __VSFVM_OBJDUMP_H__
#define __VSFVM_OBJDUMP_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsfvm_tkdump(vsfvm_bytecode_t token);
extern void vsfvm_objdump(vsfvm_bytecode_t *buff, uint_fast32_t len);

#endif      // VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN
#endif      // __VSFVM_OBJDUMP_H__
