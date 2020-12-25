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

#if VSF_HAL_USART_IMP_REQUEST_BY_FIFO == ENABLED
/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
// todo: custom isr handler
#ifndef VSF_HAL_USART_CUSTOM_ISR_HANDLER
#   define VSF_HAL_USART_CUSTOM_ISR_HANDLER  __usart_request_isr_handler
#endif

#undef vsf_usart_irq_enable
#undef vsf_usart_irq_disable
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __usart_request_write(vsf_usart_t *usart_ptr)
{
    __vsf_usart_req_t *tx_ptr = &usart_ptr->__req_by_fifo.tx;

    if (tx_ptr->count < tx_ptr->max_count) {
        uint8_t * buffer = (uint8_t *)tx_ptr->buffer;
#if VSF_USART_CFG_9_BIT == ENABLED
        buffer += __vsf_usart_get_word_length(usart_ptr) * tx_ptr->count;
#else
        buffer += tx_ptr->count;
#endif
        tx_ptr->count += vsf_usart_fifo_write(usart_ptr, buffer, tx_ptr->max_count - tx_ptr->count);
    }

    if (tx_ptr->count >= tx_ptr->max_count) {
        __vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_TX);

        vsf_usart_isr_t *req_isr_ptr = &usart_ptr->__req_by_fifo.isr;
        if (req_isr_ptr->handler_fn != NULL) {
            req_isr_ptr->handler_fn(req_isr_ptr->target_ptr, usart_ptr, USART_IRQ_MASK_TX_CPL);
        }
    }
}

static void __usart_request_read(vsf_usart_t *usart_ptr)
{
    __vsf_usart_req_t *rx_ptr = &usart_ptr->__req_by_fifo.rx;

    if (rx_ptr->count < rx_ptr->max_count) {
        uint8_t * buffer = (uint8_t *)rx_ptr->buffer;
#if VSF_USART_CFG_9_BIT == ENABLED
        buffer += __vsf_usart_get_word_length(usart_ptr) * rx_ptr->count;
#else
        buffer += rx_ptr->count;
#endif
        rx_ptr->count += vsf_usart_fifo_read(usart_ptr, buffer, rx_ptr->max_count - rx_ptr->count);
    }

    if (rx_ptr->count >= rx_ptr->max_count) {
        __vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_RX);

        vsf_usart_isr_t *req_isr_ptr = &usart_ptr->__req_by_fifo.isr;
        if (req_isr_ptr->handler_fn != NULL) {
            req_isr_ptr->handler_fn(req_isr_ptr->target_ptr, usart_ptr, USART_IRQ_MASK_RX_CPL);
        }
    }
}

static void __usart_request_isr_handler(void *target, vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_TX) {
        __usart_request_write(usart_ptr);
    }

    if (irq_mask & USART_IRQ_MASK_RX) {
        __usart_request_read(usart_ptr);
    }
}

static void __usart_req_isr_init(vsf_usart_t *usart_ptr)
{
    if ((usart_ptr->isr.handler_fn != NULL) && (usart_ptr->isr.handler_fn != __usart_request_isr_handler)) {
        usart_ptr->__req_by_fifo.isr = usart_ptr->isr;
        usart_ptr->isr.handler_fn = __usart_request_isr_handler;
    }
}

static void __usart_req_init(__vsf_usart_req_t *req_ptr, void *buffer, uint_fast32_t max_count)
{
    VSF_HAL_ASSERT(req_ptr != NULL);

    req_ptr->buffer = buffer;
    req_ptr->max_count = max_count;
    req_ptr->count = 0;
}


vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer, uint_fast32_t max_count)
{
    VSF_HAL_ASSERT((NULL != usart_ptr) && (NULL != buffer) && (max_count > 0));

    __usart_req_isr_init(usart_ptr);
    __usart_req_init(&usart_ptr->__req_by_fifo.rx, buffer, max_count);
    __vsf_usart_irq_enable(usart_ptr, USART_IRQ_MASK_RX);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer, uint_fast32_t max_count)
{
    VSF_HAL_ASSERT((NULL != usart_ptr) && (NULL != buffer) && (max_count > 0));

    __usart_req_isr_init(usart_ptr);
    __usart_req_init(&usart_ptr->__req_by_fifo.tx, buffer, max_count);
    __vsf_usart_irq_enable(usart_ptr, USART_IRQ_MASK_TX);

    // todo: use vsf_set_base_priority instead ?
    vsf_protect_t orig = __vsf_usart_protect();
    __usart_request_isr_handler(NULL, usart_ptr, USART_IRQ_MASK_TX);
    __vsf_usart_unprotect(orig);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    __vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_RX);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    __vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_TX);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return usart_ptr->__req_by_fifo.rx.count;
}

int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    return usart_ptr->__req_by_fifo.tx.count;
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    em_usart_irq_mask_t fifo_mask = 0;
    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
        fifo_mask |= USART_IRQ_MASK_RX;
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        fifo_mask |= USART_IRQ_MASK_TX;
    }
    __vsf_usart_irq_enable(usart_ptr, fifo_mask);
}

void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    em_usart_irq_mask_t fifo_mask = 0;
    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
        fifo_mask |= USART_IRQ_MASK_RX;
    }
    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
        fifo_mask |= USART_IRQ_MASK_TX;
    }
    if (irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)) {
        VSF_HAL_ASSERT(0);
    }
    __vsf_usart_irq_disable(usart_ptr, fifo_mask);
}

#endif

#if USART_MAX_PORT >= 0 && VSF_HAL_USE_USART0 == ENABLED && (USART_PORT_MASK & (1 << 0))
__vsf_hw_usart_imp_lv0(0, NULL)
#endif
#if USART_MAX_PORT >= 1 && VSF_HAL_USE_USART1 == ENABLED && (USART_PORT_MASK & (1 << 1))
__vsf_hw_usart_imp_lv0(1, NULL)
#endif
#if USART_MAX_PORT >= 2 && VSF_HAL_USE_USART2 == ENABLED && (USART_PORT_MASK & (1 << 2))
__vsf_hw_usart_imp_lv0(2, NULL)
#endif
#if USART_MAX_PORT >= 3 && VSF_HAL_USE_USART3 == ENABLED && (USART_PORT_MASK & (1 << 3))
__vsf_hw_usart_imp_lv0(3, NULL)
#endif
#if USART_MAX_PORT >= 4 && VSF_HAL_USE_USART4 == ENABLED && (USART_PORT_MASK & (1 << 4))
__vsf_hw_usart_imp_lv0(4, NULL)
#endif
#if USART_MAX_PORT >= 5 && VSF_HAL_USE_USART5 == ENABLED && (USART_PORT_MASK & (1 << 5))
__vsf_hw_usart_imp_lv0(5, NULL)
#endif
#if USART_MAX_PORT >= 6 && VSF_HAL_USE_USART6 == ENABLED && (USART_PORT_MASK & (1 << 6))
__vsf_hw_usart_imp_lv0(6, NULL)
#endif
#if USART_MAX_PORT >= 7 && VSF_HAL_USE_USART7 == ENABLED && (USART_PORT_MASK & (1 << 7))
__vsf_hw_usart_imp_lv0(7, NULL)
#endif

/*EOF*/