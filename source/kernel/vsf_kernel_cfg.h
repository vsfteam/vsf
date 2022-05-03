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

/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"

// for VSF_SWI_NUM and VSF_ARCH_PRI_NUM
#include "hal/arch/vsf_arch.h"

#ifndef __VSF_KERNAL_CFG_H__
#define __VSF_KERNAL_CFG_H__

#if VSF_USE_KERNEL == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_KERNEL_ASSERT
#   define VSF_KERNEL_ASSERT                                VSF_ASSERT
#endif


#ifndef VSF_KERNEL_CFG_SUPPORT_SYNC
#   define VSF_KERNEL_CFG_SUPPORT_SYNC                      ENABLED
#endif
#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
#   ifndef VSF_SYNC_CFG_SUPPORT_ISR
#       define VSF_SYNC_CFG_SUPPORT_ISR                     ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT
#       define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT          ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE
#       define VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE           ENABLED
#   endif
#else
#   ifndef VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT
#       define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT          DISABLED
#   endif
#   ifndef VSF_SYNC_CFG_SUPPORT_ISR
#       define VSF_SYNC_CFG_SUPPORT_ISR                     DISABLED
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE
#       define VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE           DISABLED
#   endif
#endif


#define VSF_KERNEL_CFG_TIMER_MODE_TICK                      0
#define VSF_KERNEL_CFG_TIMER_MODE_TICKLESS                  1

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                 ENABLED
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   ifndef VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER
#       define VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER        ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_TIMER_MODE
#       define VSF_KERNEL_CFG_TIMER_MODE                    VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
#   endif
#else
#   ifndef VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER
#       define VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER        DISABLED
#   endif
#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
#       warning "VSF_KERNEL_CFG_EDA_SUPPORT_TIMER MUST be enabled to use callback_timer"
#       undef VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#       define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER             ENABLED
#   endif
#endif



#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL              ENABLED
#endif

// currently, only vsf_task(VSF_KERNEL_CFG_EDA_SUPPORT_TASK) can optionally use VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE
#ifndef VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE
#   define VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE      VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
#endif

#if VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#       warning "Since VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE is ENABLED \
and it requires VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL also be set to \
ENABLED, to allow the compilation continue, \
VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL is forced to ENABLED. If this is \
not what you want, please disable VSF_KERNEL_CFG_EDA_SUBCALL_HAS_RETURN_VALUE."
#       undef VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL
#       define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL          ENABLED
#   endif
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   ifndef __VSF_KERNEL_CFG_EDA_FRAME_POOL
#       define __VSF_KERNEL_CFG_EDA_FRAME_POOL              ENABLED
#   endif
#   ifndef VSF_KERNEL_CFG_EDA_FAST_SUB_CALL
#       define VSF_KERNEL_CFG_EDA_FAST_SUB_CALL             DISABLED
#   endif
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE
#   define VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE          ENABLED
#endif

#ifndef VSF_KERNEL_CFG_SUPPORT_THREAD
#   define VSF_KERNEL_CFG_SUPPORT_THREAD                    ENABLED
#endif

#ifndef VSF_KERNEL_USE_SIMPLE_SHELL
#   define VSF_KERNEL_USE_SIMPLE_SHELL                      ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_TASK
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TASK                  ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_PT
#   define VSF_KERNEL_CFG_EDA_SUPPORT_PT                    ENABLED
#endif

#ifndef VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM
#   define VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM            ENABLED
#endif

#ifndef VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT
#   define VSF_KERNEL_OPT_AVOID_UNNECESSARY_YIELD_EVT       (VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED) || (VSF_KERNEL_CFG_EDA_SUPPORT_SIMPLE_FSM == ENABLED)
#endif



#define VSF_OS_CFG_MAIN_MODE_NONE                           0
#define VSF_OS_CFG_MAIN_MODE_THREAD                         1
#define VSF_OS_CFG_MAIN_MODE_EDA                            2
#define VSF_OS_CFG_MAIN_MODE_IDLE                           3

#ifndef VSF_OS_CFG_MAIN_MODE
#   if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
#       define VSF_OS_CFG_MAIN_MODE                         VSF_OS_CFG_MAIN_MODE_THREAD
#   else
#       define VSF_OS_CFG_MAIN_MODE                         VSF_OS_CFG_MAIN_MODE_IDLE
#   endif
#endif


// queue configurations
#if VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE == ENABLED
#   if defined(VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE) && VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE != ENABLED
#       warning VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE MSUT be enabled for VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE
#       undef VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE
#       define VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE             ENABLED
#   endif
#endif
#ifndef VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE
#   define VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE                 ENABLED
#endif


#ifndef VSF_USR_SWI_NUM
#   define VSF_USR_SWI_NUM                                  0
#endif
#if     !defined(VSF_OS_CFG_PRIORITY_NUM) && !defined(__VSF_OS_SWI_NUM)
#   if (VSF_SWI_NUM + VSF_USR_SWI_NUM) > VSF_ARCH_PRI_NUM
#       define __VSF_OS_SWI_NUM                             VSF_ARCH_PRI_NUM
#   else
#       define __VSF_OS_SWI_NUM                             (VSF_SWI_NUM + VSF_USR_SWI_NUM)
#   endif
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define VSF_OS_CFG_PRIORITY_NUM                      (__VSF_OS_SWI_NUM + 1)
#   else
#       define VSF_OS_CFG_PRIORITY_NUM                      __VSF_OS_SWI_NUM
#   endif
#elif !defined(__VSF_OS_SWI_NUM)
#   warning "VSF_OS_CFG_PRIORITY_NUM is defined while __VSF_OS_SWI_NUM is not \
automatically calculated based on VSF_OS_CFG_PRIORITY_NUM in vsf_cfg.h. This \
should not happen."
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define __VSF_OS_SWI_NUM                             (VSF_OS_CFG_PRIORITY_NUM - 1)
#   else
#       define __VSF_OS_SWI_NUM                             VSF_OS_CFG_PRIORITY_NUM
#   endif
#elif !defined(VSF_OS_CFG_PRIORITY_NUM)
#   warning "User should never define __VSF_OS_SWI_NUM which is ought to be \
calculated from macro VSF_OS_CFG_PRIORITY_NUM. Please define \
VSF_OS_CFG_PRIORITY_NUM in your vsf_usr_cfg.h (or any configuration header file \
included by vsf_usr_cfg.h)"
#   if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#       define VSF_OS_CFG_PRIORITY_NUM                      (__VSF_OS_SWI_NUM + 1)
#   else
#       define VSF_OS_CFG_PRIORITY_NUM                      __VSF_OS_SWI_NUM
#   endif
#endif


#ifndef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#   define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED      ENABLED
#endif
#ifndef VSF_OS_CFG_ADD_EVTQ_TO_IDLE
/*! \note VSF_OS_CFG_ADD_EVTQ_TO_IDLE is disabled by default, because
          tasks in idle evtq does not support dynamic priority feature
*/
#   define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                      DISABLED
#endif

#if     __VSF_OS_SWI_NUM > (VSF_USR_SWI_NUM + VSF_SWI_NUM)                      \
    ||  VSF_OS_CFG_PRIORITY_NUM > (VSF_ARCH_PRI_NUM + 1)
#   error "too many VSF_OS_CFG_PRIORITY_NUM!!!"
#endif
#if VSF_OS_CFG_PRIORITY_NUM <= 0
#   error "VSF_OS_CFG_PRIORITY_NUM MUST > 0"
#endif

#if __VSF_OS_SWI_NUM > 1 && VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED != ENABLED
#   warning "VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED MUST be enabled to support           \
VSF_OS_CFG_PRIORITY_NUM > 1"
#   undef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#   define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED      ENABLED
#endif

#if VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED
#   if      VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED != ENABLED                            \
        &&  VSF_OS_CFG_PRIORITY_NUM > 1
#       warning "VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED MUST be enabled to support       \
VSF_OS_CFG_ADD_EVTQ_TO_IDLE"
#       undef VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED
#       define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED  ENABLED
#   endif

/*! \note when VSF_OS_CFG_PRIORITY_NUM equals 1, no SWI is required, hence the
          macro __VSF_OS_SWI_PRIORITY_BEGIN should **NOT** be defined.
*/
#   if VSF_OS_CFG_PRIORITY_NUM > 1
#       define __VSF_OS_SWI_PRIORITY_BEGIN                  vsf_prio_1
#   endif
#else
#   define __VSF_OS_SWI_PRIORITY_BEGIN                      vsf_prio_0
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
#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
#       ifndef VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR
#           define VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR       ENABLED
#       endif
#   endif
#   if VSF_KERNEL_CFG_SUPPORT_EDA_QUEUE == ENABLED
#       ifndef VSF_EDA_QUEUE_CFG_SUPPORT_ISR
#           define VSF_EDA_QUEUE_CFG_SUPPORT_ISR            ENABLED
#       endif
#   endif
#   ifndef VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE
#       define VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE           ENABLED
#   endif
#else
#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
#       error "VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE requires __VSF_KERNEL_CFG_EVTQ_EN"
#   endif
#endif
#if     VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE != ENABLED                           \
    &&  (   VSF_CALLBACK_TIMER_CFG_SUPPORT_ISR == ENABLED                       \
        ||  VSF_EDA_QUEUE_CFG_SUPPORT_ISR == ENABLED)
#   error VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE MUST be enabled for callback_timer and/or queue isr APIs
#endif


#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE)                         \
    &&  defined(VSF_ARCH_STACK_PAGE_SIZE)
#   define VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE            VSF_ARCH_STACK_PAGE_SIZE
#endif

// on some arch, a stack guardian MUST be used, eg x86 on __WIN__
#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)                     \
    &&  defined(VSF_ARCH_STACK_GUARDIAN_SIZE)
#   define VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE        VSF_ARCH_STACK_GUARDIAN_SIZE
#endif

#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_CHECK)
#   define VSF_KERNEL_CFG_THREAD_STACK_CHECK                DISABLED
#endif

// to support stacks >= 64K bytes, enable VSF_KERNEL_CFG_THREAD_STACK_LARGE, disabled by default
#if     !defined(VSF_KERNEL_CFG_THREAD_STACK_LARGE)
#   define VSF_KERNEL_CFG_THREAD_STACK_LARGE                DISABLED
#endif

// VSF_KERNEL_CFG_CPU_USAGE is not compatible with all platforms in vsf, disabled by default
// VSF_KERNEL_CFG_CPU_USAGE depends on feature of arch:
//  1. if global interrupt is disabled, interrupt can weak cpu
//      supported by ARM/RISCV etc, not supported by win/linux/macos x86/x64
#if     !defined(VSF_KERNEL_CFG_CPU_USAGE)
#   define VSF_KERNEL_CFG_CPU_USAGE                         DISABLED
#endif
#if VSF_KERNEL_CFG_CPU_USAGE == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error VSF_KERNEL_CFG_CPU_USAGE needs VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif

#if  !defined(VSF_KERNEL_CFG_EDA_CPU_USAGE)
#   define VSF_KERNEL_CFG_EDA_CPU_USAGE                     VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif
#if VSF_KERNEL_CFG_EDA_CPU_USAGE == ENABLED && VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error VSF_KERNEL_CFG_EDA_CPU_USAGE needs VSF_KERNEL_CFG_EDA_SUPPORT_TIMER
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
#endif

#endif
/* EOF */
