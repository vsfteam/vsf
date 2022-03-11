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

#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_PORT_NUM
#   define VSF_USART_CFG_PORT_NUM                       8
#endif

#define VSF_USART_CFG_PREFIX                            vsf

#define VSF_USART_CFG_REIMPLEMENT_MODE                  ENABLED
#define VSF_USART_CFG_REIMPLEMENT_IRQ_MASK              ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_USART_EXTERN_REPEAD(__N, __DONT_CARE)                             \
                                  extern vsf_usart_t vsf_hw_usart##__N;

/*============================ TYPES =========================================*/

typedef enum em_usart_mode_t {
    USART_8_BIT_LENGTH              = 0x0000ul,
    USART_9_BIT_LENGTH              = 0x1000ul,
    USART_BIT_LENGTH_MASK           = USART_8_BIT_LENGTH | USART_9_BIT_LENGTH,

    USART_1_STOPBIT                 = 0x0000ul,
    USART_2_STOPBIT                 = 0x2000ul,
    USART_STOPBIT_MASK              = USART_1_STOPBIT | USART_2_STOPBIT,

    USART_NO_PARITY                 = 0x0000ul,
    USART_EVEN_PARITY               = 0x0400ul,
    USART_ODD_PARITY                = 0x0600ul,
    USART_PARITY_MASK               = USART_NO_PARITY | USART_EVEN_PARITY | USART_ODD_PARITY,

    USART_NO_HWCONTROL              = 0x0000ul,
    USART_RTS_HWCONTROL             = 0x0100ul,
    USART_CTS_HWCONTROL             = 0x0200ul,
    USART_RTS_CTS_HWCONTROL         = 0x0300ul,
    USART_HWCONTROL_MASK            =  USART_NO_HWCONTROL  | USART_RTS_HWCONTROL
                                     | USART_CTS_HWCONTROL | USART_RTS_CTS_HWCONTROL,

    USART_TX_ENABLE                 = 0x0010ul,
    USART_RX_ENABLE                 = 0x0020ul,
    USART_ENABLE_MASK               = USART_TX_ENABLE | USART_RX_ENABLE,
} em_usart_mode_t;

typedef enum em_usart_irq_mask_t {
    USART_IRQ_MASK_RX               = VSF_BIT(0),
    USART_IRQ_MASK_TX               = VSF_BIT(1),
    USART_IRQ_MASK_RX_TIMEOUT       = VSF_BIT(2),

    USART_IRQ_MASK_RX_CPL           = VSF_BIT(3),
    USART_IRQ_MASK_TX_CPL           = VSF_BIT(4),

    // optional
    USART_IRQ_MASK_RX_ERR           = VSF_BIT(5),
    USART_IRQ_MASK_TX_ERR           = VSF_BIT(6),
    USART_IRQ_MASK_ERR              = USART_IRQ_MASK_RX_ERR | USART_IRQ_MASK_TX_ERR,

    USART_IRQ_MASK                  =   USART_IRQ_MASK_RX     | USART_IRQ_MASK_TX
                                      | USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL
                                      | USART_IRQ_MASK_ERR,
} em_usart_irq_mask_t;


/*============================ INCLUDES ======================================*/

// undef after include vsf_template_i2c.h
//#define VSF_USART_CFG_DEC_PREFIX                      vsf_hw
//#define VSF_USART_CFG_DEC_UPPERCASE_PREFIX            VSF_HW

#include "hal/driver/common/template/vsf_template_usart.h"

/*============================ TYPES =========================================*/

typedef struct vsf_usart_win_expression_t {
    vsf_usart_t *vsf_usart_instance_ptr;
    uint8_t      win_serial_port_num;
}vsf_usart_win_expression_t;

/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(VSF_USART_CFG_PORT_NUM, __VSF_USART_EXTERN_REPEAD, NULL)

/*============================ PROTOTYPES ====================================*/

extern void vsf_usart_get_can_used_port(uint8_t *available_number_port);
extern bool vsf_usart_get_com_num(vsf_usart_win_expression_t arr[], uint8_t size);
extern bool vsf_usart_port_isbusy();

#endif
#endif      // __OSA_HAL_X86_WIN_USART_H__