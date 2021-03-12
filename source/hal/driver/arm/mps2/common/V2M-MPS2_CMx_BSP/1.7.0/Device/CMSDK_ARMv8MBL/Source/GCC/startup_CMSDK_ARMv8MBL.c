/**************************************************************************//**
 * @file     startup_CMSDK_ARMv8MBL.s
 * @brief    CMSIS Core Device Startup File for
 *           CMSDK_ARMv8MBL Device
 * @version  V1.00
 * @date     12. July 2016
 ******************************************************************************/
/* Copyright (c) 2015 - 2016 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdint.h>


/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;
extern uint32_t __zero_table_start__;
extern uint32_t __zero_table_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );


/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
#ifndef __START
extern void  _start(void) __attribute__((noreturn));    /* PreeMain (C library entry point) */
#else
extern int  __START(void) __attribute__((noreturn));    /* main entry point */
#endif

#ifndef __NO_SYSTEM_INIT
extern void SystemInit (void);            /* CMSIS System Initialization      */
#endif


/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void) __attribute__ ((noreturn)); /* Default handler */
void Reset_Handler  (void) __attribute__ ((noreturn)); /* Reset Handler */


/*----------------------------------------------------------------------------
  User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
#ifndef __STACK_SIZE
  #define	__STACK_SIZE  0x00000400
#endif
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

#ifndef __HEAP_SIZE
  #define	__HEAP_SIZE   0x00000C00
#endif
#if __HEAP_SIZE > 0
static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif


/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* ARMv8MBL Processor Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/* ARMv8MBL Specific Interrupts */
void UART0RX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0TX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1RX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1TX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2RX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2TX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0ALL_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1ALL_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DUALTIMER_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_0_1_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART_0_1_2_OVF_Handler (void) __attribute__ ((weak, alias("Default_Handler")));
void ETHERNET_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void TOUCHSCREEN_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3RX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3TX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4RX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4TX_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_2_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_3_4_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_0_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_1_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_2_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_3_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_4_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_5_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_6_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO0_7_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_0_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_1_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_2_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_3_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_4_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_5_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_6_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_7_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_8_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_9_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_10_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_11_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_12_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_13_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_14_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_15_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_0B_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SECURETIMER0_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void SECURETIMER1_Handler   (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_1B_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_2B_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_3B_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_4B_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
const pFunc __Vectors[] __attribute__ ((section(".vectors"))) = {
  /* ARMv8MBL Exceptions Handler */
  (pFunc)((uint32_t)&__StackTop),           /*      Initial Stack Pointer     */
  Reset_Handler,                            /*      Reset Handler             */
  NMI_Handler,                              /*      NMI Handler               */
  HardFault_Handler,                        /*      Hard Fault Handler        */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  SVC_Handler,                              /*      SVCall Handler            */
  0,                                        /*      Reserved                  */
  0,                                        /*      Reserved                  */
  PendSV_Handler,                           /*      PendSV Handler            */
  SysTick_Handler,                          /*      SysTick Handler           */

  /* External interrupts */
  UART0RX_Handler,                          /*  0 UART 0 receive interrupt */
  UART0TX_Handler,                          /*  1 UART 0 transmit interrupt */
  UART1RX_Handler,                          /*  2 UART 1 receive interrupt */
  UART1TX_Handler,                          /*  3 UART 1 transmit interrupt */
  UART2RX_Handler,                          /*  4 UART 2 receive interrupt */
  UART2TX_Handler,                          /*  5 UART 2 transmit interrupt */
  GPIO0ALL_Handler,                         /*  6 GPIO 0 combined interrupt */
  GPIO1ALL_Handler,                         /*  7 GPIO 1 combined interrupt */
  TIMER0_Handler,                           /*  8 Timer 0 interrupt */
  TIMER1_Handler,                           /*  9 Timer 1 interrupt */
  DUALTIMER_Handler,                        /* 10 Dual Timer interrupt */
  SPI_0_1_Handler,                          /* 11 SPI #0, #1 interrupt */
  UART_0_1_2_OVF_Handler,                   /* 12 UART overflow (0, 1 & 2) interrupt */
  ETHERNET_Handler,                         /* 13 Ethernet interrupt */
  I2S_Handler,                              /* 14 Audio I2S interrupt */
  TOUCHSCREEN_Handler,                      /* 15 Touch Screen interrupt */
  GPIO2_Handler,                            /* 16 GPIO 2 combined interrupt */
  GPIO3_Handler,                            /* 17 GPIO 3 combined interrupt */
  UART3RX_Handler,                          /* 18 UART 3 receive interrupt */
  UART3TX_Handler,                          /* 19 UART 3 transmit interrupt */
  UART4RX_Handler,                          /* 20 UART 4 receive interrupt */
  UART4TX_Handler,                          /* 21 UART 4 transmit interrupt */
  SPI_2_Handler,                            /* 22 SPI #2 interrupt */
  SPI_3_4_Handler,                          /* 23 SPI #3, SPI #4 interrupt */
  GPIO0_0_Handler,                          /* 24 GPIO 0 individual interrupt ( 0) */
  GPIO0_1_Handler,                          /* 25 GPIO 0 individual interrupt ( 1) */
  GPIO0_2_Handler,                          /* 26 GPIO 0 individual interrupt ( 2) */
  GPIO0_3_Handler,                          /* 27 GPIO 0 individual interrupt ( 3) */
  GPIO0_4_Handler,                          /* 28 GPIO 0 individual interrupt ( 4) */
  GPIO0_5_Handler,                          /* 29 GPIO 0 individual interrupt ( 5) */
  GPIO0_6_Handler,                          /* 30 GPIO 0 individual interrupt ( 6) */
  GPIO0_7_Handler,                          /* 31 GPIO 0 individual interrupt ( 7) */
  GPIO1_0_Handler,                          /* 32 GPIO 1 individual interrupt ( 0) */
  GPIO1_1_Handler,                          /* 33 GPIO 1 individual interrupt ( 1) */
  GPIO1_2_Handler,                          /* 34 GPIO 1 individual interrupt ( 2) */
  GPIO1_3_Handler,                          /* 35 GPIO 1 individual interrupt ( 3) */
  GPIO1_4_Handler,                          /* 36 GPIO 1 individual interrupt ( 4) */
  GPIO1_5_Handler,                          /* 37 GPIO 1 individual interrupt ( 5) */
  GPIO1_6_Handler,                          /* 38 GPIO 1 individual interrupt ( 6) */
  GPIO1_7_Handler,                          /* 39 GPIO 1 individual interrupt ( 7) */
  GPIO1_8_Handler,                          /* 40 GPIO 1 individual interrupt ( 0) */
  GPIO1_9_Handler,                          /* 41 GPIO 1 individual interrupt ( 9) */
  GPIO1_10_Handler,                         /* 42 GPIO 1 individual interrupt (10) */
  GPIO1_11_Handler,                         /* 43 GPIO 1 individual interrupt (11) */
  GPIO1_12_Handler,                         /* 44 GPIO 1 individual interrupt (12) */
  GPIO1_13_Handler,                         /* 45 GPIO 1 individual interrupt (13) */
  GPIO1_14_Handler,                         /* 46 GPIO 1 individual interrupt (14) */
  GPIO1_15_Handler,                         /* 47 GPIO 1 individual interrupt (15) */
  SPI_0B_Handler,                           /* 48 SPI #0 interrupt */
  0,                                        /* 49 Reserved */
  SECURETIMER0_Handler,                     /* 50 Secure Timer 0 interrupt */
  SECURETIMER1_Handler,                     /* 51 Secure Timer 1 interrupt */
  SPI_1B_Handler,                           /* 52 SPI #1 interrupt */
  SPI_2B_Handler,                           /* 53 SPI #2 interrupt */
  SPI_3B_Handler,                           /* 54 SPI #3 interrupt */
  SPI_4B_Handler                            /* 55 SPI #4 interrupt */
};


/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) {
  uint32_t *pSrc, *pDest;
  uint32_t *pTable __attribute__((unused));

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pTable = &__copy_table_start__;

  for (; pTable < &__copy_table_end__; pTable = pTable + 3) {
		pSrc  = (uint32_t*)*(pTable + 0);
		pDest = (uint32_t*)*(pTable + 1);
		for (; pDest < (uint32_t*)(*(pTable + 1) + *(pTable + 2)) ; ) {
      *pDest++ = *pSrc++;
		}
	}
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pSrc  = &__etext;
  pDest = &__data_start__;

  for ( ; pDest < &__data_end__ ; ) {
    *pDest++ = *pSrc++;
  }
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
  pTable = &__zero_table_start__;

  for (; pTable < &__zero_table_end__; pTable = pTable + 2) {
		pDest = (uint32_t*)*(pTable + 0);
		for (; pDest < (uint32_t*)(*(pTable + 0) + *(pTable + 1)) ; ) {
      *pDest++ = 0;
		}
	}
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
  pDest = &__bss_start__;

  for ( ; pDest < &__bss_end__ ; ) {
    *pDest++ = 0ul;
  }
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
	SystemInit();
#endif

#ifndef __START
#define __START _start
#endif
	__START();

}


/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void) {

	while(1);
}
