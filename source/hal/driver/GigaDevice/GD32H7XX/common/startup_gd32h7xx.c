/******************************************************************************
 * @file     startup_ARMCM77.c
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

#include "../__device.h"

#define __VSF_HAL_SHOW_VENDOR_INFO__
#include "hal/driver/driver.h"

// for VSF_MFOREACH
#include "utilities/vsf_utilities.h"

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

void __NO_RETURN Reset_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/

#define __INTERRUPTS                                                            \
    WWDGT_IRQHandler,                                                           \
    AVD_LVD_OVD_IRQHandler,                                                     \
    TAMPER_STAMP_LXTAL_IRQHandler,                                              \
    RTC_WKUP_IRQHandler,                                                        \
    FMC_IRQHandler,                                                             \
    RCU_IRQHandler,                                                             \
    EXTI0_IRQHandler,                                                           \
    EXTI1_IRQHandler,                                                           \
    EXTI2_IRQHandler,                                                           \
    EXTI3_IRQHandler,                                                           \
    EXTI4_IRQHandler,                                                           \
    DMA0_Channel0_IRQHandler,                                                   \
    DMA0_Channel1_IRQHandler,                                                   \
    DMA0_Channel2_IRQHandler,                                                   \
    DMA0_Channel3_IRQHandler,                                                   \
    DMA0_Channel4_IRQHandler,                                                   \
    DMA0_Channel5_IRQHandler,                                                   \
    DMA0_Channel6_IRQHandler,                                                   \
    ADC0_1_IRQHandler,                                                          \
    SWI0_IRQHandler,                                                            \
    SWI1_IRQHandler,                                                            \
    SWI2_IRQHandler,                                                            \
    SWI3_IRQHandler,                                                            \
    EXTI5_9_IRQHandler,                                                         \
    TIMER0_BRK_IRQHandler,                                                      \
    TIMER0_UP_IRQHandler,                                                       \
    TIMER0_TRG_CMT_IRQHandler,                                                  \
    TIMER0_Channel_IRQHandler,                                                  \
    TIMER1_IRQHandler,                                                          \
    TIMER2_IRQHandler,                                                          \
    TIMER3_IRQHandler,                                                          \
    I2C0_EV_IRQHandler,                                                         \
    I2C0_ER_IRQHandler,                                                         \
    I2C1_EV_IRQHandler,                                                         \
    I2C1_ER_IRQHandler,                                                         \
    SPI0_IRQHandler,                                                            \
    SPI1_IRQHandler,                                                            \
    USART0_IRQHandler,                                                          \
    USART1_IRQHandler,                                                          \
    USART2_IRQHandler,                                                          \
    EXTI10_15_IRQHandler,                                                       \
    RTC_Alarm_IRQHandler,                                                       \
    SWI4_IRQHandler,                                                            \
    TIMER7_BRK_IRQHandler,                                                      \
    TIMER7_UP_IRQHandler,                                                       \
    TIMER7_TRG_CMT_IRQHandler,                                                  \
    TIMER7_Channel_IRQHandler,                                                  \
    DMA0_Channel7_IRQHandler,                                                   \
    EXMC_IRQHandler,                                                            \
    SDIO0_IRQHandler,                                                           \
    TIMER4_IRQHandler,                                                          \
    SPI2_IRQHandler,                                                            \
    UART3_IRQHandler,                                                           \
    UART4_IRQHandler,                                                           \
    TIMER5_DAC_UDR_IRQHandler,                                                  \
    TIMER6_IRQHandler,                                                          \
    DMA1_Channel0_IRQHandler,                                                   \
    DMA1_Channel1_IRQHandler,                                                   \
    DMA1_Channel2_IRQHandler,                                                   \
    DMA1_Channel3_IRQHandler,                                                   \
    DMA1_Channel4_IRQHandler,                                                   \
    ENET0_IRQHandler,                                                           \
    ENET0_WKUP_IRQHandler,                                                      \
    SWI5_IRQHandler,                                                            \
    SWI6_IRQHandler,                                                            \
    SWI7_IRQHandler,                                                            \
    SWI8_IRQHandler,                                                            \
    SWI9_IRQHandler,                                                            \
    DMA1_Channel5_IRQHandler,                                                   \
    DMA1_Channel6_IRQHandler,                                                   \
    DMA1_Channel7_IRQHandler,                                                   \
    USART5_IRQHandler,                                                          \
    I2C2_EV_IRQHandler,                                                         \
    I2C2_ER_IRQHandler,                                                         \
    USBHS0_EP1_OUT_IRQHandler,                                                  \
    USBHS0_EP1_IN_IRQHandler,                                                   \
    USBHS0_WKUP_IRQHandler,                                                     \
    USBHS0_IRQHandler,                                                          \
    DCI_IRQHandler,                                                             \
    CAU_IRQHandler,                                                             \
    HAU_TRNG_IRQHandler,                                                        \
    FPU_IRQHandler,                                                             \
    UART6_IRQHandler,                                                           \
    UART7_IRQHandler,                                                           \
    SPI3_IRQHandler,                                                            \
    SPI4_IRQHandler,                                                            \
    SPI5_IRQHandler,                                                            \
    SAI0_IRQHandler,                                                            \
    TLI_IRQHandler,                                                             \
    TLI_ER_IRQHandler,                                                          \
    IPA_IRQHandler,                                                             \
    SAI1_IRQHandler,                                                            \
    OSPI0_IRQHandler,                                                           \
    SWI10_IRQHandler,                                                           \
    SWI11_IRQHandler,                                                           \
    I2C3_EV_IRQHandler,                                                         \
    I2C3_ER_IRQHandler,                                                         \
    RSPDIF_IRQHandler,                                                          \
    SWI12_IRQHandler,                                                           \
    SWI13_IRQHandler,                                                           \
    SWI14_IRQHandler,                                                           \
    SWI15_IRQHandler,                                                           \
    DMAMUX_OVR_IRQHandler,                                                      \
    SWI16_IRQHandler,                                                           \
    SWI17_IRQHandler,                                                           \
    SWI18_IRQHandler,                                                           \
    SWI19_IRQHandler,                                                           \
    SWI20_IRQHandler,                                                           \
    SWI21_IRQHandler,                                                           \
    SWI22_IRQHandler,                                                           \
    HPDF_INT0_IRQHandler,                                                       \
    HPDF_INT1_IRQHandler,                                                       \
    HPDF_INT2_IRQHandler,                                                       \
    HPDF_INT3_IRQHandler,                                                       \
    SAI2_IRQHandler,                                                            \
    SWI23_IRQHandler,                                                           \
    TIMER14_IRQHandler,                                                         \
    TIMER15_IRQHandler,                                                         \
    TIMER16_IRQHandler,                                                         \
    SWI24_IRQHandler,                                                           \
    MDIO_IRQHandler,                                                            \
    SWI25_IRQHandler,                                                           \
    MDMA_IRQHandler,                                                            \
    SWI26_IRQHandler,                                                           \
    SDIO1_IRQHandler,                                                           \
    HWSEM_IRQHandler,                                                           \
    SWI27_IRQHandler,                                                           \
    ADC2_IRQHandler,                                                            \
    SWI28_IRQHandler,                                                           \
    SWI29_IRQHandler,                                                           \
    SWI30_IRQHandler,                                                           \
    SWI31_IRQHandler,                                                           \
    SWI32_IRQHandler,                                                           \
    SWI33_IRQHandler,                                                           \
    SWI34_IRQHandler,                                                           \
    SWI35_IRQHandler,                                                           \
    SWI36_IRQHandler,                                                           \
    CMP0_1_IRQHandler,                                                          \
    SWI37_IRQHandler,                                                           \
    SWI38_IRQHandler,                                                           \
    SWI39_IRQHandler,                                                           \
    SWI40_IRQHandler,                                                           \
    SWI41_IRQHandler,                                                           \
    SWI42_IRQHandler,                                                           \
    CTC_IRQHandler,                                                             \
    RAMECCMU_IRQHandler,                                                        \
    SWI43_IRQHandler,                                                           \
    SWI44_IRQHandler,                                                           \
    SWI45_IRQHandler,                                                           \
    SWI46_IRQHandler,                                                           \
    OSPI1_IRQHandler,                                                           \
    RTDEC0_IRQHandler,                                                          \
    RTDEC1_IRQHandler,                                                          \
    FAC_IRQHandler,                                                             \
    TMU_IRQHandler,                                                             \
    SWI47_IRQHandler,                                                           \
    SWI48_IRQHandler,                                                           \
    SWI49_IRQHandler,                                                           \
    SWI50_IRQHandler,                                                           \
    SWI51_IRQHandler,                                                           \
    SWI52_IRQHandler,                                                           \
    TIMER22_IRQHandler,                                                         \
    TIMER23_IRQHandler,                                                         \
    TIMER30_IRQHandler,                                                         \
    TIMER31_IRQHandler,                                                         \
    TIMER40_IRQHandler,                                                         \
    TIMER41_IRQHandler,                                                         \
    TIMER42_IRQHandler,                                                         \
    TIMER43_IRQHandler,                                                         \
    TIMER44_IRQHandler,                                                         \
    TIMER50_IRQHandler,                                                         \
    TIMER51_IRQHandler,                                                         \
    USBHS1_EP1_OUT_IRQHandler,                                                  \
    USBHS1_EP1_IN_IRQHandler,                                                   \
    USBHS1_WKUP_IRQHandler,                                                     \
    USBHS1_IRQHandler,                                                          \
    ENET1_IRQHandler,                                                           \
    ENET1_WKUP_IRQHandler,                                                      \
    SWI53_IRQHandler,                                                           \
    CAN0_WKUP_IRQHandler,                                                       \
    CAN0_Message_IRQHandler,                                                    \
    CAN0_Busoff_IRQHandler,                                                     \
    CAN0_Error_IRQHandler,                                                      \
    CAN0_FastError_IRQHandler,                                                  \
    CAN0_TEC_IRQHandler,                                                        \
    CAN0_REC_IRQHandler,                                                        \
    CAN1_WKUP_IRQHandler,                                                       \
    CAN1_Message_IRQHandler,                                                    \
    CAN1_Busoff_IRQHandler,                                                     \
    CAN1_Error_IRQHandler,                                                      \
    CAN1_FastError_IRQHandler,                                                  \
    CAN1_TEC_IRQHandler,                                                        \
    CAN1_REC_IRQHandler,                                                        \
    CAN2_WKUP_IRQHandler,                                                       \
    CAN2_Message_IRQHandler,                                                    \
    CAN2_Busoff_IRQHandler,                                                     \
    CAN2_Error_IRQHandler,                                                      \
    CAN2_FastError_IRQHandler,                                                  \
    CAN2_TEC_IRQHandler,                                                        \
    CAN2_REC_IRQHandler,                                                        \
    EFUSE_IRQHandler,                                                           \
    I2C0_WKUP_IRQHandler,                                                       \
    I2C1_WKUP_IRQHandler,                                                       \
    I2C2_WKUP_IRQHandler,                                                       \
    I2C3_WKUP_IRQHandler,                                                       \
    LPDTS_IRQHandler,                                                           \
    LPDTS_WKUP_IRQHandler,                                                      \
    TIMER0_DEC_IRQHandler,                                                      \
    TIMER7_DEC_IRQHandler,                                                      \
    TIMER1_DEC_IRQHandler,                                                      \
    TIMER2_DEC_IRQHandler,                                                      \
    TIMER3_DEC_IRQHandler,                                                      \
    TIMER4_DEC_IRQHandler,                                                      \
    TIMER22_DEC_IRQHandler,                                                     \
    TIMER23_DEC_IRQHandler,                                                     \
    TIMER30_DEC_IRQHandler,                                                     \
    TIMER31_DEC_IRQHandler

VSF_MFOREACH(__imp_blocked_weak_handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
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
    __INTERRUPTS
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{
    SystemInit();

    // DO NOT enable cache here, it will make chip failure, no idea why
//    SCB_EnableICache();
//    SCB_EnableDCache();
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    __set_MSP((uintptr_t)&__INITIAL_SP);
    //! enable FPU before vsf_hal_pre_startup_init, in case vsf_hal_pre_startup_init uses FPU
    SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                   (3U << 11U*2U));           /* enable CP11 Full Access */
    SCB->VTOR = (uint32_t)__VECTOR_TABLE;

    vsf_hal_pre_startup_init();
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
