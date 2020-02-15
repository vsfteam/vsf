/******************************************************************************
 * @file     startup_ARMCM4.c(modified for STM32F730R8)
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
WEAK(NMI_Handler) 
void NMI_Handler            (void){}

WEAK(HardFault_Handler) 
void HardFault_Handler      (void){while(1);} 

WEAK(MemManage_Handler) 
void MemManage_Handler      (void){while(1);} 

WEAK(BusFault_Handler) 
void BusFault_Handler       (void){while(1);} 

WEAK(UsageFault_Handler) 
void UsageFault_Handler     (void){while(1);}

WEAK(SVC_Handler) 
void SVC_Handler            (void){} 

WEAK(DebugMon_Handler) 
void DebugMon_Handler       (void){} 

WEAK(PendSV_Handler) 
void PendSV_Handler         (void){} 

WEAK(SysTick_Handler) 
void SysTick_Handler        (void){} 

WEAK(WWDG_IRQHandler) 
void WWDG_IRQHandler        (void){} 

WEAK(PVD_IRQHandler) 
void PVD_IRQHandler         (void){} 

WEAK(TAMP_STAMP_IRQHandler) 
void TAMP_STAMP_IRQHandler  (void){} 

WEAK(RTC_WKUP_IRQHandler) 
void RTC_WKUP_IRQHandler    (void){} 

WEAK(FLASH_IRQHandler) 
void FLASH_IRQHandler       (void){} 

WEAK(RCC_IRQHandler) 
void RCC_IRQHandler         (void){} 

WEAK(EXTI0_IRQHandler) 
void EXTI0_IRQHandler       (void){} 

WEAK(EXTI1_IRQHandler) 
void EXTI1_IRQHandler       (void){} 

WEAK(EXTI2_IRQHandler) 
void EXTI2_IRQHandler       (void){} 

WEAK(EXTI3_IRQHandler) 
void EXTI3_IRQHandler       (void){} 

WEAK(EXTI4_IRQHandler) 
void EXTI4_IRQHandler       (void){} 

WEAK(DMA1_Stream0_IRQHandler) 
void DMA1_Stream0_IRQHandler(void){} 

WEAK(DMA1_Stream1_IRQHandler) 
void DMA1_Stream1_IRQHandler(void){} 

WEAK(DMA1_Stream2_IRQHandler) 
void DMA1_Stream2_IRQHandler(void){} 

WEAK(DMA1_Stream3_IRQHandler) 
void DMA1_Stream3_IRQHandler(void){} 

WEAK(DMA1_Stream4_IRQHandler) 
void DMA1_Stream4_IRQHandler(void){} 

WEAK(DMA1_Stream5_IRQHandler) 
void DMA1_Stream5_IRQHandler(void){} 

WEAK(DMA1_Stream6_IRQHandler) 
void DMA1_Stream6_IRQHandler(void){} 

WEAK(ADC_IRQHandler) 
void ADC_IRQHandler         (void){} 

WEAK(CAN1_TX_IRQHandler) 
void CAN1_TX_IRQHandler     (void){} 

WEAK(CAN1_RX0_IRQHandler) 
void CAN1_RX0_IRQHandler    (void){} 

WEAK(CAN1_RX1_IRQHandler) 
void CAN1_RX1_IRQHandler    (void){} 

WEAK(CAN1_SCE_IRQHandler) 
void CAN1_SCE_IRQHandler    (void){} 

WEAK(EXTI9_5_IRQHandler) 
void EXTI9_5_IRQHandler     (void){} 

WEAK(TIM1_BRK_TIM9_IRQHandler) 
void TIM1_BRK_TIM9_IRQHandler(void){} 

WEAK(TIM1_UP_TIM10_IRQHandler) 
void TIM1_UP_TIM10_IRQHandler(void){} 

WEAK(TIM1_TRG_COM_TIM11_IRQHandler) 
void TIM1_TRG_COM_TIM11_IRQHandler(void){} 

WEAK(TIM1_CC_IRQHandler) 
void TIM1_CC_IRQHandler     (void){} 

WEAK(TIM2_IRQHandler) 
void TIM2_IRQHandler        (void){} 

WEAK(TIM3_IRQHandler) 
void TIM3_IRQHandler        (void){} 

WEAK(TIM4_IRQHandler) 
void TIM4_IRQHandler        (void){}

WEAK(I2C1_EV_IRQHandler) 
void I2C1_EV_IRQHandler     (void){}

WEAK(I2C1_ER_IRQHandler) 
void I2C1_ER_IRQHandler     (void){}

WEAK(I2C2_EV_IRQHandler) 
void I2C2_EV_IRQHandler     (void){}

WEAK(I2C2_ER_IRQHandler) 
void I2C2_ER_IRQHandler     (void){}

WEAK(SPI1_IRQHandler) 
void SPI1_IRQHandler        (void){}

WEAK(SPI2_IRQHandler) 
void SPI2_IRQHandler        (void){}

WEAK(USART1_IRQHandler) 
void USART1_IRQHandler      (void){}


WEAK(USART2_IRQHandler) 
void USART2_IRQHandler      (void){}

WEAK(USART3_IRQHandler) 
void USART3_IRQHandler      (void){}

WEAK(EXTI15_10_IRQHandler) 
void EXTI15_10_IRQHandler   (void){}

WEAK(RTC_Alarm_IRQHandler) 
void RTC_Alarm_IRQHandler   (void){}

WEAK(OTG_FS_WKUP_IRQHandler) 
void OTG_FS_WKUP_IRQHandler (void){}

WEAK(TIM8_BRK_TIM12_IRQHandler) 
void TIM8_BRK_TIM12_IRQHandler(void){}

WEAK(TIM8_UP_TIM13_IRQHandler) 
void TIM8_UP_TIM13_IRQHandler(void){}

WEAK(TIM8_TRG_COM_TIM14_IRQHandler) 
void TIM8_TRG_COM_TIM14_IRQHandler(void){}

WEAK(TIM8_CC_IRQHandler) 
void TIM8_CC_IRQHandler(void){}

WEAK(DMA1_Stream7_IRQHandler) 
void DMA1_Stream7_IRQHandler(void){}

WEAK(FMC_IRQHandler) 
void FMC_IRQHandler         (void){}

WEAK(SDMMC1_IRQHandler) 
void SDMMC1_IRQHandler      (void){}

WEAK(TIM5_IRQHandler) 
void TIM5_IRQHandler        (void){}

WEAK(SPI3_IRQHandler) 
void SPI3_IRQHandler        (void){}

WEAK(UART4_IRQHandler) 
void UART4_IRQHandler       (void){}

WEAK(UART5_IRQHandler) 
void UART5_IRQHandler       (void){}

WEAK(TIM6_DAC_IRQHandler) 
void TIM6_DAC_IRQHandler    (void){}

WEAK(TIM7_IRQHandler) 
void TIM7_IRQHandler        (void){}

WEAK(DMA2_Stream0_IRQHandler) 
void DMA2_Stream0_IRQHandler(void){}

WEAK(DMA2_Stream1_IRQHandler) 
void DMA2_Stream1_IRQHandler(void){}

WEAK(DMA2_Stream2_IRQHandler) 
void DMA2_Stream2_IRQHandler(void){}

WEAK(DMA2_Stream3_IRQHandler) 
void DMA2_Stream3_IRQHandler(void){}

WEAK(DMA2_Stream4_IRQHandler) 
void DMA2_Stream4_IRQHandler(void){}

WEAK(ETH_IRQHandler)
void ETH_IRQHandler         (void){}

WEAK(ETH_WKUP_IRQHandler)
void ETH_WKUP_IRQHandler    (void){}

WEAK(SWI0_IRQHandler) 
void SWI0_IRQHandler        (void){}

WEAK(SWI1_IRQHandler) 
void SWI1_IRQHandler        (void){}

WEAK(SWI2_IRQHandler) 
void SWI2_IRQHandler        (void){}

WEAK(SWI3_IRQHandler) 
void SWI3_IRQHandler        (void){}

WEAK(OTG_FS_IRQHandler) 
void OTG_FS_IRQHandler      (void){}

WEAK(DMA2_Stream5_IRQHandler) 
void DMA2_Stream5_IRQHandler(void){}

WEAK(DMA2_Stream6_IRQHandler) 
void DMA2_Stream6_IRQHandler(void){}

WEAK(DMA2_Stream7_IRQHandler) 
void DMA2_Stream7_IRQHandler(void){}

WEAK(USART6_IRQHandler) 
void USART6_IRQHandler      (void){}

WEAK(I2C3_EV_IRQHandler) 
void I2C3_EV_IRQHandler     (void){}

WEAK(I2C3_ER_IRQHandler) 
void I2C3_ER_IRQHandler     (void){}

WEAK(OTG_HS_EP1_OUT_IRQHandler) 
void OTG_HS_EP1_OUT_IRQHandler(void){}

WEAK(OTG_HS_EP1_IN_IRQHandler) 
void OTG_HS_EP1_IN_IRQHandler(void){}

WEAK(OTG_HS_WKUP_IRQHandler) 
void OTG_HS_WKUP_IRQHandler (void){}

WEAK(OTG_HS_IRQHandler) 
void OTG_HS_IRQHandler      (void){}

WEAK(SWI4_IRQHandler) 
void SWI4_IRQHandler        (void){}

WEAK(AES_IRQHandler) 
void AES_IRQHandler         (void){}

WEAK(RNG_IRQHandler) 
void RNG_IRQHandler         (void){}

WEAK(FPU_IRQHandler) 
void FPU_IRQHandler         (void){}

WEAK(UART7_IRQHandler) 
void UART7_IRQHandler       (void){}

WEAK(UART8_IRQHandler) 
void UART8_IRQHandler       (void){}

WEAK(SPI4_IRQHandler) 
void SPI4_IRQHandler        (void){}

WEAK(SPI5_IRQHandler) 
void SPI5_IRQHandler        (void){}

WEAK(SWI5_IRQHandler) 
void SWI5_IRQHandler        (void){}

WEAK(SAI1_IRQHandler) 
void SAI1_IRQHandler        (void){}

WEAK(SWI6_IRQHandler) 
void SWI6_IRQHandler        (void){}

WEAK(SWI7_IRQHandler) 
void SWI7_IRQHandler        (void){}

WEAK(SWI8_IRQHandler) 
void SWI8_IRQHandler        (void){}

WEAK(SAI2_IRQHandler) 
void SAI2_IRQHandler        (void){}

WEAK(QUADSPI_IRQHandler) 
void QUADSPI_IRQHandler     (void){}

WEAK(LPTIM1_IRQHandler) 
void LPTIM1_IRQHandler      (void){}

WEAK(SWI9_IRQHandler) 
void SWI9_IRQHandler        (void){}

WEAK(SWI10_IRQHandler) 
void SWI10_IRQHandler       (void){}

WEAK(SWI11_IRQHandler) 
void SWI11_IRQHandler       (void){}

WEAK(SWI12_IRQHandler) 
void SWI12_IRQHandler       (void){}

WEAK(SDMMC2_IRQHandler) 
void SDMMC2_IRQHandler      (void){}

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

ROOT const pFunc __VECTOR_TABLE[240] __VECTOR_TABLE_ATTRIBUTE = {
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
    WWDG_IRQHandler,                          /*   0 Window WatchDog */
    PVD_IRQHandler,                           /*   1 PVD through EXTI line detection */
    TAMP_STAMP_IRQHandler,                    /*   2 Tamper and TimeStamp through the EXTI line */
    RTC_WKUP_IRQHandler,                      /*   3 RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,                         /*   4 Flash */
    RCC_IRQHandler,                           /*   5 RCC */
    EXTI0_IRQHandler,                         /*   6 EXTI Line0 */
    EXTI1_IRQHandler,                         /*   7 EXTI Line1 */
    EXTI2_IRQHandler,                         /*   8 EXTI Line2 */
    EXTI3_IRQHandler,                         /*   9 EXTI Line3 */
    EXTI4_IRQHandler,                         /*  10 EXTI Line4 */
    DMA1_Stream0_IRQHandler,                  /*  11 DMA1 Stream0 */
    DMA1_Stream1_IRQHandler,                  /*  12 DMA1 Stream1 */
    DMA1_Stream2_IRQHandler,                  /*  13 DMA1 Stream2 */
    DMA1_Stream3_IRQHandler,                  /*  14 DMA1 Stream3 */
    DMA1_Stream4_IRQHandler,                  /*  15 DMA1 Stream4 */
    DMA1_Stream5_IRQHandler,                  /*  16 DMA1 Stream5 */
    DMA1_Stream6_IRQHandler,                  /*  17 DMA1 Stream6 */
    ADC_IRQHandler,                           /*  18 ADC1, ADC2 and ADC3 */
    CAN1_TX_IRQHandler,                       /*  19 CAN1 TX */
    CAN1_RX0_IRQHandler,                      /*  20 CAN1 RX0 */
    CAN1_RX1_IRQHandler,                      /*  21 CAN1 RX1 */
    CAN1_SCE_IRQHandler,                      /*  22 CAN1 SCE */
    EXTI9_5_IRQHandler,                       /*  23 EXTI Line[9:5] */
    TIM1_BRK_TIM9_IRQHandler,                 /*  24 TIM1 Break and TIM9 */
    TIM1_UP_TIM10_IRQHandler,                 /*  25 TIM1 Update and TIM10 */
    TIM1_TRG_COM_TIM11_IRQHandler,            /*  26 TIM1 Trigger and Commutation and TIM11 */
    TIM1_CC_IRQHandler,                       /*  27 TIM1 Capture Compare */
    TIM2_IRQHandler,                          /*  28 TIM2 */
    TIM3_IRQHandler,                          /*  29 TIM3 */
    TIM4_IRQHandler,                          /*  30 TIM4 */
    I2C1_EV_IRQHandler,                       /*  31 I2C1 Event */
    I2C1_ER_IRQHandler,                       /*  32 I2C1 Error */
    I2C2_EV_IRQHandler,                       /*  33 I2C2 Event */
    I2C2_ER_IRQHandler,                       /*  34 I2C2 Error */
    SPI1_IRQHandler,                          /*  35 SPI1 */
    SPI2_IRQHandler,                          /*  36 SPI2 */
    USART1_IRQHandler,                        /*  37 USART1 */
    USART2_IRQHandler,                        /*  38 USART2 */
    USART3_IRQHandler,                        /*  39 USART3 */
    EXTI15_10_IRQHandler,                     /*  40 External Line[15:10] */
    RTC_Alarm_IRQHandler,                     /*  41 RTC Alarm (A and B) through EXTI Line */
    OTG_FS_WKUP_IRQHandler,                   /*  42 USB OTG FS Wakeup through EXTI line */
    TIM8_BRK_TIM12_IRQHandler,                /*  43 TIM8 Break and TIM12 */
    TIM8_UP_TIM13_IRQHandler,                 /*  44 TIM8 Update and TIM13 */
    TIM8_TRG_COM_TIM14_IRQHandler,            /*  45 TIM8 Trigger and Commutation and TIM14 */
    TIM8_CC_IRQHandler,                       /*  46 TIM8 Capture Compare */
    DMA1_Stream7_IRQHandler,                  /*  47 DMA1 Stream7 */
    FMC_IRQHandler,                           /*  48 FMC */
    SDMMC1_IRQHandler,                        /*  49 SDMMC1 */
    TIM5_IRQHandler,                          /*  50 TIM5 */
    SPI3_IRQHandler,                          /*  51 SPI3 */
    UART4_IRQHandler,                         /*  52 UART4 */
    UART5_IRQHandler,                         /*  53 UART5 */
    TIM6_DAC_IRQHandler,                      /*  54 TIM6 and DAC1&2 underrun errors */
    TIM7_IRQHandler,                          /*  55 TIM7 */
    DMA2_Stream0_IRQHandler,                  /*  56 DMA2 Stream0 */
    DMA2_Stream1_IRQHandler,                  /*  57 DMA2 Stream1 */
    DMA2_Stream2_IRQHandler,                  /*  58 DMA2 Stream2 */
    DMA2_Stream3_IRQHandler,                  /*  59 DMA2 Stream3 */
    DMA2_Stream4_IRQHandler,                  /*  60 DMA2 Stream4 */
    ETH_IRQHandler,                           /*  61 Ethernet */
    ETH_WKUP_IRQHandler,                      /*  62 Ethernet wakeup */
    SWI0_IRQHandler,                          /*  63 SWI0 */
    SWI1_IRQHandler,                          /*  64 SWI1 */
    SWI2_IRQHandler,                          /*  65 SWI2 */
    SWI3_IRQHandler,                          /*  66 SWI3 */
    OTG_FS_IRQHandler,                        /*  67 USB OTG FS */
    DMA2_Stream5_IRQHandler,                  /*  68 DMA2 Stream5 */
    DMA2_Stream6_IRQHandler,                  /*  69 DMA2 Stream6 */
    DMA2_Stream7_IRQHandler,                  /*  70 DMA2 Stream7 */
    USART6_IRQHandler,                        /*  71 USART6 */
    I2C3_EV_IRQHandler,                       /*  72 I2C3 event */
    I2C3_ER_IRQHandler,                       /*  73 I2C3 error */
    OTG_HS_EP1_OUT_IRQHandler,                /*  74 USB OTG HS End Point 1 Out */
    OTG_HS_EP1_IN_IRQHandler,                 /*  75 USB OTG HS End Point 1 In */
    OTG_HS_WKUP_IRQHandler,                   /*  76 USB OTG HS Wakeup through EXTI */
    OTG_HS_IRQHandler,                        /*  77 USB OTG HS */
    SWI4_IRQHandler,                          /*  78 SWI4 */
    AES_IRQHandler,                           /*  79 AES */
    RNG_IRQHandler,                           /*  80 RNG */
    FPU_IRQHandler,                           /*  81 FPU */
    UART7_IRQHandler,                         /*  82 UART7 */
    UART8_IRQHandler,                         /*  83 UART8 */
    SPI4_IRQHandler,                          /*  84 SPI4 */
    SPI5_IRQHandler,                          /*  85 SPI5 */
    SWI5_IRQHandler,                          /*  86 SWI5 */
    SAI1_IRQHandler,                          /*  87 SAI */
    SWI6_IRQHandler,                          /*  88 SWI6 */
    SWI7_IRQHandler,                          /*  89 SWI7 */
    SWI8_IRQHandler,                          /*  90 SWI8 */
    SAI2_IRQHandler,                          /*  91 SAI2 */
    QUADSPI_IRQHandler,                       /*  92 QuadSPI */
    LPTIM1_IRQHandler,                        /*  93 LP Timer1 */
    SWI9_IRQHandler,                          /*  94 SWI9 */
    SWI10_IRQHandler,                         /*  95 SWI10 */
    SWI11_IRQHandler,                         /*  96 SWI11 */
    SWI12_IRQHandler,                         /*  97 SWI12 */
    0, 0, 0, 0, 0,
    SDMMC2_IRQHandler                         /*  103 SDMMC2 */
};

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
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
