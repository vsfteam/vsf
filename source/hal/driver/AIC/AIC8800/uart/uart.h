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

#define VSF_USART_CFG_FIFO_TO_REQUEST            ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_usart_mode_t {
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 0),
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 0),
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 0),
    VSF_USART_5_BIT_LENGTH              = (0x0ul << 0),
    __AIC8800_VSF_USART_BIT_LENGTH_MASK = VSF_USART_8_BIT_LENGTH |
                                          VSF_USART_7_BIT_LENGTH |
                                          VSF_USART_6_BIT_LENGTH |
                                          VSF_USART_5_BIT_LENGTH,

    VSF_USART_1_STOPBIT                 = (0x0ul << 2),
    VSF_USART_2_STOPBIT                 = (0x1ul << 2),
    __AIC8800_VSF_USART_STOPBIT_MASK    = VSF_USART_1_STOPBIT |
                                          VSF_USART_2_STOPBIT,

    VSF_USART_NO_PARITY                 = (0x0ul << 3),
    VSF_USART_ODD_PARITY                = (0x1ul << 3),
    VSF_USART_EVEN_PARITY               = (0x2ul << 3),
    __AIC8800_VSF_USART_PARITY_MASK     = VSF_USART_NO_PARITY   |
                                          VSF_USART_ODD_PARITY  |
                                          VSF_USART_EVEN_PARITY,

    __AIC8800_USART_MASK                = __AIC8800_VSF_USART_BIT_LENGTH_MASK |
                                          __AIC8800_VSF_USART_STOPBIT_MASK |
                                          __AIC8800_VSF_USART_PARITY_MASK,

    // Not hardware, just keep for build
    VSF_USART_FORCE_0_PARITY            = (0x3ul << 20),
    VSF_USART_FORCE_1_PARITY            = (0x4ul << 20),
    VSF_USART_PARITY_MASK               = VSF_USART_NO_PARITY |
                                          VSF_USART_EVEN_PARITY |
                                          VSF_USART_ODD_PARITY |
                                          VSF_USART_FORCE_0_PARITY |
                                          VSF_USART_FORCE_1_PARITY,

    VSF_USART_1_5_STOPBIT               = (0x1ul << 23),
    VSF_USART_STOPBIT_MASK              = VSF_USART_1_STOPBIT |
                                          VSF_USART_1_5_STOPBIT |
                                          VSF_USART_2_STOPBIT,

    VSF_USART_9_BIT_LENGTH              = (0x4ul << 25),
    VSF_USART_BIT_LENGTH_MASK           = VSF_USART_5_BIT_LENGTH |
                                      VSF_USART_6_BIT_LENGTH |
                                      VSF_USART_7_BIT_LENGTH |
                                      VSF_USART_8_BIT_LENGTH |
                                      VSF_USART_9_BIT_LENGTH,

    VSF_USART_NO_HWCONTROL              = (0x0ul << 28),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << 28),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << 28),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << 28),
    VSF_USART_HWCONTROL_MASK            = VSF_USART_NO_HWCONTROL |
                                          VSF_USART_RTS_HWCONTROL |
                                          VSF_USART_CTS_HWCONTROL |
                                          VSF_USART_RTS_CTS_HWCONTROL,

    VSF_USART_TX_ENABLE                 = (0x1ul << 30),
    VSF_USART_RX_ENABLE                 = (0x1ul << 31),
    VSF_USART_ENABLE_MASK               = VSF_USART_TX_ENABLE |
                                          VSF_USART_RX_ENABLE,

    VSF_USART_MODE_ALL_BITS_MASK        = VSF_USART_PARITY_MASK |
                                          VSF_USART_STOPBIT_MASK |
                                          VSF_USART_BIT_LENGTH_MASK |
                                          VSF_USART_HWCONTROL_MASK |
                                          VSF_USART_ENABLE_MASK,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    VSF_USART_IRQ_MASK_RX               =  UART_RXIRQEN_MSK, /* 0 */
    VSF_USART_IRQ_MASK_TX               =  UART_TXIRQEN_MSK, /* 1 */
    USART_IRQ_MASK_LS                   =  UART_LSIRQEN_MSK, /* 2 */
    USART_IRQ_MASK_MS                   =  UART_MSIRQEN_MSK, /* 3 */
    USART_IRQ_MASK_PT                   =  UART_PTIRQEN_MSK, /* 7 */

    USART_IRQ_MASK_FIFO                 =  VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX
                                         | USART_IRQ_MASK_LS | USART_IRQ_MASK_MS
                                         | USART_IRQ_MASK_PT,

    // usart error interrupt
    VSF_USART_IRQ_MASK_RX_ERROR         =  0x1 << 8,
    VSF_USART_IRQ_MASK_TX_ERROR         =  0x1 << 9,
    VSF_USART_IRQ_MASK_ERROR            =  VSF_USART_IRQ_MASK_RX_ERROR | VSF_USART_IRQ_MASK_TX_ERROR,

    // usart request interrupt
    VSF_USART_IRQ_MASK_RX_CPL           = 0x1 << 10,
    VSF_USART_IRQ_MASK_TX_CPL           = 0x1 << 11,
    USART_IRQ_MASK_REQUEST              = VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL,

    VSF_USART_IRQ_MASK_RX_TIMEOUT       =  0x1 << 12,

    VSF_USART_IRQ_ALL_BITS_MASK         =  USART_IRQ_MASK_FIFO
                                         | USART_IRQ_MASK_REQUEST
                                         | VSF_USART_IRQ_MASK_RX_TIMEOUT
                                         | VSF_USART_IRQ_MASK_ERROR,
} vsf_usart_irq_mask_t;

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/usart/fifo2req_usart.h"

#define VSF_USART_CFG_DEC_PREFIX            vsf_hw
#define VSF_USART_CFG_DEC_UPCASE_PREFIX     VSF_HW
#define VSF_USART_CFG_DEC_FIFO_TO_REQUEST
#include "hal/driver/common/usart/usart_template.h"

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */