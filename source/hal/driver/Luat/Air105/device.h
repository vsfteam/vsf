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
#   define VSF_ARCH_PRI_NUM                 64
#   define VSF_ARCH_PRI_BIT                 6

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM                     9

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#   ifdef FALSE
#       undef FALSE
#   endif
#   ifdef TRUE
#       undef TRUE
#   endif

#   define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#   include "air105.h"

#else

#ifndef __HAL_DEVICE_LUAT_AIR05_H__
#define __HAL_DEVICE_LUAT_AIR05_H__

/*============================ INCLUDES ======================================*/

#include "common.h"

/*============================ MACROS ========================================*/

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST                    28, 30, 31, 34, 41, 43, 44, 45, 48

#define VSF_MUSB_FDRC_PRIV_REG_T
#define VSF_MUSB_FDRC_DYNAMIC_FIFO2

#define USB_OTG_COUNT                       1
#define USB_OTG0_CONFIG                                                         \
            .reg = (void *)0x40000C00,                                          \
            .irqn = 1,

#define VSF_HW_USART_COUNT          4
#define VSF_HW_USART_MASK           ((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_LUAT_AIR05_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
