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
#define VSF_DEV_SWI_NUM             19

#ifdef __cplusplus
extern "C" {
#endif
extern unsigned int system_core_clock;
#ifdef __cplusplus
}
#endif
#define VSF_ARCH_SYSTIMER_FREQ      system_core_clock

#else

#ifndef __HAL_DEVICE_GEEHY_APM32F407_H__
#define __HAL_DEVICE_GEEHY_APM32F407_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            70, 74, 75, 79, 80, 85, 86, 87, 88, 89, 90, 93, 95, 96, 97, 98, 99, 100, 101

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

// user configurations with default value

// HW definition

#define VSF_HW_USB_OTG_COUNT        2
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       16

#define VSF_HW_USB_OTG0_IRQHandler  OTGFS1_IRQHandler
#define VSF_HW_USB_OTG0_CONFIG                                                  \
            .dc_ep_num              = 8 << 1,                                   \
            .hc_ep_num              = 16,                                       \
            .reg                    = (void *)OTGFS1_BASE,                      \
            .irq                    = OTGFS1_IRQn,                              \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 320,                                      \
                .speed              = USB_SPEED_FULL,                           \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

#define VSF_HW_USB_OTG1_IRQHandler  OTGFS2_IRQHandler
#define VSF_HW_USB_OTG1_CONFIG                                                  \
            .dc_ep_num              = 8 << 1,                                   \
            .hc_ep_num              = 16,                                       \
            .reg                    = (void *)OTGFS2_BASE,                      \
            .irq                    = OTGFS2_IRQn,                              \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 320,                                      \
                .speed              = USB_SPEED_FULL,                           \
                .dma_en             = false,                                    \
                .ulpi_en            = false,                                    \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_GEEHY_APM32F407_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
