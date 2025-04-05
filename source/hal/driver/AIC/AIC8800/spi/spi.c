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

#if VSF_HAL_USE_SPI == ENABLED

#include "./i_reg_spi.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/dma/dma_api.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_SPI_CFG_DMA_BYTE_CNT_MAX
#   define VSF_HW_SPI_CFG_DMA_BYTE_CNT_MAX      65535
#endif

#ifndef VSF_HW_SPI_CFG_MULTI_CLASS
#   define VSF_HW_SPI_CFG_MULTI_CLASS           VSF_SPI_CFG_MULTI_CLASS
#endif

#define SPI_DATASIZE_TO_BYTE(__S)               \
    (((((__S) & VSF_SPI_DATASIZE_MASK) >> 2) + 7) / 8)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_spi_const_t {
    REG_SPI_T          *reg;

    struct {
        IRQn_Type       spi;
        IRQn_Type       dma_rx;
        IRQn_Type       dma_tx;
    } irqn;

    struct {
        struct {
            int channel;
            int cid;
        } send, recv;
    } request;

    // TODO: move to pm
    struct {
        uint32_t hclk;
        uint32_t oclk;
        uint32_t pclk;
    } clock;
} vsf_hw_spi_const_t;

typedef struct vsf_hw_spi_t {
#if VSF_HW_SPI_CFG_MULTI_CLASS == ENABLED
    vsf_spi_t vsf_spi;
#endif

    const vsf_hw_spi_const_t * spi_const;
    bool is_auto_cs;
    vsf_spi_isr_t isr;
    vsf_spi_irq_mask_t irq_mask;

    struct {
        uint32_t count;

        struct {
            void *buffer;
            uint32_t offset;
        } send, recv;
    } request;
} vsf_hw_spi_t;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __clock_init(vsf_hw_spi_t *hw_spi_ptr, uint32_t clock_hz)
{
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);

    cpusysctrl_pclkme_set(spi_const->clock.pclk);
    cpusysctrl_oclkme_set(spi_const->clock.oclk);
    cpusysctrl_hclkme_set(spi_const->clock.hclk);

    uint32_t pclk = sysctrl_clock_get(SYS_PCLK);
    int32_t value = pclk / 2 / clock_hz - 1;

    if ((value < 0) || (value > 0xFFFF)) {
        return VSF_ERR_NOT_SUPPORT;
    } else {
        uint32_t dr_value = spi_const->reg->DR & ~SPI_DR_DIV_MASK;
        spi_const->reg->DR = dr_value | ((value << SPI_DR_DIV) & SPI_DR_DIV_MASK);
        return VSF_ERR_NONE;
    }
}

static vsf_err_t __cfg_check(vsf_hw_spi_t *hw_spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((cfg_ptr->mode & ~__SPI_HW_MODE_MASK) == 0);

    // TODO: support spi slave mode
    if ((cfg_ptr->mode & VSF_SPI_DIR_MODE_MASK) != VSF_SPI_MASTER) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    // TODO: test spi datasize
    if ((cfg_ptr->mode & VSF_SPI_DATASIZE_MASK) != VSF_SPI_DATASIZE_8) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    // TODO: test spi msb
    if ((cfg_ptr->mode & VSF_SPI_BIT_ORDER_MASK) == VSF_SPI_LSB_FIRST) {
        VSF_ASSERT(0);
        return VSF_ERR_NOT_SUPPORT;
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_init(vsf_hw_spi_t *hw_spi_ptr, vsf_spi_cfg_t *cfg_ptr)
{
    vsf_err_t err;

    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr != NULL);

    err = __cfg_check(hw_spi_ptr, cfg_ptr);
    if (err != VSF_ERR_NONE) {
        return err;
    }

    err = __clock_init(hw_spi_ptr, cfg_ptr->clock_hz);
    if (err != VSF_ERR_NONE) {
        return err;
    }

    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;

    reg->MR0   = cfg_ptr->mode & VSF_SPI_DIR_MODE_MASK;
    reg->CR[0] = (0x0Ful <<  8) | (cfg_ptr->mode & (VSF_SPI_MODE_MASK | __AIC8800_VSF_SPI_DATASIZE_MASK));
    reg->CR[1] = (0x03ul << 12) | (0x00ul << 6);
    reg->CR[2] = (0x01ul <<  6); // en dma
    reg->CR[3] = ((0x02ul << 8) | (0x01ul << 0));

    hw_spi_ptr->is_auto_cs = ((cfg_ptr->mode & VSF_SPI_CS_MODE_MASK) == VSF_SPI_CS_HARDWARE_MODE);
    hw_spi_ptr->isr = cfg_ptr->isr;
    if (hw_spi_ptr->isr.handler_fn != NULL) {
        uint32_t prio = (uint32_t)hw_spi_ptr->isr.prio;

        //reg->IER = (0x01ul << 8);
        // TODO: add spi interrupt support
        //NVIC_SetPriority(spi_const->irqn.spi, prio);

        // Because the hardware only support up to 64 bytes at a time
        // So the DMA interrupt of spi needs to be enable for the relay function
        NVIC_SetPriority(spi_const->irqn.dma_rx, prio);
        NVIC_SetPriority(spi_const->irqn.dma_tx, prio);
        NVIC_EnableIRQ(spi_const->irqn.dma_rx);
        NVIC_EnableIRQ(spi_const->irqn.dma_tx);
    } else {
        //NVIC_DisableIRQ(spi_const->irqn.spi);
        NVIC_DisableIRQ(spi_const->irqn.dma_rx);
        NVIC_DisableIRQ(spi_const->irqn.dma_tx);
    }

    return VSF_ERR_NONE;
}

void vsf_hw_spi_fini(vsf_hw_spi_t *hw_spi_ptr)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    // TODO: close all clock
}

fsm_rt_t vsf_hw_spi_enable(vsf_hw_spi_t *hw_spi_ptr)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_spi_disable(vsf_hw_spi_t *hw_spi_ptr)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);

    return fsm_rt_cpl;
}

void vsf_hw_spi_irq_enable(vsf_hw_spi_t *hw_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);

    hw_spi_ptr->irq_mask |= irq_mask;
}

void vsf_hw_spi_irq_disable(vsf_hw_spi_t *hw_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);

    hw_spi_ptr->irq_mask &= ~irq_mask;
}

#if 0
// TODO : add spi fifi support
static vsf_err_t __hw_spi_is_send_full(vsf_hw_spi_t *hw_spi_ptr)
{
    return true;
}

static vsf_err_t __hw_spi_is_recv_emtpy(vsf_hw_spi_t *hw_spi_ptr)
{
    return true;
}
#endif

void vsf_hw_spi_fifo_transfer(vsf_hw_spi_t *hw_spi_ptr,
                              void *out_buffer_ptr,
                              uint_fast32_t* out_offset_ptr,
                              void *in_buffer_ptr,
                              uint_fast32_t* in_offset_ptr,
                              uint_fast32_t cnt)

{
    VSF_HAL_ASSERT(0);

#if 0
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    uint32_t byte_cnt = SPI_DATASIZE_TO_BYTE(reg->CR[0]);
    uint8_t *out_buf = (uint8_t *)out_buffer_ptr;
    uint8_t *in_buf = (uint8_t *)in_buffer_ptr;
    uint_fast32_t out_offset = 0;
    uint_fast32_t in_offset = 0;

    bool is_continue = false;
    do {
        uint32_t value = 0;

        if (out_offset < out_cnt && __hw_spi_is_send_full(hw_spi_ptr)) {
            if (out_buf != NULL) {
                memcpy(&value, out_buf + out_offset * byte_cnt, byte_cnt);
            }
            reg->IOR = value;
            out_offset++;

            is_continue = true;
        }

        if (in_offset < in_cnt && __hw_spi_is_recv_emtpy(hw_spi_ptr)) {
            uint32_t value = reg->IOR;
            if (in_buf != NULL) {
                memcpy(in_buf + in_offset * byte_cnt, &value, byte_cnt);
            }
            in_offset++;

            is_continue = true;
        }
    } while (is_continue);

    if (out_offset_ptr != NULL) {
        *out_offset_ptr = out_offset;
    }
    if (in_offset_ptr != NULL) {
        *in_offset_ptr = in_offset;
    }
#endif
}

static void __request_dma(int channel, int cid_num,
                          uint32_t count, uint32_t src_addr, uint32_t dst_addr,
                          uint32_t src_size, uint32_t dst_size,
                          bool src_const_addr, bool dst_const_addr)
{
    static const uint8_t __ahb_cnts[] = {AHB_BYTE, AHB_BYTE, AHB_HWORD, AHB_WORD, AHB_WORD};

    uint32_t tbl_val = (__ahb_cnts[dst_size] << DMA_CH_DBUSU_LSB) | (__ahb_cnts[src_size] << DMA_CH_SBUSU_LSB);
    uint32_t tsr_value = (dst_size << DMA_CH_DTRANSZ_LSB) | (src_size << DMA_CH_STRANSZ_LSB);
    if (src_const_addr) {
        tbl_val |= DMA_CH_CONSTSA_BIT;
    }
    if (dst_const_addr) {
        tbl_val |= DMA_CH_CONSTDA_BIT;
    }

    dma_erqcsr_set(cid_num, channel);
    dma_ch_rqr_erqm_clrb(channel);
    dma_ch_rqr_erql_setb(channel);
    dma_ch_dar_set(channel, dst_addr);
    dma_ch_sar_set(channel, src_addr);
    dma_ch_tbl0cr_set(channel, 1 | (REQ_FRAG << DMA_CH_RQTYP_LSB) | tbl_val);
    dma_ch_tbl1cr_set(channel, count);
    dma_ch_tbl2cr_set(channel, count);
    dma_ch_tsr_set(channel, tsr_value);
    dma_ch_wmar_set(channel, 0);
    dma_ch_wjar_set(channel, 0);
    dma_ch_lnar_set(channel, 0);
    dma_ch_tbl0sr_set(channel, ((0 << DMA_CH_STBL0SZ_LSB) | (0 << DMA_CH_DTBL0SZ_LSB)));
    dma_ch_tbl1ssr_set(channel, 0);
    dma_ch_tbl1dsr_set(channel, 0);
    dma_ch_icsr_set(channel, (DMA_CH_TBL2_IENA_BIT | DMA_CH_CE_IENA_BIT));
    dma_ch_ctlr_set(channel, (DMA_CH_CHENA_BIT | (0x01ul << DMA_CH_BUSBU_LSB)));
}

static void __spi_request_transfer(vsf_hw_spi_t *hw_spi_ptr)
{
    static uint32_t __dummy;

    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    VSF_HAL_ASSERT(hw_spi_ptr->request.count > hw_spi_ptr->request.recv.offset);

    uint32_t count = vsf_min(hw_spi_ptr->request.count - hw_spi_ptr->request.recv.offset,
                         VSF_HW_SPI_CFG_DMA_BYTE_CNT_MAX);
    uint32_t byte_cnt = SPI_DATASIZE_TO_BYTE(reg->CR[0]);

    bool recv_const_addr;
    uint8_t *recv_buffer = hw_spi_ptr->request.recv.buffer;
    if (recv_buffer != NULL) {
        recv_const_addr = false;
        recv_buffer += hw_spi_ptr->request.recv.offset * byte_cnt;
    } else {
        recv_const_addr = true;
        recv_buffer = (uint8_t *)&__dummy;
    }
    __request_dma(spi_const->request.recv.channel,
                  spi_const->request.recv.cid,
                  count, (uint32_t)&reg->IOR, (uint32_t)recv_buffer,
                  sizeof(reg->IOR), byte_cnt, true, recv_const_addr);

    bool send_const_addr;
    uint8_t *send_buffer = hw_spi_ptr->request.send.buffer;
    if (send_buffer != NULL) {
        send_const_addr = false;
        send_buffer += hw_spi_ptr->request.send.offset * byte_cnt;
    } else {
        send_buffer = (uint8_t *)&__dummy;
        send_const_addr = true;
    }
    __request_dma(spi_const->request.send.channel,
                  spi_const->request.send.cid,
                  count, (uint32_t)send_buffer, (uint32_t)(&reg->IOR),
                  byte_cnt, sizeof(reg->IOR), send_const_addr, true);

    reg->OCR = count;           // out data cnt
    reg->ICR = count;           // in data cnt
    reg->TCR = (0x01ul << 1);   // trans start
}

static void __irq_handler(vsf_hw_spi_t *hw_spi_ptr, vsf_spi_irq_mask_t irq_mask)
{
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    vsf_spi_irq_mask_t cb_irq_mask = 0;

    if (irq_mask & VSF_SPI_IRQ_MASK_CPL) {
        const int ch = spi_const->request.recv.channel;
        dma_ch_icsr_set(ch, (dma_ch_icsr_get(ch) | DMA_CH_TBL2_ICLR_BIT | DMA_CH_CE_ICLR_BIT));

        hw_spi_ptr->request.recv.offset = vsf_min(hw_spi_ptr->request.recv.offset + VSF_HW_SPI_CFG_DMA_BYTE_CNT_MAX,
                                              hw_spi_ptr->request.count);
        VSF_HAL_ASSERT(hw_spi_ptr->request.count >= hw_spi_ptr->request.recv.offset);

        if (hw_spi_ptr->request.count == hw_spi_ptr->request.recv.offset) {
            cb_irq_mask = VSF_SPI_IRQ_MASK_CPL;

            if (hw_spi_ptr->is_auto_cs) {
                vsf_hw_spi_cs_inactive(hw_spi_ptr, 0);
            }
        } else {
            __spi_request_transfer(hw_spi_ptr);
        }
    }

    if (irq_mask & VSF_SPI_IRQ_MASK_TX_CPL) {
        const int ch = spi_const->request.send.channel;
        dma_ch_icsr_set(ch, (dma_ch_icsr_get(ch) | DMA_CH_TBL2_ICLR_BIT | DMA_CH_CE_ICLR_BIT));

         hw_spi_ptr->request.send.offset = vsf_min(hw_spi_ptr->request.send.offset + VSF_HW_SPI_CFG_DMA_BYTE_CNT_MAX,
                                               hw_spi_ptr->request.count);
        if (hw_spi_ptr->request.count == hw_spi_ptr->request.send.offset) {
            cb_irq_mask = VSF_SPI_IRQ_MASK_TX_CPL;
        }
    }

    cb_irq_mask &= hw_spi_ptr->irq_mask;
    if (cb_irq_mask) {
        if (NULL != hw_spi_ptr->isr.handler_fn) {
            hw_spi_ptr->isr.handler_fn(hw_spi_ptr->isr.target_ptr, (vsf_spi_t *)hw_spi_ptr, cb_irq_mask);
        }
    }
}

vsf_err_t vsf_hw_spi_request_transfer(vsf_hw_spi_t *hw_spi_ptr,
                                      void         *send_buffer_ptr,
                                      void         *recv_buffer_ptr,
                                      uint_fast32_t count)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    hw_spi_ptr->request.count = count;
    hw_spi_ptr->request.send.offset = 0;
    hw_spi_ptr->request.send.buffer = send_buffer_ptr;
    hw_spi_ptr->request.recv.offset = 0;
    hw_spi_ptr->request.recv.buffer = recv_buffer_ptr;

    if (hw_spi_ptr->is_auto_cs) {
        vsf_hw_spi_cs_active(hw_spi_ptr, 0);
    }

    __spi_request_transfer(hw_spi_ptr);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_cancel_transfer(vsf_hw_spi_t *hw_spi_ptr)
{
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_hw_spi_cs_active(vsf_hw_spi_t *hw_spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    reg->CR[0] &= ~(1 << 8);

    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_spi_cs_inactive(vsf_hw_spi_t *hw_spi_ptr, uint_fast8_t index)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;
    VSF_HAL_ASSERT(reg != NULL);

    reg->CR[0] |= (1 << 8);

    return VSF_ERR_NONE;
}

vsf_spi_status_t vsf_hw_spi_status(vsf_hw_spi_t *hw_spi_ptr)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);
    const vsf_hw_spi_const_t *spi_const = hw_spi_ptr->spi_const;
    VSF_HAL_ASSERT(spi_const != NULL);
    REG_SPI_T *reg = spi_const->reg;

    vsf_spi_status_t status;
    status.is_busy = (reg->OCR != 0) || (reg->ICR != 0);

    return status;
}

void vsf_hw_spi_get_transferred_count(vsf_hw_spi_t *hw_spi_ptr, uint_fast32_t * tx_count, uint_fast32_t *rx_count)
{
    VSF_HAL_ASSERT(hw_spi_ptr != NULL);

    if (tx_count != NULL) {
        *tx_count = hw_spi_ptr->request.send.offset;
    }

    if (rx_count != NULL) {
        *rx_count = hw_spi_ptr->request.recv.offset;
    }
}

vsf_spi_capability_t vsf_hw_spi_capability(vsf_hw_spi_t *spi_ptr)
{
    uint32_t pclk = sysctrl_clock_get(SYS_PCLK);

    vsf_spi_capability_t spi_capability = {
        .support_hardware_cs = 1,
        .support_software_cs = 1,
        .cs_count = 1,
        .max_clock_hz = pclk / 2,
        .min_clock_hz = pclk / 2 / 0x10000,
    };

    return spi_capability;
}

/*============================ INCLUDES ======================================*/

#define VSF_SPI_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_SPI_CFG_IMP_PREFIX                  vsf_hw
#define VSF_SPI_CFG_IMP_UPCASE_PREFIX           VSF_HW
#define VSF_SPI_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    static const vsf_hw_spi_const_t __vsf_hw_spi ## __IDX ## _const = {         \
        .reg = REG_SPI ## __IDX,                                                \
        .irqn = {                                                               \
            .spi    = VSF_HW_SPI ## __IDX ## _IRQ_IDX,                          \
            .dma_rx = VSF_HW_SPI ## __IDX ## _RXDMA_IRQ_IDX,                    \
            .dma_tx = VSF_HW_SPI ## __IDX ## _TXDMA_IRQ_IDX,                    \
        },                                                                      \
        .request = {                                                            \
            .send = {                                                           \
                .channel = VSF_HW_SPI ## __IDX ## _TXDMA_CH_IDX,                \
                .cid     = VSF_HW_SPI ## __IDX ## _TXDMA_CID,                   \
            },                                                                  \
            .recv = {                                                           \
                .channel = VSF_HW_SPI ## __IDX ## _RXDMA_CH_IDX,                \
                .cid     = VSF_HW_SPI ## __IDX ## _RXDMA_CID,                   \
            },                                                                  \
        },                                                                      \
        .clock = {                                                              \
            .hclk = VSF_HW_SPI ## __IDX ## _HCLKME_EN_BIT,                      \
            .oclk = VSF_HW_SPI ## __IDX ## _OCLKME_EN_BIT,                      \
            .pclk = VSF_HW_SPI ## __IDX ## _PCLKME_EN_BIT,                      \
        },                                                                      \
    };                                                                          \
    vsf_hw_spi_t vsf_hw_spi ## __IDX = {                                        \
        .spi_const = &__vsf_hw_spi ## __IDX ## _const,                          \
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_HW_SPI ## __IDX ## _RXDMA_IRQ(void)                                \
    {                                                                           \
        __irq_handler(&vsf_hw_spi ## __IDX, VSF_SPI_IRQ_MASK_CPL);              \
    }                                                                           \
    void VSF_HW_SPI ## __IDX ## _TXDMA_IRQ(void)                                \
    {                                                                           \
        __irq_handler(&vsf_hw_spi ## __IDX, VSF_SPI_IRQ_MASK_TX_CPL);           \
    }

#include "hal/driver/common/spi/spi_template.inc"

#endif      // VSF_HAL_USE_SPI
