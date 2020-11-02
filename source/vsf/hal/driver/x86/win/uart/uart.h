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

#ifndef __OSA_HAL_X86_WIN_USART_H__
#define __OSA_HAL_X86_WIN_USART_H__

 /*============================ INCLUDES ======================================*/

#include "hal/interface/vsf_interface_usart.h"

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_PORT_NUM
#   define VSF_USART_CFG_PORT_NUM                   8
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
#define __VSF_USART_EXTERN_REPEAD(__N, __DONT_CARE)                             \
                                  extern vsf_usart_t vsf_usart##__N;
/*============================ TYPES =========================================*/

enum em_usart_mode_t {
    USART_8_BIT_LENGTH              = 0X0000U,
    USART_9_BIT_LENGTH              = 0X1000U,

    USART_1_STOPBIT                 = 0X0000U,
    USART_2_STOPBIT                 = 0X2000U,

    USART_NO_PARITY                 = 0X0000U,
    USART_EVEN_PARITY               = 0X0400U,
    USART_ODD_PARITY                = 0X0600U,

    USART_NO_HWCONTROL              = 0X0000U,
    USART_RTS_HWCONTROL             = 0X0100U,
    USART_CTS_HWCONTROL             = 0X0200U,
    USART_RTS_CTS_HWCONTROL         = 0X0300U,

    USART_TX_EN                     = 0X0000U,
    USART_RX_EN                     = 0X0000U,
};

enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               = BIT(0),
    USART_IRQ_MASK_TX               = BIT(1),
    USART_IRQ_MASK_RX_CPL           = BIT(2),
    USART_IRQ_MASK_TX_CPL           = BIT(3),

    // optional
    USART_IRQ_MASK_RX_ERR           = BIT(4),
    USART_IRQ_MASK_TX_ERR           = BIT(5),
    USART_IRQ_MASK_ERR              = USART_IRQ_MASK_RX_ERR | USART_IRQ_MASK_TX_ERR,
};

struct usart_status_t {
    union {
        inherit(peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
};

typedef struct vsf_usart_win_expression_t {
    vsf_usart_t                         *vsf_usart_instance_ptr;
    uint8_t                             win_serial_port_num;
}vsf_usart_win_expression_t;
/*============================ GLOBAL VARIABLES ==============================*/
REPEAT_MACRO(VSF_USART_CFG_PORT_NUM, __VSF_USART_EXTERN_REPEAD, NULL)
/*============================ PROTOTYPES ====================================*/

void vsf_usart_get_can_used_port(uint8_t *available_number_port);
bool vsf_usart_get_com_num(vsf_usart_win_expression_t arr[], uint8_t size);
bool vsf_usart_port_isbusy();

#endif
#endif      // __OSA_HAL_X86_WIN_USART_H__