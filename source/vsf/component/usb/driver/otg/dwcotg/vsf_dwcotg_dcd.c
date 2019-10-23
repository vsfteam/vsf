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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_DWCOTG == ENABLED

#define VSF_DWCOTG_DCD_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
#define DWCOTG_DEBUG
#define DWCOTG_DEBUG_DUMP_USB_ON
#define DWCOTG_DEBUG_DUMP_FUNC_CALL
#define DWCOTG_DEBUG_DUMP_DATA
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum __usb_evt_t {
    ______________,

    __USB_ON_START,
        __USB_ON_SETUP,
        __USB_ON_MMIS,
        __USB_ON_IN,
        __USB_ON_OUT,
        __USB_ON_STATUS,
        __USB_ON_ENUMDNE,
        __USB_ON_USBSUSP,
        __USB_ON_WKUINTP,
        __USB_ON_SOF,
        __USB_ON_RST,
        __USB_ON_IRQ,

        __USB_ON_IEPINT,
        __USB_ON_DIEPINT_INEPNE,
        __USB_ON_DIEPINT_TXFE,

        __USB_ON_OEPINT,
        __USB_ON_DOEPINT_B2BSTUPP,

        __USB_ON_RXFLVL,
        __USB_ON_RXSTAT_SETUP_UPDT,
        __USB_ON_RXSTAT_DATA_UPDT,

        __USB_ON_ISR_IN_TRANSFER,
        __USB_ON_ISR_OUT_TRANSFER,
    __USB_ON_END,

    __FUNC_START,
        __FUNC_RESET,
        __FUNC_INIT,
        __FUNC_CONNECT,
        __FUNC_DISCONNECT,
        __FUNC_STATUS_STAGE,
        __FUNC_EP_ADD,
        __FUNC_EP_WRITE,
        __FUNC_EP_READ,
        __FUNC_EP_OUT_TRANSFER,
        __FUNC_EP_IN_TRANSFER,
        __FUNC_EP_TRANSFER_SEND,
        __FUNC_EP_TRANSFER_RECV,

        __FUNC_EP_WRITE_ENABLE_FIFO_EMPTY,
        __FUNC_EP_WRITE_DISABLE_FIFO_EMPTY,
    __FUNC_END,

};
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
#ifdef DWCOTG_DEBUG
ROOT enum __usb_evt_t evt_buf[1024 * 2];
uint16_t evt_index;
#endif
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void debug_add(uint32_t data)
{
#ifdef DWCOTG_DEBUG
    if (evt_index < dimof(evt_buf)) {
        evt_buf[evt_index++] = (enum __usb_evt_t)data;
    }
#endif
}

static void debug_add_data(uint32_t data)
{
#ifdef DWCOTG_DEBUG_DUMP_DATA
    debug_add(data);
#endif
}

static void debug_add_evt(uint32_t evt)
{
#ifndef DWCOTG_DEBUG_DUMP_USB_ON
    if (evt < __USB_ON_END) return;
#endif

#ifndef DWCOTG_DEBUG_DUMP_FUNC_CALL
    if (evt > __USB_ON_END) return;
#endif

    debug_add_data(evt);
}


static void vsf_dwcotg_usbd_init_regs(vsf_dwcotg_dcd_t *usbd, void *regbase, uint_fast8_t ep_num)
{
    usbd->reg.global_regs = regbase;
    usbd->reg.dev.global_regs = (void *)((uint8_t *)regbase + 0x800);
    usbd->reg.dev.ep.in_regs = (void *)((uint8_t *)regbase + 0x900);
    usbd->reg.dev.ep.out_regs = (void *)((uint8_t *)regbase + 0xB00);

    for (uint_fast8_t i = 0; i < ep_num; i++) {
        usbd->reg.dfifo[i] = (void *)((uint8_t *)regbase + (i + 1) * 0x1000);
    }
}

static void vsf_dwcotg_usbd_flush_txfifo(vsf_dwcotg_dcd_t *usbd, uint_fast8_t fifo_num)
{
    usbd->reg.global_regs->grstctl = (fifo_num << 6U) | USB_OTG_GRSTCTL_TXFFLSH;
    while (usbd->reg.global_regs->grstctl & USB_OTG_GRSTCTL_TXFFLSH);
}

static void vsf_dwcotg_usbd_flush_rxfifo(vsf_dwcotg_dcd_t *usbd)
{
    usbd->reg.global_regs->grstctl = USB_OTG_GRSTCTL_RXFFLSH;
    while (usbd->reg.global_regs->grstctl & USB_OTG_GRSTCTL_RXFFLSH);
}

vsf_err_t vsf_dwcotg_usbd_init(vsf_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));
    VSF_USB_ASSERT((usbd->param != NULL) && (usbd->param->op != NULL));

    debug_add_evt(__FUNC_INIT);

    const vsf_dwcotg_dcd_param_t *param = usbd->param;
    struct dwcotg_core_global_regs_t *global_regs;
    struct dwcotg_dev_global_regs_t *dev_global_regs;
    vsf_dwcotg_dc_ip_info_t info;
    param->op->GetInfo(&info.use_as__usb_dc_ip_info_t);

    vsf_dwcotg_usbd_init_regs(usbd, info.regbase, info.ep_num);
    global_regs = usbd->reg.global_regs;
    dev_global_regs = usbd->reg.dev.global_regs;

    usbd->callback.evt_handler = cfg->evt_handler;
    usbd->callback.param = cfg->param;

    {
        usb_dc_ip_cfg_t ip_cfg = {
            .priority       = cfg->priority,
            .irq_handler    = (usb_ip_irq_handler_t)vsf_dwcotg_usbd_irq,
            .param          = usbd,
        };
        usbd->param->op->Init(&ip_cfg);
    }

    vsf_dwcotg_phy_init(&usbd->use_as__vsf_dwcotg_t,
                        &param->use_as__vsf_dwcotg_param_t,
                        &info.use_as__vsf_dwcotg_hw_info_t);

    global_regs->gahbcfg |= USB_OTG_GAHBCFG_TXFELVL;
    if (usbd->dma_en) {
        global_regs->gahbcfg |= USB_OTG_GAHBCFG_HBSTLEN_0 | USB_OTG_GAHBCFG_DMAEN;
    }

    // set device mode
    global_regs->gusbcfg &= ~USB_OTG_GUSBCFG_FHMOD;
    global_regs->gusbcfg |= USB_OTG_GUSBCFG_FDMOD;

    // config 80% periodic frame interval to default
    if (param->speed == USB_SPEED_HIGH) {
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
            USB_OTG_GINTMSK_IISOIXFRM | USB_OTG_GINTMSK_PXFRM_IISOOXFRM |
            USB_OTG_GINTMSK_RXFLVLM;

    global_regs->gahbcfg |= USB_OTG_GAHBCFG_GINT;
    return VSF_ERR_NONE;
}

void vsf_dwcotg_usbd_fini(vsf_dwcotg_dcd_t *usbd)
{
    usbd->param->op->Fini();
}

void vsf_dwcotg_usbd_reset(vsf_dwcotg_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    debug_add_evt(__FUNC_RESET);

    struct dwcotg_dev_global_regs_t *dev_global_regs = usbd->reg.dev.global_regs;
    vsf_dwcotg_dc_ip_info_t info;
    usbd->param->op->GetInfo(&info.use_as__usb_dc_ip_info_t);
    usbd->buffer_word_pos = info.buffer_word_size;
    usbd->ep_num = info.ep_num >> 1;
    usbd->dma_en = info.dma_en;
    usbd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;

    for (uint_fast8_t i = 0; i < usbd->ep_num; i++) {
        usbd->reg.dev.ep.out_regs[i].doepctl |= USB_OTG_DOEPCTL_SNAK;
    }
    dev_global_regs->dcfg &= ~USB_OTG_DCFG_DAD;
    memset(usbd->trans, 0, sizeof(usbd->trans));
}

void vsf_dwcotg_usbd_connect(vsf_dwcotg_dcd_t *usbd)
{
    debug_add_evt(__FUNC_CONNECT);
    usbd->reg.dev.global_regs->dctl &= ~USB_OTG_DCTL_SDIS;
}

void vsf_dwcotg_usbd_disconnect(vsf_dwcotg_dcd_t *usbd)
{
    debug_add_evt(__FUNC_DISCONNECT);
    usbd->reg.dev.global_regs->dctl |= USB_OTG_DCTL_SDIS;
}

void vsf_dwcotg_usbd_wakeup(vsf_dwcotg_dcd_t *usbd)
{
}

void vsf_dwcotg_usbd_set_address(vsf_dwcotg_dcd_t *usbd, uint_fast8_t addr)
{

}

uint_fast8_t vsf_dwcotg_usbd_get_address(vsf_dwcotg_dcd_t *usbd)
{
    return (usbd->reg.dev.global_regs->dcfg & USB_OTG_DCFG_DAD) >> 4;
}

uint_fast16_t vsf_dwcotg_usbd_get_frame_number(vsf_dwcotg_dcd_t *usbd)
{
    return (usbd->reg.dev.global_regs->dsts & USB_OTG_DSTS_FNSOF) >> 8;
}

extern uint_fast8_t vsf_dwcotg_usbd_get_mframe_number(vsf_dwcotg_dcd_t *usbd)
{
    return 0;
}

void vsf_dwcotg_usbd_get_setup(vsf_dwcotg_dcd_t *usbd, uint8_t *buffer)
{
    memcpy(buffer, &usbd->setup, sizeof(usbd->setup));
}

void vsf_dwcotg_usbd_status_stage(vsf_dwcotg_dcd_t *usbd, bool is_in)
{
    debug_add_evt(__FUNC_STATUS_STAGE);

    usbd->ctrl_transfer_state = DWCOTG_STATUS_STAGE;
    if (is_in) {
        vsf_dwcotg_usbd_ep_transfer_send(usbd, 0x80, NULL, 0, true);
    } else {
        vsf_dwcotg_usbd_ep_transfer_recv(usbd, 0, NULL, 0);
    }
}

uint_fast8_t vsf_dwcotg_usbd_ep_get_feature(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    return USB_DC_FEATURE_TRANSFER;
}

static volatile uint32_t * dwcotg_usbd_get_ep_ctrl(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    ep &= 0x0F;
    return is_in ? &usbd->reg.dev.ep.in_regs[ep].diepctl : &usbd->reg.dev.ep.out_regs[ep].doepctl;
}

vsf_err_t vsf_dwcotg_usbd_ep_add(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    volatile uint32_t *ep_ctrl = dwcotg_usbd_get_ep_ctrl(usbd, ep);
    uint_fast8_t is_in = ep & 0x80;

    debug_add_evt(__FUNC_EP_ADD);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);
    
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
#if VSF_DWCOTG_DCD_CFG_AUTO_BUFFER_INIT == ENABLED
        // TODO: make sure how to allocate buffer
        size = (size + 3) & ~3;
        size >>= 2;
        usbd->buffer_word_pos -= size;

        if (!ep) {
            usbd->reg.global_regs->gnptxfsiz = (size << 16) | usbd->buffer_word_pos;
        } else {
            ep--;
            usbd->reg.global_regs->dtxfsiz[ep] = (size << 16) | usbd->buffer_word_pos;
        }
        usbd->reg.global_regs->grxfsiz &= ~USB_OTG_GRXFSIZ_RXFD;
        usbd->reg.global_regs->grxfsiz |= usbd->buffer_word_pos;
        // flush FIFO to validate fifo settings
        vsf_dwcotg_usbd_flush_txfifo(usbd, 0x10);
        vsf_dwcotg_usbd_flush_rxfifo(usbd);
#endif
    }
    usbd->reg.dev.global_regs->daintmsk |= (1 << (is_in ? 0 : 16)) << ep;
    return VSF_ERR_NONE;
}

uint_fast16_t vsf_dwcotg_usbd_ep_get_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = dwcotg_usbd_get_ep_ctrl(usbd, ep);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);

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

vsf_err_t vsf_dwcotg_usbd_ep_set_stall(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = ep & 0x80 ?
        &usbd->reg.dev.ep.in_regs[ep].diepctl : &usbd->reg.dev.ep.out_regs[ep].doepctl;
    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);

    *ep_ctrl |= USB_OTG_DIEPCTL_STALL;
    return VSF_ERR_NONE;
}

bool vsf_dwcotg_usbd_ep_is_stalled(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = ep & 0x80 ?
        &usbd->reg.dev.ep.in_regs[ep].diepctl : &usbd->reg.dev.ep.out_regs[ep].doepctl;
    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);

    return !!(*ep_ctrl & USB_OTG_DIEPCTL_STALL);
}

vsf_err_t vsf_dwcotg_usbd_ep_clear_stall(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    volatile uint32_t *ep_ctrl = ep & 0x80 ?
        &usbd->reg.dev.ep.in_regs[ep].diepctl : &usbd->reg.dev.ep.out_regs[ep].doepctl;
    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);

    *ep_ctrl &= ~USB_OTG_DIEPCTL_STALL;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_dwcotg_usbd_ep_transaction_read_buffer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_dwcotg_usbd_ep_transaction_enable_out(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_dwcotg_usbd_ep_transaction_set_data_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vsf_dwcotg_usbd_ep_transaction_write_buffer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

static vsf_dwcotg_dcd_trans_t * vsf_dwcotg_usbd_get_trans(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    uint_fast8_t is_in = ep & 0x80;
    ep &= 0x0F;
    return &usbd->trans[(is_in ? VSF_DWCOTG_DCD_CFG_EP_NUM : 0) + ep];
}

static void vsf_dwcotg_usbd_ep_write(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep_idx)
{
    debug_add_evt(__FUNC_EP_WRITE);

    vsf_dwcotg_dcd_trans_t *trans = vsf_dwcotg_usbd_get_trans(usbd, ep_idx | 0x80);
    if (!trans->use_dma) {
        struct dwcotg_dev_global_regs_t *dev_global_regs = usbd->reg.dev.global_regs;
        struct dwcotg_dev_in_ep_regs_t *in_regs = &usbd->reg.dev.ep.in_regs[ep_idx];
        uint_fast32_t size = in_regs->dtxfsts << 2;
        uint_fast32_t remain_size = in_regs->dieptsiz & 0x7FFFF;
        uint8_t *buffer = trans->buffer;
        uint32_t data;
        bool fifo_en = remain_size > size;

        debug_add_data(0xFFFF0000 + size);
        debug_add_data(0xFFFF0000 + remain_size);

        size = min(size, remain_size);

        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
            VSF_USB_ASSERT(buffer != NULL);
            // TODO: support unaligned access
            data = *(uint32_t *)buffer;
            debug_add_data(data);
            *usbd->reg.dfifo[ep_idx] = data;
        }
        trans->buffer = buffer;

        if (fifo_en) {
            debug_add_evt(__FUNC_EP_WRITE_ENABLE_FIFO_EMPTY);
            dev_global_regs->dtknqr4_fifoemptymsk |= 1 << ep_idx;
        } else {
            debug_add_evt(__FUNC_EP_WRITE_DISABLE_FIFO_EMPTY);
            dev_global_regs->dtknqr4_fifoemptymsk &= ~(1 << ep_idx);
        }
        debug_add_data(0xFFFF0000 + dev_global_regs->dtknqr4_fifoemptymsk);
        debug_add_data(0xFFFF0000 + in_regs[ep_idx].diepint);
        debug_add_data(0xFFFF0000 + (in_regs->dtxfsts << 2));
        debug_add_data(0xFFFF0000 + (in_regs->dieptsiz & 0x7FFFF));
    }
}

static void vsf_dwcotg_usbd_ep_read(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep_idx, uint_fast16_t size)
{
    debug_add_evt(__FUNC_EP_READ);

    vsf_dwcotg_dcd_trans_t *trans = vsf_dwcotg_usbd_get_trans(usbd, ep_idx);
    if (!trans->use_dma) {
        uint_fast16_t ep_size = vsf_dwcotg_usbd_ep_get_size(usbd, ep_idx);
        uint8_t *buffer = trans->buffer;
        uint32_t data;

        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
            data = *usbd->reg.dfifo[0];
            // TODO: support unaligned access
            debug_add_evt(data);
            *(uint32_t *)buffer = data;
        }
        trans->remain -= size;
        trans->buffer = buffer;
        if ((size < ep_size) || !trans->remain) {
            trans->zlp = true;
        }
    }
}

uint_fast32_t vsf_dwcotg_usbd_ep_get_data_size(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep)
{
    vsf_dwcotg_dcd_trans_t *trans = vsf_dwcotg_usbd_get_trans(usbd, ep);
    return trans->size - trans->remain;
}

static vsf_err_t vsf_dwcotg_usbd_ep_out_transfer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep_idx)
{
    debug_add_evt(__FUNC_EP_OUT_TRANSFER);

    VSF_USB_ASSERT(ep_idx < usbd->ep_num);

    uint_fast16_t ep_size = vsf_dwcotg_usbd_ep_get_size(usbd, ep_idx);
    struct dwcotg_dev_out_ep_regs_t *out_regs = &usbd->reg.dev.ep.out_regs[ep_idx];
    vsf_dwcotg_dcd_trans_t *trans = &usbd->trans[ep_idx];
    uint_fast32_t size = trans->remain, max_size;
    if (0 == ep_idx) {
        max_size = (1 << 7) - 1;
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
    out_regs->doepctl |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
    return VSF_ERR_NONE;
}

vsf_err_t vsf_dwcotg_usbd_ep_transfer_recv(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    debug_add_evt(__FUNC_EP_TRANSFER_RECV);
    debug_add_data(0xFF000000 + (ep << 16) + size);

    VSF_USB_ASSERT(!(ep & 0x80));
    VSF_USB_ASSERT(ep < usbd->ep_num);

    VSF_USB_ASSERT(((ep == 0) && usbd->ctrl_transfer_state != DWCOTG_SETUP_STAGE) || (ep > 0));

    vsf_dwcotg_dcd_trans_t *trans = &usbd->trans[ep];
    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = false;
    trans->use_dma = usbd->dma_en && !((uint32_t)trans->buffer & 0x03);

    return vsf_dwcotg_usbd_ep_out_transfer(usbd, ep);
}

static vsf_err_t vsf_dwcotg_usbd_ep_in_transfer(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep_idx)
{
    debug_add_evt(__FUNC_EP_IN_TRANSFER);
    VSF_USB_ASSERT(ep_idx < usbd->ep_num);

    uint_fast16_t ep_size = vsf_dwcotg_usbd_ep_get_size(usbd, ep_idx | 0x80);
    struct dwcotg_dev_in_ep_regs_t *in_regs = &usbd->reg.dev.ep.in_regs[ep_idx];
    vsf_dwcotg_dcd_trans_t *trans = &usbd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep_idx];

    uint_fast32_t size = trans->remain, max_size;
    if (0 == ep_idx) {
        max_size = (1 << 7) - 1;
    } else {
        max_size = (1 << 19) - 1;
    }
    size = min(size, max_size);
    if (size < trans->remain) {
        size &= ~(ep_size - 1);
    }
    uint_fast8_t pkt_cnt = (size + ep_size - 1) / ep_size;

    trans->remain -= size;
    if (!trans->remain && trans->zlp && !(size % ep_size)) {
        pkt_cnt++;
    }

    in_regs->dieptsiz = (pkt_cnt << 19) | size;
    if (trans->use_dma) {
        in_regs->diepdma = (uint32_t)trans->buffer;
        in_regs->diepctl |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
    } else {
        in_regs->diepctl |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
        vsf_dwcotg_usbd_ep_write(usbd, ep_idx);
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_dwcotg_usbd_ep_transfer_send(vsf_dwcotg_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    debug_add_evt(__FUNC_EP_TRANSFER_SEND);

    VSF_USB_ASSERT(ep & 0x80);

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < usbd->ep_num);

    vsf_dwcotg_dcd_trans_t *trans = &usbd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep];
    trans->buffer = buffer;
    trans->remain = size;
    trans->size = size;
    trans->zlp = zlp;
    // if fifo transfer is not supported in dma mode(for unaligned buffer), add assert here
    trans->use_dma = usbd->dma_en && !((uint32_t)trans->buffer & 0x03);

    return vsf_dwcotg_usbd_ep_in_transfer(usbd, ep);
}

static void vsf_dwcotg_usbd_notify(vsf_dwcotg_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evt_handler != NULL) {
        usbd->callback.evt_handler(usbd->callback.param, evt, value);
    }
}

void vsf_dwcotg_usbd_irq(vsf_dwcotg_dcd_t *usbd)
{
    struct dwcotg_core_global_regs_t *global_regs = usbd->reg.global_regs;
    struct dwcotg_dev_global_regs_t *dev_global_regs = usbd->reg.dev.global_regs;
    struct dwcotg_dev_in_ep_regs_t *in_regs = usbd->reg.dev.ep.in_regs;
    struct dwcotg_dev_out_ep_regs_t *out_regs = usbd->reg.dev.ep.out_regs;
    uint_fast32_t intsts = global_regs->gintmsk | USB_OTG_GINTSTS_CMOD;
    vsf_dwcotg_dcd_trans_t *trans;

    intsts &= global_regs->gintsts;

    debug_add_evt(__USB_ON_IRQ);
    debug_add_evt(intsts);

    VSF_USB_ASSERT(!(intsts & USB_OTG_GINTSTS_CMOD));

    if (intsts & USB_OTG_GINTSTS_MMIS) {
        debug_add_evt(__USB_ON_MMIS);
        VSF_USB_ASSERT(false);
        global_regs->gintsts = USB_OTG_GINTSTS_MMIS;
    }
    if (intsts & USB_OTG_GINTSTS_USBRST) {
        debug_add_evt(__USB_ON_RST);
        vsf_dwcotg_usbd_notify(usbd, USB_ON_RESET, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_USBRST;
    }
    if (intsts & USB_OTG_GINTSTS_ENUMDNE) {
        debug_add_evt(__USB_ON_ENUMDNE);
        uint8_t speed = (dev_global_regs->dsts & USB_OTG_DSTS_ENUMSPD) >> 1;
        global_regs->gusbcfg &= ~USB_OTG_GUSBCFG_TRDT;
        global_regs->gusbcfg |= ((0/* USB_SPEED_HIGH*/ == speed) ? 0x09U : 0x05U) << 10;
        dev_global_regs->dctl |= USB_OTG_DCTL_CGINAK;
        global_regs->gintsts = USB_OTG_GINTSTS_ENUMDNE;
    }
    if (intsts & USB_OTG_GINTSTS_USBSUSP) {
        debug_add_evt(__USB_ON_USBSUSP);
        vsf_dwcotg_usbd_notify(usbd, USB_ON_SUSPEND, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_USBSUSP;
    }
    if (intsts & USB_OTG_GINTSTS_WKUINT) {
        debug_add_evt(__USB_ON_WKUINTP);
        vsf_dwcotg_usbd_notify(usbd, USB_ON_RESUME, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_WKUINT;
    }
    if (intsts & USB_OTG_GINTSTS_SOF) {
        debug_add_evt(__USB_ON_SOF);
        vsf_dwcotg_usbd_notify(usbd, USB_ON_SOF, 0);
        global_regs->gintsts = USB_OTG_GINTSTS_SOF;
    }

    if (intsts & USB_OTG_GINTSTS_IEPINT) {
        debug_add_evt(__USB_ON_IEPINT);
        uint_fast8_t ep_idx = 0;
        uint_fast32_t ep_int = dev_global_regs->daint;
        ep_int = (ep_int & dev_global_regs->daintmsk) & 0xffff;

        while (ep_int) {
            if (ep_int & 0x1) {
                uint_fast32_t int_status = in_regs[ep_idx].diepint;
                debug_add_data(0xFF000000 + (ep_idx << 16) + int_status);

                uint_fast32_t int_msak = dev_global_regs->diepmsk | USB_OTG_DIEPINT_INEPNE | USB_OTG_DIEPINT_NAK;
                int_status &= (int_msak | USB_OTG_DIEPINT_TXFE);

                if (int_status & USB_OTG_DIEPINT_XFRC) {
                    if ((ep_idx == 0) && (usbd->ctrl_transfer_state == DWCOTG_STATUS_STAGE)) {
                        usbd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;
                        debug_add_evt(USB_ON_STATUS);
                        vsf_dwcotg_usbd_notify(usbd, USB_ON_STATUS, 0);
                    } else {
                        trans = &usbd->trans[VSF_DWCOTG_DCD_CFG_EP_NUM + ep_idx];
                        if (trans->remain) {
                            debug_add_evt(__USB_ON_ISR_IN_TRANSFER);
                            vsf_dwcotg_usbd_ep_in_transfer(usbd, ep_idx);
                        } else {
                            debug_add_evt(__USB_ON_IN);
                            vsf_dwcotg_usbd_notify(usbd, USB_ON_IN, ep_idx);
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
                    debug_add_evt(__USB_ON_DIEPINT_TXFE);
                    vsf_dwcotg_usbd_ep_write(usbd, ep_idx);
                    in_regs[ep_idx].diepint = USB_OTG_DIEPINT_TXFE;
                }
            }
            ep_int >>= 1;
            ep_idx++;
        }
    }

    if (intsts & USB_OTG_GINTSTS_OEPINT) {
        debug_add_evt(__USB_ON_OEPINT);
        uint_fast8_t ep_idx = 0;
        uint_fast32_t ep_int = dev_global_regs->daint;
        ep_int = (ep_int & dev_global_regs->daintmsk) >> 16;

        while (ep_int) {
            if (ep_int & 0x1) {
                uint_fast32_t int_status = out_regs[ep_idx].doepint;
                debug_add_data(0xFF000000 + (ep_idx << 16) + int_status);

                int_status &= dev_global_regs->doepmsk | USB_OTG_DOEPINT_STSPHSERCVD;

                // transfer complete interrupt
                if (int_status & USB_OTG_DOEPINT_XFRC) {
                    if ((ep_idx == 0) && (usbd->ctrl_transfer_state == DWCOTG_STATUS_STAGE)) {
                        usbd->ctrl_transfer_state = DWCOTG_SETUP_STAGE;
                        debug_add_evt(__USB_ON_STATUS);
                        vsf_dwcotg_usbd_notify(usbd, USB_ON_STATUS, 0);
                    } else if (((ep_idx == 0) && usbd->ctrl_transfer_state == DWCOTG_DATA_STAGE) || (ep_idx > 0)) {
                        trans = &usbd->trans[ep_idx];
                        if (!trans->zlp) {
                            debug_add_evt(__USB_ON_ISR_OUT_TRANSFER);
                            vsf_dwcotg_usbd_ep_out_transfer(usbd, ep_idx);
                        } else {
                            debug_add_evt(__USB_ON_OUT);
                            vsf_dwcotg_usbd_notify(usbd, USB_ON_OUT, ep_idx);
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
                    if (    ((((uint32_t *)usbd->setup)[0] & 0xFF00FFFF) == 0x00000500)
                        &&  (((uint32_t *)usbd->setup)[1] == 0x0)) {
                        VSF_USB_ASSERT(!vsf_dwcotg_usbd_get_address(usbd));
                        dev_global_regs->dcfg |= (usbd->setup[2] & 0x7F) << 4;
                    }

                    debug_add_evt(__USB_ON_SETUP);
                    usbd->ctrl_transfer_state = DWCOTG_DATA_STAGE;
                    vsf_dwcotg_usbd_notify(usbd, USB_ON_SETUP, 0);
                    out_regs[ep_idx].doepint = USB_OTG_DOEPINT_STUP;
                }
                // back to back setup packets received
                if (int_status & USB_OTG_DOEPINT_B2BSTUP) {
                    debug_add_evt(__USB_ON_DOEPINT_B2BSTUPP);
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
        uint_fast8_t ep_idx, pid;
        uint_fast16_t size;
        uint_fast32_t rx_status;

        global_regs->gintmsk &= ~USB_OTG_GINTMSK_RXFLVLM;
        rx_status = global_regs->grxstsp;

        debug_add_evt(__USB_ON_RXFLVL);
        debug_add_evt(0x80000000 + ((rx_status & USB_OTG_GRXSTSP_PKTSTS) >> 17));

        ep_idx = rx_status & USB_OTG_GRXSTSP_EPNUM;
        size = (rx_status & USB_OTG_GRXSTSP_BCNT) >> 4;
        pid = (rx_status & USB_OTG_GRXSTSP_DPID) >> 15;

        switch ((rx_status & USB_OTG_GRXSTSP_PKTSTS) >> 17) {
        case 1:
        case 3:
        case 4:
            break;
        case 6: //RXSTAT_SETUP_UPDT:
            if (!ep_idx && (8 == size) && (0/*DPID_DATA0*/ == pid)) {
                // In some versions of dwcotg, We can't replace dfifo[0] with grxstsp[0]
                ((uint32_t *)usbd->setup)[0] = *usbd->use_as__vsf_dwcotg_t.reg.dfifo[0];
                ((uint32_t *)usbd->setup)[1] = *usbd->use_as__vsf_dwcotg_t.reg.dfifo[0];

                debug_add_evt(__USB_ON_RXSTAT_SETUP_UPDT);
                debug_add_data(((uint32_t *)usbd->setup)[0]);
                debug_add_data(((uint32_t *)usbd->setup)[1]);
            }
            break;
        case 2: //RXSTAT_DATA_UPDT:
            debug_add_evt(__USB_ON_RXSTAT_DATA_UPDT);
            debug_add_evt(0x80000000 + (ep_idx << 16) + size);
            vsf_dwcotg_usbd_ep_read(usbd, ep_idx, size);
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

#endif
