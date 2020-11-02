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

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "../../__device.h"

#if VSF_HAL_USE_USART == ENABLED
#include "hal/interface/vsf_interface_usart.h"

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

enum em_usart_mode_t {

    USART_5_BIT_LENGTH      = LCR_DLS_5,
    USART_6_BIT_LENGTH      = LCR_DLS_6,
    USART_7_BIT_LENGTH      = LCR_DLS_7,
    USART_8_BIT_LENGTH      = LCR_DLS_8,

    USART_1_STOPBIT         = LCR_STOP_1,
    USART_2_STOPBIT         = LCR_STOP_2,

    USART_NO_PARITY         = 0x0000U,
    USART_EVEN_PARITY       = LCR_PEN | LCR_EPS_EVEN,
    USART_ODD_PARITY        = LCR_PEN | LCR_EPS_ODD,

    USART_LOOP_BACK_MODE    = MCR_LOOP << 8,
    USART_AUTO_FLOW_CONTROL = MCR_AFCE << 8,
    USART_IRDA_SIR_MODE     = MCR_SIRE << 8,
};

enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX          = BIT(0),
    USART_IRQ_MASK_TX          = BIT(1),
    USART_IRQ_MASK_RX_CPL      = BIT(2),
    USART_IRQ_MASK_TX_CPL      = BIT(3),
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t    is_busy    : 1;
            uint32_t    ir : 8;
        };
    };
};

struct vsf_usart_request_t {
    uint8_t               * buffer_ptr;
    uint32_t                max_count;
    uint32_t                count;
    bool                    is_busy;
};
typedef struct vsf_usart_request_t vsf_usart_request_t;

struct vsf_usart_t {
    uart_reg_t         * reg;
    IRQn_Type            UART_IRQn;

    vsf_usart_request_t  rx;
    vsf_usart_request_t  tx;

    vsf_usart_isr_t      isr;
    usart_status_t       event_status;
    uint8_t              irq_mask;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
#if USART_COUNT > 0 && VSF_HAL_USE_USART0 == ENABLED
extern const i_usart_t VSF_USART0;
extern vsf_usart_t vsf_usart0;
#endif
#if USART_COUNT > 1 && VSF_HAL_USE_USART1 == ENABLED
extern const i_usart_t VSF_USART1;
extern vsf_usart_t vsf_usart1;
#endif
#if USART_COUNT > 2 && VSF_HAL_USE_USART2 == ENABLED
extern const i_usart_t VSF_USART2;
extern vsf_usart_t vsf_usart2;
#endif
#if USART_COUNT > 3 && VSF_HAL_USE_USART3 == ENABLED
extern const i_usart_t VSF_USART3;
extern vsf_usart_t vsf_usart3;
#endif
#if USART_COUNT > 4 && VSF_HAL_USE_USART4 == ENABLED
extern const i_usart_t VSF_USART4;
extern vsf_usart_t vsf_usart4;
#endif
#if USART_COUNT > 5 && VSF_HAL_USE_USART5 == ENABLED
extern const i_usart_t VSF_USART5;
extern vsf_usart_t vsf_usart5;
#endif
#if USART_COUNT > 6 && VSF_HAL_USE_USART6 == ENABLED
extern const i_usart_t VSF_USART6;
extern vsf_usart_t vsf_usart6;
#endif
#if USART_COUNT > 7 && VSF_HAL_USE_USART7 == ENABLED
extern const i_usart_t VSF_USART7;
extern vsf_usart_t vsf_usart7;
#endif

#endif /*VSF_HAL_USE_USART*/

#endif /*__HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__*/
/* EOF */
