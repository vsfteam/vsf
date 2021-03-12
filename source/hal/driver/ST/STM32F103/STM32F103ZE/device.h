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
#   define VSF_ARCH_PRI_NUM         16
#   define VSF_ARCH_PRI_BIT         4

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             0

#else

#ifndef __HAL_DEVICE_ST_STM32F103ZE_H__
#define __HAL_DEVICE_ST_STM32F103ZE_H__

#define VSF_DEV_SWI_LIST            0


#define PCLK1                       36000000
#define PCLK2                       72000000

#define USART0                      NULL

#define USART_MAX_PORT              5

//! there is no USART0
#undef VSF_HAL_USE_USART0

//! necessary name alias
#define USART4_IRQn                 UART4_IRQn
#define USART5_IRQn                 UART5_IRQn
#define USART4                      UART4
#define USART5                      UART5

#define SPI_COUNT                   3
#define IIC_COUNT                   3
#define GPIO_COUNT                  4

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_ST_STM32F103ZE_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
