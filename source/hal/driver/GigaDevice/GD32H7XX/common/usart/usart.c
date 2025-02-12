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

#include "../vendor/Include/gd32h7xx_usart.h"

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
    uint32_t                reg;
    const vsf_hw_clk_t      *clk;
    vsf_hw_peripheral_en_t  en;
    vsf_hw_peripheral_rst_t rst;
    bool                    support_sync;
    IRQn_Type               irqn;
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
// TODO: support tx_timeout
vsf_err_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_init)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != cfg_ptr);
    VSF_HAL_ASSERT(!(cfg_ptr->mode & __VSF_HW_USART_NOT_SUPPORT_MASK));

    uint32_t reg = usart_ptr->reg;
    uint32_t uclk = vsf_hw_clk_get_freq_hz(usart_ptr->clk);
    uint32_t over8 = (cfg_ptr->mode & VSF_USART_OVERSAMPLE_MASK) >> 3;
    cfg_ptr->mode &= ~VSF_USART_OVERSAMPLE_MASK;
    uint32_t intdiv = 0U, fradiv = 0U, udiv = 0U;
    uint32_t baudval = cfg_ptr->baudrate;

    vsf_hw_peripheral_enable(usart_ptr->en);

    // boardrate
    if (over8){
        /* oversampling by 8, configure the value of USART_BAUD */
        udiv = ((2U * uclk) + baudval / 2U) / baudval;
        intdiv = udiv & 0x0000fff0U;
        fradiv = (udiv >> 1U) & 0x00000007U;
        USART_BAUD(reg) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
    } else {
        /* oversampling by 16, configure the value of USART_BAUD */
        udiv = (uclk + baudval / 2U) / baudval;
        intdiv = udiv & 0x0000fff0U;
        fradiv = udiv & 0x0000000fU;
        USART_BAUD(reg) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
    }

    uint32_t ctl, cfg;
    uint32_t enabled = USART_CTL0(reg) & USART_CTL0_UEN;
    if (enabled) {
        USART_CTL0(reg) &= ~USART_CTL0_UEN;
    }

    USART_FCS(reg) |= USART_FCS_FEN;

    ctl = USART_CTL0(reg);
    ctl &= ~(USART_CTL0_REN | USART_CTL0_TEN | USART_CTL0_PM | USART_CTL0_PCEN | USART_CTL0_WL0 | USART_CTL0_WL1);
    ctl |= cfg_ptr->mode & __VSF_HW_USART_CTL0_MASK;
    USART_CTL0(reg) = ctl | over8;

    ctl = USART_CTL1(reg);
    ctl &= ~(USART_CTL1_CKEN | USART_CTL1_STB | USART_CTL1_STRP | USART_CTL1_RINV | USART_CTL1_TINV);
    cfg = cfg_ptr->mode & __VSF_HW_USART_CTL1_MASK;
    ctl |= ((cfg & 0x03) << 12) | ((cfg & 0xF00000) >> 12) | (cfg & ~0xF00003);
    USART_CTL1(reg) = ctl;

    ctl = USART_CTL2(reg);
    ctl &= ~(USART_CTL2_RTSEN | USART_CTL2_CTSEN | USART_CTL2_HDEN);
    cfg = cfg_ptr->mode & __VSF_HW_USART_CTL2_MASK;
    ctl |= ((cfg & 0x30) << 4) | ((cfg & 0x40) >> 3) | (cfg & ~0x70);
    USART_CTL2(reg) = ctl;

    if (enabled) {
        USART_CTL0(reg) |= USART_CTL0_UEN;
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
    USART_CTL0(usart_ptr->reg) |= USART_CTL0_UEN;
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    USART_CTL0(usart_ptr->reg) &= ~USART_CTL0_UEN;
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_enable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(!(irq_mask & ~__VSF_HW_USART_IRQ_MASK));

    USART_CTL0(usart_ptr->reg) |= irq_mask & __VSF_HW_USART_CTL0_IRQ_MASK;
    USART_CTL2(usart_ptr->reg) |= irq_mask & __VSF_HW_USART_CTL2_IRQ_MASK;
}

void VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_irq_disable)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    vsf_usart_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    USART_CTL0(usart_ptr->reg) &= ~(irq_mask & __VSF_HW_USART_CTL0_IRQ_MASK);
    USART_CTL0(usart_ptr->reg) &= ~(irq_mask & __VSF_HW_USART_CTL2_IRQ_MASK);
}

vsf_usart_status_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_status)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (vsf_usart_status_t) {
        .value                  = USART_STAT(usart_ptr->reg),
    };
}

uint_fast16_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_get_data_count)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    return (USART_STAT(usart_ptr->reg) & USART_STAT_RFNE) ? 1 : 0;
//    uint32_t cnt = USART_FCS(usart_ptr->reg);
//    return ((cnt & USART_FCS_RFCNT3_4) << 2) | ((cnt & USART_FCS_RFCNT0_2) >> 12);
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_rxfifo_read)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    uint32_t reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    uint32_t is16bit = USART_CTL0(reg) & USART_CTL0_WL1;
    while (cnt < count) {
        if (USART_STAT(reg) & USART_STAT_RFNE) {
            if (is16bit) {
                *(uint16_t *)buffer_ptr = USART_RDATA(reg) & USART_RDATA_RDATA;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
            } else {
                *(uint8_t *)buffer_ptr = USART_RDATA(reg);
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
    return (USART_STAT(usart_ptr->reg) & USART_STAT_TFNF) ? 1 : 0;
}

uint_fast32_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_txfifo_write)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr,
    void *buffer_ptr,
    uint_fast16_t count
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);
    VSF_HAL_ASSERT(NULL != buffer_ptr);

    uint32_t reg = usart_ptr->reg;
    uint_fast32_t cnt = 0;
    uint32_t is16bit = USART_CTL0(reg) & USART_CTL0_WL1;
    while (cnt < count) {
        if (USART_STAT(reg) & USART_STAT_TFNF) {
            if (is16bit) {
                USART_TDATA(reg) = *(uint16_t *)buffer_ptr & USART_TDATA_TDATA;
                buffer_ptr = (void *)((uintptr_t)buffer_ptr + 2);
            } else {
                USART_TDATA(reg) = *(uint8_t *)buffer_ptr;
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
    USART_CMD(usart_ptr->reg) = cmd;
    return VSF_ERR_NONE;
}

vsf_usart_capability_t VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_capability)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    return (vsf_usart_capability_t) {
        .irq_mask                   = __VSF_HW_USART_IRQ_MASK,
        .max_baudrate               = 0,
        .min_baudrate               = 0,
        .min_data_bits              = 7,
        .max_data_bits              = 10,
        .txfifo_depth               = 16,
        .rxfifo_depth               = 17,
        // TODO: support tx_timeout
        .support_rx_timeout         = 0,
        .support_send_break         = 1,
    };
}

static void VSF_MCONNECT(__, VSF_USART_CFG_IMP_PREFIX, _usart_irqhandler)(
    VSF_MCONNECT(VSF_USART_CFG_IMP_PREFIX, _usart_t) *usart_ptr
) {
    VSF_HAL_ASSERT(NULL != usart_ptr);

    uint32_t reg = usart_ptr->reg;
    vsf_usart_isr_t *isr_ptr = &usart_ptr->isr;
    uint32_t irq_mask_orig = USART_STAT(reg) & (USART_STAT_RFNE | USART_STAT_TFNF | USART_STAT_RTF | USART_STAT_PERR | USART_STAT_CTSF);
    vsf_usart_irq_mask_t irq_mask = irq_mask_orig
                                |   ((irq_mask_orig & USART_STAT_RTF) << 15)
                                |   ((irq_mask_orig & USART_STAT_PERR) << 8)
                                |   ((irq_mask_orig & USART_STAT_CTSF) << 1);

    // ORERR can be triggered if RFNEIE is enabled
    USART_INTC(reg) = USART_INTC_OREC;
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
        .reg                = VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _REG_BASE),\
        .clk                = &VSF_MCONNECT(VSF_USART_CFG_IMP_UPCASE_PREFIX, _USART, __IDX, _CLK),\
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
