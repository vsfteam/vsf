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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_DMA == ENABLED

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_DMA_CFG_MULTI_CLASS
#   define VSF_HW_DMA_CFG_MULTI_CLASS              VSF_DMA_CFG_MULTI_CLASS
#endif

#define VSF_DMA_CFG_IMP_PREFIX                     vsf_hw
#define VSF_DMA_CFG_IMP_UPCASE_PREFIX              VSF_HW

/* Per-IRQN initializer for .irqn[] array, used with VSF_MREPEAT in IMP_LV0.
 * Expands to: [N] = VSF_HW_DMAx_IRQN_N, */
#define __VSF_DMA_IRQN_ENTRY(__N, __IDX)                                        \
    [__N] =                                                                     \
        VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _IRQN_##__N),
/* RP2040 DMA TRANS_COUNT register is 24 bits wide (bits 23:0).
 * Source: RP2040 datasheet §2.5.7, DMA_CHx_TRANS_COUNT register description. */
#define VSF_HW_DMA_MAX_TRANSFER_COUNT              ((1u << 24) - 1)

/*============================================================================
 *  RP2040 DMA 驱动实现状态说明
 *  ===========================================================================
 *
 *  硬件能力：
 *    - 1 个 DMA 控制器（dma_hw_t），基地址 DMA_BASE
 *    - 12 个独立通道（VSF_HW_DMA_CHANNEL_NUM）
 *    - 2 条 NVIC IRQ 输出线（DMA_IRQ_0 / DMA_IRQ_1）
 *      每条线有独立的使能（inte0/inte1）、状态（ints0/ints1）寄存器
 *
 *  已实现：
 *    - 两条 IRQ 线的 NVIC 向量表入口均已注册（startup_RP2040.c）
 *    - init() / fini() 对两条 IRQ 线均执行 NVIC 启停
 *    - irqhandler 分为两条独立路径，各处理对应的 irq_ctrl[]
 *    - capability() 报告 irq_count = VSF_HW_DMA0_IRQ_Handler_COUNT（2）
 *
 *  尚未实现（TODO）：
 *    - 通道到 IRQ 线的映射选择。当前所有通道的中断固定路由到
 *      inte0（DMA_IRQ_0），inte1（DMA_IRQ_1）没有通道使用。
 *    - 若要实现，需在 channel_start / channel_config 层增加 irq 索引
 *      选择（例如通过 vsf_dma_channel_cfg_t 新增字段，或驱动内部按
 *      策略分配），并据此写 inte0 或 inte1。
 *============================================================================*/

/*============================ TYPES =========================================*/

typedef struct vsf_hw_dma_channel_t {
    vsf_dma_channel_cfg_t   cfg;
    vsf_dma_isr_t           isr;
    uint32_t                total_count;
    /* Scatter-gather state */
    vsf_dma_channel_sg_desc_t *sg_desc_array;
    uint32_t                sg_count;
    uint32_t                sg_index;
    uint32_t                sg_total_count;
    bool                    is_sg_active;
} vsf_hw_dma_channel_t;

typedef struct VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) {
#if VSF_HW_DMA_CFG_MULTI_CLASS == ENABLED
    vsf_dma_t               vsf_dma;
#endif
    dma_hw_t                *reg;
    uint32_t                rst_bit;
    IRQn_Type               irqn[VSF_HW_DMA0_IRQ_Handler_COUNT];
    vsf_hw_dma_channel_t    channels[VSF_HW_DMA_CHANNEL_NUM];
    uint16_t                channel_mask;
    vsf_dma_cfg_t           cfg;
} VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __rp2040_dma_reset(uint32_t rst_bit)
{
    resets_hw->reset |= rst_bit;
    resets_hw->reset &= ~rst_bit;
    /* Spin-wait: reset deassert → reset_done is a few clk_ref cycles (< 1 us). */
    while (!(resets_hw->reset_done & rst_bit));
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_init)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));

    __rp2040_dma_reset(dma_ptr->rst_bit);

    // no clock gate: RP2040 DMA clock is always-on from system bus

    /* Note: cfg_ptr->prio is NVIC IRQ priority (pre-emption), not DMA channel
     * arbitration priority.  RP2040 DMA channel arbitration is fixed in
     * hardware (lower channel number always wins); it cannot be changed. */
    dma_ptr->cfg = *cfg_ptr;
    dma_ptr->channel_mask = 0;
    for (uint8_t i = 0; i < VSF_HW_DMA_CHANNEL_NUM; i++) {
        dma_ptr->channels[i].total_count = 0;
    }

    for (uint8_t irq_idx = 0; irq_idx < VSF_HW_DMA0_IRQ_Handler_COUNT; irq_idx++) {
        if (cfg_ptr->prio != vsf_arch_prio_invalid) {
            NVIC_SetPriority(dma_ptr->irqn[irq_idx], cfg_ptr->prio);
            NVIC_EnableIRQ(dma_ptr->irqn[irq_idx]);
        } else {
            NVIC_DisableIRQ(dma_ptr->irqn[irq_idx]);
        }
    }

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    for (uint8_t irq_idx = 0; irq_idx < VSF_HW_DMA0_IRQ_Handler_COUNT; irq_idx++) {
        NVIC_DisableIRQ(dma_ptr->irqn[irq_idx]);
    }

    dma_hw_t *hw = dma_ptr->reg;
    /* Spin-wait: abort all channels; each takes a few AHB cycles (< 1 us total). */
    hw->abort = 0xFFF;
    while (hw->abort);
    for (uint8_t i = 0; i < VSF_HW_DMA_CHANNEL_NUM; i++) {
        hw->ch[i].ctrl_trig = 0;
        for (uint8_t irq_idx = 0; irq_idx < VSF_HW_DMA0_IRQ_Handler_COUNT; irq_idx++) {
            hw->irq_ctrl[irq_idx].inte &= ~(1u << i);
        }
    }
    dma_ptr->channel_mask = 0;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_get_configuration)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));

    *cfg_ptr = dma_ptr->cfg;
    return VSF_ERR_NONE;
}

vsf_dma_capability_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    return (vsf_dma_capability_t) {
        .irq_mask           = VSF_DMA_IRQ_MASK_CPL,
        .channel_count      = VSF_HW_DMA_CHANNEL_NUM,
        .irq_count          = VSF_HW_DMA0_IRQ_Handler_COUNT,
        .supported_modes    = VSF_DMA_MEMORY_TO_MEMORY
                            | VSF_DMA_MEMORY_TO_PERIPHERAL
                            | VSF_DMA_PERIPHERAL_TO_MEMORY
                            | VSF_DMA_SRC_ADDR_INCREMENT
                            | VSF_DMA_DST_ADDR_INCREMENT
                            | VSF_DMA_SRC_WIDTH_BYTE_1
                            | VSF_DMA_SRC_WIDTH_BYTES_2
                            | VSF_DMA_SRC_WIDTH_BYTES_4
                            | VSF_DMA_DST_WIDTH_BYTE_1
                            | VSF_DMA_DST_WIDTH_BYTES_2
                            | VSF_DMA_DST_WIDTH_BYTES_4,
        .max_transfer_count = VSF_HW_DMA_MAX_TRANSFER_COUNT,
        .addr_alignment     = 1,
        .support_scatter_gather = 1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_acquire)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_channel_hint_t *channel_hint_ptr)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    uint8_t start = 0;
    uint8_t end = VSF_HW_DMA_CHANNEL_NUM;

    if (channel_hint_ptr != NULL) {
        if (channel_hint_ptr->channel >= 0) {
            start = (uint8_t)channel_hint_ptr->channel;
            end = start + 1;
        }
    }

    for (uint8_t i = start; i < end; i++) {
        if (!(dma_ptr->channel_mask & (1u << i))) {
            dma_ptr->channel_mask |= (1u << i);
            if (channel_hint_ptr != NULL) {
                channel_hint_ptr->channel = (int8_t)i;
            }
            return VSF_ERR_NONE;
        }
    }

    return VSF_ERR_NOT_AVAILABLE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_release)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_hw_t *hw = dma_ptr->reg;
    /* Abort if still active; clearing EN alone pauses (BUSY stays high).
     * Spin-wait: abort completes in a few AHB cycles (< 1 us). */
    hw->abort = (1u << channel);
    while (hw->abort & (1u << channel));
    hw->inte0 &= ~(1u << channel);
    dma_ptr->channel_mask &= ~(1u << channel);
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_config)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_ptr->channels[channel].cfg = *cfg_ptr;
    dma_ptr->channels[channel].isr = cfg_ptr->isr;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_configuration)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT((NULL != dma_ptr) && (NULL != cfg_ptr));
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    *cfg_ptr = dma_ptr->channels[channel].cfg;
    return VSF_ERR_NONE;
}

static void __rp2040_dma_channel_start_raw(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_addr_t src_address,
    vsf_dma_addr_t dst_address, uint32_t count,
    vsf_dma_channel_mode_t mode, bool enable_irq)
{
    dma_hw_t *hw = dma_ptr->reg;
    dma_channel_hw_t *ch = &hw->ch[channel];

    ch->ctrl_trig = 0;
    /* RP2040: clearing EN while BUSY is high PAUSES the channel.
     * To terminate early, CHAN_ABORT must be used (see cancel).
     * If the previous user aborted properly, BUSY is already low. */

    ch->read_addr = (uint32_t)src_address;
    ch->write_addr = (uint32_t)dst_address;
    ch->transfer_count = count;

    uint32_t ctrl = DMA_CH0_CTRL_TRIG_EN_BITS;

    /* Data size */
    switch (mode & VSF_DMA_SRC_WIDTH_MASK) {
    case VSF_DMA_SRC_WIDTH_BYTES_2:
        ctrl |= (1u << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB);
        break;
    case VSF_DMA_SRC_WIDTH_BYTES_4:
        ctrl |= (2u << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB);
        break;
    default:
        /* 8-bit default */
        break;
    }

    /* Address increment */
    switch (mode & (0x03u << 2)) {
    case VSF_DMA_SRC_ADDR_DECREMENT:
    case VSF_DMA_SRC_ADDR_NO_CHANGE:
        break;
    default:
        ctrl |= DMA_CH0_CTRL_TRIG_INCR_READ_BITS;
        break;
    }
    switch (mode & (0x03u << 4)) {
    case VSF_DMA_DST_ADDR_DECREMENT:
    case VSF_DMA_DST_ADDR_NO_CHANGE:
        break;
    default:
        ctrl |= DMA_CH0_CTRL_TRIG_INCR_WRITE_BITS;
        break;
    }

    /* TREQ: 0x3F = permanent request (mem2mem), otherwise DREQ from cfg */
    vsf_dma_channel_cfg_t *cfg = &dma_ptr->channels[channel].cfg;
    switch (mode & VSF_DMA_DIRECTION_MASK) {
    case VSF_DMA_MEMORY_TO_MEMORY:
        ctrl |= (0x3Fu << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    case VSF_DMA_MEMORY_TO_PERIPHERAL:
        ctrl |= ((uint32_t)cfg->dst_request_idx << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    case VSF_DMA_PERIPHERAL_TO_MEMORY:
        ctrl |= ((uint32_t)cfg->src_request_idx << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    default:
        ctrl |= (0x3Fu << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
        break;
    }

    if (enable_irq) {
        /* TODO: 当实现通道到 IRQ 线的映射选择时，此处应根据策略写
         * inte0 或 inte1，而非固定写 inte0。 */
        hw->inte0 |= (1u << channel);
    }

    ch->ctrl_trig = ctrl;
    dma_ptr->channels[channel].total_count = count;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_start)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel, vsf_dma_addr_t src_address,
    vsf_dma_addr_t dst_address, uint32_t count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    vsf_dma_channel_mode_t mode = dma_ptr->channels[channel].cfg.mode;
    vsf_dma_isr_t *isr = &dma_ptr->channels[channel].cfg.isr;
    bool enable_irq = (isr->handler_fn != NULL)
                   && (dma_ptr->channels[channel].cfg.irq_mask & VSF_DMA_IRQ_MASK_CPL);

    __rp2040_dma_channel_start_raw(dma_ptr, channel, src_address, dst_address,
                                   count, mode, enable_irq);

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_cancel)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_hw_t *hw = dma_ptr->reg;
    /* On RP2040, clearing EN while BUSY is high PAUSES the channel; BUSY stays
     * high and the channel resumes if EN is set again.  To terminate early we
     * must use CHAN_ABORT, which clears BUSY and resets the channel state.
     * Spin-wait: abort completes in a few AHB cycles (< 1 us). */
    hw->abort = (1u << channel);
    while (hw->abort & (1u << channel));
    hw->inte0 &= ~(1u << channel);
    dma_ptr->channels[channel].is_sg_active = false;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_sg_config_desc)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel,
    vsf_dma_isr_t isr,
    vsf_dma_channel_sg_desc_t *scatter_gather_cfg,
    uint32_t sg_count)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);
    VSF_HAL_ASSERT(scatter_gather_cfg != NULL);
    VSF_HAL_ASSERT(sg_count > 0);

    if ((NULL == dma_ptr) || (NULL == scatter_gather_cfg) || (sg_count == 0)) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    vsf_hw_dma_channel_t *chp = &dma_ptr->channels[channel];
    chp->sg_desc_array = scatter_gather_cfg;
    chp->sg_count = sg_count;
    chp->isr = isr;

    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_sg_start)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    vsf_hw_dma_channel_t *chp = &dma_ptr->channels[channel];
    if (chp->sg_desc_array == NULL || chp->sg_count == 0) {
        return VSF_ERR_INVALID_PARAMETER;
    }

    chp->sg_index = 0;
    chp->sg_total_count = 0;
    chp->is_sg_active = true;

    vsf_dma_channel_sg_desc_t *desc = &chp->sg_desc_array[0];
    __rp2040_dma_channel_start_raw(dma_ptr, channel,
        desc->src_address, desc->dst_address,
        desc->count, desc->mode, true);

    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_transferred_count)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    uint32_t remain = dma_ptr->reg->ch[channel].transfer_count;
    uint32_t total = dma_ptr->channels[channel].total_count;
    uint32_t current = total - remain;

    if (dma_ptr->channels[channel].is_sg_active) {
        current += dma_ptr->channels[channel].sg_total_count;
    }

    return current;
}

vsf_dma_channel_status_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_status)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    vsf_dma_channel_status_t status = {
        .is_busy = !!(dma_ptr->reg->ch[channel].ctrl_trig
                      & DMA_CH0_CTRL_TRIG_BUSY_BITS),
    };

    return status;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_ctrl)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_ctrl_t ctrl, void *param)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_UNUSED_PARAM(ctrl);
    VSF_UNUSED_PARAM(param);
    VSF_HAL_ASSERT(0);
    return VSF_ERR_NOT_SUPPORT;
}

static void __rp2040_dma_do_irq(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t irq_idx)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);

    dma_hw_t *hw = dma_ptr->reg;
    uint32_t ints = hw->irq_ctrl[irq_idx].ints;

    for (uint8_t ch = 0; ch < VSF_HW_DMA_CHANNEL_NUM; ch++) {
        if (ints & (1u << ch)) {
            /* Clear interrupt by writing to INTS (WC — write 1 to clear). */
            hw->irq_ctrl[irq_idx].ints = (1u << ch);

            /* Ignore interrupts for channels that have already been released.
             * The inte0 bit may have been left set by an old transfer, or
             * a spurious abort-complete interrupt can fire after release. */
            if (!(dma_ptr->channel_mask & (1u << ch))) {
                continue;
            }

            vsf_hw_dma_channel_t *chp = &dma_ptr->channels[ch];

            /* Scatter-gather chaining */
            if (chp->is_sg_active) {
                chp->sg_total_count += chp->total_count;
                if (chp->sg_index + 1 < chp->sg_count) {
                    chp->sg_index++;
                    vsf_dma_channel_sg_desc_t *desc = &chp->sg_desc_array[chp->sg_index];
                    __rp2040_dma_channel_start_raw(dma_ptr, ch,
                        desc->src_address, desc->dst_address,
                        desc->count, desc->mode, true);
                    continue;   /* Don't call user callback until all done */
                } else {
                    chp->is_sg_active = false;
                    /* All descriptors complete — fall through to user callback */
                }
            }

            if (chp->isr.handler_fn != NULL) {
                chp->isr.handler_fn(
                    chp->isr.target_ptr,
                    (vsf_dma_t *)dma_ptr, ch, VSF_DMA_IRQ_MASK_CPL);
            }
        }
    }
}

static void VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr)
{
    __rp2040_dma_do_irq(dma_ptr, 0);
}

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DMA_CFG_MODE_CHECK_UNIQUE                          VSF_HAL_CHECK_MODE_LOOSE
#define VSF_DMA_CFG_IRQ_MASK_CHECK_UNIQUE                      VSF_HAL_CHECK_MODE_STRICT
#define VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY                 ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_GET_CONFIGURATION          ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_CHANNEL_GET_CONFIGURATION  ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_CTRL                       ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_SG_CONFIG_DESC             ENABLED
#define VSF_DMA_CFG_REIMPLEMENT_API_SG_START                   ENABLED

#define VSF_DMA_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)                                \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX) = {                       \
        .reg = (dma_hw_t *)VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA,    \
                                        __IDX, _REG),                           \
        .rst_bit = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX,     \
                                _RST_BIT),                                      \
        .irqn = {VSF_MREPEAT(2, __VSF_DMA_IRQN_ENTRY, __IDX)},                  \
        __HAL_OP};                                                              \
    VSF_CAL_ROOT void VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX,  \
                                   _IRQHandler)(void) {                         \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(              \
            &VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX));                \
        vsf_hal_irq_leave(ctx);                                                 \
    }

#include "hal/driver/common/dma/dma_template.inc"

/* IMP_LV0 宏每个 DMA 实例只生成一个 IRQHandler（对应 IRQ 索引 0）。
 * RP2040 硬件有两条 IRQ 线，因此额外手动定义第二条 handler。
 * 若将来 VSF HAL 模板支持每个实例生成多个 handler，此处可移除。 */
VSF_CAL_ROOT void VSF_HW_DMA0_IRQ_1_Handler(void)
{
    uintptr_t ctx = vsf_hal_irq_enter();
    __rp2040_dma_do_irq(&vsf_hw_dma0, 1);
    vsf_hal_irq_leave(ctx);
}

#endif      /* VSF_HAL_USE_DMA */
/* EOF */
