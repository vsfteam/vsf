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

#if VSF_HAL_USE_USART == ENABLED

#include "hal/vsf_hal.h"

// HW
// for vendor headers
#include "hal/driver/vendor_driver.h"
// HW end

/*============================ MACROS ========================================*/

/*\note VSF_HW_USART_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_${USART_IP}_USART_CFG_MULTI_CLASS in header file.
 */

// HW
#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS         VSF_USART_CFG_MULTI_CLASS
#endif
// HW end

// HW
#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// HW
typedef struct VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) {
#if VSF_HW_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t             vsf_usart;
#endif
    usart_type              *reg;
    vsf_usart_isr_t         isr;
    const vsf_hw_clk_t      *clk;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_en_t  rst;
     vsf_usart_irq_mask_t   irq_mask;
    uint8_t                 irqn;
} VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t);
// HW end

/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw usart only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);

    usart_type *reg = usart_ptr->reg;

    vsf_hw_peripheral_enable(usart_ptr->en);

    // baudrate
    uint32_t clk_hz = vsf_hw_clk_get_freq_hz(usart_ptr->clk);
    if (!clk_hz) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_BUG;
    }
    clk_hz /= cfg_ptr->baudrate;
    if (clk_hz < 16 || clk_hz >= 0x10000) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }

    uint32_t enabled = reg->ctrl1_bit.uen;
    if (enabled) {
        reg->ctrl1_bit.uen = 0;
    }
    reg->ctrl1 = (reg->ctrl1 & ~__VSF_HW_USART_CTRL1_MASK) | (cfg_ptr->mode & __VSF_HW_USART_CTRL1_MASK);
    reg->ctrl2 =    (reg->ctrl2 & ~(__VSF_HW_USART_CTRL2_MASK >> __VSF_HW_USART_CTRL2_SHIFT_BITS))
                |   ((cfg_ptr->mode & __VSF_HW_USART_CTRL2_MASK) >> __VSF_HW_USART_CTRL2_SHIFT_BITS);
    reg->ctrl3 =    (reg->ctrl3 & ~(__VSF_HW_USART_CTRL3_MASK << __VSF_HW_USART_CTRL3_SHIFT_BITS))
                |   ((cfg_ptr->mode & __VSF_HW_USART_CTRL3_MASK) << __VSF_HW_USART_CTRL3_SHIFT_BITS);
    reg->rtov = cfg_ptr->rx_timeout;
    reg->baudr = clk_hz;
    if (enabled) {
        reg->ctrl1_bit.uen = 1;
    }

    // configure according to cfg_ptr
    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    usart_ptr->isr = *isr_ptr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_ptr->irqn);
    } else {
        NVIC_DisableIRQ(usart_ptr->irqn);
    }
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_fini)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    vsf_hw_peripheral_rst_set(usart_ptr->rst);
    vsf_hw_peripheral_rst_clear(usart_ptr->rst);
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    usart_ptr->reg->ctrl1_bit.uen = 1;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    usart_ptr->reg->ctrl1_bit.uen = 0;
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_USART_IRQ_ALL_BITS_MASK) == 0);

    usart_type *reg = usart_ptr->reg;
    reg->ctrl1 =    (reg->ctrl1 & ~__VSF_HW_USART_IRQ_MASK_CTRL1)
                |   (irq_mask & __VSF_HW_USART_IRQ_MASK_CTRL1);
    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        reg->ctrl1_bit.retodie = 1;
        reg->ctrl1_bit.rtoden = 1;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_CTS) {
        reg->ctrl3_bit.ctscfien = 1;
    }
    if (irq_mask & (VSF_USART_IRQ_MASK_FRAME_ERR | VSF_USART_IRQ_MASK_NOISE_ERR | VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR)) {
        reg->ctrl3_bit.errien = 1;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR) {
        reg->ctrl1_bit.perrien = 1;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_BREAK_ERR) {
        reg->ctrl2_bit.bfien = 1;
    }
    usart_ptr->irq_mask |= irq_mask;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT((irq_mask & ~VSF_USART_IRQ_ALL_BITS_MASK) == 0);

    usart_ptr->irq_mask &= ~irq_mask;

    usart_type *reg = usart_ptr->reg;
    reg->ctrl1 = (reg->ctrl1 & ~(irq_mask & __VSF_HW_USART_IRQ_MASK_CTRL1));
    if (irq_mask & VSF_USART_IRQ_MASK_RX_TIMEOUT) {
        reg->ctrl1_bit.retodie = 0;
        reg->ctrl1_bit.rtoden = 0;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_CTS) {
        reg->ctrl3_bit.ctscfien = 0;
    }
    if (!(usart_ptr->irq_mask & (VSF_USART_IRQ_MASK_FRAME_ERR | VSF_USART_IRQ_MASK_NOISE_ERR | VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR))) {
        reg->ctrl3_bit.errien = 0;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR) {
        reg->ctrl1_bit.perrien = 0;
    }
    if (irq_mask & VSF_USART_IRQ_MASK_BREAK_ERR) {
        reg->ctrl2_bit.bfien = 0;
    }
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    usart_type *reg = usart_ptr->reg;
    return (vsf_usart_status_t) {
        .is_busy                = !(reg->sts & (VSF_USART_IRQ_MASK_TX_IDLE | VSF_USART_IRQ_MASK_RX_IDLE)),
        .is_tx_busy             = !(reg->sts & VSF_USART_IRQ_MASK_TX_IDLE),
        .is_rx_busy             = !(reg->sts & VSF_USART_IRQ_MASK_RX_IDLE),
        .break_sent             = !reg->ctrl1_bit.sbf,
        .tx_fifo_level          = !(reg->sts & VSF_USART_IRQ_MASK_TX),
        .rx_fifo_level          = !!(reg->sts & VSF_USART_IRQ_MASK_RX),
    };
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return usart_ptr->reg->sts_bit.rdbf ? 1 : 0;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    usart_type *reg = usart_ptr->reg;
    if (reg->sts_bit.rdbf) {
        *(uint8_t *)buffer_ptr = reg->dt;
        return 1;
    }
    return 0;
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return usart_ptr->reg->sts_bit.tdbe ? 1 : 0;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    usart_type *reg = usart_ptr->reg;
    if (reg->sts_bit.tdbe) {
        reg->dt = *(uint8_t *)buffer_ptr;
        return 1;
    }
    return 0;
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    vsf_usart_isr_t *isr_ptr = &usart_ptr->isr;
    vsf_usart_irq_mask_t irq_mask = VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)(usart_ptr, VSF_USART_IRQ_ALL_BITS_MASK);
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)usart_ptr, irq_mask);
    }
}

/*\note Implementation of APIs below is optional, because there is default implementation in usart_template.inc.
 *      VSF_USART_CFG_REIMPLEMENT_API_XXXX can be defined to ENABLED to re-write the default implementation for better performance.
 *
 *      The list of APIs and configuration:
 *      VSF_USART_CFG_REIMPLEMENT_API_REQUEST for usart_request_rx, usart_request_tx, usart_cancel_rx, usart_cancel_tx, usart_get_rx_count, usart_get_tx_count.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY for usart_capability.
 *          Default implementation will use macros below to initialize capability structure:
 *              VSF_USART_CFG_CAPABILITY_IRQ_MASK
 *              VSF_USART_CFG_CAPABILITY_MAX_BAUDRATE
 *              VSF_USART_CFG_CAPABILITY_MIN_BAUDRATE
 *              VSF_USART_CFG_CAPABILITY_TXFIFO_DEPTH
 *              VSF_USART_CFG_CAPABILITY_RXFIFO_DEPTH
 *              VSF_USART_CFG_CAPABILITY_MAX_DATA_BITS
 *              VSF_USART_CFG_CAPABILITY_MIN_DATA_BITS
 *              VSF_USART_CFG_CAPABILITY_SUPPORT_RX_TIMEOUT
 *              VSF_USART_CFG_CAPABILITY_SUPPORT_SEND_BREAK
 *              VSF_USART_CFG_CAPABILITY_SUPPORT_SET_AND_CLEAR_BREAK
 *      VSF_USART_CFG_REIMPLEMENT_API_CTRL for usart_ctrl.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_USART_CFG_REIMPLEMENT_API_GET_CONFIGURATION for usart_get_configuration.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 *      VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR for usart_irq_clear.
 *          Default implementation will assert(false) to indicate the feature is not implemented.
 */

/*\note DMA APIs below.
 *      If DMA is not supported, fifo2req template can be used,
 *          which will use fifo APIs + interrupt to implement DMA APIs.
 */

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NOT_SUPPORT;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_rx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return 0;
}

int_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_get_tx_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return 0;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    uint32_t clk_hz = vsf_hw_clk_get_freq_hz(usart_ptr->clk);
    return (vsf_usart_capability_t) {
        .irq_mask                    = VSF_USART_IRQ_ALL_BITS_MASK,
        .max_baudrate                = clk_hz >> 4,
        .min_baudrate                = clk_hz >> 16,
        .min_data_bits               = 7,
        .max_data_bits               = 8,
        .txfifo_depth                = 1,
        .rxfifo_depth                = 1,
        .support_rx_timeout          = 1,
        .support_send_break          = 1,
        .support_set_and_clear_break = 0,
        .support_sync_clock          = 1,
        .support_tx_idle             = 1,
        .support_rx_idle             = 1,
    };
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_ctrl_t ctrl,
    void * param
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    usart_type *reg = usart_ptr->reg;
    switch (ctrl) {
    case VSF_USART_CTRL_SEND_BREAK:
        reg->ctrl1_bit.sbf = 1;
        break;
    default:
        return VSF_ERR_NOT_SUPPORT;
    }
    return VSF_ERR_NONE;
}

vsf_usart_irq_mask_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_clear)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    usart_type *reg = usart_ptr->reg;
    vsf_usart_irq_mask_t irq_mask_out = reg->sts & irq_mask;

    // DO NOT clear VSF_USART_IRQ_MASK_RX, it should be set/cleared according rxfifo status by hardware.
    reg->sts &= ~irq_mask | VSF_USART_IRQ_MASK_RX;
    reg->ifc |= irq_mask;
    return irq_mask_out;
}

// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw usart only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
// TODO: add comments about fifo2req

#define VSF_USART_CFG_MODE_CHECK_UNIQUE                 VSF_HAL_CHECK_MODE_LOOSE
#define VSF_USART_CFG_IRQ_MASK_CHECK_UNIQUE             VSF_HAL_CHECK_MODE_STRICT
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST           ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL              ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_IRQ_CLEAR         ENABLED
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                            \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {               \
        .reg = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _REG),\
        .en = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _EN),\
        .rst = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _RST),\
        .clk = &VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _CLK),\
        .irqn = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_HW_INTERRUPT, VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _IRQN))(void)\
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(          \
            &VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX)              \
        );                                                                      \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/usart/usart_template.inc"
// HW end

#endif /* VSF_HAL_USE_USART */
