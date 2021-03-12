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

#ifndef __RISCV_COMPILER_H__
#define __RISCV_COMPILER_H__	1

/*============================ INCLUDES ======================================*/
#include <stdint.h>
#include "./drivers/riscv_bits.h"
#include "./drivers/riscv_const.h"
#include "./drivers/riscv_encoding.h"
#include "./drivers/n200_func.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
#if !defined(__IAR_SYSTEMS_ICC__)
#	define __enable_interrupt			__enable_irq
#	define __disable_interrupt			__disable_irq
#	define __get_interrupt_state		__get_MSTATUS
#	define __set_interrupt_state		__set_MSTATUS
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static inline uint32_t __disable_irq(void)
{
	uint32_t wTemp = read_csr(mstatus);
    clear_csr(mstatus, MSTATUS_MIE);
    return wTemp;
}

static inline void __enable_irq(void)
{
    set_csr(mstatus, MSTATUS_MIE);
}

static inline uint32_t __get_MSTATUS(void)
{
	return read_csr(mstatus) & MSTATUS_MIE;
}

static inline void __set_MSTATUS(uint32_t wValue)
{
	write_csr(mstatus, wValue & MSTATUS_MIE);
}

#ifdef __cplusplus
}
#endif

#endif
