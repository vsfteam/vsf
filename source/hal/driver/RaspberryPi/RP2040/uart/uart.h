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

#ifndef __HAL_DRIVER_RP2040_UART_H__
#define __HAL_DRIVER_RP2040_UART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "../__device.h"

/*============================ TYPES =========================================*/

// PL011 does not embed a dma controller, so dma irq should be defined here,
//  before including vsf_pl011_uart.h
enum {
    // irq of pl011_usart is 11-bit in size, so implement dma related request irq from bit16
    VSF_USART_IRQ_MASK_TX_CPL       = (0x1ul << 16),
    VSF_USART_IRQ_MASK_RX_CPL       = (0x1ul << 17),
};

/*============================ INCLUDES ======================================*/

#include "hal/driver/IPCore/ARM/PL011/vsf_pl011_uart.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */
