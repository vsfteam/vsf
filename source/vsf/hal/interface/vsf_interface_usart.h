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
#include "hal/vsf_hal_cfg.h"
#include "vsf_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

/********************************VSF_USART_FUNC_BODY*****************************/
#define ____VSF_USART_LV1_INTTERFACE_BODY(__N, __VALUE)                         \
                                                                                \
/*usart_init*/                                                                  \
static                                                                          \
vsf_err_t vsf_usart##__N##_init(usart_cfg_t *usart_cfg)                         \
{                                                                               \
    return vsf_usart_init(&vsf_usart##__N, usart_cfg);                          \
}                                                                               \
                                                                                \
/*usart_enable*/                                                                \
static                                                                          \
fsm_rt_t vsf_usart##__N##_enable(void)                                          \
{                                                                               \
    return vsf_usart_enable(&vsf_usart##__N);                                   \
}                                                                               \
                                                                                \
/*usart_disable*/                                                               \
static                                                                          \
fsm_rt_t vsf_usart##__N##_disable(void)                                         \
{                                                                               \
    return vsf_usart_disable(&vsf_usart##__N);                                  \
}                                                                               \
                                                                                \
/*usart_status*/                                                                \
static                                                                          \
usart_status_t vsf_usart##__N##_status(void)                                    \
{                                                                               \
    return vsf_usart_status(&vsf_usart##__N);                                   \
}                                                                               \
                                                                                \
/*usart_fifo_read*/                                                             \
static                                                                          \
uint_fast16_t vsf_usart##__N##_fifo_read(void *buffer_ptr, uint_fast16_t count) \
{                                                                               \
    return vsf_usart_fifo_read(&vsf_usart##__N, buffer_ptr, count);             \
}                                                                               \
                                                                                \
/*usart_fifo_write*/                                                            \
static                                                                          \
uint_fast16_t vsf_usart##__N##_fifo_write(void *buffer_ptr, uint_fast16_t count)\
{                                                                               \
    return vsf_usart_fifo_write(&vsf_usart##__N, buffer_ptr, count);            \
}                                                                               \
                                                                                \
/*usart_fifo_flush*/                                                            \
static                                                                          \
bool vsf_usart##__N##_fifo_flush(void)                                          \
{                                                                               \
    return vsf_usart_fifo_flush(&vsf_usart##__N);                               \
}                                                                               \
                                                                                \
/*usart_request_read*/                                                          \
static                                                                          \
fsm_rt_t vsf_usart##__N##_request_read(uint8_t *buffer_ptr, uint_fast32_t count)\
{                                                                               \
    return vsf_usart_request_read(&vsf_usart##__N, buffer_ptr, count);          \
}                                                                               \
                                                                                \
/*usart_request_write*/                                                         \
static                                                                          \
fsm_rt_t vsf_usart##__N##_request_write(uint8_t *buffer_ptr, uint_fast32_t count)\
{                                                                               \
    return vsf_usart_request_write(&vsf_usart##__N, buffer_ptr, count);         \
}                                                                               \
                                                                                \
/*usart_irq_enable*/                                                            \
static                                                                          \
void vsf_usart##__N##_irq_enable(em_usart_irq_mask_t irq_mask)                  \
{                                                                               \
    vsf_usart_irq_enable(&vsf_usart##__N, irq_mask);                            \
}                                                                               \
                                                                                \
/*usart_evt_enbale*/                                                            \
static                                                                          \
void vsf_usart##__N##_irq_disable(em_usart_irq_mask_t irq_mask)                 \
{                                                                               \
    vsf_usart_irq_disable(&vsf_usart##__N, irq_mask);                           \
}

#define __VSF_USART_LV1_INTTERFACE_BODY(__N, __VALUE)                           \
            ____VSF_USART_LV1_INTTERFACE_BODY(__N, __VALUE)                     \

/********************************VSF_USART_FUNC_BODY*****************************/
#define ____VSF_USART_LV1_INTERFACE_INIT(__N, __VALUE)                          \
    {                                                                           \
        .Init               = &vsf_usart##__N##_init,                           \
        .Enable             = &vsf_usart##__N##_enable,                         \
        .Disable            = &vsf_usart##__N##_disable,                        \
        .Status             = (peripheral_status_t (*)(void))vsf_usart##__N##_status,\
        .Irq = {                                                                \
            .Enable         = &vsf_usart##__N##_irq_enable,                     \
            .Disable        = &vsf_usart##__N##_irq_disable,                    \
        },                                                                      \
        .FIFO = {                                                               \
            .Read           = &vsf_usart##__N##_fifo_read,                      \
            .Write          = &vsf_usart##__N##_fifo_write,                     \
            .Flush          = &vsf_usart##__N##_fifo_flush,                     \
        },                                                                      \
        .Block = {                                                              \
            .Read.Request  = &vsf_usart##__N##_request_read,                    \
            .Write.Request = &vsf_usart##__N##_request_write,                   \
        },                                                                      \
    }

/*! \note __VSF_USART_LV1_INTERFACE_INIT is designated to be used with
 *!       MACRO_REPEATE, a "," is attached to ____VSF_USART_LV1_INTERFACE_INIT
 */
#define __VSF_USART_LV1_INTERFACE_INIT(__N, __VALUE)                            \
            ____VSF_USART_LV1_INTERFACE_INIT(__N, __VALUE),


#define __VSF_USART_LV1_IMPL(__USART_INDEX)                                     \
                                                                                \
____VSF_USART_LV1_INTTERFACE_BODY(__USART_INDEX, NULL)                          \
                                                                                \
const i_usart_t CONNECT(VSF_USART, __USART_INDEX) =                             \
        ____VSF_USART_LV1_INTERFACE_INIT(__USART_INDEX, NULL);

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

typedef void vsf_usart_isr_handler_t(   void *target_ptr,
                                        vsf_usart_t *usart_ptr,
                                        em_usart_irq_mask_t irq_mask);

typedef struct vsf_usart_isr_t {
    vsf_usart_isr_handler_t *handler_fn;
    void                    *target_ptr;
    vsf_arch_prio_t         prio;
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

extern usart_status_t     vsf_usart_status(         vsf_usart_t *usart_ptr);

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
