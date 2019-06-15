/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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


//! \note Top Level Configuration 

#ifndef __VSF_CFG_H__
#define __VSF_CFG_H__

/*============================ MACROS ========================================*/
#ifndef ENABLED
#   define ENABLED                              1
#endif

#ifndef DISABLED
#   define DISABLED                             0
#endif

/*============================ INCLUDES ======================================*/
/* do not modify this */
#include "vsf_usr_cfg.h"


/* compiler abstraction, supports GCC, IAR, Arm Compiler 5, Arm Compiler 6 */
#include "utilities/compiler.h"

/* minimal OO support for interface definie only, no class support */
#include "utilities/3rd-party/PLOOC/plooc.h"

/* definition for communication pipe and memory block */
#include "utilities/communicate.h"       

/* template for abstraction data type */
#include "utilities/template/template.h"


/*============================ MACROS ========================================*/

#ifndef Hz
#   define Hz                                   ul
#endif

/*----------------------------------------------------------------------------*
 * DO NOT MODIFY ANYTHING BELOW                                               *
 *----------------------------------------------------------------------------*/
#define VSF_CFG_SYNC_EN                             ENABLED
#define VSF_CFG_PREMPT_EN                           ENABLED
#define VSF_CFG_DYNAMIC_PRIOTIRY_EN                 ENABLED
#define VSF_CFG_TIMER_EN                            ENABLED
#define VSF_CFG_QUEUE_EN                            ENABLED
#define VSF_CFG_BMPEVT_EN                           ENABLED
#define VSF_USE_SIMPLE_SHELL                        ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM              ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL         ENABLED
#define VSF_KERNEL_CFG_SUPPORT_THREAD               ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT               ENABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE     ENABLED
#define VSF_KERNEL_CFG_CALLBACK_TIMER               ENABLED
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */