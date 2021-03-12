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

WEAK(BOD_IRQHandler) 
void BOD_IRQHandler         (void){} 

WEAK(IRC_IRQHandler) 
void IRC_IRQHandler         (void){} 

WEAK(PWRWU_IRQHandler) 
void PWRWU_IRQHandler       (void){} 

WEAK(RAMPE_IRQHandler) 
void RAMPE_IRQHandler       (void){} 

WEAK(CKFAIL_IRQHandler) 
void CKFAIL_IRQHandler      (void){} 

WEAK(RTC_IRQHandler) 
void RTC_IRQHandler         (void){} 

WEAK(TAMPER_IRQHandler) 
void TAMPER_IRQHandler      (void){} 

WEAK(WDT_IRQHandler) 
void WDT_IRQHandler         (void){} 

WEAK(WWDT_IRQHandler) 
void WWDT_IRQHandler        (void){} 

WEAK(EINT0_IRQHandler) 
void EINT0_IRQHandler       (void){} 

WEAK(EINT1_IRQHandler) 
void EINT1_IRQHandler       (void){} 

WEAK(EINT2_IRQHandler) 
void EINT2_IRQHandler       (void){} 

WEAK(EINT3_IRQHandler) 
void EINT3_IRQHandler       (void){} 

WEAK(EINT4_IRQHandler) 
void EINT4_IRQHandler       (void){} 

WEAK(EINT5_IRQHandler) 
void EINT5_IRQHandler       (void){} 

WEAK(EINT6_IRQHandler) 
void EINT6_IRQHandler       (void){} 

WEAK(EINT7_IRQHandler) 
void EINT7_IRQHandler       (void){} 

WEAK(GPA_IRQHandler) 
void GPA_IRQHandler         (void){} 

WEAK(GPB_IRQHandler) 
void GPB_IRQHandler         (void){} 

WEAK(GPC_IRQHandler) 
void GPC_IRQHandler         (void){} 

WEAK(GPD_IRQHandler) 
void GPD_IRQHandler         (void){} 

WEAK(GPE_IRQHandler) 
void GPE_IRQHandler         (void){} 

WEAK(GPF_IRQHandler) 
void GPF_IRQHandler         (void){} 

WEAK(GPG_IRQHandler) 
void GPG_IRQHandler         (void){} 

WEAK(GPH_IRQHandler) 
void GPH_IRQHandler         (void){} 

WEAK(QSPI0_IRQHandler) 
void QSPI0_IRQHandler       (void){} 

WEAK(SPI0_IRQHandler) 
void SPI0_IRQHandler        (void){} 

WEAK(SPI1_IRQHandler) 
void SPI1_IRQHandler        (void){} 

WEAK(SPI2_IRQHandler) 
void SPI2_IRQHandler        (void){} 

WEAK(SPI3_IRQHandler) 
void SPI3_IRQHandler        (void){} 

WEAK(BRAKE0_IRQHandler) 
void BRAKE0_IRQHandler      (void){}

WEAK(PWM0P0_IRQHandler) 
void PWM0P0_IRQHandler      (void){}

WEAK(PWM0P1_IRQHandler) 
void PWM0P1_IRQHandler      (void){}

WEAK(PWM0P2_IRQHandler) 
void PWM0P2_IRQHandler      (void){}

WEAK(BRAKE1_IRQHandler) 
void BRAKE1_IRQHandler      (void){}

WEAK(PWM1P0_IRQHandler) 
void PWM1P0_IRQHandler      (void){}

WEAK(PWM1P1_IRQHandler) 
void PWM1P1_IRQHandler      (void){}

WEAK(PWM1P2_IRQHandler) 
void PWM1P2_IRQHandler      (void){}


WEAK(TMR0_IRQHandler) 
void TMR0_IRQHandler        (void){}

WEAK(TMR1_IRQHandler) 
void TMR1_IRQHandler        (void){}

WEAK(TMR2_IRQHandler) 
void TMR2_IRQHandler        (void){}

WEAK(TMR3_IRQHandler) 
void TMR3_IRQHandler        (void){}

WEAK(UART0_IRQHandler) 
void UART0_IRQHandler       (void){}

WEAK(UART1_IRQHandler) 
void UART1_IRQHandler       (void){}

WEAK(UART2_IRQHandler) 
void UART2_IRQHandler       (void){}

WEAK(UART3_IRQHandler) 
void UART3_IRQHandler       (void){}

WEAK(UART4_IRQHandler) 
void UART4_IRQHandler       (void){}

WEAK(UART5_IRQHandler) 
void UART5_IRQHandler       (void){}

WEAK(I2C0_IRQHandler) 
void I2C0_IRQHandler        (void){}

WEAK(I2C1_IRQHandler) 
void I2C1_IRQHandler        (void){}

WEAK(I2C2_IRQHandler) 
void I2C2_IRQHandler        (void){}

WEAK(PDMA_IRQHandler) 
void PDMA_IRQHandler        (void){}

WEAK(DAC_IRQHandler) 
void DAC_IRQHandler         (void){}

WEAK(ADC00_IRQHandler) 
void ADC00_IRQHandler       (void){}

WEAK(ADC01_IRQHandler) 
void ADC01_IRQHandler       (void){}

WEAK(ACMP01_IRQHandler) 
void ACMP01_IRQHandler      (void){}

WEAK(ADC02_IRQHandler) 
void ADC02_IRQHandler       (void){}

WEAK(ADC03_IRQHandler) 
void ADC03_IRQHandler       (void){}

WEAK(USBD_IRQHandler) 
void USBD_IRQHandler        (void){}

WEAK(OHCI_IRQHandler) 
void OHCI_IRQHandler        (void){}

WEAK(USBOTG_IRQHandler) 
void USBOTG_IRQHandler      (void){}

WEAK(CAN0_IRQHandler) 
void CAN0_IRQHandler        (void){}

WEAK(CAN1_IRQHandler) 
void CAN1_IRQHandler        (void){}

WEAK(SC0_IRQHandler) 
void SC0_IRQHandler         (void){}

WEAK(SC1_IRQHandler) 
void SC1_IRQHandler         (void){}

WEAK(SC2_IRQHandler) 
void SC2_IRQHandler         (void){}

WEAK(SC3_IRQHandler) 
void SC3_IRQHandler         (void){}

WEAK(SDH0_IRQHandler) 
void SDH0_IRQHandler        (void){}

WEAK(SDH1_IRQHandler) 
void SDH1_IRQHandler        (void){}

WEAK(USBD20_IRQHandler) 
void USBD20_IRQHandler      (void){}

WEAK(EMAC_TX_IRQHandler) 
void EMAC_TX_IRQHandler     (void){}

WEAK(EMAC_RX_IRQHandler) 
void EMAC_RX_IRQHandler     (void){}

WEAK(I2S0_IRQHandler) 
void I2S0_IRQHandler        (void){}

WEAK(OPA0_IRQHandler) 
void OPA0_IRQHandler        (void){}

WEAK(CRYPTO_IRQHandler) 
void CRYPTO_IRQHandler      (void){}

WEAK(USCI0_IRQHandler) 
void USCI0_IRQHandler       (void){}

WEAK(USCI1_IRQHandler) 
void USCI1_IRQHandler       (void){}

WEAK(BPWM0_IRQHandler) 
void BPWM0_IRQHandler       (void){}

WEAK(BPWM1_IRQHandler) 
void BPWM1_IRQHandler       (void){}

WEAK(SPIM_IRQHandler) 
void SPIM_IRQHandler        (void){}

WEAK(QEI0_IRQHandler) 
void QEI0_IRQHandler        (void){}

WEAK(QEI1_IRQHandler) 
void QEI1_IRQHandler        (void){}

WEAK(ECAP0_IRQHandler) 
void ECAP0_IRQHandler       (void){}

WEAK(ECAP1_IRQHandler) 
void ECAP1_IRQHandler       (void){}

WEAK(EHCI_IRQHandler) 
void EHCI_IRQHandler        (void){}

WEAK(USBOTG20_IRQHandler) 
void USBOTG20_IRQHandler    (void){}

WEAK(SWI0_IRQHandler) 
void SWI0_IRQHandler        (void){}

WEAK(SWI1_IRQHandler) 
void SWI1_IRQHandler        (void){}

WEAK(SWI2_IRQHandler) 
void SWI2_IRQHandler        (void){}

WEAK(SWI3_IRQHandler) 
void SWI3_IRQHandler        (void){}

WEAK(SWI4_IRQHandler) 
void SWI4_IRQHandler        (void){}

WEAK(SWI5_IRQHandler) 
void SWI5_IRQHandler        (void){}

WEAK(SWI6_IRQHandler) 
void SWI6_IRQHandler        (void){}

WEAK(SWI7_IRQHandler) 
void SWI7_IRQHandler        (void){}

WEAK(SWI8_IRQHandler) 
void SWI8_IRQHandler        (void){}


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
    SC3_IRQHandler,                           /*  61 */
    SPI3_IRQHandler,                          /*  62 SPI3 */
    0,                                        /*  63 */
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

WEAK(vsf_hal_pre_startup_init) 
void vsf_hal_pre_startup_init(void)
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
