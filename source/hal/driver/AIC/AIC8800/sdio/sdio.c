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
#include "../vendor/plf/aic8800/src/driver/sysctrl/reg_sysctrl.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/sdmmc/reg_sdmmc.h"
#include "../vendor/plf/aic8800/src/driver/dma/dma_generic.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_SDIO_CFG_MULTI_CLASS
#   define VSF_HW_SDIO_CFG_MULTI_CLASS          VSF_SDIO_CFG_MULTI_CLASS
#endif

#define DATARD_TRIG_TH                          4
#define DATAWR_TRIG_TH                          4

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTYPES ====================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_sdio_const_t {
    AIC_SDMMC_TypeDef *reg;
    IRQn_Type irqn;
} vsf_hw_sdio_const_t;

typedef struct vsf_hw_sdio_t {
#if VSF_HW_SDIO_CFG_MULTI_CLASS == ENABLED
    vsf_sdio_t vsf_sdio;
#endif

    const vsf_hw_sdio_const_t *sdio_const;
    vsf_sdio_cfg_t cfg;
    bool is_resp_long;
} vsf_hw_sdio_t;

/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_sdio_irq_handler(vsf_hw_sdio_t *sdio_ptr)
{
    if (sdio_ptr->cfg.isr.handler_fn != NULL) {
        AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
        uint32_t irq = reg->ISR;
        reg->ICR = irq;
        uint32_t resp[4];

        if (irq & SDIO_IRQ_MASK_HOST_RESP_DONE) {
            if (sdio_ptr->is_resp_long) {
                resp[0] = reg->R0R << 1;
                resp[1] = reg->R1R;
                resp[2] = reg->R2R;
                resp[3] = reg->R3R;
            } else {
                resp[0] = reg->R3R;
            }
        }

        if (irq & SDIO_IRQ_MASK_HOST_DATA_DONE) {
            int ch = reg->CFGR & SDIO_CMDOP_WRITE ? DMA_CHANNEL_SDMMC_TX : DMA_CHANNEL_SDMMC_RX;
            dma_ch_icsr_set(ch, (dma_ch_icsr_get(ch) | DMA_CH_TBL0_ICLR_BIT |
                         DMA_CH_TBL1_ICLR_BIT | DMA_CH_TBL2_ICLR_BIT));

            reg->CTLR = (SDMMC_ENDIAN_TYPE(1) | SDMMC_DATARD_TRIGEN | SDMMC_DATAWR_TRIGEN |
                               SDMMC_DATARD_TRIGTH(DATARD_TRIG_TH) | SDMMC_DATAWR_TRIGTH(DATAWR_TRIG_TH));
            reg->CTLR = (SDMMC_RESET_N | SDMMC_ENDIAN_TYPE(1) | SDMMC_DATARD_TRIGEN | SDMMC_DATAWR_TRIGEN |
                               SDMMC_DATARD_TRIGTH(DATARD_TRIG_TH) | SDMMC_DATAWR_TRIGTH(DATAWR_TRIG_TH));
        }

        // if autocmd12 is enabled, resp is processed by hardware,
        //  and the value in RxR register are invalid, so remove resp_done flag
        if (reg->CFGR & SDMMC_AUTOCMD12_ENABLE) {
            irq &= ~SDIO_IRQ_MASK_HOST_RESP_DONE;
        }
        sdio_ptr->cfg.isr.handler_fn(sdio_ptr->cfg.isr.target_ptr, &sdio_ptr->vsf_sdio, irq, reg->GSR, resp);
    }
}

static bool __vsf_hw_sdio_host_is_busy(vsf_hw_sdio_t *sdio_ptr)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    return !!(reg->GSR & (SDMMC_COMMAND_NOT_OVER | SDMMC_HOST_BUSY | SDMMC_DATABUS_BUSY));
}

vsf_err_t vsf_hw_sdio_init(vsf_hw_sdio_t *sdio_ptr, vsf_sdio_cfg_t *cfg_ptr)
{
    cpusysctrl_hclkme_set(CSC_HCLKME_DMA_EN_BIT);
    cpusysctrl_oclkme_set(CSC_OCLKME_SDMMC_EN_BIT);
    cpusysctrl_hclk1me_set(CSC_HCLK1ME_SDMMC_EN_BIT | CSC_HCLK1ME_SDMMC_ALWAYS_EN_BIT);

    sdio_ptr->cfg = *cfg_ptr;
    NVIC_SetPriority(sdio_ptr->sdio_const->irqn, (uint32_t)cfg_ptr->isr.prio);
    NVIC_EnableIRQ(sdio_ptr->sdio_const->irqn);
    return VSF_ERR_NONE;
}

void vsf_hw_sdio_fini(vsf_hw_sdio_t *sdio_ptr)
{
}

void vsf_hw_sdio_irq_enable(vsf_hw_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    reg->IER |= irq_mask;
}

void vsf_hw_sdio_irq_disable(vsf_hw_sdio_t *sdio_ptr, vsf_sdio_irq_mask_t irq_mask)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    reg->IER &= ~irq_mask;
}

vsf_sdio_status_t vsf_hw_sdio_status(vsf_hw_sdio_t *sdio_ptr)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    vsf_sdio_status_t status = {
        .transact_status    = reg->GSR,
        .irq_status         = reg->ISR,
    };
    return status;
}

vsf_sdio_capability_t vsf_hw_sdio_capability(vsf_hw_sdio_t *sdio_ptr)
{
    vsf_sdio_capability_t capability = {
        .bus_width      = SDIO_CAP_BUS_WIDTH_1 | SDIO_CAP_BUS_WIDTH_4,
        .max_freq_hz    = 24 * 1000 * 1000,
    };
    return capability;
}

vsf_err_t vsf_hw_sdio_set_clock(vsf_hw_sdio_t *sdio_ptr, uint32_t clock_hz, bool is_ddr)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    extern uint32_t SystemCoreClock;
    if (clock_hz > SystemCoreClock / 2) {
        clock_hz = SystemCoreClock / 2;
    }
    reg->CDR = ((SystemCoreClock / clock_hz) >> 1) - 1;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_sdio_set_bus_width(vsf_hw_sdio_t *sdio_ptr, uint8_t bus_width)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    reg->DBWR = bus_width;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_sdio_host_transact_start(vsf_hw_sdio_t *sdio_ptr, vsf_sdio_trans_t *trans)
{
    VSF_HAL_ASSERT(trans != NULL);
    VSF_HAL_ASSERT(!(trans->op & (SDIO_CMDOP_CLKHOLD | SDIO_CMDOP_TRANS_STOP | SDIO_CMDOP_BYTE | SDIO_CMDOP_STREAM)));
    trans->op &= __VSF_HW_SDIO_TRANSOP_MASK;

    if (__vsf_hw_sdio_host_is_busy(sdio_ptr)) {
        return VSF_ERR_BUSY;
    }

    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;
    bool has_data = ((trans->buffer != NULL) && (trans->count > 0));
    bool is_write = trans->op & SDIO_CMDOP_WRITE;
    int ch;

    if (has_data) {
        VSF_HAL_ASSERT(trans->block_size_bits != 0);
        reg->DBLR = trans->block_size_bits;
        reg->DBCR = trans->count / (1 << trans->block_size_bits);

        reg->CTLR = (SDMMC_RESET_N | SDMMC_ENDIAN_TYPE(1) | SDMMC_DATARD_TRIGEN | SDMMC_DATAWR_TRIGEN |
                       SDMMC_DATARD_TRIGTH(DATARD_TRIG_TH) | SDMMC_DATAWR_TRIGTH(DATAWR_TRIG_TH));

        if (is_write) {
            ch = DMA_CHANNEL_SDMMC_TX;
            dma_erqcsr_set(REQ_CID_SDMMC_TX, ch);
            dma_ch_rqr_erql_setb(ch);
            dma_ch_dar_set(ch, (unsigned int)(&reg->DWRR));
            dma_ch_sar_set(ch, (unsigned int)trans->buffer);
            dma_ch_tbl0cr_set(ch, ((4 * DATAWR_TRIG_TH) | (REQ_FRAG << DMA_CH_RQTYP_LSB) | (AHB_WORD << DMA_CH_DBUSU_LSB) |
                             (AHB_WORD << DMA_CH_SBUSU_LSB) | DMA_CH_CONSTDA_BIT));
        } else {
            ch = DMA_CHANNEL_SDMMC_RX;
            dma_erqcsr_set(REQ_CID_SDMMC_RX, ch);
            dma_ch_rqr_erql_setb(ch);
            dma_ch_dar_set(ch, (unsigned int)trans->buffer);
            dma_ch_sar_set(ch, (unsigned int)(&reg->DRDR));
            dma_ch_tbl0cr_set(ch, ((4 * DATARD_TRIG_TH) | (REQ_FRAG << DMA_CH_RQTYP_LSB) | (AHB_WORD << DMA_CH_DBUSU_LSB) |
                             (AHB_WORD << DMA_CH_SBUSU_LSB) | DMA_CH_CONSTSA_BIT));
        }

        dma_ch_tbl1cr_set(ch, trans->count);
        dma_ch_tbl2cr_set(ch, trans->count);
        dma_ch_tsr_set(ch, ((4 << DMA_CH_STRANSZ_LSB) | (4 << DMA_CH_DTRANSZ_LSB)));
        dma_ch_wmar_set(ch, 0);
        dma_ch_wjar_set(ch, 0);
        dma_ch_tbl0sr_set(ch, ((0 << DMA_CH_STBL0SZ_LSB) | (0 << DMA_CH_DTBL0SZ_LSB)));
        dma_ch_tbl1ssr_set(ch, 0);
        dma_ch_tbl1dsr_set(ch, 0);
        if (is_write) {
            dma_ch_ctlr_set(ch, (DMA_CH_CHENA_BIT | (0x01UL << DMA_CH_BUSBU_LSB)));
        }
    }

    sdio_ptr->is_resp_long = !!((trans->op & (3 << 5)) == __SDIO_CMDOP_RESP_LONG_CRC);
    reg->CFGR = 0;
    reg->CMDR = trans->cmd;
    reg->ARGR = trans->arg;
    reg->CFGR = trans->op | SDMMC_COMMAND_START;

    if (has_data && !is_write) {
        dma_ch_ctlr_set(ch, (DMA_CH_CHENA_BIT | (0x01UL << DMA_CH_BUSBU_LSB)));
    }

    return VSF_ERR_NONE;
}

void vsf_hw_sdio_host_transact_stop(vsf_hw_sdio_t *sdio_ptr)
{
    AIC_SDMMC_TypeDef *reg = sdio_ptr->sdio_const->reg;

    reg->DBLR = 0x00UL;
    reg->DBCR = 0x00UL;
    reg->CTLR = (SDMMC_ENDIAN_TYPE(1) | SDMMC_DATARD_TRIGEN | SDMMC_DATAWR_TRIGEN |
                       SDMMC_DATARD_TRIGTH(DATARD_TRIG_TH) | SDMMC_DATAWR_TRIGTH(DATAWR_TRIG_TH));
    reg->CTLR = (SDMMC_RESET_N | SDMMC_ENDIAN_TYPE(1) | SDMMC_DATARD_TRIGEN | SDMMC_DATAWR_TRIGEN |
                       SDMMC_DATARD_TRIGTH(DATARD_TRIG_TH) | SDMMC_DATAWR_TRIGTH(DATAWR_TRIG_TH));
}

/*============================ INCLUDES ======================================*/

#define VSF_SDIO_CFG_REIMPLEMENT_API_CAPABILITY ENABLED
#define VSF_SDIO_CFG_IMP_PREFIX                 vsf_hw
#define VSF_SDIO_CFG_IMP_UPCASE_PREFIX          VSF_HW
#define VSF_SDIO_CFG_IMP_LV0(__IDX, __HAL_OP)                                   \
    static const vsf_hw_sdio_const_t __vsf_hw_sdio ## __IDX ## _const = {       \
        .reg  = VSF_HW_SDIO ## __IDX ## _REG,                                   \
        .irqn = VSF_HW_SDIO ## __IDX ## _IRQ_IDX,                               \
    };                                                                          \
    vsf_hw_sdio_t vsf_hw_sdio ## __IDX = {                                      \
        .sdio_const = &__vsf_hw_sdio ## __IDX ## _const,                        \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_SDIO ## __IDX ## _IRQ(void)                                     \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_sdio_irq_handler(&vsf_hw_sdio ## __IDX);                       \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/sdio/sdio_template.inc"

#endif      // VSF_HAL_USE_SDIO
