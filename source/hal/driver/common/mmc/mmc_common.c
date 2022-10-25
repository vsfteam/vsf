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

#define VSF_MMC_CFG_FUNCTION_RENAME DISABLED

#include "hal/driver/driver.h"

// in case mmc is not supported by current platform, include below to avoid compile error
#include "hal/driver/common/template/vsf_template_mmc.h"

#if VSF_HAL_USE_MMC == ENABLED && VSF_MMC_CFG_MULTI_CLASS == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_mmc_init(vsf_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((mmc_ptr != NULL) && (cfg_ptr != NULL));
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->init != NULL);

    return mmc_ptr->op->init(mmc_ptr, cfg_ptr);
}

void vsf_mmc_fini(vsf_mmc_t *mmc_ptr)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->fini != NULL);

    mmc_ptr->op->fini(mmc_ptr);
}

void vsf_mmc_irq_enable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->irq_enable != NULL);

    mmc_ptr->op->irq_enable(mmc_ptr, irq_mask);
}

void vsf_mmc_irq_disable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->irq_disable != NULL);

    mmc_ptr->op->irq_disable(mmc_ptr, irq_mask);
}

vsf_mmc_status_t vsf_mmc_status(vsf_mmc_t *mmc_ptr)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->status != NULL);

    return mmc_ptr->op->status(mmc_ptr);
}

vsf_mmc_capability_t vsf_mmc_capability(vsf_mmc_t *mmc_ptr)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->capability != NULL);

    return mmc_ptr->op->capability(mmc_ptr);
}

vsf_err_t vsf_mmc_set_clock(vsf_mmc_t *mmc_ptr, uint32_t clock_hz)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->set_clock != NULL);

    return mmc_ptr->op->set_clock(mmc_ptr, clock_hz);
}

vsf_err_t vsf_mmc_set_bus_width(vsf_mmc_t *mmc_ptr, uint8_t bus_width)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->set_bus_width != NULL);

    return mmc_ptr->op->set_bus_width(mmc_ptr, bus_width);
}

vsf_err_t vsf_mmc_host_transact_start(vsf_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->host_transact_start != NULL);

    return mmc_ptr->op->host_transact_start(mmc_ptr, trans);
}

void vsf_mmc_host_transact_stop(vsf_mmc_t *mmc_ptr)
{
    VSF_HAL_ASSERT(mmc_ptr != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op != NULL);
    VSF_HAL_ASSERT(mmc_ptr->op->host_transact_stop != NULL);

    mmc_ptr->op->host_transact_stop(mmc_ptr);
}

#endif /* VSF_HAL_USE_MMC == ENABLED && VSF_MMC_CFG_MULTI_CLASS == ENABLED */
