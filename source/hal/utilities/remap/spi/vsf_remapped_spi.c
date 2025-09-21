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

#include "hal/vsf_hal.h"

#if VSF_HAL_USE_SPI == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_SPI_CLASS_IMPLEMENT
#include "./vsf_remapped_spi.h"

#if VSF_SPI_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_SPI_CFG_MULTI_CLASS == ENABLED
const vsf_spi_op_t vsf_remapped_spi_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_SPI_APIS(vsf_remapped_spi)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_spi_init(vsf_remapped_spi_t *spi, vsf_spi_cfg_t *cfg)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_init(spi->target, cfg);
}

void vsf_remapped_spi_fini(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    vsf_spi_fini(spi->target);
}

vsf_err_t vsf_remapped_spi_get_configuration(vsf_remapped_spi_t *spi, vsf_spi_cfg_t *cfg)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_get_configuration(spi->target, cfg);
}

fsm_rt_t vsf_remapped_spi_enable(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_enable(spi->target);
}

fsm_rt_t vsf_remapped_spi_disable(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_disable(spi->target);
}

void vsf_remapped_spi_irq_enable(vsf_remapped_spi_t *spi, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    vsf_spi_irq_enable(spi->target, irq_mask);
}

void vsf_remapped_spi_irq_disable(vsf_remapped_spi_t *spi, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    vsf_spi_irq_disable(spi->target, irq_mask);
}

vsf_err_t vsf_remapped_spi_cs_active(vsf_remapped_spi_t *spi, uint_fast8_t index)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_cs_active(spi->target, index);
}

vsf_err_t vsf_remapped_spi_cs_inactive(vsf_remapped_spi_t *spi, uint_fast8_t index)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_cs_inactive(spi->target, index);
}

vsf_spi_status_t vsf_remapped_spi_status(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_status(spi->target);
}

vsf_spi_capability_t vsf_remapped_spi_capability(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_capability(spi->target);
}

void vsf_remapped_spi_fifo_transfer(vsf_remapped_spi_t *spi,
        void *out_buffer, uint_fast32_t *out_offset,
        void *in_buffer, uint_fast32_t *in_offset, uint_fast32_t cnt)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    vsf_spi_fifo_transfer(spi->target, out_buffer, out_offset,
        in_buffer, in_offset, cnt);
}

vsf_err_t vsf_remapped_spi_request_transfer(vsf_remapped_spi_t *spi,
        void *out_buffer, void *in_buffer, uint_fast32_t count)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_request_transfer(spi->target, out_buffer, in_buffer, count);
}

vsf_err_t vsf_remapped_spi_cancel_transfer(vsf_remapped_spi_t *spi)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_cancel_transfer(spi->target);
}

void vsf_remapped_spi_get_transferred_count(vsf_remapped_spi_t *spi, uint_fast32_t *tx_count, uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    vsf_spi_get_transferred_count(spi->target, tx_count, rx_count);
}

vsf_err_t vsf_remapped_spi_ctrl(vsf_remapped_spi_t *spi, vsf_spi_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT((spi != NULL) && (spi->target != NULL));
    return vsf_spi_ctrl(spi->target, ctrl, param);
}

#endif
#endif
