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

#define VSF_SDIO_CFG_FUNCTION_RENAME    DISABLED

#include "hal/driver/driver.h"

#if VSF_HAL_USE_SDIO == ENABLED && VSF_SDIO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_sdio_init(vsf_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((sdio_ptr != NULL) && (cfg_ptr != NULL));
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->init != NULL);

    return sdio_ptr->op->init(sdio_ptr, cfg_ptr);
}

void vsf_sdio_fini(vsf_sdio_t *sdio_ptr)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->fini != NULL);

    sdio_ptr->op->fini(sdio_ptr);
}

void vsf_sdio_irq_enable(vsf_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->irq_enable != NULL);

    sdio_ptr->op->irq_enable(sdio_ptr, irq_mask);
}

void vsf_sdio_irq_disable(vsf_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->irq_disable != NULL);

    sdio_ptr->op->irq_disable(sdio_ptr, irq_mask);
}

vsf_sdio_status_t vsf_sdio_status(vsf_sdio_t *sdio_ptr)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->status != NULL);

    return sdio_ptr->op->status(sdio_ptr);
}

vsf_sdio_capability_t vsf_sdio_capability(vsf_sdio_t *sdio_ptr)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->capability != NULL);

    return sdio_ptr->op->capability(sdio_ptr);
}

vsf_err_t vsf_sdio_set_clock(vsf_sdio_t *sdio_ptr, uint32_t clock_hz, bool is_ddr)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->set_clock != NULL);

    return sdio_ptr->op->set_clock(sdio_ptr, clock_hz, is_ddr);
}

vsf_err_t vsf_sdio_set_bus_width(vsf_sdio_t *sdio_ptr, uint8_t bus_width)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->set_bus_width != NULL);

    return sdio_ptr->op->set_bus_width(sdio_ptr, bus_width);
}

vsf_err_t vsf_sdio_host_request(vsf_sdio_t *sdio_ptr, vsf_sdio_req_t *req)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->host_request != NULL);

    return sdio_ptr->op->host_request(sdio_ptr, req);
}

vsf_err_t vsf_sdio_get_configuration(vsf_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op != NULL);
    VSF_HAL_ASSERT(sdio_ptr->op->get_configuration != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    return sdio_ptr->op->get_configuration(sdio_ptr, cfg_ptr);
}

#endif      // VSF_HAL_USE_SDIO && VSF_SDIO_CFG_MULTI_CLASS
