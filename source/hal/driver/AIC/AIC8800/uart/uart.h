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

#ifndef __HAL_DRIVER_AIC_8800_UART_H__
#define __HAL_DRIVER_AIC_8800_UART_H__
/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal.h"
#include "./i_reg_uart.h"
#include "hal/driver/common/template/vsf_template_usart.h"
/*============================ MACROS ========================================*/

#ifndef USART_MAX_PORT
#   define USART_MAX_PORT                           3
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum em_usart_mode_t {
    USART_8_BIT_LENGTH              = 3ul,
    USART_7_BIT_LENGTH              = 2ul,
    USART_6_BIT_LENGTH              = 1ul,
    USART_5_BIT_LENGTH              = 0ul,
    USART_BIT_LENGTH                =       USART_8_BIT_LENGTH
                                        |   USART_7_BIT_LENGTH
                                        |   USART_6_BIT_LENGTH
                                        |   USART_5_BIT_LENGTH,

    USART_1_STOPBIT                 = 0ul,
    USART_2_STOPBIT                 = 4ul,
    USART_STOPBIT                   =       USART_1_STOPBIT
                                        |   USART_2_STOPBIT,
    USART_NONE_PARITY               = 0x0ul,
    USART_ODD_PARITY                = 0x8ul,
    USART_EVEN_PARITY               = 0x18ul,
    USART_PARITY                    =       USART_NONE_PARITY
                                        |   USART_ODD_PARITY
                                        |   USART_EVEN_PARITY,

    USART_DBUFEN_RST                = 0x40ul,
    USART_RXDRST_RST                = 0x80ul,
    USART_TXDRST_RST                = 0x100ul,
    USART_DBUFCFG                   =       USART_DBUFEN_RST
                                        |   USART_TXDRST_RST
                                        |   USART_RXDRST_RST,
};

typedef enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               = UART_RXIRQEN_MSK, /* 0 */
    USART_IRQ_MASK_TX               = UART_TXIRQEN_MSK, /* 1 */
    USART_IRQ_MSK_LS                = UART_LSIRQEN_MSK, /* 2 */
    USART_IRQ_MSK_MS                = UART_MSIRQEN_MSK, /* 3 */
    USART_IRQ_MSK_PT                = UART_PTIRQEN_MSK, /* 7 */
    USART_IRQ_MSK_TIMEOUT           = BIT(4),

    USART_IRQ_MASK_SOURCE           =       USART_IRQ_MASK_RX
                                        |   USART_IRQ_MASK_TX
                                        |   USART_IRQ_MSK_LS
                                        |   USART_IRQ_MSK_MS
                                        |   USART_IRQ_MSK_PT,

    USART_IRQ_MASK_RX_ERR           = BIT(5),
    USART_IRQ_MASK_TX_ERR           = BIT(6),
    USART_IRQ_MASK_ERR              =       USART_IRQ_MASK_RX_ERR
                                        |   USART_IRQ_MASK_TX_ERR,

    USART_IRQ_MASK                  =       USART_IRQ_MASK_SOURCE
                                        |   USART_IRQ_MASK_ERR,
} em_usart_irq_mask_t;

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t        is_busy : 1;
            uint32_t        is_enabled : 1;
        };
    };
};

struct vsf_usart_t {
    uint8_t                         *tx_buf;
    uint8_t                         *rx_buf;
    usart_cfg_t                     cfg;
    usart_status_t                  status;
    em_usart_irq_mask_t             irq_mask;
    struct {
        uint8_t                     num;
        uint32_t                    hclk;
        uint32_t                    oclk;
        uint32_t                    per_uart;
        IRQn_Type                   irqn;
    };
    uart_reg_t                      *param;
};
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#if (USART_MAX_PORT > 0) && (VSF_HAL_USE_USART0 == ENABLED)
    extern vsf_usart_t vsf_usart0;
#endif

#if (USART_MAX_PORT > 1) && (VSF_HAL_USE_USART1 == ENABLED)
    extern vsf_usart_t vsf_usart1;
#endif

#if (USART_MAX_PORT > 2) && (VSF_HAL_USE_USART2 == ENABLED)
    extern vsf_usart_t vsf_usart2;
#endif

#if (USART_MAX_PORT > 3) && (VSF_HAL_USE_USART3 == ENABLED)
#   error "No instance can be used"
#endif

#if (USART_MAX_PORT > 4) && (VSF_HAL_USE_USART4 == ENABLED)
#   error "No instance can be used"
#endif

#if (USART_MAX_PORT > 5) && (VSF_HAL_USE_USART5 == ENABLED)
#   error "No instance can be used"
#endif

#if (USART_MAX_PORT > 6) && (VSF_HAL_USE_USART6 == ENABLED)
#   error "No instance can be used"
#endif

#if (USART_MAX_PORT > 7) && (VSF_HAL_USE_USART7 == ENABLED)
#   error "No instance can be used"
#endif

#endif
/* EOF */