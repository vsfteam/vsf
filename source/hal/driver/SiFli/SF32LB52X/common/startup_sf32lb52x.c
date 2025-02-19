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
#if __IS_COMPILER_IAR__ && __VER__ < 9060003
__attribute__((naked))
#endif
__NO_RETURN Reset_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

#define __INTERRUPTS                                                            \
    AON_IRQHandler,                                                             \
    ble_isr,                                                                    \
    DMAC2_CH1_IRQHandler,                                                       \
    DMAC2_CH2_IRQHandler,                                                       \
    DMAC2_CH3_IRQHandler,                                                       \
    DMAC2_CH4_IRQHandler,                                                       \
    DMAC2_CH5_IRQHandler,                                                       \
    DMAC2_CH6_IRQHandler,                                                       \
    DMAC2_CH7_IRQHandler,                                                       \
    DMAC2_CH8_IRQHandler,                                                       \
    PATCH_IRQHandler,                                                           \
    dm_isr,                                                                     \
    USART4_IRQHandler,                                                          \
    USART5_IRQHandler,                                                          \
    SECU2_IRQHandler,                                                           \
    bt_isr,                                                                     \
    BTIM3_IRQHandler,                                                           \
    BTIM4_IRQHandler,                                                           \
    PTC2_IRQHandler,                                                            \
    LPTIM3_IRQHandler,                                                          \
    GPIO2_IRQHandler,                                                           \
    HPSYS0_IRQHandler,                                                          \
    HPSYS1_IRQHandler,                                                          \
    SWI0_IRQHandler,                                                            \
    SWI1_IRQHandler,                                                            \
    SWI2_IRQHandler,                                                            \
    SWI3_IRQHandler,                                                            \
    SWI4_IRQHandler,                                                            \
    SWI5_IRQHandler,                                                            \
    SWI6_IRQHandler,                                                            \
    SWI7_IRQHandler,                                                            \
    SWI8_IRQHandler,                                                            \
    SWI9_IRQHandler,                                                            \
    SWI10_IRQHandler,                                                           \
    SWI11_IRQHandler,                                                           \
    SWI12_IRQHandler,                                                           \
    SWI13_IRQHandler,                                                           \
    SWI14_IRQHandler,                                                           \
    SWI15_IRQHandler,                                                           \
    SWI16_IRQHandler,                                                           \
    SWI17_IRQHandler,                                                           \
    SWI18_IRQHandler,                                                           \
    SWI19_IRQHandler,                                                           \
    SWI20_IRQHandler,                                                           \
    SWI21_IRQHandler,                                                           \
    SWI22_IRQHandler,                                                           \
    LPTIM1_IRQHandler,                                                          \
    LPTIM2_IRQHandler,                                                          \
    PMUC_IRQHandler,                                                            \
    RTC_IRQHandler,                                                             \
    DMAC1_CH1_IRQHandler,                                                       \
    DMAC1_CH2_IRQHandler,                                                       \
    DMAC1_CH3_IRQHandler,                                                       \
    DMAC1_CH4_IRQHandler,                                                       \
    DMAC1_CH5_IRQHandler,                                                       \
    DMAC1_CH6_IRQHandler,                                                       \
    DMAC1_CH7_IRQHandler,                                                       \
    DMAC1_CH8_IRQHandler,                                                       \
    LCPU2HCPU_IRQHandler,                                                       \
    USART1_IRQHandler,                                                          \
    SPI1_IRQHandler,                                                            \
    I2C1_IRQHandler,                                                            \
    EPIC_IRQHandler,                                                            \
    LCDC1_IRQHandler,                                                           \
    I2S1_IRQHandler,                                                            \
    GPADC_IRQHandler,                                                           \
    EFUSEC_IRQHandler,                                                          \
    AES_IRQHandler,                                                             \
    PTC1_IRQHandler,                                                            \
    TRNG_IRQHandler,                                                            \
    GPTIM1_IRQHandler,                                                          \
    GPTIM2_IRQHandler,                                                          \
    BTIM1_IRQHandler,                                                           \
    BTIM2_IRQHandler,                                                           \
    USART2_IRQHandler,                                                          \
    SPI2_IRQHandler,                                                            \
    I2C2_IRQHandler,                                                            \
    EXTDMA_IRQHandler,                                                          \
    I2C4_IRQHandler,                                                            \
    SDMMC1_IRQHandler,                                                          \
    SWI23_IRQHandler,                                                           \
    SWI24_IRQHandler,                                                           \
    PDM1_IRQHandler,                                                            \
    SWI25_IRQHandler,                                                           \
    GPIO1_IRQHandler,                                                           \
    QSPI1_IRQHandler,                                                           \
    QSPI2_IRQHandler,                                                           \
    SWI26_IRQHandler,                                                           \
    SWI27_IRQHandler,                                                           \
    EZIP_IRQHandler,                                                            \
    AUDPRC_IRQHandler,                                                          \
    TSEN_IRQHandler,                                                            \
    USBC_IRQHandler,                                                            \
    I2C3_IRQHandler,                                                            \
    ATIM1_IRQHandler,                                                           \
    USART3_IRQHandler,                                                          \
    AUD_HP_IRQHandler,                                                          \
    SWI28_IRQHandler,                                                           \
    SECU1_IRQHandler

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

    __INTERRUPTS
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
    __INTERRUPTS
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
#if __IS_COMPILER_IAR__ && __VER__ < 9060003
__attribute__((naked))
#endif
__NO_RETURN Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    __set_MSPLIM((uintptr_t)&__STACK_LIMIT);

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
