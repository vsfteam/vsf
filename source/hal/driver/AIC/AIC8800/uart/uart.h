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

#ifndef __HAL_DRIVER_AIC_8800_UART_H__
#define __HAL_DRIVER_AIC_8800_UART_H__
/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED
#include "../__device.h"
#include "./i_reg_uart.h"

/*============================ MACROS ========================================*/
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


    VSF_USART_NO_PARITY                 = (0x0ul << 3),
    VSF_USART_ODD_PARITY                = (0x1ul << 3),
    VSF_USART_EVEN_PARITY               = (0x2ul << 3),

    // Not hardware, just keep for build
    VSF_USART_9_BIT_LENGTH              = (0x1ul << 8),
    VSF_USART_10_BIT_LENGTH             = (0x2ul << 8),

    VSF_USART_FORCE_0_PARITY            = (0x1ul << 10),
    VSF_USART_FORCE_1_PARITY            = (0x2ul << 10),

    VSF_USART_1_5_STOPBIT               = (0x1ul << 12),
    VSF_USART_0_5_STOPBIT               = (0x2ul << 12),

    VSF_USART_NO_HWCONTROL              = (0x0ul << 14),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 14),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 14),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 14),

    VSF_USART_TX_ENABLE                 = (0x0ul << 15),
    VSF_USART_TX_DISABLE                = (0x1ul << 15),

    VSF_USART_RX_ENABLE                 = (0x0ul << 16),
    VSF_USART_RX_DISABLE                = (0x1ul << 16),

    VSF_USART_SYNC_CLOCK_ENABLE         = (0x0ul << 17),
    VSF_USART_SYNC_CLOCK_DISABLE        = (0x1ul << 17),

    VSF_USART_HALF_DUPLEX_DISABLE       = (0x0ul << 18),
    VSF_USART_HALF_DUPLEX_ENABLE        = (0x1ul << 18),

    VSF_USART_TX_FIFO_THRESH_ONE        = (0x0ul << 19),
    VSF_USART_TX_FIFO_THRESH_HALF_FULL  = (0x1ul << 19),
    VSF_USART_TX_FIFO_THRESH_FULL       = (0x2ul << 19),

    VSF_USART_RX_FIFO_THRESH_ONE        = (0x0ul << 21),
    VSF_USART_RX_FIFO_THRESH_HALF_FULL  = (0x1ul << 21),
    VSF_USART_RX_FIFO_THRESH_FULL       = (0x2ul << 21),

    VSF_USART_SYNC_CLOCK_POLARITY_LOW   = (0x0ul << 23),
    VSF_USART_SYNC_CLOCK_POLARITY_HIGH  = (0x1ul << 23),
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE   = (0x0ul << 24),
    VSF_USART_SYNC_CLOCK_PHASE_2_EDGE   = (0x1ul << 24),

    VSF_USART_IRDA_ENABLE               = (0x1ul << 25),
    VSF_USART_IRDA_DISABLE              = (0x0ul << 25),

    AIC8800_VSF_USART_BIT_LENGTH_MASK   = VSF_USART_8_BIT_LENGTH |
                                          VSF_USART_7_BIT_LENGTH |
                                          VSF_USART_6_BIT_LENGTH |
                                          VSF_USART_5_BIT_LENGTH,
    AIC8800_VSF_USART_STOPBIT_MASK      = VSF_USART_1_STOPBIT |
                                          VSF_USART_2_STOPBIT,
    AIC8800_VSF_USART_PARITY_MASK       = VSF_USART_NO_PARITY |
                                          VSF_USART_ODD_PARITY  |
                                          VSF_USART_EVEN_PARITY,
    AIC8800_USART_MODE_ALL_BITS_MASK    = AIC8800_VSF_USART_BIT_LENGTH_MASK |
                                          AIC8800_VSF_USART_STOPBIT_MASK |
                                          AIC8800_VSF_USART_PARITY_MASK,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    VSF_USART_IRQ_MASK_RX               =  UART_RXIRQEN_MSK, /* 0 */
    VSF_USART_IRQ_MASK_TX               =  UART_TXIRQEN_MSK, /* 1 */

    AIC8800_USART_IRQ_MASK_LS           =  UART_LSIRQEN_MSK, /* 2 */
    AIC8800_USART_IRQ_MASK_MS           =  UART_MSIRQEN_MSK, /* 3 */
    AIC8800_USART_IRQ_MASK_PT           =  UART_PTIRQEN_MSK, /* 7 */

    AIC8800_USART_IRQ_MASK_FIFO         =  VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX
                                         | AIC8800_USART_IRQ_MASK_LS | AIC8800_USART_IRQ_MASK_MS
                                         | AIC8800_USART_IRQ_MASK_PT,

    VSF_USART_IRQ_MASK_CTS              = (0x1ul << 24),

    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 25),

    // usart request interrupt
    VSF_USART_IRQ_MASK_RX_CPL           = (0x1ul << 26),
    VSF_USART_IRQ_MASK_TX_CPL           = (0x1ul << 27),

    // usart error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 28),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 29),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 30),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 31),

    AIC8800_USART_IRQ_MASK              = VSF_USART_IRQ_MASK_RX |
                                          VSF_USART_IRQ_MASK_TX |
                                          AIC8800_USART_IRQ_MASK_LS |
                                          AIC8800_USART_IRQ_MASK_MS |
                                          AIC8800_USART_IRQ_MASK_PT,
} vsf_usart_irq_mask_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */
