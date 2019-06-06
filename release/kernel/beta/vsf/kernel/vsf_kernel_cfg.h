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

#include "vsf_cfg.h"

#ifndef __VSF_KERNAL_CFG_H__
#define __VSF_KERNAL_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
#ifndef VSF_CFG_SYNC_EN
#   define VSF_CFG_SYNC_EN                      ENABLED
#endif
#if VSF_CFG_SYNC_EN == ENABLED
#   ifndef VSF_CFG_BMPEVT_EN
#       define VSF_CFG_BMPEVT_EN                ENABLED
#   endif
#   ifndef VSF_CFG_QUEUE_EN
#       define VSF_CFG_QUEUE_EN                 ENABLED
#   endif
#endif
#ifndef VSF_CFG_PREMPT_EN
#   define VSF_CFG_PREMPT_EN                    ENABLED
#endif
#ifndef VSF_CFG_TIMER_EN
#   define VSF_CFG_TIMER_EN                     ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL  ENABLED
#endif
#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_FSM       ENABLED
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#       warning "VSF_KERNEL_CFG_EDA_SUPPORT_FSM need VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL, enable by default"
#       define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL       ENABLED
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   undef VSF_KERNEL_CFG_EDA_FRAME_POOL
#   define VSF_KERNEL_CFG_EDA_FRAME_POOL        ENABLED
#endif

#if VSF_CFG_PREMPT_EN == ENABLED
#   ifndef VSF_CFG_DYNAMIC_PRIOTIRY_EN
#       define VSF_CFG_DYNAMIC_PRIOTIRY_EN  DISABLED
#   endif
#else
#   undef VSF_CFG_DYNAMIC_PRIOTIRY_EN
#   define VSF_CFG_DYNAMIC_PRIOTIRY_EN      DISABLED
#endif

#if VSF_CFG_PREMPT_EN == ENABLED
#   define VSF_CFG_EVTQ_EN                  ENABLED
#   if VSF_CFG_DYNAMIC_PRIOTIRY_EN == ENABLED
#       define VSF_CFG_EVTQ_LIST
#   else
#       define VSF_CFG_EVTQ_ARRAY
#   endif
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE
#   define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE  ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
