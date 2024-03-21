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

#ifndef __OSA_HAL_X86_WIN_USART_H__
#define __OSA_HAL_X86_WIN_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "../device.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK     ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_usart_mode_t {
    VSF_USART_8_BIT_LENGTH              = (0x00ul << 0),
    VSF_USART_9_BIT_LENGTH              = (0x01ul << 0),

    VSF_USART_1_STOPBIT                 = (0x00ul << 1),
    VSF_USART_1_5_STOPBIT               = (0x01ul << 1),
    VSF_USART_2_STOPBIT                 = (0x02ul << 1),

    VSF_USART_NO_PARITY                 = (0x00ul << 3),
    VSF_USART_EVEN_PARITY               = (0x01ul << 3),
    VSF_USART_ODD_PARITY                = (0x02ul << 3),

    VSF_USART_NO_HWCONTROL              = (0x00ul << 5),
    VSF_USART_RTS_HWCONTROL             = (0x01ul << 5),
    VSF_USART_CTS_HWCONTROL             = (0x02ul << 5),
    VSF_USART_RTS_CTS_HWCONTROL         = (0x03ul << 5),

    // unsupport for win
    VSF_USART_6_BIT_LENGTH              = (0x01ul << 7),
    VSF_USART_7_BIT_LENGTH              = (0x01ul << 8),
    VSF_USART_5_BIT_LENGTH              = (0x01ul << 9),

    VSF_USART_FORCE_0_PARITY            = (0x01ul << 10),
    VSF_USART_FORCE_1_PARITY            = (0x01ul << 11),

    VSF_USART_TX_ENABLE                 = (0x01ul << 12),
    VSF_USART_TX_DISABLE                = (0x00ul << 12),

    VSF_USART_RX_ENABLE                 = (0x01ul << 13),
    VSF_USART_RX_DISABLE                = (0x00ul << 13),

    WIN_USART_BIT_LENGTH_MASK           = VSF_USART_8_BIT_LENGTH |
                                          VSF_USART_9_BIT_LENGTH,
    WIN_USART_STOPBIT_MASK              = VSF_USART_1_STOPBIT |
                                          VSF_USART_1_5_STOPBIT |
                                          VSF_USART_2_STOPBIT,
    WIN_USART_PARITY_MASK               = VSF_USART_NO_PARITY |
                                          VSF_USART_EVEN_PARITY |
                                          VSF_USART_ODD_PARITY,
    WIN_USART_HWCONTROL_MASK            = VSF_USART_NO_HWCONTROL |
                                          VSF_USART_RTS_HWCONTROL |
                                          VSF_USART_CTS_HWCONTROL |
                                          VSF_USART_RTS_CTS_HWCONTROL,

    WIN_USART_MODE_ALL_BITS_MASK        = WIN_USART_BIT_LENGTH_MASK |
                                          WIN_USART_STOPBIT_MASK |
                                          WIN_USART_PARITY_MASK |
                                          WIN_USART_HWCONTROL_MASK,
} vsf_usart_mode_t;

typedef enum vsf_usart_irq_mask_t {
    VSF_USART_IRQ_MASK_RX               = VSF_BIT(0),
    VSF_USART_IRQ_MASK_TX               = VSF_BIT(1),
    VSF_USART_IRQ_MASK_RX_TIMEOUT       = VSF_BIT(2),

    // optional
    VSF_USART_IRQ_MASK_RX_CPL           = VSF_BIT(3),
    VSF_USART_IRQ_MASK_TX_CPL           = VSF_BIT(4),

    VSF_USART_IRQ_MASK_FRAME_ERR        = VSF_BIT(5),
    VSF_USART_IRQ_MASK_PARITY_ERR       = VSF_BIT(6),
    VSF_USART_IRQ_MASK_BREAK_ERR        = VSF_BIT(7),
    VSF_USART_IRQ_MASK_OVERFLOW_ERR     = VSF_BIT(8),
    VSF_USART_IRQ_MASK_TX_ERR           = VSF_BIT(9),

    WIN_USART_IRQ_ALL_BITS_MASK         = VSF_USART_IRQ_MASK_RX |
                                          VSF_USART_IRQ_MASK_TX,
} vsf_usart_irq_mask_t;

/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usart_t vsf_usart_t;

typedef struct vsf_usart_win_device_t {
    vsf_usart_t                         *instance;
    uint8_t                             port;
} vsf_usart_win_device_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern uint8_t vsf_win_usart_scan_devices(vsf_usart_win_device_t *devices, uint8_t device_num);

#endif
#endif      // __OSA_HAL_X86_WIN_USART_H__
