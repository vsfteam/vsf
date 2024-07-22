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
#define VSF_ARCH_PRI_NUM            16
#define VSF_ARCH_PRI_BIT            4

// software interrupt provided by a dedicated device
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          5
#endif
#if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#   warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#   undef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          VSF_ARCH_PRI_NUM
#endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#include "../common/vendor/Include/gd32h7xx.h"

#else

#ifndef __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__
#define __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                    VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST            VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where common.h is included.*/
#include "../common/common.h"

/*============================ MACROS ========================================*/

#define USB_OTG_COUNT               1
#define USB_OTG0_IRQHandler         USBFS_IRQHandler
// required by dwcotg, define the max ep number of dwcotg include ep0
#define USB_DWCOTG_MAX_EP_NUM       4

#define USB_OTG0_CONFIG                                                         \
    .ep_num = 8,                                                                \
    .irq = USBFS_IRQn,                                                          \
    .reg = (void *)USBFS_BASE,                                                  \
    .buffer_word_size = 0x500 >> 2,                                             \
    .speed = USB_SPEED_FULL,                                                    \
    .dma_en = false,                                                            \
    .ulpi_en = false,                                                           \
    .utmi_en = false,                                                           \
    .vbus_en = false,

#define VSF_HW_IO_PORT_COUNT            10
#define VSF_HW_IO_PORT_MASK             0x6FF       // no GPIOI
#define VSF_HW_IO_PIN_COUNT             16
#define VSF_HW_IO_FUNCTION_MAX          16
#define VSF_HW_IO_PORT0_REG_BASE        (0x58020000)
#define VSF_HW_IO_PORT1_REG_BASE        (0x58020400)
#define VSF_HW_IO_PORT2_REG_BASE        (0x58020800)
#define VSF_HW_IO_PORT3_REG_BASE        (0x58020C00)
#define VSF_HW_IO_PORT4_REG_BASE        (0x58021000)
#define VSF_HW_IO_PORT5_REG_BASE        (0x58021400)
#define VSF_HW_IO_PORT6_REG_BASE        (0x58021800)
#define VSF_HW_IO_PORT7_REG_BASE        (0x58021C00)
#define VSF_HW_IO_PORT9_REG_BASE        (0x58022400)
#define VSF_HW_IO_PORT10_REG_BASE       (0x58022800)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_GIGADEVICE_GD32H7XX_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
