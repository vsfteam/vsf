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

#ifndef __HAL_DRIVER_LUAT_AIR105_UART_H__
#define __HAL_DRIVER_LUAT_AIR105_UART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED
#include "../__device.h"

/*============================ MACROS ========================================*/

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_usart_mode_t {
    // hardware
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 0),
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 0),
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 0),
    VSF_USART_5_BIT_LENGTH              = (0x0ul << 0),


    VSF_USART_1_STOPBIT                 = (0x0ul << 2),
    VSF_USART_2_STOPBIT                 = (0x1ul << 2),
    VSF_USART_1_5_STOPBIT               = (0x1ul << 2), // for VSF_USART_5_BIT_LENGTH only


    VSF_USART_NO_PARITY                 = (0x0ul << 3),
    VSF_USART_ODD_PARITY                = (0x1ul << 3),
    VSF_USART_EVEN_PARITY               = (0x3ul << 3),

    // Not hardware, just keep for build
    VSF_USART_9_BIT_LENGTH              = (0x1ul << 24),

    VSF_USART_FORCE_0_PARITY            = (0x1ul << 25),
    VSF_USART_FORCE_1_PARITY            = (0x2ul << 25),

    VSF_USART_NO_HWCONTROL              = (0x0ul << 28),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 28),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 28),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 28),

    VSF_USART_TX_ENABLE                 = (0x1ul << 30),
    VSF_USART_TX_DISABLE                = (0x0ul << 30),

    VSF_USART_RX_ENABLE                 = (0x1ul << 31),
    VSF_USART_RX_DISABLE                = (0x0ul << 31),

    AIR105_VSF_USART_BIT_LENGTH_MASK    = VSF_USART_8_BIT_LENGTH |
                                          VSF_USART_7_BIT_LENGTH |
                                          VSF_USART_6_BIT_LENGTH |
                                          VSF_USART_5_BIT_LENGTH,
    AIR105_VSF_USART_STOPBIT_MASK       = VSF_USART_1_STOPBIT |
                                          VSF_USART_2_STOPBIT,
    AIR105_VSF_USART_PARITY_MASK        = VSF_USART_NO_PARITY |
                                          VSF_USART_ODD_PARITY  |
                                          VSF_USART_EVEN_PARITY,
    AIR105_USART_MODE_ALL_BITS_MASK     = AIR105_VSF_USART_BIT_LENGTH_MASK |
                                          AIR105_VSF_USART_STOPBIT_MASK |
                                          AIR105_VSF_USART_PARITY_MASK,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    VSF_USART_IRQ_MASK_RX               =  (1 << 0),
    VSF_USART_IRQ_MASK_TX               =  (1 << 1),

    AIR105_USART_IRQ_MASK_FIFO          =  VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX,

    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (1 << 8),

    // usart request interrupt
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 26),
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 27),

    // usart error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 28),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 29),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 30),
    VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR  = (0x1ul << 31),

    AIR105_USART_IRQ_MASK               = VSF_USART_IRQ_MASK_RX |
                                          VSF_USART_IRQ_MASK_TX,
} vsf_usart_irq_mask_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */
