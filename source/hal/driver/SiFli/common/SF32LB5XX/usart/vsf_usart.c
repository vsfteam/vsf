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

/*\note For IPCore drivers, define __VSF_HAL_HW_USART_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_HW_USART_CLASS_INHERIT__ before including vsf_hal.h
 */

#include "hal/vsf_hal.h"

#include "register.h"
#include "usart.h"

/*============================ MACROS ========================================*/

/*\note VSF_HW_USART_CFG_MULTI_CLASS is only for drivers for specified device(hw drivers).
 *      For other drivers, please define VSF_HW_USART_CFG_MULTI_CLASS in header file.
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
    USART_TypeDef           *reg;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    bool                    support_sync;
    bool                    __baudrate_set;
    IRQn_Type               irqn;
    vsf_usart_isr_t         isr;
    uint32_t                __baudrate;
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

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_set_baudrate)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    USART_TypeDef *reg = usart_ptr->reg;
    if (!usart_ptr->__baudrate_set) {
        usart_ptr->__baudrate_set = true;

        uint32_t brr = 48000000 / usart_ptr->__baudrate;
        if ((brr < 0x10) || (reg->CR1 & USART_CR1_OVER8)) {
            brr = (48000000 << 1) / usart_ptr->__baudrate;
            reg->CR1 |= USART_CR1_OVER8;
            reg->MISCR = (reg->MISCR & ~USART_MISCR_SMPLINI_Msk) | (2 << USART_MISCR_SMPLINI_Pos);
        } else {
            reg->CR1 &= ~USART_CR1_OVER8;
            reg->MISCR = (reg->MISCR & ~USART_MISCR_SMPLINI_Msk) | (6 << USART_MISCR_SMPLINI_Pos);
        }
        reg->BRR = brr;
    }
}

vsf_err_t VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_config_isr)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_isr_t *isr_ptr
) {
    usart_ptr->isr = *isr_ptr;
    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_ptr->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_ptr->irqn);
    } else {
        NVIC_DisableIRQ(usart_ptr->irqn);
    }
    return VSF_ERR_NONE;
}

// TODO: support tx_timeout
vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(!(cfg_ptr->mode & __VSF_HW_USART_NOT_SUPPORT_MASK));

    USART_TypeDef *reg = usart_ptr->reg;

    vsf_hw_peripheral_enable(usart_ptr->en);

    // save boardrate, baudrate can only be set when USART is enalbed
    usart_ptr->__baudrate = cfg_ptr->baudrate;
    usart_ptr->__baudrate_set = false;
    cfg_ptr->mode &= ~VSF_USART_OVERSAMPLE_MASK;

    uint32_t ctl, cfg;
    uint32_t enabled = reg->CR1 & USART_CR1_UE;
    if (enabled) {
        reg->CR1 &= ~USART_CR1_UE;
    }

    ctl = reg->CR1;
    ctl &= ~(USART_CR1_RE | USART_CR1_TE | USART_CR1_PS | USART_CR1_PCE | USART_CR1_M);
    ctl |= cfg_ptr->mode & __VSF_HW_USART_CR1_MASK;
    reg->CR1 = ctl;

    ctl = reg->CR2;
    ctl &= ~(USART_CR2_CLKEN | USART_CR2_STOP | USART_CR2_SWAP | USART_CR2_RXINV | USART_CR2_TXINV);
    cfg = cfg_ptr->mode & __VSF_HW_USART_CR2_MASK;
    ctl |= ((cfg & 0x03) << 12) | ((cfg & 0xF00000) >> 12) | (cfg & ~0xF00003);
    reg->CR2 = ctl;

    ctl = reg->CR3;
    ctl &= ~(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_HDSEL);
    cfg = cfg_ptr->mode & __VSF_HW_USART_CR3_MASK;
    ctl |= ((cfg & 0x30) << 4) | ((cfg & 0x40) >> 3) | (cfg & ~0x70);
    reg->CR3 = ctl;

    if (enabled) {
        reg->CR1 |= USART_CR1_UE;
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_set_baudrate)(usart_ptr);
    }

    return VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_config_isr)(usart_ptr, &cfg_ptr->isr);
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
    usart_ptr->reg->CR1 |= USART_CR1_UE;
    if (!usart_ptr->__baudrate_set) {
        VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_set_baudrate)(usart_ptr);
    }
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    usart_ptr->reg->CR1 &= ~USART_CR1_UE;
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(!(irq_mask & ~__VSF_HW_USART_IRQ_MASK));
    USART_TypeDef *reg = usart_ptr->reg;
    reg->CR1 |= irq_mask & __VSF_HW_USART_CR1_IRQ_MASK;
    reg->CR3 |= irq_mask & __VSF_HW_USART_CR3_IRQ_MASK;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    USART_TypeDef *reg = usart_ptr->reg;
    reg->CR1 &= ~(irq_mask & __VSF_HW_USART_CR1_IRQ_MASK);
    reg->CR3 &= ~(irq_mask & __VSF_HW_USART_CR3_IRQ_MASK);
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (vsf_usart_status_t) {
        .value                  = usart_ptr->reg->ISR,
    };
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (usart_ptr->reg->ISR & USART_ISR_RXNE) ? 1 : 0;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    USART_TypeDef *reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    bool is16bit = (reg->CR1 & USART_CR1_M) == USART_CR1_M;
    while (cnt < count) {
        if (reg->ISR & USART_ISR_RXNE) {
            if (is16bit) {
                *(uint16_t *)buffer_ptr = reg->RDR & USART_RDR_RDR;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
            } else {
                *(uint8_t *)buffer_ptr = reg->RDR;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 1);
            }
            cnt++;
        } else {
            break;
        }
    }
    return cnt;
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_get_free_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (usart_ptr->reg->ISR & USART_ISR_TXE) ? 1 : 0;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    USART_TypeDef *reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    bool is16bit = (reg->CR1 & USART_CR1_M) == USART_CR1_M;
    while (cnt < count) {
        if (reg->ISR & USART_ISR_TXE) {
            if (is16bit) {
                reg->TDR = *(uint16_t *)buffer_ptr & USART_RDR_RDR;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
            } else {
                reg->TDR = *(uint8_t *)buffer_ptr;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 1);
            }
            cnt++;
        } else {
            break;
        }
    }
    return cnt;
}

vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_ctrl)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_ctrl_t cmd,
    void *param
) {
    VSF_HAL_ASSERT(!(cmd & ~__VSF_HW_USART_SUPPORT_CMD_MASK));
    usart_ptr->reg->RQR = cmd;
    return VSF_ERR_NONE;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    return (vsf_usart_capability_t) {
        .irq_mask                   = __VSF_HW_USART_IRQ_MASK,
        .max_baudrate               = 0,
        .min_baudrate               = 0,
        .min_data_bits              = 6,
        .max_data_bits              = 9,
        .txfifo_depth               = 1,
        .rxfifo_depth               = 1,
        // TODO: support tx_timeout
        .support_rx_timeout         = 0,
        .support_send_break         = 1,
    };
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    USART_TypeDef *reg = usart_ptr->reg;
    vsf_usart_isr_t *isr_ptr = &usart_ptr->isr;
    uint32_t irq_mask_orig = reg->ISR & (USART_ISR_RXNE | USART_ISR_TXE | USART_ISR_RTOF | USART_ISR_PE | USART_ISR_CTSIF);
    vsf_usart_irq_mask_t irq_mask = irq_mask_orig
                                |   ((irq_mask_orig & USART_ISR_RTOF) << 15)
                                |   ((irq_mask_orig & USART_ISR_PE) << 8)
                                |   ((irq_mask_orig & USART_ISR_CTSIF) << 1);

    // ORERR can be triggered if RFNEIE is enabled
    reg->ICR = USART_ICR_ORECF;
    if ((irq_mask != 0) && (isr_ptr->handler_fn != NULL)) {
        isr_ptr->handler_fn(isr_ptr->target_ptr, (vsf_usart_t *)usart_ptr, irq_mask);
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

// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw usart only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
// TODO: add comments about fifo2req
#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY        ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_REQUEST           ENABLED
#define VSF_USART_CFG_REIMPLEMENT_API_CTRL              ENABLED
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t)                            \
        VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart, __IDX) = {               \
        .reg                = (USART_TypeDef *)VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _REG_BASE),\
        .en                 = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _EN),\
        .rst                = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _RST),\
        .support_sync       = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _SYNC),\
        .irqn               = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _IRQN),\
        __HAL_OP                                                                \
    };                                                                          \
    void VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _IRQHandler)(void)\
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
