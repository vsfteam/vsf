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

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

// vsf_main_entry is called in reset_handler
#define VSF_KERNEL_CFG_NON_STANDALONE   ENABLED

#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
#   define VSF_SYSTIMER_CFG_IMPL_MODE   VSF_SYSTIMER_IMPL_WITH_COMP_TIMER
#endif
#define VSF_ARCH_PRI_NUM                7
// do not use PLICSW
#define VSF_ARCH_SWI_NUM                0

#define __VSF_ARCH_SYSTIMER_BITS        64

#define VSF_ARCH_SYSTIMER_FREQ          __hpm_systimer_get_frequency()

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int __hpm_systimer_get_frequency(void);

#ifndef __VSF_ARCH_PRIO_DEFINED
#define __VSF_ARCH_PRIO_DEFINED
typedef enum vsf_arch_prio_t {
    VSF_ARCH_PRIO_INVALID = -1,
    vsf_arch_prio_invalid = -1,
    VSF_ARCH_PRIO_0       = 1,
    VSF_ARCH_PRIO_1       = 2,
    VSF_ARCH_PRIO_2       = 3,
    VSF_ARCH_PRIO_3       = 4,
    VSF_ARCH_PRIO_4       = 5,
    VSF_ARCH_PRIO_5       = 6,
    VSF_ARCH_PRIO_6       = 7,
    vsf_arch_prio_0       = 1,
    vsf_arch_prio_1       = 2,
    vsf_arch_prio_2       = 3,
    vsf_arch_prio_3       = 4,
    vsf_arch_prio_4       = 5,
    vsf_arch_prio_5       = 6,
    vsf_arch_prio_6       = 7,
    vsf_arch_prio_highest = 7,
} vsf_arch_prio_t;
#endif

#ifdef __cplusplus
}
#endif

#else

#ifndef __HAL_DEVICE_HPM_HPM6750_H__
#define __HAL_DEVICE_HPM_HPM6750_H__

/*============================ INCLUDES ======================================*/

// TODO: include header from vendor here

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_HPM_HPM6750_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
