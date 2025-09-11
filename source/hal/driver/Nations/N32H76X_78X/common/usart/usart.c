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
    USART_Module            *reg;
    const vsf_hw_clk_t      *clk;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    IRQn_Type               irqn;
    bool                    support_sync;
    bool                    is_rxne;
    bool                    is_txnf;
    uint32_t                irq_mask;
    vsf_usart_isr_t         isr;
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

    USART_Module *reg = usart_ptr->reg;
    uint32_t pclk = vsf_hw_clk_get_freq_hz(usart_ptr->clk), intdiv, fradiv;
    uint32_t mode = cfg_ptr->mode;
    bool over8 = !!(mode & VSF_USART_OVERSAMPLE_MASK);
    uint32_t baudval = cfg_ptr->baudrate;
    uint32_t enabled = reg->CTRL1 & USART_CTRL1_UEN, ctrl;

    vsf_hw_peripheral_enable(usart_ptr->en);
    vsf_hw_peripheral_rst_set(usart_ptr->rst);
    vsf_hw_peripheral_rst_clear(usart_ptr->rst);
    usart_ptr->irq_mask = 0;

    // pclk for USART is shared, so do not configure the pclk in usart module.
    // User should have configured the USART clock.
    if (0 == pclk) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    if (over8) {
        /* oversampling by 8, configure the value of USART_BAUD */
        intdiv = ((25 * (pclk / 2)) / baudval);
    } else {
        /* oversampling by 16, configure the value of USART_BAUD */
        intdiv = ((25 * (pclk / 4)) / baudval);
    }
    ctrl = intdiv / 100;
    fradiv = intdiv - (100 * ctrl);
    ctrl <<= 4;
    if (over8) {
        fradiv = ((((fradiv * 8) + 50) / 100)) & ((uint8_t)0x0F);
        if(fradiv == 0x08) {
            ctrl = ctrl + 0x10; 
        } else {
            ctrl |= fradiv;
        }
    } else {
        ctrl += ((((fradiv * 16) + 50) / 100)) & ((uint8_t)0x1F);
    }
    reg->BRCF = ctrl;

    // mode
    if (enabled) {
        reg->CTRL1 &= ~USART_CTRL1_UEN;
    }

#ifdef VSF_USART_IRQ_MASK_RX_IDLE
    if (cfg_ptr->rx_idle_cnt != 0) {
        reg->IFW = cfg_ptr->rx_idle_cnt;
    }
#endif
    reg->RTO = cfg_ptr->rx_timeout;

    ctrl = reg->CTRL1;
    ctrl &= ~__VSF_HW_USART_CTRL1_MASK;
    ctrl |= mode & __VSF_HW_USART_CTRL1_MASK;
    reg->CTRL1 = ctrl | USART_CTRL1_IFCEN;      // use IFW as idle count

    ctrl = reg->CTRL2;
    ctrl &= ~(  (__VSF_HW_USART_CTRL2_MASK ^ __VSF_HW_USART_CTRL2_SHIFT_MASK)
            |   (__VSF_HW_USART_CTRL2_SHIFT_MASK >> __VSF_HW_USART_CTRL2_SHIFT_BITS));
    ctrl |=     (mode & (__VSF_HW_USART_CTRL2_MASK ^ __VSF_HW_USART_CTRL2_SHIFT_MASK))
            |   ((mode & __VSF_HW_USART_CTRL2_SHIFT_MASK) >> __VSF_HW_USART_CTRL2_SHIFT_BITS);
    reg->CTRL2 = ctrl;

    ctrl = reg->CTRL3;
    ctrl &= ~(  (__VSF_HW_USART_CTRL3_MASK ^ __VSF_HW_USART_CTRL3_SHIFT_MASK)
            |   (__VSF_HW_USART_CTRL3_SHIFT_MASK >> __VSF_HW_USART_CTRL3_SHIFT_BITS));
    ctrl |=     (mode & (__VSF_HW_USART_CTRL3_MASK ^ __VSF_HW_USART_CTRL3_SHIFT_MASK))
            |   ((mode & __VSF_HW_USART_CTRL3_SHIFT_MASK) >> __VSF_HW_USART_CTRL3_SHIFT_BITS);
    reg->CTRL3 = ctrl;

    ctrl = reg->FIFO;
    ctrl &= ~(  (__VSF_HW_USART_FIFO_MASK ^ __VSF_HW_USART_FIFO_SHIFT_MASK)
            |   (__VSF_HW_USART_FIFO_SHIFT_MASK >> __VSF_HW_USART_FIFO_SHIFT_BITS));
    mode = (mode & __VSF_HW_USART_FIFO_MASK) >> __VSF_HW_USART_FIFO_SHIFT_BITS;
    if ((mode & USART_FIFO_RXFTCFG_MASK) != USART_FIFO_RXFTCFG_MASK) {
        ctrl |= mode & (VSF_USART_RX_FIFO_THRESHOLD_MASK >> __VSF_HW_USART_FIFO_SHIFT_BITS);
        usart_ptr->is_rxne = false;
    } else {
        usart_ptr->is_rxne = true;
    }
    if ((mode & USART_FIFO_TXFTCFG_MASK) != USART_FIFO_TXFTCFG_MASK) {
        ctrl |= mode & (VSF_USART_TX_FIFO_THRESHOLD_MASK >> __VSF_HW_USART_FIFO_SHIFT_BITS);
        usart_ptr->is_txnf = false;
    } else {
        usart_ptr->is_txnf = true;
    }
    reg->FIFO = ctrl;

    if (enabled) {
        reg->CTRL1 |= USART_CTRL1_UEN;
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
    usart_ptr->reg->CTRL1 |= USART_CTRL1_UEN;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    usart_ptr->reg->CTRL1 &= ~USART_CTRL1_UEN;
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(!(irq_mask & ~__VSF_HW_USART_IRQ_MASK));

    USART_Module *reg = usart_ptr->reg;

    usart_ptr->irq_mask |= irq_mask;
    if ((irq_mask & VSF_USART_IRQ_MASK_RX) && !usart_ptr->is_rxne) {
        irq_mask &= ~VSF_USART_IRQ_MASK_RX;
        reg->FIFO |= USART_FIFO_RXFTIEN;
    }
    if ((irq_mask & VSF_USART_IRQ_MASK_TX) && !usart_ptr->is_txnf) {
        irq_mask &= ~VSF_USART_IRQ_MASK_TX;
        reg->FIFO |= USART_FIFO_TXFTIEN;
    }

    reg->CTRL1 |= irq_mask & __VSF_HW_USART_CTRL1_IRQ_MASK;
    reg->CTRL2 |= irq_mask & __VSF_HW_USART_CTRL2_IRQ_MASK;
    reg->CTRL3 |= irq_mask & __VSF_HW_USART_CTRL3_IRQ_MASK;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(!(irq_mask & ~__VSF_HW_USART_IRQ_MASK));

    USART_Module *reg = usart_ptr->reg;

    usart_ptr->irq_mask &= ~irq_mask;
    if ((irq_mask & VSF_USART_IRQ_MASK_RX) && !usart_ptr->is_rxne) {
        irq_mask &= ~VSF_USART_IRQ_MASK_RX;
        reg->FIFO &= ~USART_FIFO_RXFTIEN;
    }
    if ((irq_mask & VSF_USART_IRQ_MASK_TX) && !usart_ptr->is_txnf) {
        irq_mask &= ~VSF_USART_IRQ_MASK_TX;
        reg->FIFO &= ~USART_FIFO_TXFTIEN;
    }

    reg->CTRL1 &= ~(irq_mask & __VSF_HW_USART_CTRL1_IRQ_MASK);
    reg->CTRL2 &= ~(irq_mask & __VSF_HW_USART_CTRL2_IRQ_MASK);
    reg->CTRL3 &= ~(irq_mask & __VSF_HW_USART_CTRL3_IRQ_MASK);
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (vsf_usart_status_t) {
        .value                  = usart_ptr->reg->STS,
    };
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (usart_ptr->reg->FIFO >> 14) & 0xF;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    USART_Module *reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    uint32_t is16bit = reg->CTRL1 & VSF_USART_9_BIT_LENGTH;
    while ((cnt < count) && (reg->STS & USART_STS_RXDNE)) {
        if (is16bit) {
            *(uint16_t *)buffer_ptr = reg->DAT & USART_DAT_DATV;
            buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
        } else {
            *(uint8_t *)buffer_ptr = reg->DAT;
            buffer_ptr = (void *)((uintptr_t)buffer_ptr + 1);
        }
        cnt++;
    }
    return cnt;
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    // TODO: is fifo depth 8?
    return 8 - ((usart_ptr->reg->FIFO >> 18) & 0xF);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    USART_Module *reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    uint32_t is16bit = reg->CTRL1 & VSF_USART_9_BIT_LENGTH;
    while ((cnt < count) && (reg->STS & USART_STS_TXDE)) {
        if (is16bit) {
            reg->DAT = *(uint16_t *)buffer_ptr & USART_DAT_DATV;
            buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
        } else {
            reg->DAT = *(uint8_t *)buffer_ptr;
            buffer_ptr = (void *)((uintptr_t)buffer_ptr + 1);
        }
        cnt++;
    }
    return cnt;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    return (vsf_usart_capability_t) {
        .irq_mask                    = __VSF_HW_USART_IRQ_MASK,
        .max_baudrate                = 0,
        .min_baudrate                = 0,
        .min_data_bits               = 8,
        .max_data_bits               = 9,
        .txfifo_depth                = 8,
        .rxfifo_depth                = 8,
        .support_rx_timeout          = 1,
        .support_send_break          = 1,
        .support_set_and_clear_break = 0,
    };
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    vsf_usart_irq_mask_t irq_mask = usart_ptr->reg->STS, irq_mask_out = irq_mask & VSF_USART_IRQ_MASK_PARITY_ERR;
    vsf_usart_isr_t *isr_ptr = &usart_ptr->isr;

    if (irq_mask & USART_STS_CTSF) {
        irq_mask_out |= VSF_USART_IRQ_MASK_CTS;
    }
    if (    (usart_ptr->is_rxne && (irq_mask & USART_STS_RXDNE))
        ||  (!usart_ptr->is_rxne && (irq_mask & USART_STS_RXFT))) {
        irq_mask_out |= VSF_USART_IRQ_MASK_RX;
    }
    if (    (usart_ptr->is_txnf && (irq_mask & USART_STS_TXDE))
        ||  (!usart_ptr->is_txnf && (irq_mask & USART_STS_TXFT))) {
        irq_mask_out |= VSF_USART_IRQ_MASK_TX;
    }
    if (irq_mask & USART_STS_IDLEF) {
        irq_mask_out |= VSF_USART_IRQ_MASK_RX_IDLE;
    }
    if (irq_mask & USART_STS_TXC) {
        irq_mask_out |= VSF_USART_IRQ_MASK_TX_IDLE;
    }
    if (irq_mask & USART_STS_RTOF) {
        irq_mask_out |= VSF_USART_IRQ_MASK_RX_TIMEOUT;
    }

    irq_mask_out &= usart_ptr->irq_mask;
    if ((irq_mask_out != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)usart_ptr, irq_mask_out);
    }
}

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
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_request_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_rx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_cancel_tx)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return VSF_ERR_NONE;
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

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_ctrl_t ctrl,
    void * param
) {
    VSF_HAL_ASSERT(!(ctrl & ~__VSF_HW_USART_SUPPORT_CMD_MASK));
    usart_ptr->reg->CTRL1 |= ctrl;
    return VSF_ERR_NONE;
}

// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw usart only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
// TODO: add comments about fifo2req
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST           ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL               ENABLED
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                            \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {               \
        .reg            = (USART_Module *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _REG),\
        .clk            = &VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _CLK),\
        .en             = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _EN),\
        .rst            = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _RST),\
        .support_sync   = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _SYNC),\
        .irqn           = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX,                          \
                      _USART, __IDX, _IRQHandler)(void)                         \
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
