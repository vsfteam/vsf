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

//#ifdef VSF_CFG_USART_EN
/*============================ INCLUDES ======================================*/

#if VSF_HAL_USE_USART == ENABLED

#ifndef __HAL_DRIVER_NUVOTON_M480_USART_H__
#define __HAL_DRIVER_NUVOTON_M480_USART_H__

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

//! include the infrastructure
#include "../io/io.h"
#include "../pm/pm.h"

/*============================ MACROS ========================================*/

/* select from { HXT, LXT, PLL, HIRC} */
#ifndef VSF_CFG_USART_CLOCK_SOURCE
#   define VSF_CFG_USART_CLOCK_SOURCE                       HXT
#endif

#ifndef USART_MAX_PORT
#   define USART_MAX_PORT                                   5
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __extern_m480_usart(__count, __dont_care)                           \
    extern vsf_usart_t vsf_usart##__count;

#define extern_m480_usart(__count)                                          \
    VSF_MREPEAT(__count, __extern_m480_usart, __count)
/*============================ TYPES =========================================*/

enum  em_usart_mode_t {
    USART_8_BIT_LENGTH              = 3ul,
    USART_7_BIT_LENGTH              = 2ul,
    USART_6_BIT_LENGTH              = 1ul,
    USART_5_BIT_LENGTH              = 0ul,
    USART_BIT_LENGTH                =       USART_8_BIT_LENGTH
                                        |   USART_7_BIT_LENGTH
                                        |   USART_6_BIT_LENGTH
                                        |   USART_5_BIT_LENGTH,

    USART_1_STOPBIT                 = 0x000ul,
    USART_1_5_STOPBIT               = 0x100ul,/*!< UART_LINE setting for 1.5
                            stop bit when 5-bit word length  \hideinitializer   */
    USART_2_STOPBIT                 = 0x100ul,/*!< UART_LINE setting for two
                        stop bit when 6, 7, 8-bit word length \hideinitializer  */
    USART_STOPBIT                   =       USART_1_STOPBIT
                                        |   USART_1_5_STOPBIT
                                        |   USART_2_STOPBIT,
    USART_NONE_PARITY               = 0x00000ul,
    USART_ODD_PARITY                = 0x10000ul,
    USART_EVEN_PARITY               = 0x30000ul,
    USART_MARK_PARITY               = 0x50000ul,
    USART_SPACE_PARITY              = 0x70000ul,
    USART_PARITY                    =       USART_NONE_PARITY
                                        |   USART_ODD_PARITY
                                        |   USART_EVEN_PARITY
                                        |   USART_MARK_PARITY
                                        |   USART_SPACE_PARITY,

    USART_1_BIT_FIFO                = 0x00000000ul,
    USART_4_BIT_FIFO                = 0x01000000ul,
    USART_8_BIT_FIFO                = 0x02000000ul,
    USART_14_BIT_FIFO               = 0x03000000ul,
    USART_FIFO_SIZE                 =       USART_1_BIT_FIFO
                                        |   USART_4_BIT_FIFO
                                        |   USART_8_BIT_FIFO
                                        |   USART_14_BIT_FIFO,

    USART_TX_INVERTED               = 0x04000000ul,
    USART_RX_INVERTED               = 0x08000000ul,
    USART_TX_RX_INVERTED            =       USART_TX_INVERTED
                                        |   USART_RX_INVERTED,
};

enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               = BIT(0),
    USART_IRQ_MASK_TX               = BIT(1),
    USART_IRQ_MASK_RX_CPL           = BIT(2),
    USART_IRQ_MASK_TX_CPL           = BIT(3),

    // optional
    USART_IRQ_MASK_RX_ERR           = BIT(4),
    USART_IRQ_MASK_TX_ERR           = BIT(5),
    USART_IRQ_MASK_ERR              = USART_IRQ_MASK_RX_ERR | USART_IRQ_MASK_TX_ERR,
};

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ TYPES =========================================*/

typedef struct m480_usart_t {
    UART_T                          *usart;
    IRQn_Type                       irq;
    uint32_t                        module;
    uint32_t                        clock_source;
    uint32_t                        uartx_rst;
} m480_usart_t;

struct usart_status_t {
    uint8_t                         is_busy : 1;
    uint8_t                         rx_error_detected : 1;
    uint8_t                         tx_error_detected : 1;
    uint8_t                         rx_cancel : 1;
    uint8_t                         tx_cancel : 1;
    uint32_t                        more_status : 27;
};

typedef struct rx_tx_gpio_t {
    struct {
        vsf_gpio_t                  *reg;
        uint32_t                    pin_mask;
    }rx;
    struct {
        vsf_gpio_t                  *reg;
        uint32_t                    pin_mask;
    }tx;
}rx_tx_gpio_t;

struct vsf_usart_t {
    usart_status_t                  status;
    uint8_t                         usart_port_num;
    uint8_t                         *tx_buf;
    uint8_t                         *rx_buf;
    usart_cfg_t                     cfg;
    rx_tx_gpio_t                    gpio_reg;
    uint8_t                         is_enabled : 1;
    const m480_usart_t              param;
};
/*============================ GLOBAL VARIABLES ==============================*/

extern_m480_usart(USART_MAX_PORT)
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
