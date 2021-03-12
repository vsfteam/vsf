/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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
//! arch info
#define VSF_ARCH_PRI_NUM            16
#define VSF_ARCH_PRI_BIT            4


// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             0

#else

#ifndef __HAL_DEVICE_WCH_CH32F103C8_H__
#define __HAL_DEVICE_WCH_CH32F103C8_H__

#define VSF_DEV_SWI_LIST            

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/vendor/inc/ch32f10x.h"
#include "../common/__common.h"

/*============================ MACROS ========================================*/

#ifndef CH32F10X_HSE_FREQ_HZ
#   define CH32F10X_HSE_FREQ_HZ     (12 * 1000 * 1000)
#endif

#ifndef CH32F10X_LSE_FREQ_HZ
#   define CH32F10X_LSE_FREQ_HZ     (32768)
#endif

#ifndef CH32F10X_PLL_FREQ_HZ
#   define CH32F10X_PLL_FREQ_HZ     (72 * 1000 * 1000)
#endif

#ifndef CH32F10X_SYS_FREQ_HZ
#   define CH32F10X_SYS_FREQ_HZ     (72 * 1000 * 1000)
#endif

#ifndef CH32F10X_AHB_FREQ_HZ
#   define CH32F10X_AHB_FREQ_HZ     (72 * 1000 * 1000)
#endif

#ifndef CH32F10X_APB1_FREQ_HZ
#   define CH32F10X_APB1_FREQ_HZ    (72 * 1000 * 1000)
#endif

#ifndef CH32F10X_APB2_FREQ_HZ
#   define CH32F10X_APB2_FREQ_HZ    (72 * 1000 * 1000)
#endif

#ifndef CH32F10X_VECTOR_TABLE
// use default
#endif

#define USB_HDC_COUNT               1
#define USB_HDC_REG_BASE            0x40023400
#define USB_HDC_IRQN                USBHD_IRQn
#define USB_HDC_IRQHandler          USBHD_IRQHandler

#define USB_DC_COUNT                1
#define USB_DC_EP_NUM               8
#define USB_DC_LP_IRQN              USB_LP_CAN1_RX0_IRQn
#define USB_DC_HP_IRQN              USB_HP_CAN1_TX_IRQn
#define USB_DC_REG_BASE             0x40005C00L
#define USB_DC_PMA_BASE             0x40006000L

#define USB_DC0_TYPE                usbd
#define USB_DC0_IRQHandler          USB_LP_CAN1_RX0_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_WCH_CH32F103C8_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
