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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED
#include "../__device.h"
#include "hal/interface/vsf_interface_pm.h"
#include "hal/interface/vsf_interface_usart.h"

// for dma
#include "../driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum em_usart_mode_t {
    // LINECTRL, bit 0-7
    USART_5_BIT_LENGTH          = (0x0ul << 5),
    USART_6_BIT_LENGTH          = (0x1ul << 5),
    USART_7_BIT_LENGTH          = (0x2ul << 5),
    USART_8_BIT_LENGTH          = (0x3ul << 5),

    USART_1_STOPBIT             = 0,
    USART_2_STOPBIT             = USART_LINECTRL_STP2,

    USART_NO_PARITY             = 0,
    USART_EVEN_PARITY           = USART_LINECTRL_PEN | USART_LINECTRL_EPS,
    USART_ODD_PARITY            = USART_LINECTRL_PEN,
    USART_FORCE_0_PARITY        = USART_LINECTRL_PEN | USART_LINECTRL_EPS | USART_LINECTRL_SPS,
    USART_FORCE_1_PARITY        = USART_LINECTRL_PEN | USART_LINECTRL_SPS,

    // CTRL2: bit8 - 23
    USART_TX_EN                 = USART_CTRL2_TXE,
    USART_RX_EN                 = USART_CTRL2_RXE,
    USART_RTS_EN                = USART_CTRL2_RTSEN,
    USART_CTS_EN                = USART_CTRL2_CTSEN,

    // LOOPBACK takes bit10, which is reserved in CTRL2
    USART_LOOPBACK_EN           = (1UL << 10),
    USART_TX_FIFO_THRES_1_8     = (0UL << 16),
    USART_TX_FIFO_THRES_1_4     = (1UL << 16),
    USART_TX_FIFO_THRES_1_2     = (2UL << 16),
    USART_TX_FIFO_THRES_3_4     = (3UL << 16),
    USART_TX_FIFO_THRES_7_8     = (4UL << 16),
    USART_RX_FIFO_THRES_1_8     = (0UL << 20),
    USART_RX_FIFO_THRES_1_4     = (1UL << 20),
    USART_RX_FIFO_THRES_1_2     = (2UL << 20),
    USART_RX_FIFO_THRES_3_4     = (3UL << 20),
    USART_RX_FIFO_THRES_7_8     = (4UL << 20),

    // CTRL3: bit24 - 31
    USART_9_BIT_LENGTH          = (1ul << (0 + 24)),
    USART_SAMPLE_RATE_8X        = (1ul << (1 + 24)),
    USART_SAMPLE_RATE_16X       = 0,
    USART_SYNC_MODE             = 0,
    USART_ASYNC_MODE            = (1ul << (4 + 24)),
    USART_SYNC_CLKOUT_EN        = (1ul << (6 + 24)),
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t            dummy               : 24;
            uint32_t            is_rx_fifo_empty    : 1;
            uint32_t            is_tx_fifo_full     : 1;
            uint32_t            is_rx_fifo_full     : 1;
            uint32_t            is_tx_fifo_empty    : 1;
            uint32_t            cts                 : 1;
            uint32_t            usart_is_busy       : 1;
        };
        uint32_t                value;
    };
};

enum em_usart_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    USART_IRQ_MASK_TX           = USART_INTR_TXIS,
    USART_IRQ_MASK_RX           = USART_INTR_RXIS,

    // request_rx/request_tx complete, use higher 16-bits
    //  USART_IRQ_MASK_TX_CPL and USART_IRQ_MASK_RX_CPL are issued from DMA interrupt
    USART_IRQ_MASK_TX_CPL       = 0x00010000,
    USART_IRQ_MASK_RX_CPL       = 0x00020000,
    USART_IRQ_MASK_RX_TIMEOUT   = 0x00040000,

    // optional
    // error
    USART_IRQ_MASK_FRAME_ERR    = USART_INTR_FEIS,
    USART_IRQ_MASK_PARITY_ERR   = USART_INTR_PEIS,
    USART_IRQ_MASK_BREAK_ERR    = USART_INTR_BEIS,
    USART_IRQ_MASK_OVERFLOW_ERR = USART_INTR_OEIS,
    USART_IRQ_MASK_ERR          = USART_IRQ_MASK_FRAME_ERR | USART_IRQ_MASK_PARITY_ERR
                                | USART_IRQ_MASK_BREAK_ERR | USART_IRQ_MASK_OVERFLOW_ERR,

    // FIFO
    USART_IRQ_MASK_TX_FIFO_EMPTY= USART_INTR_TFEIS,
};

/*============================ GLOBAL VARIABLES ==============================*/

#if USART_MAX_PORT >= 0 && VSF_HAL_USE_USART0 == ENABLED
#   if VSF_HAL_USE_LV1
extern const i_usart_t VSF_USART0;
#   endif
extern vsf_usart_t vsf_usart0;
#endif

#if USART_MAX_PORT >= 1 && VSF_HAL_USE_USART1 == ENABLED
#   if VSF_HAL_USE_LV1
extern const i_usart_t VSF_USART1;
#   endif
extern vsf_usart_t vsf_usart1;
#endif

/*============================ PROTOTYPES ====================================*/


#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_USART
#endif      // __VSF_USART_H__
