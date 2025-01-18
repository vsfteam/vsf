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
#include "../__device.h"

#include "register.h"

/*============================ MACROS ========================================*/

#ifdef SOC_BF0_HCPU
#   define RCC                  HPSYS_RCC_BASE
#else
#   define RCC                  LPSYS_RCC_BASE
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#include "hal/driver/common/swi/arm/vsf_swi_template.inc"

void vsf_hw_clkrst_region_set_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_or(32,
        &(((uint32_t *)RCC)[reg_word_offset]),
        (1 << bit_offset)
    );
}

void vsf_hw_clkrst_region_clear_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    vsf_atom_and(32,
        &(((uint32_t *)RCC)[reg_word_offset]),
        ~(1 << bit_offset)
    );
}

uint_fast8_t vsf_hw_clkrst_region_get_bit(uint32_t region)
{
    VSF_HAL_ASSERT(1 == ((region >> 8) & 0x07));
    uint8_t bit_offset = (region >> 0) & 0x1F;
    uint8_t reg_word_offset = region >> 16;

    return (((uint32_t *)RCC)[reg_word_offset] >> bit_offset) & 1;
}

#if VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED

#include "service/trace/vsf_trace.h"

// re-write HardFault_Handler to dump stack
void HardFault_Handler(void)
{
    uintptr_t stack = vsf_arch_get_stack();
    uintptr_t callstack[16] = { 0 };
    uint_fast16_t num = vsf_arch_get_callstack(stack, callstack, dimof(callstack));

    vsf_trace_error("Hardfault:" VSF_TRACE_CFG_LINEEND);

    uint32_t cfsr = SCB->CFSR;
    vsf_trace_error("CFSR: 0x%08X" VSF_TRACE_CFG_LINEEND, cfsr);

    if (cfsr & 0xFF) {
        if (cfsr & SCB_CFSR_IACCVIOL_Msk) {
            vsf_trace_error("Instruction access violation");
        }
        if (cfsr & SCB_CFSR_DACCVIOL_Msk) {
            vsf_trace_error("Data access violation");
        }
        if (cfsr & SCB_CFSR_MUNSTKERR_Msk) {
            vsf_trace_error("MemManage fault on unstacking for a return from exception");
        }
        if (cfsr & SCB_CFSR_MSTKERR_Msk) {
            vsf_trace_error("MemManage fault on stacking for exception entry");
        }
        if (cfsr & SCB_CFSR_MLSPERR_Msk) {
            vsf_trace_error("MemManage fault during floating-point lazy state preservation");
        }
        if (cfsr & SCB_CFSR_MMARVALID_Msk) {
            vsf_trace_error(" at 0x%08X" VSF_TRACE_CFG_LINEEND, SCB->MMFAR);
        } else {
            vsf_trace_error(VSF_TRACE_CFG_LINEEND);
        }
    }
    if (cfsr & 0xFF00) {
        if (cfsr & SCB_CFSR_IBUSERR_Msk) {
            vsf_trace_error("Instruction bus error");
        }
        if (cfsr & SCB_CFSR_PRECISERR_Msk) {
            vsf_trace_error("Precise data bus error");
        }
        if (cfsr & SCB_CFSR_IMPRECISERR_Msk) {
            vsf_trace_error("Imprecise data bus error");
        }
        if (cfsr & SCB_CFSR_UNSTKERR_Msk) {
            vsf_trace_error("BusFault on unstacking for a return from exception");
        }
        if (cfsr & SCB_CFSR_STKERR_Msk) {
            vsf_trace_error("BusFault on stacking for exception entry");
        }
        if (cfsr & SCB_CFSR_LSPERR_Msk) {
            vsf_trace_error("BusFault during floating-point lazy state preservation");
        }
        if (cfsr & SCB_CFSR_BFARVALID_Msk) {
            vsf_trace_error(" at 0x%08X" VSF_TRACE_CFG_LINEEND, SCB->BFAR);
        } else {
            vsf_trace_error(VSF_TRACE_CFG_LINEEND);
        }
    }
    if (cfsr & 0xFFFF0000) {
        if (cfsr & SCB_CFSR_UNDEFINSTR_Msk) {
            vsf_trace_error("Undefined instruction" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_INVSTATE_Msk) {
            vsf_trace_error("Invalid state" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_INVPC_Msk) {
            vsf_trace_error("Invalid PC" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_NOCP_Msk) {
            vsf_trace_error("No coprocessor" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_UNALIGNED_Msk) {
            vsf_trace_error("Unaligned access" VSF_TRACE_CFG_LINEEND);
        }
        if (cfsr & SCB_CFSR_DIVBYZERO_Msk) {
            vsf_trace_error("Divide by zero" VSF_TRACE_CFG_LINEEND);
        }
    }

#if VSF_USE_TRACE == ENABLED && VSF_ARCH_CFG_CALLSTACK_TRACE == ENABLED
    vsf_trace_dump_stack();
#endif
    while (1);
}

void MemManage_Handler(void)
{
    HardFault_Handler();
}

void BusFault_Handler(void)
{
    HardFault_Handler();
}

#endif


// the region added later will have higher priority
VSF_CAL_WEAK(vsf_hw_mpu_add_basic_resgions)
void vsf_hw_mpu_add_basic_resgions(void)
{
    vsf_arch_mpu_disable();

    // background, 4G from 0x00000000
    vsf_arch_mpu_add_region(0x00000000, 0,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_NON_EXECUTABLE     |
                            VSF_ARCH_MPU_ACCESS_NO          |
                            VSF_ARCH_MPU_NON_CACHABLE);

    // System Memory, 64K from 0x1FF00000
    vsf_arch_mpu_add_region(0x1FF00000, 64 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_NON_EXECUTABLE     |
                            VSF_ARCH_MPU_ACCESS_READONLY    |
                            VSF_ARCH_MPU_NON_CACHABLE);

    // AHBs & APBs, 512M from 0x40000000
    vsf_arch_mpu_add_region(0x40000000, 512 * 1024 * 1024,
                            VSF_ARCH_MPU_SHARABLE           |
                            VSF_ARCH_MPU_NON_EXECUTABLE     |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_NON_CACHABLE);

    // AXI SRAM, 512K from 0x24000000
    vsf_arch_mpu_add_region(0x24000000, 1 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    // AHB SRAM, 32K from 0x30000000
    vsf_arch_mpu_add_region(0x30000000, 32 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_BACK_NOALLOC);

    // User FLASH, 4M from 0x08000000
    vsf_arch_mpu_add_region(0x08000000, 4 * 1024 * 1024,
                            VSF_ARCH_MPU_NON_SHARABLE       |
                            VSF_ARCH_MPU_EXECUTABLE         |
                            VSF_ARCH_MPU_ACCESS_FULL        |
                            VSF_ARCH_MPU_CACHABLE_WRITE_THROUGH_NOALLOC);

    vsf_arch_mpu_enable();
}



/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();

    vsf_hw_mpu_add_basic_resgions();
    return true;
}

/* EOF */
