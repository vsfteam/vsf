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


/*! \note Top Level Configuration */

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
#include "utilities/3rd-party/PLOOC/raw/plooc.h"

/* definition for communication pipe and memory block */
#include "utilities/communicate.h"       

/* template for abstraction data type */
#include "utilities/template/template.h"

/* other high level language externsion for OOPC */
#include "utilities/language_extension/language_extension.h"

/*! \note please do not move this including */
#if VSF_KERNEL_CFG_DEPLOY_IN_LIB_WITH_FULL_FEATURE_SET == ENABLED
#   include "kernel/lib/__kernel_lib_with_full_feature_set.h"
#endif


/*============================ MACROS ========================================*/

#ifndef VSF_USE_KERNEL
#   define VSF_USE_KERNEL                       ENABLED
#endif

#ifndef Hz
#   define Hz                                   ul
#endif

#ifndef VSF_USR_SWI_NUM
#   define VSF_USR_SWI_NUM                      0
#endif

#if VSF_USE_KERNEL == ENABLED && defined(VSF_OS_CFG_PRIORITY_NUM)
#   if (VSF_OS_CFG_PRIORITY_NUM < 1)
#       error VSF_OS_CFG_PRIORITY_NUM MUST be defined to calculate \
__VSF_HAL_SWI_NUM and its value must at least be 1. 
#   endif

#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       if VSF_OS_CFG_PRIORITY_NUM > 1
#           define __VSF_HAL_SWI_NUM            (VSF_OS_CFG_PRIORITY_NUM - 1)
#       else
#           define __VSF_HAL_SWI_NUM            0
#       endif
#   else
#       define __VSF_HAL_SWI_NUM                (VSF_OS_CFG_PRIORITY_NUM)
#   endif
// priority configurations
#   define __VSF_OS_SWI_NUM                     __VSF_HAL_SWI_NUM
#endif

#if     (defined(VSF_DEBUGGER_CFG_CONSOLE) && (defined(VSF_HAL_USE_DEBUG_STREAM) && VSF_HAL_USE_DEBUG_STREAM == ENABLED))\
    ||  (defined(VSF_CFG_DEBUG_STREAM_TX_T) && (defined(VSF_HAL_USE_DEBUG_STREAM) && VSF_HAL_USE_DEBUG_STREAM == ENABLED))\
    ||  (defined(VSF_DEBUGGER_CFG_CONSOLE) && defined(VSF_CFG_DEBUG_STREAM_TX_T))
#   error "please enable one of VSF_HAL_USE_DEBUG_STREAM/VSF_DEBUGGER_CFG_CONSOLE/VSF_CFG_DEBUG_STREAM_TX_T"
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */