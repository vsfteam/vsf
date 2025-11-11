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

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_DMA_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${DMA_IP}_DMA_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_DMA_CFG_MULTI_CLASS
#   define VSF_HW_DMA_CFG_MULTI_CLASS           VSF_DMA_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_DMA_CFG_IMP_PREFIX                  vsf_hw
#define VSF_DMA_CFG_IMP_UPCASE_PREFIX           VSF_HW
// HW end

/*============================ TYPES =========================================*/

typedef struct VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) {
    vsf_dma_isr_t                               isr;
    uint32_t                                    total_count;
    uint32_t                                    remain_count;
    uint16_t                                    cur_count;
    uint8_t                                     irqn;
} VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t);

// HW
typedef struct VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) {
#if VSF_HW_DMA_CFG_MULTI_CLASS == ENABLED
    vsf_dma_t                                   vsf_dma;
#endif
    dma_type                                    *reg;
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t)
                                                channels[VSF_HW_DMA_CHANNEL_NUM];
    vsf_hw_peripheral_en_t                      en;
    vsf_hw_peripheral_rst_t                     rst;
    uint8_t                                     channel_mask;
} VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t);
// HW end


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_dma_channel_reg_t {
    dma_channel_type;
    __IO uint32_t reserved1;
} vsf_hw_dma_channel_reg_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw dma only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_init)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    dma_ptr->reg->muxsel_bit.tblsel = 1;
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
}

int8_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_request)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_channel_hint_t *channel_hint_ptr
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);
    int8_t channel;

    vsf_protect_t orig = vsf_protect_int();
        channel = vsf_ffz32(dma_ptr->channel_mask);
        if (channel >= VSF_HW_DMA_CHANNEL_NUM) {
            channel = -1;
        }
        if (channel > 0) {
            dma_ptr->channel_mask |= 1 << channel;
        }
    vsf_unprotect_int(orig);

    if (channel > 0) {
        NVIC_SetPriority(dma_ptr->channels[channel].irqn, channel_hint_ptr->interrupt_prio);
    }
    return channel;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_release)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    vsf_protect_t orig = vsf_protect_int();
        dma_ptr->channel_mask &= ~(1 << channel);
    vsf_unprotect_int(orig);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    dmamux_channel_type *mux_channel_reg = &((dmamux_channel_type *)reg->reserved2)[channel];
    channel_reg->ctrl = 0;
    mux_channel_reg->muxctrl = 0;
    NVIC_DisableIRQ(dma_ptr->channels[channel].irqn);
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_config)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel,
    vsf_dma_channel_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);
    VSF_HAL_ASSERT(cfg_ptr != NULL);
    VSF_HAL_ASSERT(cfg_ptr->dst_idx <= 127);
    VSF_HAL_ASSERT(cfg_ptr->src_idx <= 127);
    VSF_HAL_ASSERT(cfg_ptr->sync_reqcnt <= 32);
    VSF_HAL_ASSERT(cfg_ptr->sync_signal <= 127);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    dmamux_channel_type *mux_channel_reg = &((dmamux_channel_type *)reg->reserved2)[channel];
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    vsf_dma_channel_mode_t mode = cfg_ptr->mode;
    switch (mode & VSF_DMA_DIRECTION_MASK) {
    case VSF_DMA_MEMORY_TO_PERIPHERAL:
        mode = ((mode & (1 << 6)) << 1) | ((mode & (1 << 7)) >> 1) | (mode & ~(3 << 6));
        mode = ((mode & (3 << 8)) << 2) | ((mode & (3 << 10)) >> 3) | (mode & ~(15 << 8));
        mux_channel_reg->muxctrl = cfg_ptr->dst_idx;
        break;
    case VSF_DMA_PERIPHERAL_TO_MEMORY:
        mux_channel_reg->muxctrl = cfg_ptr->src_idx;
        break;
    default:
        break;
    }
    mux_channel_reg->muxctrl |= ((cfg_ptr->mode >> 16) << 8) | (cfg_ptr->sync_reqcnt << 19) | (cfg_ptr->sync_signal << 24);
    channel_reg->ctrl = cfg_ptr->mode;
    ch->isr = cfg_ptr->isr;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_EnableIRQ(ch->irqn);
    } else {
        NVIC_DisableIRQ(ch->irqn);
    }

    return VSF_ERR_NONE;
}

static bool VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_channel_update)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch,
    vsf_hw_dma_channel_reg_t *channel_reg
) {
    if (ch->remain_count > 0) {
        ch->cur_count = vsf_min(ch->remain_count, 0xFFFF);
        channel_reg->dtcnt = ch->cur_count;
        ch->remain_count -= ch->cur_count;
        channel_reg->ctrl_bit.chen = 1;
        return false;
    }
    return true;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_start)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel,
    uint32_t src_address,
    uint32_t dst_address,
    uint32_t count
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);
    VSF_HAL_ASSERT(count > 0);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    ch->total_count = ch->remain_count = count;
    if (channel_reg->ctrl_bit.dtd) {
        channel_reg->paddr = src_address;
        channel_reg->maddr = dst_address;
    } else {
        channel_reg->maddr = src_address;
        channel_reg->paddr = dst_address;
    }
    reg->clr = 1 << (channel << 2);
    VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_channel_update)(ch, channel_reg);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_cancel)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];

    channel_reg->ctrl_bit.chen = 0;
    return VSF_ERR_NONE;
}

uint32_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_get_transferred_count)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    return ch->total_count - ch->remain_count + (ch->cur_count - channel_reg->dtcnt);
}

vsf_dma_channel_status_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_status)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel)
{
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    return (vsf_dma_channel_status_t) {
        .is_busy = !!(channel_reg->ctrl_bit.chen && !(reg->sts & (2 << (channel << 2)))),
    };
}

static vsf_dma_irq_mask_t VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_mask)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    // implement this function in the device file
    VSF_HAL_ASSERT(0);
    return 0;
}

static void VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);

    vsf_dma_irq_mask_t irq_mask = VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_get_irq_mask)(dma_ptr);
//    vsf_dma_isr_t *isr_ptr = &dma_ptr->isr;
//    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
//        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_dma_t *)dma_ptr, channel, irq_mask);
//    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in dma_template.inc.
 *      VSF_DMA_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_DMA_CFG_REIMPLEMENT_API_GET_CONFIGURATION for dma_get_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY for dma_capability.
 *          Default implementation will use macros below to initialize capability structure:
 *              VSF_DMA_CFG_CAPABILITY_IRQ_MASK (default: VSF_DMA_IRQ_ALL_BITS_MASK)
 *              VSF_DMA_CFG_CAPABILITY_MAX_REQUEST_COUNT (default: 0xFFFFFFFF)
 *              VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT (default: 8)
 *      VSF_DMA_CFG_REIMPLEMENT_API_CHANNEL_GET_CONFIGURATION for dma_channel_get_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_DMA_CFG_REIMPLEMENT_API_SG_CONFIG_DESC for dma_channel_sg_config_desc.
 *          Default implementation will return VSF_ERR_NOT_SUPPORT.
 *          This implementation provides basic template that returns VSF_ERR_NOT_SUPPORT.
 *      VSF_DMA_CFG_REIMPLEMENT_API_SG_START for dma_channel_sg_start.
 *          Default implementation will return VSF_ERR_NOT_SUPPORT.
 *          This implementation provides basic template that returns VSF_ERR_NOT_SUPPORT.
 */

vsf_dma_capability_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_capability)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    return (vsf_dma_capability_t) {
        .irq_mask          = 0xff,
        .max_request_count = 8,
        .channel_count     = 16
    };
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw dma only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_DMA_CFG_REIMPLEMENT_API_CAPABILITY                  ENABLED

#define VSF_DMA_IMP_IRQHANDLER(__CHANNEL_IDX, __DMA_IDX)                        \
    void VSF_MCONNECT(DMA, __DMA_IDX, _Channel, __CHANNEL_IDX, _IRQHandler)(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(              \
            &VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __DMA_IDX),             \
            __CHANNEL_IDX                                                       \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#define VSF_DMA_IMP_CHANNEL(__CHANNEL_IDX, __DMA_IDX)                           \
        [__CHANNEL_IDX] = {                                                     \
            .irqn = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __DMA_IDX, _CHANNEL, __CHANNEL_IDX, _IRQN),\
        },

#define VSF_DMA_CFG_IMP_LV0(__IDX, __HAL_OP)                                    \
    VSF_MREPEAT(VSF_HW_DMA_CHANNEL_NUM, VSF_DMA_IMP_IRQHANDLER, __IDX)          \
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t)                                \
        VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma, __IDX) = {                   \
        .reg        = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _REG),\
        .en         = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _EN),\
        .rst        = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _RST),\
        .channels = {                                                           \
            VSF_MREPEAT(VSF_HW_DMA_CHANNEL_NUM, VSF_DMA_IMP_CHANNEL, __IDX)     \
        },                                                                      \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/dma/dma_template.inc"
// HW end

#endif /* VSF_HAL_USE_DMA */

