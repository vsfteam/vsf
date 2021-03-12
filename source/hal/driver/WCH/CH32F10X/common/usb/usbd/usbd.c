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
#include "./usbd.h"

/*============================ MACROS ========================================*/

#define CH32F10X_USBD_CFG_TRACE_ARR 0
#define CH32F10X_USBD_CFG_TRACE_SYS 1

#define CH32F10X_USBD_CFG_TRACE     CH32F10X_USBD_CFG_TRACE_SYS

/*============================ INCLUDES ======================================*/

#ifdef CH32F10X_USBD_CFG_TRACE
#   if CH32F10X_USBD_CFG_TRACE == CH32F10X_USBD_CFG_TRACE_SYS
#       include "service/trace/vsf_trace.h"
#   endif
#endif

/*============================ MACROS ========================================*/
#define USB_EPR_STAT                3
#define USB_EPR_STAT_DISABLED       0
#define USB_EPR_STAT_STALL          1
#define USB_EPR_STAT_NAK            2
#define USB_EPR_STAT_VALID          3
#define USB_EPR_STAT_RX_OFFSET      12
#define USB_EPR_STAT_TX_OFFSET      4

#define USB_EPR_CTR_RX              (1 << 15)
#define USB_EPR_CTR_TX              (1 << 7)
#define USB_EPR_SETUP               (1 << 11)
#define USB_EPR_TYPE                (3 << 9)
#define USB_EPR_DTOG_RX             (1 << 14)
#define USB_EPR_DTOG_TX             (1 << 6)

#define USB_EPR_TOGGLE_BITS                                                     \
            (   (USB_EPR_STAT_VALID << USB_EPR_STAT_RX_OFFSET)                  \
            |   (USB_EPR_STAT_VALID << USB_EPR_STAT_TX_OFFSET)                  \
            |   USB_EPR_DTOG_RX | USB_EPR_DTOG_TX)

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifdef USB_DC_REG_BASE
#   define __ch32f10x_usbd_get_reg(__usbd)          ((ch32f10x_usbd_reg_t *)USB_DC_REG_BASE)
#endif

#ifdef USB_DC_PMA_BASE
#   define __ch32f10x_usbd_get_pma(__usbd)          ((ch32f10x_usbd_pma_t *)USB_DC_PMA_BASE)
#endif

#ifdef USB_DC_HP_IRQN
#   define __ch32f10x_usbd_get_hp_irqn(__usbd)      ((IRQn_Type)USB_DC_HP_IRQN)
#endif

#ifdef USB_DC_LP_IRQN
#   define __ch32f10x_usbd_get_lp_irqn(__usbd)      ((IRQn_Type)USB_DC_LP_IRQN)
#endif

#ifdef USB_DC_EP_NUM
#   define __ch32f10x_usbd_get_ep_num(__usbd)      ((uint_fast8_t)USB_DC_EP_NUM)
#endif

#ifndef __IO
#   define __IO         volatile
#endif

/*============================ TYPES =========================================*/

typedef struct ch32f10x_usbd_reg_t {
    union {
        struct {
            __IO uint16_t EP0R;         /*!< USB Endpoint 0 register,                   Address offset: 0x00 */
            __IO uint16_t RESERVED0;    /*!< Reserved */
            __IO uint16_t EP1R;         /*!< USB Endpoint 1 register,                   Address offset: 0x04 */
            __IO uint16_t RESERVED1;    /*!< Reserved */
            __IO uint16_t EP2R;         /*!< USB Endpoint 2 register,                   Address offset: 0x08 */
            __IO uint16_t RESERVED2;    /*!< Reserved */
            __IO uint16_t EP3R;         /*!< USB Endpoint 3 register,                   Address offset: 0x0C */
            __IO uint16_t RESERVED3;    /*!< Reserved */
            __IO uint16_t EP4R;         /*!< USB Endpoint 4 register,                   Address offset: 0x10 */
            __IO uint16_t RESERVED4;    /*!< Reserved */
            __IO uint16_t EP5R;         /*!< USB Endpoint 5 register,                   Address offset: 0x14 */
            __IO uint16_t RESERVED5;    /*!< Reserved */
            __IO uint16_t EP6R;         /*!< USB Endpoint 6 register,                   Address offset: 0x18 */
            __IO uint16_t RESERVED6;    /*!< Reserved */
            __IO uint16_t EP7R;         /*!< USB Endpoint 7 register,                   Address offset: 0x1C */
            __IO uint16_t RESERVED7;    /*!< Reserved */
        };
        struct {
            __IO uint16_t EPR;          /*!< USB Endpoint register,                     Address offset: 4 * n */
            __IO uint16_t RESERVED0;    /*!< Reserved */
        } EP[16];
    };
    __IO uint16_t CNTR;                 /*!< Control register,                          Address offset: 0x40 */
    __IO uint16_t RESERVED8;            /*!< Reserved */
    __IO uint16_t ISTR;                 /*!< Interrupt status register,                 Address offset: 0x44 */
    __IO uint16_t RESERVED9;            /*!< Reserved */
    __IO uint16_t FNR;                  /*!< Frame number register,                     Address offset: 0x48 */
    __IO uint16_t RESERVEDA;            /*!< Reserved */
    __IO uint16_t DADDR;                /*!< Device address register,                   Address offset: 0x4C */
    __IO uint16_t RESERVEDB;            /*!< Reserved */
    __IO uint16_t BTABLE;               /*!< Buffer Table address register,             Address offset: 0x50 */
    __IO uint16_t RESERVEDC;            /*!< Reserved */
} ch32f10x_usbd_reg_t;

// pma can only be accessed in 32-bit mode
typedef struct ch32f10x_usbd_pma_t {
    __IO uint32_t ADDR_TX;
    __IO uint32_t COUNT_TX;
    __IO uint32_t ADDR_RX;
    __IO uint32_t COUNT_RX;
} ch32f10x_usbd_pma_t;

#ifdef CH32F10X_USBD_CFG_TRACE
typedef enum vsf_usbd_trace_t {
    VSF_USBD_TRACE_INVALID,
    VSF_USBD_TRACE_TX,
    VSF_USBD_TRACE_RX,
    VSF_USBD_TRACE_TX_SET_STALL,
    VSF_USBD_TRACE_RX_SET_STALL,
    VSF_USBD_TRACE_TX_CLEAR_STALL,
    VSF_USBD_TRACE_RX_CLEAR_STALL,
    VSF_USBD_TRACE_ON_RESET,
    VSF_USBD_TRACE_ON_SETUP,
    VSF_USBD_TRACE_ON_IN,
    VSF_USBD_TRACE_ON_OUT,
    VSF_USBD_TRACE_ON_STATUS_IN,
    VSF_USBD_TRACE_ON_STATUS_OUT,
    VSF_USBD_TRACE_NUM,
} vsf_usbd_trace_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t __ch32f10x_usbd_ep_type[4] = {
    [USB_EP_TYPE_CONTROL]   = 1,
    [USB_EP_TYPE_BULK]      = 0,
    [USB_EP_TYPE_INTERRUPT] = 3,
    [USB_EP_TYPE_ISO]       = 2,
};

#ifdef CH32F10X_USBD_CFG_TRACE
#   if CH32F10X_USBD_CFG_TRACE == CH32F10X_USBD_CFG_TRACE_SYS

static const char * __ch32f10x_usbd_trace_str[VSF_USBD_TRACE_NUM] = {
    STR(VSF_USBD_TRACE_INVALID),
    STR(VSF_USBD_TRACE_TX),
    STR(VSF_USBD_TRACE_RX),
    STR(VSF_USBD_TRACE_TX_SET_STALL),
    STR(VSF_USBD_TRACE_RX_SET_STALL),
    STR(VSF_USBD_TRACE_TX_CLEAR_STALL),
    STR(VSF_USBD_TRACE_RX_CLEAR_STALL),
    STR(VSF_USBD_TRACE_ON_RESET),
    STR(VSF_USBD_TRACE_ON_SETUP),
    STR(VSF_USBD_TRACE_ON_IN),
    STR(VSF_USBD_TRACE_ON_OUT),
    STR(VSF_USBD_TRACE_ON_STATUS_IN),
    STR(VSF_USBD_TRACE_ON_STATUS_OUT),
};

#   endif
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef CH32F10X_USBD_CFG_TRACE
static void __ch32f10x_usbd_trace(vsf_usbd_trace_t item)
{
#   if CH32F10X_USBD_CFG_TRACE == CH32F10X_USBD_CFG_TRACE_ARR
    static volatile ROOT vsf_usbd_trace_t __trace_items[512];
    static int __trace_pos = 0;

    if (__trace_pos < dimof(__trace_items)) {
        __trace_items[__trace_pos++] = item;
    }
#   elif CH32F10X_USBD_CFG_TRACE == CH32F10X_USBD_CFG_TRACE_SYS
    vsf_trace_debug("usbd: %s" VSF_TRACE_CFG_LINEEND, __ch32f10x_usbd_trace_str[item]);
#   endif
}
#else
static void __ch32f10x_usbd_trace(vsf_usbd_trace_t item){}
#endif

#ifndef USB_DC_REG_BASE
static ch32f10x_usbd_reg_t * __ch32f10x_usbd_get_reg(ch32f10x_usbd_t *usbd)
{
    return (ch32f10x_usbd_reg_t *)usbd->param->reg;
}
#endif

#ifndef USB_DC_PMA_BASE
static ch32f10x_usbd_pma_t * __ch32f10x_usbd_get_pma(ch32f10x_usbd_t *usbd)
{
    return (ch32f10x_usbd_pma_t *)usbd->param->pma;
}
#endif

#ifndef USB_DC_HP_IRQN
static IRQn_Type __ch32f10x_usbd_get_hp_irqn(ch32f10x_usbd_t *usbd)
{
    return usbd->hp_irq;
}
#endif

#ifndef USB_DC_LP_IRQN
static IRQn_Type __ch32f10x_usbd_get_lp_irqn(ch32f10x_usbd_t *usbd)
{
    return usbd->lp_irq;
}
#endif

#ifndef USB_DC_EP_NUM
static uint_fast8_t __ch32f10x_usbd_get_ep_num(ch32f10x_usbd_t *usbd)
{
    return usbd->ep_num;
}
#endif

static int_fast8_t __ch32f10x_usbd_get_idx(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t ep_num = __ch32f10x_usbd_get_ep_num(usbd);
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);

    ep &= 0x0F;
    for (int_fast8_t idx = 0; idx < ep_num; idx++) {
        if ((reg->EP[idx].EPR & 0xF) == ep) {
            return idx;
        }
    }
    return -1;
}

static int_fast8_t __ch32f10x_usbd_get_free_idx(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    uint8_t ep_num = __ch32f10x_usbd_get_ep_num(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        for (idx = 0; idx < ep_num; idx++) {
            if (!(usbd->ep_size_in[idx] + usbd->ep_size_out[idx])) {
                return idx;
            }
        }
    }
    return idx;
}

static void __ch32f10x_usbd_set_state(volatile uint16_t *ep_reg, uint_fast8_t state, uint_fast8_t offset)
{
    uint_fast16_t mask = ~USB_EPR_TOGGLE_BITS | (USB_EPR_STAT << offset);
    uint_fast16_t value = (*ep_reg & mask) | USB_EPR_CTR_RX | USB_EPR_CTR_TX;
    *ep_reg = value ^ (state << offset);
}

static void __ch32f10x_usbd_set_rx_state(volatile uint16_t *ep_reg, uint_fast8_t state)
{
    __ch32f10x_usbd_set_state(ep_reg, state, USB_EPR_STAT_RX_OFFSET);
}

static void __ch32f10x_usbd_set_tx_state(volatile uint16_t *ep_reg, uint_fast8_t state)
{
    __ch32f10x_usbd_set_state(ep_reg, state, USB_EPR_STAT_TX_OFFSET);
}

static void __ch32f10x_usbd_set_rx_count(ch32f10x_usbd_pma_t *pma, uint_fast16_t count)
{
    uint_fast16_t block;
    if (count > 62) {
        block = count >> 5;
        if (!(count & 0x1F)) {
            block--;
        }
        pma->COUNT_RX = (uint32_t)((block << 10) | 0x8000);
    } else {
        block = count >> 1;
        if (count & 1) {
            block++;
        }
        pma->COUNT_RX = (uint32_t)(block << 10);
    }
}

static void __ch32f10x_usbd_ep_reset(ch32f10x_usbd_t *usbd)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    uint8_t ep_num = __ch32f10x_usbd_get_ep_num(usbd);

    for (uint_fast8_t i = 0; i < ep_num; i++) {
        reg->EP[i].EPR &= USB_EPR_TOGGLE_BITS;
    }
}

vsf_err_t ch32f10x_usbd_init(ch32f10x_usbd_t *usbd, usb_dc_cfg_t *cfg)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);

    usbd->callback.evt_handler = cfg->evt_handler;
    usbd->callback.param = cfg->param;

    switch (CH32F10X_SYS_FREQ_HZ) {
    case 72UL * 1000 * 1000:
        RCC->CFGR0 &= ~RCC_USBPRE;
        break;
    case 48UL * 1000 * 1000:
        RCC->CFGR0 |= RCC_USBPRE;
        break;
    default:
        VSF_HAL_ASSERT(false);
        return VSF_ERR_INVALID_PARAMETER;
    }
    RCC->APB1PCENR |= RCC_USBEN;

    // reset
    reg->CNTR = USB_CNTR_FRES;
    reg->CNTR = 0;

    // 8 nop for resetting
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");

    // USB reset logic will not reset EP, reset them manually
    __ch32f10x_usbd_ep_reset(usbd);

    reg->ISTR = 0;
    reg->CNTR = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_ERRM | USB_CNTR_RESETM;
    reg->BTABLE = 0;

    if (cfg->priority >= 0) {
        NVIC_SetPriority(__ch32f10x_usbd_get_hp_irqn(usbd), cfg->priority);
        NVIC_EnableIRQ(__ch32f10x_usbd_get_hp_irqn(usbd));
        NVIC_SetPriority(__ch32f10x_usbd_get_lp_irqn(usbd), cfg->priority);
        NVIC_EnableIRQ(__ch32f10x_usbd_get_lp_irqn(usbd));
    }
    return VSF_ERR_NONE;
}

void ch32f10x_usbd_fini(ch32f10x_usbd_t *usbd)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);

    // reset
    reg->CNTR = USB_CNTR_FRES;
    reg->ISTR = 0;

    reg->CNTR = USB_CNTR_FRES | USB_CNTR_PDWN;
    NVIC_DisableIRQ(__ch32f10x_usbd_get_hp_irqn(usbd));
    NVIC_DisableIRQ(__ch32f10x_usbd_get_lp_irqn(usbd));
}

void ch32f10x_usbd_reset(ch32f10x_usbd_t *usbd, usb_dc_cfg_t *cfg)
{
    // USB reset logic will not reset EP, reset them manually
    __ch32f10x_usbd_ep_reset(usbd);

    usbd->ep_buf_ptr = 0x200;
    usbd->is_status_in = false;
    usbd->is_status_out = false;
    memset(usbd->ep_size, 0, sizeof(usbd->ep_size));
}

void ch32f10x_usbd_connect(ch32f10x_usbd_t *usbd)
{
    // set USBDPU, TODO: use MACRO when avialable
    EXTEN->EXTEN_CTR |= 1 << 1;
}

void ch32f10x_usbd_disconnect(ch32f10x_usbd_t *usbd)
{
    // clear USBDPU, TODO: use MACRO when avialable
    EXTEN->EXTEN_CTR &= ~(1 << 1);
}

void ch32f10x_usbd_wakeup(ch32f10x_usbd_t *usbd)
{
    
}

void ch32f10x_usbd_set_address(ch32f10x_usbd_t *usbd, uint_fast8_t addr)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    reg->DADDR = addr | USB_DADDR_EF;
}

uint_fast8_t ch32f10x_usbd_get_address(ch32f10x_usbd_t *usbd)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    return reg->DADDR & USB_DADDR_ADD;
}

uint_fast16_t ch32f10x_usbd_get_frame_number(ch32f10x_usbd_t *usbd)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    return reg->FNR & USB_FNR_FN;
}

uint_fast8_t ch32f10x_usbd_get_mframe_number(ch32f10x_usbd_t *usbd)
{
    return 0;
}

void ch32f10x_usbd_get_setup(ch32f10x_usbd_t *usbd, uint8_t *buffer)
{
    VSF_HAL_ASSERT(8 == ch32f10x_usbd_ep_get_data_size(usbd, 0));
    ch32f10x_usbd_ep_transaction_read_buffer(usbd, 0, buffer, 8);
}

void ch32f10x_usbd_status_stage(ch32f10x_usbd_t *usbd, bool is_in)
{
    if (is_in) {
        usbd->is_status_in = true;
        ch32f10x_usbd_ep_transaction_set_data_size(usbd, 0x80, 0);
    }
    // status out is enabled while sending short-packet or zlp
}

uint_fast8_t ch32f10x_usbd_ep_get_feature(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint_fast8_t feature)
{
    return 0;
}

vsf_err_t ch32f10x_usbd_ep_add(ch32f10x_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    if (usbd->ep_buf_ptr < size) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    ch32f10x_usbd_pma_t *pma = __ch32f10x_usbd_get_pma(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_free_idx(usbd, ep);

    if ((idx < 0) || ((usbd->ep_buf_ptr - size) < __ch32f10x_usbd_get_ep_num(usbd) * sizeof(ch32f10x_usbd_pma_t))) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    reg->EP[idx].EPR = (__ch32f10x_usbd_ep_type[type] << 9) | (ep & 0xF) | USB_EPR_CTR_RX | USB_EPR_CTR_TX;

    // fix for 16-bit aligned memory
    usbd->ep_buf_ptr -= size & 1 ? size + 1 : size;
    if (ep & 0x80) {
        usbd->ep_size_in[idx] = size;
        pma[idx].ADDR_TX = usbd->ep_buf_ptr;
        __ch32f10x_usbd_set_tx_state(&reg->EP[idx].EPR, USB_EPR_STAT_NAK);
    } else {
        usbd->ep_size_out[idx] = size;
        pma[idx].ADDR_RX = usbd->ep_buf_ptr;
        __ch32f10x_usbd_set_rx_count(&pma[idx], size);
        __ch32f10x_usbd_set_rx_state(&reg->EP[idx].EPR, !(ep & 0x0F) ? USB_EPR_STAT_VALID : USB_EPR_STAT_NAK);
    }
    return VSF_ERR_NONE;
}

uint_fast16_t ch32f10x_usbd_ep_get_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return 0;
    }

    if (ep & 0x80) {
        return usbd->ep_size_in[idx];
    } else {
        return usbd->ep_size_out[idx];
    }
}

vsf_err_t ch32f10x_usbd_ep_set_stall(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    if (ep & 0x80) {
        __ch32f10x_usbd_trace(VSF_USBD_TRACE_TX_SET_STALL);
        __ch32f10x_usbd_set_tx_state(&reg->EP[idx].EPR, USB_EPR_STAT_STALL);
    } else {
        __ch32f10x_usbd_trace(VSF_USBD_TRACE_RX_SET_STALL);
        __ch32f10x_usbd_set_rx_state(&reg->EP[idx].EPR, USB_EPR_STAT_STALL);
    }
    return VSF_ERR_NONE;
}

bool ch32f10x_usbd_ep_is_stalled(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);
    uint_fast8_t offset;

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return true;
    }

    offset = (ep & 0x80) ? 4 : 12;
    return ((reg->EP[idx].EPR >> offset) & USB_EPR_STAT) == USB_EPR_STAT_STALL;
}

vsf_err_t ch32f10x_usbd_ep_clear_stall(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    if (ep & 0x80) {
        __ch32f10x_usbd_trace(VSF_USBD_TRACE_TX_CLEAR_STALL);
        __ch32f10x_usbd_set_tx_state(&reg->EP[idx].EPR, USB_EPR_STAT_NAK);
    } else {
        __ch32f10x_usbd_trace(VSF_USBD_TRACE_RX_CLEAR_STALL);
        __ch32f10x_usbd_set_rx_state(&reg->EP[idx].EPR,
                        !(ep & 0xF) ? USB_EPR_STAT_VALID : USB_EPR_STAT_NAK);
    }
    return VSF_ERR_NONE;
}

uint_fast32_t ch32f10x_usbd_ep_get_data_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    VSF_HAL_ASSERT(!(ep & 0x80));
    ch32f10x_usbd_pma_t *pma = __ch32f10x_usbd_get_pma(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return 0;
    }

    return pma[idx].COUNT_RX & 0x3FF;
}

vsf_err_t ch32f10x_usbd_ep_transaction_read_buffer(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(!(ep & 0x80) && (size <= ch32f10x_usbd_ep_get_data_size(usbd, ep)));
    ch32f10x_usbd_pma_t *pma = __ch32f10x_usbd_get_pma(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);
    uint32_t *usb_buffer;
    uint_fast16_t tmp16;

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    usb_buffer = (uint32_t *)&(((uint16_t *)pma)[pma[idx].ADDR_RX]);
    while (size > 0) {
        tmp16 = *usb_buffer++;
        if (size > 1) {
            *(uint16_t *)buffer = tmp16;
            buffer += 2;
            size -= 2;
        } else {
            *buffer = tmp16;
            size -= 1;
        }
    }
    return VSF_ERR_NONE;
}

vsf_err_t ch32f10x_usbd_ep_transaction_enable_out(ch32f10x_usbd_t *usbd, uint_fast8_t ep)
{
    VSF_HAL_ASSERT(!(ep & 0x80));
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    __ch32f10x_usbd_trace(VSF_USBD_TRACE_RX);
    __ch32f10x_usbd_set_rx_state(&reg->EP[idx].EPR, USB_EPR_STAT_VALID);
    return VSF_ERR_NONE;
}

vsf_err_t ch32f10x_usbd_ep_transaction_set_data_size(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    VSF_HAL_ASSERT(ep & 0x80);
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    ch32f10x_usbd_pma_t *pma = __ch32f10x_usbd_get_pma(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    __ch32f10x_usbd_trace(VSF_USBD_TRACE_TX);
    pma[idx].COUNT_TX = size;
    __ch32f10x_usbd_set_tx_state(&reg->EP[idx].EPR, USB_EPR_STAT_VALID);

    if (!usbd->is_status_in && (0x80 == ep)) {
        // it will fail if recv status OUT after data sent
        //  so recv status OUT here as a workaround
        if (size < usbd->ep_size_in[0]) {
            usbd->is_status_out = true;
            // not necessary to enable status out, it seems to be enabled by hw
            // but no harm here, maybe
            ch32f10x_usbd_ep_transaction_enable_out(usbd, 0);
        }
    }
    return VSF_ERR_NONE;
}

vsf_err_t ch32f10x_usbd_ep_transaction_write_buffer(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_HAL_ASSERT(ep & 0x80);
    ch32f10x_usbd_pma_t *pma = __ch32f10x_usbd_get_pma(usbd);
    int_fast8_t idx = __ch32f10x_usbd_get_idx(usbd, ep);
    uint16_t *usb_buffer;

    if (idx < 0) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_FAIL;
    }

    usb_buffer = &(((uint16_t *)pma)[pma[idx].ADDR_TX]);
    for (uint_fast16_t i = (size + 1) >> 1; i > 0; i--) {
        *usb_buffer = get_unaligned_le16(buffer);
        usb_buffer += 2;
        buffer += 2;
    }
    return VSF_ERR_NONE;
}

vsf_err_t ch32f10x_usbd_ep_transfer_recv(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t ch32f10x_usbd_ep_transfer_send(ch32f10x_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    return VSF_ERR_NOT_SUPPORT;
}

static void ch32f10x_usbd_notify(ch32f10x_usbd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evt_handler != NULL) {
        usbd->callback.evt_handler(usbd->callback.param, evt, value);
    }
}

void ch32f10x_usbd_irq(ch32f10x_usbd_t *usbd)
{
    ch32f10x_usbd_reg_t *reg = __ch32f10x_usbd_get_reg(usbd);
    uint_fast16_t status = reg->ISTR & reg->CNTR;

    if (status & USB_ISTR_RESET) {
        reg->ISTR = ~USB_ISTR_RESET;
        __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_RESET);
        ch32f10x_usbd_notify(usbd, USB_ON_RESET, 0);
    }
    if (status & USB_ISTR_PMAOVR) {
        reg->ISTR = ~USB_ISTR_PMAOVR;
    }
    if (status & USB_ISTR_ERR) {
        reg->ISTR = ~USB_ISTR_ERR;
        // TODO: notify error
    }
    if (status & USB_ISTR_WKUP) {
        reg->ISTR = ~USB_ISTR_WKUP;
        // TODO: notify wakup
    }
    if (status & USB_ISTR_SUSP) {
        reg->ISTR = ~USB_ISTR_SUSP;
        // TODO: notify suspend
    }
    if (status & USB_ISTR_SOF) {
        reg->ISTR = ~USB_ISTR_SOF;
        // TODO: notify SOF
    }
    if (status & USB_ISTR_ESOF) {
        reg->ISTR = ~USB_ISTR_ESOF;
        // TODO: notify error
    }
    if (status & USB_ISTR_CTR) {
        uint_fast8_t idx, ep_num;
        uint_fast16_t ep_reg;

        while ((status = reg->ISTR) & USB_ISTR_CTR) {
            idx = status & USB_ISTR_EP_ID;
            ep_reg = reg->EP[idx].EPR & ~USB_EPR_TOGGLE_BITS;
            ep_num = ep_reg & 0xF;

            if (0 == ep_num) {
                if (status & USB_ISTR_DIR) {
                    reg->EP[idx].EPR = (ep_reg & ~USB_EPR_CTR_RX) | USB_EPR_CTR_TX;
                    if (ep_reg & USB_EPR_SETUP) {
                        __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_SETUP);
                        ch32f10x_usbd_notify(usbd, USB_ON_SETUP, 0);
                    } else if (ep_reg & USB_EPR_CTR_RX) {
                        if (usbd->is_status_out) {
                            usbd->is_status_out = false;
                            __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_STATUS_OUT);
                            ch32f10x_usbd_notify(usbd, USB_ON_STATUS, 0);
                        } else {
                            __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_OUT);
                            ch32f10x_usbd_notify(usbd, USB_ON_OUT, ep_num);
                        }
                    }
                } else {
                    reg->EP[idx].EPR = (ep_reg & ~USB_EPR_CTR_TX) | USB_EPR_CTR_RX;
                    if (usbd->is_status_in) {
                        usbd->is_status_in = false;
                        __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_STATUS_IN);
                        ch32f10x_usbd_notify(usbd, USB_ON_STATUS, 0);
                    } else {
                        __ch32f10x_usbd_trace(VSF_USBD_TRACE_ON_IN);
                        ch32f10x_usbd_notify(usbd, USB_ON_IN, ep_num);
                    }
                }
            } else {
                if (ep_reg & USB_EPR_CTR_RX) {
                    reg->EP[idx].EPR = (ep_reg & ~USB_EPR_CTR_RX) | USB_EPR_CTR_TX;
                    ch32f10x_usbd_notify(usbd, USB_ON_OUT, ep_num);
                }
                if (ep_reg & USB_EPR_CTR_TX) {
                    reg->EP[idx].EPR = (ep_reg & ~USB_EPR_CTR_TX) | USB_EPR_CTR_RX;
                    ch32f10x_usbd_notify(usbd, USB_ON_IN, ep_num);
                }
            }
        }
    }
}
