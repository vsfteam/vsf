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
#define VSF_DEV_SWI_NUM             2

#else

#ifndef __HAL_DEVICE_GEEHY_APM32F407_H__
#define __HAL_DEVICE_GEEHY_APM32F407_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            79, 80
#define SWI0_IRQHandler             CRYPT_IRQHandler
#define SWI1_IRQHandler             HASH_RNG_IRQHandler

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ MACROS ========================================*/

// user configurations with default value

#define VSF_HW_USB_HC_COUNT         2

#define VSF_HW_USB_HC0_TYPE         usbhc
//#define VSF_HW_USB_HC0_IRQHandler   OHCI_IRQHandler
//#define VSF_HW_USB_HC0_CONFIG

#define VSF_HW_USB_DC_COUNT         2
#define VSF_HW_USB_DC_HS_COUNT      1
#define VSF_HW_USB_DC_FS_COUNT      1

#define VSF_HW_USB_DC0_TYPE         usbdc
//#define VSF_HW_USB_DC0_IRQHandler   USBD20_IRQHandler
#define VSF_HW_USB_DC0_EP_NUM       8
//#define VSF_HW_USB_DC0_CONFIG

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __HAL_DEVICE_GEEHY_APM32F407_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
