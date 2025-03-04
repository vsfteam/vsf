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

/*\note first define basic info for arch. */
#if defined(__VSF_HEADER_ONLY_SHOW_ARCH_INFO__)

#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM                     4
#endif

#ifndef VSF_ARCH_RTOS_CFG_STACK_DEPTH
#   define VSF_ARCH_RTOS_CFG_STACK_DEPTH        4096
#endif

#define VSF_ARCH_FREERTOS_CFG_IS_IN_ISR         xPortInIsrContext

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM                         0

#ifndef __RTOS__
#   define __RTOS__
#endif
#ifndef __FREERTOS__
#   define __FREERTOS__
#endif

// alu types, should be defined in target-specified compiler header
// but xtensa uses generic compiler header, so define alu types here
typedef unsigned int                            uintalu_t;
typedef int                                     intalu_t;

// SET_STACK will fail to pass stack check
//#include "xt_instr_macros.h"
//#define VSF_ARCH_RTOS_CFG_SET_STACK(__STACK, __SIZE)      SET_STACK(__STACK)

// 16-byte align, necessary if > 8
#define VSF_ARCH_STACK_ALIGN_BIT                4

#else

#ifndef __HAL_DEVICE_ESPRESSIF_ESP32S2_H__
#define __HAL_DEVICE_ESPRESSIF_ESP32S2_H__

/*============================ INCLUDES ======================================*/

#include "soc/periph_defs.h"

// to avoid namespace pollution in original headers, copy constants need here
// for USB consts
//#include "soc/usb_periph.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       8

#define USB_BASE_REG                0x60080000

#define USB_OTG0_CONFIG                                                         \
            .dc_ep_num              = 7 << 1,                                   \
            .hc_ep_num              = 8,                                        \
            .periph_module          = PERIPH_USB_MODULE,                        \
            .intr_source            = ETS_USB_INTR_SOURCE,                      \
            .reg                    = (void *)USB_BASE_REG,                     \
            /* vk_dwcotg_hw_info_t */                                           \
                .buffer_word_size   = 1024 >> 2,                                \
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

#endif      // __HAL_DEVICE_ESPRESSIF_ESP32S2_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
