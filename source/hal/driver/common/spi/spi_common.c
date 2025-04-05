/*****************************************************************************
 *   Cop->right(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a cop-> of the License at                                  *
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

#define VSF_SPI_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_SPI == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

#   if VSF_SPI_CFG_MULTI_CLASS == ENABLED

vsf_err_t vsf_spi_init(vsf_spi_t *spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->init != NULL);

    return spi_ptr->op->init(spi_ptr, cfg_ptr);
}

void vsf_spi_fini(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->fini != NULL);

    spi_ptr->op->fini(spi_ptr);
}

fsm_rt_t vsf_spi_enable(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->enable != NULL);

    return spi_ptr->op->enable(spi_ptr);
}

fsm_rt_t vsf_spi_disable(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->disable != NULL);

    return spi_ptr->op->disable(spi_ptr);
}

void vsf_spi_irq_enable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->irq_enable != NULL);

    spi_ptr->op->irq_enable(spi_ptr, irq_mask);
}

void vsf_spi_irq_disable(vsf_spi_t *spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->irq_disable != NULL);

    spi_ptr->op->irq_disable(spi_ptr, irq_mask);
}

vsf_spi_status_t vsf_spi_status(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->status != NULL);

    return spi_ptr->op->status(spi_ptr);
}

vsf_err_t vsf_spi_cs_active(vsf_spi_t *spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->cs_active != NULL);

    return spi_ptr->op->cs_active(spi_ptr, index);
}

vsf_err_t vsf_spi_cs_inactive(vsf_spi_t *spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->cs_inactive != NULL);

    return spi_ptr->op->cs_inactive(spi_ptr, index);
}

void vsf_spi_fifo_transfer(vsf_spi_t *spi_ptr, void *out_buffer_ptr,
                           uint_fast32_t *out_offset_ptr, void *in_buffer_ptr,
                           uint_fast32_t *in_offset_ptr, uint_fast32_t cnt)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->fifo_transfer != NULL);

    spi_ptr->op->fifo_transfer(spi_ptr, out_buffer_ptr, out_offset_ptr,
                               in_buffer_ptr, in_offset_ptr, cnt);
}

vsf_err_t vsf_spi_request_transfer(vsf_spi_t *spi_ptr, void *out_buffer_ptr,
                                   void *in_buffer_ptr, uint_fast32_t count)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->request_transfer != NULL);

    return spi_ptr->op->request_transfer(spi_ptr, out_buffer_ptr, in_buffer_ptr,
                                         count);
}

vsf_err_t vsf_spi_cancel_transfer(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->cancel_transfer != NULL);

    return spi_ptr->op->cancel_transfer(spi_ptr);
}

void vsf_spi_get_transferred_count(vsf_spi_t *spi_ptr, uint_fast32_t *tx_count,
                                   uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->get_transferred_count != NULL);

    spi_ptr->op->get_transferred_count(spi_ptr, tx_count, rx_count);
}

vsf_spi_capability_t vsf_spi_capability(vsf_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->capability != NULL);

    return spi_ptr->op->capability(spi_ptr);
}

vsf_err_t vsf_spi_ctrl(vsf_spi_t *spi_ptr, vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->op != NULL);
    VSF_HAL_ASSERT(spi_ptr->op->ctrl != NULL);

    return spi_ptr->op->ctrl(spi_ptr, ctrl, param);
}

#   endif /* VSF_SPI_CFG_MULTI_CLASS == ENABLED */
#endif    /* VSF_HAL_USE_SPI == ENABLED */
