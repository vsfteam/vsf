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

#if VSF_HAL_USE_MMC == ENABLED

#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_REMAPPED_MMC_CLASS_IMPLEMENT
#include "./vsf_remapped_mmc.h"

#if VSF_MMC_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_REMAPPED_MMC_CFG_MULTI_CLASS == ENABLED
const vsf_mmc_op_t vsf_remapped_mmc_op = {
#   undef __VSF_HAL_TEMPLATE_API
#   define __VSF_HAL_TEMPLATE_API   VSF_HAL_TEMPLATE_API_OP

    VSF_MMC_APIS(vsf_remapped)
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_remapped_mmc_init(vsf_remapped_mmc_t *mmc, vsf_mmc_cfg_t *cfg)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_init(mmc->target, cfg);
}

void vsf_remapped_mmc_fini(vsf_remapped_mmc_t *mmc)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    vsf_mmc_fini(mmc->target);
}

void vsf_remapped_mmc_irq_enable(vsf_remapped_mmc_t *mmc, vsf_mmc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    vsf_mmc_irq_enable(mmc->target, irq_mask);
}

void vsf_remapped_mmc_irq_disable(vsf_remapped_mmc_t *mmc, vsf_mmc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    vsf_mmc_irq_disable(mmc->target, irq_mask);
}

vsf_mmc_status_t vsf_remapped_mmc_status(vsf_remapped_mmc_t *mmc)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_status(mmc->target);
}

vsf_mmc_capability_t vsf_remapped_mmc_capability(vsf_remapped_mmc_t *mmc)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_capability(mmc->target);
}

vsf_err_t vsf_remapped_mmc_set_clock(vsf_remapped_mmc_t *mmc, uint32_t clock_hz)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_set_clock(mmc->target, clock_hz);
}

vsf_err_t vsf_remapped_mmc_set_bus_width(vsf_remapped_mmc_t *mmc, uint8_t bus_width)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_set_bus_width(mmc->target, bus_width);
}

vsf_err_t vsf_remapped_mmc_host_transact_start(vsf_remapped_mmc_t *mmc, vsf_mmc_trans_t *trans)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    return vsf_mmc_host_transact_start(mmc->target, trans);
}

void vsf_remapped_mmc_host_transact_stop(vsf_remapped_mmc_t *mmc)
{
    VSF_HAL_ASSERT((mmc != NULL) && (mmc->target != NULL));
    vsf_mmc_host_transact_stop(mmc->target);
}

#endif
#endif
