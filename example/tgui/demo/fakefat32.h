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

#ifndef __FAKEFAT32_H__
#define __FAKEFAT32_H__

/*============================ INCLUDES ======================================*/

#include "vsf_usr_cfg.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED

/*============================ MACROS ========================================*/

#ifndef APP_USE_VSFVM_DEMO
#   define APP_USE_VSFVM_DEMO                   DISABLED
#endif
#ifndef USRAPP_FAKEFAT32_CFG_FONT
#   define USRAPP_FAKEFAT32_CFG_FONT            DISABLED
#endif
#ifndef APP_USE_EVM_DEMO
#   define APP_USE_EVM_DEMO                     DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vk_fakefat32_file_t fakefat32_root[ 3
                                +   (APP_USE_VSFVM_DEMO == ENABLED ? 1 : 0)
                                +   (USRAPP_FAKEFAT32_CFG_FONT == ENABLED ? 1 : 0)
                                +   (APP_USE_EVM_DEMO == ENABLED ? 1 : 0)
                                ];

#endif
#endif      // __FAKEFAT32_H__

/* EOF */
