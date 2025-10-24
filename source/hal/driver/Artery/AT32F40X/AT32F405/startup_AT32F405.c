/******************************************************************************
 * @file     startup_AT32F405.c
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

VSF_CAL_WEAK(PVM_IRQHandler)
void PVM_IRQHandler         (void){}

VSF_CAL_WEAK(TAMP_STAMP_IRQHandler)
void TAMP_STAMP_IRQHandler  (void){}

VSF_CAL_WEAK(ERTC_WKUP_IRQHandler)
void ERTC_WKUP_IRQHandler   (void){}

VSF_CAL_WEAK(FLASH_IRQHandler)
void FLASH_IRQHandler       (void){}

VSF_CAL_WEAK(CRM_IRQHandler)
void CRM_IRQHandler         (void){}

VSF_CAL_WEAK(EXINT0_IRQHandler)
void EXINT0_IRQHandler      (void){}

VSF_CAL_WEAK(EXINT1_IRQHandler)
void EXINT1_IRQHandler      (void){}

VSF_CAL_WEAK(EXINT2_IRQHandler)
void EXINT2_IRQHandler      (void){}

VSF_CAL_WEAK(EXINT3_IRQHandler)
void EXINT3_IRQHandler      (void){}

VSF_CAL_WEAK(EXINT4_IRQHandler)
void EXINT4_IRQHandler      (void){}

VSF_CAL_WEAK(DMA1_Channel1_IRQHandler)
void DMA1_Channel1_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel2_IRQHandler)
void DMA1_Channel2_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel3_IRQHandler)
void DMA1_Channel3_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel4_IRQHandler)
void DMA1_Channel4_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel5_IRQHandler)
void DMA1_Channel5_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel6_IRQHandler)
void DMA1_Channel6_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel7_IRQHandler)
void DMA1_Channel7_IRQHandler(void){}

VSF_CAL_WEAK(ADC1_IRQHandler)
void ADC1_IRQHandler        (void){}

VSF_CAL_WEAK(CAN1_TX_IRQHandler)
void CAN1_TX_IRQHandler     (void){}

VSF_CAL_WEAK(CAN1_RX0_IRQHandler)
void CAN1_RX0_IRQHandler    (void){}

VSF_CAL_WEAK(CAN1_RX1_IRQHandler)
void CAN1_RX1_IRQHandler    (void){}

VSF_CAL_WEAK(CAN1_SE_IRQHandler)
void CAN1_SE_IRQHandler     (void){}

VSF_CAL_WEAK(EXINT9_5_IRQHandler)
void EXINT9_5_IRQHandler    (void){}

VSF_CAL_WEAK(TMR1_BRK_TMR9_IRQHandler)
void TMR1_BRK_TMR9_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_OVF_TMR10_IRQHandler)
void TMR1_OVF_TMR10_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_TRG_HALL_TMR11_IRQHandler)
void TMR1_TRG_HALL_TMR11_IRQHandler(void){}

VSF_CAL_WEAK(TMR1_CH_IRQHandler)
void TMR1_CH_IRQHandler     (void){}

VSF_CAL_WEAK(TMR2_GLOBAL_IRQHandler)
void TMR2_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(TMR3_GLOBAL_IRQHandler)
void TMR3_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(TMR4_GLOBAL_IRQHandler)
void TMR4_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(I2C1_EVT_IRQHandler)
void I2C1_EVT_IRQHandler    (void){}

VSF_CAL_WEAK(I2C1_ERR_IRQHandler)
void I2C1_ERR_IRQHandler    (void){}

VSF_CAL_WEAK(I2C2_EVT_IRQHandler)
void I2C2_EVT_IRQHandler    (void){}

VSF_CAL_WEAK(I2C2_ERR_IRQHandler)
void I2C2_ERR_IRQHandler    (void){}

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

VSF_CAL_WEAK(EXINT15_10_IRQHandler)
void EXINT15_10_IRQHandler  (void){}

VSF_CAL_WEAK(ERTCAlarm_IRQHandler)
void ERTCAlarm_IRQHandler   (void){}

VSF_CAL_WEAK(OTGFS1_WKUP_IRQHandler)
void OTGFS1_WKUP_IRQHandler (void){}

VSF_CAL_WEAK(TMR13_GLOBAL_IRQHandler)
void TMR13_GLOBAL_IRQHandler(void){}

VSF_CAL_WEAK(TMR14_GLOBAL_IRQHandler)
void TMR14_GLOBAL_IRQHandler(void){}

VSF_CAL_WEAK(SPI3_IRQHandler)
void SPI3_IRQHandler        (void){}

VSF_CAL_WEAK(USART4_IRQHandler)
void USART4_IRQHandler      (void){}

VSF_CAL_WEAK(USART5_IRQHandler)
void USART5_IRQHandler      (void){}

VSF_CAL_WEAK(TMR6_GLOBAL_IRQHandler)
void TMR6_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(TMR7_GLOBAL_IRQHandler)
void TMR7_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(DMA2_Channel1_IRQHandler)
void DMA2_Channel1_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel2_IRQHandler)
void DMA2_Channel2_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel3_IRQHandler)
void DMA2_Channel3_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel4_IRQHandler)
void DMA2_Channel4_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel5_IRQHandler)
void DMA2_Channel5_IRQHandler(void){}

VSF_CAL_WEAK(OTGFS1_IRQHandler)
void OTGFS1_IRQHandler      (void){}

VSF_CAL_WEAK(DMA2_Channel6_IRQHandler)
void DMA2_Channel6_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel7_IRQHandler)
void DMA2_Channel7_IRQHandler(void){}

VSF_CAL_WEAK(USART6_IRQHandler)
void USART6_IRQHandler      (void){}

VSF_CAL_WEAK(I2C3_EVT_IRQHandler)
void I2C3_EVT_IRQHandler    (void){}

VSF_CAL_WEAK(I2C3_ERR_IRQHandler)
void I2C3_ERR_IRQHandler    (void){}

VSF_CAL_WEAK(OTGHS_EP1_OUT_IRQHandler)
void OTGHS_EP1_OUT_IRQHandler(void){}

VSF_CAL_WEAK(OTGHS_EP1_IN_IRQHandler)
void OTGHS_EP1_IN_IRQHandler(void){}

VSF_CAL_WEAK(OTGHS_WKUP_IRQHandler)
void OTGHS_WKUP_IRQHandler  (void){}

VSF_CAL_WEAK(OTGHS_IRQHandler)
void OTGHS_IRQHandler       (void){}

VSF_CAL_WEAK(FPU_IRQHandler)
void FPU_IRQHandler         (void){}

VSF_CAL_WEAK(UART7_IRQHandler)
void UART7_IRQHandler       (void){}

VSF_CAL_WEAK(UART8_IRQHandler)
void UART8_IRQHandler       (void){}

VSF_CAL_WEAK(I2SF5_IRQHandler)
void I2SF5_IRQHandler       (void){}

VSF_CAL_WEAK(QSPI1_IRQHandler)
void QSPI1_IRQHandler       (void){}

VSF_CAL_WEAK(DMAMUX_IRQHandler)
void DMAMUX_IRQHandler      (void){}

VSF_CAL_WEAK(ACC_IRQHandler)
void ACC_IRQHandler         (void){}

// SWI0~31: 43、46-50、61-66、70、78-80、84、86-91、93、95-102

VSF_CAL_WEAK(SWI0_IRQHandler)
void SWI0_IRQHandler        (void){}

VSF_CAL_WEAK(SWI1_IRQHandler)
void SWI1_IRQHandler        (void){}

VSF_CAL_WEAK(SWI2_IRQHandler)
void SWI2_IRQHandler        (void){}

VSF_CAL_WEAK(SWI3_IRQHandler)
void SWI3_IRQHandler        (void){}

VSF_CAL_WEAK(SWI4_IRQHandler)
void SWI4_IRQHandler        (void){}

VSF_CAL_WEAK(SWI5_IRQHandler)
void SWI5_IRQHandler        (void){}

VSF_CAL_WEAK(SWI6_IRQHandler)
void SWI6_IRQHandler        (void){}

VSF_CAL_WEAK(SWI7_IRQHandler)
void SWI7_IRQHandler        (void){}

VSF_CAL_WEAK(SWI8_IRQHandler)
void SWI8_IRQHandler        (void){}

VSF_CAL_WEAK(SWI9_IRQHandler)
void SWI9_IRQHandler        (void){}

VSF_CAL_WEAK(SWI10_IRQHandler)
void SWI10_IRQHandler       (void){}

VSF_CAL_WEAK(SWI11_IRQHandler)
void SWI11_IRQHandler       (void){}

VSF_CAL_WEAK(SWI12_IRQHandler)
void SWI12_IRQHandler       (void){}

VSF_CAL_WEAK(SWI13_IRQHandler)
void SWI13_IRQHandler       (void){}

VSF_CAL_WEAK(SWI14_IRQHandler)
void SWI14_IRQHandler       (void){}

VSF_CAL_WEAK(SWI15_IRQHandler)
void SWI15_IRQHandler       (void){}

VSF_CAL_WEAK(SWI16_IRQHandler)
void SWI16_IRQHandler       (void){}

VSF_CAL_WEAK(SWI17_IRQHandler)
void SWI17_IRQHandler       (void){}

VSF_CAL_WEAK(SWI18_IRQHandler)
void SWI18_IRQHandler       (void){}

VSF_CAL_WEAK(SWI19_IRQHandler)
void SWI19_IRQHandler       (void){}

VSF_CAL_WEAK(SWI20_IRQHandler)
void SWI20_IRQHandler       (void){}

VSF_CAL_WEAK(SWI21_IRQHandler)
void SWI21_IRQHandler       (void){}

VSF_CAL_WEAK(SWI22_IRQHandler)
void SWI22_IRQHandler       (void){}

VSF_CAL_WEAK(SWI23_IRQHandler)
void SWI23_IRQHandler       (void){}

VSF_CAL_WEAK(SWI24_IRQHandler)
void SWI24_IRQHandler       (void){}

VSF_CAL_WEAK(SWI25_IRQHandler)
void SWI25_IRQHandler       (void){}

VSF_CAL_WEAK(SWI26_IRQHandler)
void SWI26_IRQHandler       (void){}

VSF_CAL_WEAK(SWI27_IRQHandler)
void SWI27_IRQHandler       (void){}

VSF_CAL_WEAK(SWI28_IRQHandler)
void SWI28_IRQHandler       (void){}

VSF_CAL_WEAK(SWI29_IRQHandler)
void SWI29_IRQHandler       (void){}

VSF_CAL_WEAK(SWI30_IRQHandler)
void SWI30_IRQHandler       (void){}

VSF_CAL_WEAK(SWI31_IRQHandler)
void SWI31_IRQHandler       (void){}

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

    /* External Interrupts */
    WWDT_IRQHandler,                          /*   0 Window Watchdog Timer */
    PVM_IRQHandler,                           /*   1 PVM through EXINT Line detect */
    TAMP_STAMP_IRQHandler,                    /*   2 Tamper and TimeStamps through the EXINT line */
    ERTC_WKUP_IRQHandler,                     /*   3 ERTC Wakeup through the EXINT line */
    FLASH_IRQHandler,                         /*   4 Flash */
    CRM_IRQHandler,                           /*   5 CRM */
    EXINT0_IRQHandler,                        /*   6 EXINT Line 0 */
    EXINT1_IRQHandler,                        /*   7 EXINT Line 1 */
    EXINT2_IRQHandler,                        /*   8 EXINT Line 2 */
    EXINT3_IRQHandler,                        /*   9 EXINT Line 3 */
    EXINT4_IRQHandler,                        /*  10 EXINT Line 4 */
    DMA1_Channel1_IRQHandler,                 /*  11 DMA1 Channel 1 */
    DMA1_Channel2_IRQHandler,                 /*  12 DMA1 Channel 2 */
    DMA1_Channel3_IRQHandler,                 /*  13 DMA1 Channel 3 */
    DMA1_Channel4_IRQHandler,                 /*  14 DMA1 Channel 4 */
    DMA1_Channel5_IRQHandler,                 /*  15 DMA1 Channel 5 */
    DMA1_Channel6_IRQHandler,                 /*  16 DMA1 Channel 6 */
    DMA1_Channel7_IRQHandler,                 /*  17 DMA1 Channel 7 */
    ADC1_IRQHandler,                          /*  18 ADC1 */
    CAN1_TX_IRQHandler,                       /*  19 CAN1 TX */
    CAN1_RX0_IRQHandler,                      /*  20 CAN1 RX0 */
    CAN1_RX1_IRQHandler,                      /*  21 CAN1 RX1 */
    CAN1_SE_IRQHandler,                       /*  22 CAN1 SE */
    EXINT9_5_IRQHandler,                      /*  23 EXINT Line [9:5] */
    TMR1_BRK_TMR9_IRQHandler,                 /*  24 TMR1 Brake and TMR9 */
    TMR1_OVF_TMR10_IRQHandler,                /*  25 TMR1 Overflow and TMR10 */
    TMR1_TRG_HALL_TMR11_IRQHandler,           /*  26 TMR1 Trigger and hall and TMR11 */
    TMR1_CH_IRQHandler,                       /*  27 TMR1 Channel */
    TMR2_GLOBAL_IRQHandler,                   /*  28 TMR2 */
    TMR3_GLOBAL_IRQHandler,                   /*  29 TMR3 */
    TMR4_GLOBAL_IRQHandler,                   /*  30 TMR4 */
    I2C1_EVT_IRQHandler,                      /*  31 I2C1 Event */
    I2C1_ERR_IRQHandler,                      /*  32 I2C1 Error */
    I2C2_EVT_IRQHandler,                      /*  33 I2C2 Event */
    I2C2_ERR_IRQHandler,                      /*  34 I2C2 Error */
    SPI1_IRQHandler,                          /*  35 SPI1 */
    SPI2_IRQHandler,                          /*  36 SPI2 */
    USART1_IRQHandler,                        /*  37 USART1 */
    USART2_IRQHandler,                        /*  38 USART2 */
    USART3_IRQHandler,                        /*  39 USART3 */
    EXINT15_10_IRQHandler,                    /*  40 EXINT Line [15:10] */
    ERTCAlarm_IRQHandler,                     /*  41 ERTC Alarm through EXINT Line */
    OTGFS1_WKUP_IRQHandler,                   /*  42 OTGFS1 Wakeup from suspend */
    SWI0_IRQHandler,                          /*  43 SWI0 */
    TMR13_GLOBAL_IRQHandler,                  /*  44 TMR13 */
    TMR14_GLOBAL_IRQHandler,                  /*  45 TMR14 */
    SWI1_IRQHandler,                          /*  46 SWI1 */
    SWI2_IRQHandler,                          /*  47 SWI2 */
    SWI3_IRQHandler,                          /*  48 SWI3 */
    SWI4_IRQHandler,                          /*  49 SWI4 */
    SWI5_IRQHandler,                          /*  50 SWI5 */
    SPI3_IRQHandler,                          /*  51 SPI3 */
    USART4_IRQHandler,                        /*  52 USART4 */
    USART5_IRQHandler,                        /*  53 USART5 */
    TMR6_GLOBAL_IRQHandler,                   /*  54 TMR6 */
    TMR7_GLOBAL_IRQHandler,                   /*  55 TMR7 */
    DMA2_Channel1_IRQHandler,                 /*  56 DMA2 Channel 1 */
    DMA2_Channel2_IRQHandler,                 /*  57 DMA2 Channel 2 */
    DMA2_Channel3_IRQHandler,                 /*  58 DMA2 Channel 3 */
    DMA2_Channel4_IRQHandler,                 /*  59 DMA2 Channel 4 */
    DMA2_Channel5_IRQHandler,                 /*  60 DMA2 Channel 5 */
    SWI6_IRQHandler,                          /*  61 SWI6 */
    SWI7_IRQHandler,                          /*  62 SWI7 */
    SWI8_IRQHandler,                          /*  63 SWI8 */
    SWI9_IRQHandler,                          /*  64 SWI9 */
    SWI10_IRQHandler,                         /*  65 SWI10 */
    SWI11_IRQHandler,                         /*  66 SWI11 */
    OTGFS1_IRQHandler,                        /*  67 OTGFS1 */
    DMA2_Channel6_IRQHandler,                 /*  68 DMA2 Channel 6 */
    DMA2_Channel7_IRQHandler,                 /*  69 DMA2 Channel 7 */
    SWI12_IRQHandler,                         /*  70 SWI12 */
    USART6_IRQHandler,                        /*  71 USART6 */
    I2C3_EVT_IRQHandler,                      /*  72 I2C3 Event */
    I2C3_ERR_IRQHandler,                      /*  73 I2C3 Error */
    OTGHS_EP1_OUT_IRQHandler,                 /*  74 OTGHS endpoint1 out (Original: SWI1_IRQHandler) */
    OTGHS_EP1_IN_IRQHandler,                  /*  75 OTGHS endpoint1 in (Original: SWI2_IRQHandler) */
    OTGHS_WKUP_IRQHandler,                    /*  76 OTGHS Wakeup from suspend */
    OTGHS_IRQHandler,                         /*  77 OTGHS */
    SWI13_IRQHandler,                         /*  78 SWI13 */
    SWI14_IRQHandler,                         /*  79 SWI14 */
    SWI15_IRQHandler,                         /*  80 SWI15 */
    FPU_IRQHandler,                           /*  81 FPU */
    UART7_IRQHandler,                         /*  82 UART7 */
    UART8_IRQHandler,                         /*  83 UART8 */
    SWI16_IRQHandler,                         /*  84 SWI16 */
    I2SF5_IRQHandler,                         /*  85 I2SF5 (Original: SWI5_IRQHandler) */
    SWI17_IRQHandler,                         /*  86 SWI17 */
    SWI18_IRQHandler,                         /*  87 SWI18 */
    SWI19_IRQHandler,                         /*  88 SWI19 */
    SWI20_IRQHandler,                         /*  89 SWI20 */
    SWI21_IRQHandler,                         /*  90 SWI21 */
    SWI22_IRQHandler,                         /*  91 SWI22 */
    QSPI1_IRQHandler,                         /*  92 QSPI1 */
    SWI23_IRQHandler,                         /*  93 SWI23 */
    DMAMUX_IRQHandler,                        /*  94 DMAMUX */
    SWI24_IRQHandler,                         /*  95 SWI24 */
    SWI25_IRQHandler,                         /*  96 SWI25 */
    SWI26_IRQHandler,                         /*  97 SWI26 */
    SWI27_IRQHandler,                         /*  98 SWI27 */
    SWI28_IRQHandler,                         /*  99 SWI28 */
    SWI29_IRQHandler,                         /* 100 SWI29 */
    SWI30_IRQHandler,                         /* 101 SWI30 */
    SWI31_IRQHandler,                         /* 102 SWI31 */
    ACC_IRQHandler,                           /* 103 ACC */
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

    SystemInit();

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
