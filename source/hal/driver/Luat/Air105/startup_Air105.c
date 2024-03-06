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

// for WEAK in compiler
#include "utilities/vsf_utilities.h"

// header which will include core_cm4.h
#ifdef FALSE
#   undef FALSE
#endif
#ifdef TRUE
#   undef TRUE
#endif
#include "air105.h"
#include "global_config.h"

#define __imp_unprocessed_weak_handler(__name)                                  \
            VSF_CAL_WEAK(__name)                                                \
            void __name(void){}
#define __imp_blocked_weak_handler(__name)                                      \
            VSF_CAL_WEAK(__name)                                                \
            void __name(void){while(1);}

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
__imp_unprocessed_weak_handler(NMI_Handler)
__imp_blocked_weak_handler(HardFault_Handler)
__imp_blocked_weak_handler(MemManage_Handler)
__imp_blocked_weak_handler(BusFault_Handler)
__imp_blocked_weak_handler(UsageFault_Handler)

__imp_unprocessed_weak_handler(SVC_Handler)
__imp_unprocessed_weak_handler(DebugMon_Handler)
__imp_unprocessed_weak_handler(PendSV_Handler)
__imp_unprocessed_weak_handler(SysTick_Handler)

/* External interrupts */
__imp_unprocessed_weak_handler(ISR_GlobalHandler)
__imp_unprocessed_weak_handler(SWI0_IRQHandler)
__imp_unprocessed_weak_handler(SWI1_IRQHandler)
__imp_unprocessed_weak_handler(SWI2_IRQHandler)
__imp_unprocessed_weak_handler(SWI3_IRQHandler)
__imp_unprocessed_weak_handler(SWI4_IRQHandler)
__imp_unprocessed_weak_handler(SWI5_IRQHandler)
__imp_unprocessed_weak_handler(SWI6_IRQHandler)
__imp_unprocessed_weak_handler(SWI7_IRQHandler)
__imp_unprocessed_weak_handler(SWI8_IRQHandler)
__imp_unprocessed_weak_handler(USB_OTG0_Handler)
__imp_unprocessed_weak_handler(UART0_Handler)
__imp_unprocessed_weak_handler(UART1_Handler)
__imp_unprocessed_weak_handler(UART2_Handler)
__imp_unprocessed_weak_handler(UART3_Handler)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

VSF_CAL_ROOT const pFunc __VECTOR_TABLE[] VSF_CAL_ALIGN(512) = {
    (pFunc)(&__INITIAL_SP),                             /*     Initial Stack Pointer */
    Reset_Handler,                                      /*     Reset Handler */
    NMI_Handler,                                        /* -14 NMI Handler */
    HardFault_Handler,                                  /* -13 Hard Fault Handler */
    MemManage_Handler,                                  /* -12 MPU Fault Handler */
    BusFault_Handler,                                   /* -11 Bus Fault Handler */
    UsageFault_Handler,                                 /* -10 Usage Fault Handler */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    SVC_Handler,                                        /*  -5 SVCall Handler */
    DebugMon_Handler,                                   /*  -4 Debug Monitor Handler */
    0,                                                  /*     Reserved */
    PendSV_Handler,                                     /*  -2 PendSV Handler */
    SysTick_Handler,                                    /*  -1 SysTick Handler */

    /* Interrupts */
    ISR_GlobalHandler,
    USB_OTG0_Handler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    UART0_Handler,
    UART1_Handler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    SWI0_IRQHandler,
    ISR_GlobalHandler,
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    UART2_Handler,
    UART3_Handler,
    SWI3_IRQHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    SWI4_IRQHandler,
    ISR_GlobalHandler,
    SWI5_IRQHandler,
    SWI6_IRQHandler,
    SWI7_IRQHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    SWI8_IRQHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
    ISR_GlobalHandler,
};

VSF_CAL_WEAK(g_CAppInfo)
VSF_CAL_ROOT const uint32_t g_CAppInfo[] __VECTOR_TABLE_ATTRIBUTE = {
    __APP_START_MAGIC__,
    (uint32_t)__VECTOR_TABLE,
    __BL_VERSION__,
	__CORE_VERSION__,
	0,
	0,
	0,
	0,
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

    //! enable FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U) );          /* enable CP11 Full Access */

    SCB->VTOR = (uint32_t)__VECTOR_TABLE;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
