/******************************************************************************
 * @file     startup_ARMCM4.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M7 Device
 * @version  V2.0.0
 * @date     04. June 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "./device.h"

// for WEAK in compiler
#include "utilities/vsf_utilities.h"

#define __imp_unprocessed_weak_handler(__name)                                  \
            WEAK(__name)                                                        \
            void __name(void){}
#define __imp_blocked_weak_handler(__name)                                      \
            WEAK(__name)                                                        \
            void __name(void){while(1);}

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/

void __NO_RETURN Reset_Handler  (void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
__imp_unprocessed_weak_handler(NMI_Handler)
__imp_blocked_weak_handler(HardFault_Handler)
__imp_blocked_weak_handler(MemManage_Handler)
__imp_blocked_weak_handler(BusFault_Handler)
__imp_blocked_weak_handler(UsageFault_Handler)

__imp_unprocessed_weak_handler(SVC_Handler)
__imp_unprocessed_weak_handler(DebugMon_Handler)
__imp_unprocessed_weak_handler(PendSV_Handler)
__imp_unprocessed_weak_handler(SysTick_Handler)

/* External interrupts */
__imp_unprocessed_weak_handler(BTAON_IRQHandler)
__imp_unprocessed_weak_handler(WCNAON_IRQHandler)
__imp_unprocessed_weak_handler(WGEN_IRQHandler)
__imp_unprocessed_weak_handler(WPROTTRIG_IRQHandler)
__imp_unprocessed_weak_handler(WTXTRIG_IRQHandler)
__imp_unprocessed_weak_handler(WRXTRIG_IRQHandler)
__imp_unprocessed_weak_handler(WTXRXMISC_IRQHandler)
__imp_unprocessed_weak_handler(WTXRXTIM_IRQHandler)
__imp_unprocessed_weak_handler(WPHY_IRQHandler)
__imp_unprocessed_weak_handler(BLE_IRQHandler)
__imp_unprocessed_weak_handler(BT_IRQHandler)
__imp_unprocessed_weak_handler(BTDM_IRQHandler)
__imp_unprocessed_weak_handler(FREQ_IRQHandler)
__imp_unprocessed_weak_handler(DMACOMB_IRQHandler)
__imp_unprocessed_weak_handler(SDIO_IRQHandler)
__imp_unprocessed_weak_handler(USBDMA_IRQHandler)
__imp_unprocessed_weak_handler(UART0_IRQHandler)
__imp_unprocessed_weak_handler(UART1_IRQHandler)
__imp_unprocessed_weak_handler(UART2_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN0_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN1_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU0_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU1_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN0_G1_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN1_G1_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU0_G1_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU1_G1_IRQHandler)
__imp_unprocessed_weak_handler(TIMER20_IRQHandler)
__imp_unprocessed_weak_handler(TIMER21_IRQHandler)
__imp_unprocessed_weak_handler(TIMER22_IRQHandler)
__imp_unprocessed_weak_handler(WDT3_IRQHandler)
__imp_unprocessed_weak_handler(ASDMA_CMN_IRQHandler)
__imp_unprocessed_weak_handler(ASDMA_GE_SW_IRQHandler)
__imp_unprocessed_weak_handler(ASDMA_AUDIO_IRQHandler)
__imp_unprocessed_weak_handler(ASDMA_GE_HW_IRQHandler)
__imp_unprocessed_weak_handler(ASDMA_DMAC_IRQHandler)
__imp_unprocessed_weak_handler(AUD_PROC_IRQHandler)
__imp_unprocessed_weak_handler(DMA00_IRQHandler)
__imp_unprocessed_weak_handler(DMA01_IRQHandler)
__imp_unprocessed_weak_handler(DMA02_IRQHandler)
__imp_unprocessed_weak_handler(DMA03_IRQHandler)
__imp_unprocessed_weak_handler(DMA04_IRQHandler)
__imp_unprocessed_weak_handler(DMA05_IRQHandler)
__imp_unprocessed_weak_handler(DMA06_IRQHandler)
__imp_unprocessed_weak_handler(DMA07_IRQHandler)
__imp_unprocessed_weak_handler(DMA08_IRQHandler)
__imp_unprocessed_weak_handler(DMA09_IRQHandler)
__imp_unprocessed_weak_handler(DMA10_IRQHandler)
__imp_unprocessed_weak_handler(DMA11_IRQHandler)
__imp_unprocessed_weak_handler(DMA12_IRQHandler)
__imp_unprocessed_weak_handler(DMA13_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN2_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN3_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU2_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU3_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN2_G1_IRQHandler)
__imp_unprocessed_weak_handler(MCU2WCN3_G1_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU2_G1_IRQHandler)
__imp_unprocessed_weak_handler(WCN2MCU3_G1_IRQHandler)
__imp_unprocessed_weak_handler(TIMER00_IRQHandler)
__imp_unprocessed_weak_handler(TIMER01_IRQHandler)
__imp_unprocessed_weak_handler(TIMER02_IRQHandler)
__imp_unprocessed_weak_handler(TIMER10_IRQHandler)
__imp_unprocessed_weak_handler(TIMER11_IRQHandler)
__imp_unprocessed_weak_handler(TIMER12_IRQHandler)
__imp_unprocessed_weak_handler(GPIO_IRQHandler)
__imp_unprocessed_weak_handler(WDT0_IRQHandler)
__imp_unprocessed_weak_handler(WDT1_IRQHandler)
__imp_unprocessed_weak_handler(WDT2_IRQHandler)
__imp_unprocessed_weak_handler(SPI0_IRQHandler)
__imp_unprocessed_weak_handler(TRNG_IRQHandler)
__imp_unprocessed_weak_handler(I2CM_IRQHandler)
__imp_unprocessed_weak_handler(I2CS_IRQHandler)
__imp_unprocessed_weak_handler(CACHE0_IRQHandler)
__imp_unprocessed_weak_handler(CACHE1_IRQHandler)
__imp_unprocessed_weak_handler(PSRAM_IRQHandler)
__imp_unprocessed_weak_handler(SDMMC_IRQHandler)
__imp_unprocessed_weak_handler(PSIM_IRQHandler)
__imp_unprocessed_weak_handler(PWRCTRL_IRQHandler)
__imp_unprocessed_weak_handler(PMIC_IRQHandler)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

ROOT const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),                             /*     Initial Stack Pointer */
    Reset_Handler,                                      /*     Reset Handler */
    NMI_Handler,                                        /* -14 NMI Handler */
    HardFault_Handler,                                  /* -13 Hard Fault Handler */
    MemManage_Handler,                                  /* -12 MPU Fault Handler */
    BusFault_Handler,                                   /* -11 Bus Fault Handler */
    UsageFault_Handler,                                 /* -10 Usage Fault Handler */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    SVC_Handler,                                        /*  -5 SVCall Handler */
    DebugMon_Handler,                                   /*  -4 Debug Monitor Handler */
    0,                                                  /*     Reserved */
    PendSV_Handler,                                     /*  -2 PendSV Handler */
    SysTick_Handler,                                    /*  -1 SysTick Handler */

    /* Interrupts */
    BTAON_IRQHandler,                                   /*   0 BT AON */
    WCNAON_IRQHandler,                                  /*   1 WCN AON */
    WGEN_IRQHandler,                                    /*   2 WGEN */
    WPROTTRIG_IRQHandler,                               /*   3 WPROTTRIG */
    WTXTRIG_IRQHandler,                                 /*   4 WTXTRIG */
    WRXTRIG_IRQHandler,                                 /*   5 WRXTRIG */
    WTXRXMISC_IRQHandler,                               /*   6 WTXRXMISC */
    WTXRXTIM_IRQHandler,                                /*   7 WTXRXTIM */
    WPHY_IRQHandler,                                    /*   8 WPHY */
    BLE_IRQHandler,                                     /*   9 BLE */
    BT_IRQHandler,                                      /*  10 BT */
    BTDM_IRQHandler,                                    /*  11 BTDM */
    FREQ_IRQHandler,                                    /*  12 FREQ */
    DMACOMB_IRQHandler,                                 /*  13 DMA Comb */
    SDIO_IRQHandler,                                    /*  14 SDIO */
    USBDMA_IRQHandler,                                  /*  15 USB DMA */
    UART0_IRQHandler,                                   /*  16 UART0 */
    UART1_IRQHandler,                                   /*  17 UART1 */
    UART2_IRQHandler,                                   /*  18 UART2 */
    MCU2WCN0_IRQHandler,                                /*  19 MCU2WCN */
    MCU2WCN1_IRQHandler,                                /*  20 MCU2WCN */
    WCN2MCU0_IRQHandler,                                /*  21 WCN2MCU */
    WCN2MCU1_IRQHandler,                                /*  22 WCN2MCU */
    MCU2WCN0_G1_IRQHandler,                             /*  23 MCU2WCN G1 */
    MCU2WCN1_G1_IRQHandler,                             /*  24 MCU2WCN G1 */
    WCN2MCU0_G1_IRQHandler,                             /*  25 WCN2MCU G1 */
    WCN2MCU1_G1_IRQHandler,                             /*  26 WCN2MCU G1 */
    TIMER20_IRQHandler,                                 /*  27 Timer */
    TIMER21_IRQHandler,                                 /*  28 Timer */
    TIMER22_IRQHandler,                                 /*  29 Timer */
    WDT3_IRQHandler,                                    /*  30 WDT */
    ASDMA_CMN_IRQHandler,                               /*  31 ASDMA Common */
    ASDMA_GE_SW_IRQHandler,                             /*  32 ASDMA Generic SW */
    ASDMA_AUDIO_IRQHandler,                             /*  33 ASDMA Audio Channel */
    ASDMA_GE_HW_IRQHandler,                             /*  34 ASDMA Generic HW */
    ASDMA_DMAC_IRQHandler,                              /*  35 ASDMA Dmac */
    AUD_PROC_IRQHandler,                                /*  36 Audio Process */
    DMA00_IRQHandler,                                   /*  37 DMA */
    DMA01_IRQHandler,                                   /*  38 DMA */
    DMA02_IRQHandler,                                   /*  39 DMA */
    DMA03_IRQHandler,                                   /*  40 DMA */
    DMA04_IRQHandler,                                   /*  41 DMA */
    DMA05_IRQHandler,                                   /*  42 DMA */
    DMA06_IRQHandler,                                   /*  43 DMA */
    DMA07_IRQHandler,                                   /*  44 DMA */
    DMA08_IRQHandler,                                   /*  45 DMA */
    DMA09_IRQHandler,                                   /*  46 DMA */
    DMA10_IRQHandler,                                   /*  47 DMA */
    DMA11_IRQHandler,                                   /*  48 DMA */
    DMA12_IRQHandler,                                   /*  49 DMA */
    DMA13_IRQHandler,                                   /*  50 DMA */
    MCU2WCN2_IRQHandler,                                /*  51 MCU2WCN */
    MCU2WCN3_IRQHandler,                                /*  52 MCU2WCN */
    WCN2MCU2_IRQHandler,                                /*  53 WCN2MCU */
    WCN2MCU3_IRQHandler,                                /*  54 WCN2MCU */
    MCU2WCN2_G1_IRQHandler,                             /*  55 MCU2WCN G1 */
    MCU2WCN3_G1_IRQHandler,                             /*  56 MCU2WCN G1 */
    WCN2MCU2_G1_IRQHandler,                             /*  57 WCN2MCU G1 */
    WCN2MCU3_G1_IRQHandler,                             /*  58 WCN2MCU G1 */
    TIMER00_IRQHandler,                                 /*  59 Timer */
    TIMER01_IRQHandler,                                 /*  60 Timer */
    TIMER02_IRQHandler,                                 /*  61 Timer */
    TIMER10_IRQHandler,                                 /*  62 Timer */
    TIMER11_IRQHandler,                                 /*  63 Timer */
    TIMER12_IRQHandler,                                 /*  64 Timer */
    GPIO_IRQHandler,                                    /*  65 GPIO */
    WDT0_IRQHandler,                                    /*  66 WDT */
    WDT1_IRQHandler,                                    /*  67 WDT */
    WDT2_IRQHandler,                                    /*  68 WDT */
    SPI0_IRQHandler,                                    /*  69 SPI */
    TRNG_IRQHandler,                                    /*  70 TRNG */
    I2CM_IRQHandler,                                    /*  71 I2CM */
    I2CS_IRQHandler,                                    /*  72 I2CS */
    CACHE0_IRQHandler,                                  /*  73 CACHE */
    CACHE1_IRQHandler,                                  /*  74 CACHE */
    PSRAM_IRQHandler,                                   /*  75 PSRAM */
    SDMMC_IRQHandler,                                   /*  76 SDMMC */
    PSIM_IRQHandler,                                    /*  77 PSIM */
    PWRCTRL_IRQHandler,                                 /*  78 PWRCTRL */
    PMIC_IRQHandler,                                    /*  79 PMIC */

    // Image Info
    (pFunc)0x474D4946,      // FIMG, Image header
    (pFunc)0x081FC000,      // Image end, max is 2M - 16K
    (pFunc)0xFFFFFFFF,
    (pFunc)0xFFFFFFFF,
};

static NO_INIT pFunc __isr_vector[dimof(__VECTOR_TABLE) - 4] ALIGN(512);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    vsf_hal_pre_startup_init();

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U) );          /* enable CP11 Full Access */

    memcpy(__isr_vector, __VECTOR_TABLE, sizeof(__isr_vector));
    SCB->VTOR = (uint32_t)__isr_vector;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
