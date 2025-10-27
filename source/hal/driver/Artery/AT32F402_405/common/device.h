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
/*============================ MACROS ========================================*/

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM         8
#   define VSF_ARCH_PRI_BIT         3

#   define VSF_DEV_COMMON_SWI_NUM   32

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "./vendor/libraries/cmsis/cm4/device_support/at32f402_405.h"

#else

#ifndef __HAL_DEVICE_COMMON_ATTERY_AT32F402_405_H__
#define __HAL_DEVICE_COMMON_ATTERY_AT32F402_405_H__

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

// SWI

#define VSF_DEV_COMMON_SWI_LIST                                                 \
    43, 46, 47, 48, 49,  50,  61,  62,                                          \
    63, 64, 65, 66, 70,  78,  79,  80,                                          \
    84, 86, 87, 88, 89,  90,  91,  93,                                          \
    95, 96, 97, 98, 99, 100, 101, 102

// RAM

#if VSF_HW_RAM_COUNT >= 1 && !defined(VSF_HW_RAM0_ADDR)
#   define VSF_HW_RAM0_ADDR             0x20000000
#endif

// FLASH

#if VSF_HW_FLASH_COUNT >= 1 && !defined(VSF_HW_FLASH0_ADDR)
#   define VSF_HW_FLASH0_ADDR           0x08000000
#endif

// GPIO, PORT0, PORT1, PORT2, PORT3, PORT5

#if !defined(VSF_HW_GPIO_PORT_MASK) && defined(VSF_HW_GPIO_PORT_COUNT)
#   define VSF_HW_GPIO_PORT_MASK        ((1 << VSF_HW_GPIO_PORT_COUNT) - 1)
#endif
#if VSF_HW_GPIO_PORT_MASK & ~0x2F
#   error invalid VSF_HW_GPIO_PORT_MASK
#endif

#ifndef VSF_HW_GPIO_PIN_COUNT
#   define VSF_HW_GPIO_PIN_COUNT        16
#endif
#ifndef VSF_HW_GPIO_FUNCTION_MAX
#   define VSF_HW_GPIO_FUNCTION_MAX     16
#endif

#if VSF_HW_GPIO_PORT_MASK & 0x01
#   define VSF_HW_GPIO_PORT0_REG        GPIOA
#   ifndef VSF_HW_GPIO_PORT0_MASK
#       define VSF_HW_GPIO_PORT0_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT0_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT0_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & 0x02
#   define VSF_HW_GPIO_PORT1_REG        GPIOB
#   ifndef VSF_HW_GPIO_PORT1_MASK
#       define VSF_HW_GPIO_PORT1_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT1_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT1_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & 0x04
#   define VSF_HW_GPIO_PORT2_REG        GPIOC
#   ifndef VSF_HW_GPIO_PORT2_MASK
#       define VSF_HW_GPIO_PORT2_MASK   0xFFFF
#   elif VSF_HW_GPIO_PORT2_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT2_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & 0x08
#   define VSF_HW_GPIO_PORT3_REG        GPIOD
#   ifndef VSF_HW_GPIO_PORT3_MASK
#       define VSF_HW_GPIO_PORT3_MASK   0x0004
#   elif VSF_HW_GPIO_PORT3_MASK & ~0xFFFF
#       error invalid VSF_HW_GPIO_PORT3_MASK
#   endif
#endif
#if VSF_HW_GPIO_PORT_MASK & 0x20
#   define VSF_HW_GPIO_PORT5_REG        GPIOF
#   ifndef VSF_HW_GPIO_PORT5_MASK
#       define VSF_HW_GPIO_PORT5_MASK   0x08F3
#   elif VSF_HW_GPIO_PORT5_MASK & ~0x08F3
#       error invalid VSF_HW_GPIO_PORT5_MASK
#   endif
#endif

// USB OTG

// required by dwcotg, define the max ep number of dwcotg include ep0
#ifndef USB_DWCOTG_MAX_EP_NUM
#   define USB_DWCOTG_MAX_EP_NUM        16
#endif

#if VSF_HW_USB_OTG_COUNT >= 1
#   define VSF_HW_USB_OTG0_IRQHandler   OTGFS1_IRQHandler
#   define VSF_HW_USB_OTG0_CONFIG                                               \
            .dc_ep_num                  = 8 << 1,                               \
            .hc_ep_num                  = 16,                                   \
            .reg                        = (void *)OTGFS1_BASE,                  \
            .irq                        = OTGFS1_IRQn,                          \
            .en                         = VSF_HW_EN_OTGFS1,                     \
            .phyclk                     = &VSF_HW_CLK_HICK,                     \
            .phyclk_freq_required       = 48 * 1000 * 1000,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size       = 320,                                  \
                .speed                  = USB_SPEED_FULL,                       \
                .dma_en                 = false,                                \
                .ulpi_en                = false,                                \
                .utmi_en                = false,                                \
                .vbus_en                = false,
#endif

#if VSF_HW_USB_OTG_COUNT >= 2
// TODO: 修复DMA模式BUG， 修复高速模式的BUG
#   define VSF_HW_USB_OTG1_IRQHandler   OTGHS_IRQHandler
#   define VSF_HW_USB_OTG1_CONFIG                                               \
            .dc_ep_num                  = 8 << 1,                               \
            .hc_ep_num                  = 16,                                   \
            .reg                        = (void *)OTGHS_BASE,                   \
            .irq                        = OTGHS_IRQn,                           \
            .en                         = VSF_HW_EN_OTGHS,                      \
            .phyclk                     = &VSF_HW_CLK_HEXT,                     \
            .phyclk_freq_required       = 12 * 1000 * 1000,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size       = 1024,                                 \
                .speed                  = USB_SPEED_HIGH,                       \
                .dma_en                 = false,                                \
                .ulpi_en                = false,                                \
                .utmi_en                = false,                                \
                .vbus_en                = false,
#endif

/*============================ INCLUDES ======================================*/

// Include common irq and af headers after peripherals are defined, so that
//  irq and af can be adjusted according to the dedicated device configuration.

#include "./common.h"
#include "./device_irq.h"
#include "./device_af.h"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_COMMON_ATTERY_AT32F402_405_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/* EOF */
