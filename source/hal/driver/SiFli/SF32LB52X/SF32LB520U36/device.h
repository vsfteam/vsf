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
#define VSF_ARCH_PRI_NUM            8
#define VSF_ARCH_PRI_BIT            3

// software interrupt provided by a dedicated device
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          8
#endif
#if VSF_DEV_SWI_NUM > VSF_ARCH_PRI_NUM
#   warning too many VSF_DEV_SWI_NUM, max is VSF_ARCH_PRI_NUM
#   undef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM          VSF_ARCH_PRI_NUM
#endif

#elif defined(__VSF_HAL_SHOW_VENDOR_INFO__)

#define __VSF_HEADER_ONLY_SHOW_VENDOR_INFO__
#include "register.h"

#else

#ifndef __HAL_DEVICE_SIFLI_SF32LB520U36_H__
#define __HAL_DEVICE_SIFLI_SF32LB520U36_H__

// software interrupt provided by a dedicated device
#define __VSF_DEF_SWI_LIST_DEF(__N, __PARAM)                                    \
                                    VSF_MCONNECT(SWI, __N, _IRQn),
#define VSF_DEV_SWI_LIST            VSF_MREPEAT(VSF_DEV_SWI_NUM, __VSF_DEF_SWI_LIST_DEF, NULL)

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where common.h is included.*/
#include "../common/common.h"

/*============================ MACROS ========================================*/

#define VSF_HW_USART_COUNT              5
#define VSF_HW_USART_MASK               0x03E
#define VSF_HW_USART1_REG_BASE          USART1_BASE
#define VSF_HW_USART1_EN                VSF_HW_EN_USART1
#define VSF_HW_USART1_RST               VSF_HW_RST_USART1
#define VSF_HW_USART1_SYNC              true
#define VSF_HW_USART1_IRQN              USART1_IRQn
#define VSF_HW_USART1_IRQHandler        USART1_IRQHandler
#define VSF_HW_USART2_REG_BASE          USART2_BASE
#define VSF_HW_USART2_EN                VSF_HW_EN_USART2
#define VSF_HW_USART2_RST               VSF_HW_RST_USART2
#define VSF_HW_USART2_SYNC              true
#define VSF_HW_USART2_IRQN              USART2_IRQn
#define VSF_HW_USART2_IRQHandler        USART2_IRQHandler
#define VSF_HW_USART3_REG_BASE          USART3_BASE
#define VSF_HW_USART3_EN                VSF_HW_EN_USART3
#define VSF_HW_USART3_RST               VSF_HW_RST_USART3
#define VSF_HW_USART3_SYNC              true
#define VSF_HW_USART3_IRQN              USART3_IRQn
#define VSF_HW_USART3_IRQHandler        USART3_IRQHandler
#define VSF_HW_USART4_REG_BASE          USART4_BASE
#define VSF_HW_USART4_EN                VSF_HW_EN_USART4
#define VSF_HW_USART4_RST               VSF_HW_RST_USART4
#define VSF_HW_USART4_SYNC              true
#define VSF_HW_USART4_IRQN              USART4_IRQn
#define VSF_HW_USART4_IRQHandler        USART4_IRQHandler
#define VSF_HW_USART5_REG_BASE          USART5_BASE
#define VSF_HW_USART5_EN                0
#define VSF_HW_USART5_RST               0
#define VSF_HW_USART5_SYNC              true
#define VSF_HW_USART5_IRQN              USART5_IRQn
#define VSF_HW_USART5_IRQHandler        USART5_IRQHandler

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_SIFLI_SF32LB520U36_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
