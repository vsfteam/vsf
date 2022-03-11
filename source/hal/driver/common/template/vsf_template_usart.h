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

#ifndef VSF_USART_CFG_MULTI_CLASS
#   define VSF_USART_CFG_MULTI_CLASS            DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_USART_CFG_MULTI_CLASS is enabled
#ifndef VSF_USART_CFG_IMPLEMENT_OP
#   if VSF_USART_CFG_MULTI_CLASS == ENABLED
#       define VSF_USART_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_USART_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_USART_CFG_PREFIX: use for macro vsf_usart_{init, enable, ...}
#ifndef VSF_USART_CFG_PREFIX
#   if VSF_USART_CFG_MULTI_CLASS == ENABLED
#       define VSF_USART_CFG_PREFIX           vsf
#   elif VSF_HAL_USE_FIFO2REQ_USART == ENABLED
#       define VSF_USART_CFG_PREFIX           vsf_fifo2req
#   elif defined(VSF_HW_USART_COUNT) && (VSF_HW_USART_COUNT != 0)
#       define VSF_USART_CFG_PREFIX           vsf_hw
#   elif VSF_HAL_USE_GPIO_USART == ENABLED
#       warning "Enable VSF_HAL_USE_USART support but no known implementation found"
#   endif
#endif

//#ifndef VSF_USART_CFG_FIFO_TO_REQUEST
//#   define VSF_USART_CFG_FIFO_TO_REQUEST            DISABLED
//#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_MODE
#   define VSF_USART_CFG_REIMPLEMENT_MODE           DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_IRQ_MASK
#   define VSF_USART_CFG_REIMPLEMENT_IRQ_MASK       DISABLED
#endif

#ifndef VSF_USART_CFG_REIMPLEMENT_STATUS
#   define VSF_USART_CFG_REIMPLEMENT_STATUS         DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_USART_INIT(usart_ptr, cfg_ptr)                                      \
    vsf_usart_init((vsf_usart_t *)usart_ptr, cfg_ptr)
#define VSF_USART_ENABLE(usart_ptr)                                             \
    vsf_usart_enable((vsf_usart_t *)usart_ptr)
#define VSF_USART_DISABLE(usart_ptr)                                            \
    vsf_usart_disable((vsf_usart_t *)usart_ptr)
#define VSF_USART_IRQ_ENABLE(usart_ptr, irq_mask)                               \
    vsf_usart_irq_enable((vsf_usart_t *)usart_ptr, irq_mask)
#define VSF_USART_IRQ_DISABLE(usart_ptr, irq_mask)                              \
    vsf_usart_irq_disable((vsf_usart_t *)usart_ptr, irq_mask)
#define VSF_USART_STATUS(usart_ptr)                                             \
    vsf_usart_status((vsf_usart_t *)usart_ptr)
#define VSF_USART_FIFO_READ(usart_ptr, buffer_ptr, count)                       \
    vsf_usart_fifo_read((vsf_usart_t *)usart_ptr, buffer_ptr, count)
#define VSF_USART_FIFO_WRITE(usart_ptr, buffer_ptr, count)                      \
    vsf_usart_fifo_write((vsf_usart_t *)usart_ptr, buffer_ptr, count)
#define VSF_USART_FIFO_FLUSH(usart_ptr)                                         \
    vsf_usart_fifo_flush((vsf_usart_t *)usart_ptr)
#define VSF_USART_REQUEST_RX(usart_ptr, buffer_ptr, count)                      \
    vsf_usart_request_rx((vsf_usart_t *)usart_ptr, buffer_ptr, count)
#define VSF_USART_REQUEST_TX(usart_ptr, buffer_ptr, count)                      \
    vsf_usart_request_tx((vsf_usart_t *)usart_ptr, buffer_ptr, count)
#define VSF_USART_CANCEL_RX(usart_ptr)                                          \
    vsf_usart_cancel_rx((vsf_usart_t *)usart_ptr)
#define VSF_USART_CANCEL_TX(usart_ptr)                                          \
    vsf_usart_cancel_tx((vsf_usart_t *)usart_ptr)
#define VSF_USART_GET_RX_COUNT(usart_ptr)                                       \
    vsf_usart_get_rx_count((vsf_usart_t *)usart_ptr)
#define VSF_USART_GET_TX_COUNT(usart_ptr)                                       \
    vsf_usart_get_tx_count((vsf_usart_t *)usart_ptr)


#define __VSF_USART_BASE_APIS(__prefix_name)                                                                                   \
    VSF_USART_API(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)                 \
    VSF_USART_API(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    VSF_USART_API(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    VSF_USART_API(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)

#define __VSF_USART_FIFO_APIS(__prefix_name)                                                                                   \
    VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)

#define __VSF_USART_REQUEST_APIS(__prefix_name)                                                                                \
    VSF_USART_API(__prefix_name, vsf_err_t,      init,         vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)                 \
    VSF_USART_API(__prefix_name, fsm_rt_t,       enable,       vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, fsm_rt_t,       disable,      vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, void,           irq_enable,   vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    VSF_USART_API(__prefix_name, void,           irq_disable,  vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)         \
    VSF_USART_API(__prefix_name, usart_status_t, status,       vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_read,    vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    VSF_USART_API(__prefix_name, uint_fast16_t,  fifo_write,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)\
    VSF_USART_API(__prefix_name, vsf_err_t,      request_rx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    VSF_USART_API(__prefix_name, vsf_err_t,      request_tx,   vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)\
    VSF_USART_API(__prefix_name, vsf_err_t,      cancel_rx,    vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, vsf_err_t,      cancel_tx,    vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, int_fast32_t,   get_rx_count, vsf_usart_t *usart_ptr)                                       \
    VSF_USART_API(__prefix_name, int_fast32_t,   get_tx_count, vsf_usart_t *usart_ptr)

#define VSF_USART_APIS(__prefix_name)                                                                                   \
    __VSF_USART_BASE_APIS(__prefix_name)                                                                                       \
    __VSF_USART_FIFO_APIS(__prefix_name)                                                                                       \
    __VSF_USART_REQUEST_APIS(__prefix_name)

/*============================ TYPES =========================================*/

#if VSF_USART_CFG_REIMPLEMENT_MODE == DISABLED
/*! \name usart working mode
 *! \note if your peripheral has any customised configuration, please reimplement
 *!       em_usart_mode_t
 */
typedef enum em_usart_mode_t {
    USART_NO_PARITY         = (0x0ul << 0),
    USART_EVEN_PARITY       = (0x1ul << 0),
    USART_ODD_PARITY        = (0x2ul << 0),
    USART_PARITY_MASK       = USART_NO_PARITY | USART_EVEN_PARITY | USART_ODD_PARITY,

    USART_1_STOPBIT         = (0x0ul << 2),
    USART_1_5_STOPBIT       = (0x1ul << 2),
    USART_2_STOPBIT         = (0x2ul << 2),
    USART_STOPBIT_MASK      = USART_1_STOPBIT | USART_1_5_STOPBIT | USART_2_STOPBIT,

    USART_5_BIT_LENGTH      = (0x0ul << 4),
    USART_6_BIT_LENGTH      = (0x1ul << 4),
    USART_7_BIT_LENGTH      = (0x2ul << 4),
    USART_8_BIT_LENGTH      = (0x3ul << 4),
    USART_9_BIT_LENGTH      = (0x4ul << 4),
    USART_BIT_LENGTH_MASK   =  USART_5_BIT_LENGTH | USART_6_BIT_LENGTH
                             | USART_7_BIT_LENGTH | USART_8_BIT_LENGTH
                             | USART_9_BIT_LENGTH,

    USART_NO_HWCONTROL      = 0x0000ul,
    USART_RTS_HWCONTROL     = 0x0100ul,
    USART_CTS_HWCONTROL     = 0x0200ul,
    USART_RTS_CTS_HWCONTROL = 0x0300ul,
    USART_HWCONTROL_MASK    =   USART_NO_HWCONTROL
                              | USART_RTS_HWCONTROL
                              | USART_CTS_HWCONTROL
                              | USART_RTS_CTS_HWCONTROL,

    USART_TX_ENABLE             = (0x1ul << 7),
    USART_RX_ENABLE             = (0x1ul << 8),
    USART_ENABLE_MASK           = USART_TX_ENABLE | USART_RX_ENABLE,
} em_usart_mode_t;
#endif

#if VSF_USART_CFG_REIMPLEMENT_IRQ_MASK == DISABLED
/*! \brief em_usart_irq_mask_t
 *! \note em_usart_irq_mask_t should provide irq masks
 */
typedef enum em_usart_irq_mask_t {
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
} em_usart_irq_mask_t;
#endif

typedef struct vsf_usart_t vsf_usart_t;

typedef void vsf_usart_isr_handler_t(void *target_ptr,
                                     vsf_usart_t *usart_ptr,
                                     em_usart_irq_mask_t irq_mask);

typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t          prio;
} vsf_usart_isr_t;

//! \name usart configuration
//! @{
typedef struct usart_cfg_t usart_cfg_t;
struct usart_cfg_t {
    uint32_t                mode;
    uint32_t                baudrate;
    uint32_t                rx_timeout;
    vsf_usart_isr_t         isr;
};
//! @}

typedef struct usart_status_t usart_status_t;
/*! \brief usart_status_t should implement peripheral_status_t
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
#if VSF_USART_CFG_REIMPLEMENT_STATUS == DISABLED
struct usart_status_t {
    union {
        inherit(peripheral_status_t)
        struct {
            uint32_t is_busy : 1;
        };
    };
};
#endif

/* usart_capability_t should implement peripheral_capability_t */
typedef struct usart_capability_t usart_capability_t;

typedef struct vsf_usart_op_t {
    vsf_err_t          (*init)         (vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr);
    fsm_rt_t           (*enable)       (vsf_usart_t *usart_ptr);
    fsm_rt_t           (*disable)      (vsf_usart_t *usart_ptr);
    void               (*irq_enable)   (vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask);
    void               (*irq_disable)  (vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask);
    usart_status_t     (*status)       (vsf_usart_t *usart_ptr);
    usart_capability_t (*capability)   (vsf_usart_t *usart_ptr);
    uint_fast16_t      (*fifo_read)    (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);
    uint_fast16_t      (*fifo_write)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);
    vsf_err_t          (*request_rx)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
    vsf_err_t          (*request_tx)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
    vsf_err_t          (*cancel_rx)    (vsf_usart_t *usart_ptr);
    vsf_err_t          (*cancel_tx)    (vsf_usart_t *usart_ptr);
    int_fast32_t       (*get_rx_count) (vsf_usart_t *usart_ptr);
    int_fast32_t       (*get_tx_count) (vsf_usart_t *usart_ptr);
} vsf_usart_op_t;

#if VSF_USART_CFG_MULTI_CLASS == ENABLED
struct vsf_usart_t  {
        const vsf_usart_op_t * op;
};
#endif

//! \name class: usart_t
//! @{
def_interface(i_usart_t)
    union {
        implement(i_peripheral_t);
        struct {
            usart_status_t     (*Status)(void);
            usart_capability_t (*Capability)(void);
        } USART;
    };
    vsf_err_t (*Init)(usart_cfg_t *pCfg);

    //! Irq
    struct {
        void (*Enable)(em_usart_irq_mask_t tEventMask);
        void (*Disable)(em_usart_irq_mask_t tEventMask);
    } Irq;

    //! fifo access
    struct {
        //!< read from fifo
        uint_fast16_t (*Read)(void *pBuffer, uint_fast16_t nCount);
        //!< write to fifo
        uint_fast16_t (*Write)(void *pBuffer, uint_fast16_t nCount);
        //!< flush fifo
        bool (*Flush)(void);
    } FIFO;

    struct {
        vsf_async_block_access_t Read;
        vsf_async_block_access_t Write;
    } Block;

end_def_interface(i_usart_t)
//! @}

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t          vsf_usart_init(           vsf_usart_t *usart_ptr,
                                                    usart_cfg_t *cfg_ptr);

extern fsm_rt_t           vsf_usart_enable(         vsf_usart_t *usart_ptr);
extern fsm_rt_t           vsf_usart_disable(        vsf_usart_t *usart_ptr);

extern void               vsf_usart_irq_enable(     vsf_usart_t *usart_ptr,
                                                    em_usart_irq_mask_t irq_mask);
extern void               vsf_usart_irq_disable(    vsf_usart_t *usart_ptr,
                                                    em_usart_irq_mask_t irq_mask);

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#endif
extern usart_status_t     vsf_usart_status(         vsf_usart_t *usart_ptr);
extern usart_capability_t vsf_usart_capability(     vsf_usart_t *usart_ptr);
#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

extern uint_fast16_t      vsf_usart_fifo_read(      vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast16_t count);
extern uint_fast16_t      vsf_usart_fifo_write(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast16_t count);

extern vsf_err_t          vsf_usart_request_rx(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast32_t count);
extern vsf_err_t          vsf_usart_request_tx(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast32_t count);
extern vsf_err_t          vsf_usart_cancel_rx(      vsf_usart_t *usart_ptr);
extern vsf_err_t          vsf_usart_cancel_tx(      vsf_usart_t *usart_ptr);
extern int_fast32_t       vsf_usart_get_rx_count(   vsf_usart_t *usart_ptr);
extern int_fast32_t       vsf_usart_get_tx_count(   vsf_usart_t *usart_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_usart_init           VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_init)
#define vsf_usart_enable         VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_enable)
#define vsf_usart_disable        VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_disable)
#define vsf_usart_irq_enable     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_enable)
#define vsf_usart_irq_disable    VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_irq_disable)
#define vsf_usart_status         VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_status)
#define vsf_usart_fifo_read      VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fifo_read)
#define vsf_usart_fifo_write     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fifo_write)
#define vsf_usart_fifo_flush     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_fifo_flush)
#define vsf_usart_request_rx     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_rx)
#define vsf_usart_request_tx     VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_request_tx)
#define vsf_usart_cancel_rx      VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_rx)
#define vsf_usart_cancel_tx      VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_cancel_tx)
#define vsf_usart_get_rx_count   VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_rx_count)
#define vsf_usart_get_tx_count   VSF_MCONNECT(VSF_USART_CFG_PREFIX, _usart_get_tx_count)

#ifdef __cplusplus
}
#endif

#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


#if defined(VSF_USART_CFG_DEC_PREFIX)
#   undef VSF_USART_API
#   define VSF_USART_API(__prefix_name, __return, __name, ...)                   \
    VSF_TEMPLATE_HAL_API_EXTERN(__prefix_name, _usart_, __return, __name, __VA_ARGS__)

// expand to:
//  extern vsf_err_t vsf_xxx_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr);
//  ...
VSF_USART_APIS(VSF_USART_CFG_DEC_PREFIX)

#   if defined(VSF_USART_CFG_DEC_UPPERCASE_PREFIX)
#       if VSF_MCONNECT(VSF_USART_CFG_DEC_UPPERCASE_PREFIX, _USART_COUNT)
#           define __VSF_USART_DEC_COUNT VSF_MCONNECT(VSF_USART_CFG_DEC_UPPERCASE_PREFIX, _USART_COUNT)

#           if VSF_MCONNECT(VSF_USART_CFG_DEC_UPPERCASE_PREFIX, _USART_MASK)
#               define __VSF_USART_DEC_MASK    VSF_MCONNECT(VSF_USART_CFG_DEC_UPPERCASE_PREFIX, _USART_MASK)
#           else
#               define __VSF_USART_DEC_MASK    ((1ul << __VSF_USART_DEC_COUNT) - 1)
#           endif

// expand to:
//  typedef vsf_xxx_usart_t vsf_xxx_usart_t;
typedef struct VSF_MCONNECT(VSF_USART_CFG_DEC_PREFIX, _usart_t) \
            VSF_MCONNECT(VSF_USART_CFG_DEC_PREFIX, _usart_t);

// expand to:
//  extern vsf_xxx_usart_t vsf_xxx_usart{0,1,2,3,...};
#           define __VSF_USART_DEC_LV0(__count, __dont_care)   \
                extern VSF_MCONNECT(VSF_USART_CFG_DEC_PREFIX, _usart_t) \
                    VSF_MCONNECT(VSF_USART_CFG_DEC_PREFIX, _usart, __count);

#           if __VSF_USART_DEC_MASK & (1 << 0)
                __VSF_USART_DEC_LV0(0, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 1)
                __VSF_USART_DEC_LV0(1, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 2)
                __VSF_USART_DEC_LV0(2, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 3)
                __VSF_USART_DEC_LV0(3, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 4)
                __VSF_USART_DEC_LV0(4, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 5)
                __VSF_USART_DEC_LV0(5, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 6)
                __VSF_USART_DEC_LV0(6, NULL)
#           endif
#           if __VSF_USART_DEC_MASK & (1 << 7)
                __VSF_USART_DEC_LV0(7, NULL)
#           endif

#           undef __VSF_USART_DEC_COUNT
#           undef __VSF_USART_DEC_MASK
#           undef __VSF_USART_DEC_LV0
#       endif

#       undef VSF_USART_CFG_DEC_UPPERCASE_PREFIX
#   endif   /* VSF_USART_CFG_DEC_UPPERCASE_PREFIX */

#   undef VSF_USART_CFG_DEC_PREFIX
#endif /* VSF_USART_CFG_DEC_PREFIX */

