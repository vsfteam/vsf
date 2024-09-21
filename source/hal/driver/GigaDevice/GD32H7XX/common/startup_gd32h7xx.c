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

    WWDGT_IRQHandler,
    AVD_LVD_OVD_IRQHandler,
    TAMPER_STAMP_LXTAL_IRQHandler,
    RTC_WKUP_IRQHandler,
    FMC_IRQHandler,
    RCU_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA0_Channel0_IRQHandler,
    DMA0_Channel1_IRQHandler,
    DMA0_Channel2_IRQHandler,
    DMA0_Channel3_IRQHandler,
    DMA0_Channel4_IRQHandler,
    DMA0_Channel5_IRQHandler,
    DMA0_Channel6_IRQHandler,
    ADC0_1_IRQHandler,
    SWI0_IRQHandler,
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    SWI3_IRQHandler,
    EXTI5_9_IRQHandler,
    TIMER0_BRK_IRQHandler,
    TIMER0_UP_IRQHandler,
    TIMER0_TRG_CMT_IRQHandler,
    TIMER0_Channel_IRQHandler,
    TIMER1_IRQHandler,
    TIMER2_IRQHandler,
    TIMER3_IRQHandler,
    I2C0_EV_IRQHandler,
    I2C0_ER_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    SPI0_IRQHandler,
    SPI1_IRQHandler,
    USART0_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    EXTI10_15_IRQHandler,
    RTC_Alarm_IRQHandler,
    SWI4_IRQHandler,
    TIMER7_BRK_IRQHandler,
    TIMER7_UP_IRQHandler,
    TIMER7_TRG_CMT_IRQHandler,
    TIMER7_Channel_IRQHandler,
    DMA0_Channel7_IRQHandler,
    EXMC_IRQHandler,
    SDIO0_IRQHandler,
    TIMER4_IRQHandler,
    SPI2_IRQHandler,
    UART3_IRQHandler,
    UART4_IRQHandler,
    TIMER5_DAC_UDR_IRQHandler,
    TIMER6_IRQHandler,
    DMA1_Channel0_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    ENET0_IRQHandler,
    ENET0_WKUP_IRQHandler,
    SWI5_IRQHandler,
    SWI6_IRQHandler,
    SWI7_IRQHandler,
    SWI8_IRQHandler,
    SWI9_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    USART5_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    USBHS0_EP1_OUT_IRQHandler,
    USBHS0_EP1_IN_IRQHandler,
    USBHS0_WKUP_IRQHandler,
    USBHS0_IRQHandler,
    DCI_IRQHandler,
    CAU_IRQHandler,
    HAU_TRNG_IRQHandler,
    FPU_IRQHandler,
    UART6_IRQHandler,
    UART7_IRQHandler,
    SPI3_IRQHandler,
    SPI4_IRQHandler,
    SPI5_IRQHandler,
    SAI0_IRQHandler,
    TLI_IRQHandler,
    TLI_ER_IRQHandler,
    IPA_IRQHandler,
    SAI1_IRQHandler,
    OSPI0_IRQHandler,
    SWI10_IRQHandler,
    SWI11_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    RSPDIF_IRQHandler,
    SWI12_IRQHandler,
    SWI13_IRQHandler,
    SWI14_IRQHandler,
    SWI15_IRQHandler,
    DMAMUX_OVR_IRQHandler,
    SWI16_IRQHandler,
    SWI17_IRQHandler,
    SWI18_IRQHandler,
    SWI19_IRQHandler,
    SWI20_IRQHandler,
    SWI21_IRQHandler,
    SWI22_IRQHandler,
    HPDF_INT0_IRQHandler,
    HPDF_INT1_IRQHandler,
    HPDF_INT2_IRQHandler,
    HPDF_INT3_IRQHandler,
    SAI2_IRQHandler,
    SWI23_IRQHandler,
    TIMER14_IRQHandler,
    TIMER15_IRQHandler,
    TIMER16_IRQHandler,
    SWI24_IRQHandler,
    MDIO_IRQHandler,
    SWI25_IRQHandler,
    MDMA_IRQHandler,
    SWI26_IRQHandler,
    SDIO1_IRQHandler,
    HWSEM_IRQHandler,
    SWI27_IRQHandler,
    ADC2_IRQHandler,
    SWI28_IRQHandler,
    SWI29_IRQHandler,
    SWI30_IRQHandler,
    SWI31_IRQHandler,
    SWI32_IRQHandler,
    SWI33_IRQHandler,
    SWI34_IRQHandler,
    SWI35_IRQHandler,
    SWI36_IRQHandler,
    CMP0_1_IRQHandler,
    SWI37_IRQHandler,
    SWI38_IRQHandler,
    SWI39_IRQHandler,
    SWI40_IRQHandler,
    SWI41_IRQHandler,
    SWI42_IRQHandler,
    CTC_IRQHandler,
    RAMECCMU_IRQHandler,
    SWI43_IRQHandler,
    SWI44_IRQHandler,
    SWI45_IRQHandler,
    SWI46_IRQHandler,
    OSPI1_IRQHandler,
    RTDEC0_IRQHandler,
    RTDEC1_IRQHandler,
    FAC_IRQHandler,
    TMU_IRQHandler,
    SWI47_IRQHandler,
    SWI48_IRQHandler,
    SWI49_IRQHandler,
    SWI50_IRQHandler,
    SWI51_IRQHandler,
    SWI52_IRQHandler,
    TIMER22_IRQHandler,
    TIMER23_IRQHandler,
    TIMER30_IRQHandler,
    TIMER31_IRQHandler,
    TIMER40_IRQHandler,
    TIMER41_IRQHandler,
    TIMER42_IRQHandler,
    TIMER43_IRQHandler,
    TIMER44_IRQHandler,
    TIMER50_IRQHandler,
    TIMER51_IRQHandler,
    USBHS1_EP1_OUT_IRQHandler,
    USBHS1_EP1_IN_IRQHandler,
    USBHS1_WKUP_IRQHandler,
    USBHS1_IRQHandler,
    ENET1_IRQHandler,
    ENET1_WKUP_IRQHandler,
    SWI53_IRQHandler,
    CAN0_WKUP_IRQHandler,
    CAN0_Message_IRQHandler,
    CAN0_Busoff_IRQHandler,
    CAN0_Error_IRQHandler,
    CAN0_FastError_IRQHandler,
    CAN0_TEC_IRQHandler,
    CAN0_REC_IRQHandler,
    CAN1_WKUP_IRQHandler,
    CAN1_Message_IRQHandler,
    CAN1_Busoff_IRQHandler,
    CAN1_Error_IRQHandler,
    CAN1_FastError_IRQHandler,
    CAN1_TEC_IRQHandler,
    CAN1_REC_IRQHandler,
    CAN2_WKUP_IRQHandler,
    CAN2_Message_IRQHandler,
    CAN2_Busoff_IRQHandler,
    CAN2_Error_IRQHandler,
    CAN2_FastError_IRQHandler,
    CAN2_TEC_IRQHandler,
    CAN2_REC_IRQHandler,
    EFUSE_IRQHandler,
    I2C0_WKUP_IRQHandler,
    I2C1_WKUP_IRQHandler,
    I2C2_WKUP_IRQHandler,
    I2C3_WKUP_IRQHandler,
    LPDTS_IRQHandler,
    LPDTS_WKUP_IRQHandler,
    TIMER0_DEC_IRQHandler,
    TIMER7_DEC_IRQHandler,
    TIMER1_DEC_IRQHandler,
    TIMER2_DEC_IRQHandler,
    TIMER3_DEC_IRQHandler,
    TIMER4_DEC_IRQHandler,
    TIMER22_DEC_IRQHandler,
    TIMER23_DEC_IRQHandler,
    TIMER30_DEC_IRQHandler,
    TIMER31_DEC_IRQHandler
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
    WWDGT_IRQHandler,
    AVD_LVD_OVD_IRQHandler,
    TAMPER_STAMP_LXTAL_IRQHandler,
    RTC_WKUP_IRQHandler,
    FMC_IRQHandler,
    RCU_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA0_Channel0_IRQHandler,
    DMA0_Channel1_IRQHandler,
    DMA0_Channel2_IRQHandler,
    DMA0_Channel3_IRQHandler,
    DMA0_Channel4_IRQHandler,
    DMA0_Channel5_IRQHandler,
    DMA0_Channel6_IRQHandler,
    ADC0_1_IRQHandler,
    SWI0_IRQHandler,
    SWI1_IRQHandler,
    SWI2_IRQHandler,
    SWI3_IRQHandler,
    EXTI5_9_IRQHandler,
    TIMER0_BRK_IRQHandler,
    TIMER0_UP_IRQHandler,
    TIMER0_TRG_CMT_IRQHandler,
    TIMER0_Channel_IRQHandler,
    TIMER1_IRQHandler,
    TIMER2_IRQHandler,
    TIMER3_IRQHandler,
    I2C0_EV_IRQHandler,
    I2C0_ER_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    SPI0_IRQHandler,
    SPI1_IRQHandler,
    USART0_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    EXTI10_15_IRQHandler,
    RTC_Alarm_IRQHandler,
    SWI4_IRQHandler,
    TIMER7_BRK_IRQHandler,
    TIMER7_UP_IRQHandler,
    TIMER7_TRG_CMT_IRQHandler,
    TIMER7_Channel_IRQHandler,
    DMA0_Channel7_IRQHandler,
    EXMC_IRQHandler,
    SDIO0_IRQHandler,
    TIMER4_IRQHandler,
    SPI2_IRQHandler,
    UART3_IRQHandler,
    UART4_IRQHandler,
    TIMER5_DAC_UDR_IRQHandler,
    TIMER6_IRQHandler,
    DMA1_Channel0_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    ENET0_IRQHandler,
    ENET0_WKUP_IRQHandler,
    SWI5_IRQHandler,
    SWI6_IRQHandler,
    SWI7_IRQHandler,
    SWI8_IRQHandler,
    SWI9_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    USART5_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    USBHS0_EP1_OUT_IRQHandler,
    USBHS0_EP1_IN_IRQHandler,
    USBHS0_WKUP_IRQHandler,
    USBHS0_IRQHandler,
    DCI_IRQHandler,
    CAU_IRQHandler,
    HAU_TRNG_IRQHandler,
    FPU_IRQHandler,
    UART6_IRQHandler,
    UART7_IRQHandler,
    SPI3_IRQHandler,
    SPI4_IRQHandler,
    SPI5_IRQHandler,
    SAI0_IRQHandler,
    TLI_IRQHandler,
    TLI_ER_IRQHandler,
    IPA_IRQHandler,
    SAI1_IRQHandler,
    OSPI0_IRQHandler,
    SWI10_IRQHandler,
    SWI11_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    RSPDIF_IRQHandler,
    SWI12_IRQHandler,
    SWI13_IRQHandler,
    SWI14_IRQHandler,
    SWI15_IRQHandler,
    DMAMUX_OVR_IRQHandler,
    SWI16_IRQHandler,
    SWI17_IRQHandler,
    SWI18_IRQHandler,
    SWI19_IRQHandler,
    SWI20_IRQHandler,
    SWI21_IRQHandler,
    SWI22_IRQHandler,
    HPDF_INT0_IRQHandler,
    HPDF_INT1_IRQHandler,
    HPDF_INT2_IRQHandler,
    HPDF_INT3_IRQHandler,
    SAI2_IRQHandler,
    SWI23_IRQHandler,
    TIMER14_IRQHandler,
    TIMER15_IRQHandler,
    TIMER16_IRQHandler,
    SWI24_IRQHandler,
    MDIO_IRQHandler,
    SWI25_IRQHandler,
    MDMA_IRQHandler,
    SWI26_IRQHandler,
    SDIO1_IRQHandler,
    HWSEM_IRQHandler,
    SWI27_IRQHandler,
    ADC2_IRQHandler,
    SWI28_IRQHandler,
    SWI29_IRQHandler,
    SWI30_IRQHandler,
    SWI31_IRQHandler,
    SWI32_IRQHandler,
    SWI33_IRQHandler,
    SWI34_IRQHandler,
    SWI35_IRQHandler,
    SWI36_IRQHandler,
    CMP0_1_IRQHandler,
    SWI37_IRQHandler,
    SWI38_IRQHandler,
    SWI39_IRQHandler,
    SWI40_IRQHandler,
    SWI41_IRQHandler,
    SWI42_IRQHandler,
    CTC_IRQHandler,
    RAMECCMU_IRQHandler,
    SWI43_IRQHandler,
    SWI44_IRQHandler,
    SWI45_IRQHandler,
    SWI46_IRQHandler,
    OSPI1_IRQHandler,
    RTDEC0_IRQHandler,
    RTDEC1_IRQHandler,
    FAC_IRQHandler,
    TMU_IRQHandler,
    SWI47_IRQHandler,
    SWI48_IRQHandler,
    SWI49_IRQHandler,
    SWI50_IRQHandler,
    SWI51_IRQHandler,
    SWI52_IRQHandler,
    TIMER22_IRQHandler,
    TIMER23_IRQHandler,
    TIMER30_IRQHandler,
    TIMER31_IRQHandler,
    TIMER40_IRQHandler,
    TIMER41_IRQHandler,
    TIMER42_IRQHandler,
    TIMER43_IRQHandler,
    TIMER44_IRQHandler,
    TIMER50_IRQHandler,
    TIMER51_IRQHandler,
    USBHS1_EP1_OUT_IRQHandler,
    USBHS1_EP1_IN_IRQHandler,
    USBHS1_WKUP_IRQHandler,
    USBHS1_IRQHandler,
    ENET1_IRQHandler,
    ENET1_WKUP_IRQHandler,
    SWI53_IRQHandler,
    CAN0_WKUP_IRQHandler,
    CAN0_Message_IRQHandler,
    CAN0_Busoff_IRQHandler,
    CAN0_Error_IRQHandler,
    CAN0_FastError_IRQHandler,
    CAN0_TEC_IRQHandler,
    CAN0_REC_IRQHandler,
    CAN1_WKUP_IRQHandler,
    CAN1_Message_IRQHandler,
    CAN1_Busoff_IRQHandler,
    CAN1_Error_IRQHandler,
    CAN1_FastError_IRQHandler,
    CAN1_TEC_IRQHandler,
    CAN1_REC_IRQHandler,
    CAN2_WKUP_IRQHandler,
    CAN2_Message_IRQHandler,
    CAN2_Busoff_IRQHandler,
    CAN2_Error_IRQHandler,
    CAN2_FastError_IRQHandler,
    CAN2_TEC_IRQHandler,
    CAN2_REC_IRQHandler,
    EFUSE_IRQHandler,
    I2C0_WKUP_IRQHandler,
    I2C1_WKUP_IRQHandler,
    I2C2_WKUP_IRQHandler,
    I2C3_WKUP_IRQHandler,
    LPDTS_IRQHandler,
    LPDTS_WKUP_IRQHandler,
    TIMER0_DEC_IRQHandler,
    TIMER7_DEC_IRQHandler,
    TIMER1_DEC_IRQHandler,
    TIMER2_DEC_IRQHandler,
    TIMER3_DEC_IRQHandler,
    TIMER4_DEC_IRQHandler,
    TIMER22_DEC_IRQHandler,
    TIMER23_DEC_IRQHandler,
    TIMER30_DEC_IRQHandler,
    TIMER31_DEC_IRQHandler
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

VSF_CAL_WEAK(mpu_set_protection)
void mpu_set_protection(uint32_t baseaddr, uint32_t size, uint32_t rnum, uint8_t de, uint8_t ap, uint8_t sen, uint8_t cen, uint8_t ben)
{
    mpu_region_init_struct mpu_init_struct;
    mpu_region_struct_para_init(&mpu_init_struct);

    ARM_MPU_Disable();                                           /* 配置MPU之前先关闭MPU,配置完成以后再使能MPU */

    mpu_init_struct.region_number       = rnum;                  /* 设置保护区域编号 */
    mpu_init_struct.region_base_address = baseaddr;              /* 设置保护区域基地址 */
    mpu_init_struct.instruction_exec    = de;                    /* 是否允许指令访问 */
    mpu_init_struct.access_permission   = ap;                    /* 设置访问权限 */
    mpu_init_struct.tex_type            = MPU_TEX_TYPE0;         /* 设置TEX类型为type 0 */
    mpu_init_struct.access_shareable    = sen;                   /* 是否共用? */
    mpu_init_struct.access_cacheable    = cen;                   /* 是否cache? */
    mpu_init_struct.access_bufferable   = ben;                   /* 是否缓冲? */
    mpu_init_struct.subregion_disable   = 0X00;                  /* 禁止子区域 */
    mpu_init_struct.region_size         = size;                  /* 设置保护区域大小 */
    mpu_region_config(&mpu_init_struct);                         /* 配置MPU区域 */
    mpu_region_enable();                                         /* 使能MPU区域 */

    ARM_MPU_Enable(MPU_MODE_PRIV_DEFAULT);                       /* 设置完毕,使能MPU保护 */
}

VSF_CAL_WEAK(mpu_memory_protection)
void mpu_memory_protection(void)
{
    /* 保护整个ITCM,共64K字节 */
    mpu_set_protection( 0x00000000,                 /* 区域基地址 */
                        MPU_REGION_SIZE_64KB,       /* 区域大小 */
                        MPU_REGION_NUMBER0,         /* 区域编号 */
                        MPU_INSTRUCTION_EXEC_PERMIT,/* 允许指令访问 */
                        MPU_AP_FULL_ACCESS,         /* 全访问 */
                        MPU_ACCESS_NON_SHAREABLE,   /* 禁止共用 */
                        MPU_ACCESS_CACHEABLE,       /* 允许cache */
                        MPU_ACCESS_BUFFERABLE);     /* 允许缓冲 */

    /* 保护整个DTCM,共128K字节 */
    mpu_set_protection( 0x20000000,                 /* 区域基地址 */
                        MPU_REGION_SIZE_128KB,      /* 区域大小 */
                        MPU_REGION_NUMBER1,         /* 区域编号 */
                        MPU_INSTRUCTION_EXEC_PERMIT,/* 允许指令访问 */
                        MPU_AP_FULL_ACCESS,         /* 全访问 */
                        MPU_ACCESS_NON_SHAREABLE,   /* 禁止共用 */
                        MPU_ACCESS_CACHEABLE,       /* 允许cache */
                        MPU_ACCESS_BUFFERABLE);     /* 允许缓冲 */

    /* 保护整个AXI SRAM,共1024K字节 */
    mpu_set_protection( 0x24000000,                 /* 区域基地址 */
                        MPU_REGION_SIZE_1MB,        /* 区域大小 */
                        MPU_REGION_NUMBER2,         /* 区域编号 */
                        MPU_INSTRUCTION_EXEC_PERMIT,/* 允许指令访问 */
                        MPU_AP_FULL_ACCESS,         /* 全访问 */
                        MPU_ACCESS_SHAREABLE,       /* 允许共用 */
                        MPU_ACCESS_CACHEABLE,       /* 允许cache */
                        MPU_ACCESS_NON_BUFFERABLE); /* 禁止缓冲 */

    /* 保护整个SRAM0和SRAM1,共32K字节 */
    mpu_set_protection( 0x30000000,                 /* 区域基地址 */
                        MPU_REGION_SIZE_32KB,       /* 区域大小 */
                        MPU_REGION_NUMBER3,         /* 区域编号 */
                        MPU_INSTRUCTION_EXEC_PERMIT,/* 允许指令访问 */
                        MPU_AP_FULL_ACCESS,         /* 全访问 */
                        MPU_ACCESS_NON_SHAREABLE,   /* 禁止共用 */
                        MPU_ACCESS_CACHEABLE,       /* 允许cache */
                        MPU_ACCESS_BUFFERABLE);     /* 允许缓冲 */

    /* 保护SDRAM区域,共32M字节 */
    mpu_set_protection( 0xC0000000,                 /* 区域基地址 */
                        MPU_REGION_SIZE_32MB,       /* 区域大小 */
                        MPU_REGION_NUMBER4,         /* 区域编号 */
                        MPU_INSTRUCTION_EXEC_PERMIT,/* 允许指令访问 */
                        MPU_AP_FULL_ACCESS,         /* 全访问 */
                        MPU_ACCESS_NON_SHAREABLE,   /* 禁止共用 */
                        MPU_ACCESS_CACHEABLE,       /* 允许cache */
                        MPU_ACCESS_BUFFERABLE);     /* 允许缓冲 */
}

VSF_CAL_WEAK(vsf_hal_pre_startup_init)
void vsf_hal_pre_startup_init(void)
{
    SystemInit();
    SCB_EnableICache();
    SCB_EnableDCache();

    // force cache write through, will have strange behaviour if not set
    SCB->CACR |= 1 << 2;
    // also a MUST, will have strange behaviour if not called
    mpu_memory_protection();
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    vsf_arch_set_stack((uintptr_t)&__INITIAL_SP);
    vsf_hal_pre_startup_init();
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
