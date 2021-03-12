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

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

/*\note first define basic info for arch. */
//! arch info
#define VSF_ARCH_PRI_NUM            4
#define VSF_ARCH_PRI_BIT            2

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_NUM             4

#else       // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#ifndef __HAL_DEVICE_ALLWINNER_F1C100S_H__
#define __HAL_DEVICE_ALLWINNER_F1C100S_H__

// software interrupt provided by a dedicated device
#define VSF_DEV_SWI_LIST            60, 61, 62, 63

/*============================ INCLUDES ======================================*/

/*\note this is should be the only place where __common.h is included.*/
#include "../common/__common.h"
#include "./f1c100s_reg.h"

/*============================ MACROS ========================================*/

#define F1CX00S_DRAM_ADDR           0x80000000

#ifndef F1CX00S_PLL_CPU_CLK_HZ
#   define F1CX00S_PLL_CPU_CLK_HZ   (408UL * 1000 * 1000)
#endif

#ifndef F1CX00S_PLL_DDR_CLK_HZ
#   define F1CX00S_PLL_DDR_CLK_HZ   (156UL * 1000 * 1000)
#endif


//! \brief intc register base address
#define F1CX00S_INTC_BASE_ADDRESS   (0x01C20400ul)

#define USART_MAX_PORT              0x03
#define USART_PORT_MASK             0x07
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __HAL_DEVICE_ALLWINNER_F1C100S_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
