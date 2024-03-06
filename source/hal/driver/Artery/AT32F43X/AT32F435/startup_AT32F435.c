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

VSF_CAL_WEAK(EDMA_Stream1_IRQHandler)
void EDMA_Stream1_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream2_IRQHandler)
void EDMA_Stream2_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream3_IRQHandler)
void EDMA_Stream3_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream4_IRQHandler)
void EDMA_Stream4_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream5_IRQHandler)
void EDMA_Stream5_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream6_IRQHandler)
void EDMA_Stream6_IRQHandler(void){}

VSF_CAL_WEAK(EDMA_Stream7_IRQHandler)
void EDMA_Stream7_IRQHandler(void){}

VSF_CAL_WEAK(ADC1_2_3_IRQHandler)
void ADC1_2_3_IRQHandler    (void){}

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

VSF_CAL_WEAK(SPI2_I2S2EXT_IRQHandler)
void SPI2_I2S2EXT_IRQHandler(void){}

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

VSF_CAL_WEAK(TMR8_BRK_TMR12_IRQHandler)
void TMR8_BRK_TMR12_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_OVF_TMR13_IRQHandler)
void TMR8_OVF_TMR13_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_TRG_HALL_TMR14_IRQHandler)
void TMR8_TRG_HALL_TMR14_IRQHandler(void){}

VSF_CAL_WEAK(TMR8_CH_IRQHandler)
void TMR8_CH_IRQHandler     (void){}

VSF_CAL_WEAK(EDMA_Stream8_IRQHandler)
void EDMA_Stream8_IRQHandler(void){}

VSF_CAL_WEAK(XMC_IRQHandler)
void XMC_IRQHandler         (void){}

VSF_CAL_WEAK(SDIO1_IRQHandler)
void SDIO1_IRQHandler       (void){}

VSF_CAL_WEAK(TMR5_GLOBAL_IRQHandler)
void TMR5_GLOBAL_IRQHandler (void){}

VSF_CAL_WEAK(SPI3_I2S3EXT_IRQHandler)
void SPI3_I2S3EXT_IRQHandler(void){}

VSF_CAL_WEAK(UART4_IRQHandler)
void UART4_IRQHandler       (void){}

VSF_CAL_WEAK(UART5_IRQHandler)
void UART5_IRQHandler       (void){}

VSF_CAL_WEAK(TMR6_DAC_GLOBAL_IRQHandler)
void TMR6_DAC_GLOBAL_IRQHandler(void){}

VSF_CAL_WEAK(TMR7_GLOBAL_IRQHandler)
void TMR7_GLOBAL_IRQHandler (void){}

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

VSF_CAL_WEAK(EMAC_IRQHandler)
void EMAC_IRQHandler        (void){}

VSF_CAL_WEAK(EMAC_WKUP_IRQHandler)
void EMAC_WKUP_IRQHandler   (void){}

VSF_CAL_WEAK(CAN2_TX_IRQHandler)
void CAN2_TX_IRQHandler     (void){}

VSF_CAL_WEAK(CAN2_RX0_IRQHandler)
void CAN2_RX0_IRQHandler    (void){}

VSF_CAL_WEAK(CAN2_RX1_IRQHandler)
void CAN2_RX1_IRQHandler    (void){}

VSF_CAL_WEAK(CAN2_SE_IRQHandler)
void CAN2_SE_IRQHandler     (void){}

VSF_CAL_WEAK(OTGFS1_IRQHandler)
void OTGFS1_IRQHandler      (void){}

VSF_CAL_WEAK(DMA1_Channel6_IRQHandler)
void DMA1_Channel6_IRQHandler(void){}

VSF_CAL_WEAK(DMA1_Channel7_IRQHandler)
void DMA1_Channel7_IRQHandler(void){}

VSF_CAL_WEAK(USART6_IRQHandler)
void USART6_IRQHandler      (void){}

VSF_CAL_WEAK(I2C3_EVT_IRQHandler)
void I2C3_EVT_IRQHandler    (void){}

VSF_CAL_WEAK(I2C3_ERR_IRQHandler)
void I2C3_ERR_IRQHandler    (void){}

VSF_CAL_WEAK(OTGFS2_WKUP_IRQHandler)
void OTGFS2_WKUP_IRQHandler(void){}

VSF_CAL_WEAK(OTGFS2_IRQHandler)
void OTGFS2_IRQHandler     (void){}

VSF_CAL_WEAK(DVP_IRQHandler)
void DVP_IRQHandler         (void){}

VSF_CAL_WEAK(FPU_IRQHandler)
void FPU_IRQHandler         (void){}

VSF_CAL_WEAK(UART7_IRQHandler)
void UART7_IRQHandler       (void){}

VSF_CAL_WEAK(UART8_IRQHandler)
void UART8_IRQHandler       (void){}

VSF_CAL_WEAK(SPI4_IRQHandler)
void SPI4_IRQHandler        (void){}

VSF_CAL_WEAK(QSPI2_IRQHandler)
void QSPI2_IRQHandler       (void){}

VSF_CAL_WEAK(QSPI1_IRQHandler)
void QSPI1_IRQHandler       (void){}

VSF_CAL_WEAK(DMAMUX_IRQHandler)
void DMAMUX_IRQHandler      (void){}

VSF_CAL_WEAK(SDIO2_IRQHandler)
void SDIO2_IRQHandler       (void){}

VSF_CAL_WEAK(ACC_IRQHandler)
void ACC_IRQHandler         (void){}

VSF_CAL_WEAK(TMR20_BRK_IRQHandler)
void TMR20_BRK_IRQHandler   (void){}

VSF_CAL_WEAK(TMR20_OVF_IRQHandler)
void TMR20_OVF_IRQHandler   (void){}

VSF_CAL_WEAK(TMR20_TRG_HALL_IRQHandler)
void TMR20_TRG_HALL_IRQHandler(void){}

VSF_CAL_WEAK(TMR20_CH_IRQHandler)
void TMR20_CH_IRQHandler    (void){}

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

VSF_CAL_WEAK(DMA2_Channel6_IRQHandler)
void DMA2_Channel6_IRQHandler(void){}

VSF_CAL_WEAK(DMA2_Channel7_IRQHandler)
void DMA2_Channel7_IRQHandler(void){}

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
    PVM_IRQHandler,                           /*   1 PVM through EINT Line detection */
    TAMP_STAMP_IRQHandler,                    /*   2 Tamper and TimeStamps through the EINT line */
    ERTC_WKUP_IRQHandler,                     /*   3 ERTC Wakeup through the EINT line */
    FLASH_IRQHandler,                         /*   4 FLASH */
    CRM_IRQHandler,                           /*   5 CRM */
    EXINT0_IRQHandler,                        /*   6 EXINT Line0 */
    EXINT1_IRQHandler,                        /*   7 EXINT Line1 */
    EXINT2_IRQHandler,                        /*   8 EXINT Line2 */
    EXINT3_IRQHandler,                        /*   9 EXINT Line3 */
    EXINT4_IRQHandler,                        /*  10 EXINT Line4 */
    EDMA_Stream1_IRQHandler,                  /*  11 EDMA Stream 1 */
    EDMA_Stream2_IRQHandler,                  /*  12 EDMA Stream 2 */
    EDMA_Stream3_IRQHandler,                  /*  13 EDMA Stream 3 */
    EDMA_Stream4_IRQHandler,                  /*  14 EDMA Stream 4 */
    EDMA_Stream5_IRQHandler,                  /*  15 EDMA Stream 5 */
    EDMA_Stream6_IRQHandler,                  /*  16 EDMA Stream 6 */
    EDMA_Stream7_IRQHandler,                  /*  17 EDMA Stream 7 */
    ADC1_2_3_IRQHandler,                      /*  18 ADC1, ADC2 and ADC3 */
    CAN1_TX_IRQHandler,                       /*  19 CAN1 TX */
    CAN1_RX0_IRQHandler,                      /*  20 CAN1 RX0 */
    CAN1_RX1_IRQHandler,                      /*  21 CAN1 RX1 */
    CAN1_SE_IRQHandler,                       /*  22 CAN1 SE */
    EXINT9_5_IRQHandler,                      /*  23 EXINT Line[9:5] */
    TMR1_BRK_TMR9_IRQHandler,                 /*  24 TMR1 Break and TMR9 */
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
    SPI2_I2S2EXT_IRQHandler,                  /*  36 SPI2 */
    USART1_IRQHandler,                        /*  37 USART1 */
    USART2_IRQHandler,                        /*  38 USART2 */
    USART3_IRQHandler,                        /*  39 USART3 */
    EXINT15_10_IRQHandler,                    /*  40 EXINT Line[15:10] */
    ERTCAlarm_IRQHandler,                     /*  41 RTC Alarm through EINT Line */
    OTGFS1_WKUP_IRQHandler,                   /*  42 OTGFS1 Wakeup through EINT line */
    TMR8_BRK_TMR12_IRQHandler,                /*  43 TMR8 Break and TMR12 */
    TMR8_OVF_TMR13_IRQHandler,                /*  44 TMR8 Overflow and TMR13 */
    TMR8_TRG_HALL_TMR14_IRQHandler,           /*  45 TMR8 Trigger and hall and TMR14 */
    TMR8_CH_IRQHandler,                       /*  46 TMR8 Channel */
    EDMA_Stream8_IRQHandler,                  /*  47 EDMA1 Stream 7 */
    XMC_IRQHandler,                           /*  48 XMC */
    SDIO1_IRQHandler,                         /*  49 SDIO1 */
    TMR5_GLOBAL_IRQHandler,                   /*  50 TMR5 */
    SPI3_I2S3EXT_IRQHandler,                  /*  51 SPI3 */
    UART4_IRQHandler,                         /*  52 UART4 */
    UART5_IRQHandler,                         /*  53 UART5 */
    TMR6_DAC_GLOBAL_IRQHandler,               /*  54 TMR6 and DAC */
    TMR7_GLOBAL_IRQHandler,                   /*  55 TMR7 */
    DMA1_Channel1_IRQHandler,                 /*  56 DMA1 Channel 1 */
    DMA1_Channel2_IRQHandler,                 /*  57 DMA1 Channel 2 */
    DMA1_Channel3_IRQHandler,                 /*  58 DMA1 Channel 3 */
    DMA1_Channel4_IRQHandler,                 /*  59 DMA1 Channel 4 */
    DMA1_Channel5_IRQHandler,                 /*  60 DMA1 Channel 5 */
    EMAC_IRQHandler,                          /*  61 EMAC */
    EMAC_WKUP_IRQHandler,                     /*  62 EMAC Wakeup */
    CAN2_TX_IRQHandler,                       /*  63 CAN2 TX */
    CAN2_RX0_IRQHandler,                      /*  64 CAN2 RX0 */
    CAN2_RX1_IRQHandler,                      /*  65 CAN2 RX1 */
    CAN2_SE_IRQHandler,                       /*  66 CAN2 SE */
    OTGFS1_IRQHandler,                        /*  67 OTGFS1 */
    DMA1_Channel6_IRQHandler,                 /*  68 DMA1 Channel 6 */
    DMA1_Channel7_IRQHandler,                 /*  69 DMA1 Channel 7 */
    SWI0_IRQHandler,                          /*  70 SWI0 */
    USART6_IRQHandler,                        /*  71 USART6 */
    I2C3_EVT_IRQHandler,                      /*  72 I2C3 event */
    I2C3_ERR_IRQHandler,                      /*  73 I2C3 error */
    SWI1_IRQHandler,                          /*  74 SWI1 */
    SWI2_IRQHandler,                          /*  75 SWI2 */
    OTGFS2_WKUP_IRQHandler,                   /*  76 OTGFS2 Wakeup from suspend */
    OTGFS2_IRQHandler,                        /*  77 OTGFS2 */
    DVP_IRQHandler,                           /*  78 DVP */
    SWI3_IRQHandler,                          /*  79 SWI3 */
    SWI4_IRQHandler,                          /*  80 SWI4 */
    FPU_IRQHandler,                           /*  81 FPU */
    UART7_IRQHandler,                         /*  82 UART7 */
    UART8_IRQHandler,                         /*  83 UART8 */
    SPI4_IRQHandler,                          /*  84 SPI4 */
    SWI5_IRQHandler,                          /*  85 SWI5 */
    SWI6_IRQHandler,                          /*  86 SWI6 */
    SWI7_IRQHandler,                          /*  87 SWI7 */
    SWI8_IRQHandler,                          /*  88 SWI8 */
    SWI9_IRQHandler,                          /*  89 SWI9 */
    SWI10_IRQHandler,                         /*  90 SWI10 */
    QSPI2_IRQHandler,                         /*  91 QSPI2 */
    QSPI1_IRQHandler,                         /*  92 QSPI1 */
    SWI11_IRQHandler,                         /*  93 SWI11 */
    DMAMUX_IRQHandler,                        /*  94 DMAMUX */
    SWI12_IRQHandler,                         /*  95 SWI12 */
    SWI13_IRQHandler,                         /*  96 SWI13 */
    SWI14_IRQHandler,                         /*  97 SWI14 */
    SWI15_IRQHandler,                         /*  98 SWI15 */
    SWI16_IRQHandler,                         /*  99 SWI16 */
    SWI17_IRQHandler,                         /* 100 SWI17 */
    SWI18_IRQHandler,                         /* 101 SWI18 */
    SDIO2_IRQHandler,                         /* 102 DMAMUX */
    ACC_IRQHandler,                           /* 103 ACC */
    TMR20_BRK_IRQHandler,                     /* 104 TMR20 Brake */
    TMR20_OVF_IRQHandler,                     /* 105 TMR20 Overflow */
    TMR20_TRG_HALL_IRQHandler,                /* 106 TMR20 Trigger and hall */
    TMR20_CH_IRQHandler,                      /* 107 TMR20 Channel */
    DMA2_Channel1_IRQHandler,                 /* 108 DMA2 Channel 1 */
    DMA2_Channel2_IRQHandler,                 /* 109 DMA2 Channel 2 */
    DMA2_Channel3_IRQHandler,                 /* 110 DMA2 Channel 3 */
    DMA2_Channel4_IRQHandler,                 /* 111 DMA2 Channel 4 */
    DMA2_Channel5_IRQHandler,                 /* 112 DMA2 Channel 5 */
    DMA2_Channel6_IRQHandler,                 /* 113 DMA2 Channel 6 */
    DMA2_Channel7_IRQHandler,                 /* 114 DMA2 Channel 7 */
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

    //! trap unaligned access
    //SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
