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

#if VSF_HAL_USE_SDIO == ENABLED

#include "hal/vsf_hal.h"

#include "../vendor/Include/gd32h7xx_sdio.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_SDIO_CFG_MULTI_CLASS
#   define VSF_HW_SDIO_CFG_MULTI_CLASS          VSF_SDIO_CFG_MULTI_CLASS
#endif

#define VSF_SDIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_SDIO_CFG_IMP_UPCASE_PREFIX          VSF_HW

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) {
#if VSF_HW_SDIO_CFG_MULTI_CLASS == ENABLED
    vsf_sdio_t vsf_sdio;
#endif

    uint32_t                reg;
    const vsf_hw_clk_t      *clk;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    IRQn_Type               irqn;

    vsf_sdio_isr_t          isr;
} VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t);

/*============================ IMPLEMENTATION ================================*/

static void VSF_MCONNECT(__, VSF_SDIO_CFG_IMP_PREFIX, _sdio_irqhandler)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
    if (sdio_ptr->isr.handler_fn != NULL) {
    }
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_init)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_cfg_t *cfg_ptr
) {
    uint32_t reg = sdio_ptr->reg;

    vsf_hw_peripheral_enable(sdio_ptr->en);

    uint32_t tmp = SDIO_CLKCTL(reg) & ~(SDIO_CLKCTL_HWEN | SDIO_CLKCTL_CLKEDGE | SDIO_CLKCTL_BUSMODE | SDIO_CLKCTL_CLKPWRSAV);
    SDIO_CLKCTL(reg) = tmp | SDIO_BUSMODE_1BIT;
    vsf_hw_sdio_set_clock(sdio_ptr, 400000, false);
    vsf_hw_sdio_set_bus_width(sdio_ptr, 1);
    SDIO_PWRCTL(sdio_ptr->reg) = (SDIO_PWRCTL(sdio_ptr->reg) & ~SDIO_PWRCTL_PWRCTL) | SDIO_POWER_ON;

    sdio_ptr->isr = cfg_ptr->isr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(sdio_ptr->irqn, (uint32_t)cfg_ptr->isr.prio);
        NVIC_EnableIRQ(sdio_ptr->irqn);
    } else {
        NVIC_DisableIRQ(sdio_ptr->irqn);
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_fini)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
    vsf_hw_peripheral_rst_set(sdio_ptr->rst);
    vsf_hw_peripheral_rst_clear(sdio_ptr->rst);
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_enable)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_irq_mask_t irq_mask
) {
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_disable)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_irq_mask_t irq_mask
) {
}

vsf_sdio_status_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_status)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    return (vsf_sdio_status_t){
        0
    };
}

vsf_sdio_capability_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_capability)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    return (vsf_sdio_capability_t){
        0
    };
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_clock)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    uint32_t clock_hz,
    bool is_ddr
) {
    uint32_t reg = sdio_ptr->reg;
    uint32_t tmp = SDIO_CLKCTL(reg) & ~SDIO_CLKCTL_DIV;
    uint32_t div = (vsf_hw_clk_get_freq_hz(sdio_ptr->clk) / clock_hz) >> 1;
    VSF_HAL_ASSERT(div <= 0x3FF);
    SDIO_CLKCTL(reg) = tmp | div;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_bus_width)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    uint8_t bus_width
) {
    uint32_t reg = sdio_ptr->reg;
    uint32_t tmp = SDIO_CLKCTL(reg) & ~SDIO_CLKCTL_BUSMODE;
    switch (bus_width) {
    case 1:
        SDIO_CLKCTL(reg) = tmp | SDIO_BUSMODE_1BIT;
        break;
    case 4:
        SDIO_CLKCTL(reg) = tmp | SDIO_BUSMODE_4BIT;
        break;
    case 8:
        SDIO_CLKCTL(reg) = tmp | SDIO_BUSMODE_8BIT;
        break;
    default:
        VSF_HAL_ASSERT(false);
        return VSF_ERR_INVALID_PARAMETER;
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_transact_start)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_trans_t *trans
) {
    VSF_HAL_ASSERT(trans != NULL);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_transact_stop)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
}

/*============================ INCLUDES ======================================*/

#define VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY     ENABLED
#define VSF_SDIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t)                              \
        VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio, __IDX) = {                 \
        __HAL_OP                                                                \
        .reg                = VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _REG_BASE),\
        .clk                = &VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _CLK),\
        .en                 = VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _EN),\
        .rst                = VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _RST),\
        .irqn               = VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _IRQN),\
    };                                                                          \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_SDIO_CFG_IMP_UPCASE_PREFIX, _SDIO, __IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_SDIO_CFG_IMP_PREFIX, _sdio_irqhandler)(            \
            &VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio, __IDX)                \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/sdio/sdio_template.inc"

#endif      // VSF_HAL_USE_SDIO
