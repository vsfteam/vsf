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

#ifndef __HAL_DRIVER_USART_INTERFACE_H__
#define __HAL_DRIVER_USART_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_USART_CFG_MULTI_CLASS
#   define VSF_USART_CFG_MULTI_CLASS                ENABLED
#endif

// application code can redefine it
#ifndef VSF_USART_CFG_PREFIX
#   if VSF_USART_CFG_MULTI_CLASS == ENABLED
#       define VSF_USART_CFG_PREFIX                 vsf
#   elif defined(VSF_HW_USART_COUNT) && (VSF_HW_USART_COUNT != 0)
#       define VSF_USART_CFG_PREFIX                 vsf_hw
#   else
#       define VSF_USART_CFG_PREFIX                 vsf
#   endif
#endif

#ifndef VSF_USART_CFG_FUNCTION_RENAME
#   define VSF_USART_CFG_FUNCTION_RENAME            ENABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_MODE      DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK  DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS    DISABLED
#endif

#ifndef VSF_USART_CFG_FIFO_TO_REQUEST
#   define VSF_USART_CFG_FIFO_TO_REQUEST            DISABLED
#endif

#ifndef VSF_USART_CFG_REQUEST_EMPTY_IMPL
#   define VSF_USART_CFG_REQUEST_EMPTY_IMPL         DISABLED
#endif

#ifndef VSF_USART_CFG_REQUEST_API
#   define VSF_USART_CFG_REQUEST_API                ENABLED
#endif

#ifndef VSF_USART_CFG_FIFO_API
#   define VSF_USART_CFG_FIFO_API                   ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_USART_BASE_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,              usart, init,                  VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, enable,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,               usart, disable,               VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_capability_t, usart, capability,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_enable,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                   usart, irq_disable,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, vsf_usart_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_usart_status_t,     usart, status,                VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)

#if VSF_USART_CFG_FIFO_API == ENABLED
#   define __VSF_USART_FIFO_APIS(__prefix_name) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, rxfifo_get_data_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, rxfifo_read,           VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast16_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, txfifo_get_free_count, VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, uint_fast16_t,      usart, txfifo_write,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast16_t count)
#else
#   define __VSF_USART_FIFO_APIS(__prefix_name)
#endif

#if VSF_USART_CFG_REQUEST_API == ENABLED
#   define __VSF_USART_REQUEST_APIS(__prefix_name) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, request_rx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, request_tx,            VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr, void *buffer_ptr, uint_fast32_t count) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, cancel_rx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,          usart, cancel_tx,             VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,       usart, get_rx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr) \
        __VSF_HAL_TEMPLATE_API(__prefix_name, int_fast32_t,       usart, get_tx_count,          VSF_MCONNECT(__prefix_name, _usart_t) *usart_ptr)
#else
#   define __VSF_USART_REQUEST_APIS(__prefix_name)
#endif

#define VSF_USART_APIS(__prefix_name) \
    __VSF_USART_BASE_APIS(__prefix_name) \
    __VSF_USART_FIFO_APIS(__prefix_name) \
    __VSF_USART_REQUEST_APIS(__prefix_name)

/*============================ TYPES =========================================*/

#if VSF_USART_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_usart_mode_t {
    USART_NO_PARITY          = (0x0ul << 20),
    USART_EVEN_PARITY        = (0x1ul << 20),
    USART_ODD_PARITY         = (0x2ul << 20),
    USART_FORCE_0_PARITY     = (0x3ul << 20),
    USART_FORCE_1_PARITY     = (0x4ul << 20),
    USART_PARITY_MASK        = USART_NO_PARITY |
                               USART_EVEN_PARITY |
                               USART_ODD_PARITY |
                               USART_FORCE_0_PARITY |
                               USART_FORCE_1_PARITY,

    USART_1_STOPBIT          = (0x0ul << 23),
    USART_1_5_STOPBIT        = (0x1ul << 23),
    USART_2_STOPBIT          = (0x2ul << 23),
    USART_STOPBIT_MASK       = USART_1_STOPBIT |
                               USART_1_5_STOPBIT |
                               USART_2_STOPBIT,

    USART_5_BIT_LENGTH       = (0x0ul << 25),
    USART_6_BIT_LENGTH       = (0x1ul << 25),
    USART_7_BIT_LENGTH       = (0x2ul << 25),
    USART_8_BIT_LENGTH       = (0x3ul << 25),
    USART_9_BIT_LENGTH       = (0x4ul << 25),
    USART_BIT_LENGTH_MASK    = USART_5_BIT_LENGTH |
                               USART_6_BIT_LENGTH |
                               USART_7_BIT_LENGTH |
                               USART_8_BIT_LENGTH |
                               USART_9_BIT_LENGTH,

    USART_NO_HWCONTROL       = (0x0ul << 28),
    USART_RTS_HWCONTROL      = (0x1ul << 28),
    USART_CTS_HWCONTROL      = (0x2ul << 28),
    USART_RTS_CTS_HWCONTROL  = (0x3ul << 28),
    USART_HWCONTROL_MASK     = USART_NO_HWCONTROL |
                               USART_RTS_HWCONTROL |
                               USART_CTS_HWCONTROL |
                               USART_RTS_CTS_HWCONTROL,

    USART_TX_ENABLE          = (0x1ul << 30),
    USART_RX_ENABLE          = (0x1ul << 31),
    USART_ENABLE_MASK        = USART_TX_ENABLE |
                               USART_RX_ENABLE,

    USART_MODE_ALL_BITS_MASK = USART_PARITY_MASK |
                               USART_STOPBIT_MASK |
                               USART_BIT_LENGTH_MASK |
                               USART_HWCONTROL_MASK |
                               USART_ENABLE_MASK,
} vsf_usart_mode_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_usart_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    USART_IRQ_MASK_TX               = (0x1ul << 0),
    USART_IRQ_MASK_RX               = (0x1ul << 1),
    USART_IRQ_MASK_RX_TIMEOUT       = (0x1ul << 2),

    // request_rx/request_tx complete
    USART_IRQ_MASK_TX_CPL           = (0x1ul << 3),
    USART_IRQ_MASK_RX_CPL           = (0x1ul << 4),

    // optional
    // error
    USART_IRQ_MASK_FRAME_ERR        = (0x1ul << 5),
    USART_IRQ_MASK_PARITY_ERR       = (0x1ul << 6),
    USART_IRQ_MASK_BREAK_ERR        = (0x1ul << 7),
    USART_IRQ_MASK_OVERFLOW_ERR     = (0x1ul << 8),
    USART_IRQ_MASK_ERR              =   USART_IRQ_MASK_FRAME_ERR
                                      | USART_IRQ_MASK_PARITY_ERR
                                      | USART_IRQ_MASK_BREAK_ERR
                                      | USART_IRQ_MASK_OVERFLOW_ERR,

    USART_IRQ_MASK                  =   USART_IRQ_MASK_TX
                                      | USART_IRQ_MASK_RX
                                      | USART_IRQ_MASK_RX_TIMEOUT
                                      | USART_IRQ_MASK_TX_CPL
                                      | USART_IRQ_MASK_RX_CPL
                                      | USART_IRQ_MASK_ERR,

} vsf_usart_irq_mask_t;
#endif

typedef struct vsf_usart_t vsf_usart_t;

typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     vsf_usart_irq_mask_t irq_mask);

typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_usart_isr_t;

typedef struct vsf_usart_cfg_t {
    uint32_t                mode;
    uint32_t                baudrate;
    uint32_t                rx_timeout;
    vsf_usart_isr_t         isr;
} vsf_usart_cfg_t;

/*! \brief vsf_usart_status_t should implement peripheral_status_t
 *! \note uart_status_t should provide dedicated bits for
 *!       indicating whether a read or write timeout event is detected
 *!       or not:
 *!       bIsRXTimeOut
 *!       bIsTXTimeOut
 *!
 *! \note uart_status_t should provide dedicated bits for indicating
 *!       whether a read or write operation is cancelled by user:
 *!       bIsRXCancelled
 *!       bIsTXCancelled
 *!
 *! \note uart_status_t should provide dedicated bits for indicating
 *!       whether a read or write operation is encountered an error:
 *!       bIsRXErrorDetected
 *!       bIsTXErrorDetected
 *!
 *! \note:
 *!       Those bits will not be cleared until corresponding transmission
 *!       operation is request. E.g. When a Block.Read.Request is called,
 *!       then the bIsRXTimeOut bit should be cleared.
 */
#if VSF_USART_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_usart_status_t;
#endif

typedef struct vsf_usart_capability_t {
    inherit(vsf_peripheral_capability_t)
} vsf_usart_capability_t;

typedef struct vsf_usart_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_USART_APIS(vsf)
} vsf_usart_op_t;

#if VSF_USART_CFG_MULTI_CLASS == ENABLED
struct vsf_usart_t  {
    const vsf_usart_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, vsf_usart_cfg_t *cfg_ptr);

extern fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr);
extern fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr);

extern void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);
extern void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, vsf_usart_irq_mask_t irq_mask);

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif
extern vsf_usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr);
extern vsf_usart_capability_t vsf_usart_capability(vsf_usart_t *usart_ptr);
#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

extern uint_fast16_t vsf_usart_rxfifo_get_data_count(vsf_usart_t *usart_ptr);
extern uint_fast16_t vsf_usart_rxfifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);
extern uint_fast16_t vsf_usart_txfifo_get_free_count(vsf_usart_t *usart_ptr);
extern uint_fast16_t vsf_usart_txfifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);

extern vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
extern vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
extern vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr);
extern vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr);
extern int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr);
extern int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_USART_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_usart_t                                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_t)
#   define vsf_usart_init(__USART, ...)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_init)                  ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_enable(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_enable)                ((__vsf_usart_t *)__USART)
#   define vsf_usart_disable(__USART)                    VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_disable)               ((__vsf_usart_t *)__USART)
#   define vsf_usart_irq_enable(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_enable)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_irq_disable(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_disable)           ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_status(__USART)                     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_status)                ((__vsf_usart_t *)__USART)
#   define vsf_usart_rxfifo_get_data_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_get_data_count) ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_rxfifo_read(__USART, ...)           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_read)           ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_rxfifo_get_data_count(__USART, ...) VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_rxfifo_get_data_count) ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_txfifo_write(__USART, ...)          VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_txfifo_write)          ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_fifo_flush(__USART)                 VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fifo_flush)            ((__vsf_usart_t *)__USART)
#   define vsf_usart_request_rx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_rx)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_request_tx(__USART, ...)            VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_tx)            ((__vsf_usart_t *)__USART, ##__VA_ARGS__)
#   define vsf_usart_cancel_rx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_rx)             ((__vsf_usart_t *)__USART)
#   define vsf_usart_cancel_tx(__USART)                  VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_tx)             ((__vsf_usart_t *)__USART)
#   define vsf_usart_get_rx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_rx_count)          ((__vsf_usart_t *)__USART)
#   define vsf_usart_get_tx_count(__USART)               VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_tx_count)          ((__vsf_usart_t *)__USART)
#endif

#ifdef __cplusplus
}
#endif

#endif
