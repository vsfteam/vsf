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
VSF_CAL_WEAK(NMI_Handler)
void NMI_Handler            (void){}

VSF_CAL_WEAK(HardFault_Handler)
void HardFault_Handler      (void){while(1);}

VSF_CAL_WEAK(MemManage_Handler)
void MemManage_Handler      (void){while(1);}

VSF_CAL_WEAK(BusFault_Handler)
void BusFault_Handler       (void){while(1);}

VSF_CAL_WEAK(UsageFault_Handler)
void UsageFault_Handler     (void){while(1);}

VSF_CAL_WEAK(SVC_Handler)
void SVC_Handler            (void){}

VSF_CAL_WEAK(DebugMon_Handler)
void DebugMon_Handler       (void){}

VSF_CAL_WEAK(PendSV_Handler)
void PendSV_Handler         (void){}

VSF_CAL_WEAK(SysTick_Handler)
void SysTick_Handler        (void){}

VSF_CAL_WEAK(PWR_IRQHandler)
void PWR_IRQHandler         (void){}

VSF_CAL_WEAK(WDT_IRQHandler)
void WDT_IRQHandler         (void){}

VSF_CAL_WEAK(APU_IRQHandler)
void APU_IRQHandler         (void){}

VSF_CAL_WEAK(I2S_IRQHandler)
void I2S_IRQHandler         (void){}

VSF_CAL_WEAK(EINT0_IRQHandler)
void EINT0_IRQHandler       (void){}

VSF_CAL_WEAK(EINT1_IRQHandler)
void EINT1_IRQHandler       (void){}

VSF_CAL_WEAK(EINT2_IRQHandler)
void EINT2_IRQHandler       (void){}

VSF_CAL_WEAK(EINT3_IRQHandler)
void EINT3_IRQHandler       (void){}

VSF_CAL_WEAK(SPIM_IRQHandler)
void SPIM_IRQHandler        (void){}

VSF_CAL_WEAK(USBD_IRQHandler)
void USBD_IRQHandler        (void){}

VSF_CAL_WEAK(TMR0_IRQHandler)
void TMR0_IRQHandler        (void){}

VSF_CAL_WEAK(TMR1_IRQHandler)
void TMR1_IRQHandler        (void){}

VSF_CAL_WEAK(TMR2_IRQHandler)
void TMR2_IRQHandler        (void){}

VSF_CAL_WEAK(TMR3_IRQHandler)
void TMR3_IRQHandler        (void){}

VSF_CAL_WEAK(SDH_IRQHandler)
void SDH_IRQHandler         (void){}

VSF_CAL_WEAK(PWM0_IRQHandler)
void PWM0_IRQHandler        (void){}

VSF_CAL_WEAK(PWM1_IRQHandler)
void PWM1_IRQHandler        (void){}

VSF_CAL_WEAK(PWM2_IRQHandler)
void PWM2_IRQHandler        (void){}

VSF_CAL_WEAK(PWM3_IRQHandler)
void PWM3_IRQHandler        (void){}

VSF_CAL_WEAK(RTC_IRQHandler)
void RTC_IRQHandler         (void){}

VSF_CAL_WEAK(SPI0_IRQHandler)
void SPI0_IRQHandler        (void){}

VSF_CAL_WEAK(I2C1_IRQHandler)
void I2C1_IRQHandler        (void){}

VSF_CAL_WEAK(I2C0_IRQHandler)
void I2C0_IRQHandler        (void){}

VSF_CAL_WEAK(UART0_IRQHandler)
void UART0_IRQHandler       (void){}

VSF_CAL_WEAK(UART1_IRQHandler)
void UART1_IRQHandler       (void){}

VSF_CAL_WEAK(ADC_IRQHandler)
void ADC_IRQHandler         (void){}

VSF_CAL_WEAK(WWDT_IRQHandler)
void WWDT_IRQHandler        (void){}

VSF_CAL_WEAK(USBH_IRQHandler)
void USBH_IRQHandler        (void){}

VSF_CAL_WEAK(UART2_IRQHandler)
void UART2_IRQHandler       (void){}

VSF_CAL_WEAK(LVD_IRQHandler)
void LVD_IRQHandler         (void){}

VSF_CAL_WEAK(SPI1_IRQHandler)
void SPI1_IRQHandler        (void){}

VSF_CAL_WEAK(SWI0_IRQHandler)
void SWI0_IRQHandler        (void){}

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
    PWR_IRQHandler,                           /*   0 Power On Interrupt */
    WDT_IRQHandler,                           /*   1 Watch Dog Interrupt */
    APU_IRQHandler,                           /*   2 Audio Process Unit Interrupt */
    I2S_IRQHandler,                           /*   3 I2S Interrupt */
    EINT0_IRQHandler,                         /*   4 External GPIO Group 0 */
    EINT1_IRQHandler,                         /*   5 External GPIO Group 1 */
    EINT2_IRQHandler,                         /*   6 External GPIO Group 2 */
    EINT3_IRQHandler,                         /*   7 External GPIO Group 3 */
    SPIM_IRQHandler,                          /*   8 SPIM Interrupt */
    USBD_IRQHandler,                          /*   9 USB Device 2.0 Interrupt */
    TMR0_IRQHandler,                          /*  10 Timer 0 Interrupt */
    TMR1_IRQHandler,                          /*  11 Timer 1 Interrupt */
    TMR2_IRQHandler,                          /*  12 Timer 2 Interrupt */
    TMR3_IRQHandler,                          /*  13 Timer 3 Interrupt */
    SDH_IRQHandler,                           /*  14 SD Host Interrupt */
    PWM0_IRQHandler,                          /*  15 PWM0 Interrupt */
    PWM1_IRQHandler,                          /*  16 PWM1 Interrupt */
    PWM2_IRQHandler,                          /*  17 PWM2 Interrupt */
    PWM3_IRQHandler,                          /*  18 PWM3 Interrupt */
    RTC_IRQHandler,                           /*  19 Real Time Clock Interrupt */
    SPI0_IRQHandler,                          /*  20 SPI0 Interrupt */
    I2C1_IRQHandler,                          /*  21 I2C1 Interrupt */
    I2C0_IRQHandler,                          /*  22 I2C0 Interrupt */
    UART0_IRQHandler,                         /*  23 UART0 Interrupt */
    UART1_IRQHandler,                         /*  24 UART1 Interrupt */
    ADC_IRQHandler,                           /*  25 ADC Interrupt */
    WWDT_IRQHandler,                          /*  26 Window Watch Dog Timer Interrupt */
    USBH_IRQHandler,                          /*  27 USB Host 1.1 Interrupt */
    UART2_IRQHandler,                         /*  28 UART2 Interrupt */
    LVD_IRQHandler,                           /*  29 Low Voltage Detection Interrupt */
    SPI1_IRQHandler,                          /*  30 SPI1 Interrupt */
    SWI0_IRQHandler,                          /*  31 SWI0 Interrupt */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |         /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
