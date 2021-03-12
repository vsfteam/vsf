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

WEAK(TAMPER_IRQHandler)
void TAMPER_IRQHandler      (void){}

WEAK(RTC_IRQHandler)
void RTC_IRQHandler         (void){}

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

WEAK(DMA1_Channel1_IRQHandler)
void DMA1_Channel1_IRQHandler(void){}

WEAK(DMA1_Channel2_IRQHandler)
void DMA1_Channel2_IRQHandler(void){}

WEAK(DMA1_Channel3_IRQHandler)
void DMA1_Channel3_IRQHandler(void){}

WEAK(DMA1_Channel4_IRQHandler)
void DMA1_Channel4_IRQHandler(void){}

WEAK(DMA1_Channel5_IRQHandler)
void DMA1_Channel5_IRQHandler(void){}

WEAK(DMA1_Channel6_IRQHandler)
void DMA1_Channel6_IRQHandler(void){}

WEAK(DMA1_Channel7_IRQHandler)
void DMA1_Channel7_IRQHandler(void){}

WEAK(ADC1_2_IRQHandler)
void ADC1_2_IRQHandler      (void){}

WEAK(USB_HP_CAN1_TX_IRQHandler)
void USB_HP_CAN1_TX_IRQHandler(void){}

WEAK(USB_LP_CAN1_RX0_IRQHandler)
void USB_LP_CAN1_RX0_IRQHandler(void){}

WEAK(CAN1_RX1_IRQHandler)
void CAN1_RX1_IRQHandler    (void){}

WEAK(CAN1_SCE_IRQHandler)
void CAN1_SCE_IRQHandler    (void){}

WEAK(EXTI9_5_IRQHandler)
void EXTI9_5_IRQHandler     (void){}

WEAK(TIM1_BRK_IRQHandler)
void TIM1_BRK_IRQHandler    (void){}

WEAK(TIM1_UP_IRQHandler)
void TIM1_UP_IRQHandler     (void){}

WEAK(TIM1_TRG_COM_IRQHandler)
void TIM1_TRG_COM_IRQHandler(void){}

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

WEAK(RTCAlarm_IRQHandler)
void RTCAlarm_IRQHandler    (void){}

WEAK(USBWakeUp_IRQHandler)
void USBWakeUp_IRQHandler   (void){}

WEAK(USBHD_IRQHandler)
void USBHD_IRQHandler       (void){}

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

ROOT const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
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
    WWDG_IRQHandler,                          /*   0 WWDG Interrupt */
    PVD_IRQHandler,                           /*   1 PVD Interrupt */
    TAMPER_IRQHandler,                        /*   2 TAMPER Interrupt */
    RTC_IRQHandler,                           /*   3 RTC Interrupt */
    FLASH_IRQHandler,                         /*   4 Flash Interrupt */
    RCC_IRQHandler,                           /*   5 RCC Interrupt */
    EXTI0_IRQHandler,                         /*   6 EXTI0 */
    EXTI1_IRQHandler,                         /*   7 EXTI1 */
    EXTI2_IRQHandler,                         /*   8 EXTI2 */
    EXTI3_IRQHandler,                         /*   9 EXTI3 */
    EXTI4_IRQHandler,                         /*  10 EXTI4 */
    DMA1_Channel1_IRQHandler,                 /*  11 DMA Channel1 Interrupt */
    DMA1_Channel2_IRQHandler,                 /*  12 DMA Channel2 Interrupt */
    DMA1_Channel3_IRQHandler,                 /*  13 DMA Channel3 Interrupt */
    DMA1_Channel4_IRQHandler,                 /*  14 DMA Channel4 Interrupt */
    DMA1_Channel5_IRQHandler,                 /*  15 DMA Channel5 Interrupt */
    DMA1_Channel6_IRQHandler,                 /*  16 DMA Channel6 Interrupt */
    DMA1_Channel7_IRQHandler,                 /*  17 DMA Channel7 Interrupt */
    ADC1_2_IRQHandler,                        /*  18 ADC1_2 Interrupt */
    USB_HP_CAN1_TX_IRQHandler,                /*  19 USB_HP_CAN1_TX Interrupt */
    USB_LP_CAN1_RX0_IRQHandler,               /*  20 USB_LP_CAN1_RX0 Interrupt */
    CAN1_RX1_IRQHandler,                      /*  21 CAN1_RX1 Interrupt */
    CAN1_SCE_IRQHandler,                      /*  22 CAN1_SCE Interrupt*/
    EXTI9_5_IRQHandler,                       /*  23 EXIT5_9 Interrupt */
    TIM1_BRK_IRQHandler,                      /*  24 TIM1_BRK Interrupt */
    TIM1_UP_IRQHandler,                       /*  25 TIM1_UP Interrupt */
    TIM1_TRG_COM_IRQHandler,                  /*  26 TIM1_TRG_COM Interrupt */
    TIM1_CC_IRQHandler,                       /*  27 TIM1_CC Interrupt */
    TIM2_IRQHandler,                          /*  28 TIM2 Interrupt */
    TIM3_IRQHandler,                          /*  29 TIM3 Interrupt */
    TIM4_IRQHandler,                          /*  30 TIM4 Interrupt */
    I2C1_EV_IRQHandler,                       /*  31 I2C1_EV Interrupt */
    I2C1_ER_IRQHandler,                       /*  32 I2C1_ER Interrupt */
    I2C2_EV_IRQHandler,                       /*  33 I2C2_EV Interrupt */
    I2C2_ER_IRQHandler,                       /*  34 I2C2_ER Interrupt */
    SPI1_IRQHandler,                          /*  35 SPI1 Interrupt */
    SPI2_IRQHandler,                          /*  36 SPI2 Interrupt */
    USART1_IRQHandler,                        /*  37 USART1 Interrupt */
    USART2_IRQHandler,                        /*  38 USART2 Interrupt */
    USART3_IRQHandler,                        /*  39 USART3 Interrupt */
    EXTI15_10_IRQHandler,                     /*  40 EXTI10_15 Interrupt */
    RTCAlarm_IRQHandler,                      /*  41 RTCAlarm Interrupt */
    USBWakeUp_IRQHandler,                     /*  42 USBWakeUp Interrupt */
    USBHD_IRQHandler,                         /*  43 USBHD Interrupt */
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

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
