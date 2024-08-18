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

#include "../vendor/Inc/stm32h7xx.h"

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

    SDMMC_TypeDef           *reg;
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
    SDMMC_TypeDef *reg = sdio_ptr->reg;
    uint32_t sts = reg->STA;
    reg->ICR = SDMMC_ICR_CTIMEOUTC | SDMMC_ICR_CCRCFAILC | SDMMC_ICR_DCRCFAILC | SDMMC_ICR_CMDRENDC | SDMMC_ICR_CMDSENTC | SDMMC_ICR_DATAENDC;
    if (sdio_ptr->isr.handler_fn != NULL) {
        uint32_t resp[4];
        if (((reg->CMD >> SDMMC_CMD_WAITRESP_Pos) & 0x03) == 0x03) {
            resp[0] = reg->RESP4;
            resp[1] = reg->RESP3;
            resp[2] = reg->RESP2;
            resp[3] = reg->RESP1;
        } else {
            resp[0] = reg->RESP1;
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
    SDMMC_TypeDef *reg = sdio_ptr->reg;

    vsf_hw_peripheral_enable(sdio_ptr->en);

    reg->CLKCR &= ~(SDMMC_CLKCR_HWFC_EN | SDMMC_CLKCR_NEGEDGE | SDMMC_CLKCR_WIDBUS | SDMMC_CLKCR_PWRSAV);
    vsf_hw_sdio_set_clock(sdio_ptr, 400000, false);
    vsf_hw_sdio_set_bus_width(sdio_ptr, 1);
    reg->POWER = (reg->POWER & ~SDMMC_POWER_PWRCTRL) | 3;

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
    irq_mask |= SDMMC_MASK_CTIMEOUTIE | SDMMC_MASK_CCRCFAILIE | SDMMC_MASK_DCRCFAILIE;
    sdio_ptr->reg->MASK |= irq_mask;
}

void VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_irq_disable)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    vsf_sdio_irq_mask_t irq_mask
) {
    irq_mask |= SDMMC_MASK_CTIMEOUTIE | SDMMC_MASK_CCRCFAILIE | SDMMC_MASK_DCRCFAILIE;
    sdio_ptr->reg->MASK &= ~irq_mask;
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
    SDMMC_TypeDef *reg = sdio_ptr->reg;
    uint32_t tmp = reg->CLKCR & ~SDMMC_CLKCR_CLKDIV;
    uint32_t div = (vsf_hw_clk_get_freq_hz(sdio_ptr->clk) / clock_hz) >> 1;
    VSF_HAL_ASSERT(div <= 0x3FF);
    reg->CLKCR = tmp | div;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_set_bus_width)(
    VSF_MCONNECT(VSF_SDIO_CFG_IMP_PREFIX, _sdio_t) *sdio_ptr,
    uint8_t bus_width
) {
    VSF_HAL_ASSERT(sdio_ptr != NULL);
    SDMMC_TypeDef *reg = sdio_ptr->reg;
    uint32_t tmp = reg->CLKCR & ~SDMMC_CLKCR_WIDBUS;
    switch (bus_width) {
    case 1:
        reg->CLKCR = tmp | (0 << SDMMC_CLKCR_WIDBUS_Pos);
        break;
    case 4:
        reg->CLKCR = tmp | (1 << SDMMC_CLKCR_WIDBUS_Pos);
        break;
    case 8:
        reg->CLKCR = tmp | (2 << SDMMC_CLKCR_WIDBUS_Pos);
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

    SDMMC_TypeDef *reg = sdio_ptr->reg;
    uint32_t datactl = req->op & 0x00FF;
    uint32_t cmdctl = (req->op & 0xFF00) >> 2;

    VSF_HAL_ASSERT(!(reg->CMD & SDMMC_CMD_CPSMEN));

    if (req->op & __SDIO_CMDOP_DTEN) {
        VSF_HAL_ASSERT(!(req->count & 0x1F) && ((req->count >> 5) <= 2040));
        VSF_HAL_ASSERT(req->block_size_bits < 15);
        VSF_HAL_ASSERT(!(req->count % (1 << req->block_size_bits)));
        VSF_HAL_ASSERT(!((uint32_t)req->buffer & 3));

        // Clean cache first even for read operation, because the cache maybe uncommited,
        //  while SDIO DMA write to the same ram.
        SCB_CleanDCache_by_Addr(req->buffer, req->count);
        if (req->op & __SDIO_CMDOP_DTDIR) {
            // for read operation, invalid buffer after done
            sdio_ptr->buf = req->buffer;
            sdio_ptr->bufsiz = req->count;
        } else {
            sdio_ptr->buf = NULL;
        }

        reg->IDMABSIZE = req->count;
        reg->DLEN = req->count;
        reg->IDMABASE0 = (uint32_t)req->buffer;
        reg->DTIMER = 0xFFFFFFFF;
        reg->IDMACTRL = SDMMC_IDMA_IDMAEN;
        datactl |= req->block_size_bits << 4;
    } else {
        reg->IDMACTRL = 0;
    }

    reg->ARG = req->arg;
    reg->DCTRL = datactl;
    reg->CMD = req->cmd | cmdctl | SDMMC_CMD_CPSMEN;

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
