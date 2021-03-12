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

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED

#define __VSF_DWCOTG_DCD_CLASS_IMPLEMENT
#include "./vsf_dwcotg_dcd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern uint_fast16_t vsf_dwcotg_dcd_get_fifo_size(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_DWCOTG_DCD_GET_FIFO_SIZE
WEAK(vsf_dwcotg_dcd_get_fifo_size)
uint_fast16_t vsf_dwcotg_dcd_get_fifo_size(uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    return size;
}
#endif

static void __vk_dwcotg_dcd_init_regs(vk_dwcotg_dcd_t *dwcotg_dcd, void *regbase, uint_fast8_t ep_num)
{
    dwcotg_dcd->reg.global_regs = regbase;
    dwcotg_dcd->reg.dev.global_regs = (void *)((uint8_t *)regbase + 0x800);
    dwcotg_dcd->reg.dev.ep.in_regs = (void *)((uint8_t *)regbase + 0x900);
    dwcotg_dcd->reg.dev.ep.out_regs = (void *)((uint8_t *)regbase + 0xB00);

    for (uint_fast8_t i = 0; i < ep_num; i++) {
        dwcotg_dcd->reg.dfifo[i] = (void *)((uint8_t *)regbase + (i + 1) * 0x1000);
    }
}

static void __vk_dwcotg_dcd_flush_txfifo(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t fifo_num)
{
    dwcotg_dcd->reg.global_regs->grstctl = (fifo_num << 6U) | USB_OTG_GRSTCTL_TXFFLSH;
    while (dwcotg_dcd->reg.global_regs->grstctl & USB_OTG_GRSTCTL_TXFFLSH);
}

static void __vk_dwcotg_dcd_flush_rxfifo(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    dwcotg_dcd->reg.global_regs->grstctl = USB_OTG_GRSTCTL_RXFFLSH;
    while (dwcotg_dcd->reg.global_regs->grstctl & USB_OTG_GRSTCTL_RXFFLSH);
}

vsf_err_t vk_dwcotg_dcd_init(vk_dwcotg_dcd_t *dwcotg_dcd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((dwcotg_dcd != NULL) && (cfg != NULL));
    VSF_USB_ASSERT((dwcotg_dcd->param != NULL) && (dwcotg_dcd->param->op != NULL));

    const vk_dwcotg_dcd_param_t *param = dwcotg_dcd->param;
    struct dwcotg_core_global_regs_t *global_regs;
    struct dwcotg_dev_global_regs_t *dev_global_regs;
    vk_dwcotg_dc_ip_info_t info;
    param->op->GetInfo(&info.use_as__usb_dc_ip_info_t);

    __vk_dwcotg_dcd_init_regs(dwcotg_dcd, info.regbase, info.ep_num);
    global_regs = dwcotg_dcd->reg.global_regs;
    dev_global_regs = dwcotg_dcd->reg.dev.global_regs;

    dwcotg_dcd->callback.evt_handler = cfg->evt_handler;
    dwcotg_dcd->callback.param = cfg->param;

    if (param->dma_en) {
        VSF_USB_ASSERT(info.use_as__vk_dwcotg_hw_info_t.dma_en);
    }
    dwcotg_dcd->dma_en = param->dma_en;

    {
        usb_dc_ip_cfg_t ip_cfg = {
            .priority       = cfg->priority,
            .irq_handler    = (usb_ip_irq_handler_t)vk_dwcotg_dcd_irq,
            .param          = dwcotg_dcd,
        };
        dwcotg_dcd->param->op->Init(&ip_cfg);
    }

    vk_dwcotg_phy_init(&dwcotg_dcd->use_as__vk_dwcotg_t,
                        &param->use_as__vk_dwcotg_param_t,
                        &info.use_as__vk_dwcotg_hw_info_t);

    global_regs->gahbcfg |= USB_OTG_GAHBCFG_TXFELVL;
    if (dwcotg_dcd->dma_en) {
        global_regs->gahbcfg |= USB_OTG_GAHBCFG_HBSTLEN_0 | USB_OTG_GAHBCFG_DMAEN;
    }

    // set device mode
    global_regs->gusbcfg &= ~USB_OTG_GUSBCFG_FHMOD;
    global_regs->gusbcfg |= USB_OTG_GUSBCFG_FDMOD;

    // config 80% periodic frame interval to default
    if (param->speed == USB_DC_SPEED_HIGH) {
        dev_global_regs->dcfg = USB_OTG_DCFG_NZLSOHSK;
    } else if (param->ulpi_en || param->utmi_en) {
        dev_global_regs->dcfg = USB_OTG_DCFG_NZLSOHSK | USB_OTG_DCFG_DSPD_0;
    } else {
        // set full speed PHY
        dev_global_regs->dcfg = USB_OTG_DCFG_NZLSOHSK | USB_OTG_DCFG_DSPD_0 | USB_OTG_DCFG_DSPD_1;
    }

    // disconnect
    dev_global_regs->dctl |= USB_OTG_DCTL_SDIS;

    dev_global_regs->doepmsk = USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_STUPM;
    dev_global_regs->diepmsk = USB_OTG_DIEPMSK_XFRCM;
    dev_global_regs->daint = 0xffffffff;
    dev_global_regs->daintmsk = 0;

    global_regs->gintsts = 0xbfffffff;
    global_regs->gotgint = 0xffffffff;

    global_regs->gintmsk = USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM |
            USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT |
//            USB_OTG_GINTMSK_IISOIXFRM | USB_OTG_GINTMSK_PXFRM_IISOOXFRM |
            USB_OTG_GINTMSK_RXFLVLM;

    global_regs->gahbcfg |= USB_OTG_GAHBCFG_GINT;
    return VSF_ERR_NONE;
}

void vk_dwcotg_dcd_fini(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    dwcotg_dcd->param->op->Fini();
}

void vk_dwcotg_dcd_reset(vk_dwcotg_dcd_t *dwcotg_dcd, usb_dc_cfg_t *cfg)
{
    struct dwcotg_dev_global_regs_t *dev_global_regs = dwcotg_dcd->reg.dev.global_regs;
    vk_dwcotg_dc_ip_info_t info;

    dwcotg_dcd->param->op->GetInfo(&info.use_as__usb_dc_ip_info_t);
    dwcotg_dcd->buffer_word_pos = info.buffer_word_size;
    dwcotg_dcd->ep_num = info.ep_num >> 1;
    dwcotg_dcd->dma_en = dwcotg_dcd->param->dma_en;
    dwcotg_dcd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;

    for (uint_fast8_t i = 0; i < dwcotg_dcd->ep_num; i++) {
        dwcotg_dcd->reg.dev.ep.out_regs[i].doepctl |= USB_OTG_DOEPCTL_SNAK;
    }
    dev_global_regs->dcfg &= ~USB_OTG_DCFG_DAD;
    memset(dwcotg_dcd->trans, 0, sizeof(dwcotg_dcd->trans));
}

void vk_dwcotg_dcd_connect(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    dwcotg_dcd->reg.dev.global_regs->dctl &= ~USB_OTG_DCTL_SDIS;
}

void vk_dwcotg_dcd_disconnect(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    dwcotg_dcd->reg.dev.global_regs->dctl |= USB_OTG_DCTL_SDIS;
}

void vk_dwcotg_dcd_wakeup(vk_dwcotg_dcd_t *dwcotg_dcd)
{
}

void vk_dwcotg_dcd_set_address(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t addr)
{

}

uint_fast8_t vk_dwcotg_dcd_get_address(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    return (dwcotg_dcd->reg.dev.global_regs->dcfg & USB_OTG_DCFG_DAD) >> 4;
}

uint_fast16_t vk_dwcotg_dcd_get_frame_number(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    return (dwcotg_dcd->reg.dev.global_regs->dsts & USB_OTG_DSTS_FNSOF) >> 8;
}

extern uint_fast8_t vk_dwcotg_dcd_get_mframe_number(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    return 0;
}

void vk_dwcotg_dcd_get_setup(vk_dwcotg_dcd_t *dwcotg_dcd, uint8_t *buffer)
{
    memcpy(buffer, &dwcotg_dcd->setup, sizeof(dwcotg_dcd->setup));
}

void vk_dwcotg_dcd_status_stage(vk_dwcotg_dcd_t *dwcotg_dcd, bool is_in)
{
    dwcotg_dcd->ctrl_transfer_state = DWCOTG_STATUS_STAGE;
    if (is_in) {
        vk_dwcotg_dcd_ep_transfer_send(dwcotg_dcd, 0x80, NULL, 0, true);
    } else {
        vk_dwcotg_dcd_ep_transfer_recv(dwcotg_dcd, 0, NULL, 0);
    }
}

uint_fast8_t vk_dwcotg_dcd_ep_get_feature(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint_fast8_t feature)
{
    return USB_DC_FEATURE_TRANSFER;
}

static volatile uint32_t * __vk_dwcotg_dcd_get_ep_ctrl(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    ep &= 0x0F;
    return is_in ? &dwcotg_dcd->reg.dev.ep.in_regs[ep].diepctl : &dwcotg_dcd->reg.dev.ep.out_regs[ep].doepctl;
}

vsf_err_t vk_dwcotg_dcd_ep_add(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep);
    uint_fast8_t is_in = ep & 0x80;

    ep &= 0x0F;
#if VSF_DWCOTG_DCD_CFG_FAKE_EP == ENABLED
    if (ep >= dwcotg_dcd->ep_num) {
        return VSF_ERR_NONE;
    }
#else
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);
#endif

    *ep_ctrl &= ~USB_OTG_DIEPCTL_MPSIZ;
    if (0 == ep) {
        *ep_ctrl |= USB_OTG_DIEPCTL_USBAEP;
        switch (size) {
        case 64:                    break;
        case 32:    *ep_ctrl |= 1;  break;
        case 16:    *ep_ctrl |= 2;  break;
        case 8:     *ep_ctrl |= 3;  break;
        default:
            VSF_USB_ASSERT(false);
            return VSF_ERR_NOT_SUPPORT;
        }
    } else {
        *ep_ctrl &= ~USB_OTG_DIEPCTL_EPTYP;

        switch (type) {
        case USB_EP_TYPE_CONTROL:
            VSF_USB_ASSERT(false);
            break;
        case USB_EP_TYPE_INTERRUPT:
            *ep_ctrl |= (0x3ul << 18) | USB_OTG_DIEPCTL_USBAEP;
            break;
        case USB_EP_TYPE_BULK:
            *ep_ctrl |= (0x2ul << 18) | USB_OTG_DIEPCTL_USBAEP;
            break;
        case USB_EP_TYPE_ISO:
            *ep_ctrl |= (0x1ul << 18) | USB_OTG_DIEPCTL_USBAEP;
            break;
        }
        *ep_ctrl |= size;
        if (is_in) {
            *ep_ctrl |= ep << 22;
        }
    }
    if (is_in) {
        size = (size + 3) & ~3;
        size = vsf_dwcotg_dcd_get_fifo_size(ep | 0x80, type, size);
        size >>= 2;
        dwcotg_dcd->buffer_word_pos -= size;

        if (!ep) {
            dwcotg_dcd->reg.global_regs->gnptxfsiz = (size << 16) | dwcotg_dcd->buffer_word_pos;
        } else {
            dwcotg_dcd->reg.global_regs->dtxfsiz[ep - 1] = (size << 16) | dwcotg_dcd->buffer_word_pos;
        }
        dwcotg_dcd->reg.global_regs->grxfsiz &= ~USB_OTG_GRXFSIZ_RXFD;
        dwcotg_dcd->reg.global_regs->grxfsiz |= dwcotg_dcd->buffer_word_pos;
        // flush FIFO to validate fifo settings
        __vk_dwcotg_dcd_flush_txfifo(dwcotg_dcd, 0x10);
        __vk_dwcotg_dcd_flush_rxfifo(dwcotg_dcd);
    }
    dwcotg_dcd->reg.dev.global_regs->daintmsk |= (1 << (is_in ? 0 : 16)) << ep;
    return VSF_ERR_NONE;
}

uint_fast16_t vk_dwcotg_dcd_ep_get_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    if (0 == ep) {
        switch (*ep_ctrl & USB_OTG_DIEPCTL_MPSIZ) {
        case 0:     return 64;
        case 1:     return 32;
        case 2:     return 16;
        case 3:     return 8;
        }
    } else {
        return *ep_ctrl & USB_OTG_DIEPCTL_MPSIZ;
    }
    return 0;
}

vsf_err_t vk_dwcotg_dcd_ep_set_stall(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    *ep_ctrl |= USB_OTG_DIEPCTL_STALL;
    return VSF_ERR_NONE;
}

bool vk_dwcotg_dcd_ep_is_stalled(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    return !!(*ep_ctrl & USB_OTG_DIEPCTL_STALL);
}

vsf_err_t vk_dwcotg_dcd_ep_clear_stall(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    *ep_ctrl &= ~USB_OTG_DIEPCTL_STALL;
    return VSF_ERR_NONE;
}

vsf_err_t vk_dwcotg_dcd_ep_transaction_read_buffer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_dwcotg_dcd_ep_transaction_enable_out(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_dwcotg_dcd_ep_transaction_set_data_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_dwcotg_dcd_ep_transaction_write_buffer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

static vk_dwcotg_dcd_trans_t * __vk_dwcotg_dcd_get_trans(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    ep &= 0x0F;
    return &dwcotg_dcd->trans[(is_in ? VSF_DWCOTG_DCD_CFG_EP_NUM : 0) + ep];
}

static void __vk_dwcotg_dcd_ep_write(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep_idx)
{
    vk_dwcotg_dcd_trans_t *trans = __vk_dwcotg_dcd_get_trans(dwcotg_dcd, ep_idx | 0x80);
    if (!trans->use_dma) {
        struct dwcotg_dev_global_regs_t *dev_global_regs = dwcotg_dcd->reg.dev.global_regs;
        struct dwcotg_dev_in_ep_regs_t *in_regs = &dwcotg_dcd->reg.dev.ep.in_regs[ep_idx];
        uint_fast32_t size = in_regs->dtxfsts << 2;
        uint_fast32_t remain_size = in_regs->dieptsiz & 0x7FFFF;
        uint8_t *buffer = trans->buffer;
        uint32_t data;
        bool fifo_en = remain_size > size;

        size = min(size, remain_size);
        if (size > 0) {
            VSF_USB_ASSERT(buffer != NULL);
        }

        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
#ifndef UNALIGNED
            data = get_unaligned_cpu32(buffer);
#else
            data = *(uint32_t UNALIGNED *)buffer;
#endif
            *dwcotg_dcd->reg.dfifo[ep_idx] = data;
        }
        trans->buffer = buffer;

        if (fifo_en) {
            dev_global_regs->dtknqr4_fifoemptymsk |= 1 << ep_idx;
        } else {
            dev_global_regs->dtknqr4_fifoemptymsk &= ~(1 << ep_idx);
        }
    }
}

static void __vk_dwcotg_dcd_ep_read(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep_idx, uint_fast16_t size)
{
    vk_dwcotg_dcd_trans_t *trans = __vk_dwcotg_dcd_get_trans(dwcotg_dcd, ep_idx);
    if (!trans->use_dma) {
        uint_fast16_t ep_size = vk_dwcotg_dcd_ep_get_size(dwcotg_dcd, ep_idx);
        uint8_t *buffer = trans->buffer;
        uint32_t data;

        if (size > 0) {
            VSF_USB_ASSERT(buffer != NULL);
        }

        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
            data = *dwcotg_dcd->reg.dfifo[0];
#ifndef UNALIGNED
            put_unaligned_cpu32(data, buffer);
#else
            *(uint32_t UNALIGNED *)buffer = data;
#endif
        }
        trans->remain -= size;
        trans->buffer = buffer;
        if ((size < ep_size) || !trans->remain) {
            trans->zlp = true;
        }
    }
}

uint_fast32_t vk_dwcotg_dcd_ep_get_data_size(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep)
{
    vk_dwcotg_dcd_trans_t *trans = __vk_dwcotg_dcd_get_trans(dwcotg_dcd, ep);
    return trans->size - trans->remain;
}

static vsf_err_t __vk_dwcotg_dcd_ep_out_transfer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep_idx)
{
    VSF_USB_ASSERT(ep_idx < dwcotg_dcd->ep_num);

    uint_fast16_t ep_size = vk_dwcotg_dcd_ep_get_size(dwcotg_dcd, ep_idx);
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep_idx);
    struct dwcotg_dev_out_ep_regs_t *out_regs = &dwcotg_dcd->reg.dev.ep.out_regs[ep_idx];
    vk_dwcotg_dcd_trans_t *trans = &dwcotg_dcd->trans[ep_idx];
    uint_fast32_t size = trans->remain, max_size;
    if (0 == ep_idx) {
        // pkt_cnt can only be 1(has only 1 bit) for EP0
        max_size = ep_size;
//        max_size = (1 << 7) - 1;
    } else {
        max_size = (1 << 19) - 1;
    }
    size = min(size, max_size);
    if (size < trans->remain) {
        size &= ~(ep_size - 1);
    }
    uint_fast8_t pkt_cnt = (size + ep_size - 1) / ep_size;

    if (!pkt_cnt) {
        pkt_cnt++;
    }
    out_regs->doeptsiz &= ~(USB_OTG_DOEPTSIZ_XFRSIZ | USB_OTG_DOEPTSIZ_PKTCNT);
    out_regs->doeptsiz |= (pkt_cnt << 19) | size;
    if (trans->use_dma) {
        out_regs->doepdma = (uint32_t)trans->buffer;
    }

    // set odd/even frame for iso endpoint
    if (1 == ((*ep_ctrl >> 18) & 0x03)) {
        if (vk_dwcotg_dcd_get_frame_number(dwcotg_dcd) & 1) {
            out_regs->doepctl |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
        } else {
            out_regs->doepctl |= USB_OTG_DOEPCTL_SODDFRM;
        }
    }

    out_regs->doepctl |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
    return VSF_ERR_NONE;
}

vsf_err_t vk_dwcotg_dcd_ep_transfer_recv(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    VSF_USB_ASSERT(!(ep & 0x80));
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    VSF_USB_ASSERT(((ep == 0) && dwcotg_dcd->ctrl_transfer_state != DWCOTG_SETUP_STAGE) || (ep > 0));

    vk_dwcotg_dcd_trans_t *trans = &dwcotg_dcd->trans[ep];
    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = false;
    trans->use_dma = dwcotg_dcd->dma_en && !((uint32_t)trans->buffer & 0x03);

    return __vk_dwcotg_dcd_ep_out_transfer(dwcotg_dcd, ep);
}

static vsf_err_t __vk_dwcotg_dcd_ep_in_transfer(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep_idx)
{
    VSF_USB_ASSERT(ep_idx < dwcotg_dcd->ep_num);

    uint_fast16_t ep_size = vk_dwcotg_dcd_ep_get_size(dwcotg_dcd, ep_idx | 0x80);
    volatile uint32_t *ep_ctrl = __vk_dwcotg_dcd_get_ep_ctrl(dwcotg_dcd, ep_idx | 0x80);
    struct dwcotg_dev_in_ep_regs_t *in_regs = &dwcotg_dcd->reg.dev.ep.in_regs[ep_idx];
    vk_dwcotg_dcd_trans_t *trans = &dwcotg_dcd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep_idx];

    uint_fast32_t size = trans->remain, max_size;
    if (0 == ep_idx) {
        // pkt_cnt can only be 1(has only 1 bit) for EP0
        max_size = ep_size;
//        max_size = (1 << 7) - 1;
    } else {
        max_size = (1 << 19) - 1;
    }
    size = min(size, max_size);
    if (size < trans->remain) {
        size &= ~(ep_size - 1);
    }
    uint_fast8_t pkt_cnt = (size + ep_size - 1) / ep_size;

    trans->remain -= size;
    if (!trans->remain && (size % ep_size)) {
        trans->zlp = false;
    }

    if (!pkt_cnt) {
        // if run here and pkt_cnt is 0, means send zlp without trans->zlp set
        pkt_cnt++;
    }
    in_regs->dieptsiz = (pkt_cnt << 19) | size;

    // set odd/even frame and mulcnt for iso endpoint
    if (1 == ((*ep_ctrl >> 18) & 0x03)) {
        in_regs->dieptsiz &= ~USB_OTG_DIEPTSIZ_MULCNT;
        in_regs->dieptsiz |= 1 << 29;

        if (vk_dwcotg_dcd_get_frame_number(dwcotg_dcd) & 1) {
            in_regs->diepctl |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
        } else {
            in_regs->diepctl |= USB_OTG_DIEPCTL_SODDFRM;
        }
    }

    if (trans->use_dma) {
        in_regs->diepdma = (uint32_t)trans->buffer;
        in_regs->diepctl |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
    } else {
        in_regs->diepctl |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
        __vk_dwcotg_dcd_ep_write(dwcotg_dcd, ep_idx);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vk_dwcotg_dcd_ep_transfer_send(vk_dwcotg_dcd_t *dwcotg_dcd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    VSF_USB_ASSERT(ep & 0x80);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dwcotg_dcd->ep_num);

    vk_dwcotg_dcd_trans_t *trans = &dwcotg_dcd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep];
    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = zlp;
    // if fifo transfer is not supported in dma mode(for unaligned buffer), add assert here
    trans->use_dma = dwcotg_dcd->dma_en && !((uint32_t)trans->buffer & 0x03);

    return __vk_dwcotg_dcd_ep_in_transfer(dwcotg_dcd, ep);
}

static void __vk_dwcotg_dcd_notify(vk_dwcotg_dcd_t *dwcotg_dcd, usb_evt_t evt, uint_fast8_t value)
{
    if (dwcotg_dcd->callback.evt_handler != NULL) {
        dwcotg_dcd->callback.evt_handler(dwcotg_dcd->callback.param, evt, value);
    }
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

void vk_dwcotg_dcd_irq(vk_dwcotg_dcd_t *dwcotg_dcd)
{
    struct dwcotg_core_global_regs_t *global_regs = dwcotg_dcd->reg.global_regs;
    struct dwcotg_dev_global_regs_t *dev_global_regs = dwcotg_dcd->reg.dev.global_regs;
    struct dwcotg_dev_in_ep_regs_t *in_regs = dwcotg_dcd->reg.dev.ep.in_regs;
    struct dwcotg_dev_out_ep_regs_t *out_regs = dwcotg_dcd->reg.dev.ep.out_regs;
    uint_fast32_t intsts = global_regs->gintmsk | USB_OTG_GINTSTS_CMOD;
    vk_dwcotg_dcd_trans_t *trans;

    intsts &= global_regs->gintsts;

    VSF_USB_ASSERT(!(intsts & USB_OTG_GINTSTS_CMOD));

    if (intsts & USB_OTG_GINTSTS_MMIS) {
        VSF_USB_ASSERT(false);
        global_regs->gintsts = USB_OTG_GINTSTS_MMIS;
    }
    if (intsts & USB_OTG_GINTSTS_USBRST) {
        __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_RESET, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_USBRST;
    }
    if (intsts & USB_OTG_GINTSTS_ENUMDNE) {
        uint8_t speed = (dev_global_regs->dsts & USB_OTG_DSTS_ENUMSPD) >> 1;
        global_regs->gusbcfg &= ~USB_OTG_GUSBCFG_TRDT;
        global_regs->gusbcfg |= ((0/* USB_SPEED_HIGH*/ == speed) ? 0x09U : 0x05U) << 10;
        dev_global_regs->dctl |= USB_OTG_DCTL_CGINAK;
        global_regs->gintsts = USB_OTG_GINTSTS_ENUMDNE;
    }
    if (intsts & USB_OTG_GINTSTS_USBSUSP) {
        __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_SUSPEND, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_USBSUSP;
    }
    if (intsts & USB_OTG_GINTSTS_WKUINT) {
        __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_RESUME, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_WKUINT;
    }
    if (intsts & USB_OTG_GINTSTS_SOF) {
        __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_SOF, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_SOF;
    }

    if (intsts & USB_OTG_GINTSTS_IEPINT) {
        uint_fast8_t ep_idx = 0;
        uint_fast32_t ep_int = dev_global_regs->daint;
        ep_int = (ep_int & dev_global_regs->daintmsk) & 0xffff;

        while (ep_int) {
            if (ep_int & 0x1) {
                uint_fast32_t int_status = in_regs[ep_idx].diepint;

                uint_fast32_t int_msak = dev_global_regs->diepmsk | USB_OTG_DIEPINT_INEPNE | USB_OTG_DIEPINT_NAK;
                int_status &= (int_msak | USB_OTG_DIEPINT_TXFE);

                if (int_status & USB_OTG_DIEPINT_XFRC) {
                    if ((ep_idx == 0) && (dwcotg_dcd->ctrl_transfer_state == DWCOTG_STATUS_STAGE)) {
                        dwcotg_dcd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;
                        __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_STATUS, 0);
                    } else {
                        trans = &dwcotg_dcd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep_idx];
                        if (trans->remain) {
                            __vk_dwcotg_dcd_ep_in_transfer(dwcotg_dcd, ep_idx);
                        } else if (trans->zlp) {
                            // OTG Programmer's Guild, 8.4:
                            // To transmit a few maximum-packet-size packets and a zero-length
                            //  data packet at the end of thetransfer, the application must split
                            //  the transfer in two parts. The first sends maximum-packet-sizedata
                            //  packets and the second sends the zero-length data packet alone.
                            trans->zlp = false;
                            __vk_dwcotg_dcd_ep_in_transfer(dwcotg_dcd, ep_idx);
                        } else {
                            __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_IN, ep_idx);
                        }
                    }
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_XFRC;
                }
                if (int_status & USB_OTG_DIEPINT_EPDISD) {
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_EPDISD;
                }
                if (int_status & USB_OTG_DIEPINT_TOC) {
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_TOC;
                }
                if (int_status & USB_OTG_DIEPINT_INEPNE) {
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_INEPNE;
                }
                if (int_status & USB_OTG_DIEPINT_TXFE) {
                    __vk_dwcotg_dcd_ep_write(dwcotg_dcd, ep_idx);
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_TXFE;
                }
            }
            ep_int >>= 1;
            ep_idx++;
        }
    }

    if (intsts & USB_OTG_GINTSTS_OEPINT) {
        uint_fast8_t ep_idx = 0;
        uint_fast32_t ep_int = dev_global_regs->daint;
        ep_int = (ep_int & dev_global_regs->daintmsk) >> 16;

        while (ep_int) {
            if (ep_int & 0x1) {
                uint_fast32_t int_status = out_regs[ep_idx].doepint;

                int_status &= dev_global_regs->doepmsk | USB_OTG_DOEPINT_STSPHSERCVD;

                // transfer complete interrupt
                if (int_status & USB_OTG_DOEPINT_XFRC) {
                    if ((ep_idx == 0) && (dwcotg_dcd->ctrl_transfer_state == DWCOTG_STATUS_STAGE)) {
                        if (!(int_status & USB_OTG_DOEPINT_STSPHSERCVD)) {
                            dwcotg_dcd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;
                            __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_STATUS, 0);
                        }
                    } else if (((ep_idx == 0) && dwcotg_dcd->ctrl_transfer_state == DWCOTG_DATA_STAGE) || (ep_idx > 0)) {
                        trans = &dwcotg_dcd->trans[ep_idx];
                        if (!trans->zlp) {
                            __vk_dwcotg_dcd_ep_out_transfer(dwcotg_dcd, ep_idx);
                        } else {
                            __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_OUT, ep_idx);
                        }
                    }
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_XFRC;
                }
                // endpoint disable interrupt
                if (int_status & USB_OTG_DOEPINT_EPDISD) {
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_EPDISD;
                }
                // setup phase finished interrupt (just for control endpoints)
                if (int_status & USB_OTG_DOEPINT_STUP) {
                    // need update address immediately
                    if (    ((((uint32_t *)dwcotg_dcd->setup)[0] & 0xFF00FFFF) == 0x00000500)
                        &&  (((uint32_t *)dwcotg_dcd->setup)[1] == 0x0)) {
                        VSF_USB_ASSERT(!vk_dwcotg_dcd_get_address(dwcotg_dcd));
                        dev_global_regs->dcfg |= (dwcotg_dcd->setup[2] & 0x7F) << 4;
                    }

                    dwcotg_dcd->ctrl_transfer_state = DWCOTG_DATA_STAGE;
                    __vk_dwcotg_dcd_notify(dwcotg_dcd, USB_ON_SETUP, 0);
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_STUP;
                }
                // back to back setup packets received
                if (int_status & USB_OTG_DOEPINT_B2BSTUP) {
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_B2BSTUP;
                }
                if (int_status & USB_OTG_DOEPINT_STSPHSERCVD) {
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_STSPHSERCVD;
                }
            }
            ep_int >>= 1;
            ep_idx++;
        }
    }
    if (intsts & USB_OTG_GINTSTS_RXFLVL) {
        uint_fast8_t ep_idx, pid, pktsts;
        uint_fast16_t size;
        uint_fast32_t rx_status;

        global_regs->gintmsk &= ~USB_OTG_GINTMSK_RXFLVLM;
        rx_status = global_regs->grxstsp;

        ep_idx = rx_status & USB_OTG_GRXSTSP_EPNUM;
        size = (rx_status & USB_OTG_GRXSTSP_BCNT) >> 4;
        pid = (rx_status & USB_OTG_GRXSTSP_DPID) >> 15;
        pktsts = (rx_status & USB_OTG_GRXSTSP_PKTSTS) >> 17;

        switch (pktsts) {
        case 1:
        case 3:
        case 4:
            break;
        case 6: //RXSTAT_SETUP_UPDT:
            if (!ep_idx && (8 == size) && (0/*DPID_DATA0*/ == pid)) {
                // In some versions of dwcotg, We can't replace dfifo[0] with grxstsp[0]
                ((uint32_t *)dwcotg_dcd->setup)[0] = *dwcotg_dcd->use_as__vk_dwcotg_t.reg.dfifo[0];
                ((uint32_t *)dwcotg_dcd->setup)[1] = *dwcotg_dcd->use_as__vk_dwcotg_t.reg.dfifo[0];
            }
            break;
        case 2: //RXSTAT_DATA_UPDT:
            __vk_dwcotg_dcd_ep_read(dwcotg_dcd, ep_idx, size);
            break;
        //case RXSTAT_GOUT_NAK:
        //case RXSTAT_SETUP_COMP:
        default:
            VSF_HAL_ASSERT(false);
            break;
        }

        global_regs->gintmsk |= USB_OTG_GINTMSK_RXFLVLM;
    }
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif
