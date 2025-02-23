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

#include "bf0_hal.h"
#include "sifli_bbm.h"

// for vsf_systimer_get_ms
#if VSF_USE_KERNEL != ENABLED
#   error VSF_USE_KERNEL MUST be enabled, bacause SF32 HAL need systimer
#else
#   include "kernel/vsf_kernel.h"
#endif

/*============================ MACROS ========================================*/

#ifdef SOC_BF0_HCPU
#   define RCC                  HPSYS_RCC_BASE
#else
#   define RCC                  LPSYS_RCC_BASE
#endif

#define ATTR_CODE               ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0), ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0))
#define ATTR_RAM                ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE)
#define ATTR_DEVICE             ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE)
/* write back */
#define ATTR_PSRAM_WB           ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1), ARM_MPU_ATTR_MEMORY_(0, 1, 1, 1))
/* write through */
#define ATTR_PSRAM_WT           ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 1), ARM_MPU_ATTR_MEMORY_(0, 0, 1, 1))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    ATTR_CODE_IDX,
    ATTR_RAM_IDX,
    ATTR_DEVICE_IDX,
    ATTR_PSRAM_WB_IDX,
    ATTR_PSRAM_WT_IDX,
};

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
        if (cfsr & SCB_CFSR_STKOF_Msk) {
            vsf_trace_error("Stack Overflow" VSF_TRACE_CFG_LINEEND);
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



static void mpu_clear_region(void)
{
    for (uint32_t i = 0; i < MPU_REGION_NUM; i++) {
        ARM_MPU_ClrRegion(i);
    }
}

VSF_CAL_WEAK(mpu_config)
void mpu_config(void)
{
    uint32_t rnr, rbar, rlar;

#if defined(SOC_BF0_HCPU)
    SCB_InvalidateDCache();
    SCB_InvalidateICache();

    ARM_MPU_Disable();

    mpu_clear_region();

    ARM_MPU_SetMemAttr(ATTR_CODE_IDX, ATTR_CODE);
    ARM_MPU_SetMemAttr(ATTR_RAM_IDX, ATTR_RAM);
    ARM_MPU_SetMemAttr(ATTR_DEVICE_IDX, ATTR_DEVICE);
    ARM_MPU_SetMemAttr(ATTR_PSRAM_WB_IDX, ATTR_PSRAM_WB);
    ARM_MPU_SetMemAttr(ATTR_PSRAM_WT_IDX, ATTR_PSRAM_WT);

    rnr = 0;

    //  hpsys rom
    rbar = ARM_MPU_RBAR(0x0, ARM_MPU_SH_NON, 1, 1, 1); //Non-shareable,RO,any privilege,executable
    rlar = ARM_MPU_RLAR(0x0000ffff, ATTR_CODE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // hpsys RETM/ITCM ram disable sram cache
    rbar = ARM_MPU_RBAR(0x00010000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x0002ffff, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    //  flash1, region 1
    rbar = ARM_MPU_RBAR(0x10000000, ARM_MPU_SH_NON, 1, 1, 0); //Non-shareable,RO,any privilege,executable
    rlar = ARM_MPU_RLAR(0x1fffffff, ATTR_CODE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // hpsys ram, disable sram cache
    rbar = ARM_MPU_RBAR(0x20000000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x2027ffff, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // peripheral
    rbar = ARM_MPU_RBAR(0x40000000, ARM_MPU_SH_NON, 0, 1, 1); //Non-shareable,RW,any privilege,non-executable
    rlar = ARM_MPU_RLAR(0x5fffffff, ATTR_DEVICE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // psram
    rbar = ARM_MPU_RBAR(0x60000000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
#ifdef PSRAM_CACHE_WB
    rlar = ARM_MPU_RLAR(0x61ffffff, ATTR_PSRAM_WB_IDX);
#else
    rlar = ARM_MPU_RLAR(0x61ffffff, ATTR_PSRAM_WT_IDX);
#endif

    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // lpsys ram
    rbar = ARM_MPU_RBAR(0x203fc000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x204fffff, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // lpsys ITCM and CBUS
    rbar = ARM_MPU_RBAR(0x20bfc000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x20cbffff, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // ble rom
    rbar = ARM_MPU_RBAR(0x20800000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x208fffff, ATTR_CODE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // nand 2
    rbar = ARM_MPU_RBAR(0x62000000, ARM_MPU_SH_NON, 1, 1, 0); //Non-shareable,RO,any privilege,executable
    rlar = ARM_MPU_RLAR(0x6FFFFFFF, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    VSF_HAL_ASSERT(rnr <= MPU_REGION_NUM);
    ARM_MPU_Enable(MPU_CTRL_HFNMIENA_Msk);
#else
    ARM_MPU_Disable();

    mpu_clear_region();

    ARM_MPU_SetMemAttr(ATTR_CODE_IDX, ATTR_CODE);
    ARM_MPU_SetMemAttr(ATTR_RAM_IDX, ATTR_RAM);
    ARM_MPU_SetMemAttr(ATTR_DEVICE_IDX, ATTR_DEVICE);

    rnr  = 0;

    // LPSYS ROM, ITCM and C-BUS RAM space
    rbar = ARM_MPU_RBAR(0x0, ARM_MPU_SH_NON, 1, 1, 0); //Non-shareable,RO,any privilege,executable
    rlar = ARM_MPU_RLAR(0x004bffff, ATTR_CODE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    //mpi1 and mpi2
    rbar = ARM_MPU_RBAR(0x60000000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RO,any privilege,executable
    rlar = ARM_MPU_RLAR(0x9FFFFFFF, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // HPSYS RAM
    /* disable cache */
    rbar = ARM_MPU_RBAR(0x2A000000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x2A27FFFF, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // peripheral
    rbar = ARM_MPU_RBAR(0x40000000, ARM_MPU_SH_NON, 0, 1, 1); //Non-shareable,RW,any privilege,non-executable
    rlar = ARM_MPU_RLAR(0x5FFFFFFF, ATTR_DEVICE_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    // LPSYS DTCM and RAM
    // disable cache
    rbar = ARM_MPU_RBAR(0x203fc000, ARM_MPU_SH_NON, 0, 1, 0); //Non-shareable,RW,any privilege,executable
    rlar = ARM_MPU_RLAR(0x204FFFFF, ATTR_RAM_IDX);
    ARM_MPU_SetRegion(rnr++, rbar, rlar);

    HAL_ASSERT(rnr <= MPU_REGION_NUM);

    ARM_MPU_Enable(MPU_CTRL_HFNMIENA_Msk);
#endif
}




// HAL wrapper for bf0_hal.c

VSF_CAL_WEAK(SystemCoreClock) uint32_t SystemCoreClock;

void HAL_Delay_us2_(__IO uint32_t us)
{
    VSF_HAL_ASSERT(false);
    while (1);
}

#define WAIT_US_LOOP_CYCLE 12
__weak void HAL_Delay_us_(__IO uint32_t us)
{
    static uint32_t __hal_sysclk_mhz;
    if (0 == us) {
        __hal_sysclk_mhz = HAL_RCC_GetHCLKFreq(CORE_ID_DEFAULT) / 1000000;
        return;
    }
    if (us > 0) {
        volatile uint32_t i = __hal_sysclk_mhz * (us - 1) / WAIT_US_LOOP_CYCLE;
        while (i-- > 0);
    }
}

void HAL_Delay_us(uint32_t us)
{
    HAL_Delay_us_(us);
}

// ms delay
void HAL_Delay(__IO uint32_t Delay)
{
    HAL_Delay_us(1000 * Delay);
}

uint32_t HAL_GetTick(void)
{
    if ((SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) == 0) {
        VSF_HAL_ASSERT(false);
        while (1);
    }

    return vsf_systimer_get_ms();
}





// Boot source
#define BOOT_FROM_SIP_PUYA  1
#define BOOT_FROM_SIP_GD    2
#define BOOT_FROM_NOR       3
#define BOOT_FROM_NAND      4
#define BOOT_FROM_SD        5
#define BOOT_FROM_EMMC      6

// MPI1 SIP
#define BOOT_SIP_PUYA       0
#define BOOT_SIP_GD         1
#define BOOT_PSRAM_APS_128P 2
#define BOOT_PSRAM_APS_64P  3
#define BOOT_PSRAM_APS_32P  4
#define BOOT_PSRAM_APS_16P  5
#define BOOT_PSRAM_WINBOND  6
#define BOOT_SIP_NONE       7



/* APS 128p*/
static void board_pinmux_psram_func0(int func)
{
    HAL_PIN_Set(PAD_SA01, MPI1_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA02, MPI1_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA03, MPI1_DIO2, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA04, MPI1_DIO3, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA05, MPI1_DIO4, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA06, MPI1_DIO5, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA07, MPI1_DIO6, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA08, MPI1_DIO7, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA09, MPI1_DQSDM, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA10, MPI1_CLK,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA11, MPI1_CS,   PIN_NOPULL, 1);

    HAL_PIN_Set_Analog(PAD_SA00, 1);
    HAL_PIN_Set_Analog(PAD_SA12, 1);
}

/* APS 1:64p 2:32P, 4:Winbond 32/64/128p*/
static void board_pinmux_psram_func1_2_4(int func)
{
    HAL_PIN_Set(PAD_SA01, MPI1_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA02, MPI1_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA03, MPI1_DIO2, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA04, MPI1_DIO3, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA08, MPI1_DIO4, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA09, MPI1_DIO5, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA10, MPI1_DIO6, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA11, MPI1_DIO7, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA07, MPI1_CLK,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA05, MPI1_CS,   PIN_NOPULL, 1);

#ifdef FPGA
    HAL_PIN_Set(PAD_SA00, MPI1_DM, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA06, MPI1_CLKB, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA12, MPI1_DQSDM, PIN_PULLDOWN, 1);
#else
    switch (func)
    {
    case BOOT_PSRAM_APS_64P:
        HAL_PIN_Set(PAD_SA12, MPI1_DQSDM, PIN_PULLDOWN, 1);
        HAL_PIN_Set_Analog(PAD_SA00, 1);
        HAL_PIN_Set_Analog(PAD_SA06, 1);
        break;
    case BOOT_PSRAM_APS_32P:
        HAL_PIN_Set(PAD_SA00, MPI1_DM, PIN_PULLDOWN, 1);
        HAL_PIN_Set(PAD_SA12, MPI1_DQS, PIN_PULLDOWN, 1);
        HAL_PIN_Set(PAD_SA06, MPI1_CLKB, PIN_NOPULL, 1);
        break;
    case BOOT_PSRAM_WINBOND:
        //HAL_PIN_Set(PAD_SA06, MPI1_CLKB, PIN_NOPULL, 1);
        HAL_PIN_Set(PAD_SA12, MPI1_DQSDM, PIN_NOPULL, 1);
        HAL_PIN_Set_Analog(PAD_SA00, 1);
        HAL_PIN_Set_Analog(PAD_SA06, 1);
        break;
    }
#endif
}

/* APS 16p*/
static void board_pinmux_psram_func3(int func)
{
    HAL_PIN_Set(PAD_SA09, MPI1_CLK, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA08, MPI1_CS,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA05, MPI1_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA07, MPI1_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA06, MPI1_DIO2, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_SA10, MPI1_DIO3, PIN_PULLUP, 1);

    HAL_PIN_Set_Analog(PAD_SA00, 1);
    HAL_PIN_Set_Analog(PAD_SA01, 1);
    HAL_PIN_Set_Analog(PAD_SA02, 1);
    HAL_PIN_Set_Analog(PAD_SA03, 1);
    HAL_PIN_Set_Analog(PAD_SA04, 1);
    HAL_PIN_Set_Analog(PAD_SA11, 1);
    HAL_PIN_Set_Analog(PAD_SA12, 1);
}

static void board_pinmux_flash_puya(int func)
{
    HAL_PIN_Set(PAD_SA01, MPI1_CS,   PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA09, MPI1_CLK,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA07, MPI1_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA02, MPI1_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA10, MPI1_DIO3, PIN_NOPULL, 1);
}

static void board_pinmux_mpi1_puya_ext(int is64Mb)
{
    HAL_PIN_Set_Analog(PAD_SA04, 1);
    HAL_PIN_Set_Analog(PAD_SA05, 1);
    HAL_PIN_Set_Analog(PAD_SA06, 1);
    HAL_PIN_Set_Analog(PAD_SA08, 1);
    HAL_PIN_Set_Analog(PAD_SA11, 1);
    HAL_PIN_Set_Analog(PAD_SA12, 1);

    if (is64Mb)
    {
        HAL_PIN_Set_Analog(PAD_SA00, 1);
        HAL_PIN_Set(PAD_SA03, MPI1_DIO2, PIN_PULLUP, 1);
    }
    else
    {
        HAL_PIN_Set_Analog(PAD_SA03, 1);
        HAL_PIN_Set(PAD_SA00, MPI1_DIO2, PIN_PULLUP, 1);
    }
}

static void board_pinmux_flash_gd(int func)
{
    HAL_PIN_Set(PAD_SA04, MPI1_CS,   PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA09, MPI1_CLK,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_SA11, MPI1_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA02, MPI1_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_SA00, MPI1_DIO2, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_SA08, MPI1_DIO3, PIN_PULLUP, 1);

    HAL_PIN_Set_Analog(PAD_SA01, 1);
    HAL_PIN_Set_Analog(PAD_SA03, 1);
    HAL_PIN_Set_Analog(PAD_SA05, 1);
    HAL_PIN_Set_Analog(PAD_SA06, 1);
    HAL_PIN_Set_Analog(PAD_SA07, 1);
    HAL_PIN_Set_Analog(PAD_SA10, 1);
    HAL_PIN_Set_Analog(PAD_SA12, 1);
}

static void board_pinmux_mpi2(void)
{
    HAL_PIN_Set(PAD_PA16, MPI2_CLK, PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA12, MPI2_CS,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA15, MPI2_DIO0, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_PA13, MPI2_DIO1, PIN_PULLDOWN, 1);
    HAL_PIN_Set(PAD_PA14, MPI2_DIO2, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA17, MPI2_DIO3, PIN_PULLUP, 1);
}

static void board_pinmux_sd(void)
{
    HAL_PIN_Set(PAD_PA15, SD1_CMD, PIN_PULLUP, 1);
    HAL_Delay_us(20);   // add a delay before clock setting to avoid wrong cmd happen

    HAL_PIN_Set(PAD_PA14, SD1_CLK,  PIN_NOPULL, 1);
    HAL_PIN_Set(PAD_PA16, SD1_DIO0, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA17, SD1_DIO1, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA12, SD1_DIO2, PIN_PULLUP, 1);
    HAL_PIN_Set(PAD_PA13, SD1_DIO3, PIN_PULLUP, 1);
}

static void board_pinmux_mpi1_none(int func)
{
    uint32_t i;

    for (i = 0; i <= 12; i++)
    {
        HAL_PIN_Set_Analog(PAD_SA00 + i, 1);
    }
}

typedef struct {
    uint8_t mode;
    uint16_t size_mb;
    void (*pinmux_fn)(int);
} mpi1_info_t;

static const mpi1_info_t __mpi1_info[] = {
    [BOOT_SIP_PUYA]         = { SPI_MODE_NOR,       4,  board_pinmux_flash_puya },
    [BOOT_SIP_GD]           = { SPI_MODE_NOR,       4,  board_pinmux_flash_gd },
    [BOOT_PSRAM_APS_128P]   = { SPI_MODE_OPSRAM,    8,  board_pinmux_psram_func0 },
    [BOOT_PSRAM_APS_64P]    = { SPI_MODE_OPSRAM,    8,  board_pinmux_psram_func1_2_4 },
    [BOOT_PSRAM_APS_32P]    = { SPI_MODE_LEGPSRAM,  4,  board_pinmux_psram_func1_2_4 },
    [BOOT_PSRAM_APS_16P]    = { SPI_MODE_PSRAM,     2,  board_pinmux_psram_func3 },
    [BOOT_PSRAM_WINBOND]    = { SPI_MODE_HBPSRAM,   0,  board_pinmux_psram_func1_2_4 },
    [BOOT_SIP_NONE]         = { 0xFF,               0,  board_pinmux_mpi1_none },
};

__HAL_ROM_USED uint32_t HAL_Get_backup(uint8_t idx)
{
    volatile uint32_t *p = &hwp_rtc->BKP0R;
    return *(p + idx);
}





// flash driver

#define INIT_FLASH 0
#if INIT_FLASH
static uint8_t board_boot_src;
static QSPI_FLASH_CTX_T spi_flash_handle[FLASH_MAX_INSTANCE];
static DMA_HandleTypeDef spi_flash_dma_handle[FLASH_MAX_INSTANCE];

typedef int (*flash_read_func)(uint32_t addr, const int8_t *buf, uint32_t size);
typedef int (*flash_write_func)(uint32_t addr, const int8_t *buf, uint32_t size);
typedef int (*flash_erase_func)(uint32_t addr, uint32_t size);
static flash_read_func g_flash_read;
static flash_write_func g_flash_write;
static flash_erase_func g_flash_erase;

static QSPI_FLASH_CTX_T *flash_ctx = NULL;
uint32_t g_config_addr;
#ifdef HAL_USE_NAND
static uint32_t nand_pagesize = 2048;
static uint32_t nand_blksize = 0x20000;
#endif

static int read_nor(uint32_t addr, const int8_t *buf, uint32_t size)
{
    memcpy((void *)buf, (uint8_t *)addr, size);
    return size;
}

static int write_nor(uint32_t addr, const int8_t *buf, uint32_t size)
{
    FLASH_HandleTypeDef *hflash;
    uint32_t taddr, start, remain, fill;
    uint8_t *tbuf;
    int res;

    if (addr >= MPI2_MEM_BASE)
    {
        hflash = &(spi_flash_handle[1].handle);
    }
    else
    {
        hflash = &(spi_flash_handle[0].handle);
    }

    if ((addr < hflash->base) || (addr > (hflash->base + hflash->size)))
        return 0;

    taddr = addr - hflash->base;
    tbuf = (uint8_t *)buf;
    remain = size;

    start = taddr & (QSPI_NOR_PAGE_SIZE - 1);
    if (start > 0)    // start address not page aligned
    {
        fill = QSPI_NOR_PAGE_SIZE - start;   // remained size in one page
        if (fill > size)    // not over one page
        {
            fill = size;
        }
        res = HAL_QSPIEX_WRITE_PAGE(hflash, taddr, tbuf, fill);
        if (res != fill)
            return 0;
        taddr += fill;
        tbuf += fill;
        remain -= fill;
    }
    while (remain > 0)
    {
        fill = remain > QSPI_NOR_PAGE_SIZE ? QSPI_NOR_PAGE_SIZE : remain;
        res = HAL_QSPIEX_WRITE_PAGE(hflash, taddr, tbuf, fill);
        if (res != fill)
            return 0;
        taddr += fill;
        tbuf += fill;
        remain -= fill;
    }

    return size;
}

static int erase_nor(uint32_t addr, uint32_t size)
{
    FLASH_HandleTypeDef *hflash;
    uint32_t taddr, remain;
    int res;

    if (addr >= MPI2_MEM_BASE)
    {
        hflash = &(spi_flash_handle[1].handle);
    }
    else
    {
        hflash = &(spi_flash_handle[0].handle);
    }

    if ((addr < hflash->base) || (addr > (hflash->base + hflash->size)))
        return 1;

    taddr = addr - hflash->base;
    remain = size;

    if ((taddr & (QSPI_NOR_SECT_SIZE - 1)) != 0)
        return -1;
    if ((remain & (QSPI_NOR_SECT_SIZE - 1)) != 0)
        return -2;

    while (remain > 0)
    {
        res = HAL_QSPIEX_SECT_ERASE(hflash, taddr);
        if (res != 0)
            return 1;

        remain -= QSPI_NOR_SECT_SIZE;
        taddr += QSPI_NOR_SECT_SIZE;
    }

    return 0;
}
#endif





/*! \note initialize device driver
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
VSF_CAL_WEAK(vsf_driver_init)
bool vsf_driver_init(void)
{
    // mpu_config MUST be called before enable cache
    mpu_config();
    SCB_EnableICache();
    SCB_EnableDCache();

#ifdef SOC_BF0_HCPU
    SystemCoreClock = HAL_RCC_GetHCLKFreq(CORE_ID_HCPU);
    HAL_Delay_us(0);
    if (SystemCoreClock != 240 * 1000 * 1000) {
        HAL_RCC_HCPU_ConfigHCLK(240);
    }
    if (HAL_RCC_HCPU_GetDLL2Freq() != 288 * 1000 * 1000) {
        HAL_RCC_HCPU_EnableDLL2(288000000);
    }

    uint32_t pid = ((hwp_hpsys_cfg->IDR & HPSYS_CFG_IDR_PID_Msk) >> HPSYS_CFG_IDR_PID_Pos) & 7;
#if INIT_FLASH
    board_boot_src = HAL_Get_backup(RTC_BACKUP_BOOTOPT);
#endif

    const mpi1_info_t *mpi_info = &__mpi1_info[pid];
    qspi_configure_t qspi_cfg;
    struct dma_config flash_dma;

    if (mpi_info->pinmux_fn != NULL) {
        mpi_info->pinmux_fn(pid);
    }
    if ((mpi_info->mode != SPI_MODE_NOR) && (mpi_info->size_mb > 0)) {
        // only set 1.8 for PSRAM, do NOT set if no psram !!!
        HAL_PMU_ConfigPeriLdo(PMU_PERI_LDO_1V8, true, true);
        HAL_RCC_HCPU_ClockSelect(RCC_CLK_MOD_PSRAM1, RCC_CLK_PSRAM_DLL2);

        qspi_cfg.Instance = hwp_qspi1;
        qspi_cfg.SpiMode = mpi_info->mode;
        qspi_cfg.msize = mpi_info->size_mb;
        qspi_cfg.base = QSPI1_MEM_BASE;

        static FLASH_HandleTypeDef f_handle;
        HAL_MPI_PSRAM_Init(&f_handle, &qspi_cfg, 2);
    } else if ((mpi_info->mode == SPI_MODE_NOR) && (mpi_info->size_mb > 0)) {
#if INIT_FLASH
        VSF_HAL_ASSERT(pid == board_boot_src);
        HAL_RCC_HCPU_ClockSelect(RCC_CLK_MOD_FLASH1, RCC_CLK_FLASH_DLL2);

        qspi_cfg.Instance = FLASH1;
        qspi_cfg.line = (board_boot_src == BOOT_FROM_SIP_PUYA) ? HAL_FLASH_NOR_MODE : HAL_FLASH_QMODE;
        qspi_cfg.SpiMode = mpi_info->mode;
        qspi_cfg.msize = mpi_info->size_mb;
        qspi_cfg.base = FLASH_BASE_ADDR;

        flash_dma.dma_irq_prio = 0;
        flash_dma.Instance = DMA1_Channel1;
        flash_dma.dma_irq = DMAC1_CH1_IRQn;
        flash_dma.request = DMA_REQUEST_0;

        spi_flash_handle[0].dual_mode = 1;
        HAL_FLASH_Init(&spi_flash_handle[0], &qspi_cfg, &spi_flash_dma_handle[0], &flash_dma, 6);

        if (board_boot_src == BOOT_FROM_SIP_PUYA) {
            board_pinmux_mpi1_puya_ext(spi_flash_handle[0].dev_id == 0x176085);
            HAL_FLASH_SET_QUAL_SPI((FLASH_HandleTypeDef *) & (spi_flash_handle[0].handle), true);
            spi_flash_handle[0].handle.Mode = HAL_FLASH_QMODE;
        }
#endif
    }

#if INIT_FLASH
    switch (board_boot_src) {
    case BOOT_FROM_SIP_PUYA:
    case BOOT_FROM_SIP_GD:
        flash_ctx = &spi_flash_handle[0];
        g_flash_read = read_nor;
        g_flash_write = write_nor;
        g_flash_erase = erase_nor;
        break;
    case BOOT_FROM_NAND:
#ifndef HAL_USE_NAND
        break;
#endif
    case BOOT_FROM_NOR:
        board_pinmux_mpi2();
        HAL_RCC_HCPU_ClockSelect(RCC_CLK_MOD_FLASH2, RCC_CLK_FLASH_DLL2);

#ifdef HAL_USE_NAND
        bool is_nand = BOOT_FROM_NAND == board_boot_src;
#endif
        qspi_cfg.Instance = FLASH2;
        qspi_cfg.line = HAL_FLASH_QMODE;
        qspi_cfg.msize = 4;
        qspi_cfg.base = FLASH2_BASE_ADDR;
#ifdef HAL_USE_NAND
        if (is_nand) {
            g_flash_read = read_nand;
            static uint32_t nand_cache[(4096 + 128) / 4];

            qspi_cfg.base += HPSYS_MPI_MEM_CBUS_2_SBUS_OFFSET;
            qspi_cfg.SpiMode = SPI_MODE_NAND;
            spi_flash_handle[1].handle.data_buf = (uint8_t *)nand_cache;
        } else
#endif
        {
            g_flash_read = read_nor;
            qspi_cfg.SpiMode = SPI_MODE_NOR;
        }

        flash_dma.dma_irq_prio = 0;
        flash_dma.Instance = DMA1_Channel2;
        flash_dma.dma_irq = DMAC1_CH2_IRQn;
        flash_dma.request = DMA_REQUEST_1;

        spi_flash_handle[1].dual_mode = 1;
#ifdef HAL_USE_NAND
        spi_flash_handle[1].flash_mode = is_nand;
#endif
        HAL_StatusTypeDef res = HAL_FLASH_Init(&spi_flash_handle[1], &qspi_cfg, &spi_flash_dma_handle[1], &flash_dma, 6);
#ifdef HAL_USE_NAND
        if ((HAL_OK == res) && is_nand) {
            nand_pagesize = HAL_NAND_PAGE_SIZE(&spi_flash_handle[1].handle);
            nand_blksize = HAL_NAND_BLOCK_SIZE(&spi_flash_handle[1].handle);

            spi_flash_handle[1].handle.buf_mode = 1;    // default set to buffer mode for nand
            HAL_NAND_CONF_ECC(&spi_flash_handle[1].handle, 1); // default enable ECC if support !
            bbm_set_page_size(nand_pagesize);
            bbm_set_blk_size(nand_blksize);

            static uint32_t bbm_cache_buf[(4096 + 128) / 4];
            sif_bbm_init(spi_flash_handle[1].total_size, (uint8_t *)bbm_cache_buf);
        }
#else
        VSF_HAL_ASSERT(HAL_OK == res);
#endif

        flash_ctx = &spi_flash_handle[1];
        break;
    case BOOT_FROM_SD:
    case BOOT_FROM_EMMC:
        break;
    default:
        VSF_HAL_ASSERT(false);
        break;
    }
#endif      // INIT_FLASH
#endif      // SOC_BF0_HCPU
    return true;
}

/* EOF */
