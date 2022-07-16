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

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "hal/driver/driver.h"
#include "fifo2req_usart.h"

/*============================ MACROS ========================================*/

#define __USART_IRQ_MASK_REQUEST      \
    (USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __vsf_usart_fifo2req_process(vsf_usart_fifo2req_item_t *item, vsf_usart_t *usart_ptr)
{
    if (item->count < item->max_count) {
        uint8_t *buffer = (uint8_t *)item->buffer;
        buffer += item->count;  // TODO: add usart more than 8-bits support

        VSF_HAL_ASSERT(item->fifo_fn != NULL);
        item->count += item->fifo_fn(usart_ptr, buffer, item->max_count - item->count);
    }

    return item->count >= item->max_count;
}

static void __vsf_usart_fifo2req_isr_handler(void * target_ptr,
                                             vsf_usart_t *usart_ptr,
                                             em_usart_irq_mask_t irq_mask)
{
    vsf_usart_fifo2req_t * request_ptr = (vsf_usart_fifo2req_t *)target_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->disable_irq_fn != NULL);

    em_usart_irq_mask_t current_irq_mask = irq_mask & ~(USART_IRQ_MASK_RX | USART_IRQ_MASK_TX);

    if (irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_RX_TIMEOUT)) {
        if (__vsf_usart_fifo2req_process(&request_ptr->rx, usart_ptr)) {
            request_ptr->disable_irq_fn(usart_ptr, USART_IRQ_MASK_RX);
            current_irq_mask |= USART_IRQ_MASK_RX_CPL;
        }
    }

    if (irq_mask & USART_IRQ_MASK_TX) {
        if (__vsf_usart_fifo2req_process(&request_ptr->tx, usart_ptr)) {
            request_ptr->disable_irq_fn(usart_ptr, USART_IRQ_MASK_TX);
            current_irq_mask |= USART_IRQ_MASK_TX_CPL;
        }
    }

    if (current_irq_mask & USART_IRQ_MASK) {
        vsf_usart_isr_t *isr_ptr = &request_ptr->isr;
        if (isr_ptr->handler_fn != NULL) {
            isr_ptr->handler_fn(isr_ptr->target_ptr, usart_ptr, current_irq_mask);
        }
    }
}

static void __vsf_usart_fifo2req_isr_init(vsf_usart_fifo2req_item_t *request_item_ptr,
                                          void * buffer,
                                          uint_fast32_t max_count)
{
    VSF_HAL_ASSERT(request_item_ptr != NULL);

    request_item_ptr->buffer    = buffer;
    request_item_ptr->max_count = max_count;
    request_item_ptr->count     = 0;
}

vsf_err_t vsf_usart_fifo2req_init(vsf_usart_fifo2req_t *request_ptr,
                                  vsf_usart_fifo2req_init_fn_t  * init_fn,
                                  vsf_usart_fifo2req_irq_fn_t   * enable_irq_fn,
                                  vsf_usart_fifo2req_irq_fn_t   * disable_irq_fn,
                                  vsf_usart_fifo2req_fifo_fn_t  * rxfifo_fn,
                                  vsf_usart_fifo2req_fifo_fn_t  * txfifo_fn,
                                  vsf_usart_t *usart_ptr,
                                  usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(init_fn != NULL);
    VSF_HAL_ASSERT(enable_irq_fn != NULL);
    VSF_HAL_ASSERT(disable_irq_fn != NULL);
    VSF_HAL_ASSERT(rxfifo_fn != NULL);
    VSF_HAL_ASSERT(txfifo_fn != NULL);
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    usart_cfg_t request_cfg = *cfg_ptr;

    request_ptr->isr = request_cfg.isr;
    request_cfg.isr.handler_fn = __vsf_usart_fifo2req_isr_handler;
    request_cfg.isr.target_ptr = (void *)request_ptr;

    request_ptr->enable_irq_fn = enable_irq_fn;
    request_ptr->disable_irq_fn = disable_irq_fn;
    request_ptr->rx.fifo_fn = rxfifo_fn;
    request_ptr->tx.fifo_fn = txfifo_fn;

    return init_fn(usart_ptr, &request_cfg);
}

vsf_err_t vsf_usart_fifo2req_request_rx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    __vsf_usart_fifo2req_isr_init(&request_ptr->rx, buffer_ptr, count);
    request_ptr->enable_irq_fn(usart_ptr, USART_IRQ_MASK_RX);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_fifo2req_cancel_rx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr)
{
    request_ptr->disable_irq_fn(usart_ptr, USART_IRQ_MASK_RX);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_usart_fifo2req_get_rx_count(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(request_ptr != NULL);
    return request_ptr->rx.count;
}

vsf_err_t vsf_usart_fifo2req_request_tx(vsf_usart_fifo2req_t *request_ptr,
                                        vsf_usart_t *usart_ptr,
                                        void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    __vsf_usart_fifo2req_isr_init(&request_ptr->tx, buffer_ptr, count);
    __vsf_usart_fifo2req_isr_handler(request_ptr, usart_ptr, USART_IRQ_MASK_TX);
    request_ptr->enable_irq_fn(usart_ptr, USART_IRQ_MASK_TX);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_fifo2req_cancel_tx(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr)
{
    request_ptr->disable_irq_fn(usart_ptr, USART_IRQ_MASK_TX);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_usart_fifo2req_get_tx_count(vsf_usart_fifo2req_t *request_ptr, vsf_usart_t *usart_ptr)
{
    VSF_HAL_ASSERT(request_ptr != NULL);

    return request_ptr->tx.count;
}

void vsf_usart_fifo2req_irq_enable(vsf_usart_fifo2req_t *request_ptr,
                                   vsf_usart_t *usart_ptr,
                                   em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr != NULL);

    VSF_HAL_ASSERT((irq_mask & ~USART_IRQ_MASK) == 0);
    VSF_HAL_ASSERT((irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)) == 0);

    em_usart_irq_mask_t request_irq_mask = irq_mask & (USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
    em_usart_irq_mask_t others_irq_mask = irq_mask & ~(USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);

    request_ptr->irq_mask |= request_irq_mask;
    if (others_irq_mask) {
        VSF_HAL_ASSERT(request_ptr->enable_irq_fn != NULL);
        request_ptr->enable_irq_fn(usart_ptr, others_irq_mask);
    }
}

void vsf_usart_fifo2req_irq_disable(vsf_usart_fifo2req_t *request_ptr,
                                    vsf_usart_t *usart_ptr,
                                    em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(request_ptr != NULL);

    VSF_HAL_ASSERT((irq_mask & ~USART_IRQ_MASK) == 0);
    VSF_HAL_ASSERT((irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)) == 0);

    em_usart_irq_mask_t request_irq_mask = irq_mask & (USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);
    em_usart_irq_mask_t others_irq_mask = irq_mask & ~~(USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL);

    request_ptr->irq_mask &= ~request_irq_mask;
    if (others_irq_mask) {
        VSF_HAL_ASSERT(request_ptr->disable_irq_fn != NULL);
        request_ptr->disable_irq_fn(usart_ptr, others_irq_mask);
    }
}

#endif      // VSF_USART_CFG_FIFO_TO_REQUEST && VSF_HAL_USE_USART
