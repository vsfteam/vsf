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

	.syntax	unified
	.arch	armv6-m

	.section .stack
	.align	3
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0x00000400
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0x00000C00
#endif
	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .vectors
	.align 2
	.globl	__Vectors
__Vectors:
	.long	__StackTop            /* Top of Stack */
	.long	Reset_Handler         /* Reset Handler */
	.long	NMI_Handler           /* NMI Handler */
	.long	HardFault_Handler     /* Hard Fault Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	SVC_Handler           /* SVCall Handler */
	.long	0                     /* Reserved */
	.long	0                     /* Reserved */
	.long	PendSV_Handler        /* PendSV Handler */
	.long	SysTick_Handler       /* SysTick Handler */

	/* External interrupts */
	.long	UART0RX_Handler            /*  0 UART 0 receive interrupt */
	.long	UART0TX_Handler            /*  1 UART 0 transmit interrupt */
	.long	UART1RX_Handler            /*  2 UART 1 receive interrupt */
	.long	UART1TX_Handler            /*  3 UART 1 transmit interrupt */
	.long	UART2RX_Handler            /*  4 UART 2 receive interrupt */
	.long	UART2TX_Handler            /*  5 UART 2 transmit interrupt */
	.long	GPIO0ALL_Handler           /*  6 GPIO 0 combined interrupt */
	.long	GPIO1ALL_Handler           /*  7 GPIO 1 combined interrupt */
	.long	TIMER0_Handler             /*  8 Timer 0 interrupt */
	.long	TIMER1_Handler             /*  9 Timer 1 interrupt */
	.long	DUALTIMER_Handler          /* 10 Dual Timer interrupt */
	.long	SPI_0_1_Handler            /* 11 SPI #0, #1 interrupt */
	.long	UART_0_1_2_OVF_Handler     /* 12 UART overflow (0, 1 & 2) interrupt */
	.long	ETHERNET_Handler           /* 13 Ethernet interrupt */
	.long	I2S_Handler                /* 14 Audio I2S interrupt */
	.long	TOUCHSCREEN_Handler        /* 15 Touch Screen interrupt */
	.long	GPIO2_Handler              /* 16 GPIO 2 combined interrupt */
	.long	GPIO3_Handler              /* 17 GPIO 3 combined interrupt */
	.long	UART3RX_Handler            /* 18 UART 3 receive interrupt */
	.long	UART3TX_Handler            /* 19 UART 3 transmit interrupt */
	.long	UART4RX_Handler            /* 20 UART 4 receive interrupt */
	.long	UART4TX_Handler            /* 21 UART 4 transmit interrupt */
	.long	SPI_2_Handler              /* 22 SPI #2 interrupt */
	.long	SPI_3_4_Handler            /* 23 SPI #3, SPI #4 interrupt */
	.long	GPIO0_0_Handler            /* 24 GPIO 0 individual interrupt ( 0) */
	.long	GPIO0_1_Handler            /* 25 GPIO 0 individual interrupt ( 1) */
	.long	GPIO0_2_Handler            /* 26 GPIO 0 individual interrupt ( 2) */
	.long	GPIO0_3_Handler            /* 27 GPIO 0 individual interrupt ( 3) */
	.long	GPIO0_4_Handler            /* 28 GPIO 0 individual interrupt ( 4) */
	.long	GPIO0_5_Handler            /* 29 GPIO 0 individual interrupt ( 5) */
	.long	GPIO0_6_Handler            /* 30 GPIO 0 individual interrupt ( 6) */
	.long	GPIO0_7_Handler            /* 31 GPIO 0 individual interrupt ( 7) */
	.long	GPIO1_0_Handler            /* 32 GPIO 1 individual interrupt ( 0) */
	.long	GPIO1_1_Handler            /* 33 GPIO 1 individual interrupt ( 1) */
	.long	GPIO1_2_Handler            /* 34 GPIO 1 individual interrupt ( 2) */
	.long	GPIO1_3_Handler            /* 35 GPIO 1 individual interrupt ( 3) */
	.long	GPIO1_4_Handler            /* 36 GPIO 1 individual interrupt ( 4) */
	.long	GPIO1_5_Handler            /* 37 GPIO 1 individual interrupt ( 5) */
	.long	GPIO1_6_Handler            /* 38 GPIO 1 individual interrupt ( 6) */
	.long	GPIO1_7_Handler            /* 39 GPIO 1 individual interrupt ( 7) */
	.long	GPIO1_8_Handler            /* 40 GPIO 1 individual interrupt ( 0) */
	.long	GPIO1_9_Handler            /* 41 GPIO 1 individual interrupt ( 9) */
	.long	GPIO1_10_Handler           /* 42 GPIO 1 individual interrupt (10) */
	.long	GPIO1_11_Handler           /* 43 GPIO 1 individual interrupt (11) */
	.long	GPIO1_12_Handler           /* 44 GPIO 1 individual interrupt (12) */
	.long	GPIO1_13_Handler           /* 45 GPIO 1 individual interrupt (13) */
	.long	GPIO1_14_Handler           /* 46 GPIO 1 individual interrupt (14) */
	.long	GPIO1_15_Handler           /* 47 GPIO 1 individual interrupt (15) */
	.long	SPI_0B_Handler             /* 48 SPI #0 interrupt */
	.long	0                          /* 49 Reserved */
	.long	SECURETIMER0_Handler       /* 50 Secure Timer 0 interrupt */
	.long	SECURETIMER1_Handler       /* 51 Secure Timer 1 interrupt */
	.long	SPI_1B_Handler             /* 52 SPI #1 interrupt */
	.long	SPI_2B_Handler             /* 53 SPI #2 interrupt */
	.long	SPI_3B_Handler             /* 54 SPI #3 interrupt */
	.long	SPI_4B_Handler             /* 55 SPI #4 interrupt */

	.size	__Vectors, . - __Vectors

	.text
	.thumb
	.thumb_func
	.align	1
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
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
	ldr	r4, =__copy_table_start__
	ldr	r5, =__copy_table_end__

.L_loop0:
	cmp	r4, r5
	bge	.L_loop0_done
	ldr	r1, [r4]
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]

.L_loop0_0:
	subs	r3, #4
	blt	.L_loop0_0_done
	ldr	r0, [r1, r3]
	str	r0, [r2, r3]
	b	.L_loop0_0

.L_loop0_0_done:
	adds	r4, #12
	b	.L_loop0

.L_loop0_done:
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
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

	subs	r3, r2
	ble	.L_loop1_done

.L_loop1:
	subs	r3, #4
	ldr	r0, [r1,r3]
	str	r0, [r2,r3]
	bgt	.L_loop1

.L_loop1_done:
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
	ldr	r3, =__zero_table_start__
	ldr	r4, =__zero_table_end__

.L_loop2:
	cmp	r3, r4
	bge	.L_loop2_done
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	movs	r0, 0

.L_loop2_0:
	subs	r2, #4
	blt	.L_loop2_0_done
	str	r0, [r1, r2]
	b	.L_loop2_0
.L_loop2_0_done:

	adds	r3, #8
	b	.L_loop2
.L_loop2_done:
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0

	subs	r2, r1
	ble	.L_loop3_done

.L_loop3:
	subs	r2, #4
	str	r0, [r1, r2]
	bgt	.L_loop3
.L_loop3_done:
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
	bl	SystemInit
#endif

#ifndef __START
#define __START _start
#endif
	bl	__START

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	SysTick_Handler

	def_irq_handler	UART0RX_Handler
	def_irq_handler	UART0TX_Handler
	def_irq_handler	UART1RX_Handler
	def_irq_handler	UART1TX_Handler
	def_irq_handler	UART2RX_Handler
	def_irq_handler	UART2TX_Handler
	def_irq_handler	GPIO0ALL_Handler
	def_irq_handler	GPIO1ALL_Handler
	def_irq_handler	TIMER0_Handler
	def_irq_handler	TIMER1_Handler
	def_irq_handler	DUALTIMER_Handler
	def_irq_handler	SPI_0_1_Handler
	def_irq_handler	UART_0_1_2_OVF_Handler
	def_irq_handler	ETHERNET_Handler
	def_irq_handler	I2S_Handler
	def_irq_handler	TOUCHSCREEN_Handler
	def_irq_handler	GPIO2_Handler
	def_irq_handler	GPIO3_Handler
	def_irq_handler	UART3RX_Handler
	def_irq_handler	UART3TX_Handler
	def_irq_handler	UART4RX_Handler
	def_irq_handler	UART4TX_Handler
	def_irq_handler	SPI_2_Handler
	def_irq_handler	SPI_3_4_Handler
	def_irq_handler	GPIO0_0_Handler
	def_irq_handler	GPIO0_1_Handler
	def_irq_handler	GPIO0_2_Handler
	def_irq_handler	GPIO0_3_Handler
	def_irq_handler	GPIO0_4_Handler
	def_irq_handler	GPIO0_5_Handler
	def_irq_handler	GPIO0_6_Handler
	def_irq_handler	GPIO0_7_Handler
	def_irq_handler	GPIO1_0_Handler
	def_irq_handler	GPIO1_1_Handler
	def_irq_handler	GPIO1_2_Handler
	def_irq_handler	GPIO1_3_Handler
	def_irq_handler	GPIO1_4_Handler
	def_irq_handler	GPIO1_5_Handler
	def_irq_handler	GPIO1_6_Handler
	def_irq_handler	GPIO1_7_Handler
	def_irq_handler	GPIO1_8_Handler
	def_irq_handler	GPIO1_9_Handler
	def_irq_handler	GPIO1_10_Handler
	def_irq_handler	GPIO1_11_Handler
	def_irq_handler	GPIO1_12_Handler
	def_irq_handler	GPIO1_13_Handler
	def_irq_handler	GPIO1_14_Handler
	def_irq_handler	GPIO1_15_Handler
	def_irq_handler	SPI_0B_Handler
	def_irq_handler	SECURETIMER0_Handler
	def_irq_handler	SECURETIMER1_Handler
	def_irq_handler	SPI_1B_Handler
	def_irq_handler	SPI_2B_Handler
	def_irq_handler	SPI_3B_Handler
	def_irq_handler	SPI_4B_Handler

	.end
