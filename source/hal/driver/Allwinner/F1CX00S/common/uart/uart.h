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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

#if VSF_HAL_USE_USART == ENABLED

#ifndef VSF_HAL_USART_IMP_REQUEST_BY_FIFO
#   define VSF_HAL_USART_IMP_REQUEST_BY_FIFO    ENABLED
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum em_clk_reg_rw_t {
    USART1_CLK_EN           = 0x04000U,
    USART2_CLK_EN           = 0x20000U,
    USART3_CLK_EN           = 0x40000U,

    GPIOA_CLK_EN            = 0X00004U,
    GPIOB_CLK_EN            = 0X00008U,
};

enum em_gpio_reg_rw_t {
    USART1_GPIO_MODE_CLEAR  = 0XFFFFF00FU,
    USART1_GPIO_MODE        = 0x000008B0U,

    USART2_GPIO_MODE_CLEAR  = 0XFFFF00FFU,
    USART2_GPIO_MODE        = 0x00008B00U,

    USART3_GPIO_MODE_CLEAR  = 0XFFFF00FFU,
    USART3_GPIO_MODE        = 0x00008B00U,
};

enum vsf_usart_mode_t {
    VSF_USART_5_BIT_LENGTH      = LCR_DLS_5,
    VSF_USART_6_BIT_LENGTH      = LCR_DLS_6,
    VSF_USART_7_BIT_LENGTH      = LCR_DLS_7,
    VSF_USART_8_BIT_LENGTH      = LCR_DLS_8,

    VSF_USART_1_STOPBIT         = LCR_STOP_1,
    VSF_USART_2_STOPBIT         = LCR_STOP_2,

    VSF_USART_NO_PARITY         = 0x0000U,
    VSF_USART_EVEN_PARITY       = LCR_PEN | LCR_EPS_EVEN,
    VSF_USART_ODD_PARITY        = LCR_PEN | LCR_EPS_ODD,

    USART_LOOP_BACK_MODE        = MCR_LOOP << 8,
    USART_AUTO_FLOW_CONTROL     = MCR_AFCE << 8,
    USART_IRDA_SIR_MODE         = MCR_SIRE << 8,

    USART_TX_FIFO_THRES_EMPTY   = FCR_TFT_EMPTY   << 16,
    USART_TX_FIFO_THRES_2       = FCR_TFT_2       << 16,
    USART_TX_FIFO_THRES_QUARTER = FCR_TFT_QUARTER << 16,
    USART_TX_FIFO_THRES_HALF    = FCR_TFT_HALF    << 16,

    USART_RX_FIFO_THRES_1       = FCR_RT_1        << 16,
    USART_RX_FIFO_THRES_QUARTER = FCR_RT_QUARTER  << 16,
    USART_RX_FIFO_THRES_HALF    = FCR_RT_HALF     << 16,
    USART_RX_FIFO_THRES_2_LESS  = FCR_RT_2_LESS   << 16,

    // TODO:
    VSF_USART_TX_ENABLE             = 0,
    VSF_USART_RX_ENABLE             = 0,
};

enum vsf_usart_irq_mask_t {
    VSF_USART_IRQ_MASK_RX           = IER_ERBFI,
    VSF_USART_IRQ_MASK_TX           = IER_ETBEI,

#ifdef VSF_HAL_USART_IMP_REQUEST_BY_FIFO
    VSF_USART_IRQ_MASK_TX_CPL       = (1UL << 2),
    VSF_USART_IRQ_MASK_RX_CPL       = (1UL << 3),
#endif
};

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/template/vsf_template_usart.h"
#if VSF_HAL_USART_IMP_REQUEST_BY_FIFO == ENABLED
#   include "hal/driver/common/usart/__usart_common.h"
#endif

/*============================ TYPES =========================================*/

struct vsf_usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t        is_busy : 1;
            uint32_t        ir      : 8;
        };
    };
};

struct vsf_usart_t {
    uart_reg_t            * reg;

    IRQn_Type               UART_IRQn;
    vsf_usart_isr_t         isr;

    vsf_hal_usart_def_req_by_fifo()
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif /*VSF_HAL_USE_USART*/

#endif /*__HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__*/
/* EOF */
