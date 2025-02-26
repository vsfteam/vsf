/******************************************************************************
 * @file     startup_ARMCM33.c
 * @brief    CMSIS-Core(M) Device Startup File for a Cortex-M33 Device
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

#include "../__device.h"

#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/driver.h"

// for VSF_MFOREACH
#include "utilities/vsf_utilities.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/

typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/

extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/

void
#if __IS_COMPILER_IAR__
__attribute__((naked))
#endif
__NO_RETURN Reset_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

#define __imp_blocked_weak_handler(__name)                                      \
            __WEAK void __name(void){while(1);}

VSF_MFOREACH(__imp_blocked_weak_handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    SecureFault_Handler,
    SVC_Handler,
    DebugMon_Handler,
    PendSV_Handler,
    SysTick_Handler,

    VSF_HW_INTERRUPTS
)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

__USED const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
    (pFunc)(&__INITIAL_SP),                   /*     Initial Stack Pointer */
    Reset_Handler,                            /*     Reset Handler */
    NMI_Handler,                              /* -14 NMI Handler */
    HardFault_Handler,                        /* -13 Hard Fault Handler */
    MemManage_Handler,                        /* -12 MPU Fault Handler */
    BusFault_Handler,                         /* -11 Bus Fault Handler */
    UsageFault_Handler,                       /* -10 Usage Fault Handler */
    SecureFault_Handler,                      /* -9  Secure Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    SVC_Handler,                              /*  -5 SVCall Handler */
    DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
    0,                                        /*     Reserved */
    PendSV_Handler,                           /*  -2 PendSV Handler */
    SysTick_Handler,                          /*  -1 SysTick Handler */

    /* Interrupts */
    VSF_HW_INTERRUPTS
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

__WEAK void vsf_hal_pre_startup_init(void)
{
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void
#if __IS_COMPILER_IAR__
__attribute__((naked))
#endif
__NO_RETURN Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    __set_MSPLIM(0);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
//    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

    //! enable FPU before vsf_hal_pre_startup_init, in case vsf_hal_pre_startup_init uses FPU
    SCB->CPACR |= ((3U << 0U * 2U)
                  |(3U << 1U * 2U)
                  |(3U << 2U * 2U)
                  |(3U << 10U* 2U)
                  |(3U << 11U* 2U)
                  );
    SCB->VTOR = (uint32_t)__VECTOR_TABLE;

    vsf_hal_pre_startup_init();
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
