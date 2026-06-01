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

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM                         4
#   define VSF_ARCH_PRI_BIT                         2

// Software interrupts provided by dedicated device IRQs. RP2040 NVIC slots
// 26..31 (SPARE_IRQ_0..5) are not wired to any peripheral and are usable as
// software interrupts. Each one gives the EDA scheduler an extra preemption
// level on top of PendSV.
//
// Cortex-M0+ implements only VSF_ARCH_PRI_BIT==2 priority bits => 4 distinct
// NVIC priorities (VSF_ARCH_PRI_NUM==4). The kernel can therefore use at most
// 4 preemptible levels (PendSV + 3 device SWIs); with 4 in use the systimer
// shares the top level. That is the hardware ceiling: more device SWIs cannot
// become additional preemption levels (unlike Cortex-M4 parts such as
// AT32F405 with 3 priority bits / 8 levels). We expose exactly 3 here.
//
// The IRQ numbers are listed in VSF_DEV_SWI_LIST (main section below) and
// routed to SWIn_IRQHandler in the vector table (startup_RP2040.c).
#   define VSF_DEV_SWI_NUM                          3

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "RP2040.h"

#else

#ifndef __HAL_DEVICE_RASPBERRYPI_RP2040_H__
#define __HAL_DEVICE_RASPBERRYPI_RP2040_H__

// NVIC IRQ numbers backing the device software interrupts declared by
// VSF_DEV_SWI_NUM, in SWI0,SWI1,SWI2 order. SPARE_IRQ_0..2 == 26,27,28 (CMSIS
// IRQn_Type stops at RTC_IRQ=25, so the spare slots have no symbolic name —
// use the literals).
#define VSF_DEV_SWI_LIST                            26, 27, 28

/*============================ INCLUDES ======================================*/

// for XXXX_BASE
#include "hardware/regs/addressmap.h"
#include "hardware/regs/resets.h"

// Vendor peripheral headers — centralized here so individual driver .c files
// don't pull them in directly. Anything a driver reaches for as `xxx_hw` /
// `XXX_<FIELD>_BITS` is provided through this block.
#include "hardware/structs/adc.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/dma.h"
#include "hardware/structs/io_bank0.h"
#include "hardware/structs/pads_bank0.h"
#include "hardware/structs/pwm.h"
#include "hardware/structs/resets.h"
#include "hardware/structs/rosc.h"
#include "hardware/structs/rtc.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/spi.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/watchdog.h"

#include "hardware/regs/adc.h"
#include "hardware/regs/dma.h"
#include "hardware/regs/psm.h"
#include "hardware/regs/pwm.h"
#include "hardware/regs/rtc.h"
#include "hardware/regs/spi.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/watchdog.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

#define USB_OTG0_IRQHandler         USBDMA_IRQHandler
#define USB_OTG0_CONFIG                                                         \
    .dc_ep_num = 4 << 1, .hc_ep_num = 5,                                        \
    .reg = (void *)0, /* vk_dwcotg_hw_info_t */                                 \
        .buffer_word_size = 948, .speed = USB_SPEED_FULL, .dma_en = true,       \
    .ulpi_en = true, .utmi_en = false, .vbus_en = false,

#define VSF_HW_I2C_COUNT            2
#define VSF_HW_I2C0_IRQN            I2C0_IRQ_IRQn
#define VSF_HW_I2C0_IRQHandler      I2C0_IRQHandler
#define VSF_HW_I2C0_REG             I2C0_BASE
#define VSF_HW_I2C0_RST_BIT         (1u << RESET_I2C0)
#define VSF_HW_I2C1_IRQN            I2C1_IRQ_IRQn
#define VSF_HW_I2C1_IRQHandler      I2C1_IRQHandler
#define VSF_HW_I2C1_REG             I2C1_BASE
#define VSF_HW_I2C1_RST_BIT         (1u << RESET_I2C1)

#define VSF_HW_USART_COUNT          2
#define VSF_HW_USART0_IRQN          UART0_IRQ_IRQn
#define VSF_HW_USART0_IRQHandler    UART0_IRQHandler
#define VSF_HW_USART0_REG           UART0_BASE
#define VSF_HW_USART0_RST_BIT       (1u << RESET_UART0)
#define VSF_HW_USART1_IRQN          UART1_IRQ_IRQn
#define VSF_HW_USART1_IRQHandler    UART1_IRQHandler
#define VSF_HW_USART1_REG           UART1_BASE
#define VSF_HW_USART1_RST_BIT       (1u << RESET_UART1)

/* DMA DREQ (data request) signals for RP2040 peripherals.
 * Values from RP2040 datasheet Table 124. */
#define VSF_HW_USART0_TX_DREQ       20
#define VSF_HW_USART0_RX_DREQ       21
#define VSF_HW_USART1_TX_DREQ       22
#define VSF_HW_USART1_RX_DREQ       23

// RP2040 has a single GPIO bank (BANK0) with 30 pins (GP0..GP29).
// EXTI lives inside IO_BANK0 (per-pin INTR/PROC0_INTE), not a separate IP.
// The minimal driver implements the digital GPIO subset; exti_irq_* return
// VSF_ERR_NOT_SUPPORT until EXTI support is added.
#define VSF_HW_GPIO_PORT_COUNT      1
#define VSF_HW_GPIO_PIN_COUNT       30
#define VSF_HW_GPIO_PIN_MASK        0x3FFFFFFFu
#define VSF_HW_GPIO0_IRQN           IO_IRQ_BANK0_IRQn

// Enable VSF_GPIO_PORT_PIN macros (VSF_PA0..VSF_PA29) and the
// vsf_hw_gpio_ports_config_pin batch API.
#define VSF_GPIO_CFG_PORT0
#define VSF_GPIO_CFG_PIN_COUNT      30

#define VSF_HW_RTC_COUNT            1
#define VSF_HW_RTC0_IRQN            RTC_IRQ_IRQn
#define VSF_HW_RTC0_IRQHandler      RTC_IRQHandler
#define VSF_HW_RTC0_REG             RTC_BASE
#define VSF_HW_RTC0_RST_BIT         (1u << RESET_RTC)

#define VSF_HW_FLASH_COUNT          1
#define VSF_HW_FLASH0_SIZE          (2 * 1024 * 1024)
#define VSF_HW_FLASH0_SECTOR_SIZE   4096
#define VSF_HW_FLASH0_PAGE_SIZE     256
#define VSF_HW_FLASH0_SECTOR_NUM    512
#define VSF_HW_FLASH0_BLOCK_SIZE    65536
#define VSF_HW_FLASH0_XIP_BASE      XIP_BASE

#define VSF_HW_WDT_COUNT            1
#define VSF_HW_WDT0_REG             WATCHDOG_BASE

#define VSF_HW_ADC_COUNT            1
#define VSF_HW_ADC0_REG             ADC_BASE
#define VSF_HW_ADC0_RST_BIT         (1u << RESET_ADC)
#define VSF_HW_ADC0_IRQN            ADC_IRQ_FIFO_IRQn
#define VSF_HW_ADC0_IRQHandler      ADC_IRQ_FIFO_IRQHandler
#define VSF_HW_ADC_CHANNEL_COUNT            5
#define VSF_HW_ADC_TEMP_SENSOR_CHANNEL      4
#define VSF_HW_ADC_MAX_DATA_BITS            12

#define VSF_HW_PWM_COUNT            8
#define VSF_HW_PWM0_IRQN            PWM_IRQ_WRAP_IRQn
#define VSF_HW_PWM_IRQHandler       PWM_IRQ_WRAP_IRQHandler
#define VSF_HW_PWM0_REG             PWM_BASE
#define VSF_HW_PWM0_RST_BIT         (1u << RESET_PWM)
#define VSF_HW_PWM1_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM1_REG             PWM_BASE
#define VSF_HW_PWM1_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM2_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM2_REG             PWM_BASE
#define VSF_HW_PWM2_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM3_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM3_REG             PWM_BASE
#define VSF_HW_PWM3_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM4_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM4_REG             PWM_BASE
#define VSF_HW_PWM4_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM5_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM5_REG             PWM_BASE
#define VSF_HW_PWM5_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM6_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM6_REG             PWM_BASE
#define VSF_HW_PWM6_RST_BIT         VSF_HW_PWM0_RST_BIT
#define VSF_HW_PWM7_IRQN            VSF_HW_PWM0_IRQN
#define VSF_HW_PWM7_REG             PWM_BASE
#define VSF_HW_PWM7_RST_BIT         VSF_HW_PWM0_RST_BIT

#define VSF_HW_TIMER_COUNT          4
#define VSF_HW_TIMER0_REG           TIMER_BASE
#define VSF_HW_TIMER1_REG           TIMER_BASE
#define VSF_HW_TIMER2_REG           TIMER_BASE
#define VSF_HW_TIMER3_REG           TIMER_BASE
#define VSF_HW_TIMER0_IRQN          TIMER_IRQ_0_IRQn
#define VSF_HW_TIMER1_IRQN          TIMER_IRQ_1_IRQn
#define VSF_HW_TIMER2_IRQN          TIMER_IRQ_2_IRQn
#define VSF_HW_TIMER3_IRQN          TIMER_IRQ_3_IRQn
#define VSF_HW_TIMER0_IRQHandler    TIMER_IRQ_0_IRQHandler
#define VSF_HW_TIMER1_IRQHandler    TIMER_IRQ_1_IRQHandler
#define VSF_HW_TIMER2_IRQHandler    TIMER_IRQ_2_IRQHandler
#define VSF_HW_TIMER3_IRQHandler    TIMER_IRQ_3_IRQHandler
#define VSF_HW_TIMER_CHANNEL_COUNT  2

#define VSF_HW_SPI_COUNT            2
#define VSF_HW_SPI0_IRQN            SPI0_IRQ_IRQn
#define VSF_HW_SPI0_IRQHandler      SPI0_IRQHandler
#define VSF_HW_SPI0_REG             SPI0_BASE
#define VSF_HW_SPI0_RST_BIT         (1u << RESET_SPI0)
#define VSF_HW_SPI1_IRQN            SPI1_IRQ_IRQn
#define VSF_HW_SPI1_IRQHandler      SPI1_IRQHandler
#define VSF_HW_SPI1_REG             SPI1_BASE
#define VSF_HW_SPI1_RST_BIT         (1u << RESET_SPI1)

#define VSF_HW_RNG_COUNT            1
#define VSF_HW_RNG_BITLEN           32

#define VSF_HW_DMA_COUNT            1
#define VSF_HW_DMA_MASK             0x1
#define VSF_HW_DMA_CHANNEL_NUM      12
#define VSF_HW_DMA0_REG             DMA_BASE
#define VSF_HW_DMA0_RST_BIT         (1u << RESET_DMA)
#define VSF_HW_DMA0_IRQN            DMA_IRQ_0_IRQn
#define VSF_HW_DMA0_IRQN_0          VSF_HW_DMA0_IRQN
#define VSF_HW_DMA0_IRQN_1          DMA_IRQ_1_IRQn
#define VSF_HW_DMA0_IRQHandler      VSF_HW_DMA0_IRQ_0_Handler
#define VSF_HW_DMA0_IRQ_0_Handler   DMA_IRQ_0_Handler
#define VSF_HW_DMA0_IRQ_1_Handler   DMA_IRQ_1_Handler
#define VSF_HW_DMA0_IRQ_Handler_COUNT   2

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_RASPBERRYPI_RP2040_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
