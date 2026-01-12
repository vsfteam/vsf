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

#ifndef __HAL_DRIVER_ARTERY_AT32F402_405_USART_H__
#define __HAL_DRIVER_ARTERY_AT32F402_405_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

// HW/IPCore
/**
 * \note When vsf_peripheral_status_t is inherited, vsf_template_hal_driver.h needs to be included
 */
#include "hal/driver/common/template/vsf_template_hal_driver.h"
// HW/IPCore end

/*\note Refer to template/README.md for usage cases.
 *      For peripheral drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 */

/*\note Includes CAN ONLY be put here. */
/*\note If current header is for a peripheral driver(hw driver), and inherit from an IPCore driver, include IPCore header here. */

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
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL         ENABLED
// HW end

// TODO: add comments about fifo2req

/*============================ TYPES =========================================*/

// HW/IPCore, not for emulated drivers
#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == ENABLED
typedef enum vsf_usart_mode_t {
    // 1: SLBEN(3) in USART_CTRL3, shift right by 2
    VSF_USART_HALF_DUPLEX_ENABLE                = (1 << 3) >> 2,
    VSF_USART_HALF_DUPLEX_DISABLE               = (0 << 3) >> 2,
#define VSF_USART_HALF_DUPLEX_ENABLE            VSF_USART_HALF_DUPLEX_ENABLE

    // 2..3: REN(2)/TEN(3) in USART_CTRL1
    VSF_USART_TX_ENABLE                         = (1 << 3),
    VSF_USART_TX_DISABLE                        = (0 << 3),
    VSF_USART_RX_ENABLE                         = (1 << 2),
    VSF_USART_RX_DISABLE                        = (0 << 2),
#define VSF_USART_TX_DISABLE                    VSF_USART_TX_DISABLE
#define VSF_USART_RX_DISABLE                    VSF_USART_RX_DISABLE

    // 6..7: RTSEN(8)/CTSEN(9) in USART_CTRL3, shift right by 2
    VSF_USART_NO_HWCONTROL                      = (0) >> 2,
    VSF_USART_RTS_HWCONTROL                     = (1 << 8) >> 2,
    VSF_USART_CTS_HWCONTROL                     = (1 << 9) >> 2,
    VSF_USART_RTS_CTS_HWCONTROL                 = VSF_USART_RTS_HWCONTROL
                                                | VSF_USART_CTS_HWCONTROL,
#define VSF_USART_RTS_HWCONTROL                 VSF_USART_RTS_HWCONTROL
#define VSF_USART_CTS_HWCONTROL                 VSF_USART_CTS_HWCONTROL
#define VSF_USART_RTS_CTS_HWCONTROL             VSF_USART_RTS_CTS_HWCONTROL

    // 9..10: PSEL(9)/PEN(10) in USART_CTRL1
    VSF_USART_NO_PARITY                         = (0 << 10),
    VSF_USART_ODD_PARITY                        = (1 << 10) | (1 << 9),
    VSF_USART_EVEN_PARITY                       = (1 << 10) | (0 << 9),

    // 12|28: DBN0(12)/DBN1(28) in USART_CTRL1
    VSF_USART_7_BIT_LENGTH                      = (0 << 12) | (1 << 28),
    VSF_USART_8_BIT_LENGTH                      = (0 << 12) | (0 << 28),
#define VSF_USART_7_BIT_LENGTH                  VSF_USART_7_BIT_LENGTH
    // if parity is enabled, the MSB of the 9-bit data will be over-written by parity bit
    //  so VSF_USART_9_BIT_LENGTH is not fully supported
//    VSF_USART_9_BIT_LENGTH                      = (1 << 12) | (0 << 28),
//#define VSF_USART_9_BIT_LENGTH                  VSF_USART_9_BIT_LENGTH

    // 17..18: STOP(12..13) in USART_CTRL2, shift left by 5
    VSF_USART_0_5_STOPBIT                       = (1 << 12) << 5,
    VSF_USART_1_STOPBIT                         = (0 << 12) << 5,
    VSF_USART_1_5_STOPBIT                       = (3 << 12) << 5,
    VSF_USART_2_STOPBIT                         = (2 << 12) << 5,
#define VSF_USART_0_5_STOPBIT                   VSF_USART_0_5_STOPBIT
#define VSF_USART_1_5_STOPBIT                   VSF_USART_1_5_STOPBIT
#define VSF_USART_2_STOPBIT                     VSF_USART_2_STOPBIT

    // 16: CLKEN(11) in USART_CTRL2, shift left by 5
    VSF_USART_SYNC_CLOCK_ENABLE                 = (1 << 11) << 5,
    VSF_USART_SYNC_CLOCK_DISABLE                = (0 << 11) << 5,
    // 14..15: CLKPHA(9)/CLKPOL(10) in USART_CTRL2, shift left by 5
    VSF_USART_SYNC_CLOCK_POLARITY_LOW           = (0 << 10) << 5,
    VSF_USART_SYNC_CLOCK_POLARITY_HIGH          = (1 << 10) << 5,
    VSF_USART_SYNC_CLOCK_PHASE_1_EDGE           = (0 << 9) << 5,
    VSF_USART_SYNC_CLOCK_PHASE_2_EDGE           = (1 << 9) << 5,
    // 13: LBCP(8) in USART_CTRL2, shift left by 5
    VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE        = (1 << 11) << 5,
    VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE       = (0 << 11) << 5,
#define VSF_USART_SYNC_CLOCK_ENABLE             VSF_USART_SYNC_CLOCK_ENABLE
#define VSF_USART_SYNC_CLOCK_DISABLE            VSF_USART_SYNC_CLOCK_DISABLE
#define VSF_USART_SYNC_CLOCK_POLARITY_LOW       VSF_USART_SYNC_CLOCK_POLARITY_LOW
#define VSF_USART_SYNC_CLOCK_POLARITY_HIGH      VSF_USART_SYNC_CLOCK_POLARITY_HIGH
#define VSF_USART_SYNC_CLOCK_PHASE_1_EDGE       VSF_USART_SYNC_CLOCK_PHASE_1_EDGE
#define VSF_USART_SYNC_CLOCK_PHASE_2_EDGE       VSF_USART_SYNC_CLOCK_PHASE_2_EDGE
#define VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE    VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE
#define VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE   VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE
#define VSF_USART_SYNC_CLOCK_LAST_BIT_MASK      VSF_USART_SYNC_CLOCK_LAST_BIT_ENABLE | VSF_USART_SYNC_CLOCK_LAST_BIT_DISABLE

    // more vendor specified modes can be added here

    // 20: TRPSWAP(15) in USART_CTRL2, shift left by 10
    VSF_USART_SWAP                              = (1 << 15) << 5,
#define VSF_USART_SWAP                          VSF_USART_SWAP
    // 21..22: RXREV(16)/TXREV(17) in USART_CTRL2, shift left by 10
    VSF_USART_TX_INV                            = (1 << 17) << 5,
    VSF_USART_RX_INV                            = (1 << 16) << 5,
#define VSF_USART_TX_INV                        VSF_USART_TX_INV
#define VSF_USART_RX_INV                        VSF_USART_RX_INV

    __VSF_HW_USART_CTRL1_MASK                   = VSF_USART_8_BIT_LENGTH
                                                | VSF_USART_7_BIT_LENGTH
                                                | VSF_USART_NO_PARITY
                                                | VSF_USART_ODD_PARITY
                                                | VSF_USART_EVEN_PARITY
                                                | VSF_USART_TX_ENABLE
                                                | VSF_USART_RX_ENABLE,

    __VSF_HW_USART_CTRL2_MASK                   = VSF_USART_SYNC_CLOCK_ENABLE
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
                                                | VSF_USART_2_STOPBIT
                                                | VSF_USART_SWAP
                                                | VSF_USART_TX_INV
                                                | VSF_USART_RX_INV,
    __VSF_HW_USART_CTRL2_SHIFT_BITS             = 5,

    __VSF_HW_USART_CTRL3_MASK                   = VSF_USART_RTS_CTS_HWCONTROL
                                                | VSF_USART_HALF_DUPLEX_ENABLE,
    __VSF_HW_USART_CTRL3_SHIFT_BITS             = 2,

    // not supported, allocate unused bits: 23..27 | 29..31
    // 23..25
    VSF_USART_5_BIT_LENGTH                      = (1 << 23),
    VSF_USART_6_BIT_LENGTH                      = (2 << 23),
    VSF_USART_9_BIT_LENGTH                      = (3 << 23),
    VSF_USART_10_BIT_LENGTH                     = (4 << 23),
    // 26..27
    VSF_USART_TX_FIFO_THRESHOLD_EMPTY           = (0 << 26), //!< \~english TX FIFO empty \~chinese 发送 FIFO 空
#define VSF_USART_TX_FIFO_THRESHOLD_EMPTY       VSF_USART_TX_FIFO_THRESHOLD_EMPTY
    VSF_USART_TX_FIFO_THRESHOLD_HALF_EMPTY      = (1 << 26), //!< \~english TX FIFO half empty \~chinese 发送 FIFO 半空
    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL        = (2 << 26), //!< \~english TX FIFO not full \~chinese 发送 FIFO 未满
#define VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL    VSF_USART_TX_FIFO_THRESHOLD_NOT_FULL
    // 29..30
    VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY       = (0 << 29), //!< \~english RX FIFO not empty \~chinese 接收 FIFO 非空
#define VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY   VSF_USART_RX_FIFO_THRESHOLD_NOT_EMPTY
    VSF_USART_RX_FIFO_THRESHOLD_HALF_FULL       = (1 << 29), //!< \~english RX FIFO half full \~chinese 接收 FIFO 半满
    VSF_USART_RX_FIFO_THRESHOLD_FULL            = (2 << 29), //!< \~english RX FIFO full \~chinese 接收 FIFO 满
#define VSF_USART_RX_FIFO_THRESHOLD_FULL        VSF_USART_RX_FIFO_THRESHOLD_FULL
    // 31
    VSF_USART_FORCE_0_PARITY                    = (0 << 31),
    VSF_USART_FORCE_1_PARITY                    = (1 << 31),

    __VSF_HW_USART_NOT_SUPPORT_MASK             = VSF_USART_5_BIT_LENGTH
                                                | VSF_USART_6_BIT_LENGTH
                                                | VSF_USART_9_BIT_LENGTH
                                                | VSF_USART_10_BIT_LENGTH
                                                | VSF_USART_FORCE_0_PARITY
                                                | VSF_USART_FORCE_1_PARITY,
} vsf_usart_mode_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == ENABLED
typedef enum vsf_usart_irq_mask_t {
    // TODO: add dma support
    VSF_USART_IRQ_MASK_TX_CPL           = (1 << 12),
    VSF_USART_IRQ_MASK_RX_CPL           = (1 << 13),

    // usart fifo interrupt
    // 7: TDBE(7) in USART_STS | TDBEIEN(7) in USART_CTRL1
    VSF_USART_IRQ_MASK_TX               = (1 << 7),
    // 6: TDC(6) in USART_STS | TDCIEN(6) in USART_CTRL1
    VSF_USART_IRQ_MASK_TX_IDLE          = (1 << 6),
#define VSF_USART_IRQ_MASK_TX_IDLE      VSF_USART_IRQ_MASK_TX_IDLE
    // 5: RDBF(5) in USART_STS | RDBFIEN(5) in USART_CTRL1
    VSF_USART_IRQ_MASK_RX               = (1 << 5),
    // 11: RTODF(11) in USART_STS | RTODIE(26) in USART_CTRL1
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (1 << 11),
    // 4: IDLEF(4) in USART_STS | IDLEIEN(4) in USART_CTRL1
    VSF_USART_IRQ_MASK_RX_IDLE          = (1 << 4),
#define VSF_USART_IRQ_MASK_RX_IDLE      VSF_USART_IRQ_MASK_RX_IDLE

    // 9: CTSCF(9) in USART_STS | CTSCFIEN(10) in USART_CTRL3
    VSF_USART_IRQ_MASK_CTS              = (1 << 9),

    // Error interrupt
    // 1: FERR(1) in USART_STS | ERRIEN(0) in USART_CTRL3
    VSF_USART_IRQ_MASK_FRAME_ERR        = (1 << 1),
    // 2: NERR(2) in USART_STS | ERRIEN(0) in USART_CTRL3
    VSF_USART_IRQ_MASK_NOISE_ERR        = (1 << 2),
    // 3: ROERR(3) in USART_STS | ERRIEN(0) in USART_CTRL3
    VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR  = (1 << 3),
    // 0: PERR(0) in USART_STS | PERRIEN(8) in USART_CTRL1
    VSF_USART_IRQ_MASK_PARITY_ERR       = (1 << 0),
    // 8: BFF(8) in USART_STS | BFIEN(6) in USART_CTRL2
    VSF_USART_IRQ_MASK_BREAK_ERR        = (1 << 8),

    // more vendor specified irq_masks can be added here

    __VSF_HW_USART_IRQ_MASK_CTRL1       = VSF_USART_IRQ_MASK_TX
                                        | VSF_USART_IRQ_MASK_TX_IDLE
                                        | VSF_USART_IRQ_MASK_RX
                                        | VSF_USART_IRQ_MASK_RX_IDLE,
} vsf_usart_irq_mask_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_CTRL == ENABLED
// By default, the template contains two control commands:
// - send BREAK signal (self-clearing)
// - set and clear BREAK signal
// The driver needs to select and implement according to the hardware support situation.
typedef enum vsf_usart_ctrl_t {
    // 0: SBF in USART_CTRL1
    VSF_USART_CTRL_SEND_BREAK           = (1 << 0),
    #define VSF_USART_CTRL_SEND_BREAK VSF_USART_CTRL_SEND_BREAK

    // more vendor specified command can be added here

    __VSF_HW_USART_SUPPORT_CMD_MASK     = VSF_USART_CTRL_SEND_BREAK,

    // not supported
    VSF_USART_CTRL_SET_BREAK            = (1 << 8),
    VSF_USART_CTRL_CLEAR_BREAK          = (1 << 9),
} vsf_usart_ctrl_t;
#endif
// HW/IPCore end

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_USART
#endif      // __HAL_DRIVER_ARTERY_AT32F402_405_USART_H__
/* EOF */
