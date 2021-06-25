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

#include "./uart.h"
#include "../vendor/plf/aic8800/src/driver/uart/reg_uart1.h"
#include "../vendor/plf/aic8800/src/driver/iomux/reg_iomux.h"
#include "../vendor/plf/aic8800/src/driver/ipc/reg_ipc_comreg.h"
#include "sysctrl_api.h"
//#include "reg_access.h"
/*============================ MACROS ========================================*/

#define __UART_BASE_ADDR(__N)                                               \
        UART##__N

#define UART_BASE_ADDR(__N)                                                 \
        __UART_BASE_ADDR(__N)
/*============================ MACROFIED FUNCTIONS ===========================*/

#define ______vsf_hw_usart_imp_lv0(__count, __dont_care)                    \
    vsf_usart_t vsf_usart##__count = {                                      \
        .param = UART_BASE_ADDR(__count),                                   \
        .hclk = CSC_HCLKME_UART##__count##_EN_BIT,                          \
        .oclk = CSC_OCLKME_UART##__count##_EN_BIT,                          \
        .per_uart = PER_UART##__count,                                      \
        .irqn = UART##__count##_IRQn,                                       \
        .num = __count,                                                     \
    };                                                                      \
    WEAK(UART##__count##_IRQHandler)                                        \
    void UART##__count##_IRQHandler(void)                                   \
    {                                                                       \
        if (NULL != vsf_usart##__count.cfg.isr.handler_fn) {                \
            vsf_usart##__count.cfg.isr.handler_fn(vsf_usart##__count.cfg.isr.target_ptr, \
                                                  &vsf_usart##__count,      \
                                                  __get_uart_irq_mask(&vsf_usart##__count));\
        }                                                                   \
    }

#if VSF_HAL_USE_USART0 == ENABLED
#   define ____vsf_hw_usart_imp_lv0_0(__count, __dont_care)                 \
    ______vsf_hw_usart_imp_lv0(__count, __dont_care)
#else
#   define ____vsf_hw_usart_imp_lv0_0(__count, __dont_care)
#endif

#if VSF_HAL_USE_USART1 == ENABLED
#   define ____vsf_hw_usart_imp_lv0_1(__count, __dont_care)                 \
    ______vsf_hw_usart_imp_lv0(__count, __dont_care)
#else
#   define ____vsf_hw_usart_imp_lv0_1(__count, __dont_care)
#endif

#if VSF_HAL_USE_USART2 == ENABLED
#   define ____vsf_hw_usart_imp_lv0_2(__count, __dont_care)                 \
    ______vsf_hw_usart_imp_lv0(__count, __dont_care)
#else
#   define ____vsf_hw_usart_imp_lv0_2(__count, __dont_care)
#endif

#define ____vsf_hw_usart_imp_lv0(__count, __dont_care)                      \
    ____vsf_hw_usart_imp_lv0_##__count(__count, __dont_care)

#define ____vsf_hw_usart_imp_lv0(__count, __dont_care)                      \
    ____vsf_hw_usart_imp_lv0_##__count(__count, __dont_care)

#define __vsf_hw_usart_imp_lv0(__count)                                     \
    VSF_MREPEAT(__count, ____vsf_hw_usart_imp_lv0, NULL)
#define aic8800_uart_init(__count)                                          \
    __vsf_hw_usart_imp_lv0(__count)
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    uint32_t div;
    NVIC_DisableIRQ(usart_ptr->irqn);
    //
        iomux_gpio_config_sel_setf(10, 0x01);
        iomux_gpio_config_sel_setf(11, 0x01);
    //
    cpusysctrl_hclkme_set(CSC_HCLKME_UART1_EN_BIT);
    cpusysctrl_oclkme_set(CSC_OCLKME_UART1_EN_BIT);
    usart_ptr->param->DBUFCFG_REG = ((0 << UART_DBUFEN) | (0 << UART_RXDRST) | (0 << UART_TXDRST));
    usart_ptr->param->IRQCTL_REG = (    (0 << UART_PTIRQEN)
                                    |   (0 << UART_MSIRQEN)
                                    |   (0 << UART_LSIRQEN)
                                    |   (0 << UART_TXIRQEN)
                                    |   (0 << UART_RXIRQEN));
    usart_ptr->param->DBUFTH_REG = (    (1 << UART_RXTRIGTH)
                                     |  (0 << UART_TXTRIGTH));
    usart_ptr->param->MDMCFG_REG |= UART_CLK_P_MSK;
    if (0 == (cfg_ptr->mode & USART_PARITY)) {
        cfg_ptr->mode |= USART_NONE_PARITY;
    }
    usart_ptr->param->DFMTCFG_REG =     (       (cfg_ptr->mode & USART_BIT_LENGTH)
                                            |   (cfg_ptr->mode & USART_STOPBIT)
                                            |   (cfg_ptr->mode & USART_NONE_PARITY)
                                        )
                                    &   (0x1f);
    div = sysctrl_clock_get(usart_ptr->per_uart) / cfg_ptr->baudrate;
    usart_ptr->param->DFMTCFG_REG |= UART_DIVAE_MSK;
    usart_ptr->param->DIV0_REG = (div >> 4) & UART_DIV0_MSK;
    usart_ptr->param->DIV1_REG = (div >> 12) & UART_DIV1_MSK;
    usart_ptr->param->DIV2_REG = (div & 0x1) + ((div >> 1) & 0x7) + ((div << 3) & 0x7);
    usart_ptr->param->DFMTCFG_REG &= ~UART_DIVAE_MSK;
    usart_ptr->param->DBUFCFG_REG = (cfg_ptr->mode & USART_DBUFCFG) >> 6;
    usart_ptr->cfg = *cfg_ptr;
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    usart_ptr->status.is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    usart_ptr->status.is_enabled = false;
    return fsm_rt_cpl;
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    NVIC_DisableIRQ(usart_ptr->irqn);
    usart_ptr->param->IRQCTL_REG |= USART_IRQ_MASK_SOURCE & irq_mask;
    NVIC_SetPriority(usart_ptr->irqn, (uint32_t)usart_ptr->cfg.isr.prio);
    NVIC_EnableIRQ(usart_ptr->irqn);

    ipccomreg_state_uart1inited_setb();
}

void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    NVIC_DisableIRQ(usart_ptr->irqn);
    usart_ptr->param->IRQCTL_REG &= ~(irq_mask & USART_IRQ_MASK_SOURCE);
}

usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    return usart_ptr->status;
}

uint_fast16_t vsf_usart_fifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    for (uint_fast16_t i = 0; i < count; i++) {
        if (usart_ptr->param->DBUFSTS_REG & UART_RX_DBUF_EMPTY_MSK) {
            return i;
        }
        *((uint8_t *)buffer_ptr + i) = usart_ptr->param->TXRXD_REG & UART_RXDATA_MSK;
    }
    return count;
}

uint_fast16_t vsf_usart_fifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    VSF_HAL_ASSERT(NULL != usart_ptr);
    for (uint_fast16_t i = 0; i < count; i++) {
        if (usart_ptr->param->DBUFSTS_REG & UART_TX_DBUF_FULL_MSK) {
            return i;
        }
        (usart_ptr->param->TXRXD_REG) = (uint32_t)(*((uint8_t *)buffer_ptr + i)) & UART_TXDATA_MSK;
    }
    return count;
}

inline static em_usart_irq_mask_t __get_uart_irq_mask(vsf_usart_t *usart_ptr)
{
    switch (uart1_irqtyp_getf())
    {
        case UART_RX_ERROR_INT_MSK:
            uart1_irqsts_get();
            return USART_IRQ_MASK_RX_ERR;
        case UART_RX_INT_MSK:
            return USART_IRQ_MASK_RX;
        case UART_TIMEOUT_INT_MSK:
            return USART_IRQ_MSK_TIMEOUT;
        case UART_TX_INT_MSK:
            return USART_IRQ_MASK_TX;
        case UART_MODEM_INT_MSK:
            uart1_mdmsts_get();
            //TODO
            return (em_usart_irq_mask_t)0;
        case UART_NO_INT_MSK:
            return (em_usart_irq_mask_t)0;
        default:
            //TODO
            return (em_usart_irq_mask_t)0;
    }
}
/*
bool               vsf_usart_fifo_flush(     vsf_usart_t *usart_ptr);

vsf_err_t          vsf_usart_request_rx(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast32_t count);
vsf_err_t          vsf_usart_request_tx(     vsf_usart_t *usart_ptr,
                                                    void *buffer_ptr,
                                                    uint_fast32_t count);
vsf_err_t          vsf_usart_cancel_rx(      vsf_usart_t *usart_ptr);
vsf_err_t          vsf_usart_cancel_tx(      vsf_usart_t *usart_ptr);
int_fast32_t       vsf_usart_get_rx_count(   vsf_usart_t *usart_ptr);
int_fast32_t       vsf_usart_get_tx_count(   vsf_usart_t *usart_ptr);
*/
/*============================ IMPLEMENTATION ================================*/
aic8800_uart_init(USART_MAX_PORT)

