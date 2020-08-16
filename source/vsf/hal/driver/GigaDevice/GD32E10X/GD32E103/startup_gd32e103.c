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

WEAK(ADC0_1_IRQHandler)
void ADC0_1_IRQHandler (void){}

WEAK(CAN0_EWMC_IRQHandler)
void CAN0_EWMC_IRQHandler (void){}

WEAK(CAN0_RX0_IRQHandler)
void CAN0_RX0_IRQHandler (void){}

WEAK(CAN0_RX1_IRQHandler)
void CAN0_RX1_IRQHandler (void){}

WEAK(CAN0_TX_IRQHandler)
void CAN0_TX_IRQHandler (void){}

WEAK(CAN1_EWMC_IRQHandler)
void CAN1_EWMC_IRQHandler (void){}

WEAK(CAN1_RX0_IRQHandler)
void CAN1_RX0_IRQHandler (void){}

WEAK(CAN1_RX1_IRQHandler)
void CAN1_RX1_IRQHandler (void){}

WEAK(CAN1_TX_IRQHandler)
void CAN1_TX_IRQHandler (void){}

WEAK(DMA0_Channel0_IRQHandler)
void DMA0_Channel0_IRQHandler (void){}

WEAK(DMA0_Channel1_IRQHandler)
void DMA0_Channel1_IRQHandler (void){}

WEAK(DMA0_Channel2_IRQHandler)
void DMA0_Channel2_IRQHandler (void){}

WEAK(DMA0_Channel3_IRQHandler)
void DMA0_Channel3_IRQHandler (void){}

WEAK(DMA0_Channel4_IRQHandler)
void DMA0_Channel4_IRQHandler (void){}

WEAK(DMA0_Channel5_IRQHandler)
void DMA0_Channel5_IRQHandler (void){}

WEAK(DMA0_Channel6_IRQHandler)
void DMA0_Channel6_IRQHandler (void){}

WEAK(DMA1_Channel0_IRQHandler)
void DMA1_Channel0_IRQHandler (void){}

WEAK(DMA1_Channel1_IRQHandler)
void DMA1_Channel1_IRQHandler (void){}

WEAK(DMA1_Channel2_IRQHandler)
void DMA1_Channel2_IRQHandler (void){}

WEAK(DMA1_Channel3_IRQHandler)
void DMA1_Channel3_IRQHandler (void){}

WEAK(DMA1_Channel4_IRQHandler)
void DMA1_Channel4_IRQHandler (void){}

WEAK(EXMC_IRQHandler)
void EXMC_IRQHandler (void){}

WEAK(EXTI0_IRQHandler)
void EXTI0_IRQHandler (void){}

WEAK(EXTI1_IRQHandler)
void EXTI1_IRQHandler (void){}

WEAK(EXTI10_15_IRQHandler)
void EXTI10_15_IRQHandler (void){}

WEAK(EXTI2_IRQHandler)
void EXTI2_IRQHandler (void){}

WEAK(EXTI3_IRQHandler)
void EXTI3_IRQHandler (void){}

WEAK(EXTI4_IRQHandler)
void EXTI4_IRQHandler (void){}

WEAK(EXTI5_9_IRQHandler)
void EXTI5_9_IRQHandler (void){}

WEAK(FMC_IRQHandler)
void FMC_IRQHandler (void){}

WEAK(I2C0_ER_IRQHandler)
void I2C0_ER_IRQHandler (void){}

WEAK(I2C0_EV_IRQHandler)
void I2C0_EV_IRQHandler (void){}

WEAK(I2C1_ER_IRQHandler)
void I2C1_ER_IRQHandler (void){}

WEAK(I2C1_EV_IRQHandler)
void I2C1_EV_IRQHandler (void){}

WEAK(LVD_IRQHandler)
void LVD_IRQHandler (void){}

WEAK(RCU_CTC_IRQHandler)
void RCU_CTC_IRQHandler (void){}

WEAK(RTC_Alarm_IRQHandler)
void RTC_Alarm_IRQHandler (void){}

WEAK(RTC_IRQHandler)
void RTC_IRQHandler (void){}

WEAK(SPI0_IRQHandler)
void SPI0_IRQHandler (void){}

WEAK(SPI1_IRQHandler)
void SPI1_IRQHandler (void){}

WEAK(SPI2_IRQHandler)
void SPI2_IRQHandler (void){}

WEAK(SWI0_IRQHandler)
void SWI0_IRQHandler (void){}

WEAK(SWI1_IRQHandler)
void SWI1_IRQHandler (void){}

WEAK(SWI2_IRQHandler)
void SWI2_IRQHandler (void){}

WEAK(SWI3_IRQHandler)
void SWI3_IRQHandler (void){}

WEAK(SWI4_IRQHandler)
void SWI4_IRQHandler (void){}

WEAK(TAMPER_IRQHandler)
void TAMPER_IRQHandler (void){}

WEAK(TIMER0_BRK_TIMER8_IRQHandler)
void TIMER0_BRK_TIMER8_IRQHandler (void){}

WEAK(TIMER0_Channel_IRQHandler)
void TIMER0_Channel_IRQHandler (void){}

WEAK(TIMER0_TRG_CMT_TIMER10_IRQHandler)
void TIMER0_TRG_CMT_TIMER10_IRQHandler (void){}

WEAK(TIMER0_UP_TIMER9_IRQHandler)
void TIMER0_UP_TIMER9_IRQHandler (void){}

WEAK(TIMER1_IRQHandler)
void TIMER1_IRQHandler (void){}

WEAK(TIMER2_IRQHandler)
void TIMER2_IRQHandler (void){}

WEAK(TIMER3_IRQHandler)
void TIMER3_IRQHandler (void){}

WEAK(TIMER4_IRQHandler)
void TIMER4_IRQHandler (void){}

WEAK(TIMER5_IRQHandler)
void TIMER5_IRQHandler (void){}

WEAK(TIMER6_IRQHandler)
void TIMER6_IRQHandler (void){}

WEAK(TIMER7_BRK_TIMER11_IRQHandler)
void TIMER7_BRK_TIMER11_IRQHandler (void){}

WEAK(TIMER7_Channel_IRQHandler)
void TIMER7_Channel_IRQHandler (void){}

WEAK(TIMER7_TRG_CMT_TIMER13_IRQHandler)
void TIMER7_TRG_CMT_TIMER13_IRQHandler (void){}

WEAK(TIMER7_UP_TIMER12_IRQHandler)
void TIMER7_UP_TIMER12_IRQHandler (void){}

WEAK(UART3_IRQHandler)
void UART3_IRQHandler (void){}

WEAK(UART4_IRQHandler)
void UART4_IRQHandler (void){}

WEAK(USART0_IRQHandler)
void USART0_IRQHandler (void){}

WEAK(USART1_IRQHandler)
void USART1_IRQHandler (void){}

WEAK(USART2_IRQHandler)
void USART2_IRQHandler (void){}

WEAK(USBFS_IRQHandler)
void USBFS_IRQHandler (void){}

WEAK(USBFS_WKUP_IRQHandler)
void USBFS_WKUP_IRQHandler (void){}

WEAK(WWDGT_IRQHandler)
void WWDGT_IRQHandler (void){}

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
    WWDGT_IRQHandler,                   /* 16:Window Watchdog Timer */
    LVD_IRQHandler,                     /* 17:LVD through EXTI Line detect */
    TAMPER_IRQHandler,                  /* 18:Tamper through EXTI Line detect */
    RTC_IRQHandler,                     /* 19:RTC through EXTI Line */
    FMC_IRQHandler,                     /* 20:FMC */
    RCU_CTC_IRQHandler,                 /* 21:RCU and CTC */
    EXTI0_IRQHandler,                   /* 22:EXTI Line 0 */
    EXTI1_IRQHandler,                   /* 23:EXTI Line 1 */
    EXTI2_IRQHandler,                   /* 24:EXTI Line 2 */
    EXTI3_IRQHandler,                   /* 25:EXTI Line 3 */
    EXTI4_IRQHandler,                   /* 26:EXTI Line 4 */
    DMA0_Channel0_IRQHandler,           /* 27:DMA0 Channel0 */
    DMA0_Channel1_IRQHandler,           /* 28:DMA0 Channel1 */
    DMA0_Channel2_IRQHandler,           /* 29:DMA0 Channel2 */
    DMA0_Channel3_IRQHandler,           /* 30:DMA0 Channel3 */
    DMA0_Channel4_IRQHandler,           /* 31:DMA0 Channel4 */
    DMA0_Channel5_IRQHandler,           /* 32:DMA0 Channel5 */
    DMA0_Channel6_IRQHandler,           /* 33:DMA0 Channel6 */
    ADC0_1_IRQHandler,                  /* 34:ADC0 and ADC1 */
    CAN0_TX_IRQHandler,                 /* 35:CAN0 TX */
    CAN0_RX0_IRQHandler,                /* 36:CAN0 RX0 */
    CAN0_RX1_IRQHandler,                /* 37:CAN0 RX1 */
    CAN0_EWMC_IRQHandler,               /* 38:CAN0 EWMC */
    EXTI5_9_IRQHandler,                 /* 39:EXTI5 to EXTI9 */
    TIMER0_BRK_TIMER8_IRQHandler,       /* 40:TIMER0 Break and TIMER8 */
    TIMER0_UP_TIMER9_IRQHandler,        /* 41:TIMER0 Update and TIMER9 */
    TIMER0_TRG_CMT_TIMER10_IRQHandler,  /* 42:TIMER0 Trigger and Commutation and TIMER10 */
    TIMER0_Channel_IRQHandler,          /* 43:TIMER0 Channel Capture Compare */
    TIMER1_IRQHandler,                  /* 44:TIMER1 */
    TIMER2_IRQHandler,                  /* 45:TIMER2 */
    TIMER3_IRQHandler,                  /* 46:TIMER3 */
    I2C0_EV_IRQHandler,                 /* 47:I2C0 Event */
    I2C0_ER_IRQHandler,                 /* 48:I2C0 Error */
    I2C1_EV_IRQHandler,                 /* 49:I2C1 Event */
    I2C1_ER_IRQHandler,                 /* 50:I2C1 Error */
    SPI0_IRQHandler,                    /* 51:SPI0 */
    SPI1_IRQHandler,                    /* 52:SPI1 */
    USART0_IRQHandler,                  /* 53:USART0 */
    USART1_IRQHandler,                  /* 54:USART1 */
    USART2_IRQHandler,                  /* 55:USART2 */
    EXTI10_15_IRQHandler,               /* 56:EXTI10 to EXTI15 */
    RTC_Alarm_IRQHandler,               /* 57:RTC Alarm */
    USBFS_WKUP_IRQHandler,              /* 58:USBFS Wakeup */
    TIMER7_BRK_TIMER11_IRQHandler,      /* 59:TIMER7 Break and TIMER11 */
    TIMER7_UP_TIMER12_IRQHandler,       /* 60:TIMER7 Update and TIMER12 */
    TIMER7_TRG_CMT_TIMER13_IRQHandler,  /* 61:TIMER7 Trigger and Commutation and TIMER13 */
    TIMER7_Channel_IRQHandler,          /* 62:TIMER7 Channel Capture Compare */
    SWI0_IRQHandler,                    /* 63:Reserved */
    EXMC_IRQHandler,                    /* 64:EXMC */
    SWI1_IRQHandler,                    /* 65:Reserved */
    TIMER4_IRQHandler,                  /* 66:TIMER4 */
    SPI2_IRQHandler,                    /* 67:SPI2 */
    UART3_IRQHandler,                   /* 68:UART3 */
    UART4_IRQHandler,                   /* 69:UART4 */
    TIMER5_IRQHandler,                  /* 70:TIMER5  */
    TIMER6_IRQHandler,                  /* 71:TIMER6 */
    DMA1_Channel0_IRQHandler,           /* 72:DMA1 Channel0 */
    DMA1_Channel1_IRQHandler,           /* 73:DMA1 Channel1 */
    DMA1_Channel2_IRQHandler,           /* 74:DMA1 Channel2 */
    DMA1_Channel3_IRQHandler,           /* 75:DMA1 Channel3 */
    DMA1_Channel4_IRQHandler,           /* 76:DMA1 Channel4 */
    SWI2_IRQHandler,                    /* 77:Reserved */
    SWI3_IRQHandler,                    /* 78:Reserved */
    CAN1_TX_IRQHandler,                 /* 79:CAN1 TX */
    CAN1_RX0_IRQHandler,                /* 80:CAN1 RX0 */
    CAN1_RX1_IRQHandler,                /* 81:CAN1 RX1 */
    CAN1_EWMC_IRQHandler,               /* 82:CAN1 EWMC */
    USBFS_IRQHandler,                   /* 83:USBFS */
    SWI4_IRQHandler,                    /* 84:Reserved */
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
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  /* set CP10 and CP11 Full Access */

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
