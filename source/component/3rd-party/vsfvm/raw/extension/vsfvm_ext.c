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

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)

#include "../common/vsfvm_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

vsf_slist_t vsfvm_ext_list;

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int vsfvm_register_ext(vsfvm_ext_t *ext)
{
    vsf_slist_init_node(vsfvm_ext_t, ext_node, ext);
    vsf_slist_append(vsfvm_ext_t, ext_node, &vsfvm_ext_list, ext);
    return 0;
}

#endif      // VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN
