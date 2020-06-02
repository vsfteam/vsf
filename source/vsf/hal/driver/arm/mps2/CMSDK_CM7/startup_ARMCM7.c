/******************************************************************************
 * @file     startup_ARMCM7.c
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

#define WEAK_ISR(__NAME, ...)                                                   \
    WEAK(__NAME)                                                                \
    void __NAME(void) { __VA_ARGS__ }

/* Exceptions */
WEAK_ISR( NMI_Handler            )
WEAK_ISR( HardFault_Handler      , while(1);)
WEAK_ISR( MemManage_Handler      , while(1);)
WEAK_ISR( BusFault_Handler       , while(1);)
WEAK_ISR( UsageFault_Handler     , while(1);)
WEAK_ISR( SVC_Handler            )
WEAK_ISR( DebugMon_Handler       , while(1);)
WEAK_ISR( PendSV_Handler         )
WEAK_ISR( SysTick_Handler        )

WEAK_ISR( Interrupt0_Handler     )
WEAK_ISR( Interrupt1_Handler     )
WEAK_ISR( Interrupt2_Handler     )
WEAK_ISR( Interrupt3_Handler     )
WEAK_ISR( Interrupt4_Handler     )
WEAK_ISR( Interrupt5_Handler     )
WEAK_ISR( Interrupt6_Handler     )
WEAK_ISR( Interrupt7_Handler     )
WEAK_ISR( Interrupt8_Handler     )
WEAK_ISR( Interrupt9_Handler     )

WEAK_ISR( UART0RX_Handler        ) //   0 UART 0 receive interrupt
WEAK_ISR( UART0TX_Handler        ) //   1 UART 0 transmit interrupt
WEAK_ISR( UART1RX_Handler        ) //   2 UART 1 receive interrupt
WEAK_ISR( UART1TX_Handler        ) //   3 UART 1 transmit interrupt
WEAK_ISR( UART2RX_Handler        ) //   4 UART 2 receive interrupt
WEAK_ISR( UART2TX_Handler        ) //   5 UART 2 transmit interrupt
WEAK_ISR( GPIO0ALL_Handler       ) //   6 GPIO 0 combined interrupt
WEAK_ISR( GPIO1ALL_Handler       ) //   7 GPIO 1 combined interrupt
WEAK_ISR( TIMER0_Handler         ) //   8 Timer 0 interrupt
WEAK_ISR( TIMER1_Handler         ) //   9 Timer 1 interrupt
WEAK_ISR( DUALTIMER_Handler      ) //  10 Dual Timer interrupt
WEAK_ISR( SPI_0_1_Handler        ) //  11 SPI #0, #1 interrupt
WEAK_ISR( UART_0_1_2_OVF_Handler ) //  12 UART overflow (0, 1 & 2) interrupt
WEAK_ISR( ETHERNET_Handler       ) //  13 Ethernet interrupt
WEAK_ISR( I2S_Handler            ) //  14 Audio I2S interrupt
WEAK_ISR( TOUCHSCREEN_Handler    ) //  15 Touch Screen interrupt
WEAK_ISR( GPIO2_Handler          ) //  16 GPIO 2 combined interrupt
WEAK_ISR( GPIO3_Handler          ) //  17 GPIO 3 combined interrupt
WEAK_ISR( UART3RX_Handler        ) //  18 UART 3 receive interrupt
WEAK_ISR( UART3TX_Handler        ) //  19 UART 3 transmit interrupt
WEAK_ISR( UART4RX_Handler        ) //  20 UART 4 receive interrupt
WEAK_ISR( UART4TX_Handler        ) //  21 UART 4 transmit interrupt
WEAK_ISR( SPI_2_Handler          ) //  22 SPI #2 interrupt
WEAK_ISR( SPI_3_4_Handler        ) //  23 SPI #3, SPI #4 interrupt
/*
void GPIO0_0_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  24 GPIO 0 individual interrupt ( 0)
void GPIO0_1_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  25 GPIO 0 individual interrupt ( 1)
void GPIO0_2_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  26 GPIO 0 individual interrupt ( 2)
void GPIO0_3_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  27 GPIO 0 individual interrupt ( 3)
void GPIO0_4_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  28 GPIO 0 individual interrupt ( 4)
void GPIO0_5_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  29 GPIO 0 individual interrupt ( 5)
void GPIO0_6_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  30 GPIO 0 individual interrupt ( 6)
void GPIO0_7_Handler        (void) __attribute__ ((weak, alias("Default_Handler"))); //  31 GPIO 0 individual interrupt ( 7)
*/

#define __DECLARE_SWI_IRQ_HANDLER(__N, __NULL)                                  \
    WEAK_ISR(SWI##__N##_IRQHandler)                                             

REPEAT_MACRO(VSF_DEV_SWI_NUM, __DECLARE_SWI_IRQ_HANDLER, NULL)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if __IS_COMPILER_GCC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

 const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE  = {
    (pFunc)(&__INITIAL_SP),                    /*     Initial Stack Pointer */
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
    UART0RX_Handler            , //   0 UART 0 receive interrupt
    UART0TX_Handler            , //   1 UART 0 transmit interrupt
    UART1RX_Handler            , //   2 UART 1 receive interrupt
    UART1TX_Handler            , //   3 UART 1 transmit interrupt
    UART2RX_Handler            , //   4 UART 2 receive interrupt
    UART2TX_Handler            , //   5 UART 2 transmit interrupt
    GPIO0ALL_Handler           , //   6 GPIO 0 combined interrupt
    GPIO1ALL_Handler           , //   7 GPIO 1 combined interrupt
    TIMER0_Handler             , //   8 Timer 0 interrupt
    TIMER1_Handler             , //   9 Timer 1 interrupt
    DUALTIMER_Handler          , //  10 Dual Timer interrupt
    SPI_0_1_Handler            , //  11 SPI #0, #1 interrupt
    UART_0_1_2_OVF_Handler     , //  12 UART overflow (0, 1 & 2) interrupt
    ETHERNET_Handler           , //  13 Ethernet interrupt
    I2S_Handler                , //  14 Audio I2S interrupt
    TOUCHSCREEN_Handler        , //  15 Touch Screen interrupt
    GPIO2_Handler              , //  16 GPIO 2 combined interrupt
    GPIO3_Handler              , //  17 GPIO 3 combined interrupt
    UART3RX_Handler            , //  18 UART 3 receive interrupt
    UART3TX_Handler            , //  19 UART 3 transmit interrupt
    UART4RX_Handler            , //  20 UART 4 receive interrupt
    UART4TX_Handler            , //  21 UART 4 transmit interrupt
    SPI_2_Handler              , //  22 SPI #2 interrupt
    SPI_3_4_Handler            , //  23 SPI #3, SPI #4 interrupt
    SWI0_IRQHandler            , //  24 SWI 0 individual interrupt ( 0)
    SWI1_IRQHandler            , //  25 SWI 1 individual interrupt ( 1)
    SWI2_IRQHandler            , //  26 SWI 2 individual interrupt ( 2)
    SWI3_IRQHandler            , //  27 SWI 3 individual interrupt ( 3)
    SWI4_IRQHandler            , //  28 SWI 4 individual interrupt ( 4)
    SWI5_IRQHandler            , //  29 SWI 5 individual interrupt ( 5)
    SWI6_IRQHandler            , //  30 SWI 6 individual interrupt ( 6)
    //SWI7_IRQHandler            , //  31 SWI 7 individual interrupt ( 7)
};

#if __IS_COMPILER_GCC__
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
  SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
