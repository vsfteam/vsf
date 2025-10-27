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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/*\note first define basic info for arch. */
//! arch info
#   define VSF_ARCH_PRI_NUM         8
#   define VSF_ARCH_PRI_BIT         3

// software interrupt provided by a dedicated device
#   define VSF_DEV_SWI_NUM          32

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

/*\note __VSF_HAL_SHOW_VENDOR_INFO__ is defined to include vendor information only.
 *      Vendor information means the registers/structures/macros from vendor SDK.
 *      Usually these information are not visible from user side to avoid name-space pollution.
 */

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "../common/vendor/libraries/cmsis/cm4/device_support/at32f402_405.h"

#else

#ifndef __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#define __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST                                                        \
    43, 46, 47, 48, 49,  50,  61,  62,                                          \
    63, 64, 65, 66, 70,  78,  79,  80,                                          \
    84, 86, 87, 88, 89,  90,  91,  93,                                          \
    95, 96, 97, 98, 99, 100, 101, 102


/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

// RAM

#define VSF_HW_RAM_COUNT                            1
#define VSF_HW_RAM0_ADDR                            0x20000000
// user can define VSF_HW_RAM0_SIZE to 96KB if parity check is enabled
#ifndef VSF_HW_RAM0_SIZE
#   define VSF_HW_RAM0_SIZE                         ((96 + 6) * 1024)
#endif

// FLASH

#define VSF_HW_FLASH_COUNT                          1
#define VSF_HW_FLASH0_ADDR                          0x08000000
#define VSF_HW_FLASH0_SIZE                          (256 * 1024)

#define VSF_HW_USB_OTG_COUNT        2
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

#define VSF_HW_USB_OTG0_IRQHandler  OTGFS1_IRQHandler
#define VSF_HW_USB_OTG0_CONFIG                                                  \
            .dc_ep_num              = 8 << 1,                                   \
            .hc_ep_num              = 16,                                       \
            .reg                    = (void *)OTGFS1_BASE,                      \
            .irq                    = OTGFS1_IRQn,                              \
            .en                     = VSF_HW_EN_OTGFS1,                         \
            .phyclk                 = &VSF_HW_CLK_HICK,                         \
            .phyclk_freq_required   = 48 * 1000 * 1000,                         \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 320,                                      \
                .speed              = USB_SPEED_FULL,                           \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

// TODO: 修复DMA模式BUG， 修复高速模式的BUG
#define VSF_HW_USB_OTG1_IRQHandler  OTGHS_IRQHandler
#define VSF_HW_USB_OTG1_CONFIG                                                  \
            .dc_ep_num              = 8 << 1,                                   \
            .hc_ep_num              = 16,                                       \
            .reg                    = (void *)OTGHS_BASE,                       \
            .irq                    = OTGHS_IRQn,                               \
            .en                     = VSF_HW_EN_OTGHS,                          \
            .phyclk                 = &VSF_HW_CLK_HEXT,                         \
            .phyclk_freq_required   = 12 * 1000 * 1000,                         \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 1024,                                     \
                .speed              = USB_SPEED_HIGH,                           \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

/*============================ INCLUDES ======================================*/

// Include common irq and af headers after peripherals are defined, so that
//  irq and af can be adjusted according to the dedicated device configuration.

#include "../common/device_irq.h"
#include "../common/device_af.h"

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_ATTERY_AT32F405KCU7_4_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
