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

/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"

// for VSF_SWI_NUM and VSF_ARCH_PRI_NUM
#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/vsf_hal.h"

#ifndef __VSF_KERNAL_CFG_H__
#define __VSF_KERNAL_CFG_H__

#if VSF_USE_KERNEL == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_KERNEL_ASSERT
#   define VSF_KERNEL_ASSERT                    ASSERT
#endif


#ifndef VSF_KERNEL_CFG_SUPPORT_SYNC
#   define VSF_KERNEL_CFG_SUPPORT_SYNC                      ENABLED
#endif
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
#   ifndef VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ
#       define VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ              ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT
#       define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT          ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE
#       define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE             ENABLED
#   endif
#endif


#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                 ENABLED
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   ifndef VSF_KERNEL_CFG_CALLBACK_TIMER
#       define VSF_KERNEL_CFG_CALLBACK_TIMER                ENABLED
#   endif
#else
#   if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
#       warning "VSF_KERNEL_CFG_EDA_SUPPORT_TIMER MUST be enabled to use callback_timer"
#       undef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#       define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER             ENABLED
#   endif
#endif



#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL              ENABLED
#endif
#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_FSM                   ENABLED
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#       warning "VSF_KERNEL_CFG_EDA_SUPPORT_FSM need VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL, enable by default"
#       define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL  ENABLED
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   ifndef __VSF_KERNEL_CFG_EDA_FRAME_POOL
#       define __VSF_KERNEL_CFG_EDA_FRAME_POOL              ENABLED
#   endif
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE
#   define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE          ENABLED
#endif

#ifndef VSF_KERNEL_CFG_SUPPORT_THREAD
#   define VSF_KERNEL_CFG_SUPPORT_THREAD                    ENABLED
#endif

#ifndef VSF_USE_KERNEL_SIMPLE_SHELL
#   define VSF_USE_KERNEL_SIMPLE_SHELL                      ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_PT
#   define VSF_KERNEL_CFG_EDA_SUPPORT_PT                    ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM            ENABLED
#endif



#define VSF_OS_CFG_MAIN_MODE_NONE               0
#define VSF_OS_CFG_MAIN_MODE_THREAD             1
#define VSF_OS_CFG_MAIN_MODE_EDA                2
#define VSF_OS_CFG_MAIN_MODE_IDLE               3
    
#ifndef VSF_OS_CFG_MAIN_MODE
#   define VSF_OS_CFG_MAIN_MODE                 VSF_OS_CFG_MAIN_MODE_THREAD
#endif

#if VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD
//#   ifndef VSF_OS_CFG_MAIN_STACK_SIZE
//#       warning VSF_OS_CFG_MAIN_STACK_SIZE not defined, define to 4K by default
//#       define VSF_OS_CFG_MAIN_STACK_SIZE                   (4096)
//#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_THREAD
#       define VSF_KERNEL_CFG_SUPPORT_THREAD                ENABLED
#   elif VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#       error VSF_KERNEL_CFG_SUPPORT_THREAD MUST be enabled to use VSF_OS_CFG_MAIN_MODE_THREAD
#   endif
#endif


// queue configurations
#if VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE == ENABLED
#   if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE != ENABLED
#       undef __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE
#       define __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE       ENABLED
#   endif
#   define __VSF_KERNEL_CFG_SUPPORT_EDA_SLIST_QUEUE         ENABLED
#endif


#ifndef VSF_USR_SWI_NUM
#   define VSF_USR_SWI_NUM                      0
#endif
#if     !defined(VSF_OS_CFG_PRIORITY_NUM) && !defined(__VSF_OS_SWI_NUM)
#   if (VSF_SWI_NUM + VSF_USR_SWI_NUM) > VSF_ARCH_PRI_NUM
#       define __VSF_OS_SWI_NUM             VSF_ARCH_PRI_NUM
#   else
#       define __VSF_OS_SWI_NUM             (VSF_SWI_NUM + VSF_USR_SWI_NUM)
#   endif
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define VSF_OS_CFG_PRIORITY_NUM      (__VSF_OS_SWI_NUM+1)
#   else
#       define VSF_OS_CFG_PRIORITY_NUM      __VSF_OS_SWI_NUM
#   endif
#elif !defined(__VSF_OS_SWI_NUM)
#   warning "VSF_OS_CFG_PRIORITY_NUM is defined while __VSF_OS_SWI_NUM is not \
automatically calculated based on VSF_OS_CFG_PRIORITY_NUM in vsf_cfg.h. This \
should not happen."
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define __VSF_OS_SWI_NUM             (VSF_OS_CFG_PRIORITY_NUM-1)
#   else
#       define __VSF_OS_SWI_NUM             VSF_OS_CFG_PRIORITY_NUM
#   endif
#elif !defined(VSF_OS_CFG_PRIORITY_NUM)
#   warning "User should never define __VSF_OS_SWI_NUM which is ought to be \
calculated from macro VSF_OS_CFG_PRIORITY_NUM. Please define \
VSF_OS_CFG_PRIORITY_NUM in your vsf_usr_cfg.h (or any configuration header file \
included by vsf_usr_cfg.h)"
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define VSF_OS_CFG_PRIORITY_NUM      (__VSF_OS_SWI_NUM+1)
#   else
#       define VSF_OS_CFG_PRIORITY_NUM      __VSF_OS_SWI_NUM
#   endif
#endif


#ifndef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#   define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED                    ENABLED
#endif
#ifndef VSF_OS_CFG_ADD_EVTQ_TO_IDLE
#   define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                      DISABLED
#endif

#if __VSF_OS_SWI_NUM > (VSF_USR_SWI_NUM + VSF_SWI_NUM)
#   error "too many VSF_OS_CFG_PRIORITY_NUM!!!"
#endif
#if VSF_OS_CFG_PRIORITY_NUM <= 0
#   error "VSF_OS_CFG_PRIORITY_NUM MUST be > 0"
#endif

#if __VSF_OS_SWI_NUM > 1 && VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED != ENABLED
#   warning "VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED MUST be enabled to support           \
VSF_OS_CFG_PRIORITY_NUM > 1"
#   undef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#   define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED                    ENABLED
#endif

#if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED != ENABLED
#       warning "VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED MUST be enabled to support       \
VSF_OS_CFG_ADD_EVTQ_TO_IDLE"
#       undef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#       define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED                ENABLED
#   endif

/*! \note when VSF_OS_CFG_PRIORITY_NUM equals 1, no SWI is required, hence the 
          macro __VSF_OS_SWI_PRIORITY_BEGIN should **NOT** be defined.
*/
#   if VSF_OS_CFG_PRIORITY_NUM > 1
#       define __VSF_OS_SWI_PRIORITY_BEGIN      vsf_prio_1
#   endif
#else
#   define __VSF_OS_SWI_PRIORITY_BEGIN          vsf_prio_0
#endif

#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#   define __VSF_KERNEL_CFG_EVTQ_EN                         ENABLED

#   ifndef VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY
#       define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY      ENABLED
#   endif

#   if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
#       define __VSF_OS_CFG_EVTQ_LIST
#   else
#       define __VSF_OS_CFG_EVTQ_ARRAY
#       ifndef VSF_OS_CFG_EVTQ_BITSIZE
#           define VSF_OS_CFG_EVTQ_BITSIZE                  4
#       endif
#   endif
#else
#   undef VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY
#   define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY          DISABLED
#endif

#if __VSF_KERNEL_CFG_EVTQ_EN == ENABLED
#   ifndef VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE
#       define VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE           ENABLED
#   endif
#else
#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
#       error "VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE requires __VSF_KERNEL_CFG_EVTQ_EN"
#   endif
#endif


#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                         \
    &&  defined(VSF_ARCH_STACK_PAGE_SIZE)
#   define VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE        VSF_ARCH_STACK_PAGE_SIZE
#endif

#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)                     \
    &&  defined(VSF_ARCH_STACK_GUARDIAN_SIZE)                               
#   define VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE    VSF_ARCH_STACK_GUARDIAN_SIZE
#endif


#ifndef VSF_KERNEL_CFG_FRAME_USER_BITS
#   define VSF_KERNEL_CFG_FRAME_USER_BITS           6
#elif VSF_KERNEL_CFG_FRAME_USER_BITS <= 0
#   undef VSF_KERNEL_CFG_FRAME_USER_BITS
#   define VSF_KERNEL_CFG_FRAME_USER_BITS           6
#endif

/*----------------------------------------------------------------------------*
 * Forced disabled features/modules when C89/90 is used                       *
 *----------------------------------------------------------------------------*/
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   undef VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM            DISABLED

#   undef VSF_KERNEL_CFG_SUPPORT_THREAD
#   define VSF_KERNEL_CFG_SUPPORT_THREAD                    DISABLED
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
#endif

#endif
/* EOF */
