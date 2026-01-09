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

/*\note DMA_CxDTCNT register: 16-bit transfer count register (bits [15:0])
 *      Maximum transfer count: 65535 (0xFFFF)
 *      Reference: RM_AT32F402_405_CH_V2.02 Section 9.5.4
 */
#define VSF_HW_DMA_MAX_TRANSFER_COUNT               0xFFFF

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
    vsf_dma_irq_mask_t                          enabled_irq_mask;
    uint32_t                                    total_count;
    uint32_t                                    remain_count;
    uint16_t                                    cur_count;
    uint8_t                                     irqn;
    vsf_dma_channel_mode_t                      mode;  // Store mode for address mapping in _dma_channel_start
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

    vsf_hw_peripheral_enable(dma_ptr->en);

    dma_ptr->reg->muxsel_bit.tblsel = 1;

    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_fini)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);

    vsf_hw_peripheral_disable(dma_ptr->en);
}

vsf_err_t VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_acquire)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    vsf_dma_channel_hint_t *channel_hint_ptr
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);
    VSF_HAL_ASSERT(channel_hint_ptr != NULL);

    int8_t channel;

    vsf_protect_t orig = vsf_protect_int();
        channel = vsf_ffz32(dma_ptr->channel_mask);
        if (channel >= VSF_HW_DMA_CHANNEL_NUM) {
            channel = -1;
        }
        if (channel >= 0) {
            dma_ptr->channel_mask |= 1 << channel;
        }
    vsf_unprotect_int(orig);

    if (channel >= 0) {
        channel_hint_ptr->channel = channel;
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
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
    VSF_HAL_ASSERT(cfg_ptr->dst_request_idx <= 127);
    VSF_HAL_ASSERT(cfg_ptr->src_request_idx <= 127);
    VSF_HAL_ASSERT(cfg_ptr->sync_reqcnt <= 32);
    VSF_HAL_ASSERT(cfg_ptr->sync_signal <= 127);
    if (cfg_ptr->prio == vsf_arch_prio_invalid) {
        VSF_HAL_ASSERT(0);
        return VSF_ERR_INVALID_PARAMETER;
    }

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    dmamux_channel_type *mux_channel_reg = &((dmamux_channel_type *)reg->reserved2)[channel];
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    channel_reg->ctrl_bit.chen = 0;

    vsf_dma_channel_mode_t mode = cfg_ptr->mode;

    switch (mode & VSF_DMA_DIRECTION_MASK) {
    case VSF_DMA_MEMORY_TO_PERIPHERAL:
        // For M2P: swap address increment bits (6<->7) and width bits (8-9<->10-11)
        // Swap bit 6 and 7: ((mode & (1 << 6)) << 1) | ((mode & (1 << 7)) >> 1)
        // Swap bits 8-9 and 10-11: ((mode & (3 << 8)) << 2) | ((mode & (3 << 10)) >> 2)
        // Fix: original code had >> 3, should be >> 2
        mode = ((mode & (1 << 6)) << 1) | ((mode & (1 << 7)) >> 1) | (mode & ~(3 << 6));
        mode = ((mode & (3 << 8)) << 2) | ((mode & (3 << 10)) >> 2) | (mode & ~(15 << 8));
        mux_channel_reg->muxctrl = cfg_ptr->dst_request_idx;
        break;

    case VSF_DMA_PERIPHERAL_TO_MEMORY:
        mux_channel_reg->muxctrl = cfg_ptr->src_request_idx;
        break;

    default:
        break;
    }

    // Set sync and event generation bits: EVTGEN(9), REQCNT(19..23), SYNCSEL(24..28)
    // mode bit 17 -> muxctrl bit 9 (EVTGEN)
    // sync_reqcnt -> muxctrl bits 19-23 (REQCNT)
    // sync_signal -> muxctrl bits 24-28 (SYNCSEL)
    mux_channel_reg->muxctrl |= ((cfg_ptr->mode >> 16) << 8) | (cfg_ptr->sync_reqcnt << 19) | (cfg_ptr->sync_signal << 24);

    channel_reg->ctrl = mode | cfg_ptr->irq_mask;

    ch->isr = cfg_ptr->isr;
    ch->enabled_irq_mask = cfg_ptr->irq_mask;
    ch->mode = mode;
    if (cfg_ptr->isr.handler_fn != NULL) {
        NVIC_SetPriority(ch->irqn, cfg_ptr->prio);
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
        ch->cur_count = vsf_min(ch->remain_count, VSF_HW_DMA_MAX_TRANSFER_COUNT);
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
    vsf_dma_addr_t src_address,
    vsf_dma_addr_t dst_address,
    uint32_t count
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);
    VSF_HAL_ASSERT(count > 0);

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    channel_reg->ctrl_bit.chen = 0;

    vsf_dma_channel_mode_t mode = ch->mode;
    vsf_dma_channel_mode_t direction = mode & VSF_DMA_DIRECTION_MASK;
    uint32_t paddr, maddr;
    if (direction == VSF_DMA_MEMORY_TO_PERIPHERAL) {
        paddr = (uint32_t)dst_address;
        maddr = (uint32_t)src_address;
    } else if (direction == VSF_DMA_PERIPHERAL_TO_MEMORY) {
        paddr = (uint32_t)src_address;
        maddr = (uint32_t)dst_address;
    } else {
        paddr = (uint32_t)dst_address;
        maddr = (uint32_t)src_address;
    }
    channel_reg->paddr = paddr;
    channel_reg->maddr = maddr;

    ch->total_count = ch->remain_count = count;

    reg->clr |= (uint32_t)(0x0F << (channel << 2));

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

    return ch->remain_count + (ch->cur_count - channel_reg->dtcnt);
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

static vsf_dma_irq_mask_t VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irq_clear)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(dma_ptr != NULL);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    dma_type *reg = dma_ptr->reg;
    uint32_t sts_shift = channel << 2;
    uint32_t sts_mask = reg->sts >> sts_shift;
    vsf_dma_irq_mask_t irq_mask = sts_mask & VSF_DMA_IRQ_ALL_BITS_MASK;

    reg->clr = (uint32_t)(0x0F << sts_shift);

    return irq_mask;
}

static void VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irqhandler)(
    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_t) *dma_ptr,
    uint8_t channel
) {
    VSF_HAL_ASSERT(NULL != dma_ptr);
    VSF_HAL_ASSERT(channel < VSF_HW_DMA_CHANNEL_NUM);

    VSF_MCONNECT(VSF_DMA_CFG_IMP_PREFIX, _dma_channel_t) *ch = &dma_ptr->channels[channel];

    dma_type *reg = dma_ptr->reg;
    vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];

    if (ch->isr.handler_fn == NULL) {
        channel_reg->ctrl_bit.fdtien = 0;
        channel_reg->ctrl_bit.hdtien = 0;
        channel_reg->ctrl_bit.dterrien = 0;
        reg->clr |= (uint32_t)(0x0F << (channel << 2));
        VSF_HAL_ASSERT(0);
        return;
    }

    vsf_dma_irq_mask_t hw_irq_mask = VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_irq_clear)(dma_ptr, channel);

    if (hw_irq_mask == 0) {
        return;
    }

    vsf_dma_irq_mask_t irq_mask = hw_irq_mask & ch->enabled_irq_mask;

    if (irq_mask & (VSF_DMA_IRQ_MASK_CPL | VSF_DMA_IRQ_MASK_ERROR)) {
        dma_type *reg = dma_ptr->reg;
        vsf_hw_dma_channel_reg_t *channel_reg = &((vsf_hw_dma_channel_reg_t *)reg->reserved1)[channel];
        if (ch->remain_count > 0) {
            uint16_t transferred = ch->cur_count - channel_reg->dtcnt;
            if (transferred > ch->remain_count) {
                transferred = ch->remain_count;
            }
            ch->remain_count -= transferred;
        }
        if (irq_mask & VSF_DMA_IRQ_MASK_CPL) {
            ch->remain_count = 0;
            ch->cur_count = 0;
        } else if (ch->remain_count > 0) {
            VSF_MCONNECT(__, VSF_DMA_CFG_IMP_PREFIX, _dma_channel_update)(ch, channel_reg);
        }
    }

    if ((irq_mask != 0) && (ch->isr.handler_fn != NULL)) {
        ch->isr.handler_fn(ch->isr.target_ptr, (vsf_dma_t *)dma_ptr, channel, irq_mask);
    }
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
 *              VSF_DMA_CFG_CAPABILITY_CHANNEL_COUNT (default: 8)
 *              VSF_DMA_CFG_CAPABILITY_MAX_TRANSFER_COUNT (default: 0xFFFFFFFF)
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
        .irq_mask          = VSF_DMA_IRQ_ALL_BITS_MASK,
        .channel_count     = VSF_HW_DMA_CHANNEL_NUM,
        .irq_count         = VSF_HW_DMA_CHANNEL_NUM,
        .supported_modes   = VSF_DMA_MODE_ALL_BITS_MASK |
                             VSF_DMA_EVENT |
                             VSF_DMA_SYNC_RISING |
                             VSF_DMA_SYNC_FALLING |
                             VSF_DMA_SYNC_RISING_AND_FALLING,
        .max_transfer_count = VSF_HW_DMA_MAX_TRANSFER_COUNT,
        .addr_alignment    = 1,
        .support_scatter_gather = 0,
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
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __DMA_IDX, _CHANNEL, __CHANNEL_IDX, _IRQN))(void)\
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
        .reg   = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _REG),\
        .en    = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _EN), \
        .rst   = VSF_MCONNECT(VSF_DMA_CFG_IMP_UPCASE_PREFIX, _DMA, __IDX, _RST),\
        .channels = {                                                           \
            VSF_MREPEAT(VSF_HW_DMA_CHANNEL_NUM, VSF_DMA_IMP_CHANNEL, __IDX)     \
        },                                                                      \
        __HAL_OP                                                                \
    };
#include "hal/driver/common/dma/dma_template.inc"
// HW end

#endif /* VSF_HAL_USE_DMA */

