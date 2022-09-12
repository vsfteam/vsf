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
#define VSF_DEV_SWI_NUM             1

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned int SystemCoreClock;
#ifdef __cplusplus
}
#endif
#define VSF_ARCH_SYSTIMER_FREQ      SystemCoreClock

#else

#ifndef __HAL_DEVICE_GEEHY_APM32F407_H__
#define __HAL_DEVICE_GEEHY_APM32F407_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            79
#define SWI0_IRQHandler             CRYPT_IRQHandler

#define RNG_IRQHandler              HASH_RNG_IRQHandler

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

// user configurations with default value

#ifndef VSF_HW_USB_OTG1_USB_CFG_SPEED
// TODO: USB_SPEED_HIGH is not tested OK
#   define VSF_HW_USB_OTG1_USB_CFG_SPEED        USB_SPEED_FULL
#endif

// HW definition

#define VSF_HW_USB_OTG_COUNT        2
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       6

#define USB_OTG_FS_BASE             ((uint32_t)0x50000000)
#define USB_OTG_HS_BASE             ((uint32_t)0x40040000)

#define VSF_HW_USB_OTG0_IRQHandler  OTG_FS_IRQHandler
#define VSF_HW_USB_OTG0_CONFIG                                                  \
            .dc_ep_num              = 4 << 1,                                   \
            .hc_ep_num              = USB_DWCOTG_MAX_EP_NUM,                    \
            .reg                    = (void *)USB_OTG_FS_BASE,                  \
            .irq                    = OTG_FS_IRQn,                              \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 320,                                      \
                .speed              = USB_SPEED_FULL,                           \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

#define VSF_HW_USB_OTG1_IRQHandler  OTG_HS1_IRQHandler
#define VSF_HW_USB_OTG1_CONFIG                                                  \
            .dc_ep_num              = (USB_DWCOTG_MAX_EP_NUM - 1) << 1,         \
            .hc_ep_num              = USB_DWCOTG_MAX_EP_NUM,                    \
            .reg                    = (void *)USB_OTG_HS_BASE,                  \
            .phy_reg                = (void *)(USB_OTG_HS_BASE + 0x200),        \
            .irq                    = OTG_HS1_IRQn,                             \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 1024,                                     \
                .speed              = VSF_HW_USB_OTG1_USB_CFG_SPEED,            \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = true,                                     \
                .vbus_en            = false,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_GEEHY_APM32F407_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
