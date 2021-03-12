/******************************************************************************
 * @file     startup_mt071.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M0 Device
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

WEAK(SWI0_IRQHandler)
void SWI0_IRQHandler        (void){}

WEAK(SWI1_IRQHandler)
void SWI1_IRQHandler        (void){}

WEAK(SWI2_IRQHandler)
void SWI2_IRQHandler        (void){}

WEAK(RTC_IRQHandler)
void RTC_IRQHandler         (void){}

WEAK(USB_IRQHandler)
void USB_IRQHandler         (void){}

WEAK(USB_DMA_IRQHandler)
void USB_DMA_IRQHandler     (void){}

WEAK(FLASH_IRQHandler)
void FLASH_IRQHandler       (void){}

WEAK(USART0_IRQHandler)
void USART0_IRQHandler      (void){}

WEAK(USART1_IRQHandler)
void USART1_IRQHandler      (void){}

WEAK(I2C0_IRQHandler)
void I2C0_IRQHandler        (void){}

WEAK(SPI0_IRQHandler)
void SPI0_IRQHandler        (void){}

WEAK(TIMER0_IRQHandler)
void TIMER0_IRQHandler      (void){}

WEAK(TIMER1_IRQHandler)
void TIMER1_IRQHandler      (void){}

WEAK(MCPWM_IRQHandler)
void MCPWM_IRQHandler       (void){}

WEAK(DMA_IRQHandler)
void DMA_IRQHandler         (void){}

WEAK(GPIO0_TO_GPIO3_IRQHandler)
void GPIO0_TO_GPIO3_IRQHandler(void){}

WEAK(ADC_IRQHandler)
void ADC_IRQHandler         (void){}

WEAK(ACMP_IRQHandler)
void ACMP_IRQHandler        (void){}

WEAK(BOD_IRQHandler)
void BOD_IRQHandler         (void){}

WEAK(BOR_IRQHandler)
void BOR_IRQHandler         (void){}

WEAK(GPIO4_TO_GPIO6_IRQHandler)
void GPIO4_TO_GPIO6_IRQHandler(void){}

WEAK(DAC_IRQHandler)
void DAC_IRQHandler         (void){}

WEAK(I2C1_IRQHandler)
void I2C1_IRQHandler        (void){}

WEAK(TSC_IRQHandler)
void TSC_IRQHandler         (void){}

WEAK(SPI1_IRQHandler)
void SPI1_IRQHandler        (void){}

WEAK(TIMER2_IRQHandler)
void TIMER2_IRQHandler      (void){}

WEAK(TIMER3_IRQHandler)
void TIMER3_IRQHandler      (void){}

WEAK(CORDIC_IRQHandler)
void CORDIC_IRQHandler      (void){}

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
    SWI0_IRQHandler,                    /* 16:SWI0 */
    SWI1_IRQHandler,                    /* 17:SWI1 */
    SWI2_IRQHandler,                    /* 18:SWI2 */
    0,                                  /* 19: */
    0,                                  /* 20: */
    RTC_IRQHandler,                     /* 21:RTC */
    USB_IRQHandler,                     /* 22:USB */
    USB_DMA_IRQHandler,                 /* 23:USB_DMA */
    FLASH_IRQHandler,                   /* 24:Flash */
    USART0_IRQHandler,                  /* 25:USART0 */
    USART1_IRQHandler,                  /* 26:USART1 */
    I2C0_IRQHandler,                    /* 27:I2C0 */
    0,                                  /* 28: */
    SPI0_IRQHandler,                    /* 29:SPI0 */
    TIMER0_IRQHandler,                  /* 30:Timer0 */
    TIMER1_IRQHandler,                  /* 31:Timer1 */
    MCPWM_IRQHandler,                   /* 32:MCPWM */
    DMA_IRQHandler,                     /* 33:DMA */
    GPIO0_TO_GPIO3_IRQHandler,          /* 34:GPIO0_3 */
    ADC_IRQHandler,                     /* 35:ADC */
    ACMP_IRQHandler,                    /* 36:ACMP */
    BOD_IRQHandler,                     /* 37:BOD */
    BOR_IRQHandler,                     /* 38:BOR */
    GPIO4_TO_GPIO6_IRQHandler,          /* 39:GPIO4_6 */
    DAC_IRQHandler,                     /* 40:DAC */
    I2C1_IRQHandler,                    /* 41:I2C1 */
    TSC_IRQHandler,                     /* 42:TSC */
    SPI1_IRQHandler,                    /* 43:SPI1 */
    0,                                  /* 44: */
    TIMER2_IRQHandler,                  /* 45:Timer2 */
    TIMER3_IRQHandler,                  /* 46:Timer3 */
    CORDIC_IRQHandler,                  /* 47:CORDIC */
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
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
