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

#include "../driver.h"

#if VSF_HAL_USE_WDT == ENABLED

#include "hal/vsf_hal.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/wdt/reg_wdt.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_WDT_CFG_MULTI_CLASS
#   define VSF_HW_WDT_CFG_MULTI_CLASS           VSF_WDT_CFG_MULTI_CLASS
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_hw_wdt_const_t {
    AIC_WDT_TypeDef* reg;
    IRQn_Type irqn;

    // TODO: move to pm
    uint32_t pclk;
    uint32_t oclk;
} vsf_hw_wdt_const_t;

typedef struct vsf_hw_wdt_t {
#if VSF_HW_WDT_CFG_MULTI_CLASS == ENABLED
    vsf_wdt_t vsf_wdt;
#endif

    const vsf_hw_wdt_const_t *wdt_const;

    vsf_wdt_isr_handler_t  *handler_fn;
    void                   *target_ptr;
} vsf_hw_wdt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_wdt_init(vsf_hw_wdt_t *wdt_ptr, vsf_wdt_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    const vsf_hw_wdt_const_t *wdt_const = wdt_ptr->wdt_const;
    VSF_HAL_ASSERT(wdt_const != NULL);
    AIC_WDT_TypeDef* reg = wdt_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    cpusysctrl_pclkme_set(wdt_const->pclk);
    cpusysctrl_oclkme_set(wdt_const->oclk);

    AIC_AONSYSCTRL->WDTRE |= (0x01UL /*<< 0*/); // wdt reset enable
    reg->LOAD = cfg_ptr->max_ms * 32768 / 1000;
    reg->CTRL = cfg_ptr->mode & __AIC8800_WDT_MODE_ALL_BITS;

    if (cfg_ptr->isr.handler_fn != NULL) {
        wdt_ptr->handler_fn = cfg_ptr->isr.handler_fn;
        wdt_ptr->target_ptr = cfg_ptr->isr.target_ptr;
        NVIC_SetPriority(wdt_const->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(wdt_const->irqn);
    } else {
        NVIC_DisableIRQ(wdt_const->irqn);
    }

    return VSF_ERR_NONE;
}

void vsf_hw_wdt_fini(vsf_hw_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    const vsf_hw_wdt_const_t *wdt_const = wdt_ptr->wdt_const;
    VSF_HAL_ASSERT(wdt_const != NULL);

    NVIC_DisableIRQ(wdt_const->irqn);
}

fsm_rt_t vsf_hw_wdt_enable(vsf_hw_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->wdt_const != NULL);
    VSF_HAL_ASSERT(wdt_ptr->wdt_const->reg != NULL);

    wdt_ptr->wdt_const->reg->CTRL |= 0x01UL << 1;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_wdt_disable(vsf_hw_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    VSF_HAL_ASSERT(wdt_ptr->wdt_const != NULL);
    VSF_HAL_ASSERT(wdt_ptr->wdt_const->reg != NULL);

    wdt_ptr->wdt_const->reg->CTRL &= ~0x01UL << 1;

    return fsm_rt_cpl;
}

vsf_wdt_capability_t vsf_hw_wdt_capability(vsf_hw_wdt_t *wdt_ptr)
{
    vsf_wdt_capability_t cap = {
        .support_early_wakeup = 1,
        .support_reset_soc    = 1,
        .support_reset_cpu   = 0,
        .support_reset_none   = 0,
        .support_disable      = 1,
        .max_timeout_ms       = (uint64_t)0x100000000ull * 1000 / 32768,
    };

    return cap;
}

void vsf_hw_wdt_feed(vsf_hw_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    const vsf_hw_wdt_const_t *wdt_const = wdt_ptr->wdt_const;
    VSF_HAL_ASSERT(wdt_const != NULL);
    AIC_WDT_TypeDef* reg = wdt_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    reg->LOAD = reg->LOAD;
}

void __vsf_hw_wdt_irq_handler(vsf_hw_wdt_t *wdt_ptr)
{
    VSF_HAL_ASSERT(wdt_ptr != NULL);
    const vsf_hw_wdt_const_t *wdt_const = wdt_ptr->wdt_const;
    VSF_HAL_ASSERT(wdt_const != NULL);
    AIC_WDT_TypeDef* reg = wdt_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    reg->INT_CLR = 1;

    if (wdt_ptr->handler_fn != NULL) {
        wdt_ptr->handler_fn(wdt_ptr->target_ptr, (vsf_wdt_t *)wdt_ptr);
    }
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_WDT_CFG_REIMPLEMENT_API_CAPABILITY      ENABLED
#define VSF_WDT_CFG_IMP_PREFIX                      vsf_hw
#define VSF_WDT_CFG_IMP_UPCASE_PREFIX               VSF_HW
#define VSF_WDT_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const vsf_hw_wdt_const_t __vsf_hw_wdt ## __IDX ## _const = {         \
        .reg    = (AIC_WDT_TypeDef *)VSF_HW_WDT ## __IDX ## _BASE_ADDRESS,      \
        .irqn   = VSF_HW_WDT ## __IDX ## _IRQN,                                 \
        .pclk   = VSF_HW_WDT ## __IDX ## _PCLK_EN_BIT,                          \
        .oclk   = VSF_HW_WDT ## __IDX ## _OCLK_EN_BIT,                          \
    };                                                                          \
    vsf_hw_wdt_t vsf_hw_wdt ## __IDX = {                                        \
        .wdt_const  = &__vsf_hw_wdt ## __IDX ## _const,                         \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_WDT ## __IDX ## _IRQHandler(void)                               \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_wdt_irq_handler(&vsf_hw_wdt ## __IDX);                         \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/wdt/wdt_template.inc"

#endif      // VSF_HAL_USE_WDT
