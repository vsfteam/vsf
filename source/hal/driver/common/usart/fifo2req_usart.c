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

#define __VSF_HAL_USE_FIFO2REQ_USART_CLASS_IMPLEMENT  ENABLED
#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "hal/driver/driver.h"
#include "fifo2req_usart.h"

/*============================ MACROS ========================================*/

#ifdef VSF_FIFO2REQ_USART_CFG_CALL_PREFIX
#   undef VSF_USART_CFG_IMP_PREFIX
#   define VSF_USART_CFG_IMP_PREFIX     VSF_FIFO2REQ_USART_CFG_CALL_PREFIX
#endif

#define VSF_USART_IMP_DEC_EXTERN_OP     ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint_fast16_t vsf_fifo2req_usart_fifo_fn_t(vsf_usart_t *usart_ptr,
                                                   void *buffer_ptr,
                                                   uint_fast16_t count);

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

static bool __vsf_fifo2req_usart_process(vsf_fifo2req_usart_item_t *item, vsf_usart_t *usart_ptr, vsf_fifo2req_usart_fifo_fn_t *fifo_fn)
{
    if (item->count < item->max_count) {
        uint8_t *buffer = (uint8_t *)item->buffer;
        buffer += item->count;  // TODO: add usart more than 8-bits support

        VSF_HAL_ASSERT(fifo_fn != NULL);
        item->count += fifo_fn(usart_ptr, buffer, item->max_count - item->count);
    }

    return item->count >= item->max_count;
}

static void __vsf_fifo2req_usart_isr_handler(void * target_ptr,
                                             vsf_usart_t *usart_ptr,
                                             vsf_usart_irq_mask_t irq_mask)
{
    vsf_fifo2req_usart_t * fifo2req_ptr = (vsf_fifo2req_usart_t *)target_ptr;
    VSF_HAL_ASSERT(fifo2req_ptr != NULL);
    VSF_HAL_ASSERT(usart_ptr != NULL);

    vsf_usart_irq_mask_t current_irq_mask = irq_mask & ~(VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX);

    if (irq_mask & VSF_USART_IRQ_MASK_RX) {
        if (fifo2req_ptr->irq_mask & VSF_USART_IRQ_MASK_RX_CPL) {
            if (__vsf_fifo2req_usart_process(&fifo2req_ptr->rx, usart_ptr, vsf_usart_rxfifo_read)) {
                vsf_usart_irq_disable(usart_ptr, VSF_USART_IRQ_MASK_RX);
                current_irq_mask |= VSF_USART_IRQ_MASK_RX_CPL;
            }
        } else {
            current_irq_mask |= VSF_USART_IRQ_MASK_RX;
        }
    }

    if (irq_mask & VSF_USART_IRQ_MASK_TX) {
        if (fifo2req_ptr->irq_mask & VSF_USART_IRQ_MASK_TX_CPL) {
            if (__vsf_fifo2req_usart_process(&fifo2req_ptr->tx, usart_ptr, vsf_usart_txfifo_write)) {
                vsf_usart_irq_disable(usart_ptr, VSF_USART_IRQ_MASK_TX);
                current_irq_mask |= VSF_USART_IRQ_MASK_TX_CPL;
            }
        } else {
            current_irq_mask |= VSF_USART_IRQ_MASK_TX;
        }
    }

    if (current_irq_mask & VSF_USART_IRQ_ALL_BITS_MASK) {
        vsf_usart_isr_t *isr_ptr = &fifo2req_ptr->isr;
        if (isr_ptr->handler_fn != NULL) {
            isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)fifo2req_ptr, current_irq_mask);
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

vsf_err_t vsf_fifo2req_usart_init(vsf_fifo2req_usart_t * fifo2req_usart_ptr,
                                  vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    vsf_usart_cfg_t request_cfg = *cfg_ptr;

    fifo2req_usart_ptr->isr = request_cfg.isr;
    request_cfg.isr.handler_fn = __vsf_fifo2req_usart_isr_handler;
    request_cfg.isr.target_ptr = (void *)fifo2req_usart_ptr;

    vsf_err_t err = vsf_usart_init(fifo2req_usart_ptr->usart, &request_cfg);
    if (err == VSF_ERR_NONE) {
        vsf_usart_capability_t capability = vsf_usart_capability(fifo2req_usart_ptr->usart);
        vsf_usart_irq_mask_t irq_mask = capability.irq_mask & (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX);
        if (irq_mask != (VSF_USART_IRQ_MASK_RX | VSF_USART_IRQ_MASK_TX)) {
            return VSF_ERR_NOT_SUPPORT;
        }
    }
    return err;
}

void vsf_fifo2req_usart_fini(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    vsf_usart_fini(fifo2req_usart_ptr->usart);
}


fsm_rt_t vsf_fifo2req_usart_enable(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_enable(fifo2req_usart_ptr->usart);
}


fsm_rt_t vsf_fifo2req_usart_disable(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_disable(fifo2req_usart_ptr->usart);
}

void vsf_fifo2req_usart_irq_enable(vsf_fifo2req_usart_t *fifo2req_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    VSF_HAL_ASSERT((irq_mask & ~VSF_USART_IRQ_ALL_BITS_MASK) == 0);
    VSF_HAL_ASSERT(((irq_mask & VSF_USART_IRQ_MASK_RX) == 0) || ((fifo2req_usart_ptr->irq_mask & VSF_USART_IRQ_MASK_RX_CPL) == 0));
    VSF_HAL_ASSERT(((irq_mask & VSF_USART_IRQ_MASK_TX) == 0) || ((fifo2req_usart_ptr->irq_mask & VSF_USART_IRQ_MASK_TX_CPL) == 0));

    vsf_usart_irq_mask_t request_irq_mask = irq_mask & (VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_usart_irq_mask_t others_irq_mask = irq_mask & ~(VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL);

    fifo2req_usart_ptr->irq_mask |= request_irq_mask;
    if (others_irq_mask) {
        vsf_usart_irq_enable(fifo2req_usart_ptr->usart, others_irq_mask);
    }
}

void vsf_fifo2req_usart_irq_disable(vsf_fifo2req_usart_t *fifo2req_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    VSF_HAL_ASSERT((irq_mask & ~VSF_USART_IRQ_ALL_BITS_MASK) == 0);

    vsf_usart_irq_mask_t request_irq_mask = irq_mask & (VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL);
    vsf_usart_irq_mask_t others_irq_mask = irq_mask & ~(VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL);

    fifo2req_usart_ptr->irq_mask &= ~request_irq_mask;
    if (others_irq_mask) {
        vsf_usart_irq_disable(fifo2req_usart_ptr->usart, others_irq_mask);
    }
}

vsf_usart_status_t vsf_fifo2req_usart_status(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_status(fifo2req_usart_ptr->usart);
}

vsf_usart_capability_t vsf_fifo2req_usart_capability(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    vsf_usart_capability_t capability = vsf_usart_capability(fifo2req_usart_ptr->usart);

    capability.irq_mask |= (VSF_USART_IRQ_MASK_RX_CPL | VSF_USART_IRQ_MASK_TX_CPL);

    return capability;
}

uint_fast16_t vsf_fifo2req_usart_rxfifo_get_data_count(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_rxfifo_get_data_count(fifo2req_usart_ptr->usart);
}

uint_fast16_t vsf_fifo2req_usart_rxfifo_read(vsf_fifo2req_usart_t *fifo2req_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_rxfifo_read(fifo2req_usart_ptr->usart, buffer_ptr, count);
}

uint_fast16_t vsf_fifo2req_usart_txfifo_get_free_count(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_txfifo_get_free_count(fifo2req_usart_ptr->usart);
}

uint_fast16_t vsf_fifo2req_usart_txfifo_write(vsf_fifo2req_usart_t *fifo2req_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_txfifo_write(fifo2req_usart_ptr->usart, buffer_ptr, count);
}

vsf_err_t vsf_fifo2req_usart_request_rx(vsf_fifo2req_usart_t *fifo2req_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

   __vsf_fifo2req_usart_isr_init(&fifo2req_usart_ptr->rx, buffer_ptr, count);
   vsf_usart_irq_enable(fifo2req_usart_ptr->usart, VSF_USART_IRQ_MASK_RX);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_usart_request_tx(vsf_fifo2req_usart_t *fifo2req_usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);
    VSF_HAL_ASSERT(buffer_ptr != NULL);
    VSF_HAL_ASSERT(count > 0);

    __vsf_fifo2req_usart_isr_init(&fifo2req_usart_ptr->tx, buffer_ptr, count);
    __vsf_fifo2req_usart_isr_handler(fifo2req_usart_ptr, fifo2req_usart_ptr->usart, VSF_USART_IRQ_MASK_TX);

    if (fifo2req_usart_ptr->tx.count < fifo2req_usart_ptr->tx.max_count) {
        vsf_usart_irq_enable(fifo2req_usart_ptr->usart, VSF_USART_IRQ_MASK_TX);
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_usart_cancel_rx(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    vsf_usart_irq_disable(fifo2req_usart_ptr->usart, VSF_USART_IRQ_MASK_RX);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_usart_cancel_tx(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    vsf_usart_irq_disable(fifo2req_usart_ptr->usart, VSF_USART_IRQ_MASK_TX);
    return VSF_ERR_NONE;
}

int_fast32_t vsf_fifo2req_usart_get_rx_count(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return fifo2req_usart_ptr->rx.count;
}

int_fast32_t vsf_fifo2req_usart_get_tx_count(vsf_fifo2req_usart_t *fifo2req_usart_ptr)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return fifo2req_usart_ptr->tx.count;
}

vsf_err_t vsf_fifo2req_usart_ctrl(vsf_fifo2req_usart_t *fifo2req_usart_ptr, vsf_usart_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(fifo2req_usart_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_usart_ptr->usart != NULL);

    return vsf_usart_ctrl(fifo2req_usart_ptr->usart, ctrl, param);
}

/*============================ LOCAL VARIABLES ===============================*/

#define VSF_USART_CFG_IMP_PREFIX                      vsf_fifo2req
#define VSF_USART_CFG_IMP_UPCASE_PREFIX               VSF_FIFO2REQ
#define VSF_USART_CFG_IMP_EXTERN_OP                   ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST         ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL            ENABLED
#include "hal/driver/common/usart/usart_template.inc"

#endif      // VSF_HAL_USE_USART
