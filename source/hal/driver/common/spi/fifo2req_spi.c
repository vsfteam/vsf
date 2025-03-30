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

#define __VSF_HAL_USE_FIFO2REQ_SPI_CLASS_IMPLEMENT  ENABLED
#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_SPI == ENABLED) && (VSF_HAL_USE_FIFO2REQ_SPI == ENABLED)

#include "hal/driver/driver.h"
#include "./fifo2req_spi.h"

/*============================ MACROS ========================================*/

#ifdef VSF_FIFO2REQ_SPI_CFG_CALL_PREFIX
#   undef VSF_SPI_CFG_PREFIX
#   define VSF_SPI_CFG_PREFIX                       VSF_FIFO2REQ_SPI_CFG_CALL_PREFIX
#endif

#define VSF_SPI_IMP_DEC_EXTERN_OP                   ENABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_fifo2req_spi_isr_handler(void * target_ptr, vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    vsf_fifo2req_spi_t * fifo2req_spi_ptr = (vsf_fifo2req_spi_t *)target_ptr;
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    vsf_spi_irq_mask_t current_irq_mask = (irq_mask & ~(VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_TX));

    if (irq_mask & VSF_SPI_IRQ_MASK_RX) {
        irq_mask &= ~VSF_SPI_IRQ_MASK_RX;
        if (fifo2req_spi_ptr->in.offset < fifo2req_spi_ptr->in.cnt) {
            vsf_spi_fifo_transfer(fifo2req_spi_ptr->spi,
                                  NULL, NULL,
                                  fifo2req_spi_ptr->in.buffer_ptr, &fifo2req_spi_ptr->in.offset,
                                  fifo2req_spi_ptr->in.cnt);
        }

        if (fifo2req_spi_ptr->in.offset >= fifo2req_spi_ptr->in.cnt) {
            vsf_spi_irq_disable(fifo2req_spi_ptr->spi, VSF_SPI_IRQ_MASK_RX);
            memset(&fifo2req_spi_ptr->in, 0, sizeof(fifo2req_spi_ptr->in));
            current_irq_mask |= VSF_SPI_IRQ_MASK_CPL;
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_TX) {
        irq_mask &= ~VSF_SPI_IRQ_MASK_TX;
        if (fifo2req_spi_ptr->out.offset < fifo2req_spi_ptr->out.cnt) {
            vsf_spi_fifo_transfer(fifo2req_spi_ptr->spi,
                                  fifo2req_spi_ptr->out.buffer_ptr, &fifo2req_spi_ptr->out.offset,
                                  NULL, NULL,
                                  fifo2req_spi_ptr->out.cnt);
        }

        if (fifo2req_spi_ptr->out.offset >= fifo2req_spi_ptr->out.cnt) {
            vsf_spi_irq_disable(fifo2req_spi_ptr->spi, VSF_SPI_IRQ_MASK_TX);
            memset(&fifo2req_spi_ptr->out, 0, sizeof(fifo2req_spi_ptr->out));
            current_irq_mask |= VSF_SPI_IRQ_MASK_TX_CPL;
        }
    }

    current_irq_mask &= fifo2req_spi_ptr->irq_mask;
    if (current_irq_mask & VSF_SPI_IRQ_ALL_BITS_MASK) {
        vsf_spi_isr_t *isr_ptr = &fifo2req_spi_ptr->isr;
        if (isr_ptr->handler_fn != NULL) {
            isr_ptr->handler_fn(isr_ptr->target_ptr, (void *)fifo2req_spi_ptr, current_irq_mask);
        }
    }
}

vsf_err_t vsf_fifo2req_spi_init(vsf_fifo2req_spi_t *fifo2req_spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    vsf_spi_cfg_t request_cfg = *cfg_ptr;

    fifo2req_spi_ptr->isr = request_cfg.isr;
    request_cfg.isr.handler_fn = __vsf_fifo2req_spi_isr_handler;
    request_cfg.isr.target_ptr = (void *)fifo2req_spi_ptr;

    return vsf_spi_init(fifo2req_spi_ptr->spi, &request_cfg);
}

void vsf_fifo2req_spi_fini(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    vsf_spi_fini(fifo2req_spi_ptr->spi);
}

fsm_rt_t vsf_fifo2req_spi_enable(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_enable(fifo2req_spi_ptr->spi);
}

fsm_rt_t vsf_fifo2req_spi_disable(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_disable(fifo2req_spi_ptr->spi);
}

void vsf_fifo2req_spi_irq_enable(vsf_fifo2req_spi_t *fifo2req_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);
    VSF_HAL_ASSERT((irq_mask & ~VSF_SPI_IRQ_ALL_BITS_MASK) == 0);
    VSF_HAL_ASSERT(((irq_mask & VSF_SPI_IRQ_MASK_RX) == 0) || ((fifo2req_spi_ptr->irq_mask & VSF_SPI_IRQ_MASK_CPL) == 0));
    VSF_HAL_ASSERT(((irq_mask & VSF_SPI_IRQ_MASK_TX) == 0) || ((fifo2req_spi_ptr->irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) == 0));

    vsf_spi_irq_mask_t request_irq_mask = irq_mask & (VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_TX_CPL);
    vsf_spi_irq_mask_t others_irq_mask = irq_mask & ~(VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_TX_CPL);

    fifo2req_spi_ptr->irq_mask |= request_irq_mask;
    if (others_irq_mask) {
        vsf_spi_irq_enable(fifo2req_spi_ptr->spi, others_irq_mask);
    }
}

void vsf_fifo2req_spi_irq_disable(vsf_fifo2req_spi_t *fifo2req_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT((irq_mask & ~VSF_SPI_IRQ_ALL_BITS_MASK) == 0);

    vsf_spi_irq_mask_t request_irq_mask = irq_mask & (VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_TX_CPL);
    vsf_spi_irq_mask_t others_irq_mask = irq_mask & ~(VSF_SPI_IRQ_MASK_CPL | VSF_SPI_IRQ_MASK_TX_CPL);

    fifo2req_spi_ptr->irq_mask &= ~request_irq_mask;
    if (others_irq_mask) {
        vsf_spi_irq_disable(fifo2req_spi_ptr->spi, irq_mask);
    }
}

vsf_spi_status_t vsf_fifo2req_spi_status(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_status(fifo2req_spi_ptr->spi);
}

vsf_err_t vsf_fifo2req_spi_cs_active(vsf_fifo2req_spi_t *fifo2req_spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_cs_active(fifo2req_spi_ptr->spi, index);
}

vsf_err_t vsf_fifo2req_spi_cs_inactive(vsf_fifo2req_spi_t *fifo2req_spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_cs_inactive(fifo2req_spi_ptr->spi, index);
}

void vsf_fifo2req_spi_fifo_transfer(vsf_fifo2req_spi_t *fifo2req_spi_ptr,
                                    void               *out_buffer_ptr,
                                    uint_fast32_t      *out_offset_ptr,
                                    void               *in_buffer_ptr,
                                    uint_fast32_t      *in_offset_ptr,
                                    uint_fast32_t       count)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    // TODO
    VSF_HAL_ASSERT(0);
}

uint_fast16_t vsf_fifo2req_spi_slave_fifo_transfer(vsf_fifo2req_spi_t *fifo2req_spi_ptr,
                                                   bool transmit_or_receive,
                                                   uint_fast16_t count,
                                                   uint8_t *buffer_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    // TODO
    VSF_HAL_ASSERT(0);
    return 0;
}

vsf_err_t vsf_fifo2req_spi_request_transfer(vsf_fifo2req_spi_t *fifo2req_spi_ptr, void *out_buffer_ptr,
                                   void *in_buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);
    VSF_HAL_ASSERT(count != 0);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->in.cnt == 0);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->out.cnt == 0);

    fifo2req_spi_ptr->in.cnt = count;
    fifo2req_spi_ptr->in.offset = 0;
    fifo2req_spi_ptr->in.buffer_ptr = in_buffer_ptr;
    fifo2req_spi_ptr->out.cnt = count;
    fifo2req_spi_ptr->out.offset = 0;
    fifo2req_spi_ptr->out.buffer_ptr = out_buffer_ptr;

    __vsf_fifo2req_spi_isr_handler((void *)fifo2req_spi_ptr, NULL, VSF_SPI_IRQ_MASK_TX);

    vsf_spi_irq_enable(fifo2req_spi_ptr->spi, VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_TX);
    return VSF_ERR_NONE;
}

vsf_err_t vsf_fifo2req_spi_cancel_transfer(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    vsf_spi_irq_disable(fifo2req_spi_ptr->spi, VSF_SPI_IRQ_MASK_RX | VSF_SPI_IRQ_MASK_TX);

    return VSF_ERR_NONE;
}

void vsf_fifo2req_spi_get_transferred_count(vsf_fifo2req_spi_t *fifo2req_spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    if (tx_count != NULL) {
        *tx_count = fifo2req_spi_ptr->out.offset;
    }

    if (rx_count != NULL) {
        *rx_count = fifo2req_spi_ptr->in.offset;
    }
}

vsf_err_t vsf_fifo2req_spi_ctrl(vsf_fifo2req_spi_t *fifo2req_spi_ptr, vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_ctrl(fifo2req_spi_ptr->spi, ctrl, param);
}

vsf_spi_capability_t vsf_fifo2req_spi_capability(vsf_fifo2req_spi_t *fifo2req_spi_ptr)
{
    VSF_HAL_ASSERT(fifo2req_spi_ptr != NULL);
    VSF_HAL_ASSERT(fifo2req_spi_ptr->spi != NULL);

    return vsf_spi_capability(fifo2req_spi_ptr->spi);
}

/*============================ LOCAL VARIABLES ===============================*/

#define VSF_SPI_CFG_IMP_PREFIX                      vsf_fifo2req
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX               VSF_FIFO2REQ
#define VSF_SPI_CFG_IMP_EXTERN_OP                   ENABLED
#define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#include "hal/driver/common/spi/spi_template.inc"


#endif      // VSF_SPI_CFG_FIFO_TO_REQUEST && VSF_HAL_USE_SPI
