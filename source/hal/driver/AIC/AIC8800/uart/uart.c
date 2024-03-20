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

#if VSF_HAL_USE_USART == ENABLED

#include "hal/vsf_hal.h"
#include "../vendor/plf/aic8800/src/driver/uart/reg_uart1.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/ipc/reg_ipc_comreg.h"
#include "../vendor/plf/aic8800/src/driver/sysctrl/sysctrl_api.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HW_USART_CFG_MULTI_CLASS
#   define VSF_HW_USART_CFG_MULTI_CLASS             VSF_USART_CFG_MULTI_CLASS
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct __vsf_hw_usart_const_t {
    IRQn_Type irqn;
    uart_reg_t *reg;

    uint32_t hclk;
    uint32_t oclk;
    uint32_t perclk;
} __vsf_hw_usart_const_t;

typedef struct __vsf_hw_usart_t {
#if VSF_HW_USART_CFG_MULTI_CLASS == ENABLED
    vsf_usart_t vsf_usart;
#endif
    const __vsf_hw_usart_const_t *usart_const;

    vsf_usart_isr_t         isr;
} __vsf_hw_usart_t;

/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vsf_hw_usart_init(__vsf_hw_usart_t *hw_usart_ptr, vsf_usart_cfg_t *cfg_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    uart_reg_t *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    cpusysctrl_hclkme_set(usart_const->hclk);                               // usart_const enable
    cpusysctrl_oclkme_set(usart_const->oclk);

    reg->DBUFCFG_REG = 0x0;                                                 // reset fifo
    reg->IRQCTL_REG = 0x0;                                                  // clean all interrupt
    reg->DBUFTH_REG = (1 << UART_RXTRIGTH) |  (0 << UART_TXTRIGTH);         // tx and rx data buffer trigger threshold
    reg->MDMCFG_REG |= UART_CLK_P_MSK;                                      // force 48M Clock
    reg->DFMTCFG_REG = cfg_ptr->mode & AIC8800_USART_MODE_ALL_BITS_MASK;

    uint32_t div = sysctrl_clock_get(usart_const->perclk) / cfg_ptr->baudrate;
    reg->DFMTCFG_REG |= UART_DIVAE_MSK;                                     // div reg access enable
    reg->DIV0_REG = (div >> 4) & UART_DIV0_MSK;
    reg->DIV1_REG = (div >> 12) & UART_DIV1_MSK;
    reg->DIV2_REG = (div & 0x1) + ((div >> 1) & 0x7) + ((div << 3) & 0x70);
    reg->DFMTCFG_REG &= ~UART_DIVAE_MSK;                                    // div reg access disable

    reg->DBUFCFG_REG = UART_DBUFEN_MSK | UART_RXDRST_MSK | UART_TXDRST_MSK; // tx/rx data buf reset and enable

    vsf_usart_isr_t *isr_ptr = &cfg_ptr->isr;
    hw_usart_ptr->isr = *isr_ptr;

    if (isr_ptr->handler_fn != NULL) {
        NVIC_SetPriority(usart_const->irqn, (uint32_t)isr_ptr->prio);
        NVIC_EnableIRQ(usart_const->irqn);
    } else {
        NVIC_DisableIRQ(usart_const->irqn);
    }

    return VSF_ERR_NONE;
}

void __vsf_hw_usart_fini(__vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(hw_usart_ptr != NULL);
}

vsf_usart_capability_t __vsf_hw_usart_capability(__vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);

    uint32_t pclk = sysctrl_clock_get(usart_const->perclk);
    vsf_usart_capability_t usart_capability = {
        .irq_mask = AIC8800_USART_MODE_ALL_BITS_MASK,
        .max_baudrate = pclk / (16 * 1),
        .min_baudrate = pclk / (16 * 1) / 65535,
        .min_data_bits = 8,
        .max_data_bits = 9,
        .max_tx_fifo_counter = 64,
        .max_rx_fifo_counter = 64,
        .support_rx_timeout = 0,
    };

    return usart_capability;
}

fsm_rt_t __vsf_hw_usart_enable(__vsf_hw_usart_t *hw_usart_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t __vsf_hw_usart_disable(__vsf_hw_usart_t *hw_usart_ptr)
{
    return fsm_rt_cpl;
}

void __vsf_hw_usart_irq_enable(__vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    VSF_HAL_ASSERT(NULL != usart_const->reg);
    VSF_HAL_ASSERT((irq_mask & ~AIC8800_USART_IRQ_MASK) == 0);

    usart_const->reg->IRQCTL_REG |= irq_mask;
}

void __vsf_hw_usart_irq_disable(__vsf_hw_usart_t *hw_usart_ptr, vsf_usart_irq_mask_t irq_mask)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    VSF_HAL_ASSERT(NULL != usart_const->reg);
    VSF_HAL_ASSERT((irq_mask & ~AIC8800_USART_IRQ_MASK) == 0);

    usart_const->reg->IRQCTL_REG &= ~irq_mask;
}

static bool __hw_usart_read_fifo_is_empty(__vsf_hw_usart_t *hw_usart_ptr)
{
    return hw_usart_ptr->usart_const->reg->DBUFSTS_REG & UART_RX_DBUF_EMPTY_MSK;
}

static bool __hw_usart_write_fifo_is_full(__vsf_hw_usart_t *hw_usart_ptr)
{
    return hw_usart_ptr->usart_const->reg->DBUFSTS_REG & UART_TX_DBUF_FULL_MSK;
}

vsf_usart_status_t __vsf_hw_usart_status(__vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);

    uint32_t idle_mask = UART_RX_DBUF_EMPTY_MSK | UART_TX_DBUF_EMPTY_MSK;

    vsf_usart_status_t status = {
        .is_busy = (hw_usart_ptr->usart_const->reg->DBUFSTS_REG & idle_mask) != idle_mask,
    };

    return status;
}

uint_fast16_t __vsf_hw_usart_rxfifo_get_data_count(__vsf_hw_usart_t *hw_usart_ptr)
{
    return (hw_usart_ptr->usart_const->reg->DBUFSTS_REG & UART_RX_COUNT_MSK) >> UART_RX_COUNT;
}

uint_fast16_t __vsf_hw_usart_rxfifo_read(__vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    uart_reg_t *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint8_t *buf_ptr = (uint8_t *) buffer_ptr;
    VSF_HAL_ASSERT(NULL != buf_ptr);

    uint_fast16_t i = 0;
    while (i < count) {
        if (__hw_usart_read_fifo_is_empty(hw_usart_ptr)) {
            break;
        }
        buf_ptr[i++] = reg->TXRXD_REG & UART_TXDATA_MSK;
    }
    return i;
}

uint_fast16_t __vsf_hw_usart_txfifo_get_free_count(__vsf_hw_usart_t *hw_usart_ptr)
{
    return 64 - ((hw_usart_ptr->usart_const->reg->DBUFSTS_REG & UART_TX_COUNT_MSK) >> UART_TX_COUNT);
}

uint_fast16_t __vsf_hw_usart_txfifo_write(__vsf_hw_usart_t *hw_usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    uart_reg_t *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint8_t *buf_ptr = (uint8_t *) buffer_ptr;
    VSF_HAL_ASSERT(NULL != buf_ptr);

    uint_fast16_t i = 0;
    while (i < count) {
        if (__hw_usart_write_fifo_is_full(hw_usart_ptr)) {
            break;
        }
        reg->TXRXD_REG = buf_ptr[i++] & UART_TXDATA_MSK;
    }
    return i;
}

static vsf_usart_irq_mask_t __get_uart_irq_mask(__vsf_hw_usart_t *hw_usart_ptr)
{
    VSF_HAL_ASSERT(NULL != hw_usart_ptr);
    const __vsf_hw_usart_const_t *usart_const = hw_usart_ptr->usart_const;
    VSF_HAL_ASSERT(NULL != usart_const);
    uart_reg_t *reg = usart_const->reg;
    VSF_HAL_ASSERT(NULL != reg);

    uint32_t value;
    uint32_t irq_type = reg->IRQTYP_REG & UART_IRQTYP_MSK;

    switch (irq_type) {
        case UART_IRQTYP_RX_ERROR_INT:
            value = reg->IRQSTS_REG;        // TODO: report more specific rx error
            (void)value;
            return VSF_USART_IRQ_MASK_ERR;

        case UART_IRQTYP_RX_INT:
            return VSF_USART_IRQ_MASK_RX;

        case UART_IRQTYP_TIMEOUT_INT:
            return 0;

        case UART_IRQTYP_TX_INT:
            return VSF_USART_IRQ_MASK_TX;

        case UART_IRQTYP_MODEM_INT:
            value = reg->MDMSTS_REG;        // TODO: report more specific moden line interrupt
            (void)value;
            return (vsf_usart_irq_mask_t)0;

        case UART_IRQTYP_NO_INT:
            return (vsf_usart_irq_mask_t)0;

        default:
            VSF_HAL_ASSERT(0);
            return (vsf_usart_irq_mask_t)0;
    }
}

static void __vsf_hw_usart_irq_handler(__vsf_hw_usart_t *hw_usart_ptr)
{
    vsf_usart_irq_mask_t irq_mask = __get_uart_irq_mask(hw_usart_ptr);
    if (irq_mask & VSF_USART_IRQ_ALL_BITS_MASK) {
        if (NULL != hw_usart_ptr->isr.handler_fn) {
            hw_usart_ptr->isr.handler_fn(hw_usart_ptr->isr.target_ptr, (vsf_usart_t *)hw_usart_ptr, irq_mask);
        }
    }
}

/*============================ INCLUDES ======================================*/

#define VSF_USART_CFG_REIMPLEMENT_API_CAPABILITY    ENABLED
#define VSF_USART_CFG_IMP_INSTANCE_PREFIX           __vsf_hw
#define VSF_USART_CFG_IMP_PREFIX                    __vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX             __VSF_HW
#define __VSF_HW_USART_COUNT                        VSF_HW_USART_COUNT
#define __VSF_HW_USART_MASK                         VSF_HW_USART_MASK
#define VSF_USART_CFG_IMP_FIFO_TO_REQUEST           ENABLED
#define VSF_USART_CFG_IMP_LV0(__IDX, __HAL_OP)                                  \
    static const vsf_hw_usart_const_t __vsf_hw_usart ## __IDX ## _clock = {     \
        .reg    = (uart_reg_t *)VSF_HW_USART ## __IDX ## _REG,                  \
        .irqn   = UART ## __IDX ## _IRQn,                                       \
        .hclk   = CSC_HCLKME_UART ## __IDX ## _EN_BIT,                          \
        .oclk   = CSC_OCLKME_UART ## __IDX ## _EN_BIT,                          \
        .perclk = PER_UART ## __IDX,                                            \
    };                                                                          \
    vsf_hw_usart_t __vsf_hw_usart ## __IDX = {                                  \
        .usart_const  = &__vsf_hw_usart ## __IDX ## _clock,                     \
        __HAL_OP                                                                \
    };                                                                          \
    void UART ## __IDX ## _IRQHandler(void)                                     \
    {                                                                           \
        uintptr_t ctx = vsf_hal_irq_enter();                                    \
        __vsf_hw_usart_irq_handler(&__vsf_hw_usart ## __IDX);                   \
        vsf_hal_irq_leave(ctx);                                                 \
    }
#include "hal/driver/common/usart/usart_template.inc"
#define VSF_USART_CFG_IMP_INSTANCE_PREFIX       vsf_hw
#define VSF_USART_CFG_IMP_PREFIX                vsf_hw
#define VSF_USART_CFG_IMP_UPCASE_PREFIX         VSF_HW
#define VSF_USART_CFG_IMP_LV0(__COUNT, __HAL_OP)                                \
    describe_fifo2req_usart(VSF_USART_CFG_IMP_INSTANCE_PREFIX,                  \
                            vsf_hw_usart ## __COUNT, __vsf_hw_usart ## __COUNT)
#include "hal/driver/common/usart/usart_template.inc"

#endif      // VSF_HAL_USE_USART
