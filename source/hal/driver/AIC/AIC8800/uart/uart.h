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

#define VSF_USART_CFG_FIFO_TO_REQUEST       ENABLED
#define VSF_USART_CFG_REIMPLEMENT_MODE      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_IRQ_MASK  ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum em_usart_mode_t {
    USART_8_BIT_LENGTH              = (0x3ul << 0),
    USART_7_BIT_LENGTH              = (0x2ul << 0),
    USART_6_BIT_LENGTH              = (0x1ul << 0),
    USART_5_BIT_LENGTH              = (0x0ul << 0),
    USART_BIT_LENGTH_MASK           =   USART_8_BIT_LENGTH
                                      | USART_7_BIT_LENGTH
                                      | USART_6_BIT_LENGTH
                                      | USART_5_BIT_LENGTH,

    USART_1_STOPBIT                 = (0x0ul << 2),
    USART_2_STOPBIT                 = (0x1ul << 2),
    USART_STOPBIT_MASK              =   USART_1_STOPBIT
                                      | USART_2_STOPBIT,

    USART_NO_PARITY                 = (0x0ul << 3),
    USART_ODD_PARITY                = (0x1ul << 3),
    USART_EVEN_PARITY               = (0x2ul << 3),
    USART_PARITY_MASK               =       USART_NO_PARITY
                                        |   USART_ODD_PARITY
                                        |   USART_EVEN_PARITY,

    __USART_AIC8800_MASK            =   USART_BIT_LENGTH_MASK
                                      | USART_STOPBIT_MASK
                                      | USART_PARITY_MASK,

    // Not hardware, just keep for build
    USART_TX_ENABLE                     = (0x1ul << 8),
    USART_RX_ENABLE                     = (0x2ul << 8),
    USART_ENABLE_MASK                   = (0x3ul << 8),
} em_usart_mode_t;

typedef enum em_usart_irq_mask_t {
    // usart fifo interrupt
    USART_IRQ_MASK_RX               =  UART_RXIRQEN_MSK, /* 0 */
    USART_IRQ_MASK_TX               =  UART_TXIRQEN_MSK, /* 1 */
    USART_IRQ_MASK_LS               =  UART_LSIRQEN_MSK, /* 2 */
    USART_IRQ_MASK_MS               =  UART_MSIRQEN_MSK, /* 3 */
    USART_IRQ_MASK_PT               =  UART_PTIRQEN_MSK, /* 7 */

    USART_IRQ_MASK_FIFO             =  USART_IRQ_MASK_RX | USART_IRQ_MASK_TX
                                     | USART_IRQ_MASK_LS | USART_IRQ_MASK_MS
                                     | USART_IRQ_MASK_PT,

    // usart error interrupt
    USART_IRQ_MASK_RX_ERROR         =  0x1 << 8,
    USART_IRQ_MASK_TX_ERROR         =  0x1 << 9,
    USART_IRQ_MASK_ERROR            =  USART_IRQ_MASK_RX_ERROR | USART_IRQ_MASK_TX_ERROR,

    // usart request interrupt
    USART_IRQ_MASK_RX_CPL           = 0x1 << 10,
    USART_IRQ_MASK_TX_CPL           = 0x1 << 11,
    USART_IRQ_MASK_REQUEST          = USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL,

    USART_IRQ_MASK_RX_TIMEOUT       =  0x1 << 12,

    USART_IRQ_MASK                  =  USART_IRQ_MASK_FIFO
                                     | USART_IRQ_MASK_REQUEST
                                     | USART_IRQ_MASK_RX_TIMEOUT
                                     | USART_IRQ_MASK_ERROR,
} em_usart_irq_mask_t;

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_API_DECLARATION_PREFIX            vsf_hw
#define VSF_USART_CFG_INSTANCE_DECLARATION_PREFIX       VSF_HW
#include "hal/driver/common/usart/usart_template.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */