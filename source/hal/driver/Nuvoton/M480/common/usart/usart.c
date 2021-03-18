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

#include "utilities/vsf_utilities.h"
#include "./usart.h"

/*============================ MACROS ========================================*/

#define UART0_RST                               ((4UL<<24) | SYS_IPRST1_UART0RST_Pos)       
#define UART1_RST                               ((4UL<<24) | SYS_IPRST1_UART1RST_Pos)       
#define UART2_RST                               ((4UL<<24) | SYS_IPRST1_UART2RST_Pos)       
#define UART3_RST                               ((4UL<<24) | SYS_IPRST1_UART3RST_Pos)       
#define UART4_RST                               ((4UL<<24) | SYS_IPRST1_UART4RST_Pos)       
#define UART5_RST                               ((4UL<<24) | SYS_IPRST1_UART5RST_Pos)       

#define UART0_MODULE                            ((1UL<<30)|(1UL<<28) | (0x3UL<<25) | (24UL<<20) | (0UL<<18) | (0xFUL<<10) | (8UL<<5) |(16UL<<0)) 
#define UART1_MODULE                            ((1UL<<30)|(1UL<<28) | (0x3UL<<25) | (26UL<<20) | (0UL<<18) | (0xFUL<<10) | (12UL<<5) |(17UL<<0))
#define UART2_MODULE                            ((1UL<<30)|(3UL<<28) | (0x3UL<<25) | (24UL<<20) | (3UL<<18) | (0xFUL<<10) | (0UL<<5) |(18UL<<0))
#define UART3_MODULE                            ((1UL<<30)|(3UL<<28) | (0x3UL<<25) | (26UL<<20) | (3UL<<18) | (0xFUL<<10) | (4UL<<5) |(19UL<<0))
#define UART4_MODULE                            ((1UL<<30)|(3UL<<28) | (0x3UL<<25) | (28UL<<20) | (3UL<<18) | (0xFUL<<10) | (8UL<<5) |(20UL<<0))
#define UART5_MODULE                            ((1UL<<30)|(3UL<<28) | (0x3UL<<25) | (30UL<<20) | (3UL<<18) | (0xFUL<<10) | (12UL<<5) |(21UL<<0))

#define UART0SEL_HXT                            (0x0UL << CLK_CLKSEL1_UART0SEL_Pos)        
#define UART0SEL_LXT                            (0x2UL << CLK_CLKSEL1_UART0SEL_Pos)        
#define UART0SEL_PLL                            (0x1UL << CLK_CLKSEL1_UART0SEL_Pos)        
#define UART0SEL_HIRC                           (0x3UL << CLK_CLKSEL1_UART0SEL_Pos)        

#define UART1SEL_HXT                            (0x0UL << CLK_CLKSEL1_UART1SEL_Pos)        
#define UART1SEL_LXT                            (0x2UL << CLK_CLKSEL1_UART1SEL_Pos)        
#define UART1SEL_PLL                            (0x1UL << CLK_CLKSEL1_UART1SEL_Pos)        
#define UART1SEL_HIRC                           (0x3UL << CLK_CLKSEL1_UART1SEL_Pos)        

#define UART2SEL_HXT                            (0x0UL << CLK_CLKSEL3_UART2SEL_Pos)        
#define UART2SEL_LXT                            (0x2UL << CLK_CLKSEL3_UART2SEL_Pos)        
#define UART2SEL_PLL                            (0x1UL << CLK_CLKSEL3_UART2SEL_Pos)        
#define UART2SEL_HIRC                           (0x3UL << CLK_CLKSEL3_UART2SEL_Pos)        

#define UART3SEL_HXT                            (0x0UL << CLK_CLKSEL3_UART3SEL_Pos)        
#define UART3SEL_LXT                            (0x2UL << CLK_CLKSEL3_UART3SEL_Pos)        
#define UART3SEL_PLL                            (0x1UL << CLK_CLKSEL3_UART3SEL_Pos)        
#define UART3SEL_HIRC                           (0x3UL << CLK_CLKSEL3_UART3SEL_Pos)        

#define UART4SEL_HXT                            (0x0UL << CLK_CLKSEL3_UART4SEL_Pos)        
#define UART4SEL_LXT                            (0x2UL << CLK_CLKSEL3_UART4SEL_Pos)        
#define UART4SEL_PLL                            (0x1UL << CLK_CLKSEL3_UART4SEL_Pos)        
#define UART4SEL_HIRC                           (0x3UL << CLK_CLKSEL3_UART4SEL_Pos)        

#define UART5SEL_HXT                            (0x0UL << CLK_CLKSEL3_UART5SEL_Pos)        
#define UART5SEL_LXT                            (0x2UL << CLK_CLKSEL3_UART5SEL_Pos)        
#define UART5SEL_PLL                            (0x1UL << CLK_CLKSEL3_UART5SEL_Pos)        
#define UART5SEL_HIRC                           (0x3UL << CLK_CLKSEL3_UART5SEL_Pos)        
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __UART0SEL(__TYPE)                      UART0SEL_##__TYPE
#define __UART1SEL(__TYPE)                      UART1SEL_##__TYPE
#define __UART2SEL(__TYPE)                      UART2SEL_##__TYPE
#define __UART3SEL(__TYPE)                      UART3SEL_##__TYPE
#define __UART4SEL(__TYPE)                      UART4SEL_##__TYPE
#define __UART5SEL(__TYPE)                      UART5SEL_##__TYPE

#define UART0SEL(__TYPE)                        __UART0SEL(__TYPE)
#define UART1SEL(__TYPE)                        __UART1SEL(__TYPE)
#define UART2SEL(__TYPE)                        __UART2SEL(__TYPE)
#define UART3SEL(__TYPE)                        __UART3SEL(__TYPE)
#define UART4SEL(__TYPE)                        __UART4SEL(__TYPE)
#define UART5SEL(__TYPE)                        __UART5SEL(__TYPE)

#define UART_SEL(__N)                           UART##__N##SEL(VSF_CFG_USART_CLOCK_SOURCE)

#define ____vsf_hw_usart_imp_lv0(__count, __dont_care)                      \
    vsf_usart_t vsf_usart##__count = {                                      \
        .param = {                                                          \
            .usart = (UART##__count),                                       \
            .irq = (UART##__count##_IRQn),                                  \
            .module = (UART##__count##_MODULE),                             \
                .uartx_rst = UART##__count##_RST,                           \
            .clock_source = UART_SEL(__count),                              \
        }                                                                   \
    };

#define __vsf_hw_usart_imp_lv0(__name, __count)                             \
    VSF_MREPEAT(__count, ____vsf_hw_usart_imp_lv0, __name)

#define m480_usart_init(__name, __count)                                    \
    __vsf_hw_usart_imp_lv0(__name, __count)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
m480_usart_init(m480_usart, USART_MAX_PORT);
/*============================ PROTOTYPES ====================================*/

//#ifndef VSF_USART_CFG_INT_FUNC
//WEAK()


static uint32_t CLK_GetPLLClockFreq(void)
{
    uint32_t u32PllFreq = 0UL, u32PllReg;
    uint32_t u32FIN, u32NF, u32NR, u32NO;
    uint8_t au8NoTbl[4] = {1U, 2U, 2U, 4U};

    u32PllReg = CLK->PLLCTL;
    if (u32PllReg & (CLK_PLLCTL_PD_Msk | CLK_PLLCTL_OE_Msk)) {
        u32PllFreq = 0UL;
    } else if ((u32PllReg & CLK_PLLCTL_BP_Msk) == CLK_PLLCTL_BP_Msk) {
        if ((u32PllReg & 0x00080000ul) == 0x00080000ul) {
            u32FIN = __HIRC;
        } else {
            u32FIN = __HXT;
        }
        u32PllFreq = u32FIN;
    } else {
        if ((u32PllReg & 0x00080000ul) == 0x00080000ul) {
            u32FIN = __HIRC;
        } else {
            u32FIN = __HXT;
        }
        u32NO = au8NoTbl[((u32PllReg & CLK_PLLCTL_OUTDIV_Msk) >> CLK_PLLCTL_OUTDIV_Pos)];
        u32NF = ((u32PllReg & CLK_PLLCTL_FBDIV_Msk) >> CLK_PLLCTL_FBDIV_Pos) + 2UL;
        u32NR = ((u32PllReg & CLK_PLLCTL_INDIV_Msk) >> CLK_PLLCTL_INDIV_Pos) + 1UL;
        u32PllFreq = (((u32FIN >> 2) * u32NF) / (u32NR * u32NO) << 2) * 2UL;
    }
    return u32PllFreq;
}

vsf_err_t vsf_usart_init(vsf_usart_t *usart_ptr, usart_cfg_t *cfg_ptr)
{
    uint32_t u32sel = 0U, u32div = 0U;
    uint32_t u32tmpVal = 0UL, u32tmpAddr = 0UL;
    uint32_t u32UartClkSrcSel = 0ul, u32UartClkDivNum = 0ul;
    uint32_t u32ClkTbl[4] = {__HXT, 0ul, __LXT, __HIRC};
    uint32_t u32Baud_Div = 0ul;

    if (((usart_ptr->param.module >> 10) & 0xfful) != 0) {
        if (((usart_ptr->param.module >> 18) & 0x3ul) == 2U) {
            u32div = (uint32_t)&CLK->CLKDIV3;
        } else if (((usart_ptr->param.module >> 18) & 0x3ul) == 3U) {
            u32div = (uint32_t)&CLK->CLKDIV4;
        } else {
            u32div = (uint32_t)&CLK->CLKDIV0 + (((usart_ptr->param.module >> 18) & 0x3ul) * 4U);
        }
        M32(u32div) = (M32(u32div) & (~(((usart_ptr->param.module >> 10) & 0xfful) << ((usart_ptr->param.module >> 5) & 0x1ful)))) | 0;
    }

    if (((usart_ptr->param.module >> 25) & 0x7ul) != 0) {
        u32sel = (uint32_t)&CLK->CLKSEL0 + (((usart_ptr->param.module >> 28) & 0x3ul) * 4U);
        M32(u32sel) = (M32(u32sel) & (~(((usart_ptr->param.module >> 25) & 0x7ul) << ((usart_ptr->param.module >> 20) &0x1ful)))) | usart_ptr->param.clock_source;
    }
    u32tmpVal = (1UL << ((usart_ptr->param.module >> 0) & 0x1ful));
    u32tmpAddr = (uint32_t)&CLK->AHBCLK;
    u32tmpAddr += (((usart_ptr->param.module >> 30) & 0x3ul) * 4UL);
    *(volatile uint32_t *)u32tmpAddr |= u32tmpVal;
    u32tmpVal = (1UL << (usart_ptr->param.uartx_rst & 0x00ffffffUL));
    u32tmpAddr = (uint32_t)&SYS->IPRST0 + ((usart_ptr->param.uartx_rst >> 24UL));
    *(uint32_t *)u32tmpAddr |= u32tmpVal;
    u32tmpVal = ~(1UL << (usart_ptr->param.uartx_rst & 0x00ffffffUL));
    *(uint32_t *)u32tmpAddr &= u32tmpVal;
    
    if (usart_ptr->param.usart == (UART_T *)UART0) {
        u32UartClkSrcSel = ((uint32_t)(CLK->CLKSEL1 & CLK_CLKSEL1_UART0SEL_Msk)) >> CLK_CLKSEL1_UART0SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV0 & CLK_CLKDIV0_UART0DIV_Msk) >> CLK_CLKDIV0_UART0DIV_Pos;
    } else if (usart_ptr->param.usart == (UART_T *)UART1) {
        u32UartClkSrcSel = (CLK->CLKSEL1 & CLK_CLKSEL1_UART1SEL_Msk) >> CLK_CLKSEL1_UART1SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV0 & CLK_CLKDIV0_UART1DIV_Msk) >> CLK_CLKDIV0_UART1DIV_Pos;
    } else if (usart_ptr->param.usart == (UART_T *)UART2) {
        u32UartClkSrcSel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART2SEL_Msk) >> CLK_CLKSEL3_UART2SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV4 & CLK_CLKDIV4_UART2DIV_Msk) >> CLK_CLKDIV4_UART2DIV_Pos;
    } else if (usart_ptr->param.usart == (UART_T *)UART3) {
        u32UartClkSrcSel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART3SEL_Msk) >> CLK_CLKSEL3_UART3SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV4 & CLK_CLKDIV4_UART3DIV_Msk) >> CLK_CLKDIV4_UART3DIV_Pos;
    } else if (usart_ptr->param.usart == (UART_T *)UART4) {
        u32UartClkSrcSel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART4SEL_Msk) >> CLK_CLKSEL3_UART4SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV4 & CLK_CLKDIV4_UART4DIV_Msk) >> CLK_CLKDIV4_UART4DIV_Pos;
    } else if (usart_ptr->param.usart == (UART_T *)UART5) {
        u32UartClkSrcSel = (CLK->CLKSEL3 & CLK_CLKSEL3_UART5SEL_Msk) >> CLK_CLKSEL3_UART5SEL_Pos;
        u32UartClkDivNum = (CLK->CLKDIV4 & CLK_CLKDIV4_UART5DIV_Msk) >> CLK_CLKDIV4_UART5DIV_Pos;
    }
    usart_ptr->param.usart->FUNCSEL = 0x0ul << UART_FUNCSEL_FUNCSEL_Pos;
    usart_ptr->param.usart->LINE =      (cfg_ptr->mode & USART_BIT_LENGTH)
                                    |   (((cfg_ptr->mode & USART_PARITY) >> 16) << UART_LINE_PBE_Pos)
                                    |   (((cfg_ptr->mode & USART_STOPBIT) >> 8) << UART_LINE_NSB_Pos);
    usart_ptr->param.usart->FIFO &= ~(UART_FIFO_RFITL_Msk | UART_FIFO_RTSTRGLV_Msk| UART_FIFO_RXOFF_Msk);

    if (u32UartClkSrcSel == 1ul) {
        u32ClkTbl[u32UartClkSrcSel] = CLK_GetPLLClockFreq();
    }
    if (cfg_ptr->baudrate != 0ul) {
        u32Baud_Div = (((u32ClkTbl[u32UartClkSrcSel] / (u32UartClkDivNum + 1ul)) 
                         + cfg_ptr->baudrate / 2ul) 
                        / cfg_ptr->baudrate) 
                       - 2ul;
        if (u32Baud_Div > 0xFFFFul) {
            usart_ptr->param.usart->BAUD = (0ul | (((u32ClkTbl[u32UartClkSrcSel] / (u32UartClkDivNum + 1ul) + cfg_ptr->baudrate * 8ul) / cfg_ptr->baudrate >> 4ul) - 2ul));
        } else {
            usart_ptr->param.usart->BAUD = (UART_BAUD_BAUDM1_Msk | UART_BAUD_BAUDM0_Msk | u32Baud_Div);
        }
    }
    return VSF_ERR_NONE;
}

fsm_rt_t vsf_usart_enable(vsf_usart_t *usart_ptr)
{
    usart_ptr->is_abled = true;
    return fsm_rt_cpl;
}

fsm_rt_t vsf_usart_disable(vsf_usart_t *usart_ptr)
{
    usart_ptr->is_abled = true;
    return fsm_rt_cpl;
}

usart_status_t vsf_usart_status(vsf_usart_t *usart_ptr)
{
    return  usart_ptr->status;
    //todo: write status
}

uint_fast16_t vsf_usart_fifo_read(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    uint_fast16_t ret_count;
    if (!(usart_ptr->is_abled)) {
        return 0;
    }
    for (ret_count = 0u; ret_count < count;) {
        while((usart_ptr->param.usart->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) != 0);
        *((uint8_t *)buffer_ptr + ret_count) = usart_ptr->param.usart->DAT;
        ret_count++;
    }
    return ret_count;
}

uint_fast16_t vsf_usart_fifo_write(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast16_t count)
{
    uint_fast16_t ret_count;
    if (!(usart_ptr->is_abled)) {
        return 0;
    }
    for (ret_count = 0u; ret_count < count; ret_count++) {
        while(!(usart_ptr->param.usart->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk));
        usart_ptr->param.usart->DAT = *((uint8_t *)buffer_ptr + ret_count);
    }
    return ret_count;
}

bool vsf_usart_fifo_flush(vsf_usart_t *usart_ptr)
{
    while (     (!(usart_ptr->param.usart->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk))
           &&   ((usart_ptr->param.usart->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) != 0));
    return true;
}

void vsf_usart_irq_enable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_RX) {
        usart_ptr->param.usart->INTEN |= UART_INTEN_RDAIEN_Msk;
    }
    if (irq_mask & USART_IRQ_MASK_TX) {
        usart_ptr->param.usart->INTEN |= UART_INTEN_THREIEN_Msk;
    }
    //TODO:
//    if (irq_mask & USART_IRQ_MASK_RX_CPL) {
//        
//    }
//    if (irq_mask & USART_IRQ_MASK_TX_CPL) {
//        
//    }
//    if (irq_mask & USART_IRQ_MASK_RX_ERR) {
//        
//    }
//    if (irq_mask & USART_IRQ_MASK_TX_ERR) {
//        
//    }
}
void vsf_usart_irq_disable(vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_RX) {
        usart_ptr->param.usart->INTEN &= ~(UART_INTEN_RDAIEN_Msk | 0xffffffff);
    }
    if (irq_mask & USART_IRQ_MASK_TX) {
        usart_ptr->param.usart->INTEN &= ~(UART_INTEN_THREIEN_Msk | 0xffffffff);
    }
}

vsf_err_t vsf_usart_request_rx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    if (!(usart_ptr->is_abled)) {
        return VSF_ERR_NOT_READY;
    }
    //todo:
    return VSF_ERR_NONE;
}
vsf_err_t vsf_usart_request_tx(vsf_usart_t *usart_ptr, void *buffer_ptr, uint_fast32_t count)
{
    if (!(usart_ptr->is_abled)) {
        return VSF_ERR_NOT_READY;
    }
    //todo:
    return VSF_ERR_NONE;
}









































/*============================ IMPLEMENTATION ================================*/