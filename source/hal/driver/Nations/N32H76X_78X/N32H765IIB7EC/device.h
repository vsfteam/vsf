/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ is defined to include arch information only.
 *      Some arch provide chip-vendor with some vendor-specified options, define these options here.
 */

//! arch info
//  for ARM, number of interrupt priority is vendor-specified, so define here

#   define VSF_ARCH_PRI_NUM                         16
#   define VSF_ARCH_PRI_BIT                         4

// software interrupt provided by a dedicated device
#   ifndef VSF_DEV_SWI_NUM
#       define VSF_DEV_SWI_NUM                      11
#   endif
#   if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#       warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#       undef VSF_DEV_SWI_NUM
#       define VSF_DEV_SWI_NUM                      VSF_ARCH_PRI_NUM
#   endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   if !defined(CORE_CM4) && !defined(CORE_CM7)
#       define CORE_CM7
#   endif

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "../common/vendor/n32h76x_78x.h"

// SWI
#   define SWI0_IRQn                                53
#   define SWI1_IRQn                                54
#   define SWI2_IRQn                                134
#   define SWI3_IRQn                                166
#   define SWI4_IRQn                                167
#   define SWI5_IRQn                                173
#   define SWI6_IRQn                                187
#   define SWI7_IRQn                                188
#   define SWI8_IRQn                                189
#   define SWI9_IRQn                                190
#   define SWI10_IRQn                               191

#else

/*\note __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ and __VSF_HAL_SHOW_VENDOR_INFO__ are not defined.
 *      Define device information here.
 */

#ifndef __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__
#define __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                    VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST            VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included if implemented.
        The path of common.h is up to developer to decide.
*/

#if !defined(CORE_CM4) && !defined(CORE_CM7)
#   define CORE_CM7
#endif
#include "../common/common.h"

/*============================ MACROS ========================================*/

/*\note For specified peripheral, VSF_HW_PERIPHERIAL_COUNT MUST be defined as number of peripheral instance.
 *      If peripheral instances start from 0, and are in sequence order(eg, 3 I2Cs: I2C0, I2C1, I2C2), VSF_HW_PERIPHERIAL_MASK is not needed.
 *      Otherwise, define VSF_HW_PERIPHERIAL_MASK to indicate which peripheral instances to implmenent.
 *      eg: 3 I2Cs: I2C0, I2C2, I2C4, define VSF_HW_I2C_MASK to 0x15(BIT(0) | BIT(2)) | BIT(4)).
 *
 *      Other configurations are vendor specified, drivers will use these information to generate peripheral instances.
 *      Usually need irqn, irqhandler, peripheral clock enable bits, peripheral reset bites, etc.
 */

// Interrupts

#define VSF_HW_INTERRUPTS                                                       \
    WWDG1_IRQHandler,                                                           \
    PVD_IRQHandler,                                                             \
    RTC_TAMPER_IRQHandler,                                                      \
    RTC_WKUP_IRQHandler,                                                        \
    RCC_IRQHandler,                                                             \
    EXTI0_IRQHandler,                                                           \
    EXTI1_IRQHandler,                                                           \
    EXTI2_IRQHandler,                                                           \
    EXTI3_IRQHandler,                                                           \
    EXTI4_IRQHandler,                                                           \
    EXTI9_5_IRQHandler,                                                         \
    EXTI15_10_IRQHandler,                                                       \
    DMA1_Channel0_IRQHandler,                                                   \
    DMA1_Channel1_IRQHandler,                                                   \
    DMA1_Channel2_IRQHandler,                                                   \
    DMA1_Channel3_IRQHandler,                                                   \
    DMA1_Channel4_IRQHandler,                                                   \
    DMA1_Channel5_IRQHandler,                                                   \
    DMA1_Channel6_IRQHandler,                                                   \
    DMA1_Channel7_IRQHandler,                                                   \
    DMA2_Channel0_IRQHandler,                                                   \
    DMA2_Channel1_IRQHandler,                                                   \
    DMA2_Channel2_IRQHandler,                                                   \
    DMA2_Channel3_IRQHandler,                                                   \
    DMA2_Channel4_IRQHandler,                                                   \
    DMA2_Channel5_IRQHandler,                                                   \
    DMA2_Channel6_IRQHandler,                                                   \
    DMA2_Channel7_IRQHandler,                                                   \
    DMA3_Channel0_IRQHandler,                                                   \
    DMA3_Channel1_IRQHandler,                                                   \
    DMA3_Channel2_IRQHandler,                                                   \
    DMA3_Channel3_IRQHandler,                                                   \
    DMA3_Channel4_IRQHandler,                                                   \
    DMA3_Channel5_IRQHandler,                                                   \
    DMA3_Channel6_IRQHandler,                                                   \
    DMA3_Channel7_IRQHandler,                                                   \
    MDMA_Channel0_IRQHandler,                                                   \
    MDMA_Channel1_IRQHandler,                                                   \
    MDMA_Channel2_IRQHandler,                                                   \
    MDMA_Channel3_IRQHandler,                                                   \
    MDMA_Channel4_IRQHandler,                                                   \
    MDMA_Channel5_IRQHandler,                                                   \
    MDMA_Channel6_IRQHandler,                                                   \
    MDMA_Channel7_IRQHandler,                                                   \
    MDMA_Channel8_IRQHandler,                                                   \
    MDMA_Channel9_IRQHandler,                                                   \
    MDMA_Channel10_IRQHandler,                                                  \
    MDMA_Channel11_IRQHandler,                                                  \
    MDMA_Channel12_IRQHandler,                                                  \
    MDMA_Channel13_IRQHandler,                                                  \
    MDMA_Channel14_IRQHandler,                                                  \
    MDMA_Channel15_IRQHandler,                                                  \
    SDPU_IRQHandler,                                                            \
    SWI0_IRQHandler,                                                            \
    SWI1_IRQHandler,                                                            \
    FPU_CPU1_IRQHandler,                                                        \
    ECCMON_IRQHandler,                                                          \
    RTC_ALARM_IRQHandler,                                                       \
    I2C1_EV_IRQHandler,                                                         \
    I2C1_ER_IRQHandler,                                                         \
    I2C2_EV_IRQHandler,                                                         \
    I2C2_ER_IRQHandler,                                                         \
    I2C3_EV_IRQHandler,                                                         \
    I2C3_ER_IRQHandler,                                                         \
    I2C4_EV_IRQHandler,                                                         \
    I2C4_ER_IRQHandler,                                                         \
    I2C5_EV_IRQHandler,                                                         \
    I2C5_ER_IRQHandler,                                                         \
    I2C6_EV_IRQHandler,                                                         \
    I2C6_ER_IRQHandler,                                                         \
    I2C7_EV_IRQHandler,                                                         \
    I2C7_ER_IRQHandler,                                                         \
    I2C8_EV_IRQHandler,                                                         \
    I2C8_ER_IRQHandler,                                                         \
    I2C9_EV_IRQHandler,                                                         \
    I2C9_ER_IRQHandler,                                                         \
    I2C10_EV_IRQHandler,                                                        \
    I2C10_ER_IRQHandler,                                                        \
    I2S1_IRQHandler,                                                            \
    I2S2_IRQHandler,                                                            \
    I2S3_IRQHandler,                                                            \
    I2S4_IRQHandler,                                                            \
    xSPI1_IRQHandler,                                                           \
    xSPI2_IRQHandler,                                                           \
    SPI1_IRQHandler,                                                            \
    SPI2_IRQHandler,                                                            \
    SPI3_IRQHandler,                                                            \
    SPI4_IRQHandler,                                                            \
    SPI5_IRQHandler,                                                            \
    SPI6_IRQHandler,                                                            \
    SPI7_IRQHandler,                                                            \
    LCD_EV_IRQHandler,                                                          \
    LCD_ER_IRQHandler,                                                          \
    DVP1_IRQHandler,                                                            \
    DVP2_IRQHandler,                                                            \
    DMAMUX2_IRQHandler,                                                         \
    USB1_HS_EPx_OUT_IRQHandler,                                                 \
    USB1_HS_EPx_IN_IRQHandler,                                                  \
    USB1_HS_WKUP_IRQHandler,                                                    \
    USB1_HS_IRQHandler,                                                         \
    USB2_HS_EPx_OUT_IRQHandler,                                                 \
    USB2_HS_EPx_IN_IRQHandler,                                                  \
    USB2_HS_WKUP_IRQHandler,                                                    \
    USB2_HS_IRQHandler,                                                         \
    ETH1_IRQHandler,                                                            \
    ETH1_PMT_LPI_IRQHandler,                                                    \
    ETH2_IRQHandler,                                                            \
    ETH2_PMT_LPI_IRQHandler,                                                    \
    FDCAN1_INT0_IRQHandler,                                                     \
    FDCAN2_INT0_IRQHandler,                                                     \
    FDCAN3_INT0_IRQHandler,                                                     \
    FDCAN4_INT0_IRQHandler,                                                     \
    FDCAN1_INT1_IRQHandler,                                                     \
    FDCAN2_INT1_IRQHandler,                                                     \
    FDCAN3_INT1_IRQHandler,                                                     \
    FDCAN4_INT1_IRQHandler,                                                     \
    USART1_IRQHandler,                                                          \
    USART2_IRQHandler,                                                          \
    USART3_IRQHandler,                                                          \
    USART4_IRQHandler,                                                          \
    USART5_IRQHandler,                                                          \
    USART6_IRQHandler,                                                          \
    USART7_IRQHandler,                                                          \
    USART8_IRQHandler,                                                          \
    UART9_IRQHandler,                                                           \
    UART10_IRQHandler,                                                          \
    UART11_IRQHandler,                                                          \
    UART12_IRQHandler,                                                          \
    UART13_IRQHandler,                                                          \
    UART14_IRQHandler,                                                          \
    UART15_IRQHandler,                                                          \
    LPUART1_IRQHandler,                                                         \
    LPUART2_IRQHandler,                                                         \
    GPU_IRQHandler,                                                             \
    SWI2_IRQHandler,                                                            \
    SDMMC1_IRQHandler,                                                          \
    SDMMC2_IRQHandler,                                                          \
    ADC1_IRQHandler,                                                            \
    ADC2_IRQHandler,                                                            \
    ADC3_IRQHandler,                                                            \
    COMP1_2_IRQHandler,                                                         \
    COMP3_4_IRQHandler,                                                         \
    SHRTIM1_INT1_IRQHandler,                                                    \
    SHRTIM1_INT2_IRQHandler,                                                    \
    SHRTIM1_INT3_IRQHandler,                                                    \
    SHRTIM1_INT4_IRQHandler,                                                    \
    SHRTIM1_INT5_IRQHandler,                                                    \
    SHRTIM1_INT6_IRQHandler,                                                    \
    SHRTIM1_INT7_IRQHandler,                                                    \
    SHRTIM1_INT8_IRQHandler,                                                    \
    SHRTIM2_INT1_IRQHandler,                                                    \
    SHRTIM2_INT2_IRQHandler,                                                    \
    SHRTIM2_INT3_IRQHandler,                                                    \
    SHRTIM2_INT4_IRQHandler,                                                    \
    SHRTIM2_INT5_IRQHandler,                                                    \
    SHRTIM2_INT6_IRQHandler,                                                    \
    SHRTIM2_INT7_IRQHandler,                                                    \
    SHRTIM2_INT8_IRQHandler,                                                    \
    FDCAN5_INT0_IRQHandler,                                                     \
    FDCAN6_INT0_IRQHandler,                                                     \
    FDCAN7_INT0_IRQHandler,                                                     \
    FDCAN8_INT0_IRQHandler,                                                     \
    FDCAN5_INT1_IRQHandler,                                                     \
    FDCAN6_INT1_IRQHandler,                                                     \
    FDCAN7_INT1_IRQHandler,                                                     \
    FDCAN8_INT1_IRQHandler,                                                     \
    SWI3_IRQHandler,                                                            \
    SWI4_IRQHandler,                                                            \
    LPTIM5_WKUP_IRQHandler,                                                     \
    JPEG_SGDMA_H2P_IRQHandler,                                                  \
    JPEG_SGDMA_P2H_IRQHandler,                                                  \
    WAKEUP_IO_IRQHandler,                                                       \
    SEMA4_INT1_IRQHandler,                                                      \
    SWI5_IRQHandler,                                                            \
    WWDG2_RST_IRQHandler,                                                       \
    OTPC_IRQHandler,                                                            \
    FEMC_IRQHandler,                                                            \
    DCMUB_IRQHandler,                                                           \
    DAC1_IRQHandler,                                                            \
    DAC2_IRQHandler,                                                            \
    MDMA_AHBS_ER_IRQHandler,                                                    \
    CM7_CATCH_READ_ER_IRQHandler,                                               \
    DAC3_IRQHandler,                                                            \
    DAC4_IRQHandler,                                                            \
    EMC_IRQHandler,                                                             \
    DAC5_IRQHandler,                                                            \
    DAC6_IRQHandler,                                                            \
    SWI6_IRQHandler,                                                            \
    SWI7_IRQHandler,                                                            \
    SWI8_IRQHandler,                                                            \
    SWI9_IRQHandler,                                                            \
    SWI10_IRQHandler,                                                           \
    ATIM1_BRK_IRQHandler,                                                       \
    ATIM1_TRG_COM_IRQHandler,                                                   \
    ATIM1_CC_IRQHandler,                                                        \
    ATIM1_UP_IRQHandler,                                                        \
    ATIM2_BRK_IRQHandler,                                                       \
    ATIM2_TRG_COM_IRQHandler,                                                   \
    ATIM2_CC_IRQHandler,                                                        \
    ATIM2_UP_IRQHandler,                                                        \
    ATIM3_BRK_IRQHandler,                                                       \
    ATIM3_TRG_COM_IRQHandler,                                                   \
    ATIM3_CC_IRQHandler,                                                        \
    ATIM3_UP_IRQHandler,                                                        \
    ATIM4_BRK_IRQHandler,                                                       \
    ATIM4_TRG_COM_IRQHandler,                                                   \
    ATIM4_CC_IRQHandler,                                                        \
    ATIM4_UP_IRQHandler,                                                        \
    GTIMA1_IRQHandler,                                                          \
    GTIMA2_IRQHandler,                                                          \
    GTIMA3_IRQHandler,                                                          \
    GTIMA4_IRQHandler,                                                          \
    GTIMA5_IRQHandler,                                                          \
    GTIMA6_IRQHandler,                                                          \
    GTIMA7_IRQHandler,                                                          \
    GTIMB1_IRQHandler,                                                          \
    GTIMB2_IRQHandler,                                                          \
    GTIMB3_IRQHandler,                                                          \
    BTIM1_IRQHandler,                                                           \
    BTIM2_IRQHandler,                                                           \
    BTIM3_IRQHandler,                                                           \
    BTIM4_IRQHandler,                                                           \
    LPTIM1_WKUP_IRQHandler,                                                     \
    LPTIM2_WKUP_IRQHandler,                                                     \
    LPTIM3_WKUP_IRQHandler,                                                     \
    LPTIM4_WKUP_IRQHandler,                                                     \
    DSMU_FLT0_IRQHandler,                                                       \
    DSMU_FLT1_IRQHandler,                                                       \
    DSMU_FLT2_IRQHandler,                                                       \
    DSMU_FLT3_IRQHandler,                                                       \
    FMAC_IRQHandler,                                                            \
    CORDIC_IRQHandler,                                                          \
    DMAMUX1_IRQHandler,                                                         \
    MMU_IRQHandler

// FLASH

#define VSF_HW_FLASH_COUNT                          1

// GPIO

#define VSF_HW_GPIO_PORT_COUNT                      11
#define VSF_HW_GPIO_PIN_COUNT                       16
#define VSF_HW_GPIO_FUNCTION_MAX                    16
#define VSF_HW_GPIO_PORT0_REG                       GPIOA
#define VSF_HW_GPIO_PORT0_EN                        VSF_HW_EN_GPIOA
#define VSF_HW_GPIO_PORT0_RST                       VSF_HW_RST_GPIOA
#define VSF_HW_GPIO_PORT1_REG                       GPIOB
#define VSF_HW_GPIO_PORT1_EN                        VSF_HW_EN_GPIOB
#define VSF_HW_GPIO_PORT1_RST                       VSF_HW_RST_GPIOB
#define VSF_HW_GPIO_PORT2_REG                       GPIOC
#define VSF_HW_GPIO_PORT2_EN                        VSF_HW_EN_GPIOC
#define VSF_HW_GPIO_PORT2_RST                       VSF_HW_RST_GPIOC
#define VSF_HW_GPIO_PORT3_REG                       GPIOD
#define VSF_HW_GPIO_PORT3_EN                        VSF_HW_EN_GPIOD
#define VSF_HW_GPIO_PORT3_RST                       VSF_HW_RST_GPIOD
#define VSF_HW_GPIO_PORT4_REG                       GPIOE
#define VSF_HW_GPIO_PORT4_EN                        VSF_HW_EN_GPIOE
#define VSF_HW_GPIO_PORT4_RST                       VSF_HW_RST_GPIOE
#define VSF_HW_GPIO_PORT5_REG                       GPIOF
#define VSF_HW_GPIO_PORT5_EN                        VSF_HW_EN_GPIOF
#define VSF_HW_GPIO_PORT5_RST                       VSF_HW_RST_GPIOF
#define VSF_HW_GPIO_PORT6_REG                       GPIOG
#define VSF_HW_GPIO_PORT6_EN                        VSF_HW_EN_GPIOG
#define VSF_HW_GPIO_PORT6_RST                       VSF_HW_RST_GPIOG
#define VSF_HW_GPIO_PORT7_REG                       GPIOH
#define VSF_HW_GPIO_PORT7_EN                        VSF_HW_EN_GPIOH
#define VSF_HW_GPIO_PORT7_RST                       VSF_HW_RST_GPIOH
#define VSF_HW_GPIO_PORT9_REG                       GPIOJ
#define VSF_HW_GPIO_PORT9_EN                        VSF_HW_EN_GPIOJ
#define VSF_HW_GPIO_PORT9_RST                       VSF_HW_RST_GPIOJ
#define VSF_HW_GPIO_PORT10_REG                      GPIOK
#define VSF_HW_GPIO_PORT10_EN                       VSF_HW_EN_GPIOK
#define VSF_HW_GPIO_PORT10_RST                      VSF_HW_RST_GPIOK

// USART UART

#define VSF_HW_USART_COUNT                          15
#define VSF_HW_USART_MASK                           0xFFFE
#define VSF_HW_USART1_REG                           USART1
#define VSF_HW_USART1_CLK                           VSF_HW_CLK_USART1
#define VSF_HW_USART1_EN                            VSF_HW_EN_USART1
#define VSF_HW_USART1_RST                           VSF_HW_RST_USART1
#define VSF_HW_USART1_SYNC                          true
#define VSF_HW_USART1_IRQN                          USART1_IRQn
#define VSF_HW_USART1_IRQHandler                    USART1_IRQHandler
#define VSF_HW_USART2_REG                           USART2
#define VSF_HW_USART2_CLK                           VSF_HW_CLK_USART2
#define VSF_HW_USART2_EN                            VSF_HW_EN_USART2
#define VSF_HW_USART2_RST                           VSF_HW_RST_USART2
#define VSF_HW_USART2_SYNC                          true
#define VSF_HW_USART2_IRQN                          USART2_IRQn
#define VSF_HW_USART2_IRQHandler                    USART2_IRQHandler
#define VSF_HW_USART3_REG                           USART3
#define VSF_HW_USART3_CLK                           VSF_HW_CLK_USART3
#define VSF_HW_USART3_EN                            VSF_HW_EN_USART3
#define VSF_HW_USART3_RST                           VSF_HW_RST_USART3
#define VSF_HW_USART3_SYNC                          true
#define VSF_HW_USART3_IRQN                          USART3_IRQn
#define VSF_HW_USART3_IRQHandler                    USART3_IRQHandler
#define VSF_HW_USART4_REG                           USART4
#define VSF_HW_USART4_CLK                           VSF_HW_CLK_USART4
#define VSF_HW_USART4_EN                            VSF_HW_EN_USART4
#define VSF_HW_USART4_RST                           VSF_HW_RST_USART4
#define VSF_HW_USART4_SYNC                          true
#define VSF_HW_USART4_IRQN                          USART4_IRQn
#define VSF_HW_USART4_IRQHandler                    USRT4_IRQHandler
#define VSF_HW_USART5_REG                           USART5
#define VSF_HW_USART5_CLK                           VSF_HW_CLK_USART5
#define VSF_HW_USART5_EN                            VSF_HW_EN_USART5
#define VSF_HW_USART5_RST                           VSF_HW_RST_USART5
#define VSF_HW_USART5_SYNC                          true
#define VSF_HW_USART5_IRQN                          USART5_IRQn
#define VSF_HW_USART5_IRQHandler                    USART5_IRQHandler
#define VSF_HW_USART6_REG                           USART6
#define VSF_HW_USART6_CLK                           VSF_HW_CLK_USART6
#define VSF_HW_USART6_EN                            VSF_HW_EN_USART6
#define VSF_HW_USART6_RST                           VSF_HW_RST_USART6
#define VSF_HW_USART6_SYNC                          true
#define VSF_HW_USART6_IRQN                          USART6_IRQn
#define VSF_HW_USART6_IRQHandler                    USART6_IRQHandler
#define VSF_HW_USART7_REG                           USART7
#define VSF_HW_USART7_CLK                           VSF_HW_CLK_USART7
#define VSF_HW_USART7_EN                            VSF_HW_EN_USART7
#define VSF_HW_USART7_RST                           VSF_HW_RST_USART7
#define VSF_HW_USART7_SYNC                          true
#define VSF_HW_USART7_IRQN                          USART7_IRQn
#define VSF_HW_USART7_IRQHandler                    USART7_IRQHandler
#define VSF_HW_USART8_REG                           USART8
#define VSF_HW_USART8_CLK                           VSF_HW_CLK_USART8
#define VSF_HW_USART8_EN                            VSF_HW_EN_USART8
#define VSF_HW_USART8_RST                           VSF_HW_RST_USART8
#define VSF_HW_USART8_SYNC                          true
#define VSF_HW_USART8_IRQN                          USART8_IRQn
#define VSF_HW_USART8_IRQHandler                    USART8_IRQHandler
#define VSF_HW_USART9_REG                           UART9
#define VSF_HW_USART9_CLK                           VSF_HW_CLK_UART9
#define VSF_HW_USART9_EN                            VSF_HW_EN_UART9
#define VSF_HW_USART9_RST                           VSF_HW_RST_UART9
#define VSF_HW_USART9_SYNC                          false
#define VSF_HW_USART9_IRQN                          UART9_IRQn
#define VSF_HW_USART9_IRQHandler                    UART9_IRQHandler
#define VSF_HW_USART10_REG                          UART10
#define VSF_HW_USART10_CLK                          VSF_HW_CLK_UART10
#define VSF_HW_USART10_EN                           VSF_HW_EN_UART10
#define VSF_HW_USART10_RST                          VSF_HW_RST_UART10
#define VSF_HW_USART10_SYNC                         false
#define VSF_HW_USART10_IRQN                         UART10_IRQn
#define VSF_HW_USART10_IRQHandler                   UART10_IRQHandler
#define VSF_HW_USART11_REG                          UART11
#define VSF_HW_USART11_CLK                          VSF_HW_CLK_UART11
#define VSF_HW_USART11_EN                           VSF_HW_EN_UART11
#define VSF_HW_USART11_RST                          VSF_HW_RST_UART11
#define VSF_HW_USART11_SYNC                         false
#define VSF_HW_USART11_IRQN                         UART11_IRQn
#define VSF_HW_USART11_IRQHandler                   UART11_IRQHandler
#define VSF_HW_USART12_REG                          UART12
#define VSF_HW_USART12_CLK                          VSF_HW_CLK_UART12
#define VSF_HW_USART12_EN                           VSF_HW_EN_UART12
#define VSF_HW_USART12_RST                          VSF_HW_RST_UART12
#define VSF_HW_USART12_SYNC                         false
#define VSF_HW_USART12_IRQN                         UART12_IRQn
#define VSF_HW_USART12_IRQHandler                   UART12_IRQHandler
#define VSF_HW_USART13_REG                          UART13
#define VSF_HW_USART13_CLK                          VSF_HW_CLK_UART13
#define VSF_HW_USART13_EN                           VSF_HW_EN_UART13
#define VSF_HW_USART13_RST                          VSF_HW_RST_UART13
#define VSF_HW_USART13_SYNC                         false
#define VSF_HW_USART13_IRQN                         UART13_IRQn
#define VSF_HW_USART13_IRQHandler                   UART13_IRQHandler
#define VSF_HW_USART14_REG                          UART14
#define VSF_HW_USART14_CLK                          VSF_HW_CLK_UART14
#define VSF_HW_USART14_EN                           VSF_HW_EN_UART14
#define VSF_HW_USART14_RST                          VSF_HW_RST_UART14
#define VSF_HW_USART14_SYNC                         false
#define VSF_HW_USART14_IRQN                         UART14_IRQn
#define VSF_HW_USART14_IRQHandler                   UART14_IRQHandler
#define VSF_HW_USART15_REG                          UART15
#define VSF_HW_USART15_CLK                          VSF_HW_CLK_UART15
#define VSF_HW_USART15_EN                           VSF_HW_EN_UART15
#define VSF_HW_USART15_RST                          VSF_HW_RST_UART15
#define VSF_HW_USART15_SYNC                         false
#define VSF_HW_USART15_IRQN                         UART15_IRQn
#define VSF_HW_USART15_IRQHandler                   UART15_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __VSF_HAL_DEVICE_NATIONS_N32H765IIB7EC_H__
#endif
/* EOF */
