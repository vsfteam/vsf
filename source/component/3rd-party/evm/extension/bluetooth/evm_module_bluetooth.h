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

#ifndef __VSF_EVM_MODULE_BLUETOOTH_H__
#define __VSF_EVM_MODULE_BLUETOOTH_H__

/*============================ INCLUDES ======================================*/

#include "vsf_cfg.h"

#if VSF_EVM_USE_BLUETOOTH == ENABLED && VSF_USE_EVM == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern evm_val_t evm_class_bluetooth(evm_t * e);

#ifdef __cplusplus
}
#endif

#endif      // VSF_EVM_USE_BLUETOOTH
#endif      // __VSF_EVM_MODULE_BLUETOOTH_H__
