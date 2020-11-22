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

#ifndef __CORTEX_M_GENERIC_H__
#define __CORTEX_M_GENERIC_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/driver/driver.h"
#undef  __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#include "./SysTick/systick.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#ifndef __LITTLE_ENDIAN
#   define __LITTLE_ENDIAN                 1
#endif
#ifndef __BYTE_ORDER
#   define __BYTE_ORDER                    __LITTLE_ENDIAN
#endif

#if __ARM_ARCH == 6 || __TARGET_ARCH_6_M == 1 || __TARGET_ARCH_6S_M == 1
#   ifndef VSF_ARCH_PRI_NUM
#       define VSF_ARCH_PRI_NUM         4
#       undef  VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         2
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         2
#   endif
#elif __ARM_ARCH >= 7 || __TARGET_ARCH_7_M == 1 || __TARGET_ARCH_7E_M == 1
#   ifndef VSF_ARCH_PRI_NUM
#       define VSF_ARCH_PRI_NUM         16
#       undef  VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         4
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         4
#   endif
#endif

// software interrupt provided by arch
#define VSF_ARCH_SWI_NUM                1
#define VSF_SYSTIMER_CFG_IMPL_MODE      VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER
#define __VSF_ARCH_SYSTIMER_BITS        24

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_arch_wakeup()

/*============================ TYPES =========================================*/

typedef uint64_t vsf_systimer_cnt_t;

#define __VSF_ARCH_PRI_INDEX(__N, __UNUSED)                                     \
            __vsf_arch_prio_index_##__N = (__N),

enum {
    REPEAT_MACRO(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI_INDEX, VSF_ARCH_PRI_BIT)
    __vsf_arch_prio_index_number,
};

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,  \
            vsf_arch_prio_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,


typedef enum vsf_arch_prio_t {
    // avoid vsf_arch_prio_t to be optimized to 8bit
    __VSF_ARCH_PRIO_LEAST_MAX       = INT16_MAX,
    __VSF_ARCH_PRIO_LEAST_MIN       = INT16_MIN,
    VSF_ARCH_PRIO_INVALID           = -1,
    vsf_arch_prio_invalid           = -1,

    REPEAT_MACRO(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI,VSF_ARCH_PRI_BIT)

    vsf_arch_prio_highest           = ((VSF_ARCH_PRI_NUM - 1 - (__vsf_arch_prio_index_number - 1))) & 0xFF ,
} vsf_arch_prio_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack)
{
    __set_MSP(stack);
}

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

