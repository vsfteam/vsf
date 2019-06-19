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

#include "../../common.h"
#include "./usbd_hs.h"
//#define VSF_HAL_USBD_TRACE_EN           ENABLED
#if VSF_HAL_USBD_TRACE_EN == ENABLED
#include "./service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

// CEPCTL
#define USB_CEPCTL_NAKCLR                   ((uint32_t)0x00000000)
#define USB_CEPCTL_STALL                    ((uint32_t)0x00000002)
#define USB_CEPCTL_ZEROLEN                  ((uint32_t)0x00000004)
#define USB_CEPCTL_FLUSH                    ((uint32_t)0x00000008)

// EPxCFG
#define USB_EP_CFG_VALID                    ((uint32_t)0x00000001)
#define USB_EP_CFG_TYPE_BULK                ((uint32_t)0x00000002)
#define USB_EP_CFG_TYPE_INT                 ((uint32_t)0x00000004)
#define USB_EP_CFG_TYPE_ISO                 ((uint32_t)0x00000006)
#define USB_EP_CFG_TYPE_MASK                ((uint32_t)0x00000006)
#define USB_EP_CFG_DIR_OUT                  ((uint32_t)0x00000000)
#define USB_EP_CFG_DIR_IN                   ((uint32_t)0x00000008)

// EPxRSPCTL
#define USB_EP_RSPCTL_FLUSH                 ((uint32_t)0x00000001)
#define USB_EP_RSPCTL_MODE_AUTO             ((uint32_t)0x00000000)
#define USB_EP_RSPCTL_MODE_MANUAL           ((uint32_t)0x00000002)
#define USB_EP_RSPCTL_MODE_FLY              ((uint32_t)0x00000004)
#define USB_EP_RSPCTL_MODE_MASK             ((uint32_t)0x00000006)
#define USB_EP_RSPCTL_TOGGLE                ((uint32_t)0x00000008)
#define USB_EP_RSPCTL_HALT                  ((uint32_t)0x00000010)
#define USB_EP_RSPCTL_ZEROLEN               ((uint32_t)0x00000020)
#define USB_EP_RSPCTL_SHORTTXEN             ((uint32_t)0x00000040)
#define USB_EP_RSPCTL_DISBUF                ((uint32_t)0x00000080)

#define M480_USBD_EP_REG(__IDX, __REG)                                          \
    *((__IO uint32_t *)((uint32_t)&reg->__REG + (uint32_t)((uint8_t)(__IDX) * 0x28)))
#define M480_USBD_EP_REG8(__IDX, __REG)                                         \
    *((__IO uint8_t *)((uint32_t)&reg->__REG + (uint32_t)((uint8_t)(__IDX) * 0x28)))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const uint8_t m480_usbd_hs_ep_type[4] = {
    [USB_EP_TYPE_CONTROL]   = 0 << 1,
    [USB_EP_TYPE_BULK]      = 1 << 1,
    [USB_EP_TYPE_INTERRUPT] = 2 << 1,
    [USB_EP_TYPE_ISO]       = 3 << 1,
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



static HSUSBD_T * m480_usbd_hs_get_reg(m480_usbd_hs_t *usbd_hs)
{
    return usbd_hs->param->reg;
}

static int_fast8_t m480_usbd_hs_get_idx(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg;

    if (!(ep & 0x0F)) {
        return ep >> 7;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    for (int_fast8_t cfg, idx = 0; idx < (m480_usbd_hs_ep_number - 2); idx++) {
        cfg = M480_USBD_EP_REG(idx, EP[0].EPCFG);
        if (cfg & USB_EP_CFG_VALID) {
            cfg = ((cfg & 0xF0) >> 4) | ((cfg & 0x08) << 4);
            if (ep == cfg) {
                return idx + 2;
            }
        }
    }
    return -1;
}

static int_fast8_t m480_usbd_hs_get_free_idx(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg;

    if (!(ep & 0x0F)) {
        return ep >> 7;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    for (int_fast8_t cfg, idx = 0; idx < (m480_usbd_hs_ep_number - 2); idx++) {
        cfg = M480_USBD_EP_REG(idx, EP[0].EPCFG);
        if (!(cfg & USB_EP_CFG_VALID)) {
            return idx + 2;
        }
    }
    return -1;
}

vsf_err_t m480_usbd_hs_init(m480_usbd_hs_t *usbd_hs, usb_dc_cfg_t *cfg)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);

    usbd_hs->callback.evt_handler = cfg->evt_handler;
    usbd_hs->callback.param = cfg->param;

    m480_enable_usbphy(M480_USBPHY_HS, M480_USBPHY_DEV);

    // TODO: use pm to config clock
    CLK->AHBCLK |= CLK_AHBCLK_HSUSBDCKEN_Msk;

    reg->PHYCTL |= HSUSBD_PHYCTL_PHYEN_Msk;
    while (1) {
        reg->EP[0ul].EPMPS = 0x20ul;
        if (reg->EP[0ul].EPMPS == 0x20ul) {
            break;
        }
    }

    switch (cfg->speed) {
    case USB_DC_SPEED_FULL:
        reg->OPER = 0;
        break;
    case USB_DC_SPEED_HIGH:
        reg->OPER = HSUSBD_OPER_HISPDEN_Msk;
        break;
    default:
        ASSERT(false);
        return VSF_ERR_NOT_SUPPORT;
    }

    // 8 nop for reg sync
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");
    __asm("nop");

    // Enable USB interrupt
    reg->GINTEN = 0;
    // Enable BUS interrupt
    reg->BUSINTEN = HSUSBD_BUSINTEN_RSTIEN_Msk;
    reg->CEPINTEN = HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_RXPKIEN_Msk |
            HSUSBD_CEPINTEN_TXPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk;
    // Enable USB interrupt
    reg->GINTEN = HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk;

    if (cfg->priority >= 0) {
        IRQn_Type irq = usbd_hs->param->irq;
        NVIC_SetPriority(irq, (uint32_t)cfg->priority);
        NVIC_EnableIRQ(irq);
    }
    return VSF_ERR_NONE;
}

void m480_usbd_hs_fini(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->PHYCTL &= ~HSUSBD_PHYCTL_PHYEN_Msk;
    reg->GINTEN = 0;
    NVIC_DisableIRQ(usbd_hs->param->irq);
    // TODO: use pm to config clock
    CLK->AHBCLK &= ~CLK_AHBCLK_HSUSBDCKEN_Msk;
}

void m480_usbd_hs_reset(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    usbd_hs->ep_buf_ptr = 0x1000;
#ifdef M480_USBD_HS_WROKAROUND_ISO
    usbd_hs->ep_tx_mask = 0;
#endif
    for (uint_fast8_t i = 0; i < (m480_usbd_hs_ep_number - 2); i++) {
        M480_USBD_EP_REG(i, EP[0].EPCFG) = 0;
        M480_USBD_EP_REG(i, EP[0].EPINTEN) = 0;
    }
}

void m480_usbd_hs_connect(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->PHYCTL |= HSUSBD_PHYCTL_DPPUEN_Msk;
}

void m480_usbd_hs_disconnect(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->PHYCTL &= ~HSUSBD_PHYCTL_DPPUEN_Msk;
}

void m480_usbd_hs_wakeup(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->OPER |= HSUSBD_OPER_RESUMEEN_Msk;
}

void m480_usbd_hs_set_address(m480_usbd_hs_t *usbd_hs, uint_fast8_t addr)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->FADDR = addr;
}

uint_fast8_t m480_usbd_hs_get_address(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    return reg->FADDR;
}

uint_fast16_t m480_usbd_hs_get_frame_number(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    return reg->FRAMECNT >> 3;
}

uint_fast8_t m480_usbd_hs_get_mframe_number(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    return reg->FRAMECNT & 7;
}

void m480_usbd_hs_get_setup(m480_usbd_hs_t *usbd_hs, uint8_t *buffer)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    uint_fast16_t temp = reg->SETUP1_0;
    buffer[0] = temp & 0xFF;
    buffer[1] = temp >> 8;
    temp = reg->SETUP3_2;
    buffer[2] = temp & 0xFF;
    buffer[3] = temp >> 8;
    temp = reg->SETUP5_4;
    buffer[4] = temp & 0xFF;
    buffer[5] = temp >> 8;
    temp = reg->SETUP7_6;
    buffer[6] = temp & 0xFF;
    buffer[7] = temp >> 8;
}

void m480_usbd_hs_status_stage(m480_usbd_hs_t *usbd_hs, bool is_in)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    reg->CEPCTL = USB_CEPCTL_NAKCLR;
}

vsf_err_t m480_usbd_hs_ep_add(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if (usbd_hs->ep_buf_ptr < size) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_free_idx(usbd_hs, ep);
    if (idx < 0) {
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }

    if (idx <= 1) {
        if (0 == idx) {
            usbd_hs->ep_buf_ptr -= size & 1 ? size + 1 : size;
            reg->CEPBUFST = usbd_hs->ep_buf_ptr;
            reg->CEPBUFEND = usbd_hs->ep_buf_ptr + size - 1;
        }
    } else {
        idx -= 2;
        ep = ((ep & 0x80) >> 4) | ((ep & 0x0F) << 4);
        usbd_hs->ep_buf_ptr -= size & 1 ? size + 1 : size;
        M480_USBD_EP_REG(idx, EP[0].EPBUFST)    = usbd_hs->ep_buf_ptr;
        M480_USBD_EP_REG(idx, EP[0].EPBUFEND)   = usbd_hs->ep_buf_ptr + size - 1;
        M480_USBD_EP_REG(idx, EP[0].EPMPS)      = size;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL)   = USB_EP_RSPCTL_FLUSH | USB_EP_RSPCTL_MODE_MANUAL;
        M480_USBD_EP_REG(idx, EP[0].EPCFG)      = ep | m480_usbd_hs_ep_type[type] | USB_EP_CFG_VALID;
        if (ep & 0x08) {
            M480_USBD_EP_REG(idx, EP[0].EPINTEN)= HSUSBD_EPINTEN_TXPKIEN_Msk;
        }
        reg->GINTEN |= HSUSBD_GINTEN_EPAIEN_Msk << idx;
    }
    return VSF_ERR_NONE;
}

uint_fast16_t m480_usbd_hs_ep_get_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    int_fast8_t idx = m480_usbd_hs_get_idx(usbd_hs, ep);

    if (idx < 0) {
        return 0;
    }

    if (idx <= 1) {
        return reg->CEPBUFEND - reg->CEPBUFST + 1;
    } else {
        idx -= 2;
        return M480_USBD_EP_REG(idx, EP[0].EPMPS);
    }
}

vsf_err_t m480_usbd_hs_ep_set_stall(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    int_fast8_t idx = m480_usbd_hs_get_idx(usbd_hs, ep);

    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
        reg->CEPCTL = USB_CEPCTL_STALL;
        reg->CEPCTL |= USB_CEPCTL_FLUSH;
    } else {
        idx -= 2;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) =
            (M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) & 0xF7) | USB_EP_RSPCTL_HALT;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
    }
    return VSF_ERR_NONE;
}

bool m480_usbd_hs_ep_is_stalled(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    int_fast8_t idx = m480_usbd_hs_get_idx(usbd_hs, ep);

    if (idx < 0) {
        return true;
    }

    if (idx <= 1) {
        return (reg->CEPCTL & USB_CEPCTL_STALL) > 0;
    } else {
        idx -= 2;
        return (M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) & USB_EP_RSPCTL_HALT) > 0;
    }
}

vsf_err_t m480_usbd_hs_ep_clear_stall(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    int_fast8_t idx = m480_usbd_hs_get_idx(usbd_hs, ep);

    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
        reg->CEPCTL &= ~(USB_CEPCTL_STALL | USB_CEPCTL_NAKCLR);
    } else {
        idx -= 2;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) &= ~USB_EP_RSPCTL_HALT;
        M480_USBD_EP_REG(idx, EP[0].EPRSPCTL) |= USB_EP_RSPCTL_TOGGLE;
    }
    return VSF_ERR_NONE;
}

uint_fast16_t m480_usbd_hs_ep_get_data_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if (ep & 0x80) {
        return 0;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_idx(usbd_hs, ep);
    if (idx < 0) {
        return 0;
    }

    if (idx <= 1) {
        return reg->CEPRXCNT;
    } else {
        idx -= 2;
        return M480_USBD_EP_REG(idx, EP[0].EPDATCNT) & 0xFFFF;
    }
}

vsf_err_t m480_usbd_hs_ep_read_buffer(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if ((ep & 0x80) || (size > m480_usbd_hs_ep_get_data_size(usbd_hs, ep))) {
        return VSF_ERR_BUG;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_idx(usbd_hs, ep);
    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
        for (uint_fast16_t i = 0; i < size; i++) {
            buffer[i] = reg->CEPDAT_BYTE;
        }
    } else {
        idx -= 2;
        // EPJ - EPL has BUG in DWORD read
        if ((idx < 8) && !((uint32_t)buffer & 0x03)) {
            while (size > 4) {
                *(uint32_t *)buffer = M480_USBD_EP_REG(idx, EP[0].EPDAT);
                size -= 4;
                buffer += 4;
            }
        }
        while (size > 0) {
            *buffer = M480_USBD_EP_REG8(idx, EP[0].EPDAT_BYTE);
            size -= 1;
            buffer += 1;
        }
    }
    return VSF_ERR_NONE;
}

vsf_err_t m480_usbd_hs_ep_enable_OUT(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if (ep & 0x80) {
        return VSF_ERR_BUG;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_idx(usbd_hs, ep);
    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
    } else {
        idx -= 2;
        M480_USBD_EP_REG(idx, EP[0].EPINTEN) |= HSUSBD_EPINTEN_RXPKIEN_Msk;
    }
    return VSF_ERR_NONE;
}

vsf_err_t m480_usbd_hs_ep_set_data_size(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint_fast16_t size)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if (!(ep & 0x80)) {
        return VSF_ERR_BUG;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_idx(usbd_hs, ep);
    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
        reg->CEPTXCNT = size;
    } else {
        idx -= 2;
#ifdef M480_USBD_HS_WROKAROUND_ISO
        uint32_t ep_int_en = M480_USBD_EP_REG(idx, EP[0].EPINTEN) & HSUSBD_EPINTEN_TXPKIEN_Msk;
        M480_USBD_EP_REG(idx, EP[0].EPINTEN) &= ~HSUSBD_EPINTEN_TXPKIEN_Msk;
            ASSERT(!(usbd_hs->ep_tx_mask & (1 << idx)));
            M480_USBD_EP_REG(idx, EP[0].EPTXCNT) = size;
            usbd_hs->retry_cnt[idx] = 0;
            usbd_hs->tx_size[idx] = size;
            usbd_hs->ep_tx_mask |= 1 << idx;
        M480_USBD_EP_REG(idx, EP[0].EPINTEN) |= ep_int_en;
#else
        M480_USBD_EP_REG(idx, EP[0].EPTXCNT) = size;
#endif

#if VSF_HAL_USBD_TRACE_EN == ENABLED
        vsf_trace(0, "set ep%d DATSIZE to %d.\r\n", idx, size);
//        vsf_trace(0, "EPTXCNT=%d,EPDATCNT=%d\r\n",
//                              M480_USBD_EP_REG(idx, EP[0].EPTXCNT),
//                              M480_USBD_EP_REG(idx, EP[0].EPDATCNT) & 0xFFFF);
#endif
    }
    return VSF_ERR_NONE;
}

vsf_err_t m480_usbd_hs_ep_write_buffer(m480_usbd_hs_t *usbd_hs, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    HSUSBD_T *reg;
    int_fast8_t idx;

    if (!(ep & 0x80)) {
        return VSF_ERR_BUG;
    }

    reg = m480_usbd_hs_get_reg(usbd_hs);
    idx = m480_usbd_hs_get_idx(usbd_hs, ep);
    if (idx < 0) {
        return VSF_ERR_FAIL;
    }

    if (idx <= 1) {
        reg->CEPCTL = USB_CEPCTL_FLUSH;
        for (uint_fast16_t i = 0; i < size; i++) {
            reg->CEPDAT_BYTE = buffer[i];
        }
    } else {
        idx -= 2;

#if VSF_HAL_USBD_TRACE_EN == ENABLED
        vsf_trace(0, "write ep%d buffer %d bytes.\r\n", idx, size);
#endif

        // EPJ - EPL has BUG in DWORD write
        if ((idx < 8) && !((uint32_t)buffer & 0x03)) {
            while (size > 4) {
                M480_USBD_EP_REG(idx, EP[0].EPDAT) = *(uint32_t *)buffer;
                size -= 4;
                buffer += 4;
            }
        }
        while (size > 0) {
            M480_USBD_EP_REG8(idx, EP[0].EPDAT_BYTE) = *buffer;
            size -= 1;
            buffer += 1;
        }

#if VSF_HAL_USBD_TRACE_EN == ENABLED
//        vsf_trace(0, "EPTXCNT=%d,EPDATCNT=%d\r\n",
//                              M480_USBD_EP_REG(idx, EP[0].EPTXCNT),
//                              M480_USBD_EP_REG(idx, EP[0].EPDATCNT) & 0xFFFF);
#endif
    }
    return VSF_ERR_NONE;
}

static void m480_usbd_hs_notify(m480_usbd_hs_t *usbd_hs, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd_hs->callback.evt_handler != NULL) {
        usbd_hs->callback.evt_handler(usbd_hs->callback.param, evt, value);
    }
}

void m480_usbd_hs_irq(m480_usbd_hs_t *usbd_hs)
{
    HSUSBD_T *reg = m480_usbd_hs_get_reg(usbd_hs);
    uint_fast32_t gstatus, status;

    gstatus = reg->GINTSTS;
    gstatus &= reg->GINTEN;

    if (!gstatus) {
        return;
    }

    // USB interrupt
    if (gstatus & HSUSBD_GINTSTS_USBIF_Msk) {
        status = reg->BUSINTSTS;
        status &= reg->BUSINTEN;

        if (status & HSUSBD_BUSINTSTS_SOFIF_Msk) {
            status &= ~HSUSBD_BUSINTSTS_SOFIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_SOF, 0);
        }
        if (status & HSUSBD_BUSINTSTS_RSTIF_Msk) {
            status &= ~HSUSBD_BUSINTSTS_RSTIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_RESET, 0);
            reg->BUSINTSTS = HSUSBD_BUSINTSTS_RSTIF_Msk;
            reg->CEPINTSTS = 0x1ffc;
        }
        if (status & HSUSBD_BUSINTSTS_RESUMEIF_Msk) {
            status &= ~HSUSBD_BUSINTSTS_RESUMEIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_RESUME, 0);
        }
        if (status & HSUSBD_BUSINTSTS_SUSPENDIF_Msk) {
            status &= ~HSUSBD_BUSINTSTS_SUSPENDIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_SUSPEND, 0);
        }
        if (status) {
            reg->BUSINTSTS = status;
        }
    }

    // CEP interrupt
    if (gstatus & HSUSBD_GINTSTS_CEPIF_Msk) {
        status = reg->CEPINTSTS;
        status &= reg->CEPINTEN;

        // IMPORTANT:
        //         the OUT ep of M480 has no flow control, so the order of
        //         checking the interrupt flash MUST be as follow:
        //         IN0 -->> STATUS -->> SETUP -->> OUT0
        // consider this:
        //         SETUP -->> IN0 -->> STATUS -->> SETUP -->> OUT0 -->> STATUS
        //                    ------------------------------------
        //        in some condition, the under line interrupt MAYBE in one routine
        if (status & HSUSBD_CEPINTSTS_TXPKIF_Msk) {
            reg->CEPINTSTS = HSUSBD_CEPINTSTS_TXPKIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_IN, 0);
        }

        if (status & HSUSBD_CEPINTSTS_STSDONEIF_Msk) {
            reg->CEPINTSTS = HSUSBD_CEPINTSTS_STSDONEIF_Msk;

            m480_usbd_hs_notify(usbd_hs, USB_ON_STATUS, 0);
        }

        if (status & HSUSBD_CEPINTSTS_SETUPPKIF_Msk) {
            reg->CEPINTSTS = HSUSBD_CEPINTSTS_SETUPPKIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_SETUP, 0);
        }

        if (status & HSUSBD_CEPINTSTS_RXPKIF_Msk) {
            reg->CEPINTSTS = HSUSBD_CEPINTSTS_RXPKIF_Msk;
            m480_usbd_hs_notify(usbd_hs, USB_ON_OUT, 0);
        }
    }

    // EP interrupt
    if (gstatus & (~3)) {
        for (uint_fast8_t ep, idx = 0; idx < m480_usbd_hs_ep_number - 2; idx++) {
            ep = M480_USBD_EP_REG(idx, EP[0].EPCFG);
            if (!(ep & USB_EP_CFG_VALID)) {
                continue;
            }
            ep = (ep >> 4) & 0x0F;

            if (gstatus & (1 << (idx + 2))) {
                status = M480_USBD_EP_REG(idx, EP[0].EPINTSTS);
                status &= M480_USBD_EP_REG(idx, EP[0].EPINTEN);

                if (status & HSUSBD_EPINTSTS_TXPKIF_Msk) {
                    M480_USBD_EP_REG(idx, EP[0].EPINTSTS) = HSUSBD_EPINTSTS_TXPKIF_Msk;

#if VSF_HAL_USBD_TRACE_EN == ENABLED
                    vsf_trace(0, "%ctxpkif%d: EPTXCNT=%d,EPDATCNT=%d\r\n",
                              (usbd_hs->ep_tx_mask & (1 << idx)) ? ' ' : '*',
                              idx,
                              M480_USBD_EP_REG(idx, EP[0].EPTXCNT),
                              M480_USBD_EP_REG(idx, EP[0].EPDATCNT) & 0xFFFF);
#endif

#ifdef M480_USBD_HS_WROKAROUND_ISO
                    // ISO EP of M480 will issue interrupt even if ZLP is sent.
                    // and there is possibility that the EP handed even if TXCNT is written
                    // so if ep_tx_mask is enabled, and received 10 TXIF interrupt with no data sent
                    // re-write the EPTXCNT register
                    if (usbd_hs->ep_tx_mask & (1 << idx)) {
                        if (0 == M480_USBD_EP_REG(idx, EP[0].EPDATCNT)) {
                            usbd_hs->ep_tx_mask &= ~(1 << idx);
                            m480_usbd_hs_notify(usbd_hs, USB_ON_IN, ep);
                        } else {
                            if (++usbd_hs->retry_cnt[idx] > 2) {
                                usbd_hs->retry_cnt[idx] = 0;
                                M480_USBD_EP_REG(idx, EP[0].EPTXCNT) = usbd_hs->tx_size[idx];
//                                vsf_trace(0, "resend EP%c %d\r\n", 'A' + idx, usbd_hs->tx_size[idx]);
                            }
                        }
                    }
#else
                    m480_usbd_hs_notify(usbd_hs, USB_ON_IN, ep);
#endif
                }
                if (status & HSUSBD_EPINTSTS_RXPKIF_Msk) {
                    M480_USBD_EP_REG(idx, EP[0].EPINTEN) &= ~HSUSBD_EPINTEN_RXPKIEN_Msk;
                    M480_USBD_EP_REG(idx, EP[0].EPINTSTS) = HSUSBD_EPINTSTS_RXPKIF_Msk;
                    m480_usbd_hs_notify(usbd_hs, USB_ON_OUT, ep);
                }
                if (status & HSUSBD_EPINTSTS_NAKIF_Msk) {
                    M480_USBD_EP_REG(idx, EP[0].EPINTSTS) = HSUSBD_EPINTSTS_NAKIF_Msk;
                    m480_usbd_hs_notify(usbd_hs, USB_ON_NAK, ep);
                }
            }
        }
    }
}
