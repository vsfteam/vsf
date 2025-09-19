/******************************************************************************
 * @file     startup_ARMCM0.c
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
    VSF_CAL_WEAK(__NAME)                                                        \
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

{{INTERRUPT_HANDLERS}}

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
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
{{INTERRUPT_VECTORS}}
};

#if __IS_COMPILER_GCC__
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

  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}