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

#ifndef __HAL_DRIVER_PL011_UART_H__
#define __HAL_DRIVER_PL011_UART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#if     defined(__VSF_HAL_PL011_UART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_PL011_UART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_PL011_USART_CFG_MULTI_CLASS
#   define VSF_PL011_USART_CFG_MULTI_CLASS      VSF_USART_CFG_MULTI_CLASS
#endif

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE     ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS   ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_usart_mode_t {
    // UARTLCR_H.WLEN
    VSF_USART_8_BIT_LENGTH              = (0x3ul << 5),
    VSF_USART_7_BIT_LENGTH              = (0x2ul << 5),
    VSF_USART_6_BIT_LENGTH              = (0x1ul << 5),
    VSF_USART_5_BIT_LENGTH              = (0x0ul << 5),

    // UARTLCR_H.STP2
    VSF_USART_1_STOPBIT                 = (0x0ul << 3),
    VSF_USART_2_STOPBIT                 = (0x1ul << 3),

    // UARTLCR_H.PEN / UARTLCR_H.EPS / UARTLCR_H.SPS    PEN     EPS     SPS
    VSF_USART_NO_PARITY                 = 0x0ul,    //  0       X       X
    VSF_USART_ODD_PARITY                = 0x2ul,    //  1       0       0
    VSF_USART_EVEN_PARITY               = 0x6ul,    //  1       1       0
    VSF_USART_FORCE_0_PARITY            = 0xEul,    //  1       1       1
    VSF_USART_FORCE_1_PARITY            = 0xAul,    //  1       0       1

    PL011_USART_BIT_LENGTH_MASK         = VSF_USART_8_BIT_LENGTH
                                        | VSF_USART_7_BIT_LENGTH
                                        | VSF_USART_6_BIT_LENGTH
                                        | VSF_USART_5_BIT_LENGTH,
    PL011_USART_STOPBIT_MASK            = VSF_USART_1_STOPBIT
                                        | VSF_USART_2_STOPBIT,
    PL011_USART_PARITY_MASK             = VSF_USART_NO_PARITY
                                        | VSF_USART_ODD_PARITY
                                        | VSF_USART_EVEN_PARITY
                                        | VSF_USART_FORCE_0_PARITY
                                        | VSF_USART_FORCE_1_PARITY,
    PL011_USART_MODE_MASK               = PL011_USART_BIT_LENGTH_MASK
                                        | PL011_USART_STOPBIT_MASK
                                        | PL011_USART_PARITY_MASK,

    // UARTCR.TXE
    VSF_USART_TX_ENABLE                 = (0x1ul << (8 + 8)),
    VSF_USART_TX_DISABLE                = (0x0ul << (8 + 8)),

    // UARTCR.RXE
    VSF_USART_RX_ENABLE                 = (0x1ul << (8 + 9)),
    VSF_USART_RX_DISABLE                = (0x0ul << (8 + 9)),

    // UARTCR.CTSEN / UARTCR.RTSEN
    VSF_USART_NO_HWCONTROL              = (0x0ul << (8 + 14)),
    VSF_USART_RTS_HWCONTROL             = (0x1ul << (8 + 14)),
    VSF_USART_CTS_HWCONTROL             = (0x2ul << (8 + 14)),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x3ul << (8 + 14)),

    PL011_USART_ENABLE_MASK             = VSF_USART_TX_ENABLE
                                        | VSF_USART_RX_ENABLE
                                        | VSF_USART_RTS_CTS_HWCONTROL,

    // Not supported by hardware, just keep for build
    VSF_USART_9_BIT_LENGTH              = (0x1ul << 24),
    VSF_USART_1_5_STOPBIT               = (0x1ul << 25),
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    VSF_USART_IRQ_MASK_RX               = (0x1ul << 4),
    VSF_USART_IRQ_MASK_TX               = (0x1ul << 5),

    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 6),

    // usart error interrupt
    VSF_USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 7),
    VSF_USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 8),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 9),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 10),

    PL011_USART_IRQ_MASK_ERROR          = VSF_USART_IRQ_MASK_FRAME_ERR
                                        | VSF_USART_IRQ_MASK_PARITY_ERR
                                        | VSF_USART_IRQ_MASK_BREAK_ERR
                                        | VSF_USART_IRQ_MASK_OVERFLOW_ERR,
    PL011_USART_IRQ_MASK_FIFO           = VSF_USART_IRQ_MASK_RX
                                        | VSF_USART_IRQ_MASK_TX,
    PL011_USART_IRQ_MASK                = PL011_USART_IRQ_MASK_FIFO
                                        | VSF_USART_IRQ_MASK_RX_TIMEOUT
                                        | PL011_USART_IRQ_MASK_ERROR,
} vsf_usart_irq_mask_t;

typedef struct vsf_usart_status_t {
    union {
        struct {
            uint32_t            cts : 1;
            uint32_t            dsr : 1;
            uint32_t            dcd : 1;
            uint32_t            is_busy : 1;
            uint32_t            rxfe : 1;
            uint32_t            txff : 1;
            uint32_t            rxff : 1;
            uint32_t            txfe : 1;
            uint32_t            ri : 1;
        };
        uint32_t                value;
    };
} vsf_usart_status_t;

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/

// IP cores which will export class(es), need to include template before definition
//  of the class. Because the class member(s) maybe depend on the template.
#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ TYPES =========================================*/

vsf_class(vsf_pl011_usart_t) {
#if VSF_PL011_USART_CFG_MULTI_CLASS == ENABLED
    public_member(
        vsf_usart_t             vsf_usart;
    )
#endif
    public_member(
        void                   *reg;
    )
    protected_member(
        vsf_usart_isr_t         isr;
   )
};

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_pl011_usart_init(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_cfg_t *cfg_ptr, uint_fast32_t clk_hz);
extern void vsf_pl011_usart_fini(vsf_pl011_usart_t *pl011_usart_ptr);
extern vsf_usart_capability_t vsf_pl011_usart_capability(vsf_pl011_usart_t *pl011_usart_ptr, uint_fast32_t clk_hz);
extern fsm_rt_t vsf_pl011_usart_enable(vsf_pl011_usart_t *pl011_usart_ptr);
extern fsm_rt_t vsf_pl011_usart_disable(vsf_pl011_usart_t *pl011_usart_ptr);
extern vsf_usart_irq_mask_t vsf_pl011_usart_irq_enable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask);
extern vsf_usart_irq_mask_t vsf_pl011_usart_irq_disable(vsf_pl011_usart_t *pl011_usart_ptr, vsf_usart_irq_mask_t irq_mask);
extern vsf_usart_status_t vsf_pl011_usart_status(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_rxfifo_get_data_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_rxfifo_read(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count);
extern uint_fast16_t vsf_pl011_usart_txfifo_get_free_count(vsf_pl011_usart_t *pl011_usart_ptr);
extern uint_fast16_t vsf_pl011_usart_txfifo_write(vsf_pl011_usart_t *pl011_usart_ptr, void *buffer_ptr, uint_fast16_t count);
extern uintptr_t vsf_pl011_usart_rxdma_config(vsf_pl011_usart_t *pl011_usart_ptr, bool enable);
extern uintptr_t vsf_pl011_usart_txdma_config(vsf_pl011_usart_t *pl011_usart_ptr, bool enable);
extern void vsf_pl011_usart_irqhandler(vsf_pl011_usart_t *pl011_usart_ptr);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
