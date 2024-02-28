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

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "RP2040.h"

#else

#ifndef __HAL_DEVICE_RASPBERRYPI_RP2040_H__
#define __HAL_DEVICE_RASPBERRYPI_RP2040_H__

/*============================ INCLUDES ======================================*/

// for XXXX_BASE
#include "hardware/regs/addressmap.h"

//#include "common.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

#define USB_OTG0_IRQHandler         USBDMA_IRQHandler
#define USB_OTG0_CONFIG                                                         \
            .dc_ep_num              = 4 << 1,                                   \
            .hc_ep_num              = 5,                                        \
            .reg                    = (void *)0,                                \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 948,                                      \
                .speed              = USB_SPEED_FULL,                           \
                .dma_en             = true,                                     \
                .ulpi_en            = true,                                     \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

#define VSF_HW_I2C_COUNT            2
#define VSF_HW_I2C0_IRQN            I2C0_IRQ_IRQn
#define VSF_HW_I2C0_IRQHandler      I2C0_IRQHandler
#define VSF_HW_I2C0_REG             I2C0_BASE
#define VSF_HW_I2C1_IRQN            I2C1_IRQ_IRQn
#define VSF_HW_I2C1_IRQHandler      I2C1_IRQHandler
#define VSF_HW_I2C1_REG             I2C1_BASE

#define VSF_HW_USART_COUNT          2
#define VSF_HW_USART0_IRQN          UART0_IRQ_IRQn
#define VSF_HW_USART0_IRQHandler    UART0_IRQHandler
#define VSF_HW_USART0_REG           UART0_BASE
#define VSF_HW_USART1_IRQN          UART1_IRQ_IRQn
#define VSF_HW_USART1_IRQHandler    UART1_IRQHandler
#define VSF_HW_USART1_REG           UART1_BASE

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_RASPBERRYPI_RP2040_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
