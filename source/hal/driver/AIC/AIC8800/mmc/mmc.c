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

#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/sdmmc/reg_sdmmc.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_MMC_CFG_MULTI_CLASS
#   define VSF_HW_MMC_CFG_MULTI_CLASS           VSF_MMC_CFG_MULTI_CLASS
#endif

/* Interrupt Register not in reg_sdmmc.h */
#define SDMMC_RESP_DONE_FLAG                    (1ul << 9)

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
    vsf_mmc_cfg_t cfg;
    vsf_mmc_trans_t trans;
} vsf_hw_mmc_t;

/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_mmc_irq_handler(vsf_hw_mmc_t *mmc_ptr)
{
    if (mmc_ptr->cfg.isr.handler_fn != NULL) {
        AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
        uint32_t irq = reg->ISR;
        reg->ICR = irq;
        uint32_t resp[4] = {
            reg->R0R << 1, reg->R1R, reg->R2R, reg->R3R,
        };
        mmc_ptr->cfg.isr.handler_fn(mmc_ptr->cfg.isr.target_ptr, &mmc_ptr->vsf_mmc, irq, reg->GSR, resp);
    }
}

static bool __vsf_hw_mmc_host_is_busy(vsf_hw_mmc_t *mmc_ptr)
{
    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    return !!(reg->GSR & (SDMMC_COMMAND_NOT_OVER | SDMMC_HOST_BUSY | SDMMC_DATABUS_BUSY));
}

vsf_err_t vsf_hw_mmc_init(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)
{
    cpusysctrl_hclkme_set(CSC_HCLKME_DMA_EN_BIT);
    cpusysctrl_oclkme_set(CSC_OCLKME_SDMMC_EN_BIT);
    cpusysctrl_hclk1me_set(CSC_HCLK1ME_SDMMC_EN_BIT | CSC_HCLK1ME_SDMMC_ALWAYS_EN_BIT);

    iomux_gpio_config_sel_setf(10, 6);
    iomux_gpio_config_sel_setf(11, 6);
    iomux_gpio_config_sel_setf(12, 6);
    iomux_gpio_config_sel_setf(13, 6);
    iomux_gpio_config_sel_setf(14, 6);
    iomux_gpio_config_sel_setf(15, 6);

    mmc_ptr->cfg = *cfg_ptr;
    NVIC_SetPriority(mmc_ptr->mmc_const->irqn, (uint32_t)cfg_ptr->isr.prio);
    NVIC_EnableIRQ(mmc_ptr->mmc_const->irqn);
    return VSF_ERR_NONE;
}

void vsf_hw_mmc_fini(vsf_hw_mmc_t *mmc_ptr)
{
}

void vsf_hw_mmc_irq_enable(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    reg->IER |= irq_mask;
}

void vsf_hw_mmc_irq_disable(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)
{
    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    reg->IER &= ~irq_mask;
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
    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    extern uint32_t SystemCoreClock;
    if (clock_hz > SystemCoreClock / 2) {
        clock_hz = SystemCoreClock / 2;
    }
    reg->CDR = ((SystemCoreClock / clock_hz) >> 1) - 1;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_mmc_host_set_buswidth(vsf_hw_mmc_t *mmc_ptr, uint8_t buswidth)
{
    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    reg->DBWR = buswidth;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_mmc_host_transact_start(vsf_hw_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans)
{
    VSF_HAL_ASSERT(trans != NULL);
    if (__vsf_hw_mmc_host_is_busy(mmc_ptr)) {
        return VSF_ERR_BUSY;
    }
    mmc_ptr->trans = *trans;

    AIC_SDMMC_TypeDef *reg = mmc_ptr->mmc_const->reg;
    reg->CFGR = 0;
    reg->CMDR = trans->cmd;
    reg->ARGR = trans->arg;
    reg->IER |= SDMMC_RESP_DONE_FLAG;
    reg->CFGR = trans->op | SDMMC_COMMAND_START;
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
