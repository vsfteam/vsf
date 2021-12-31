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

//! arch info
#   define VSF_ARCH_PRI_NUM         1
#   define VSF_ARCH_PRI_BIT         1

#else

#   ifndef __HAL_DEVICE_HERCULES_CMEM7_H__
#       define __HAL_DEVICE_HERCULES_CMEM7_H__

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

// TODO: fixed high speed bug
// In usbd_msc demo with USB_SPEED_HIGH, device send Bulk IN(512 byte),
// which is 511 bytes in the usb analyzer
#define VSF_CMEM7_USB_CFG_SPEED                USB_SPEED_FULL


#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USB_IRQHandler
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       4
#define USB_OTG0_CONFIG                                                         \
            .dc_ep_num              = 7 << 1,                                   \
            .hc_ep_num              = 8,                                        \
            .reg                    = (void *)USB_BASE,                         \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = (8 * 1024) >> 2,                          \
                .speed              = VSF_CMEM7_USB_CFG_SPEED,                  \
                .dma_en             = true,                                     \
                .ulpi_en            = true,                                     \
                .utmi_en            = false,                                    \
                .vbus_en            = false,

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
#   endif   // __HAL_DEVICE_HERCULES_CMEM7_H__

#endif      //__VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
