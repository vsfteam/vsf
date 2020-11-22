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

#ifndef __VSF_USART_H__
#define __VSF_USART_H__

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED
#include "../../__device.h"
#include "hal/interface/vsf_interface_usart.h"

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

    USART_8_BIT_LENGTH      = 0x0000U,
    USART_9_BIT_LENGTH      = 0x1000U,

    USART_1_STOPBIT         = 0x0000U,
    USART_2_STOPBIT         = 0x2000U,

    USART_NO_PARITY         = 0x0000U,
    USART_EVEN_PARITY       = 0x0400U,
    USART_ODD_PARITY        = 0x0600U,

    USART_NO_HWCONTROL      = 0x0000U,
    USART_RTS_HWCONTROL     = 0x0100U,
    USART_CTS_HWCONTROL     = 0x0200U,
    USART_RTS_CTS_HWCONTROL = 0x0300U,

    USART_RX_MODE           = 0x0004U,
    USART_TX_MODE           = 0x0008U,
    USART_TX_RX_MODE        = 0x000CU
};

enum em_usart_reg_rw_t {
    USART_EN               = 0x2000U,
    USART_DISEN            = 0xDFFFU,

    USART_SR_IDLE_FREE     = 0x10U,
    USART_SR_RXNE_FULL     = 0X20U,
    USART_SR_TC_TRUE       = 0X40U,
    USART_SR_TXE_TRUE      = 0X80U,

    USART_CR1_TXEIE_EN     = 0X0080U,
    USART_CR1_TXEIE_DISEN  = 0XFF7FU,
    USART_CR1_RXNEIE_EN    = 0X0020U,
    USART_CR1_RXNEIE_DISEN = 0XFFDFU,
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t    ip_is_busy   : 1;
            uint32_t    bw_is_busy   : 1;
            uint32_t    br_is_busy   : 1;
            uint32_t    event_status : 8;
            uint32_t                 : 21;
        };
    };
};

struct vsf_usart_t {
    USART_TypeDef      *reg_ptr;
    IRQn_Type           USART_IRQn;
    //pm_periph_async_clk_no_t    pclk_idx;
    //pm_sync_clk_no_t            sync_clk_idx;
    uint8_t            *write_buffer_ptr;
    uint8_t            *read_buffer_ptr;
    uint32_t            write_size;
    uint32_t            read_size;
    uint32_t            write_sizecounter;
    uint32_t            read_sizecounter;
    bool                is_writing;
    bool                is_loading;
    vsf_usart_event_t    event_rx;
    vsf_usart_event_t    event_tx;
    vsf_usart_event_t    event_rcv;
    vsf_usart_event_t    event_send;
    usart_status_t       event_status;
    usart_event_status_t event_mask;
};

#endif

#endif
