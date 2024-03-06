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

#ifndef __VSF_HAL_DISTBUS_USART_H__
#define __VSF_HAL_DISTBUS_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED && VSF_HAL_DISTBUS_USE_USART == ENABLED

// for stream
#include "service/vsf_service.h"

#if     defined(__VSF_HAL_DISTBUS_USART_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_DISTBUS_USART_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_DISTBUS_USART_CFG_MULTI_CLASS
#   define VSF_HAL_DISTBUS_USART_CFG_MULTI_CLASS    VSF_USART_CFG_MULTI_CLASS
#endif

#ifndef VSF_HAL_DISTBUS_USART_CFG_FIFO_SIZE
#   define VSF_HAL_DISTBUS_USART_CFG_FIFO_SIZE      1024
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if defined(__VSF_HAL_DISTBUS_USART_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_USART_CLASS_INHERIT__)
enum {
    // commands to slave
    VSF_HAL_DISTBUS_USART_CMD_INIT = 0,
    VSF_HAL_DISTBUS_USART_CMD_ENABLE,
    VSF_HAL_DISTBUS_USART_CMD_DISABLE,
    VSF_HAL_DISTBUS_USART_CMD_IRQ_ENABLE,
    VSF_HAL_DISTBUS_USART_CMD_IRQ_DISABLE,
    VSF_HAL_DISTBUS_USART_CMD_TX,

    // commands to host
    VSF_HAL_DISTBUS_USART_CMD_TXED_COUNT,
    VSF_HAL_DISTBUS_USART_CMD_RX,
    VSF_HAL_DISTBUS_USART_CMD_ISR,

    VSF_HAL_DISTBUS_USART_CMD_ADDR_RANGE,
};

typedef enum vsf_hal_distbus_usart_mode_t {
    // parity: 3 bits
    VSF_HAL_DISTBUS_USART_NO_PARITY             = (0x0ul << 0),
    VSF_HAL_DISTBUS_USART_EVEN_PARITY           = (0x1ul << 0),
    VSF_HAL_DISTBUS_USART_ODD_PARITY            = (0x2ul << 0),
    VSF_HAL_DISTBUS_USART_FORCE_0_PARITY        = (0x3ul << 0),
    VSF_HAL_DISTBUS_USART_FORCE_1_PARITY        = (0x4ul << 0),
    VSF_HAL_DISTBUS_USART_PARITY_MASK           = VSF_HAL_DISTBUS_USART_NO_PARITY
                                                | VSF_HAL_DISTBUS_USART_EVEN_PARITY
                                                | VSF_HAL_DISTBUS_USART_ODD_PARITY
                                                | VSF_HAL_DISTBUS_USART_FORCE_0_PARITY
                                                | VSF_HAL_DISTBUS_USART_FORCE_1_PARITY,

    // stopbit: 2 bits
    VSF_HAL_DISTBUS_USART_1_STOPBIT             = (0x0ul << 3),
    VSF_HAL_DISTBUS_USART_1_5_STOPBIT           = (0x1ul << 3),
    VSF_HAL_DISTBUS_USART_2_STOPBIT             = (0x2ul << 3),
    VSF_HAL_DISTBUS_USART_STOPBIT_MASK          = VSF_HAL_DISTBUS_USART_1_STOPBIT
                                                | VSF_HAL_DISTBUS_USART_1_5_STOPBIT
                                                | VSF_HAL_DISTBUS_USART_2_STOPBIT,

    // bit length: 3 bits
    VSF_HAL_DISTBUS_USART_5_BIT_LENGTH          = (0x0ul << 5),
    VSF_HAL_DISTBUS_USART_6_BIT_LENGTH          = (0x1ul << 5),
    VSF_HAL_DISTBUS_USART_7_BIT_LENGTH          = (0x2ul << 5),
    VSF_HAL_DISTBUS_USART_8_BIT_LENGTH          = (0x3ul << 5),
    VSF_HAL_DISTBUS_USART_9_BIT_LENGTH          = (0x4ul << 5),
    VSF_HAL_DISTBUS_USART_BIT_LENGTH_MASK       = VSF_HAL_DISTBUS_USART_5_BIT_LENGTH
                                                | VSF_HAL_DISTBUS_USART_6_BIT_LENGTH
                                                | VSF_HAL_DISTBUS_USART_7_BIT_LENGTH
                                                | VSF_HAL_DISTBUS_USART_8_BIT_LENGTH
                                                | VSF_HAL_DISTBUS_USART_9_BIT_LENGTH,

    // hwcontrol: 2 bits
    VSF_HAL_DISTBUS_USART_NO_HWCONTROL          = (0x0ul << 8),
    VSF_HAL_DISTBUS_USART_RTS_HWCONTROL         = (0x1ul << 8),
    VSF_HAL_DISTBUS_USART_CTS_HWCONTROL         = (0x2ul << 8),
    VSF_HAL_DISTBUS_USART_RTS_CTS_HWCONTROL     = (0x3ul << 8),
    VSF_HAL_DISTBUS_USART_HWCONTROL_MASK        = VSF_HAL_DISTBUS_USART_NO_HWCONTROL
                                                | VSF_HAL_DISTBUS_USART_RTS_HWCONTROL
                                                | VSF_HAL_DISTBUS_USART_CTS_HWCONTROL
                                                | VSF_HAL_DISTBUS_USART_RTS_CTS_HWCONTROL,

    // enable: 2 bits
    VSF_HAL_DISTBUS_USART_TX_ENABLE             = (0x1ul << 11),
    VSF_HAL_DISTBUS_USART_RX_ENABLE             = (0x1ul << 12),
    VSF_HAL_DISTBUS_USART_ENABLE_MASK           = VSF_HAL_DISTBUS_USART_TX_ENABLE
                                                | VSF_HAL_DISTBUS_USART_RX_ENABLE,

    VSF_HAL_DISTBUS_USART_MODE_ALL_BITS_MASK    = VSF_HAL_DISTBUS_USART_PARITY_MASK
                                                | VSF_HAL_DISTBUS_USART_STOPBIT_MASK
                                                | VSF_HAL_DISTBUS_USART_BIT_LENGTH_MASK
                                                | VSF_HAL_DISTBUS_USART_HWCONTROL_MASK
                                                | VSF_HAL_DISTBUS_USART_ENABLE_MASK,
} vsf_hal_distbus_usart_mode_t;

typedef enum vsf_hal_distbus_usart_irq_mask_t {
    VSF_HAL_DISTBUS_USART_IRQ_MASK_TX_CPL       = 1UL << 0,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_RX_CPL       = 1UL << 1,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_TX           = 1UL << 2,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_RX           = 1UL << 3,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_FRAME_ERR    = 1UL << 8,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_PARITY_ERR   = 1UL << 9,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_BREAK_ERR    = 1UL << 10,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_OVERFLOW_ERR = 1UL << 11,
    VSF_HAL_DISTBUS_USART_IRQ_MASK_RX_TIMEOUT   = 1UL << 12,

    VSF_HAL_DISTBUS_USART_IRQ_MASK_ERR          = VSF_HAL_DISTBUS_USART_IRQ_MASK_FRAME_ERR
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_PARITY_ERR
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_BREAK_ERR
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_OVERFLOW_ERR,

    VSF_HAL_DISTBUS_USART_IRQ_ALL_BITS_MASK     = VSF_HAL_DISTBUS_USART_IRQ_MASK_TX
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_RX
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_RX_TIMEOUT
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_TX_CPL
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_RX_CPL
                                                | VSF_HAL_DISTBUS_USART_IRQ_MASK_ERR,
} vsf_hal_distbus_usart_irq_mask_t;

typedef struct vsf_hal_distbus_usart_init_t {
    uint32_t                                    mode;
    uint32_t                                    baudrate;
    uint32_t                                    rx_timeout;
} VSF_CAL_PACKED vsf_hal_distbus_usart_init_t;

typedef struct vsf_hal_distbus_usart_isr_t {
    uint32_t                                    irq_mask;
} VSF_CAL_PACKED vsf_hal_distbus_usart_isr_t;

typedef struct vsf_hal_distbus_usart_txed_cnt_t {
    uint32_t                                    count;
} VSF_CAL_PACKED vsf_hal_distbus_usart_txed_cnt_t;
#endif

vsf_class(vsf_hal_distbus_usart_t) {
#if VSF_HAL_DISTBUS_USART_CFG_MULTI_CLASS == ENABLED
    public_member(
        implement(vsf_usart_t)
    )
#endif
    protected_member(
        vsf_distbus_service_t                   service;
    )
    private_member(
        vsf_distbus_t                           *distbus;
        struct {
            vsf_usart_isr_handler_t             *handler;
            void                                *target;
            vsf_usart_irq_mask_t                mask;
        } irq;
        struct {
            struct {
                vsf_mem_stream_t                stream;
                uint8_t                         buffer[VSF_HAL_DISTBUS_USART_CFG_FIFO_SIZE];
            } rx;
            struct {
                vsf_mem_stream_t                stream;
                uint8_t                         buffer[VSF_HAL_DISTBUS_USART_CFG_FIFO_SIZE];
                bool                            is_pending;
            } tx;
        } fifo;
        struct {
            struct {
                uint8_t                         *buffer;
                uint32_t                        size;
            } tx;
            struct {
                uint8_t                         *buffer;
                uint32_t                        size;
            } rx;
        } dma;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern uint32_t vsf_hal_distbus_usart_register_service(vsf_distbus_t *distbus, vsf_hal_distbus_usart_t *usart, void *info, uint32_t infolen);

#if defined(__VSF_HAL_DISTBUS_USART_CLASS_IMPLEMENT) || defined(__VSF_HAL_DISTBUS_USART_CLASS_INHERIT__)
extern vsf_usart_mode_t vsf_hal_distbus_usart_mode_to_generic_usart_mode(uint32_t hal_distbus_usart_mode);
extern uint32_t vsf_generic_usart_mode_to_hal_distbus_usart_mode(vsf_usart_mode_t generic_usart_mode);
extern vsf_usart_irq_mask_t vsf_hal_distbus_usart_irqmask_to_generic_usart_irqmask(uint32_t hal_distbus_usart_irqmask);
extern uint32_t vsf_generic_usart_irqmask_to_hal_distbus_usart_irqmask(vsf_usart_irq_mask_t generic_usart_irqmask);
#endif

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_DEC_PREFIX                vsf_hal_distbus
#define VSF_USART_CFG_DEC_UPCASE_PREFIX         VSF_HAL_DISTBUS
#define VSF_USART_CFG_DEC_FIFO_TO_REQUEST
#include "hal/driver/common/usart/usart_template.h"

#ifdef __cplusplus
}
#endif

#undef __VSF_HAL_DISTBUS_USART_CLASS_IMPLEMENT
#undef __VSF_HAL_DISTBUS_USART_CLASS_INHERIT__

#endif
#endif
/* EOF */

