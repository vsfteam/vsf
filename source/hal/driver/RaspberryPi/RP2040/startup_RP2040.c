/******************************************************************************
 * @file     startup_ARMCM0PLUS.c
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

// for WEAK in compiler
#include "utilities/vsf_utilities.h"
// for vsf_arch_set_stack in case bootloader does not set the SP register
#include "hal/arch/vsf_arch.h"

// header which will include core_cm0plus.h
#include "RP2040.h"

#define __imp_unprocessed_weak_handler(__name)                                  \
            WEAK(__name)                                                        \
            void __name(void){}
#define __imp_blocked_weak_handler(__name)                                      \
            WEAK(__name)                                                        \
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

VSF_MFOREACH(__imp_unprocessed_weak_handler,
    NMI_Handler,
    SVC_Handler,
    PendSV_Handler,
    SysTick_Handler,
    __Dummy_Handler_DONOTUSE,
)
VSF_MFOREACH(__imp_blocked_weak_handler,
    HardFault_Handler,
)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

ROOT const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),                             /*     Initial Stack Pointer */
    Reset_Handler,                                      /*     Reset Handler */
    NMI_Handler,                                        /* -14 NMI Handler */
    HardFault_Handler,                                  /* -13 Hard Fault Handler */
    0,                                                  /* -12 MPU Fault Handler */
    0,                                                  /* -11 Bus Fault Handler */
    0,                                                  /* -10 Usage Fault Handler */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    0,                                                  /*     Reserved */
    SVC_Handler,                                        /*  -5 SVCall Handler */
    0,                                                  /*  -4 Debug Monitor Handler */
    0,                                                  /*     Reserved */
    PendSV_Handler,                                     /*  -2 PendSV Handler */
    SysTick_Handler,                                    /*  -1 SysTick Handler */

    /* Interrupts */
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
    __Dummy_Handler_DONOTUSE,
};

uint32_t __attribute__((section(".ram_vector_table"))) ram_vector_table[dimof(__VECTOR_TABLE)];

WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/

void _entry_point(void)
{
    register uint32_t *vtab;
#if PICO_NO_FLASH
    vtab = __VECTOR_TABLE;
#else
    // in bootrom, not NULL
    vtab = (uint32_t *)0;
#endif
    SCB->VTOR = (uint32_t)vtab;

    // call reset handler
    vsf_arch_set_stack((uintptr_t)vtab[0]);
    // code below maybe buggy, if vtab is in original stack, will cause error becasue SP register is updated
    ((void (*)(void))(vtab[1]))();
}

void Reset_Handler(void)
{
    vsf_arch_set_stack((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();

    memcpy(ram_vector_table, __VECTOR_TABLE, sizeof(ram_vector_table));
    SCB->VTOR = (uint32_t)ram_vector_table;

    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
