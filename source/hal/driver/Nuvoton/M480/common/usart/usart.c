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

#define VSF_USART_CFG_PREFIX                vsf_hw
#define VSF_USART_CFG_UPPERCASE_PREFIX      VSF_HW
#define VSF_USART_CFG_REQUEST_API           DISABLED

/*============================ INCLUDES ======================================*/

#include "./usart.h"

#if VSF_HAL_USE_USART == ENABLED

//! include the infrastructure
#include "../io/io.h"
#include "../pm/pm.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_hw_usart_const_t {
    UART_T    *usart;
    IRQn_Type  irq;
    uint32_t   module;
    uint32_t   clock_source;
    uint32_t   uartx_rst;
} vsf_hw_usart_const_t;

typedef struct vsf_hw_usart_t {
    usart_status_t              status;
    uint8_t                    *tx_buf;
    uint8_t                    *rx_buf;
    usart_cfg_t                 cfg;
    uint8_t                     is_enabled : 1;
    const vsf_hw_usart_const_t *usart_const;
} vsf_hw_usart_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static uint32_t __clk_get_pllclockfreq(void)
{
    uint32_t u32_pll_freq = 0ul, u32_pll_reg;
    uint32_t u32_fin, u32_nf, u32_nr, u32_no;
    uint8_t au8NoTbl[4] = {1ul, 2ul, 2ul, 4ul};

    u32_pll_reg = CLK->PLLCTL;
    if (u32_pll_reg & (CLK_PLLCTL_PD_Msk | CLK_PLLCTL_OE_Msk)) {
        u32_pll_freq = 0ul;
    } else if ((u32_pll_reg & CLK_PLLCTL_BP_Msk) == CLK_PLLCTL_BP_Msk) {
        if ((u32_pll_reg & 0x00080000ul) == 0x00080000ul) {
            u32_fin = __HIRC;
        } else {
            u32_fin = __HXT;
        }
        u32_pll_freq = u32_fin;
    } else {
        if ((u32_pll_reg & 0x00080000ul) == 0x00080000ul) {
            u32_fin = __HIRC;
        } else {
            u32_fin = __HXT;
        }
        u32_no = au8NoTbl[((u32_pll_reg & CLK_PLLCTL_OUTDIV_Msk) >> CLK_PLLCTL_OUTDIV_Pos)];
        u32_nf = ((u32_pll_reg & CLK_PLLCTL_FBDIV_Msk) >> CLK_PLLCTL_FBDIV_Pos) + 2ul;
        u32_nr = ((u32_pll_reg & CLK_PLLCTL_INDIV_Msk) >> CLK_PLLCTL_INDIV_Pos) + 1ul;
        u32_pll_freq = (((u32_fin >> 2) * u32_nf) / (u32_nr * u32_no) << 2) * 2ul;
    }
    return u32_pll_freq;
}

vsf_err_t vsf_hw_usart_init(vsf_hw_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    uint32_t u32_sel = 0ul, u32_div = 0ul;
    uint32_t u32_tmp_val = 0ul, u32_tmp_addr = 0ul;
    uint32_t u32_uart_clk_src_sel = 0ul, u32_uart_clk_div_num = 0ul;
    uint32_t u32_baud_div = 0ul;
    uint32_t u32_clk_tbl[4] = {__HXT, 0ul, __LXT, __HIRC};

    usart_ptr->status.is_busy = true;
    if (((usart_ptr->usart_const->module >> 10) & 0xfful) != 0) {
        if (((usart_ptr->usart_const->module >> 18) & 0x3ul) == 2ul) {
            u32_div = (uint32_t)&CLK->CLKDIV3;
        } else if (((usart_ptr->usart_const->module >> 18) & 0x3ul) == 3ul) {
            u32_div = (uint32_t)&CLK->CLKDIV4;
        } else {
            u32_div = (uint32_t)&CLK->CLKDIV0 + (((usart_ptr->usart_const->module >> 18) & 0x3ul) * 4ul);
        }
        M32(u32_div) = (M32(u32_div) & (~(((usart_ptr->usart_const->module >> 10) & 0xfful) << ((usart_ptr->usart_const->module >> 5) & 0x1ful)))) | 0;
    }
    if (((usart_ptr->usart_const->module >> 25) & 0x7ul) != 0) {
        u32_sel = (uint32_t)&CLK->CLKSEL0 + (((usart_ptr->usart_const->module >> 28) & 0x3ul) * 4ul);
        M32(u32_sel) = (M32(u32_sel) & (~(((usart_ptr->usart_const->module >> 25) & 0x7ul) << ((usart_ptr->usart_const->module >> 20) &0x1ful)))) | usart_ptr->usart_const->clock_source;
    }
    u32_tmp_val = (1ul << ((usart_ptr->usart_const->module >> 0) & 0x1ful));
    u32_tmp_addr = (uint32_t)&CLK->AHBCLK;
    u32_tmp_addr += (((usart_ptr->usart_const->module >> 30) & 0x3ul) * 4ul);
    *(volatile uint32_t *)u32_tmp_addr |= u32_tmp_val;
    u32_tmp_val = (1ul << (usart_ptr->usart_const->uartx_rst & 0x00fffffful));
    u32_tmp_addr = (uint32_t)&SYS->IPRST0 + ((usart_ptr->usart_const->uartx_rst >> 24ul));
    *(uint32_t *)u32_tmp_addr |= u32_tmp_val;
    u32_tmp_val = ~(1ul << (usart_ptr->usart_const->uartx_rst & 0x00fffffful));
    *(uint32_t *)u32_tmp_addr &= u32_tmp_val;

    if (usart_ptr->usart_const->usart == (UART_T *)UART0) {
        u32_uart_clk_src_sel = ((uint32_t)(CLK->CLKSEL1 & CLK_CLKSEL1_UART0SEL_Msk)) >> CLK_CLKSEL1_UART0SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV0 & CLK_CLKDIV0_UART0DIV_Msk) >> CLK_CLKDIV0_UART0DIV_Pos;
    } else if (usart_ptr->usart_const->usart == (UART_T *)UART1) {
        u32_uart_clk_src_sel = (CLK->CLKSEL1 & CLK_CLKSEL1_UART1SEL_Msk) >> CLK_CLKSEL1_UART1SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV0 & CLK_CLKDIV0_UART1DIV_Msk) >> CLK_CLKDIV0_UART1DIV_Pos;
    } else if (usart_ptr->usart_const->usart == (UART_T *)UART2) {
        u32_uart_clk_src_sel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART2SEL_Msk) >> CLK_CLKSEL3_UART2SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV4 & CLK_CLKDIV4_UART2DIV_Msk) >> CLK_CLKDIV4_UART2DIV_Pos;
    } else if (usart_ptr->usart_const->usart == (UART_T *)UART3) {
        u32_uart_clk_src_sel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART3SEL_Msk) >> CLK_CLKSEL3_UART3SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV4 & CLK_CLKDIV4_UART3DIV_Msk) >> CLK_CLKDIV4_UART3DIV_Pos;
    } else if (usart_ptr->usart_const->usart == (UART_T *)UART4) {
        u32_uart_clk_src_sel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART4SEL_Msk) >> CLK_CLKSEL3_UART4SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV4 & CLK_CLKDIV4_UART4DIV_Msk) >> CLK_CLKDIV4_UART4DIV_Pos;
    } else if (usart_ptr->usart_const->usart == (UART_T *)UART5) {
        u32_uart_clk_src_sel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART5SEL_Msk) >> CLK_CLKSEL3_UART5SEL_Pos;
        u32_uart_clk_div_num = (CLK->CLKDIV4 & CLK_CLKDIV4_UART5DIV_Msk) >> CLK_CLKDIV4_UART5DIV_Pos;
    }
    if (0 != cfg_ptr->rx_timeout) {
        usart_ptr->usart_const->usart->INTEN |= UART_INTEN_TOCNTEN_Msk;
        usart_ptr->usart_const->usart->TOUT |= ((cfg_ptr->rx_timeout & 0xfful) << UART_TOUT_TOIC_Pos);
    }
    usart_ptr->usart_const->usart->FUNCSEL = 0x0ul << UART_FUNCSEL_FUNCSEL_Pos;
    usart_ptr->usart_const->usart->LINE =      (cfg_ptr->mode & USART_BIT_LENGTH)
                                    |   (((cfg_ptr->mode & USART_PARITY) >> 16) << UART_LINE_PBE_Pos)
                                    |   (((cfg_ptr->mode & USART_STOPBIT) >> 8) << UART_LINE_NSB_Pos)
                                    |   ((cfg_ptr->mode & USART_TX_INVERTED >> 26) << UART_LINE_TXDINV_Pos)
                                    |   ((cfg_ptr->mode & USART_RX_INVERTED >> 27) << UART_LINE_RXDINV_Pos);
    usart_ptr->usart_const->usart->FIFO &= ~(UART_FIFO_RFITL_Msk | UART_FIFO_RTSTRGLV_Msk| UART_FIFO_RXOFF_Msk);

    if (u32_uart_clk_src_sel == 1ul) {
        u32_clk_tbl[u32_uart_clk_src_sel] = __clk_get_pllclockfreq();
    }
    if (cfg_ptr->baudrate != 0ul) {
        u32_baud_div = (((u32_clk_tbl[u32_uart_clk_src_sel] / (u32_uart_clk_div_num + 1ul)) + cfg_ptr->baudrate / 2ul) / cfg_ptr->baudrate) - 2ul;
        if (u32_baud_div > 0xFFFFul) {
            usart_ptr->usart_const->usart->BAUD = (0ul | (((u32_clk_tbl[u32_uart_clk_src_sel] / (u32_uart_clk_div_num + 1ul) + cfg_ptr->baudrate * 8ul) / cfg_ptr->baudrate >> 4ul) - 2ul));
        } else {
            usart_ptr->usart_const->usart->BAUD = (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk | u32_baud_div);
        }
    }
    usart_ptr->status.is_busy = false;
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_hw_usart_enable(vsf_hw_usart_t *usart_ptr)
{
    usart_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_hw_usart_disable(vsf_hw_usart_t *usart_ptr)
{
    usart_ptr->is_enabled = true;
    return fsm_rt_cpl;
}

usart_status_t vsf_hw_usart_status(vsf_hw_usart_t *usart_ptr)
{
    return  usart_ptr->status;
    //todo: write status
}

uint_fast16_t vsf_hw_usart_fifo_read(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    uint_fast16_t ret_count;
    usart_ptr->status.is_busy = true;
    if (!(usart_ptr->is_enabled)) {
        return 0;
    }
    for (ret_count = 0u; ret_count < count;) {
        if ((usart_ptr->usart_const->usart->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) != 0) {
            usart_ptr->status.rx_error_detected = true;
            return ret_count;
        }
        *((uint8_t *)buffer_ptr + ret_count) = usart_ptr->usart_const->usart->DAT;
        ret_count++;
    }
    usart_ptr->status.is_busy = false;
    return ret_count;
}

uint_fast16_t vsf_hw_usart_fifo_write(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    uint_fast16_t ret_count;
    usart_ptr->status.is_busy = true;
    if (!(usart_ptr->is_enabled)) {
        return 0;
    }
    for (ret_count = 0u; ret_count < count; ret_count++) {
        if(!(usart_ptr->usart_const->usart->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk)) {
            usart_ptr->status.tx_error_detected = true;
            return ret_count;
        }
        usart_ptr->usart_const->usart->DAT = 0x5500;
    }
    usart_ptr->status.is_busy = false;
    return ret_count;
}

bool vsf_hw_usart_fifo_flush(vsf_hw_usart_t *usart_ptr)
{
    while (     (!(usart_ptr->usart_const->usart->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk))
           &&   ((usart_ptr->usart_const->usart->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) != 0));
    return true;
}

void vsf_hw_usart_irq_enable(vsf_hw_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_RX) {
        usart_ptr->usart_const->usart->INTEN |= UART_INTEN_RDAIEN_Msk;
    }
    if (irq_mask & USART_IRQ_MASK_TX) {
        usart_ptr->usart_const->usart->INTEN |= UART_INTEN_THREIEN_Msk;
    }
    //todo:
//    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
//
//    }
//    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
//
//    }
//    if (irq_mask & USART_IRQ_MASK_RX_ERROR) {
//
//    }
//    if (irq_mask & USART_IRQ_MASK_TX_ERROR) {
//
//    }
}
void vsf_hw_usart_irq_disable(vsf_hw_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_RX) {
        usart_ptr->usart_const->usart->INTEN &= ~(UART_INTEN_RDAIEN_Msk | 0xffffffff);
    }
    if (irq_mask & USART_IRQ_MASK_TX) {
        usart_ptr->usart_const->usart->INTEN &= ~(UART_INTEN_THREIEN_Msk | 0xffffffff);
    }
}

vsf_err_t vsf_hw_usart_request_rx(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    if (!(usart_ptr->is_enabled)) {
        return VSF_ERR_NOT_READY;
    }
    //todo:
    return VSF_ERR_NONE;
}

vsf_err_t vsf_hw_usart_request_tx(vsf_hw_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    if (!(usart_ptr->is_enabled)) {
        return VSF_ERR_NOT_READY;
    }
    //todo:
    return VSF_ERR_NONE;
}

/*============================ IMPLEMENTATION ================================*/

#define VSF_USART_CFG_IMP_LV0(__count, __hal_op)                                                \
    static const vsf_hw_usart_const_t vsf_hw_usart ## __count ## _const = {                     \
        .usart = (UART ## __count),                                                             \
        .irq = (UART ## __count##_IRQn),                                                        \
        .module = (VSF_HW_USART ## __count##_MODULE),                                           \
        .uartx_rst = VSF_HW_USART ## __count##_RST,                                             \
        .clock_source = VSF_MCONNECT(VSF_HW_USART, __count, SEL_, VSF_CFG_USART_CLOCK_SOURCE),  \
    };                                                                                          \
    vsf_hw_usart_t vsf_hw_usart ## __count = {                                                  \
        .usart_const = &vsf_hw_usart ## __count ## _const,                                      \
    };
#include "hal/driver/common/usart/usart_template.inc"

#endif
