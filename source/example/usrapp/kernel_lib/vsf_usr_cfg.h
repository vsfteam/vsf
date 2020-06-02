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


//! \note Top Level Library Configuration 

#ifndef __KERNEL_LIB_CFG_H__
#define __KERNEL_LIB_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define ASSERT(...)
#define VSF_USE_KERNEL                                          ENABLED
#define VSF_KERNEL_CFG_DEPLOY_IN_LIB_WITH_FULL_FEATURE_SET      ENABLED


#define     VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED                       ENABLED
#define         VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY         ENABLED
#define         VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE              ENABLED

#define     VSF_OS_CFG_ADD_EVTQ_TO_IDLE                         ENABLED

#define     VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE                    ENABLED
#define         __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE          ENABLED
#define         __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE        ENABLED

#define     VSF_KERNEL_CFG_SUPPORT_SYNC                         ENABLED
#define         VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ                 ENABLED
#define         VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT             ENABLED
#define         VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE                ENABLED
#define         VSF_USE_KERNEL_SIMPLE_SHELL                     ENABLED

#define     VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                    ENABLED
#define         VSF_KERNEL_CFG_CALLBACK_TIMER                   ENABLED

#define     VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL                 ENABLED
#define         VSF_KERNEL_CFG_EDA_SUPPORT_FSM                  ENABLED
#define         __VSF_KERNEL_CFG_EDA_FRAME_POOL                 ENABLED

#define     VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE             ENABLED

#define     VSF_KERNEL_CFG_SUPPORT_THREAD                       ENABLED
#define         VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE           1
#define         VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE       0
#define     VSF_KERNEL_CFG_EDA_SUPPORT_PT                       ENABLED
#define     VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM               ENABLED

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */