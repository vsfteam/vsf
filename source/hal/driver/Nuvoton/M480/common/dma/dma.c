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
#include "./dma.h"


/*============================ MACROS ========================================*/
#define M484_CHANNAL_MAX    16
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static m484_dma_channel_t __channals[M484_CHANNAL_MAX];
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t m484_dma_init(m484_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    bool state = m480_reg_unlock();

    // enable dma clock
    CLK->AHBCLK |= CLK_AHBCLK_PDMACKEN_Msk;

    // reset pdma
    SYS->IPRST0  |= SYS_IPRST0_PDMARST_Msk;
    SYS->IPRST0  &= ~SYS_IPRST0_PDMARST_Msk;

    // enable dma interrupt and update interrupt priority
    NVIC_SetPriority(PDMA_IRQn, (uint32_t)cfg_ptr->prio);
    NVIC_EnableIRQ(PDMA_IRQn);
    //NVIC_SetPendingIRQ(PDMA_IRQn);

    m480_reg_lock(state);

    return VSF_ERR_NONE;
}

vsf_err_t m484_dma_channel_config(uint8_t channel, m484_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(channel < M484_CHANNAL_MAX);
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT((0 < cfg_ptr->count) && (cfg_ptr->count <= UINT16_MAX + 1));
    VSF_HAL_ASSERT(cfg_ptr->src_address != NULL);
    VSF_HAL_ASSERT(cfg_ptr->dst_address != NULL);

    if (PDMA->TRGSTS & (1 << channel)) {
        return VSF_ERR_FAIL;
    }

    uint32_t peripheral_index = cfg_ptr->mode & PDMA_REQSEL0_3_REQSRC0_Msk;

    PDMA->CHCTL &= ~(1 << channel);     // disable current channel
    PDMA->TDSTS = 1 << channel;         // clean last transfer flag

    PDMA->DSCT[channel].CTL   = (0x01 << PDMA_DSCT_CTL_OPMODE_Pos)    |   // Basic mode
                                (0x01 << PDMA_DSCT_CTL_TXTYPE_Pos)    |   // signal transfer type
                                (0x00 << PDMA_DSCT_CTL_TBINTDIS_Pos)  |   // Table interrupt Disabled
                                (0x00 << PDMA_DSCT_CTL_STRIDEEN_Pos)  |   // Stride transfer mode Disabled
                                cfg_ptr->mode & (PDMA_DSCT_CTL_TXWIDTH_Msk | PDMA_DSCT_CTL_SAINC_Msk | PDMA_DSCT_CTL_DAINC_Msk);
    PDMA->DSCT[channel].NEXT  = 0;

    __IO uint32_t* reqsel     = &PDMA->REQSEL0_3;
    uint32_t reg_offset       = channel / 4;
    uint32_t bits_pos         = (channel % 4) * 8;

    reqsel[reg_offset]        = (reqsel[reg_offset] & ~(PDMA_REQSEL0_3_REQSRC0_Msk << bits_pos)) | (peripheral_index << bits_pos) ;
    __channals[channel].count = cfg_ptr->count;

    PDMA->DSCT[channel].SA    = (uint32_t)cfg_ptr->src_address;
    PDMA->DSCT[channel].DA    = (uint32_t)cfg_ptr->dst_address;
    PDMA->DSCT[channel].CTL   = (PDMA->DSCT[channel].CTL & ~(PDMA_DSCT_CTL_TXCNT_Msk)) | ((cfg_ptr->count - 1) << PDMA_DSCT_CTL_TXCNT_Pos);

    PDMA->CHCTL |= 1 << channel;        // enabled current channel

    if (cfg_ptr->isr.handler_fn != NULL) {
        __channals[channel].isr = cfg_ptr->isr;
    }

    return VSF_ERR_NONE;
}

bool m484_dma_channel_is_done(uint8_t channel)
{
    uint32_t tdstd = PDMA->TDSTS & (1 << channel);
    PDMA->TDSTS = tdstd;

    return !!tdstd;
}

vsf_err_t m484_dma_channel_irq_enable(uint8_t channel, uint32_t irq_mask)
{
    if (irq_mask & M484_DMA_TRANSFER_DONE_INTERRUPT) {
        PDMA->INTEN |= 1 << channel;
        __channals[channel].irq_mask |= M484_DMA_TRANSFER_DONE_INTERRUPT;
    }
    return VSF_ERR_NONE;
}

vsf_err_t m484_dma_channel_irq_disable(uint8_t channel, uint32_t irq_mask)
{
    if (irq_mask & M484_DMA_TRANSFER_DONE_INTERRUPT) {
        PDMA->INTEN &= ~(1 << channel);
        __channals[channel].irq_mask &= ~M484_DMA_TRANSFER_DONE_INTERRUPT;
    }
    return VSF_ERR_NONE;
}

vsf_err_t m484_dma_cancel_transfer(int8_t channel)
{
    //TODO: add cancel support
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t m484_dma_get_transfered_count(int8_t channel)
{
    //TODO: test transfered count
    uint32_t transfer_count = (PDMA->DSCT[channel].CTL & PDMA_DSCT_CTL_TXCNT_Msk) >> PDMA_DSCT_CTL_TXCNT_Pos;
    return __channals[channel].count - transfer_count - 1;
}

void PDMA_IRQHandler(void)
{
    uint32_t intsts = PDMA->INTSTS;
    if (intsts & PDMA_INTSTS_TDIF_Msk) {
        uint32_t transfer_done = PDMA->TDSTS;
        PDMA->TDSTS = transfer_done;

        for (int i = 0; i < M484_CHANNAL_MAX; i++) {
            if (transfer_done & (1 << i)) {
                if (__channals[i].isr.handler_fn != NULL && (__channals[i].irq_mask & M484_DMA_TRANSFER_DONE_INTERRUPT)) {
                    __channals[i].isr.handler_fn(__channals[i].isr.target_ptr, M484_DMA_TRANSFER_DONE_INTERRUPT);
                }
            }
        }

    }

    // Todo: Add timeout

    PDMA->INTSTS = intsts;
}
