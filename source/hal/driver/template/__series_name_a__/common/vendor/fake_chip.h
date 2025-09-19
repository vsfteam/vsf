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

#ifndef __FAKE_CHIP_H__
#define __FAKE_CHIP_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

/*============================ MACROS ========================================*/

typedef enum IRQn_Type {
    Reset_IRQn                  = -15,
    NonMaskableInt_IRQn         = -14,
    HardFault_IRQn              = -13,
    MemoryManagement_IRQn       = -12,
    BusFault_IRQn               = -11,
    UsageFault_IRQn             = -10,
    SVCall_IRQn                 = -5,
    DebugMonitor_IRQn           = -4,
    PendSV_IRQn                 = -2,
    SysTick_IRQn                = -1,

    // Peripheral interrupts - grouped by peripheral type
    // ADC
    ADC0_IRQn           = 0,
    ADC1_IRQn           = 1,
    ADC2_IRQn           = 2,
    // DAC
    DAC0_IRQn           = 3,
    DAC1_IRQn           = 4,
    // DMA
    DMA0_IRQn           = 5,
    DMA1_IRQn           = 6,
    DMA2_IRQn           = 7,
    DMA3_IRQn           = 8,
    // ETH
    ETH0_IRQn           = 9,
    ETH1_IRQn           = 10,
    // FLASH
    FLASH0_IRQn         = 11,
    FLASH1_IRQn         = 12,
    // GPIO
    GPIO0_IRQn          = 13,
    GPIO1_IRQn          = 14,
    GPIO2_IRQn          = 15,
    GPIO3_IRQn          = 16,
    // I2C
    I2C0_IRQn           = 23,
    I2C1_IRQn           = 24,
    I2C2_IRQn           = 25,
    I2C3_IRQn           = 26,
    // I2S
    I2S0_IRQn           = 27,
    I2S1_IRQn           = 28,
    I2S2_IRQn           = 29,
    // PWM
    PWM0_IRQn           = 30,
    PWM1_IRQn           = 31,
    PWM2_IRQn           = 32,
    PWM3_IRQn           = 33,
    // RNG
    RNG0_IRQn           = 34,
    RNG1_IRQn           = 35,
    // RTC
    RTC0_IRQn           = 36,
    RTC1_IRQn           = 37,
    // SDIO
    SDIO0_IRQn          = 38,
    SDIO1_IRQn          = 39,
    // SPI
    SPI0_IRQn           = 40,
    SPI1_IRQn           = 41,
    SPI2_IRQn           = 42,
    SPI3_IRQn           = 43,
    // QSPI
    QSPI0_IRQn          = 44,
    QSPI1_IRQn          = 45,
    // TIMER
    TIMER0_IRQn         = 46,
    TIMER1_IRQn         = 47,
    TIMER2_IRQn         = 48,
    TIMER3_IRQn         = 49,
    TIMER4_IRQn         = 50,
    TIMER5_IRQn         = 51,
    TIMER6_IRQn         = 52,
    TIMER7_IRQn         = 53,
    // USART
    USART0_IRQn         = 54,
    USART1_IRQn         = 55,
    USART2_IRQn         = 56,
    USART3_IRQn         = 57,
    USART4_IRQn         = 58,
    USART5_IRQn         = 59,
    USART6_IRQn         = 60,
    USART7_IRQn         = 61,
    // USB_OTG
    USB_OTG0_IRQn       = 62,
    USB_OTG1_IRQn       = 63,
    // WDT
    WDT0_IRQn           = 64,
    WDT1_IRQn           = 65,
    // SWI (Software Interrupt)
    SWI0_IRQn           = 66,
    SWI1_IRQn           = 67,
    SWI2_IRQn           = 68,
    SWI3_IRQn           = 69,
    SWI4_IRQn           = 70,
    SWI5_IRQn           = 71,
    SWI6_IRQn           = 72,
    SWI7_IRQn           = 73,
    SWI8_IRQn           = 74,
    SWI9_IRQn           = 75,
    SWI10_IRQn          = 76,
    SWI11_IRQn          = 77,
    SWI12_IRQn          = 78,
    SWI13_IRQn          = 79,
    SWI14_IRQn          = 80,
    SWI15_IRQn          = 81,
} IRQn_Type;

#define __CHECK_DEVICE_DEFINES

// CMSIS configuration definitions
#define __CM4_REV                       0x0001U  // Core revision r0p1
#define __MPU_PRESENT                   1U       // MPU present
#define __NVIC_PRIO_BITS                4U       // Number of Bits used for Priority Levels
#define __Vendor_SysTickConfig          0U       // Set to 1 if different SysTick Config is used
#define __FPU_PRESENT                   1U       // FPU present
#define __VTOR_PRESENT                  1U       // VTOR present

#include "utilities/compiler/arm/3rd-party/CMSIS/CMSIS/Core/Include/core_cm4.h"

// Add base register address definitions for all peripherals
// ADC register base addresses
#define ADC0_BASE           0x40000000UL
#define ADC1_BASE           0x40001000UL
#define ADC2_BASE           0x40002000UL
// DAC register base addresses
#define DAC0_BASE           0x40010000UL
#define DAC1_BASE           0x40011000UL

// DMA register base addresses
#define DMA0_BASE           0x40020000UL
#define DMA1_BASE           0x40021000UL
#define DMA2_BASE           0x40022000UL
#define DMA3_BASE           0x40023000UL

// ETH register base addresses
#define ETH0_BASE           0x40030000UL
#define ETH1_BASE           0x40031000UL

// FLASH register base addresses
#define FLASH0_BASE         0x40040000UL
#define FLASH1_BASE         0x40041000UL

// GPIO register base addresses
#define GPIOA_BASE          0x40050000UL
#define GPIOB_BASE          0x40051000UL
#define GPIOC_BASE          0x40052000UL
#define GPIOD_BASE          0x40053000UL

// I2C register base addresses
#define I2C0_BASE           0x40060000UL
#define I2C1_BASE           0x40061000UL
#define I2C2_BASE           0x40062000UL
#define I2C3_BASE           0x40063000UL

// I2S register base addresses
#define I2S0_BASE           0x40070000UL
#define I2S1_BASE           0x40071000UL
#define I2S2_BASE           0x40072000UL

// PWM register base addresses
#define PWM0_BASE           0x40080000UL
#define PWM1_BASE           0x40081000UL
#define PWM2_BASE           0x40082000UL
#define PWM3_BASE           0x40083000UL

// RNG register base addresses
#define RNG0_BASE           0x40090000UL
#define RNG1_BASE           0x40091000UL

// RTC register base addresses
#define RTC0_BASE           0x400A0000UL
#define RTC1_BASE           0x400A1000UL

// SDIO register base addresses
#define SDIO0_BASE          0x400B0000UL
#define SDIO1_BASE          0x400B1000UL

// SPI register base addresses
#define SPI0_BASE           0x400C0000UL
#define SPI1_BASE           0x400C1000UL
#define SPI2_BASE           0x400C2000UL
#define SPI3_BASE           0x400C3000UL

// QSPI register base addresses
#define QSPI0_BASE          0x400D0000UL
#define QSPI1_BASE          0x400D1000UL

// TIMER register base addresses
#define TIMER0_BASE         0x400E0000UL
#define TIMER1_BASE         0x400E1000UL
#define TIMER2_BASE         0x400E2000UL
#define TIMER3_BASE         0x400E3000UL
#define TIMER4_BASE         0x400E4000UL
#define TIMER5_BASE         0x400E5000UL
#define TIMER6_BASE         0x400E6000UL
#define TIMER7_BASE         0x400E7000UL

// USART register base addresses
#define USART0_BASE         0x400F0000UL
#define USART1_BASE         0x400F1000UL
#define USART2_BASE         0x400F2000UL
#define USART3_BASE         0x400F3000UL
#define USART4_BASE         0x400F4000UL
#define USART5_BASE         0x400F5000UL
#define USART6_BASE         0x400F6000UL
#define USART7_BASE         0x400F7000UL

// USB_OTG register base addresses
#define USB_OTG0_BASE       0x50000000UL
#define USB_OTG1_BASE       0x50010000UL

// WDT register base addresses
#define WDT0_BASE           0x40100000UL
#define WDT1_BASE           0x40110000UL

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __FAKE_CHIP_H__
/* EOF */
