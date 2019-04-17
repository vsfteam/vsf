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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vsf_arch_priority_t {
    VSF_ARCH_PRIO_IVALID    = -1,
    VSF_ARCH_PRIO_0         = 0xFF,
    VSF_ARCH_PRIO_1         = 0xFE,
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
    __asm__("MOV pc, %0" : :"r"(pc));
}

static ALWAYS_INLINE uint32_t vsf_arch_get_lr(void)
{
    uint32_t reg;
    __asm__("MOV %0, lr" : "=r"(reg));
    return reg;
}

#endif
/* EOF */
