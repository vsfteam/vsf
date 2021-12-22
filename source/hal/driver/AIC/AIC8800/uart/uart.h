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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED
#include "../__device.h"
#include "./i_reg_uart.h"

#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_USART_DEC_LV0(__COUNT, __dont_care)                             \
    extern vsf_hw_usart_t vsf_usart##__COUNT;

/*============================ TYPES =========================================*/

enum em_usart_mode_t {
    USART_8_BIT_LENGTH              = 3ul,
    USART_7_BIT_LENGTH              = 2ul,
    USART_6_BIT_LENGTH              = 1ul,
    USART_5_BIT_LENGTH              = 0ul,
    USART_BIT_LENGTH_MASK           =   USART_8_BIT_LENGTH
                                      | USART_7_BIT_LENGTH
                                      | USART_6_BIT_LENGTH
                                      | USART_5_BIT_LENGTH,

    USART_1_STOPBIT                 = 0ul,
    USART_2_STOPBIT                 = 4ul,
    USART_STOPBIT_MASK              =   USART_1_STOPBIT
                                      | USART_2_STOPBIT,
                                        
    USART_NO_PARITY                 = 0x0ul,
    USART_ODD_PARITY                = 0x8ul,
    USART_EVEN_PARITY               = 0x18ul,
    USART_PARITY_MASK               =       USART_NO_PARITY
                                        |   USART_ODD_PARITY
                                        |   USART_EVEN_PARITY,

    USART_DBUFEN_RST                = 0x40ul,
    USART_RXDRST_RST                = 0x80ul,
    USART_TXDRST_RST                = 0x100ul,
    USART_DBUFCFG_MASK              =       USART_DBUFEN_RST
                                        |   USART_TXDRST_RST
                                        |   USART_RXDRST_RST,
                                        
    USART_TX_EN                     = 0x00,
    USART_RX_EN                     = 0x00,
};

typedef enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               =  UART_RXIRQEN_MSK, /* 0 */
    USART_IRQ_MASK_TX               =  UART_TXIRQEN_MSK, /* 1 */
    USART_IRQ_MASK_LS               =  UART_LSIRQEN_MSK, /* 2 */
    USART_IRQ_MASK_MS               =  UART_MSIRQEN_MSK, /* 3 */
    USART_IRQ_MASK_PT               =  UART_PTIRQEN_MSK, /* 7 */
    USART_IRQ_MASK_TIMEOUT          =  BIT(4),

    USART_IRQ_MASK_SOURCE           =  USART_IRQ_MASK_RX
                                     | USART_IRQ_MASK_TX
                                     | USART_IRQ_MASK_LS
                                     | USART_IRQ_MASK_MS
                                     | USART_IRQ_MASK_PT,

    USART_IRQ_MASK_RX_ERR           =  BIT(5),
    USART_IRQ_MASK_TX_ERR           =  BIT(6),
    USART_IRQ_MASK_ERR              =  USART_IRQ_MASK_RX_ERR
                                     | USART_IRQ_MASK_TX_ERR,

    USART_IRQ_MASK                  =  USART_IRQ_MASK_SOURCE
                                     | USART_IRQ_MASK_ERR,
} em_usart_irq_mask_t;

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
            uint32_t is_enabled : 1;
        };
    };
};

typedef struct vsf_hw_usart_t vsf_hw_usart_t;

/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(USART_COUNT, __VSF_HW_USART_DEC_LV0, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif
#endif
/* EOF */