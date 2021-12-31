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

#ifndef __VSF_USR_CFG_WEAK_H__
#define __VSF_USR_CFG_WEAK_H__

#ifdef __COMPILER_DOES_NOT_SUPPORT_WEAK__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define WEAK_VSF_KERNEL_ERR_REPORT
#define WEAK___POST_VSF_KERNEL_INIT

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   define WEAK_VSF_SYSTIMER_EVTHANDLER
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_RESOLUTION___FROM_USR
#   define WEAK_VSF_ARCH_REQ___SYSTIMER_FREQ___FROM_USR
#endif

#define WEAK_VSF_DRIVER_INIT
#define WEAK_VSF_HEAP_MALLOC_ALIGNED

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __COMPILER_DOES_NOT_SUPPORT_WEAK__
#endif      // __VSF_USR_CFG_WEAK_H__
