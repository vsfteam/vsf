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

// force convert vsf_usart_init to vsf_usart_init
#define __VSF_HAL_FIFO2REQ_USART_CLASS_IMPLEMENT    ENABLED

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_USART == ENABLED) && (VSF_HAL_USE_FIFO2REQ_USART == ENABLED)

#include "hal/driver/driver.h"

/*============================ MACROS ========================================*/

#ifdef VSF_FIFO2REQ_USART_CFG_CALL_PREFIX
#   define VSF_USART_CFG_PREFIX                 VSF_FIFO2REQ_USART_CFG_CALL_PREFIX
#endif

#define __USART_IRQ_MASK_REQUEST      \
    (USART_IRQ_MASK_RX_CPL | USART_IRQ_MASK_TX_CPL)

/*============================ MACROFIED FUNCTIONS ===========================*/


#ifndef VSF_FIFO2REQ_USART_CFG_PROTECT_LEVEL
#   define VSF_FIFO2REQ_USART_CFG_PROTECT_LEVEL interrupt
#endif

#ifndef __vsf_fifo2req_usart_protect
#   define __vsf_fifo2req_usart_protect         vsf_protect(VSF_FIFO2REQ_USART_CFG_PROTECT_LEVEL)
#endif

#ifndef __vsf_fifo2req_usart_unprotect
#   define __vsf_fifo2req_usart_unprotect       vsf_unprotect(VSF_FIFO2REQ_USART_CFG_PROTECT_LEVEL)
#endif

/*============================ TYPES =========================================*/

typedef uint_fast16_t fifo_handler_t(vsf_usart_t *usart_ptr,
                                     void *buffer_ptr,
                                     uint_fast16_t count);

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

static uint8_t __vsf_usart_get_data_length(vsf_usart_t * usart_ptr)
{
    // TODO: add usart capability support
    return 1;
}

static bool __vsf_fifo2req_usart_process(vsf_usart_t *usart_ptr,
                                        vsf_fifo2req_usart_item_t *item,
                                        fifo_handler_t *fifo_handler)
{
    if (item->count < item->max_count) {
        uint8_t *buffer = (uint8_t *)item->buffer;
        buffer += __vsf_usart_get_data_length(usart_ptr) * item->count;
        item->count += fifo_handler(usart_ptr, buffer, item->max_count - item->count);
    }

    return item->count >= item->max_count;
}

static void __vsf_fifo2req_usart_isr_handler(void *target,
                                            vsf_usart_t *usart_ptr,
                                            em_usart_irq_mask_t irq_mask)
{
    // One interrupt handles multipile interrupt sources
    vsf_fifo2req_usart_t * request_ptr = (vsf_fifo2req_usart_t *)target;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr == usart_ptr);

    em_usart_irq_mask_t current_irq_mask = irq_mask & ~(USART_IRQ_MASK_RX | USART_IRQ_MASK_TX);

    if (irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_RX_TIMEOUT)) {
        if (__vsf_fifo2req_usart_process(usart_ptr, &request_ptr->rx, vsf_usart_fifo_read)) {
            vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_RX);
            current_irq_mask |= USART_IRQ_MASK_RX_CPL;
        }
    }

    if (irq_mask & USART_IRQ_MASK_TX) {
        if (__vsf_fifo2req_usart_process(usart_ptr, &request_ptr->tx, vsf_usart_fifo_write)) {
            vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK_TX);
            current_irq_mask |= USART_IRQ_MASK_TX_CPL;
        }
    }

    if (current_irq_mask & USART_IRQ_MASK) {
        vsf_usart_isr_t *isr_ptr = &request_ptr->isr;
        if (isr_ptr->handler_fn != NULL) {
            isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)request_ptr, current_irq_mask);
        }
    }
}

static void __vsf_fifo2req_usart_isr_init(vsf_fifo2req_usart_item_t *request_item_ptr,
                                         void * buffer,
                                         uint_fast32_t max_count)
{
    VSF_HAL_ASSERT(request_item_ptr != NULL);

    request_item_ptr->buffer    = buffer;
    request_item_ptr->max_count = max_count;
    request_item_ptr->count     = 0;
}

vsf_err_t vsf_fifo2req_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);
    usart_cfg_t request_cfg = *cfg_ptr;

    request_ptr->isr = request_cfg.isr;
    request_cfg.isr.handler_fn = __vsf_fifo2req_usart_isr_handler;
    request_cfg.isr.target_ptr = (void *)request_ptr;

    return vsf_usart_init(request_ptr->real_usart_ptr, &request_cfg);
}

fsm_rt_t vsf_fifo2req_usart_enable(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    return vsf_usart_enable(request_ptr->real_usart_ptr);
}

fsm_rt_t vsf_fifo2req_usart_disable(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    return vsf_usart_disable(request_ptr->real_usart_ptr);
}

usart_status_t vsf_fifo2req_usart_status(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    return vsf_usart_status(request_ptr->real_usart_ptr);
}

uint_fast16_t vsf_fifo2req_usart_fifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    return vsf_usart_fifo_read(request_ptr->real_usart_ptr, buffer_ptr, count);
}

uint_fast16_t vsf_fifo2req_usart_fifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    return vsf_usart_fifo_write(request_ptr->real_usart_ptr, buffer_ptr, count);
}

vsf_err_t  vsf_fifo2req_usart_request_rx(vsf_usart_t *usart_ptr,
                                         void *buffer_ptr,
                                         uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    __vsf_fifo2req_usart_isr_init(&request_ptr->rx, buffer_ptr, count);
    vsf_usart_irq_enable(request_ptr->real_usart_ptr, USART_IRQ_MASK_RX);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_usart_cancel_rx(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    vsf_usart_irq_disable(request_ptr->real_usart_ptr, USART_IRQ_MASK_RX);

    return VSF_ERR_NONE;
}

int_fast32_t vsf_fifo2req_usart_get_rx_count(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);

    return request_ptr->rx.count;
}

vsf_err_t vsf_fifo2req_usart_request_tx(vsf_usart_t *usart_ptr,
                                        void *buffer_ptr,
                                        uint_fast32_t count)
{
    VSF_HAL_ASSERT(usart_ptr != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);


    __vsf_fifo2req_usart_isr_init(&request_ptr->tx, buffer_ptr, count);

    vsf_usart_irq_enable(request_ptr->real_usart_ptr, USART_IRQ_MASK_TX);

    vsf_protect_t orig = __vsf_fifo2req_usart_protect();
    __vsf_fifo2req_usart_isr_handler((void *)request_ptr, request_ptr->real_usart_ptr, USART_IRQ_MASK_TX);
    __vsf_fifo2req_usart_unprotect(orig);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_usart_cancel_tx(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    vsf_usart_irq_disable(request_ptr->real_usart_ptr, USART_IRQ_MASK_TX);

    return VSF_ERR_NONE;
}

int_fast32_t vsf_fifo2req_usart_get_tx_count(vsf_usart_t *usart_ptr)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);

    return request_ptr->tx.count;
}

void vsf_fifo2req_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    VSF_HAL_ASSERT((irq_mask & ~USART_IRQ_MASK) == 0);
    VSF_HAL_ASSERT((irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)) == 0);

    em_usart_irq_mask_t request_irq_mask = irq_mask & __USART_IRQ_MASK_REQUEST;
    em_usart_irq_mask_t others_irq_mask = irq_mask & ~__USART_IRQ_MASK_REQUEST;

    request_ptr->irq_mask |= request_irq_mask;
    if (others_irq_mask) {
        vsf_usart_irq_enable(request_ptr->real_usart_ptr, others_irq_mask);
    }
}

void vsf_fifo2req_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    vsf_fifo2req_usart_t *request_ptr = (vsf_fifo2req_usart_t *)usart_ptr;
    VSF_HAL_ASSERT(request_ptr != NULL);
    VSF_HAL_ASSERT(request_ptr->real_usart_ptr != NULL);

    VSF_HAL_ASSERT((irq_mask & ~USART_IRQ_MASK) == 0);
    VSF_HAL_ASSERT((irq_mask & (USART_IRQ_MASK_RX | USART_IRQ_MASK_TX)) == 0);

    em_usart_irq_mask_t request_irq_mask = irq_mask & __USART_IRQ_MASK_REQUEST;
    em_usart_irq_mask_t others_irq_mask = irq_mask & ~__USART_IRQ_MASK_REQUEST;

    request_ptr->irq_mask &= ~request_irq_mask;
    if (others_irq_mask) {
        vsf_usart_irq_disable(request_ptr->real_usart_ptr, others_irq_mask);
    }
}

#endif      // VSF_USART_CFG_FIFO_TO_REQUEST && VSF_HAL_USE_USART
