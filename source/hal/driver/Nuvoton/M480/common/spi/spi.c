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
#include "./spi.h"

#if VSF_HAL_USE_SPI == ENABLED

#include "../dma/dma.h"

/*============================ MACROS ========================================*/

#define SPI_CTL_MODE_MASK           (  SPI_CTL_SLAVE_Msk  | SPI_CTL_CLKPOL_Msk  \
                                     | SPI_CTL_TXNEG_Msk  | SPI_CTL_RXNEG_Msk   \
                                     | SPI_CTL_DWIDTH_Msk | SPI_CTL_HALFDPX_Msk \
                                     | SPI_CTL_DATDIR_Msk | SPI_CTL_RXONLY_Msk )

#define SPI_SSCTL_MASK              (  SPI_AUTO_SLAVE_SELECTION_ENABLE \
                                     | SPI_SLAVE_SELECTION_ACTIVE_HIGH)



/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_spi_dma_t {
    uint8_t  channel;
    uint8_t  per_index;

    void     *buffer;
} vsf_hw_spi_dma_t;

typedef struct vsf_hw_spi_t {
#if VSF_SPI_CFG_IMPLEMENT_OP == ENABLED
    vsf_spi_t vsf_spi;
#endif

    SPI_T           *reg;
    IRQn_Type       irq;

    vsf_spi_isr_t   isr;
    uint32_t        irq_mask;

    uint32_t req_cnt;
    uint32_t cur_cnt;

    vsf_hw_spi_dma_t tx_dma;
    vsf_hw_spi_dma_t rx_dma;
} vsf_hw_spi_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static volatile uint32_t __dummy;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsf_hw_spi_dma_request(vsf_hw_spi_t *spi_ptr);

/*============================ IMPLEMENTATION ================================*/

// TODO: support Slave mode
// TODO: test spi 9~32bit
// TODO: spi request : test master input

static void __vsf_hw_spi_enable_clock(vsf_hw_spi_t *spi_ptr)
{
    // PCLK0 and PCLK1 all are 96MHz, and SPI only up to 96MHz.
    // So force spi using PCLK0(SPI0/SPI2/SPI4)/PCLK1(SP1/SP3)

    // TODO: SPI slave mode Clock

    bool state = m480_reg_unlock();

    // spi clock select
#if VSF_HAL_USE_SPI0 == ENABLED
    if (spi_ptr->reg == SPI0) {
        CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_SPI0SEL_Msk)) | (0x2UL << CLK_CLKSEL2_SPI0SEL_Pos);
        CLK->APBCLK0 |= CLK_APBCLK0_SPI0CKEN_Msk;
        SYS->IPRST1 |= SYS_IPRST1_SPI0RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_SPI0RST_Msk;
    } else
#endif
#if VSF_HAL_USE_SPI1 == ENABLED
    if (spi_ptr->reg == SPI1) {
        CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_SPI1SEL_Msk)) | (0x2UL << CLK_CLKSEL2_SPI1SEL_Pos);
        CLK->APBCLK0 |= CLK_APBCLK0_SPI1CKEN_Msk;
        SYS->IPRST1 |= SYS_IPRST1_SPI1RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_SPI1RST_Msk;
#endif
#if VSF_HAL_USE_SPI2 == ENABLED
    } else if (spi_ptr->reg == SPI2) {
        CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_SPI2SEL_Msk)) | (0x2UL << CLK_CLKSEL2_SPI2SEL_Pos);
        CLK->APBCLK0 |= CLK_APBCLK0_SPI2CKEN_Msk;
        SYS->IPRST1 |= SYS_IPRST1_SPI2RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_SPI2RST_Msk;
    } else
#endif
#if VSF_HAL_USE_SPI2 == ENABLED
    if (spi_ptr->reg == SPI3) {
        CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_SPI3SEL_Msk)) | (0x2UL << CLK_CLKSEL2_SPI3SEL_Pos);
        CLK->APBCLK1 |= CLK_APBCLK1_SPI3CKEN_Msk;
        SYS->IPRST2 |= SYS_IPRST2_SPI3RST_Msk;
        SYS->IPRST2 &= ~SYS_IPRST2_SPI3RST_Msk;
    } else
#endif
    {
        VSF_HAL_ASSERT(0);
    }

    m480_reg_lock(state);
}

static uint32_t __vsf_hw_spi_get_clock(vsf_hw_spi_t *spi_ptr)
{
    // spi clock select
    if (spi_ptr->reg == SPI0) {
        return M480_PCLK1_FREQ_HZ;
    } else if (spi_ptr->reg == SPI1) {
        return M480_PCLK0_FREQ_HZ;
    } else if (spi_ptr->reg == SPI2) {
        return M480_PCLK1_FREQ_HZ;
    } else if (spi_ptr->reg == SPI3) {
        return M480_PCLK0_FREQ_HZ;
    } else {
        return 0;
    }
}

static void __vsf_hw_spi_init_nvic(vsf_hw_spi_t *spi_ptr, const vsf_spi_isr_t* cfg_isr_ptr)
{
    if (cfg_isr_ptr->handler_fn != NULL) {
        spi_ptr->isr = *cfg_isr_ptr;
        NVIC_SetPriority(spi_ptr->irq, (uint32_t)cfg_isr_ptr->prio);
        NVIC_EnableIRQ(spi_ptr->irq);
    } else {
        NVIC_DisableIRQ(spi_ptr->irq);
    }
}

static uint8_t __vsf_hw_spi_get_data_width(vsf_hw_spi_t *spi_ptr)
{
    uint8_t data_width = (spi_ptr->reg->CTL & SPI_CTL_DWIDTH_Msk) >> SPI_CTL_DWIDTH_Pos;
    if (data_width == 0) {
        data_width = 32;
    }
    return data_width;
}

static uint8_t __vsf_hw_spi_get_data_byte_cnt(vsf_hw_spi_t *spi_ptr)
{
    uint8_t data_width = __vsf_hw_spi_get_data_width(spi_ptr);
    return (data_width + 7) / 8;
}

static vsf_err_t __vsf_hw_spi_reg_init(vsf_hw_spi_t *spi_ptr, spi_cfg_t *cfg_ptr)
{
    uint32_t clock_source;
    SPI_T* reg;

    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    reg = spi_ptr->reg;
    VSF_HAL_ASSERT(reg != NULL);

    // TODO: use pm module
    clock_source = __vsf_hw_spi_get_clock(spi_ptr);
    VSF_HAL_ASSERT(clock_source > 0);
    VSF_HAL_ASSERT(cfg_ptr->clock_hz > 0);
    VSF_HAL_ASSERT(cfg_ptr->clock_hz <= clock_source);
    uint32_t clkdiv = (clock_source + cfg_ptr->clock_hz / 2) / cfg_ptr->clock_hz - 1;
    clkdiv = vsf_min(0x100, clkdiv);

    VSF_HAL_ASSERT((cfg_ptr->mode & ~(SPI_CTL_MODE_MASK | SPI_SSCTL_MASK)) == 0);
    // TODO: Test Slave Mode
    reg->CTL        = cfg_ptr->mode & SPI_CTL_MODE_MASK;
    reg->SSCTL      = (cfg_ptr->mode & SPI_SSCTL_MASK) >> SPI_SSCTL_POS;
    reg->CLKDIV     = clkdiv;
    reg->FIFOCTL   |= SPI_FIFOCTL_RXFBCLR_Msk | SPI_FIFOCTL_TXFBCLR_Msk;

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_init(vsf_hw_spi_t *spi_ptr, spi_cfg_t *cfg_ptr)
{
    vsf_err_t result;

    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    // TODO: use pm module to enable sync clock
    __vsf_hw_spi_enable_clock(spi_ptr);

    // spi init
    result = __vsf_hw_spi_reg_init(spi_ptr, cfg_ptr);
    if (VSF_ERR_NONE == result) {
        __vsf_hw_spi_init_nvic(spi_ptr, &cfg_ptr->isr);
    }

    m484_dma_cfg_t cfg = {
        .prio = vsf_arch_prio_1,
    };
    m484_dma_init(&cfg);

    return result;
}

fsm_rt_t vsf_hw_spi_enable(vsf_hw_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    spi_ptr->reg->CTL |= SPI_CTL_SPIEN_Msk;

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_spi_disable(vsf_hw_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    spi_ptr->reg->CTL &= ~SPI_CTL_SPIEN_Msk;

    return fsm_rt_cpl;
}

void vsf_hw_spi_irq_enable(vsf_hw_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    VSF_HAL_ASSERT((irq_mask & ~(SPI_IRQ_MASK_TX_CPL | SPI_IRQ_MASK_CPL)) == 0);

    spi_ptr->irq_mask |= irq_mask;
}

void vsf_hw_spi_irq_disable(vsf_hw_spi_t *spi_ptr, em_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    VSF_HAL_ASSERT((irq_mask & ~(SPI_IRQ_MASK_TX_CPL | SPI_IRQ_MASK_CPL)) == 0);

    spi_ptr->irq_mask &= ~irq_mask;
}

spi_status_t vsf_hw_spi_status(vsf_hw_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);

    spi_status_t status = {
        .use_as__peripheral_status_t.is_busy = !!(spi_ptr->reg->STATUS & SPI_STATUS_BUSY_Msk),
    };

    return status;
}

void vsf_hw_spi_fifo_transfer(vsf_hw_spi_t *spi_ptr, void *out_buffer_ptr,
                                               uint_fast32_t  out_cnt,
                                               uint_fast32_t* out_offset_ptr,
                                               void *in_buffer_ptr,
                                               uint_fast32_t  in_cnt,
                                               uint_fast32_t* in_offset_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);

    if ((out_cnt == 0) && (in_cnt == 0)) {
        VSF_HAL_ASSERT(0);
    }

    uint8_t data_width = __vsf_hw_spi_get_data_width(spi_ptr);
    uint32_t widty_bytes;
    uint32_t value = 0;
    if (data_width == 8) {
        widty_bytes = 1;
    } else if (data_width <= 16) {
        widty_bytes = 2;
    } else {
        widty_bytes = 4;
    }

    uint_fast32_t out_offset = 0;
    uint_fast32_t in_offset = 0;

    while (out_cnt > out_offset || in_cnt > in_offset) {
        uint32_t status = spi_ptr->reg->STATUS & (SPI_STATUS_TXFULL_Msk | SPI_STATUS_RXEMPTY_Msk);
        if (status == (SPI_STATUS_RXEMPTY_Msk | SPI_STATUS_TXFULL_Msk)) {
            break;
        }

        if ((out_cnt > out_offset) && !(status & SPI_STATUS_TXFULL_Msk)) {
            out_offset++;

            if (out_buffer_ptr != NULL) {
                memcpy(&value, out_buffer_ptr, widty_bytes);
            }
            spi_ptr->reg->TX = value;
        }

        if ((in_cnt > in_offset) && !(status & SPI_STATUS_RXEMPTY_Msk)) {
            in_offset--;

            value = spi_ptr->reg->RX;
            if (out_buffer_ptr != NULL) {
                memcpy(&value, out_buffer_ptr, widty_bytes);
            }
        }
    }
}

void vsf_hw_spi_cs_active(vsf_hw_spi_t *spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    VSF_HAL_ASSERT((spi_ptr->reg->SSCTL & SPI_SSCTL_AUTOSS_Msk) == 0);

    if (index == 0) {
        spi_ptr->reg->SSCTL |= SPI_SSCTL_SS_Msk;
    }
}

void vsf_hw_spi_cs_inactive(vsf_hw_spi_t *spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);
    VSF_HAL_ASSERT((spi_ptr->reg->SSCTL & SPI_SSCTL_AUTOSS_Msk) == 0);

    if (index == 0) {
        spi_ptr->reg->SSCTL &= ~SPI_SSCTL_SS_Msk;
    }
}

static void __m484_spi_tx_dma_done_handler(void *target_ptr, uint32_t dma_irq_mask)
{
    vsf_hw_spi_t *spi_ptr = (vsf_hw_spi_t *)target_ptr;

    if (spi_ptr->isr.handler_fn != NULL && (spi_ptr->irq_mask & SPI_IRQ_MASK_TX_CPL)) {
        spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr, (vsf_spi_t *)spi_ptr, SPI_IRQ_MASK_TX_CPL);
    }
}

static void __m484_spi_rx_dma_done_handler(void *target_ptr, uint32_t dma_irq_mask)
{
    vsf_hw_spi_t *spi_ptr = (vsf_hw_spi_t *)target_ptr;

    spi_ptr->cur_cnt += M484_DMA_REQUEST_MAX_SIZE;
    if (spi_ptr->cur_cnt < spi_ptr->req_cnt) {
        __vsf_hw_spi_dma_request(spi_ptr);
        return ;
    }

    if (spi_ptr->isr.handler_fn != NULL && (spi_ptr->irq_mask & SPI_IRQ_MASK_CPL)) {
        spi_ptr->isr.handler_fn(spi_ptr->isr.target_ptr, (vsf_spi_t *)spi_ptr, SPI_IRQ_MASK_CPL);
    }
}

static vsf_err_t __vsf_hw_spi_dma_request(vsf_hw_spi_t *spi_ptr)
{
    VSF_HAL_ASSERT(spi_ptr != NULL);
    VSF_HAL_ASSERT(spi_ptr->reg != NULL);

    uint32_t width_mode;
    uint32_t widty_bytes;
    uint32_t inc_mode;

    uint8_t data_width = __vsf_hw_spi_get_data_width(spi_ptr);
    if (data_width == 8) {
        width_mode = M484_DMA_TRANSFER_WIDTH_8_BIT;
        widty_bytes = 1;
    } else if (data_width <= 16) {
        width_mode = M484_DMA_TRANSFER_WIDTH_16_BIT;
        widty_bytes = 2;
    } else {
        width_mode = M484_DMA_TRANSFER_WIDTH_32_BIT;
        widty_bytes = 4;
    }

    uint_fast32_t current_size = vsf_min(spi_ptr->req_cnt, M484_DMA_REQUEST_MAX_SIZE);
    vsf_hw_spi_dma_t *tx_dma = &spi_ptr->tx_dma;
    vsf_hw_spi_dma_t *rx_dma = &spi_ptr->rx_dma;

    if (rx_dma->buffer != &__dummy) {
        rx_dma->buffer = ((uint8_t *)rx_dma->buffer) + spi_ptr->cur_cnt * widty_bytes;
        inc_mode = M484_DMA_TRANSFER_SOURCE_FIXED | M484_DMA_TRANSFER_DESTINATION_INC;
    } else {
        inc_mode = M484_DMA_TRANSFER_SOURCE_FIXED | M484_DMA_TRANSFER_DESTINATION_FIXED;
    }
    m484_dma_channel_cfg_t rx_channel_cfg = {
        .isr = {
            .handler_fn = __m484_spi_rx_dma_done_handler,
            .target_ptr = spi_ptr,
        },
        .mode = inc_mode | width_mode | rx_dma->per_index,
        .src_address = (void *)&spi_ptr->reg->RX,
        .dst_address = rx_dma->buffer,
        .count = current_size,
    };

    if (tx_dma->buffer != &__dummy) {
        tx_dma->buffer = ((uint8_t *)tx_dma->buffer) + spi_ptr->cur_cnt * widty_bytes;
        inc_mode = M484_DMA_TRANSFER_SOURCE_INC | M484_DMA_TRANSFER_DESTINATION_FIXED;
    } else {
        inc_mode = M484_DMA_TRANSFER_SOURCE_FIXED | M484_DMA_TRANSFER_DESTINATION_FIXED;
    }
    m484_dma_channel_cfg_t tx_channel_cfg = {
        .isr = {
            .handler_fn = __m484_spi_tx_dma_done_handler,
            .target_ptr = spi_ptr,
        },
        .mode = inc_mode | width_mode | tx_dma->per_index,
        .src_address = tx_dma->buffer,
        .dst_address = (void *)&spi_ptr->reg->TX,
        .count = current_size,
    };


    // TODO: when spi send and recevice are not enabled at then same time,
    //       A problem is encountered: RX completed early error before TX.
    //       In StdDriver Library, SPI_TRIGGER_TX_PDMA/SPI_TRIGGER_RX_PDMA
    //       can be triggered individually.
    spi_ptr->reg->PDMACTL &= ~(SPI_PDMACTL_RXPDMAEN_Msk | SPI_PDMACTL_TXPDMAEN_Msk);

    m484_dma_channel_config(rx_dma->channel, &rx_channel_cfg);
    m484_dma_channel_irq_enable(rx_dma->channel, M484_DMA_TRANSFER_DONE_INTERRUPT);
    m484_dma_channel_config(tx_dma->channel, &tx_channel_cfg);
    m484_dma_channel_irq_enable(tx_dma->channel, M484_DMA_TRANSFER_DONE_INTERRUPT);

    spi_ptr->reg->PDMACTL |= (SPI_PDMACTL_RXPDMAEN_Msk | SPI_PDMACTL_TXPDMAEN_Msk);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_request_transfer(vsf_hw_spi_t *spi_ptr, void *out_buffer_ptr, void *in_buffer_ptr, uint_fast32_t count)
{
    uint32_t fifo_count = (spi_ptr->reg->STATUS & SPI_STATUS_RXCNT_Msk) >> SPI_STATUS_RXCNT_Pos;
    (void)fifo_count;
    VSF_HAL_ASSERT(fifo_count == 0);

    spi_ptr->cur_cnt = 0;
    spi_ptr->req_cnt = count;
    spi_ptr->tx_dma.buffer = (out_buffer_ptr == NULL) ? (void *)&__dummy : out_buffer_ptr;
    spi_ptr->rx_dma.buffer = (in_buffer_ptr == NULL) ? (void *)&__dummy : in_buffer_ptr;

    __vsf_hw_spi_dma_request(spi_ptr);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_cancel_transfer(vsf_hw_spi_t *spi_ptr)
{
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t vsf_hw_spi_get_transfered_count(vsf_hw_spi_t *spi_ptr)
{
    return 0;
}

/*============================ INCLUDES ======================================*/

#define VSF_SPI_CFG_PREFIX                  vsf_hw
#define VSF_SPI_CFG_UPPERCASE_PREFIX        VSF_HW

#define VSF_SPI_CFG_IMP_LV0(__count, __hal_op)                                  \
    vsf_hw_spi_t vsf_hw_spi ## __count = {                                      \
        .reg = SPI ## __count,                                                  \
        .irq = SPI ## __count##_IRQn,                                           \
        .tx_dma = {                                                             \
            .per_index = M484_DMA_SPI ## __count##_TX,                          \
            .channel = VSF_HW_SPI ## __count##_DMA_TX_CHANNEL,                  \
        },                                                                      \
        .rx_dma = {                                                             \
            .per_index = M484_DMA_SPI ## __count##_RX,                          \
            .channel = VSF_HW_SPI ## __count##_DMA_RX_CHANNEL,                  \
        },                                                                      \
    };

#include "hal/driver/common/spi/spi_template.inc"

#endif
