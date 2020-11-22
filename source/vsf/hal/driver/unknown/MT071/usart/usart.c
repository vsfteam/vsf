/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#include "utilities/vsf_utilities.h"

#define __VSF_USART_CLASS_IMPLEMENT
#include "./usart.h"

#if VSF_HAL_USE_USART == ENABLED

/*============================ MACROS ========================================*/

/*============================ MACROFIED FUNCTIONS ===========================*/

#define ____USART_LV0_IMPL(__N, __DONT_CARE)                                    \
vsf_usart_t vsf_usart##__N = {                                                  \
    .ip = {                                                                     \
        .reg        = USART##__N,                                               \
        .irq        = USART##__N##_IRQn,                                        \
        .pclk_idx   = PCLK_USART##__N##_IDX,                                    \
        .sclk_idx   = SCLK_USART##__N##_IDX,                                    \
        .tx_dma     = USART##__N##_TX_DMA,                                      \
        .rx_dma     = USART##__N##_RX_DMA,                                      \
    },                                                                          \
};                                                                              \
ROOT void USART##__N##_IRQHandler(void)                                         \
{                                                                               \
    __vsf_usart_irqhandler(&vsf_usart##__N);                                    \
}

#define __USART_LV0_IMPL(__N, __DONT_CARE)                                      \
            ____USART_LV0_IMPL(__N, __DONT_CARE)

/*============================ TYPES =========================================*/

typedef struct vsf_usart_ip_t {
    USART_TypeDef               *reg;
    IRQn_Type                   irq;

    pm_pclk_no_t                pclk_idx;
    pm_sclk_no_t                sclk_idx;

    uint8_t                     tx_dma;
    uint8_t                     rx_dma;
} vsf_usart_ip_t;

struct vsf_usart_t {
    const vsf_usart_ip_t        ip;
    vsf_usart_isr_t             isr;

    struct {
        struct {
            uint32_t            count;
            __dma_channel_t     *channel;
        } tx;
        struct {
            uint32_t            count;
            __dma_channel_t     *channel;
        } rx;
    } dma;
};

/*============================ PROTOTYPES ====================================*/

static void __vsf_usart_irqhandler(vsf_usart_t *usart);
static void __vsf_usart_dma_irqhandler(void *param, __dma_channel_t *channel);

/*============================ GLOBAL VARIABLES ==============================*/

#if USART_MAX_PORT >= 0 && VSF_HAL_USE_USART0 == ENABLED
#   if VSF_HAL_USE_LV1
__VSF_USART_LV1_IMPL(0)
#   endif
__USART_LV0_IMPL(0, NULL)
#endif

#if USART_MAX_PORT >= 1 && VSF_HAL_USE_USART1 == ENABLED
#   if VSF_HAL_USE_LV1
__VSF_USART_LV1_IMPL(1)
#   endif
__USART_LV0_IMPL(1, NULL)
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USART_CFG_9_BIT == ENABLED
static inline uint_fast8_t __vsf_usart_get_word_length(vsf_usart_t *usart)
{
    // TODO: use MACROs instead of 1
    return 1 + (usart->ip.reg->CTRL3 & 1);
}
#endif

vsf_err_t vsf_usart_init(vsf_usart_t *usart, usart_cfg_t *cfg)
{
    VSF_HAL_ASSERT((NULL != usart) && (NULL != cfg));
    USART_TypeDef *reg = usart->ip.reg;
    uint32_t baudrate_ctrl, outclk_ctrl = 0;

    // TODO: use pm module to enable sync clock
    RCC->AHBCLKCTRL0_SET = 1 << usart->ip.sclk_idx;
    RCC->USARTCLKSEL = RCC_USARTCLKSEL_SEL_SYSTEM_PLL;
    RCC->USARTCLKUEN = 0;
    RCC->USARTCLKUEN = 1;

    //! calculate baudrate
    {
        // TODO: get usart clock from pm
        uint_fast64_t usart_clk = __PLL_FREQ_HZ;
        uint_fast32_t baud_div = (usart_clk << 6) / (cfg->baudrate << (cfg->mode & USART_SAMPLE_RATE_8X ? 3 : 4));
        VSF_HAL_ASSERT(!(baud_div & ~((1 << 23) - 1)));
        baudrate_ctrl = ((baud_div & 0x3FFFC0) << 10) | ((baud_div & 0x3F) << 8);
    }

    if ((cfg->mode & USART_SYNC_MODE) && (cfg->mode & USART_SYNC_CLKOUT_EN)) {
        // TODO:
        outclk_ctrl = 0;
    }

    reg->LINECTRL = (cfg->mode & 0xFF) | baudrate_ctrl | USART_LINECTRL_FEN;
    reg->CTRL2 = ((cfg->mode & ~USART_LOOPBACK_EN) & 0xFFFF00) | USART_CTRL2_USARTEN | (cfg->mode & USART_LOOPBACK_EN ? USART_CTRL2_LBE : 0);
    reg->CTRL3 = ((cfg->mode & 0xFF000000) >> 24) | outclk_ctrl;

    if (cfg->rx_timeout > 0) {
        VSF_HAL_ASSERT(cfg->rx_timeout <= BITMASK(20));
        reg->RXTIMEOUT = (reg->RXTIMEOUT & ~BITMASK(20)) | cfg->rx_timeout | USART_RXTIMEROUT_STTTO;
    }

    IRQn_Type irq = usart->ip.irq;
    if (cfg->isr.handler_fn != NULL) {
        usart->isr = cfg->isr;
        NVIC_SetPriority(irq, (uint32_t)cfg->isr.prio);
        NVIC_EnableIRQ(irq);
    } else {
        NVIC_DisableIRQ(irq);
    }

    return VSF_ERR_NONE;
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    *((uint32_t *)RCC + ((usart->ip.pclk_idx >> 8) & 0xFF)) = 1;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    *((uint32_t *)RCC + ((usart->ip.pclk_idx >> 8) & 0xFF)) = 0;
    return fsm_rt_cpl;
}

void vsf_usart_irq_enable(vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != usart);
    USART_TypeDef *reg = usart->ip.reg;
    reg->INTR_SET = irq_mask & ~(USART_IRQ_MASK_TX_CPL | USART_IRQ_MASK_RX_CPL);
    if (irq_mask & USART_IRQ_MASK_RX_TIMEOUT) {
        reg->RXTIMEOUT |= USART_RXTIMEROUT_IE;
    }
    // TODO: DMA interrupt USART_IRQ_MASK_TX_CPL and USART_IRQ_MASK_RX_CPL
}

void vsf_usart_irq_disable(vsf_usart_t *usart, em_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != usart);
    USART_TypeDef *reg = usart->ip.reg;
    reg->INTR_CLR = irq_mask & ~(USART_IRQ_MASK_TX_CPL | USART_IRQ_MASK_RX_CPL);
    if (irq_mask & USART_IRQ_MASK_RX_TIMEOUT) {
        reg->RXTIMEOUT &= ~USART_RXTIMEROUT_IE;
    }
    // TODO: DMA interrupt USART_IRQ_MASK_TX_CPL and USART_IRQ_MASK_RX_CPL
}

usart_status_t vsf_usart_status(vsf_usart_t *usart)
{
    VSF_HAL_ASSERT(NULL != usart);
    uint_fast32_t usart_status = usart->ip.reg->STAT & ~1;
    usart_status_t status = {
        .value = usart_status | ((usart_status & USART_STAT_BUSY) >> 29),
    };
    return status;
}

uint_fast16_t vsf_usart_fifo_read(vsf_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT((NULL != usart) && (buffer != NULL) && (count > 0));
    USART_TypeDef *reg = usart->ip.reg;
    uint_fast16_t actual_count = 0;
    union {
        uint8_t     *ptr_u8;
        uint16_t    *ptr_u16;
        void        *ptr;
    } ptr = {
        .ptr        = buffer,
    };

#if VSF_USART_CFG_9_BIT == ENABLED
    switch (__vsf_usart_get_word_length(usart)) {
    case 1:
#endif
        for ( ; (actual_count < count) && !(reg->STAT & USART_STAT_RXFE); actual_count++) {
            *ptr.ptr_u8++ = reg->DATA;
        }
#if VSF_USART_CFG_9_BIT == ENABLED
        break;
    case 2:
        for ( ; (actual_count < count) && !(reg->STAT & USART_STAT_RXFE); actual_count++) {
            *ptr.ptr_u16++ = reg->DATA;
        }
        break;
    }
#endif
    return actual_count;
}

uint_fast16_t vsf_usart_fifo_write(vsf_usart_t *usart, void *buffer, uint_fast16_t count)
{
    VSF_HAL_ASSERT((NULL != usart) && (buffer != NULL) && (count > 0));
    USART_TypeDef *reg = usart->ip.reg;
    uint_fast16_t actual_count = 0;
    union {
        uint8_t     *ptr_u8;
        uint16_t    *ptr_u16;
        void        *ptr;
    } ptr = {
        .ptr        = buffer,
    };

#if VSF_USART_CFG_9_BIT == ENABLED
    switch (__vsf_usart_get_word_length(usart)) {
    case 1:
#endif
        for ( ; (actual_count < count) && !(reg->STAT & USART_STAT_TXFF); actual_count++) {
            reg->DATA = *ptr.ptr_u8++;
        }
#if VSF_USART_CFG_9_BIT == ENABLED
        break;
    case 2:
        for ( ; (actual_count < count) && !(reg->STAT & USART_STAT_TXFF); actual_count++) {
            reg->DATA == *ptr.ptr_u16++;
        }
        break;
    }
#endif
    return actual_count;
}

bool vsf_usart_fifo_flush(vsf_usart_t *usart)
{
    return !(usart->ip.reg->STAT & (USART_STAT_RXFE | USART_STAT_TXFE));
}

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart, void *buffer, uint_fast32_t count)
{
    ASSERT((NULL != usart) && (NULL != buffer) && (count > 0));

    if (usart->dma.rx.channel != NULL) {
        return VSF_ERR_IO;
    }

    __dma_channel_t *channel = __vsf_dma_alloc_channel(NULL, 0);
    if (channel != NULL) {
        USART_TypeDef *reg = usart->ip.reg;
#if VSF_USART_CFG_9_BIT == ENABLED
        uint_fast8_t word_len = __vsf_usart_get_word_length(usart);
#endif

        channel->trans.dst = buffer;
        channel->trans.src = (void *)&reg->DATA;
        channel->trans.ctrl =   DMA_CTRL_INT_EN
#if VSF_USART_CFG_9_BIT == ENABLED
                            |   ((1 == word_len) ?
                                    DMA_CTRL_DST_WIDTH_8 | DMA_CTRL_SRC_WIDTH_8
                                :   DMA_CTRL_DST_WIDTH_16 | DMA_CTRL_SRC_WIDTH_16)
#else
                            |   DMA_CTRL_DST_WIDTH_8 | DMA_CTRL_SRC_WIDTH_8
#endif
                            |   DMA_CTRL_DST_INC | DMA_CTRL_SRC_KEEP
                                /*  2 options for DMA RX
                                 *  1) DMA SRC MSIZE MUST be the same as USART RXIFLSEL
                                 *  2) DMA SRC MSIZE is 1, and USART FIFO disabled
                                 *  refer to CTRL2 settings below
                                 */
                            |   DMA_DST_BURST_1 | DMA_SRC_BURST_8
                            |   DMA_CTRL_TRANS_TYPE_P2M;
        channel->trans.nxt = NULL;
        channel->trans.count = count;
        channel->callback.fn = __vsf_usart_dma_irqhandler;
        channel->callback.param = (void *)((uintptr_t)usart | 0);

        usart->dma.rx.count = count;
        usart->dma.rx.channel = channel;

        reg->CTRL2_CLR = USART_CTRL2_RXIFLSEL;
        reg->CTRL2_SET = 0x00100000 | USART_CTRL2_RXDMAE;
        reg->CTRL0_SET = USART_CTRL0_RUN;
        __vsf_dma_channel_start(channel, (uint_fast64_t)usart->ip.rx_dma << 39);
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
}

vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart, void *buffer, uint_fast32_t count)
{
    ASSERT((NULL != usart) && (NULL != buffer) && (count > 0));

    if (usart->dma.tx.channel != NULL) {
        return VSF_ERR_IO;
    }

    __dma_channel_t *channel = __vsf_dma_alloc_channel(NULL, 0);
    if (channel != NULL) {
        USART_TypeDef *reg = usart->ip.reg;
#if VSF_USART_CFG_9_BIT == ENABLED
        uint_fast8_t word_len = __vsf_usart_get_word_length(usart);
#endif

        channel->trans.dst = (void *)&reg->DATA;
        channel->trans.src = buffer;
        channel->trans.ctrl =   DMA_CTRL_INT_EN
#if VSF_USART_CFG_9_BIT == ENABLED
                            |   ((1 == word_len) ?
                                    DMA_CTRL_DST_WIDTH_8 | DMA_CTRL_SRC_WIDTH_8
                                :   DMA_CTRL_DST_WIDTH_16 | DMA_CTRL_SRC_WIDTH_16)
#else
                            |   DMA_CTRL_DST_WIDTH_8 | DMA_CTRL_SRC_WIDTH_8
#endif
                            |   DMA_CTRL_DST_KEEP | DMA_CTRL_SRC_INC
                            |   DMA_DST_BURST_1 | DMA_SRC_BURST_1
                            |   DMA_CTRL_TRANS_TYPE_M2P;
        channel->trans.nxt = NULL;
        channel->trans.count = count;
        channel->callback.fn = __vsf_usart_dma_irqhandler;
        channel->callback.param = (void *)((uintptr_t)usart | 1);

        usart->dma.tx.count = count;
        usart->dma.tx.channel = channel;

        reg->CTRL2_SET = USART_CTRL2_TXDMAE;
        reg->CTRL1_SET = USART_CTRL1_RUN;
        __vsf_dma_channel_start(channel, (uint_fast64_t)usart->ip.tx_dma << 43);
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
}

vsf_err_t vsf_usart_cancel_rx(vsf_usart_t *usart)
{
    ASSERT(NULL != usart);
    ASSERT(NULL != usart->dma.rx.channel);

    __vsf_dma_channel_stop(usart->dma.rx.channel);
    usart->ip.reg->CTRL2_CLR = USART_CTRL2_RXDMAE;
    usart->ip.reg->CTRL0_CLR = USART_CTRL0_RUN;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_usart_cancel_tx(vsf_usart_t *usart)
{
    ASSERT(NULL != usart);
    ASSERT(NULL != usart->dma.tx.channel);

    __vsf_dma_channel_stop(usart->dma.tx.channel);
    usart->ip.reg->CTRL2_CLR = USART_CTRL2_TXDMAE;
    usart->ip.reg->CTRL1_CLR = USART_CTRL1_RUN;
    return VSF_ERR_NONE;
}

int_fast32_t vsf_usart_get_rx_count(vsf_usart_t *usart)
{
    ASSERT(false);
    return -1;
}

int_fast32_t vsf_usart_get_tx_count(vsf_usart_t *usart)
{
    ASSERT(false);
    return -1;
}

static void __vsf_usart_dma_irqhandler(void *param, __dma_channel_t *channel)
{
    vsf_usart_t *usart = (vsf_usart_t *)((uintptr_t)param & ~1);
    USART_TypeDef *reg = usart->ip.reg;
    em_usart_irq_mask_t irq_mask;

    if ((uintptr_t)param & 1) {
        // tx
        usart->dma.tx.channel = NULL;
        irq_mask = USART_IRQ_MASK_TX_CPL;
        reg->CTRL2_CLR = USART_CTRL2_TXDMAE;
        reg->CTRL1_CLR = USART_CTRL1_RUN;
    } else {
        // rx
        usart->dma.rx.channel = NULL;
        irq_mask = USART_IRQ_MASK_RX_CPL;
        reg->CTRL2_CLR = USART_CTRL2_RXDMAE;
        reg->CTRL0_CLR = USART_CTRL0_RUN;
    }

    if (usart->isr.handler_fn != NULL) {
        usart->isr.handler_fn(usart->isr.target_ptr, usart, irq_mask);
    }
    __vsf_dma_free_channel(channel);
}

static void __vsf_usart_irqhandler(vsf_usart_t *usart)
{
    USART_TypeDef *reg = usart->ip.reg;
    uint_fast32_t irq_reg = reg->INTR;
    uint_fast32_t irq_mask = irq_reg & 0xFFFF;

    reg->INTR_CLR = irq_mask;
    irq_mask &= irq_reg >> 16;

    if (reg->RXTIMEOUT & USART_RXTIMEROUT_IE) {
        irq_mask |= USART_IRQ_MASK_RX_TIMEOUT;
        reg->RXTIMEOUT &= ~0x00200000;
    }

    if (usart->isr.handler_fn != NULL) {
        usart->isr.handler_fn(usart->isr.target_ptr, usart, (em_usart_irq_mask_t)irq_mask);
    }
}

#endif

