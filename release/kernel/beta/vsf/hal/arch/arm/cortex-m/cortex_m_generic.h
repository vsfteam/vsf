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
#include "./SysTick/systick.h"
/*============================ MACROS ========================================*/

#define __LITTLE_ENDIAN             1
#define __BYTE_ORDER                __LITTLE_ENDIAN

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
#       define VSF_ARCH_PRI_NUM         256
#       undef  VSF_ARCH_PRI_BIT         
#       define VSF_ARCH_PRI_BIT         8
#   endif

#   ifndef VSF_ARCH_PRI_BIT
#       define VSF_ARCH_PRI_BIT         8
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint64_t vsf_systimer_cnt_t;

#define __VSF_ARCH_PRI_INDEX(__N, __UNUSED)                                     \
            __vsf_arch_prio_index_##__N = (__N),

enum {
    MREPEAT(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI_INDEX, VSF_ARCH_PRI_BIT)
};

#define __VSF_ARCH_PRI(__N, __BIT)                                              \
            VSF_ARCH_PRIO_##__N =                                               \
                ((VSF_ARCH_PRI_NUM - 1 - __vsf_arch_prio_index_##__N)           \
                    << (8- (__BIT))) & 0xFF,

enum vsf_arch_priority_t {
    VSF_ARCH_PRIO_IVALID    = -1,
    MREPEAT(VSF_ARCH_PRI_NUM,__VSF_ARCH_PRI,VSF_ARCH_PRI_BIT)
};
typedef enum vsf_arch_priority_t vsf_arch_priority_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static ALWAYS_INLINE void vsf_arch_set_stack(uint32_t stack)
{
    __set_MSP(stack);
}

static ALWAYS_INLINE void vsf_arch_set_pc(uint32_t pc)
{
#if __IS_COMPILER_ARM_COMPILER_5__
    register uint32_t temp __asm("pc");
    temp = pc;
#else
    __asm__("MOV pc, %0" : :"r"(pc));
#endif
}

static ALWAYS_INLINE uint32_t vsf_arch_get_lr(void)
{

    uint32_t reg;
#if __IS_COMPILER_ARM_COMPILER_5__
    register uint32_t temp __asm("lr");
    reg = temp;
#else
    __asm__("MOV %0, lr" : "=r"(reg));
#endif
    return reg;
}

#endif
/* EOF */

