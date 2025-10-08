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

#ifndef __HAL_DRIVER_NATIONS_N32H76X_N32H78X_USART_H__
#define __HAL_DRIVER_NATIONS_N32H76X_N32H78X_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// HW
/*\note hw USART driver can reimplement following types:
 *      To enable reimplementation, please enable macro below:
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_MODE for vsf_usart_mode_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS for vsf_usart_status_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_usart_irq_mask_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL for vsf_usart_ctrl_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CFG for vsf_usart_cfg_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY for vsf_usart_capability_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CFG          DISABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY   DISABLED
// HW end

// TODO: add comments about fifo2req

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_usart_mode_t {
    // 1..2: TXEN(2)/RXEN(1) in USART_CTRL1
    VSF_USART_TX_ENABLE                 = (1 << 2),
    VSF_USART_TX_DISABLE                = (0 << 2),
    VSF_USART_RX_ENABLE                 = (1 << 1),
    VSF_USART_RX_DISABLE                = (0 << 1),

    // 3..4: PCEN(4)/PSEL(3) in USART_CTRL1
    VSF_USART_NO_PARITY                 = (0 << 4),
    VSF_USART_ODD_PARITY                = (1 << 4) | (1 << 3),
    VSF_USART_EVEN_PARITY               = (1 << 4),

    // 5: WL(5) in USART_CTRL1
    VSF_USART_8_BIT_LENGTH              = (0 << 5),
    VSF_USART_9_BIT_LENGTH              = (1 << 5),
#define VSF_USART_9_BIT_LENGTH          VSF_USART_9_BIT_LENGTH

    // 6..7: STPB(5:6) in USART_CTRL2, left shifted by 1
    VSF_USART_1_STOPBIT                 = (0 << 5) << 1,
    VSF_USART_0_5_STOPBIT               = (1 << 5) << 1,
#define VSF_USART_0_5_STOPBIT           VSF_USART_0_5_STOPBIT
    VSF_USART_1_5_STOPBIT               = (3 << 5) << 1,
#define VSF_USART_1_5_STOPBIT           VSF_USART_1_5_STOPBIT
    VSF_USART_2_STOPBIT                 = (2 << 5) << 1,
#define VSF_USART_2_STOPBIT             VSF_USART_2_STOPBIT

    // 8: CLKEN(8) in USART_CTRL2
    VSF_USART_SYNC_CLOCK_ENABLE         = (8 << 11),
#define VSF_USART_SYNC_CLOCK_ENABLE     VSF_USART_SYNC_CLOCK_ENABLE
    VSF_USART_SYNC_CLOCK_DISABLE        = (8 << 11),
#define VSF_USART_SYNC_CLOCK_DISABLE    VSF_USART_SYNC_CLOCK_DISABLE
    // 9: CLKPOL(9) in USART_CTRL2
    VSF_USART_SYNC_CLOCK_POLARITY_LOW           = (0 << 9),
#define VSF_USART_SYNC_CLOCK_POLARITY_LOW       VSF_USART_SYNC_CLOCK_POLARITY_LOW
    VSF_USART_SYNC_CLOCK_POLARITY_HIGH          = (1 << 9),
#define VSF_USART_SYNC_CLOCK_POLARITY_HIGH      VSF_USART_SYNC_CLOCK_POLARITY_HIGH
    // 10: CLKPHA(9) in USART_CTRL2
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE           = (0 << 10),
#define VSF_USART_SYNC_CLOCK_PHASE_1_EDGE       VSF_USART_SYNC_CLOCK_PHASE_1_EDGE
    VSF_USART_SYNC_CLOCK_PHASE_2_EDGE           = (1 << 10),
#define VSF_USART_SYNC_CLOCK_PHASE_2_EDGE       VSF_USART_SYNC_CLOCK_PHASE_2_EDGE
    // 11: LBCLK(11) in USART_CTRL2
    VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE        = (1 << 11),
    VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE       = (0 << 11),
#define VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE    VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE
#define VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE   VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE
#define VSF_USART_SYNC_CLOCK_LAST_BIT_MASK      VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE | VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE

    // 12|14..19: RXFTCFG(5..7)/TXFTCFG(2..4)/EN(0) in USART_FIFO, left shifted by 12
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL        = ((7 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY      = ((2 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY  VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY
    VSF_USART_TX_FIFO_THRESHOLD_ONE_EIGHTH      = ((0 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_ONE_EIGHTH  VSF_USART_TX_FIFO_THRESHOLD_ONE_EIGHTH
    VSF_USART_TX_FIFO_THRESHOLD_QUARTER         = ((1 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_QUARTER     VSF_USART_TX_FIFO_THRESHOLD_QUARTER
    VSF_USART_TX_FIFO_THRESHOLD_THREE_FOURTH    = ((1 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_THREE_FOURTH    VSF_USART_TX_FIFO_THRESHOLD_THREE_FOURTH
    VSF_USART_TX_FIFO_THRESHOLD_EMPTY           = ((5 << 2) | (1 << 0)) << 12,
#define VSF_USART_TX_FIFO_THRESHOLD_EMPTY       VSF_USART_TX_FIFO_THRESHOLD_EMPTY

    VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY       = ((7 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY   VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL       = ((2 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL   VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL
    VSF_USART_RX_FIFO_THRESHOLD_ONE_EIGHTH      = ((0 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_ONE_EIGHTH  VSF_USART_RX_FIFO_THRESHOLD_ONE_EIGHTH
    VSF_USART_RX_FIFO_THRESHOLD_QUARTER         = ((1 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_QUARTER     VSF_USART_RX_FIFO_THRESHOLD_QUARTER
    VSF_USART_RX_FIFO_THRESHOLD_THREE_FOURTH    = ((1 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_THREE_FOURTH    VSF_USART_RX_FIFO_THRESHOLD_THREE_FOURTH
    VSF_USART_RX_FIFO_THRESHOLD_FULL            = ((5 << 5) | (1 << 0)) << 12,
#define VSF_USART_RX_FIFO_THRESHOLD_FULL        VSF_USART_RX_FIFO_THRESHOLD_FULL

#define VSF_USART_TX_FIFO_THRESHOLD_MASK    ( VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL\
                                            | VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY\
                                            | VSF_USART_TX_FIFO_THRESHOLD_EMPTY\
                                            | VSF_USART_TX_FIFO_THRESHOLD_ONE_EIGHTH\
                                            | VSF_USART_TX_FIFO_THRESHOLD_QUARTER\
                                            | VSF_USART_TX_FIFO_THRESHOLD_THREE_FOURTH)
#define VSF_USART_RX_FIFO_THRESHOLD_MASK    ( VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY\
                                            | VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL\
                                            | VSF_USART_RX_FIFO_THRESHOLD_FULL\
                                            | VSF_USART_RX_FIFO_THRESHOLD_ONE_EIGHTH\
                                            | VSF_USART_RX_FIFO_THRESHOLD_QUARTER\
                                            | VSF_USART_RX_FIFO_THRESHOLD_THREE_FOURTH )

    // 20|22: RTSEN(2)/CTSEN(0) in USART_CTRL3, right shifted by 20
    VSF_USART_NO_HWCONTROL              = (0) << 16,
#define VSF_USART_NO_HWCONTROL          VSF_USART_NO_HWCONTROL
    VSF_USART_RTS_HWCONTROL             = (1 << 2) << 20,
#define VSF_USART_RTS_HWCONTROL         VSF_USART_RTS_HWCONTROL
    VSF_USART_CTS_HWCONTROL             = (1 << 0) << 20,
#define VSF_USART_CTS_HWCONTROL         VSF_USART_CTS_HWCONTROL
    VSF_USART_RTS_CTS_HWCONTROL         = VSF_USART_RTS_HWCONTROL
                                        | VSF_USART_CTS_HWCONTROL,
#define VSF_USART_RTS_CTS_HWCONTROL     VSF_USART_RTS_CTS_HWCONTROL

    // 24: HDMEN(4) in USART_CTRL3, right shifted by 20
    VSF_USART_HALF_DUPLEX_ENABLE        = (1 << 4) << 20,
#define VSF_USART_HALF_DUPLEX_ENABLE    VSF_USART_HALF_DUPLEX_ENABLE
    VSF_USART_HALF_DUPLEX_DISABLE       = 0,
#define VSF_USART_HALF_DUPLEX_DISABLE   VSF_USART_HALF_DUPLEX_DISABLE

    // more vendor specified modes can be added here

    // 26: OSPM(26) in USART_CTRL1
    VSF_USART_OVERSAMPLE_8              = (1 << 26),
    VSF_USART_OVERSAMPLE_16             = (0 << 26),
    VSF_USART_OVERSAMPLE_MASK           = (1 << 26),
    // 27: SWAP(27) in USART_CTRL1
    VSF_USART_SWAP                      = (1 << 27),
#define VSF_USART_SWAP                  VSF_USART_SWAP


    __VSF_HW_USART_CTRL1_MASK           = VSF_USART_9_BIT_LENGTH
                                        | VSF_USART_8_BIT_LENGTH
                                        | VSF_USART_NO_PARITY
                                        | VSF_USART_ODD_PARITY
                                        | VSF_USART_EVEN_PARITY
                                        | VSF_USART_TX_ENABLE
                                        | VSF_USART_RX_ENABLE
                                        | VSF_USART_OVERSAMPLE_MASK
                                        | VSF_USART_SWAP,

    __VSF_HW_USART_CTRL2_MASK           = VSF_USART_SYNC_CLOCK_ENABLE
                                        | VSF_USART_SYNC_CLOCK_DISABLE
                                        | VSF_USART_SYNC_CLOCK_POLARITY_LOW
                                        | VSF_USART_SYNC_CLOCK_POLARITY_HIGH
                                        | VSF_USART_SYNC_CLOCK_PHASE_1_EDGE
                                        | VSF_USART_SYNC_CLOCK_PHASE_2_EDGE
                                        | VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE
                                        | VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE
                                        | VSF_USART_0_5_STOPBIT
                                        | VSF_USART_1_STOPBIT
                                        | VSF_USART_1_5_STOPBIT
                                        | VSF_USART_2_STOPBIT,
    __VSF_HW_USART_CTRL2_SHIFT_MASK     = VSF_USART_0_5_STOPBIT
                                        | VSF_USART_1_STOPBIT
                                        | VSF_USART_1_5_STOPBIT
                                        | VSF_USART_2_STOPBIT,
    __VSF_HW_USART_CTRL2_SHIFT_BITS     = 1,

    __VSF_HW_USART_CTRL3_MASK           = VSF_USART_RTS_CTS_HWCONTROL
                                        | VSF_USART_HALF_DUPLEX_ENABLE,
    __VSF_HW_USART_CTRL3_SHIFT_MASK     = __VSF_HW_USART_CTRL3_MASK,
    __VSF_HW_USART_CTRL3_SHIFT_BITS     = 20,

    __VSF_HW_USART_FIFO_MASK            = VSF_USART_TX_FIFO_THRESHOLD_MASK
                                        | VSF_USART_RX_FIFO_THRESHOLD_MASK,
    __VSF_HW_USART_FIFO_SHIFT_MASK      = __VSF_HW_USART_FIFO_MASK,
    __VSF_HW_USART_FIFO_SHIFT_BITS      = 12,

    // not supported, allocate unused bits: 0|13|21|25|28..31
    // 28..30
    VSF_USART_10_BIT_LENGTH             = (1 << 28),
    VSF_USART_7_BIT_LENGTH              = (2 << 28),
    VSF_USART_6_BIT_LENGTH              = (3 << 28),
    VSF_USART_5_BIT_LENGTH              = (4 << 28),
    // 31
    VSF_USART_FORCE_0_PARITY            = (0 << 31),
    VSF_USART_FORCE_1_PARITY            = (1 << 31),

    __VSF_HW_USART_NOT_SUPPORT_MASK     = VSF_USART_5_BIT_LENGTH
                                        | VSF_USART_6_BIT_LENGTH
                                        | VSF_USART_7_BIT_LENGTH
                                        | VSF_USART_10_BIT_LENGTH
                                        | VSF_USART_FORCE_0_PARITY
                                        | VSF_USART_FORCE_1_PARITY,
} vsf_usart_mode_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_usart_irq_mask_t {
    // 3: CTSIEN(1) in USART_CTRL3
    VSF_USART_IRQ_MASK_CTS              = (1 << 3),
#define VSF_USART_IRQ_MASK_CTS          VSF_USART_IRQ_MASK_CTS

    // usart request interrupt
    // TODO: add DMA support
    VSF_USART_IRQ_MASK_RX_CPL           = (0),
    VSF_USART_IRQ_MASK_TX_CPL           = (0),

    // 7: TXDE(7) in USART_CTTL1
    VSF_USART_IRQ_MASK_TX               = (1 << 7),

    // 8: IDLEIEN(8) in USART_CTRL1
    VSF_USART_IRQ_MASK_RX_IDLE          = (1 << 8),
#define VSF_USART_IRQ_MASK_RX_IDLE      VSF_USART_IRQ_MASK_RX_IDLE

    // 9: RXDNEIE(9) in USART_CTRL1
    VSF_USART_IRQ_MASK_RX               = (1 << 9),

    // 11: TXCIEN(11) in USART_CTRL1
    VSF_USART_IRQ_MASK_TX_IDLE          = (1 << 11),
#define VSF_USART_IRQ_MASK_TX_IDLE      VSF_USART_IRQ_MASK_TX_IDLE

    // usart error interrupt
    // 12: PEIEN(12) in USART_CTRL1
    VSF_USART_IRQ_MASK_PARITY_ERR       = (1 << 12),

    // 15|17: RTOIEN(17)/RTOEN(15) in USART_CTRL2
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (1 << 15) | (1 << 17),
#define VSF_USART_IRQ_MASK_RX_TIMEOUT   VSF_USART_IRQ_MASK_RX_TIMEOUT

    // 18: OREF(13) in USART_STS
    VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR  = (1 << 18),
    // 19: OREF(14) in USART_STS
    VSF_USART_IRQ_MASK_NOISE_ERR        = (1 << 19),
#define VSF_USART_IRQ_MASK_NOISE_ERR    VSF_USART_IRQ_MASK_NOISE_ERR
    // 20: FEF(15) in USART_STS
    VSF_USART_IRQ_MASK_FRAME_ERR        = (1 << 20),
    VSF_USART_IRQ_MASK_BREAK_ERR        = VSF_USART_IRQ_MASK_FRAME_ERR,
#define VSF_USART_IRQ_MASK_BREAK_ERR    VSF_USART_IRQ_MASK_BREAK_ERR

    // more vendor specified irq_masks can be added here

    __VSF_HW_USART_IRQ_MASK             = VSF_USART_IRQ_MASK_RX
                                        | VSF_USART_IRQ_MASK_TX
                                        | VSF_USART_IRQ_MASK_RX_TIMEOUT
                                        | VSF_USART_IRQ_MASK_RX_IDLE
                                        | VSF_USART_IRQ_MASK_TX_IDLE
                                        | VSF_USART_IRQ_MASK_RX_CPL
                                        | VSF_USART_IRQ_MASK_TX_CPL
                                        | VSF_USART_IRQ_MASK_PARITY_ERR
                                        | VSF_USART_IRQ_MASK_CTS,
    __VSF_HW_USART_CTRL1_IRQ_MASK       = VSF_USART_IRQ_MASK_RX
                                        | VSF_USART_IRQ_MASK_TX
                                        | VSF_USART_IRQ_MASK_RX_IDLE
                                        | VSF_USART_IRQ_MASK_TX_IDLE
                                        | VSF_USART_IRQ_MASK_PARITY_ERR
                                        | VSF_USART_IRQ_MASK_RX_CPL
                                        | VSF_USART_IRQ_MASK_TX_CPL
                                        | VSF_USART_IRQ_MASK_PARITY_ERR,
    __VSF_HW_USART_CTRL2_IRQ_MASK       = VSF_USART_IRQ_MASK_RX_TIMEOUT,
    __VSF_HW_USART_CTRL3_IRQ_MASK       = VSF_USART_IRQ_MASK_CTS,
    __VSF_HW_USART_STS_IRQ_MASK         = VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR
                                        | VSF_USART_IRQ_MASK_NOISE_ERR
                                        | VSF_USART_IRQ_MASK_FRAME_ERR,
} vsf_usart_irq_mask_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
typedef enum vsf_usart_ctrl_t {
    // 13: SDBRK in USART_CTRL1
    VSF_USART_CTRL_SEND_BREAK           = (1 << 13),
#define VSF_USART_CTRL_SEND_BREAK       VSF_USART_CTRL_SEND_BREAK

    __VSF_HW_USART_SUPPORT_CMD_MASK     = VSF_USART_CTRL_SEND_BREAK,

    // not supported
    VSF_USART_CTRL_SET_BREAK            = (1 << 8),
    VSF_USART_CTRL_CLEAR_BREAK          = (1 << 9),
} vsf_usart_ctrl_t;
#endif

/** \note It's not obligated to inherit from vsf_peripheral_status_t.
 *      If not, there MUST be a is_busy bit in vsf_usart_status_t.
 */

#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == ENABLED
typedef struct vsf_usart_status_t {
    union {
// bits to be inverted:
//  TXC(is_tx_complete) => is_tx_busy
#define __VSF_HW_USART_STS_XOR_MASK     (1 << 8)
#define __VSF_HW_USART_STS_MASK         0xF7FF
        struct {
            uint32_t is_txfifo_full     : 1;    // TXFF(0) in USART_STS
            uint32_t is_rxfifo_full     : 1;    // RXFF(1) in USART_STS
            uint32_t is_txfifo_empty    : 1;    // TXFE(2) in USART_STS
            uint32_t is_rxfifo_empty    : 1;    // RXFE(3) in USART_STS
            uint32_t is_rxfifo_threshold: 1;    // RXFT(4) in USART_STS
            uint32_t is_txfifo_threahold: 1;    // TXFT(5) in USART_STS
            uint32_t is_rx_idle         : 1;    // IDLE(6) in USART_STS
            uint32_t txde               : 1;    // TXDEF(7) in USART_STS
            uint32_t is_tx_busy         : 1;    // TXC(8) in USART_STS
            uint32_t rxdne              : 1;    // RXDNE(9) in USART_STS
            uint32_t cts_changed        : 1;    // CTSF(10) in USART_STS
            uint32_t break_sent         : 1;
#define VSF_USART_STATUS_BREAK_SENT     11
            uint32_t parity_err         : 1;    // PEF(12) in USART_STS
            uint32_t overrun_err        : 1;    // OREF(13) in USART_STS
            uint32_t noise_err          : 1;    // NEF(14) in USART_STS
            uint32_t frame_err          : 1;    // FEF(15) in USART_STS
            uint32_t rx_timeout         : 1;    // RTOF(16) in USART_STS
            uint32_t tx_fifo_data       : 8;
            uint32_t rx_fifo_data       : 8;
        };
        uint32_t value;
    };
} vsf_usart_status_t;
#endif

/** \note Redefining vsf_usart_cfg_t usually requires declaring vsf_usart_t
 *        and vsf_usart_isr_handler_t types and define vsf_usart_isr_t.
 */

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CFG == ENABLED
typedef struct vsf_usart_t vsf_usart_t;
typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     vsf_usart_irq_mask_t irq_mask);
typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t         prio;
} vsf_usart_isr_t;
typedef struct vsf_usart_cfg_t {
    uint32_t                mode;
    uint32_t                baudrate;
    uint32_t                rx_timeout;
    vsf_usart_isr_t         isr;

    // more vendor specified cfg can be added here
} vsf_usart_cfg_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CAPABILITY == ENABLED
typedef struct vsf_usart_capability_t {
    vsf_usart_irq_mask_t irq_mask;

    uint32_t max_baudrate;
    uint32_t min_baudrate;

    uint8_t txfifo_depth;
    uint8_t rxfifo_depth;

    uint8_t max_data_bits;
    uint8_t min_data_bits;

    uint8_t support_rx_timeout          : 1;
    uint8_t support_send_break          : 1;
    uint8_t support_set_and_clear_break : 1;
    uint8_t support_sync_clock          : 1;
#   ifdef VSF_USART_IRQ_MASK_TX_IDLE
    uint8_t support_tx_idle             : 1;
#   endif
#   ifdef VSF_USART_IRQ_MASK_RX_IDLE
    uint8_t support_rx_idle             : 1;
#   endif

    // more vendor specified capability can be added here
} vsf_usart_capability_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_USART
#endif      // __HAL_DRIVER_NATIONS_N32H76X_N32H78X_USART_H__
/* EOF */
