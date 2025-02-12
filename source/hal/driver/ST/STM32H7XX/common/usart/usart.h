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

#ifndef __HAL_DRIVER_GIGADEVICE_GD32H7XX_USART_H__
#define __HAL_DRIVER_GIGADEVICE_GD32H7XX_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "../../__device.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

// IPCore
#if     defined(__VSF_HAL_HW_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_HW_USART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"
// IPCore end

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note VSF_HW_USART_CFG_MULTI_CLASS should be implemented for IP drives and open to user,
 *          while VSF_HW_USART_CFG_MULTI_CLASS should be in usart.c.
 */

// IPCore
#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS             VSF_USART_CFG_MULTI_CLASS
#endif
// IPCore end

// HW
/*\note hw USART driver can reimplement vsf_usart_mode_t/vsf_usart_irq_mask_t/vsf_usart_status_t.
 *      To enable reimplementation, please enable macro below:
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_MODE for vsf_usart_mode_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS for vsf_usart_status_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK for vsf_usart_irq_mask_t
 *          VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL for vsf_usart_ctrl_t
 *      Reimplementation is used for optimization hw/IPCore drivers, reimplement the bit mask according to hw registers.
 *      *** DO NOT reimplement these in emulated drivers. ***
 */

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS       ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL          ENABLED
// HW end

// TODO: add comments about fifo2req

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
typedef enum vsf_usart_mode_t {
    // 0..1: STOP(13:12) in USART_CR2, shift right by 12 to avoid conflict with BIT_LENGTH
    VSF_USART_0_5_STOPBIT               = (1 << 12) >> 12,
    VSF_USART_1_STOPBIT                 = (0 << 12) >> 12,
    VSF_USART_1_5_STOPBIT               = (3 << 12) >> 12,
    VSF_USART_2_STOPBIT                 = (2 << 12) >> 12,

    // 2..3: RE(2)/TE(3) in USART_CR1
    VSF_USART_TX_ENABLE                 = (1 << 3),
    VSF_USART_TX_DISABLE                = (0 << 3),
    VSF_USART_RX_ENABLE                 = (1 << 2),
    VSF_USART_RX_DISABLE                = (0 << 2),

    // 4..5: RTSE(8)/CTSE(9) in USART_CR3, shift right by 4 to void conflict with PARITY
    VSF_USART_NO_HWCONTROL              = (0),
    VSF_USART_RTS_HWCONTROL             = (1 << 8) >> 4,
    VSF_USART_CTS_HWCONTROL             = (1 << 9) >> 4,
    VSF_USART_RTS_CTS_HWCONTROL         = VSF_USART_RTS_HWCONTROL
                                        | VSF_USART_CTS_HWCONTROL,

    // 6: HDSEL(3) in USART_CR3, shift left by 3
    VSF_USART_HALF_DUPLEX_ENABLE        = (1 << 3) << 3,
    VSF_USART_HALF_DUPLEX_DISABLE       = 0,

    // 9..10: PS(9)/PCE(10) in USART_CR1
    VSF_USART_NO_PARITY                 = (0 << 10),
    VSF_USART_ODD_PARITY                = (1 << 10) | (1 << 9),
    VSF_USART_EVEN_PARITY               = (1 << 10),

    // 11: CLKEN(11) in USART_CR1
    VSF_USART_SYNC                      = (1 << 11),

    // 12&28: M0(12):M1(28) in USART_CR1
    VSF_USART_9_BIT_LENGTH              = (1 << 12),
    VSF_USART_8_BIT_LENGTH              = (0),
    VSF_USART_7_BIT_LENGTH              = (1 << 28),

    // more vendor specified modes can be added here

    // 15: SWAP(15) in USART_CR2
    VSF_USART_SWAP                      = (1 << 15),
    // 16..17: RXINV(16)/TXINV(17) in USART_CR2
    VSF_USART_TX_INV                    = (1 << 17),
    VSF_USART_RX_INV                    = (1 << 16),
    // 18: OVER8(15) in USART_CR1
    VSF_USART_OVERSAMPLE_8              = (1 << 18),
    VSF_USART_OVERSAMPLE_16             = (0 << 18),
    VSF_USART_OVERSAMPLE_MASK           = (1 << 18),


    __VSF_HW_USART_CR1_MASK             = VSF_USART_9_BIT_LENGTH
                                        | VSF_USART_8_BIT_LENGTH
                                        | VSF_USART_7_BIT_LENGTH
                                        | VSF_USART_NO_PARITY
                                        | VSF_USART_ODD_PARITY
                                        | VSF_USART_EVEN_PARITY
                                        | VSF_USART_TX_ENABLE
                                        | VSF_USART_RX_ENABLE
                                        | VSF_USART_OVERSAMPLE_MASK,
    __VSF_HW_USART_CR2_MASK             = VSF_USART_SYNC
                                        | VSF_USART_0_5_STOPBIT
                                        | VSF_USART_1_STOPBIT
                                        | VSF_USART_1_5_STOPBIT
                                        | VSF_USART_2_STOPBIT
                                        | VSF_USART_TX_INV
                                        | VSF_USART_RX_INV
                                        | VSF_USART_SWAP,
    __VSF_HW_USART_CR3_MASK             = VSF_USART_RTS_CTS_HWCONTROL
                                        | VSF_USART_HALF_DUPLEX_ENABLE,


    // not supported, allocate unused bits
    VSF_USART_6_BIT_LENGTH              = (1 << 20),
    VSF_USART_5_BIT_LENGTH              = (2 << 20),
    VSF_USART_10_BIT_LENGTH             = (3 << 20),
    VSF_USART_FORCE_0_PARITY            = (0 << 8),
    VSF_USART_FORCE_1_PARITY            = (1 << 8),

    VSF_USART_TX_FIFO_THRESHOLD_EMPTY   = (0x0ul << 20),
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY
                                        = (0x1ul << 20),
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL
                                        = (0x2ul << 20),

    VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY
                                        = (0x0ul << 22),
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL
                                        = (0x1ul << 22),
    VSF_USART_RX_FIFO_THRESHOLD_FULL    = (0x2ul << 22),

    VSF_USART_SYNC_CLOCK_ENABLE         = (0x1ul << 23),
    VSF_USART_SYNC_CLOCK_DISABLE        = (0x0ul << 23),
    VSF_USART_SYNC_CLOCK_POLARITY_LOW   = (0x0ul << 24),
    VSF_USART_SYNC_CLOCK_POLARITY_HIGH  = (0x1ul << 24),
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE   = (0x0ul << 25),
    VSF_USART_SYNC_CLOCK_PHASE_2_EDGE   = (0x1ul << 25),

    __VSF_HW_USART_NOT_SUPPORT_MASK     = VSF_USART_5_BIT_LENGTH
                                        | VSF_USART_5_BIT_LENGTH
                                        | VSF_USART_FORCE_0_PARITY
                                        | VSF_USART_FORCE_1_PARITY,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    // usart fifo interrupt
    // 5: RFNEIE(5) in USART_CTL0
    VSF_USART_IRQ_MASK_RX               = (1 << 5),
    // 7: TFNFIE(7) in USART_CTL0
    VSF_USART_IRQ_MASK_TX               = (1 << 7),
    // 26: RTIE(26) in USART_CTL0
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (1 << 26),

    // usart request interrupt
    // TODO: add DMA support
    VSF_USART_IRQ_MASK_RX_CPL           = (0),
    VSF_USART_IRQ_MASK_TX_CPL           = (0),

    // usart error interrupt
    // 8: PERRIE(8) in USART_CTL0
    VSF_USART_IRQ_MASK_PARITY_ERR       = (1 << 8),

    // more vendor specified irq_masks can be added here

    __VSF_HW_USART_IRQ_MASK =           VSF_USART_IRQ_MASK_RX
                                        | VSF_USART_IRQ_MASK_TX
                                        | VSF_USART_IRQ_MASK_RX_TIMEOUT
                                        | VSF_USART_IRQ_MASK_RX_CPL
                                        | VSF_USART_IRQ_MASK_TX_CPL
                                        | VSF_USART_IRQ_MASK_PARITY_ERR,

    // not supported
    // 0..2
    VSF_USART_IRQ_MASK_FRAME_ERR        = (1 << 0),
    VSF_USART_IRQ_MASK_BREAK_ERR        = (1 << 1),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = (1 << 2),
    VSF_USART_IRQ_MASK_CTS              = (1 << 3),

    __VSF_HW_USART_NOT_SUPPORT_IRQ_MASK = VSF_USART_IRQ_MASK_FRAME_ERR
                                        | VSF_USART_IRQ_MASK_BREAK_ERR
                                        | VSF_USART_IRQ_MASK_OVERFLOW_ERR,
} vsf_usart_irq_mask_t;

typedef enum vsf_usart_ctrl_t {
    VSF_USART_CTRL_SEND_BREAK            = (1 << 1),

    VSF_USART_CTRL_SET_BREAK             = (1 << 8),
    VSF_USART_CTRL_CLEAR_BREAK           = (1 << 9),

    __VSF_HW_USART_SUPPORT_CTRL_MASK     = VSF_USART_CTRL_SEND_BREAK,
} vsf_usart_ctrl_t;

typedef struct vsf_usart_status_t {
    union {
        struct {
            uint32_t parity_err         : 1;    // PERR(0) in USART_STAT
            uint32_t frame_err          : 1;    // FERR(1) in USART_STAT
            uint32_t noise_err          : 1;    // NERR(2) in USART_STAT
            uint32_t overrun_err        : 1;    // ORERR(3) in USART_STAT
            uint32_t idle               : 1;    // IDLEF(4) in USART_STAT
            uint32_t rfne               : 1;    // RFNE(5) in USART_STAT
            uint32_t trans_complete     : 1;    // TC(6) in USART_STAT
            uint32_t tfnf               : 1;    // TFNF(7) in USART_STAT
            uint32_t __dummy0           : 1;    // LBDF(8) in USART_STAT
            uint32_t cts_changed        : 1;    // CTSF(9) in USART_STAT
            uint32_t cts_level          : 1;    // CTS(10) in USART_STAT
            uint32_t rx_timeouted       : 1;    // RTF(11) in USART_STAT
            uint32_t __dummy1           : 4;
            uint32_t is_busy            : 1;    // BSY(16) in USART_STAT
            uint32_t __dummy2           : 15;
        };
        uint32_t value;
    };
} vsf_usart_status_t;

// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// IPCore
/*\note Extern APIs for ip core diriver.
 *      There is no requirement about how APIs of IPCore drivers should be implemented.
 *      Just consider the simplicity for actual peripheral drivers.
 */
// IPCore end

#ifdef __cplusplus
}
#endif

// IPCore
#undef __VSF_HAL_HW_USART_CLASS_IMPLEMENT
#undef __VSF_HAL_HW_USART_CLASS_INHERIT__
// IPCore end

#endif      // VSF_HAL_USE_USART
#endif      // __HAL_DRIVER_GIGADEVICE_GD32H7XX_USART_H__
/* EOF */
