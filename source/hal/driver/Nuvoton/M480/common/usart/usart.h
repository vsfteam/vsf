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

/*============================ INCLUDES ======================================*/

#ifndef __HAL_DRIVER_NUVOTON_M480_USART_H__
#define __HAL_DRIVER_NUVOTON_M480_USART_H__

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

/* select from { HXT, LXT, PLL, HIRC} */
#ifndef VSF_CFG_USART_CLOCK_SOURCE
#   define VSF_CFG_USART_CLOCK_SOURCE                           HXT
#endif

#define VSF_USART_CFG_REIMPLEMENT_MODE                          ENABLED
#define VSF_USART_CFG_REIMPLEMENT_IRQ_MASK                      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_STATUS                        ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum  em_usart_mode_t {
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
} em_usart_mode_t;

typedef enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               = VSF_BIT(0),
    USART_IRQ_MASK_TX               = VSF_BIT(1),
    USART_IRQ_MASK_RX_CPL           = VSF_BIT(2),
    USART_IRQ_MASK_TX_CPL           = VSF_BIT(3),

    // optional
    USART_IRQ_MASK_RX_ERROR         = VSF_BIT(4),
    USART_IRQ_MASK_TX_ERROR         = VSF_BIT(5),
    USART_IRQ_MASK_ERROR            = USART_IRQ_MASK_RX_ERROR | USART_IRQ_MASK_TX_ERROR,

    //todo: add rx timeout support
    USART_IRQ_MASK_RX_TIMEOUT       = VSF_BIT(6),

    USART_IRQ_MASK                  =  USART_IRQ_MASK_RX     | USART_IRQ_MASK_TX
                                     | USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL
                                     | USART_IRQ_MASK_ERROR    | USART_IRQ_MASK_RX_TIMEOUT,
} em_usart_irq_mask_t;

typedef struct usart_status_t {
    uint8_t                         is_busy : 1;
    uint8_t                         rx_error_detected : 1;
    uint8_t                         tx_error_detected : 1;
    uint8_t                         rx_cancel : 1;
    uint8_t                         tx_cancel : 1;
    uint32_t                        more_status : 27;
} usart_status_t;

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_API_DECLARATION_PREFIX            vsf_hw
#define VSF_USART_CFG_INSTANCE_DECLARATION_PREFIX       VSF_HW
#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#endif /* VSF_HAL_USE_USART == ENABLED */
#endif /* __HAL_DRIVER_NUVOTON_M480_USART_H__ */
/* EOF */
