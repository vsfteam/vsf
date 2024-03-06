/******************************************************************************
 * @file     startup_ARMCM4.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M4 Device
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

VSF_CAL_WEAK(WWDT_IRQHandler)
void WWDT_IRQHandler        (void){}

VSF_CAL_WEAK(PVD_IRQHandler)
void PVD_IRQHandler         (void){}

VSF_CAL_WEAK(TAMP_STAMP_IRQHandler)
void TAMP_STAMP_IRQHandler  (void){}

VSF_CAL_WEAK(RTC_WKUP_IRQHandler)
void RTC_WKUP_IRQHandler    (void){}

VSF_CAL_WEAK(FLASH_IRQHandler)
void FLASH_IRQHandler       (void){}

VSF_CAL_WEAK(RCM_IRQHandler)
void RCM_IRQHandler         (void){}

VSF_CAL_WEAK(EINT0_IRQHandler)
void EINT0_IRQHandler       (void){}

VSF_CAL_WEAK(EINT1_IRQHandler)
void EINT1_IRQHandler       (void){}

VSF_CAL_WEAK(EINT2_IRQHandler)
void EINT2_IRQHandler       (void){}

VSF_CAL_WEAK(EINT3_IRQHandler)
void EINT3_IRQHandler       (void){}

VSF_CAL_WEAK(EINT4_IRQHandler)
void EINT4_IRQHandler       (void){}

VSF_CAL_WEAK(DMA1_STR0_IRQHandler)
void DMA1_STR0_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR1_IRQHandler)
void DMA1_STR1_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR2_IRQHandler)
void DMA1_STR2_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR3_IRQHandler)
void DMA1_STR3_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR4_IRQHandler)
void DMA1_STR4_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR5_IRQHandler)
void DMA1_STR5_IRQHandler   (void){}

VSF_CAL_WEAK(DMA1_STR6_IRQHandler)
void DMA1_STR6_IRQHandler   (void){}

VSF_CAL_WEAK(ADC_IRQHandler)
void ADC_IRQHandler         (void){}

VSF_CAL_WEAK(CAN1_TX_IRQHandler)
void CAN1_TX_IRQHandler     (void){}

VSF_CAL_WEAK(CAN1_RX0_IRQHandler)
void CAN1_RX0_IRQHandler    (void){}

VSF_CAL_WEAK(CAN1_RX1_IRQHandler)
void CAN1_RX1_IRQHandler    (void){}

VSF_CAL_WEAK(CAN1_SCE_IRQHandler)
void CAN1_SCE_IRQHandler    (void){}

VSF_CAL_WEAK(EINT9_5_IRQHandler)
void EINT9_5_IRQHandler     (void){}

VSF_CAL_WEAK(TMR1_BRK_TMR9_IRQHandler)
void TMR1_BRK_TMR9_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_UP_TMR10_IRQHandler)
void TMR1_UP_TMR10_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_TRG_COM_TMR11_IRQHandler)
void TMR1_TRG_COM_TMR11_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_CC_IRQHandler)
void TMR1_CC_IRQHandler     (void){}

VSF_CAL_WEAK(TMR2_IRQHandler)
void TMR2_IRQHandler        (void){}

VSF_CAL_WEAK(TMR3_IRQHandler)
void TMR3_IRQHandler        (void){}

VSF_CAL_WEAK(TMR4_IRQHandler)
void TMR4_IRQHandler        (void){}

VSF_CAL_WEAK(I2C1_EV_IRQHandler)
void I2C1_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C1_ER_IRQHandler)
void I2C1_ER_IRQHandler     (void){}

VSF_CAL_WEAK(I2C2_EV_IRQHandler)
void I2C2_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C2_ER_IRQHandler)
void I2C2_ER_IRQHandler     (void){}

VSF_CAL_WEAK(SPI1_IRQHandler)
void SPI1_IRQHandler        (void){}

VSF_CAL_WEAK(SPI2_IRQHandler)
void SPI2_IRQHandler        (void){}

VSF_CAL_WEAK(USART1_IRQHandler)
void USART1_IRQHandler      (void){}

VSF_CAL_WEAK(USART2_IRQHandler)
void USART2_IRQHandler      (void){}

VSF_CAL_WEAK(USART3_IRQHandler)
void USART3_IRQHandler      (void){}

VSF_CAL_WEAK(EINT15_10_IRQHandler)
void EINT15_10_IRQHandler   (void){}

VSF_CAL_WEAK(RTC_Alarm_IRQHandler)
void RTC_Alarm_IRQHandler   (void){}

VSF_CAL_WEAK(OTG_FS_WKUP_IRQHandler)
void OTG_FS_WKUP_IRQHandler (void){}

VSF_CAL_WEAK(TMR8_BRK_TMR12_IRQHandler)
void TMR8_BRK_TMR12_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_UP_TMR13_IRQHandler)
void TMR8_UP_TMR13_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_TRG_COM_TMR14_IRQHandler)
void TMR8_TRG_COM_TMR14_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_CC_IRQHandler)
void TMR8_CC_IRQHandler     (void){}

VSF_CAL_WEAK(DMA1_STR7_IRQHandler)
void DMA1_STR7_IRQHandler   (void){}

VSF_CAL_WEAK(EMMC_IRQHandler)
void EMMC_IRQHandler        (void){}

VSF_CAL_WEAK(SDIO_IRQHandler)
void SDIO_IRQHandler        (void){}

VSF_CAL_WEAK(TMR5_IRQHandler)
void TMR5_IRQHandler        (void){}

VSF_CAL_WEAK(SPI3_IRQHandler)
void SPI3_IRQHandler        (void){}

VSF_CAL_WEAK(UART4_IRQHandler)
void UART4_IRQHandler       (void){}

VSF_CAL_WEAK(UART5_IRQHandler)
void UART5_IRQHandler       (void){}

VSF_CAL_WEAK(TMR6_DAC_IRQHandler)
void TMR6_DAC_IRQHandler    (void){}

VSF_CAL_WEAK(TMR7_IRQHandler)
void TMR7_IRQHandler        (void){}

VSF_CAL_WEAK(DMA2_STR0_IRQHandler)
void DMA2_STR0_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR1_IRQHandler)
void DMA2_STR1_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR2_IRQHandler)
void DMA2_STR2_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR3_IRQHandler)
void DMA2_STR3_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR4_IRQHandler)
void DMA2_STR4_IRQHandler   (void){}

VSF_CAL_WEAK(ETH_IRQHandler)
void ETH_IRQHandler         (void){}

VSF_CAL_WEAK(ETH_WKUP_IRQHandler)
void ETH_WKUP_IRQHandler    (void){}

VSF_CAL_WEAK(CAN2_TX_IRQHandler)
void CAN2_TX_IRQHandler     (void){}

VSF_CAL_WEAK(CAN2_RX0_IRQHandler)
void CAN2_RX0_IRQHandler    (void){}

VSF_CAL_WEAK(CAN2_RX1_IRQHandler)
void CAN2_RX1_IRQHandler    (void){}

VSF_CAL_WEAK(CAN2_SCE_IRQHandler)
void CAN2_SCE_IRQHandler    (void){}

VSF_CAL_WEAK(OTG_FS_IRQHandler)
void OTG_FS_IRQHandler      (void){}

VSF_CAL_WEAK(DMA2_STR5_IRQHandler)
void DMA2_STR5_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR6_IRQHandler)
void DMA2_STR6_IRQHandler   (void){}

VSF_CAL_WEAK(DMA2_STR7_IRQHandler)
void DMA2_STR7_IRQHandler   (void){}

VSF_CAL_WEAK(USART6_IRQHandler)
void USART6_IRQHandler      (void){}

VSF_CAL_WEAK(I2C3_EV_IRQHandler)
void I2C3_EV_IRQHandler     (void){}

VSF_CAL_WEAK(I2C3_ER_IRQHandler)
void I2C3_ER_IRQHandler     (void){}

VSF_CAL_WEAK(OTG_HS1_EP1_OUT_IRQHandler)
void OTG_HS1_EP1_OUT_IRQHandler(void){}

VSF_CAL_WEAK(OTG_HS1_EP1_IN_IRQHandler)
void OTG_HS1_EP1_IN_IRQHandler(void){}

VSF_CAL_WEAK(OTG_HS1_WKUP_IRQHandler)
void OTG_HS1_WKUP_IRQHandler(void){}

VSF_CAL_WEAK(OTG_HS1_IRQHandler)
void OTG_HS1_IRQHandler     (void){}

VSF_CAL_WEAK(DCI_IRQHandler)
void DCI_IRQHandler         (void){}

VSF_CAL_WEAK(CRYPT_IRQHandler)
void CRYPT_IRQHandler       (void){}

VSF_CAL_WEAK(HASH_RNG_IRQHandler)
void HASH_RNG_IRQHandler    (void){}

VSF_CAL_WEAK(FPU_IRQHandler)
void FPU_IRQHandler         (void){}

VSF_CAL_WEAK(SM3_IRQHandler)
void SM3_IRQHandler         (void){}

VSF_CAL_WEAK(SM4_IRQHandler)
void SM4_IRQHandler         (void){}

VSF_CAL_WEAK(BN_IRQHandler)
void BN_IRQHandler          (void){}


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

VSF_CAL_ROOT const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
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
    WWDT_IRQHandler,                          /*   0 Window WatchDog */
    PVD_IRQHandler,                           /*   1 PVD through EINT Line detection */
    TAMP_STAMP_IRQHandler,                    /*   2 Tamper and TimeStamps through the EINT line */
    RTC_WKUP_IRQHandler,                      /*   3 RTC Wakeup through the EINT line */
    FLASH_IRQHandler,                         /*   4 FLASH */
    RCM_IRQHandler,                           /*   5 RCM */
    EINT0_IRQHandler,                         /*   6 EINT Line0 */
    EINT1_IRQHandler,                         /*   7 EINT Line1 */
    EINT2_IRQHandler,                         /*   8 EINT Line2 */
    EINT3_IRQHandler,                         /*   9 EINT Line3 */
    EINT4_IRQHandler,                         /*  10 EINT Line4 */
    DMA1_STR0_IRQHandler,                     /*  11 DMA1 Stream 0 */
    DMA1_STR1_IRQHandler,                     /*  12 DMA1 Stream 1 */
    DMA1_STR2_IRQHandler,                     /*  13 DMA1 Stream 2 */
    DMA1_STR3_IRQHandler,                     /*  14 DMA1 Stream 3 */
    DMA1_STR4_IRQHandler,                     /*  15 DMA1 Stream 4 */
    DMA1_STR5_IRQHandler,                     /*  16 DMA1 Stream 5 */
    DMA1_STR6_IRQHandler,                     /*  17 DMA1 Stream 6 */
    ADC_IRQHandler,                           /*  18 ADC1, ADC2 and ADC3s */
    CAN1_TX_IRQHandler,                       /*  19 CAN1 TX */
    CAN1_RX0_IRQHandler,                      /*  20 CAN1 RX0 */
    CAN1_RX1_IRQHandler,                      /*  21 CAN1 RX1 */
    CAN1_SCE_IRQHandler,                      /*  22 CAN1 SCE */
    EINT9_5_IRQHandler,                       /*  23 External Line[9:5]s */
    TMR1_BRK_TMR9_IRQHandler,                 /*  24 TMR1 Break and TMR9 */
    TMR1_UP_TMR10_IRQHandler,                 /*  25 TMR1 Update and TMR10 */
    TMR1_TRG_COM_TMR11_IRQHandler,            /*  26 TMR1 Trigger and Commutation and TMR11 */
    TMR1_CC_IRQHandler,                       /*  27 TMR1 Capture Compare */
    TMR2_IRQHandler,                          /*  28 TMR2 */
    TMR3_IRQHandler,                          /*  29 TMR3 */
    TMR4_IRQHandler,                          /*  30 TMR4 */
    I2C1_EV_IRQHandler,                       /*  31 I2C1 Event */
    I2C1_ER_IRQHandler,                       /*  32 I2C1 Error */
    I2C2_EV_IRQHandler,                       /*  33 I2C2 Event */
    I2C2_ER_IRQHandler,                       /*  34 I2C2 Error */
    SPI1_IRQHandler,                          /*  35 SPI1 */
    SPI2_IRQHandler,                          /*  36 SPI2 */
    USART1_IRQHandler,                        /*  37 USART1 */
    USART2_IRQHandler,                        /*  38 USART2 */
    USART3_IRQHandler,                        /*  39 USART3 */
    EINT15_10_IRQHandler,                     /*  40 External Line[15:10]s */
    RTC_Alarm_IRQHandler,                     /*  41 RTC Alarm (A and B) through EINT Line */
    OTG_FS_WKUP_IRQHandler,                   /*  42 OTG_FS Wakeup through EINT line */
    TMR8_BRK_TMR12_IRQHandler,                /*  43 TMR8 Break and TMR12 */
    TMR8_UP_TMR13_IRQHandler,                 /*  44 TMR8 Update and TMR13 */
    TMR8_TRG_COM_TMR14_IRQHandler,            /*  45 TMR8 Trigger and Commutation and TMR14 */
    TMR8_CC_IRQHandler,                       /*  46 TMR8 Capture Compare */
    DMA1_STR7_IRQHandler,                     /*  47 DMA1 Stream 7 */
    EMMC_IRQHandler,                          /*  48 EMMC */
    SDIO_IRQHandler,                          /*  49 SDIO */
    TMR5_IRQHandler,                          /*  50 TMR5 */
    SPI3_IRQHandler,                          /*  51 SPI3 */
    UART4_IRQHandler,                         /*  52 UART4 */
    UART5_IRQHandler,                         /*  53 UART5 */
    TMR6_DAC_IRQHandler,                      /*  54 TMR6 and DAC1&2 underrun errors */
    TMR7_IRQHandler,                          /*  55 TMR7 */
    DMA2_STR0_IRQHandler,                     /*  56 DMA2 Stream 0 */
    DMA2_STR1_IRQHandler,                     /*  57 DMA2 Stream 1 */
    DMA2_STR2_IRQHandler,                     /*  58 DMA2 Stream 2 */
    DMA2_STR3_IRQHandler,                     /*  59 DMA2 Stream 3 */
    DMA2_STR4_IRQHandler,                     /*  60 DMA2 Stream 4 */
    ETH_IRQHandler,                           /*  61 Ethernet */
    ETH_WKUP_IRQHandler,                      /*  62 Ethernet Wakeup through EINT line */
    CAN2_TX_IRQHandler,                       /*  63 CAN2 TX */
    CAN2_RX0_IRQHandler,                      /*  64 CAN2 RX0 */
    CAN2_RX1_IRQHandler,                      /*  65 CAN2 RX1 */
    CAN2_SCE_IRQHandler,                      /*  66 CAN2 SCE */
    OTG_FS_IRQHandler,                        /*  67 OTG_FS */
    DMA2_STR5_IRQHandler,                     /*  68 DMA2 Stream 5 */
    DMA2_STR6_IRQHandler,                     /*  69 DMA2 Stream 6 */
    DMA2_STR7_IRQHandler,                     /*  70 DMA2 Stream 7 */
    USART6_IRQHandler,                        /*  71 USART6 */
    I2C3_EV_IRQHandler,                       /*  72 I2C3 event */
    I2C3_ER_IRQHandler,                       /*  73 I2C3 error */
    OTG_HS1_EP1_OUT_IRQHandler,               /*  74 OTG_HS1 End Point 1 Out */
    OTG_HS1_EP1_IN_IRQHandler,                /*  75 OTG_HS1 End Point 1 In */
    OTG_HS1_WKUP_IRQHandler,                  /*  76 OTG_HS1 Wakeup through EINT */
    OTG_HS1_IRQHandler,                       /*  77 OTG_HS1 */
    DCI_IRQHandler,                           /*  78 DCI */
    CRYPT_IRQHandler,                         /*  79 CRYPT */
    HASH_RNG_IRQHandler,                      /*  80 HASH RNG */
    FPU_IRQHandler,                           /*  81 FPU */
    SM3_IRQHandler,                           /*  82 SM3 */
    SM4_IRQHandler,                           /*  83 SM4 */
    BN_IRQHandler,                            /*  84 BN */
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
    vsf_arch_set_stack((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();

    SystemInit();

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U));           /* enable CP11 Full Access */

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
