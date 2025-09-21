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

#if VSF_HAL_USE_SDIO == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_SDIO_CLASS_IMPLEMENT
#include "./vsf_remapped_sdio.h"

#if VSF_SDIO_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_SDIO_CFG_MULTI_CLASS == ENABLED
const vsf_sdio_op_t vsf_remapped_sdio_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_SDIO_APIS(vsf_remapped_sdio)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_sdio_init(vsf_remapped_sdio_t *sdio, vsf_sdio_cfg_t *cfg)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_init(sdio->target, cfg);
}

void vsf_remapped_sdio_fini(vsf_remapped_sdio_t *sdio)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    vsf_sdio_fini(sdio->target);
}

vsf_err_t vsf_remapped_sdio_get_configuration(vsf_remapped_sdio_t *sdio, vsf_sdio_cfg_t *cfg)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_get_configuration(sdio->target, cfg);
}

void vsf_remapped_sdio_irq_enable(vsf_remapped_sdio_t *sdio, vsf_sdio_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    vsf_sdio_irq_enable(sdio->target, irq_mask);
}

void vsf_remapped_sdio_irq_disable(vsf_remapped_sdio_t *sdio, vsf_sdio_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    vsf_sdio_irq_disable(sdio->target, irq_mask);
}

vsf_sdio_status_t vsf_remapped_sdio_status(vsf_remapped_sdio_t *sdio)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_status(sdio->target);
}

vsf_sdio_capability_t vsf_remapped_sdio_capability(vsf_remapped_sdio_t *sdio)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_capability(sdio->target);
}

vsf_err_t vsf_remapped_sdio_set_clock(vsf_remapped_sdio_t *sdio, uint32_t clock_hz, bool is_ddr)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_set_clock(sdio->target, clock_hz, is_ddr);
}

vsf_err_t vsf_remapped_sdio_set_bus_width(vsf_remapped_sdio_t *sdio, uint8_t bus_width)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_set_bus_width(sdio->target, bus_width);
}

vsf_err_t vsf_remapped_sdio_host_request(vsf_remapped_sdio_t *sdio, vsf_sdio_req_t *req)
{
    VSF_HAL_ASSERT((sdio != NULL) && (sdio->target != NULL));
    return vsf_sdio_host_request(sdio->target, req);
}

#endif
#endif
