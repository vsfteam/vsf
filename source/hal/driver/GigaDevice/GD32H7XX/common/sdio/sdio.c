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
    void                   *buf;
    uint32_t                bufsiz;
} VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t);

/*============================ IMPLEMENTATION ================================*/

static void VSF_MCONNECT(__, VSF_SDIO_CFG_IMP_PREFIX, _sdio_irqhandler)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr
) {
    uint32_t reg = sdio_ptr->reg;
    uint32_t sts = SDIO_STAT(reg);
    SDIO_INTC(reg) = SDIO_INTC_CMDTMOUTC | SDIO_INTC_CCRCERRC | SDIO_INTC_DTCRCERRC | SDIO_INTC_CMDRECVC | SDIO_INTC_CMDSENDC | SDIO_INTC_DTENDC;
    if (sdio_ptr->isr.handler_fn != NULL) {
        uint32_t resp[4];
        if (((SDIO_CMDCTL(reg) >> 8) & 0x03) == 0x03) {
            resp[0] = SDIO_RESP3(reg);
            resp[1] = SDIO_RESP2(reg);
            resp[2] = SDIO_RESP1(reg);
            resp[3] = SDIO_RESP0(reg);
        } else {
            resp[0] = SDIO_RESP0(reg);
        }
        if ((sdio_ptr->buf != NULL) && (sdio_ptr->bufsiz > 0)) {
            SCB_InvalidateDCache_by_Addr(sdio_ptr->buf, sdio_ptr->bufsiz);
            sdio_ptr->buf = NULL;
        }
        sdio_ptr->isr.handler_fn(sdio_ptr->isr.target_ptr, &sdio_ptr->vsf_sdio, (vsf_sdio_irq_mask_t)sts, (vsf_sdio_reqsts_t)sts, resp);
    }
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_init)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_cfg_t *cfg_ptr
) {
    uint32_t reg = sdio_ptr->reg;

    vsf_hw_peripheral_enable(sdio_ptr->en);

    SDIO_CLKCTL(reg) &= ~(SDIO_CLKCTL_HWEN | SDIO_CLKCTL_CLKEDGE | SDIO_CLKCTL_BUSMODE | SDIO_CLKCTL_CLKPWRSAV);
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
    irq_mask |= SDIO_INTEN_CMDTMOUTIE | SDIO_INTEN_CCRCERRIE | SDIO_INTEN_DTCRCERRIE;
    SDIO_INTEN(sdio_ptr->reg) |= irq_mask;
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_disable)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_irq_mask_t irq_mask
) {
    irq_mask |= SDIO_INTEN_CMDTMOUTIE | SDIO_INTEN_CCRCERRIE | SDIO_INTEN_DTCRCERRIE;
    SDIO_INTEN(sdio_ptr->reg) &= ~irq_mask;
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
        .bus_width      = SDIO_CAP_BUS_WIDTH_1 | SDIO_CAP_BUS_WIDTH_4 | SDIO_CAP_BUS_WIDTH_8,
        .max_freq_hz    = 200 * 1000 * 1000,
        .support_ddr    = true,
    };
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_clock)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    uint32_t clock_hz,
    bool is_ddr
) {
    VSF_HAL_ASSERT(sdio_ptr != NULL);
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
    VSF_HAL_ASSERT(sdio_ptr != NULL);
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

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_host_request)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_req_t *req
) {
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    VSF_HAL_ASSERT(req != NULL);
    VSF_HAL_ASSERT(!(req->cmd & ~0x3F));
    req->op &= 0xFFFF;

    uint32_t reg = sdio_ptr->reg;
    uint32_t datactl = req->op & 0x00FF;
    uint32_t cmdctl = (req->op & 0xFF00) >> 2;

    VSF_HAL_ASSERT(!(SDIO_CMDCTL(reg) & SDIO_CMDCTL_CSMEN));

    if (req->op & __SDIO_CMDOP_DATAEN) {
        VSF_HAL_ASSERT(!(req->count & 0x1F) && ((req->count >> 5) <= 2040));
        VSF_HAL_ASSERT(req->block_size_bits < 15);
        VSF_HAL_ASSERT(!(req->count % (1 << req->block_size_bits)));
        VSF_HAL_ASSERT(!((uint32_t)req->buffer & 3));

        // Clean cache first even for read operation, because the cache maybe uncommited,
        //  while SDIO DMA write to the same ram.
        SCB_CleanDCache_by_Addr(req->buffer, req->count);
        if (req->op & __SDIO_CMDOP_DATADIR) {
            // for read operation, invalid buffer after done
            sdio_ptr->buf = req->buffer;
            sdio_ptr->bufsiz = req->count;
        } else {
            sdio_ptr->buf = NULL;
        }

        SDIO_IDMASIZE(reg) = req->count;
        SDIO_DATALEN(reg) = req->count;
        SDIO_IDMAADDR0(reg) = (uint32_t)req->buffer;
        SDIO_DATATO(reg) = 0xFFFFFFFF;
        SDIO_IDMACTL(reg) = SDIO_IDMA_SINGLE_BUFFER | SDIO_IDMACTL_IDMAEN;
        datactl |= req->block_size_bits << 4;
    } else {
        SDIO_IDMACTL(reg) = 0;
    }

    SDIO_CMDAGMT(reg) = req->arg;
    SDIO_DATACTL(reg) = datactl;
    SDIO_CMDCTL(reg) = req->cmd | cmdctl | SDIO_CMDCTL_CSMEN;

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
