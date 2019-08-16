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
void __NO_RETURN Default_Handler(void);
void __NO_RETURN Reset_Handler  (void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

void BOD_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void IRC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void PWRWU_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void RAMPE_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void CKFAIL_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TAMPER_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void WDT_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void WWDT_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT4_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT5_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT6_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT7_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPA_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPB_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPD_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPE_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPF_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPG_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void GPH_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QSPI0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void BRAKE0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0P0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0P1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM0P2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BRAKE1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1P0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1P1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1P2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TMR0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TMR1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TMR2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void TMR3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDMA_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void DAC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC00_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC01_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ACMP01_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC02_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC03_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void USBD_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void OHCI_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USBOTG_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SC0_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SC1_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SC2_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SDH0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SDH1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USBD20_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void EMAC_TX_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void EMAC_RX_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void OPA0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CRYPTO_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void USCI0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void USCI1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void BPWM0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void BPWM1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SPIM_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void ECAP0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ECAP1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EHCI_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USBOTG20_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI4_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI5_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI6_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI7_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI8_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

const pFunc __VECTOR_TABLE[240] __VECTOR_TABLE_ATTRIBUTE  = {
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
    BOD_IRQHandler,                           /*   0 Brown Out detection */
    IRC_IRQHandler,                           /*   1 Internal RC */
    PWRWU_IRQHandler,                         /*   2 Power down wake up */
    RAMPE_IRQHandler,                         /*   3 RAM parity error */
    CKFAIL_IRQHandler,                        /*   4 Clock detection fail */
    SWI0_IRQHandler,                          /*   5 SWI0 */
    RTC_IRQHandler,                           /*   6 Real Time Clock */
    TAMPER_IRQHandler,                        /*   7 Tamper detection */
    WDT_IRQHandler,                           /*   8 Watchdog timer */
    WWDT_IRQHandler,                          /*   9 Window watchdog timer */
    EINT0_IRQHandler,                         /*  10 External Input 0 */
    EINT1_IRQHandler,                         /*  11 External Input 1 */
    EINT2_IRQHandler,                         /*  12 External Input 2 */
    EINT3_IRQHandler,                         /*  13 External Input 3 */
    EINT4_IRQHandler,                         /*  14 External Input 4 */
    EINT5_IRQHandler,                         /*  15 External Input 5 */
    GPA_IRQHandler,                           /*  16 GPIO Port A */
    GPB_IRQHandler,                           /*  17 GPIO Port B */
    GPC_IRQHandler,                           /*  18 GPIO Port C */
    GPD_IRQHandler,                           /*  19 GPIO Port D */
    GPE_IRQHandler,                           /*  20 GPIO Port E */
    GPF_IRQHandler,                           /*  21 GPIO Port F */
    QSPI0_IRQHandler,                         /*  22 QSPI0 */
    SPI0_IRQHandler,                          /*  23 SPI0 */
    BRAKE0_IRQHandler,                        /*  24 */
    PWM0P0_IRQHandler,                        /*  25 */
    PWM0P1_IRQHandler,                        /*  26 */
    PWM0P2_IRQHandler,                        /*  27 */
    BRAKE1_IRQHandler,                        /*  28 */
    PWM1P0_IRQHandler,                        /*  29 */
    PWM1P1_IRQHandler,                        /*  30 */
    PWM1P2_IRQHandler,                        /*  31 */
    TMR0_IRQHandler,                          /*  32 Timer 0 */
    TMR1_IRQHandler,                          /*  33 Timer 1 */
    TMR2_IRQHandler,                          /*  34 Timer 2 */
    TMR3_IRQHandler,                          /*  35 Timer 3 */
    UART0_IRQHandler,                         /*  36 UART0 */
    UART1_IRQHandler,                         /*  37 UART1 */
    I2C0_IRQHandler,                          /*  38 I2C0 */
    I2C1_IRQHandler,                          /*  39 I2C1 */
    PDMA_IRQHandler,                          /*  40 Peripheral DMA */
    DAC_IRQHandler,                           /*  41 DAC */
    ADC00_IRQHandler,                         /*  42 ADC0 interrupt source 0 */
    ADC01_IRQHandler,                         /*  43 ADC0 interrupt source 1 */
    ACMP01_IRQHandler,                        /*  44 ACMP0 and ACMP1 */
    SWI1_IRQHandler,                          /*  45 SWI1 */
    ADC02_IRQHandler,                         /*  46 ADC0 interrupt source 2 */
    ADC03_IRQHandler,                         /*  47 ADC0 interrupt source 3 */
    UART2_IRQHandler,                         /*  48 UART2 */
    UART3_IRQHandler,                         /*  49 UART3 */
    SWI2_IRQHandler,                          /*  50 SWI2 */
    SPI1_IRQHandler,                          /*  51 SPI1 */
    SPI2_IRQHandler,                          /*  52 SPI2 */
    USBD_IRQHandler,                          /*  53 USB device */
    OHCI_IRQHandler,                          /*  54 OHCI */
    USBOTG_IRQHandler,                        /*  55 USB OTG */
    CAN0_IRQHandler,                          /*  56 CAN0 */
    CAN1_IRQHandler,                          /*  57 CAN1 */
    SC0_IRQHandler,                           /*  58 */
    SC1_IRQHandler,                           /*  59 */
    SC2_IRQHandler,                           /*  60 */
    Default_Handler,                          /*  61 */
    SPI3_IRQHandler,                          /*  62 SPI3 */
    Default_Handler,                          /*  63 */
    SDH0_IRQHandler,                          /*  64 SDH0 */
    USBD20_IRQHandler,                        /*  65 USBD20 */
    EMAC_TX_IRQHandler,                       /*  66 EMAC_TX */
    EMAC_RX_IRQHandler,                       /*  67 EMAX_RX */
    I2S0_IRQHandler,                          /*  68 I2S0 */
    SWI3_IRQHandler,                          /*  69 SWI3 */
    OPA0_IRQHandler,                          /*  70 OPA0 */
    CRYPTO_IRQHandler,                        /*  71 CRYPTO */
    GPG_IRQHandler,                           /*  72 */
    EINT6_IRQHandler,                         /*  73 */
    UART4_IRQHandler,                         /*  74 UART4 */
    UART5_IRQHandler,                         /*  75 UART5 */
    USCI0_IRQHandler,                         /*  76 USCI0 */
    USCI1_IRQHandler,                         /*  77 USCI1 */
    BPWM0_IRQHandler,                         /*  78 BPWM0 */
    BPWM1_IRQHandler,                         /*  79 BPWM1 */
    SPIM_IRQHandler,                          /*  80 SPIM */
    SWI4_IRQHandler,                          /*  81 SWI4 */
    I2C2_IRQHandler,                          /*  82 I2C2 */
    SWI5_IRQHandler,                          /*  83 */
    QEI0_IRQHandler,                          /*  84 QEI0 */
    QEI1_IRQHandler,                          /*  85 QEI1 */
    ECAP0_IRQHandler,                         /*  86 ECAP0 */
    ECAP1_IRQHandler,                         /*  87 ECAP1 */
    GPH_IRQHandler,                           /*  88 */
    EINT7_IRQHandler,                         /*  89 */
    SDH1_IRQHandler,                          /*  90 SDH1 */
    SWI6_IRQHandler,                          /*  91 SWI6 */
    EHCI_IRQHandler,                          /*  92 EHCI */
    USBOTG20_IRQHandler,                      /*  93 */
    SWI7_IRQHandler,                          /*  94 SWI7 */
    SWI8_IRQHandler,                          /*  95 SWI8 */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

WEAK void vsf_hal_pre_startup_init(void)
{}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    bool orig = m480_reg_unlock();

    vsf_hal_pre_startup_init();

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;


#ifndef ENABLE_SPIM_CACHE
    CLK->AHBCLK |= CLK_AHBCLK_SPIMCKEN_Msk;
    SPIM->CTL1 |= SPIM_CTL1_CACHEOFF_Msk;     /* _SPIM_DISABLE_CACHE */
    SPIM->CTL1 |= SPIM_CTL1_CCMEN_Msk;        /* _SPIM_ENABLE_CCM */
#endif

    SYS->PORCTL = 0x00005AA5;

    m480_reg_lock(orig);

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    while(1);
}