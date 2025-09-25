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

#if VSF_HAL_USE_USART == ENABLED

#include "../../__device.h"
#include "hal/driver/common/template/vsf_template_hal_driver.h"

/*============================ MACROS ========================================*/

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS    ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/


typedef enum vsf_usart_mode_t {
    VSF_USART_NO_PARITY                   = 0x0000U,
    VSF_USART_EVEN_PARITY                 = LCR_PEN | LCR_EPS_EVEN,
    VSF_USART_ODD_PARITY                  = LCR_PEN | LCR_EPS_ODD,
    F1C100S_USART_PARITY_MASK             = VSF_USART_NO_PARITY |
                                            VSF_USART_EVEN_PARITY |
                                            VSF_USART_ODD_PARITY,

    VSF_USART_1_STOPBIT                   = LCR_STOP_1,
    VSF_USART_2_STOPBIT                   = LCR_STOP_2,
    F1C100S_USART_STOPBIT_MASK            = VSF_USART_1_STOPBIT |
                                            VSF_USART_2_STOPBIT,

    VSF_USART_5_BIT_LENGTH                = LCR_DLS_5,
    VSF_USART_6_BIT_LENGTH                = LCR_DLS_6,
    VSF_USART_7_BIT_LENGTH                = LCR_DLS_7,
    VSF_USART_8_BIT_LENGTH                = LCR_DLS_8,
    F1C100S_USART_BIT_LENGTH_MASK         = VSF_USART_5_BIT_LENGTH |
                                            VSF_USART_6_BIT_LENGTH |
                                            VSF_USART_7_BIT_LENGTH |
                                            VSF_USART_8_BIT_LENGTH,

    VSF_USART_RTS_CTS_HWCONTROL           = MCR_AFCE << 8,
    F1C100S_USART_HWCONTROL_MASK          = VSF_USART_RTS_CTS_HWCONTROL,

    F1C100S_USART_IRDA_SIR_MODE           = MCR_SIRE << 8,
    F1C100S_USART_LOOP_BACK_MODE          = MCR_LOOP << 8,

    F1C100S_USART_TX_FIFO_THRES_EMPTY     = FCR_TFT_EMPTY   << 16,
    F1C100S_USART_TX_FIFO_THRES_2         = FCR_TFT_2       << 16,
    F1C100S_USART_TX_FIFO_THRES_QUARTER   = FCR_TFT_QUARTER << 16,
    F1C100S_USART_TX_FIFO_THRES_HALF      = FCR_TFT_HALF    << 16,
    F1C100S_USART_TX_FIFO_THRES_MASK      = F1C100S_USART_TX_FIFO_THRES_EMPTY |
                                              F1C100S_USART_TX_FIFO_THRES_2 |
                                              F1C100S_USART_TX_FIFO_THRES_QUARTER |
                                              F1C100S_USART_TX_FIFO_THRES_HALF,

    F1C100S_USART_RX_FIFO_THRES_1         = FCR_RT_1        << 16,
    F1C100S_USART_RX_FIFO_THRES_QUARTER   = FCR_RT_QUARTER  << 16,
    F1C100S_USART_RX_FIFO_THRES_HALF      = FCR_RT_HALF     << 16,
    F1C100S_USART_RX_FIFO_THRES_2_LESS    = FCR_RT_2_LESS   << 16,
    F1C100S_USART_RX_FIFO_THRES_MASK      = F1C100S_USART_RX_FIFO_THRES_1 |
                                            F1C100S_USART_RX_FIFO_THRES_QUARTER |
                                            F1C100S_USART_RX_FIFO_THRES_HALF |
                                            F1C100S_USART_RX_FIFO_THRES_HALF,

    F1C100S_USART_MODE_ALL_BITS_MASK      = F1C100S_USART_PARITY_MASK |
                                            F1C100S_USART_STOPBIT_MASK |
                                            F1C100S_USART_BIT_LENGTH_MASK |
                                            F1C100S_USART_HWCONTROL_MASK |
                                            F1C100S_USART_IRDA_SIR_MODE |
                                            F1C100S_USART_LOOP_BACK_MODE |
                                            F1C100S_USART_TX_FIFO_THRES_MASK |
                                            F1C100S_USART_RX_FIFO_THRES_MASK,

    VSF_USART_FORCE_0_PARITY              = (0x1ul << 31),
    VSF_USART_FORCE_1_PARITY              = (0x1ul << 30),
    VSF_USART_PARITY_MASK                 = F1C100S_USART_PARITY_MASK |
                                            VSF_USART_FORCE_0_PARITY |
                                            VSF_USART_FORCE_1_PARITY,

    VSF_USART_1_5_STOPBIT                 = (0x1ul << 29),
    VSF_USART_STOPBIT_MASK                = F1C100S_USART_STOPBIT_MASK |
                                            VSF_USART_1_5_STOPBIT,

    VSF_USART_9_BIT_LENGTH                = (0x4ul << 28),
    VSF_USART_BIT_LENGTH_MASK             = F1C100S_USART_BIT_LENGTH_MASK |
                                            VSF_USART_9_BIT_LENGTH,

    VSF_USART_NO_HWCONTROL                = (0x0ul << 26),
    VSF_USART_RTS_HWCONTROL               = (0x1ul << 26),
    VSF_USART_CTS_HWCONTROL               = (0x2ul << 26),
    VSF_USART_HWCONTROL_MASK              = F1C100S_USART_HWCONTROL_MASK |
                                            VSF_USART_NO_HWCONTROL |
                                            VSF_USART_RTS_HWCONTROL |
                                            VSF_USART_CTS_HWCONTROL,

    VSF_USART_TX_ENABLE                   = (0x0ul << 25),
    VSF_USART_RX_ENABLE                   = (0x0ul << 24),
    VSF_USART_ENABLE_MASK                 = VSF_USART_TX_ENABLE |
                                            VSF_USART_RX_ENABLE,

    VSF_USART_MODE_ALL_BITS_MASK          = VSF_USART_PARITY_MASK |
                                            VSF_USART_STOPBIT_MASK |
                                            VSF_USART_BIT_LENGTH_MASK |
                                            VSF_USART_HWCONTROL_MASK |
                                            VSF_USART_ENABLE_MASK,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    VSF_USART_IRQ_MASK_RX           = IER_ERBFI,
    VSF_USART_IRQ_MASK_TX           = IER_ETBEI,

    F1C100S_USART_IRQ_ALL_BITS_MASK = VSF_USART_IRQ_MASK_RX |
                                      VSF_USART_IRQ_MASK_TX,

    VSF_USART_IRQ_MASK_TX_CPL       = (0x1ul << 23),
    VSF_USART_IRQ_MASK_RX_CPL       = (0x1ul << 24),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 27),

    VSF_USART_IRQ_MASK_FRAME_ERR    = (0x1ul << 28),
    VSF_USART_IRQ_MASK_PARITY_ERR   = (0x1ul << 29),
    VSF_USART_IRQ_MASK_BREAK_ERR    = (0x1ul << 30),
    VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR
                                    = (0x1ul << 31),
    VSF_USART_IRQ_MASK_ERR          = VSF_USART_IRQ_MASK_FRAME_ERR |
                                      VSF_USART_IRQ_MASK_PARITY_ERR |
                                      VSF_USART_IRQ_MASK_BREAK_ERR |
                                      VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR,

    VSF_USART_IRQ_ALL_BITS_MASK     = F1C100S_USART_IRQ_ALL_BITS_MASK |
                                      VSF_USART_IRQ_MASK_RX_TIMEOUT |
                                      VSF_USART_IRQ_MASK_TX_CPL |
                                      VSF_USART_IRQ_MASK_RX_CPL |
                                      VSF_USART_IRQ_MASK_ERR,
} vsf_usart_irq_mask_t;

typedef struct vsf_usart_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        struct {
            uint32_t        is_busy : 1;
            uint32_t        ir      : 8;
        };
    };
} vsf_usart_status_t;

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_DEC_PREFIX            vsf_hw
#define VSF_USART_CFG_DEC_UPCASE_PREFIX     VSF_HW
#include "hal/driver/common/usart/usart_template.h"

/*============================ TYPES =========================================*/


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/


#endif /*VSF_HAL_USE_USART*/

#endif /*__HAL_DRIVER_ALLWINNER_F1CX00S_UART_H__*/
/* EOF */
