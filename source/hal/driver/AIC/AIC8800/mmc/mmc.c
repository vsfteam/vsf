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

#define VSF_MMC_CFG_IMP_PREFIX                  vsf_hw
#define VSF_MMC_CFG_IMP_UPCASE_PREFIX           VSF_HW

/*============================ INCLUDES ======================================*/

#include "./mmc.h"

#if VSF_HAL_USE_MMC == ENABLED

#include "../vendor/plf/aic8800/src/driver/sdmmc/reg_sdmmc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_MMC_CFG_MULTI_CLASS
#   define VSF_HW_MMC_CFG_MULTI_CLASS           VSF_MMC_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_mmc_const_t {
    AIC_SDMMC_TypeDef *reg;
    IRQn_Type irqn;
} vsf_hw_mmc_const_t;

typedef struct vsf_hw_mmc_t {
#if VSF_HW_MMC_CFG_MULTI_CLASS == ENABLED
    vsf_mmc_t vsf_mmc;
#endif

    const vsf_hw_mmc_const_t *mmc_const;
} vsf_hw_mmc_t;

/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_mmc_irq_handler(vsf_hw_mmc_t *mmc_ptr)
{
}

vsf_err_t vsf_hw_mmc_init(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)
{
    return VSF_ERR_NONE;
}

void vsf_hw_mmc_fini(vsf_hw_mmc_t *mmc_ptr)
{
}

fsm_rt_t vsf_hw_mmc_enable(vsf_hw_mmc_t *mmc_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_mmc_disable(vsf_hw_mmc_t *mmc_ptr)
{
    return fsm_rt_cpl;
}

void vsf_hw_mmc_irq_enable(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
}

void vsf_hw_mmc_irq_disable(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
}

vsf_mmc_status_t vsf_hw_mmc_status(vsf_hw_mmc_t *mmc_ptr)
{
    vsf_mmc_status_t status = {
        0
    };
    return status;
}

vsf_mmc_capability_t vsf_hw_mmc_capability(vsf_hw_mmc_t *mmc_ptr)
{
    vsf_mmc_capability_t capability = {
        0
    };
    return capability;
}

vsf_err_t vsf_hw_mmc_host_set_clock(vsf_hw_mmc_t *mmc_ptr, uint32_t clock_hz)
{
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_mmc_host_set_buswidth(vsf_hw_mmc_t *mmc_ptr, uint8_t buswidth)
{
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_mmc_host_transact_start(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans)
{
    return VSF_ERR_NONE;
}

void vsf_hw_mmc_host_transact_stop(vsf_hw_mmc_t *mmc_ptr)
{
}

/*============================ INCLUDES ======================================*/

#define VSF_MMC_CFG_IMP_LV0(__count, __hal_op)                                  \
    static const vsf_hw_mmc_const_t __vsf_hw_mmc ##__count ## _const = {        \
        .reg  = VSF_HW_MMC ## __count ## _REG,                                  \
        .irqn = VSF_HW_MMC ## __count ## _IRQ_IDX,                              \
    };                                                                          \
    vsf_hw_mmc_t vsf_hw_mmc ##__count = {                                       \
        .mmc_const = &__vsf_hw_mmc ##__count ## _const,                         \
        __hal_op                                                                \
    };                                                                          \
    void VSF_HW_MMC ## __count ## _IRQ(void)                                    \
    {                                                                           \
        __vsf_hw_mmc_irq_handler(&vsf_hw_mmc##__count);                         \
    }
#include "hal/driver/common/mmc/mmc_template.inc"

#endif      // VSF_HAL_USE_MMC
