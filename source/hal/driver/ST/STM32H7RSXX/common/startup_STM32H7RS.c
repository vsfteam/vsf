/******************************************************************************
 * @file     startup_ARMCM7.c(modified for STM32H7RS)
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

#include "../__device.h"

#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/driver.h"

// for VSF_MFOREACH
#include "utilities/vsf_utilities.h"

#define __imp_blocked_weak_handler(__name)                                      \
            VSF_CAL_WEAK(__name)                                                \
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

VSF_MFOREACH(__imp_blocked_weak_handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    SVC_Handler,
    DebugMon_Handler,
    PendSV_Handler,
    SysTick_Handler,
    PVD_PVM_IRQHandler,
    SWI0_IRQHandler,
    DTS_IRQHandler,
    IWDG_IRQHandler,
    WWDG_IRQHandler,
    RCC_IRQHandler,
    LOOKUP_IRQHandler,
    CACHE_ECC_IRQHandler,
    FLASH_IRQHandler,
    RAMECC_IRQHandler,
    FPU_IRQHandler,
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    TAMP_IRQHandler,
    SWI3_IRQHandler,
    SWI4_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    EXTI5_IRQHandler,
    EXTI6_IRQHandler,
    EXTI7_IRQHandler,
    EXTI8_IRQHandler,
    EXTI9_IRQHandler,
    EXTI10_IRQHandler,
    EXTI11_IRQHandler,
    EXTI12_IRQHandler,
    EXTI13_IRQHandler,
    EXTI14_IRQHandler,
    EXTI15_IRQHandler,
    RTC_IRQHandler,
    SAES_IRQHandler,
    CRYP_IRQHandler,
    PKA_IRQHandler,
    HASH_IRQHandler,
    RNG_IRQHandler,
    ADC1_2_IRQHandler,
    GPDMA1_Channel0_IRQHandler,
    GPDMA1_Channel1_IRQHandler,
    GPDMA1_Channel2_IRQHandler,
    GPDMA1_Channel3_IRQHandler,
    GPDMA1_Channel4_IRQHandler,
    GPDMA1_Channel5_IRQHandler,
    GPDMA1_Channel6_IRQHandler,
    GPDMA1_Channel7_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_IRQHandler,
    TIM1_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    TIM5_IRQHandler,
    TIM6_IRQHandler,
    TIM7_IRQHandler,
    TIM9_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    SPI3_IRQHandler,
    SPI4_IRQHandler,
    SPI5_IRQHandler,
    SPI6_IRQHandler,
    HPDMA1_Channel0_IRQHandler,
    HPDMA1_Channel1_IRQHandler,
    HPDMA1_Channel2_IRQHandler,
    HPDMA1_Channel3_IRQHandler,
    HPDMA1_Channel4_IRQHandler,
    HPDMA1_Channel5_IRQHandler,
    HPDMA1_Channel6_IRQHandler,
    HPDMA1_Channel7_IRQHandler,
    SAI1_A_IRQHandler,
    SAI1_B_IRQHandler,
    SAI2_A_IRQHandler,
    SAI2_B_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    UART7_IRQHandler,
    UART8_IRQHandler,
    I3C1_EV_IRQHandler,
    I3C1_ER_IRQHandler,
    OTG_HS_IRQHandler,
    ETH_IRQHandler,
    CORDIC_IRQHandler,
    GFXTIM_IRQHandler,
    DCMIPP_IRQHandler,
    LTDC_IRQHandler,
    LTDC_ER_IRQHandler,
    DMA2D_IRQHandler,
    JPEG_IRQHandler,
    GFXMMU_IRQHandler,
    I3C1_WKUP_IRQHandler,
    MCE1_IRQHandler,
    MCE2_IRQHandler,
    MCE3_IRQHandler,
    XSPI1_IRQHandler,
    XSPI2_IRQHandler,
    FMC_IRQHandler,
    SDMMC1_IRQHandler,
    SDMMC2_IRQHandler,
    SWI5_IRQHandler,
    SWI6_IRQHandler,
    OTG_FS_IRQHandler,
    TIM12_IRQHandler,
    TIM13_IRQHandler,
    TIM14_IRQHandler,
    TIM15_IRQHandler,
    TIM16_IRQHandler,
    TIM17_IRQHandler,
    LPTIM1_IRQHandler,
    LPTIM2_IRQHandler,
    LPTIM3_IRQHandler,
    LPTIM4_IRQHandler,
    LPTIM5_IRQHandler,
    SPDIF_RX_IRQHandler,
    MDIOS_IRQHandler,
    ADF1_FLT0_IRQHandler,
    CRS_IRQHandler,
    UCPD1_IRQHandler,
    CEC_IRQHandler,
    PSSI_IRQHandler,
    LPUART1_IRQHandler,
    WAKEUP_PIN_IRQHandler,
    GPDMA1_Channel8_IRQHandler,
    GPDMA1_Channel9_IRQHandler,
    GPDMA1_Channel10_IRQHandler,
    GPDMA1_Channel11_IRQHandler,
    GPDMA1_Channel12_IRQHandler,
    GPDMA1_Channel13_IRQHandler,
    GPDMA1_Channel14_IRQHandler,
    GPDMA1_Channel15_IRQHandler,
    HPDMA1_Channel8_IRQHandler,
    HPDMA1_Channel9_IRQHandler,
    HPDMA1_Channel10_IRQHandler,
    HPDMA1_Channel11_IRQHandler,
    HPDMA1_Channel12_IRQHandler,
    HPDMA1_Channel13_IRQHandler,
    HPDMA1_Channel14_IRQHandler,
    HPDMA1_Channel15_IRQHandler,
    GPU2D_IRQHandler,
    GPU2D_ER_IRQHandler,
    ICACHE_IRQHandler,
    FDCAN1_IT0_IRQHandler,
    FDCAN1_IT1_IRQHandler,
    FDCAN2_IT0_IRQHandler,
    FDCAN2_IT1_IRQHandler
)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

VSF_CAL_ROOT const pFunc __VECTOR_TABLE[240] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),                   /*     Initial Stack Pointer */
    Reset_Handler,                            /*     Reset Handler */
    NMI_Handler,                              /* -14 NMI Handler */
    HardFault_Handler,                        /* -13 Hard Fault Handler */
    MemManage_Handler,                        /* -12 MPU Fault Handler */
    BusFault_Handler,                         /* -11 Bus Fault Handler */
    UsageFault_Handler,                       /* -10 Usage Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    SVC_Handler,                              /*  -5 SVCall Handler */
    DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
    0,                                        /*     Reserved */
    PendSV_Handler,                           /*  -2 PendSV Handler */
    SysTick_Handler,                          /*  -1 SysTick Handler */

    /* Interrupts */
    PVD_PVM_IRQHandler,                       /*   0 PVD/PVM through EXTI Line detection */
    SWI0_IRQHandler,                          /*   1 SWI0 */
    DTS_IRQHandler,                           /*   2 Digital Temperature Sensor */
    IWDG_IRQHandler,                          /*   3 Internal Watchdog */
    WWDG_IRQHandler,                          /*   4 Window Watchdog */
    RCC_IRQHandler,                           /*   5 RCC */
    LOOKUP_IRQHandler,                        /*   6 LOOKUP */
    CACHE_ECC_IRQHandler,                     /*   7 CACHE ECC */
    FLASH_IRQHandler,                         /*   8 Flash */
    RAMECC_IRQHandler,                        /*   9 RAMRCC */
    FPU_IRQHandler,                           /*  10 FPU */
    SWI1_IRQHandler,                          /*  11 SWI1 */
    SWI2_IRQHandler,                          /*  12 SWI2 */
    TAMP_IRQHandler,                          /*  13 Tamper and TimeStamp interrupts through EXTI Line detection */
    SWI3_IRQHandler,                          /*  14 SWI3 */
    SWI4_IRQHandler,                          /*  15 SWI4 */
    EXTI0_IRQHandler,                         /*  16 EXTI Line0 */
    EXTI1_IRQHandler,                         /*  17 EXTI Line1 */
    EXTI2_IRQHandler,                         /*  18 EXTI Line2 */
    EXTI3_IRQHandler,                         /*  19 EXTI Line3 */
    EXTI4_IRQHandler,                         /*  20 EXTI Line4 */
    EXTI5_IRQHandler,                         /*  21 EXTI Line5 */
    EXTI6_IRQHandler,                         /*  22 EXTI Line6 */
    EXTI7_IRQHandler,                         /*  23 EXTI Line7 */
    EXTI8_IRQHandler,                         /*  24 EXTI Line8 */
    EXTI9_IRQHandler,                         /*  25 EXTI Line9 */
    EXTI10_IRQHandler,                        /*  26 EXTI Line10 */
    EXTI11_IRQHandler,                        /*  27 EXTI Line11 */
    EXTI12_IRQHandler,                        /*  28 EXTI Line12 */
    EXTI13_IRQHandler,                        /*  29 EXTI Line13 */
    EXTI14_IRQHandler,                        /*  30 EXTI Line14 */
    EXTI15_IRQHandler,                        /*  31 EXTI Line15 */
    RTC_IRQHandler,                           /*  32 RTC wakeup and alarm interrupts through EXTI Line detection */
    SAES_IRQHandler,                          /*  33 SAES */
    CRYP_IRQHandler,                          /*  34 CRYPT */
    PKA_IRQHandler,                           /*  35 PKA */
    HASH_IRQHandler,                          /*  36 HASH */
    RNG_IRQHandler,                           /*  37 RNG */
    ADC1_2_IRQHandler,                        /*  38 ADC1 & ADC2 */
    GPDMA1_Channel0_IRQHandler,               /*  39 GPDMA1 Channel 0 */
    GPDMA1_Channel1_IRQHandler,               /*  40 GPDMA1 Channel 1 */
    GPDMA1_Channel2_IRQHandler,               /*  41 GPDMA1 Channel 2 */
    GPDMA1_Channel3_IRQHandler,               /*  42 GPDMA1 Channel 3 */
    GPDMA1_Channel4_IRQHandler,               /*  43 GPDMA1 Channel 4 */
    GPDMA1_Channel5_IRQHandler,               /*  44 GPDMA1 Channel 5 */
    GPDMA1_Channel6_IRQHandler,               /*  45 GPDMA1 Channel 6 */
    GPDMA1_Channel7_IRQHandler,               /*  46 GPDMA1 Channel 7 */
    TIM1_BRK_IRQHandler,                      /*  47 TIM1 Break */
    TIM1_UP_IRQHandler,                       /*  48 TIM1 Update */
    TIM1_TRG_COM_IRQHandler,                  /*  49 TIM1 Trigger and Commutation */
    TIM1_CC_IRQHandler,                       /*  50 TIM1 Capture Compare */
    TIM2_IRQHandler,                          /*  51 TIM2 */
    TIM3_IRQHandler,                          /*  52 TIM3 */
    TIM4_IRQHandler,                          /*  53 TIM4 */
    TIM5_IRQHandler,                          /*  54 TIM5 */
    TIM6_IRQHandler,                          /*  55 TIM6 */
    TIM7_IRQHandler,                          /*  56 TIM7 */
    TIM9_IRQHandler,                          /*  57 TIM9 */
    SPI1_IRQHandler,                          /*  58 SPI1 */
    SPI2_IRQHandler,                          /*  59 SPI2 */
    SPI3_IRQHandler,                          /*  60 SPI3 */
    SPI4_IRQHandler,                          /*  61 SPI4 */
    SPI5_IRQHandler,                          /*  62 SPI5 */
    SPI6_IRQHandler,                          /*  63 SPI6 */
    HPDMA1_Channel0_IRQHandler,               /*  64 HPDMA1 Channel 0 */
    HPDMA1_Channel1_IRQHandler,               /*  65 HPDMA1 Channel 1 */
    HPDMA1_Channel2_IRQHandler,               /*  66 HPDMA1 Channel 2 */
    HPDMA1_Channel3_IRQHandler,               /*  67 HPDMA1 Channel 3 */
    HPDMA1_Channel4_IRQHandler,               /*  68 HPDMA1 Channel 4 */
    HPDMA1_Channel5_IRQHandler,               /*  69 HPDMA1 Channel 5 */
    HPDMA1_Channel6_IRQHandler,               /*  70 HPDMA1 Channel 6 */
    HPDMA1_Channel7_IRQHandler,               /*  71 HPDMA1 Channel 7 */
    SAI1_A_IRQHandler,                        /*  72 Serial Audio Interface 1 block A */
    SAI1_B_IRQHandler,                        /*  73 Serial Audio Interface 1 block B */
    SAI2_A_IRQHandler,                        /*  74 Serial Audio Interface 2 block A */
    SAI2_B_IRQHandler,                        /*  75 Serial Audio Interface 2 block B */
    I2C1_EV_IRQHandler,                       /*  76 I2C1 Event */
    I2C1_ER_IRQHandler,                       /*  77 I2C1 Error */
    I2C2_EV_IRQHandler,                       /*  78 I2C2 Event */
    I2C2_ER_IRQHandler,                       /*  79 I2C2 Error */
    I2C3_EV_IRQHandler,                       /*  80 I2C3 Event */
    I2C3_ER_IRQHandler,                       /*  81 I2C3 Error */
    USART1_IRQHandler,                        /*  82 USART1 */
    USART2_IRQHandler,                        /*  83 USART2 */
    USART3_IRQHandler,                        /*  84 USART3 */
    UART4_IRQHandler,                         /*  85 UART4 */
    UART5_IRQHandler,                         /*  86 UART5 */
    UART7_IRQHandler,                         /*  87 UART7 */
    UART8_IRQHandler,                         /*  88 UART8 */
    I3C1_EV_IRQHandler,                       /*  89 I3C1 Event */
    I3C1_ER_IRQHandler,                       /*  90 I3C1 Error */
    OTG_HS_IRQHandler,                        /*  91 USB OTG HS */
    ETH_IRQHandler,                           /*  92 Ethernet */
    CORDIC_IRQHandler,                        /*  93 CORDIC */
    GFXTIM_IRQHandler,                        /*  94 GFXTIM */
    DCMIPP_IRQHandler,                        /*  95 DCMIPP */
    LTDC_IRQHandler,                          /*  96 LTDC */
    LTDC_ER_IRQHandler,                       /*  97 LTDC error */
    DMA2D_IRQHandler,                         /*  98 DMA2D */
    JPEG_IRQHandler,                          /*  99 JPEG */
    GFXMMU_IRQHandler,                        /*  100 GFXMMU */
    I3C1_WKUP_IRQHandler,                     /*  101 I3C1 wakeup */
    MCE1_IRQHandler,                          /*  102 MCE1 */
    MCE2_IRQHandler,                          /*  103 MCE2 */
    MCE3_IRQHandler,                          /*  104 MCE3 */
    XSPI1_IRQHandler,                         /*  105 XSPI1 */
    XSPI2_IRQHandler,                         /*  106 XSPI2 */
    FMC_IRQHandler,                           /*  107 FMC */
    SDMMC1_IRQHandler,                        /*  108 SDMMC1 */
    SDMMC2_IRQHandler,                        /*  109 SDMMC2 */
    SWI5_IRQHandler,                          /*  110 SWI5 */
    SWI6_IRQHandler,                          /*  111 SWI6 */
    OTG_FS_IRQHandler,                        /*  112 USB OTG FS */
    TIM12_IRQHandler,                         /*  113 TIM12 */
    TIM13_IRQHandler,                         /*  114 TIM13 */
    TIM14_IRQHandler,                         /*  115 TIM14 */
    TIM15_IRQHandler,                         /*  116 TIM15 */
    TIM16_IRQHandler,                         /*  117 TIM16 */
    TIM17_IRQHandler,                         /*  118 TIM17 */
    LPTIM1_IRQHandler,                        /*  119 LP TIM1 */
    LPTIM2_IRQHandler,                        /*  120 LP TIM2 */
    LPTIM3_IRQHandler,                        /*  121 LP TIM3 */
    LPTIM4_IRQHandler,                        /*  122 LP TIM4 */
    LPTIM5_IRQHandler,                        /*  123 LP TIM5 */
    SPDIF_RX_IRQHandler,                      /*  124 SPDIF_RX */
    MDIOS_IRQHandler,                         /*  125 MDIOS */
    ADF1_FLT0_IRQHandler,                     /*  126 ADF1 Filter 0 */
    CRS_IRQHandler,                           /*  127 CRS */
    UCPD1_IRQHandler,                         /*  128 UCPD1 */
    CEC_IRQHandler,                           /*  129 HDMI_CEC */
    PSSI_IRQHandler,                          /*  130 PSSI */
    LPUART1_IRQHandler,                       /*  131 LP UART1 */
    WAKEUP_PIN_IRQHandler,                    /*  132 Wake-up pins interrupt */
    GPDMA1_Channel8_IRQHandler,               /*  133 GPDMA1 Channel 8 */
    GPDMA1_Channel9_IRQHandler,               /*  134 GPDMA1 Channel 9 */
    GPDMA1_Channel10_IRQHandler,              /*  135 GPDMA1 Channel 10 */
    GPDMA1_Channel11_IRQHandler,              /*  136 GPDMA1 Channel 11 */
    GPDMA1_Channel12_IRQHandler,              /*  137 GPDMA1 Channel 12 */
    GPDMA1_Channel13_IRQHandler,              /*  138 GPDMA1 Channel 13 */
    GPDMA1_Channel14_IRQHandler,              /*  139 GPDMA1 Channel 14 */
    GPDMA1_Channel15_IRQHandler,              /*  140 GPDMA1 Channel 15 */
    HPDMA1_Channel8_IRQHandler,               /*  141 HPDMA1 Channel 8 */
    HPDMA1_Channel9_IRQHandler,               /*  142 HPDMA1 Channel 9 */
    HPDMA1_Channel10_IRQHandler,              /*  143 HPDMA1 Channel 10 */
    HPDMA1_Channel11_IRQHandler,              /*  144 HPDMA1 Channel 11 */
    HPDMA1_Channel12_IRQHandler,              /*  145 HPDMA1 Channel 12 */
    HPDMA1_Channel13_IRQHandler,              /*  146 HPDMA1 Channel 13 */
    HPDMA1_Channel14_IRQHandler,              /*  147 HPDMA1 Channel 14 */
    HPDMA1_Channel15_IRQHandler,              /*  148 HPDMA1 Channel 15 */
    GPU2D_IRQHandler,                         /*  149 GPU2D */
    GPU2D_ER_IRQHandler,                      /*  150 GPU2D error */
    ICACHE_IRQHandler,                        /*  151 ICACHE */
    FDCAN1_IT0_IRQHandler,                    /*  152 FDCAN1 Interrupt 0 */
    FDCAN1_IT1_IRQHandler,                    /*  153 FDCAN1 Interrupt 1 */
    FDCAN2_IT0_IRQHandler,                    /*  154 FDCAN2 Interrupt 0 */
    FDCAN2_IT1_IRQHandler,                    /*  155 FDCAN2 Interrupt 1 */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{
    SystemInit();
    SCB_EnableICache();
    SCB_EnableDCache();
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    vsf_arch_set_stack((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                  (3U << 11U*2U));            /* enable CP11 Full Access */

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
