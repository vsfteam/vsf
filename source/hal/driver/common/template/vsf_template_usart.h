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

#ifndef __HAL_DRIVER_USART_INTERFACE_H__
#define __HAL_DRIVER_USART_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#ifndef VSF_USART_CFG_MULTI_INSTANCES
#   define VSF_USART_CFG_MULTI_INSTANCES DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/


#if VSF_USART_CFG_MULTI_INSTANCES == DISABLED
#   ifndef VSF_USART_CFG_PREFIX
#       define VSF_USART_CFG_PREFIX vsf_hw
#   endif
#   define ____VSF_USART_WRAPPER(__header, __api)   __header ## _ ## __api
#   define __VSF_USART_WRAPPER(__header, __api)     ____VSF_USART_WRAPPER(__header, __api)
#   define vsf_usart_init           __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_init)
#   define vsf_usart_enable         __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_enable)
#   define vsf_usart_disable        __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_disable)
#   define vsf_usart_irq_enable     __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_irq_enable)
#   define vsf_usart_irq_disable    __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_irq_disable)
#   define vsf_usart_status         __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_status)
#   define vsf_usart_fifo_read      __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_fifo_read)
#   define vsf_usart_fifo_write     __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_fifo_write)
#   define vsf_usart_fifo_flush     __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_fifo_flush)
#   define vsf_usart_request_rx     __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_request_rx)
#   define vsf_usart_request_tx     __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_request_tx)
#   define vsf_usart_cancel_rx      __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_cancel_rx)
#   define vsf_usart_cancel_tx      __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_cancel_tx)
#   define vsf_usart_get_rx_count   __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_get_rx_count)
#   define vsf_usart_get_tx_count   __VSF_USART_WRAPPER(VSF_USART_CFG_PREFIX, usart_get_tx_count)
#endif

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

/*============================ TYPES =========================================*/

typedef enum em_usart_mode_t em_usart_mode_t;

/*! \name usart working mode
 *! \note if your peripheral has any customised configuration, please add it by yourself
 *!       when implementing em_usart_mode_t
 *!
//! @{
enum em_usart_mode_t {
    USART_NO_PARITY         = 0x00,
    USART_EVEN_PARITY       = 0x18,
    USART_ODD_PARITY        = 0x08,
    USART_1_STOPBIT         = 0x00,
    USART_2_STOPBIT         = 0x40,
    USART_X_BIT_LENGTH      = 0x00,
    USART_SYNC_MODE         = 0x0400,
    USART_ASYNC_MODE        = 0x0000,
    USART_SYNC_CLKOUT_EN    = 0,
    USART_TX_EN             = 0,
    USART_RX_EN             = 0,
    USART_RTS_EN            = 0,
    USART_CTS_EN            = 0,

    // customised functionality
    USART_NO_AUTO_BAUD      = 0x00,
    USART_AUTO_BAUD_MODE0   = 0x01,
    USART_AUTO_BAUD_MODE1   = 0x03,
    USART_AUTO_RESTART      = 0x04,
    USART_NO_AUTO_RESTART   = 0x00,
    USART_FORCE_1_PARITY    = 0x28,
    USART_FORCE_0_PARITY    = 0x38,
    USART_ENABLE_FIFO       = 0x00,
    USART_DISABLE_FIFO      = 0x80,
};
//! @}
*/

/*! \brief em_usart_irq_mask_t
 *! \note em_usart_irq_mask_t should provide irq masks
//! @{
enum em_usart_irq_mask_t {
    // TX/RX reach fifo threshold, threshold on some devices is bound to 1
    USART_IRQ_MASK_TX,
    USART_IRQ_MASK_RX,

    // request_rx/request_tx complete
    USART_IRQ_MASK_TX_CPL,
    USART_IRQ_MASK_RX_CPL,

    // optional
    // error
    USART_IRQ_MASK_FRAME_ERR,
    USART_IRQ_MASK_PARITY_ERR,
    USART_IRQ_MASK_BREAK_ERR,
    USART_IRQ_MASK_OVERFLOW_ERR,
    USART_IRQ_MASK_ERR      = ALL_ERROR_MASK,

    // FIFO
    USART_IRQ_MASK_RX_FIFO_NOT_EMPTY,
    USART_IRQ_MASK_RX_FIFO_FULL,
    USART_IRQ_MASK_TX_FIFO_EMPTY,
};
//! @}
 */
typedef enum em_usart_irq_mask_t em_usart_irq_mask_t;

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
typedef struct usart_status_t usart_status_t;

#if VSF_USART_CFG_MULTI_INSTANCES == ENABLED
//! \name usart multiplex
//! @{
typedef struct vsf_usart_op_t {
    vsf_err_t       (*init)         (vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr);
    fsm_rt_t        (*enable)       (vsf_usart_t *usart_ptr);
    fsm_rt_t        (*disable)      (vsf_usart_t *usart_ptr);
    void            (*irq_enable)   (vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask);
    void            (*irq_disable)  (vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask);
    usart_status_t  (*status)       (vsf_usart_t *usart_ptr);
    uint_fast16_t   (*fifo_read)    (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);
    uint_fast16_t   (*fifo_write)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count);
    bool            (*fifo_flush)   (vsf_usart_t *usart_ptr);
    vsf_err_t       (*request_rx)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
    vsf_err_t       (*request_tx)   (vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count);
    vsf_err_t       (*cancel_rx)    (vsf_usart_t *usart_ptr);
    vsf_err_t       (*cancel_tx)    (vsf_usart_t *usart_ptr);
    int_fast32_t    (*get_rx_count) (vsf_usart_t *usart_ptr);
    int_fast32_t    (*get_tx_count) (vsf_usart_t *usart_ptr);
} vsf_usart_op_t;
//! @}

typedef struct vsf_usart_t  {
        const vsf_usart_op_t * op;
} vsf_usart_t;
#endif

/* usart_capability_t should implement peripheral_capability_t */
typedef struct usart_capability_t usart_capability_t;

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

/*============================ GLOBAL VARIABLES ==============================*/
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
#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic pop
#endif

extern uint_fast16_t      vsf_usart_fifo_read(      vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast16_t count);
extern uint_fast16_t      vsf_usart_fifo_write(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast16_t count);
extern bool               vsf_usart_fifo_flush(     vsf_usart_t *usart_ptr);

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

#ifdef __cplusplus
}
#endif

#endif
