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

#ifndef __CORTEX_A_GENERIC_H__
#define __CORTEX_A_GENERIC_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"

#define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#include "hal/driver/driver.h"
#undef  __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/*============================ MACROS ========================================*/

#define __LITTLE_ENDIAN                 1
#define __BYTE_ORDER                    __LITTLE_ENDIAN

// TODO:
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
#       define VSF_ARCH_PRI_NUM         128
#       undef  VSF_ARCH_PRI_BIT         
#       define VSF_ARCH_PRI_BIT         7
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         7
#   endif
#endif

// software interrupt provided by arch
#define VSF_ARCH_SWI_NUM                1

#define __VSF_ARCH_SYSTIMER_BITS        24
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint64_t vsf_systimer_cnt_t;

#define __VSF_ARCH_PRI_INDEX(__N, __UNUSED)                                     \
            __vsf_arch_prio_index_##__N = (__N),

enum {
    REPEAT_MACRO(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI_INDEX, VSF_ARCH_PRI_BIT)
};

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,  \
            vsf_arch_prio_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)) & 0xFF,

enum vsf_arch_prio_t {
    // avoid vsf_arch_prio_t to be optimized to 8bit
    __VSF_ARCH_PRIO_LEAST_MAX       = INT16_MAX,
    __VSF_ARCH_PRIO_LEAST_MIN       = INT16_MIN,
    VSF_ARCH_PRIO_IVALID            = -1,
    vsf_arch_prio_ivalid            = -1,

    REPEAT_MACRO(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI,VSF_ARCH_PRI_BIT)
};
typedef enum vsf_arch_prio_t vsf_arch_prio_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack)
{
    VSF_HAL_ASSERT(false);
}

#endif
/* EOF */

