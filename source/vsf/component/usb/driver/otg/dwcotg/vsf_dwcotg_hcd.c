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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_DWCOTG_HCD_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__

#include "kernel/vsf_kernel.h"
#include "component/usb/host/vsf_usbh.h"
#include "./vsf_dwcotg_hcd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    DWCOTG_HCD_DPIP_DATA0 = 0,
    DWCOTG_HCD_DPIP_DATA1 = 2,
    DWCOTG_HCD_DPIP_SETUP = 3,
};

enum vk_dwcotg_hcd_urb_channel_phase_t {
    VSF_DWCOTG_HCD_PHASE_IDLE = 0,
    VSF_DWCOTG_HCD_PHASE_PERIOD,
    VSF_DWCOTG_HCD_PHASE_SETUP,
    VSF_DWCOTG_HCD_PHASE_DATA,
    VSF_DWCOTG_HCD_PHASE_STATUS,
    VSF_DWCOTG_HCD_PHASE_DONE,
} vk_dwcotg_hcd_urb_channel_phase_t;

enum vk_dwcotg_hcd_urb_channel_result_t {
    VSF_DWCOTG_HCD_CHANNEL_DONE = 0,
    VSF_DWCOTG_HCD_CHANNEL_NAK,
    VSF_DWCOTG_HCD_CHANNEL_NYET,
    VSF_DWCOTG_HCD_CHANNEL_ERROR,
    VSF_DWCOTG_HCD_CHANNEL_STALL,
    VSF_DWCOTG_HCD_CHANNEL_TOGGLE_ERROR,
    VSF_DWCOTG_HCD_CHANNEL_FRAME_ERROR,
    VSF_DWCOTG_HCD_CHANNEL_TRANSACT_ERROR,
} vk_dwcotg_hcd_urb_channel_result_t;

typedef struct vk_dwcotg_hcd_urb_t {
    vsf_slist_node_t node;

    uint16_t channel_idx    : 4;
    uint16_t is_alloced     : 1;
    uint16_t is_discarded   : 1;
    uint16_t err_cnt        : 2;
    uint16_t phase          : 3;
    uint16_t toggle         : 1;
    uint16_t do_ping        : 1;

    uint16_t current_size;
} vk_dwcotg_hcd_urb_t;

typedef struct vk_dwcotg_hcd_t {
    implement(vk_dwcotg_t)
    implement(vk_dwcotg_param_t)

    enum {
        DWCOTG_HCD_STATE_WAIT_ABH_IDLE,
        DWCOTG_HCD_STATE_WAIT_RESET,
        DWCOTG_HCD_STATE_WAIT_ENABLE,
        DWCOTG_HCD_STATE_WAIT_FLUSH_TXFIFO,
        DWCOTG_HCD_STATE_WAIT_FLUSH_RXFIFO,
        DWCOTG_HCD_STATE_WORKING,
    } state;

    uint8_t is_port_changed : 1;
    uint8_t is_connected    : 1;
    uint16_t ep_mask;
    volatile uint32_t softick;

    vsf_eda_t task;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;
    vsf_slist_queue_t ready_queue;
    vsf_slist_queue_t pending_queue;

    vk_usbh_hcd_urb_t *urb[0];
} vk_dwcotg_hcd_t;

/*============================ PROTOTYPES ====================================*/

extern void vsf_dwcotg_hcd_get_fifo_size(vk_usbh_hcd_t *hcd,
        uint16_t *rx_fifo_size, uint16_t *non_periodic_tx_fifo_size, uint16_t *periodic_tx_fifo_size);

static vsf_err_t __vk_dwcotg_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __vk_dwcotg_hcd_get_frame_number(vk_usbh_hcd_t *hcd);
static vk_usbh_hcd_urb_t * __vk_dwcotg_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_dwcotg_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_dwcotg_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_dwcotg_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static int __vk_dwcotg_hcd_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);

static void __vk_dwcotg_hcd_interrupt(void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_dwcotg_hcd_drv = {
    .init_evthandler    = __vk_dwcotg_hcd_init_evthandler,
    .fini               = __vk_dwcotg_hcd_fini,
    .suspend            = __vk_dwcotg_hcd_suspend,
    .resume             = __vk_dwcotg_hcd_resume,
    .get_frame_number   = __vk_dwcotg_hcd_get_frame_number,
    .alloc_urb          = __vk_dwcotg_hcd_alloc_urb,
    .free_urb           = __vk_dwcotg_hcd_free_urb,
    .submit_urb         = __vk_dwcotg_hcd_submit_urb,
    .relink_urb         = __vk_dwcotg_hcd_relink_urb,
    .rh_control         = __vk_dwcotg_hcd_rh_control,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_DWCOTG_HCD_GET_FIFO_SIZE
WEAK(vsf_dwcotg_hcd_get_fifo_size)
void vsf_dwcotg_hcd_get_fifo_size(vk_usbh_hcd_t *hcd,
        uint16_t *rx_fifo_size, uint16_t *non_periodic_tx_fifo_size, uint16_t *periodic_tx_fifo_size)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    switch (dwcotg_hcd->speed) {
    case USB_SPEED_HIGH:
        *rx_fifo_size = 1024;
        *non_periodic_tx_fifo_size = 512;
        *periodic_tx_fifo_size = 1024;
        break;
    case USB_SPEED_FULL:
    default:
        *rx_fifo_size = 256;
        *non_periodic_tx_fifo_size = 256;
        *periodic_tx_fifo_size = 256;
        break;
    }
}
#endif

static void __vk_dwcotg_hcd_init_regs(vk_dwcotg_hcd_t *dwcotg_hcd, void *regbase, uint_fast8_t ep_num)
{
    dwcotg_hcd->reg.global_regs = regbase;
    dwcotg_hcd->reg.host.global_regs = (void *)((uint8_t *)regbase + 0x400);
    dwcotg_hcd->reg.host.hprt0 = (void *)((uint8_t *)regbase + 0x440);
    dwcotg_hcd->reg.host.hc_regs = (void *)((uint8_t *)regbase + 0x500);

    VSF_USB_ASSERT(dimof(dwcotg_hcd->reg.dfifo) >= ep_num);
    for (uint_fast8_t i = 0; i < ep_num; i++) {
        dwcotg_hcd->reg.dfifo[i] = (void *)((uint8_t *)regbase + (i + 1) * 0x1000);
    }
}

static void __vk_dwcotg_hcd_free_urb_do(vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_hcd_urb_free_buffer(urb);
    vsf_usbh_free(urb);
}

static void __vk_dwcotg_hcd_halt_channel(vk_dwcotg_hcd_t *dwcotg_hcd, uint_fast8_t channel_idx)
{
    struct dwcotg_hc_regs_t *channel_regs = &dwcotg_hcd->reg.host.hc_regs[channel_idx];
    vk_usbh_hcd_urb_t *urb = dwcotg_hcd->urb[channel_idx];
    VSF_USB_ASSERT(urb != NULL);

    channel_regs->hcchar |= USB_OTG_HCCHAR_CHDIS;
    if (    (urb->pipe.type == USB_ENDPOINT_XFER_BULK)
        ||  (urb->pipe.type == USB_ENDPOINT_XFER_INT)) {
        if (dwcotg_hcd->reg.global_regs->gnptxsts & 0xFFFF) {
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_EPDIR;
            // skip delay
        } else {
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
        }
    } else {
        if (dwcotg_hcd->reg.host.global_regs->hptxsts & 0xFFFF) {
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_EPDIR;
            // skip delay
        } else {
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
        }
    }
}

static void __vk_dwcotg_hcd_commit_urb(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb,
                        uint_fast8_t dpid, bool dir_in1out0, uint8_t *buffer, uint_fast16_t size)
{
    const uint8_t eptype_to_dwctype[4] = {
        [USB_ENDPOINT_XFER_CONTROL] = 0,
        [USB_ENDPOINT_XFER_ISOC]    = 1,
        [USB_ENDPOINT_XFER_BULK]    = 2,
        [USB_ENDPOINT_XFER_INT]     = 3,
    };

    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    struct dwcotg_hc_regs_t *channel_regs = &dwcotg_hcd->reg.host.hc_regs[dwcotg_urb->channel_idx];
    vk_usbh_pipe_t pipe = urb->pipe;
    uint_fast32_t pkt_num;

    VSF_USB_ASSERT(!(dwcotg_hcd->reg.host.global_regs->haintmsk & (1 << dwcotg_urb->channel_idx)));
//    VSF_USB_ASSERT(!(channel_regs->hcchar & USB_OTG_HCCHAR_CHDIS));

    channel_regs->hcchar =  (pipe.address << 22) | (pipe.endpoint << 11) |
                            (dir_in1out0 ? USB_OTG_HCCHAR_EPDIR : 0) |
                            ((pipe.speed == USB_SPEED_LOW) << 17) |
                            ((uint32_t)eptype_to_dwctype[pipe.type] << 18) |
                            (pipe.size & USB_OTG_HCCHAR_MPSIZ);
    if (pipe.type == USB_ENDPOINT_XFER_INT) {
        channel_regs->hcchar |= USB_OTG_HCCHAR_ODDFRM;
    }

    if (size > 0) {
        pkt_num = (size + pipe.size - 1) / pipe.size;
        VSF_USB_ASSERT(pkt_num <= 256);
    } else {
        pkt_num = 1;
    }

    if (dir_in1out0) {
        size = pipe.size * pkt_num;
    }
    dwcotg_urb->current_size = size;
    channel_regs->hctsiz = ((pkt_num << 19) & USB_OTG_HCTSIZ_PKTCNT) | ((uint32_t)dpid << 29) | size;

    if (dwcotg_hcd->dma_en) {
        VSF_USB_ASSERT(!((uintptr_t)buffer & 0x03));
        channel_regs->hcdma = (uint32_t)buffer;
    }

    channel_regs->hcchar |= ((dwcotg_hcd->reg.host.global_regs->hfnum & 1) ^ 1) << 29;
    channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;

    channel_regs->hcintmsk = USB_OTG_HCINTMSK_CHHM;
    vsf_protect_t orig = vsf_protect_int();
        dwcotg_hcd->reg.host.global_regs->haintmsk |= 1 << dwcotg_urb->channel_idx;
    vsf_unprotect_int(orig);

    if (!dwcotg_hcd->dma_en && !dir_in1out0 && (dwcotg_urb->current_size > 0)) {
        // TODO: check FIFO space
        uint32_t *fifo_reg = dwcotg_hcd->reg.dfifo[dwcotg_urb->channel_idx];
        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
            *fifo_reg = get_unaligned_le32(buffer);
        }
    }
}

static void __vk_dwcotg_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
//    vk_dwcotg_hcd_t *dwcotg_hcd = container_of(eda, vk_dwcotg_hcd_t, task);

    switch (evt) {
    case VSF_EVT_MESSAGE: {
            vk_usbh_hcd_urb_t *urb = vsf_eda_get_cur_msg();
            vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;

            if (dwcotg_urb->is_discarded) {
                __vk_dwcotg_hcd_free_urb_do(urb);
                return;
            }
            VSF_USB_ASSERT(false);
        }
        break;
    }
}

static vsf_err_t __vk_dwcotg_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = NULL;
    vk_dwcotg_hcd_param_t *param;
    vk_dwcotg_reg_t *reg = NULL;

    VSF_USB_ASSERT(hcd != NULL);
    if (hcd->priv != NULL) {
        dwcotg_hcd = hcd->priv;
        reg = &dwcotg_hcd->reg;
    }
    param = hcd->param;

    switch (evt) {
    case VSF_EVT_INIT: {
            vk_dwcotg_hc_ip_info_t info;
            param->op->GetInfo(&info.use_as__usb_hc_ip_info_t);
            VSF_USB_ASSERT(info.dma_en);

            uint_fast32_t dwcotg_size = sizeof(*dwcotg_hcd) + info.ep_num * sizeof(vk_usbh_hcd_urb_t *);
            dwcotg_hcd = hcd->priv = vsf_usbh_malloc(dwcotg_size);
            if (NULL == dwcotg_hcd) {
                VSF_USB_ASSERT(false);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }
            memset(dwcotg_hcd, 0, dwcotg_size);
            dwcotg_hcd->hcd = hcd;

            {
                usb_hc_ip_cfg_t cfg = {
                    .priority       = param->priority,
                    .irq_handler    = __vk_dwcotg_hcd_interrupt,
                    .param          = dwcotg_hcd,
                };
                // interrupt is disabled after init
                param->op->Init(&cfg);
            }

            dwcotg_hcd->dma_en = info.dma_en;
            hcd->rh_speed = dwcotg_hcd->speed = info.speed;
            dwcotg_hcd->ep_num = info.ep_num;
            dwcotg_hcd->ep_mask = ~((1 << info.ep_num) - 1);
            __vk_dwcotg_hcd_init_regs(dwcotg_hcd, info.regbase, dwcotg_hcd->ep_num);
            reg = &dwcotg_hcd->reg;

            if (dwcotg_hcd->ulpi_en) {
                // GCCFG &= ~(USB_OTG_GCCFG_PWRDWN)
                reg->global_regs->gccfg &= ~USB_OTG_GCCFG_PWRDWN;

                // Init The ULPI Interface
                reg->global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
                // Select vbus source
                reg->global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
                if (dwcotg_hcd->vbus_en) {
                    reg->global_regs->gusbcfg |= USB_OTG_GUSBCFG_ULPIEVBUSD;
                }
            } else {
                reg->global_regs->gusbcfg |= USB_OTG_GUSBCFG_PHYSEL;
            }

            dwcotg_hcd->state = DWCOTG_HCD_STATE_WAIT_ABH_IDLE;
        }
        // fall through
    case VSF_EVT_TIMER:
        switch (dwcotg_hcd->state) {
        case DWCOTG_HCD_STATE_WAIT_ABH_IDLE:
            if (!(reg->global_regs->grstctl & USB_OTG_GRSTCTL_AHBIDL)) {
                vsf_teda_set_timer_ms(2);
                break;
            }

            reg->global_regs->grstctl |= USB_OTG_GRSTCTL_CSRST;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WAIT_RESET;
            // fall through
        case DWCOTG_HCD_STATE_WAIT_RESET:
            if (reg->global_regs->grstctl & USB_OTG_GRSTCTL_CSRST) {
                vsf_teda_set_timer_ms(2);
                break;
            }

            if (dwcotg_hcd->ulpi_en == 0) {
                reg->global_regs->gccfg = USB_OTG_GCCFG_PWRDWN;
            }
            if (dwcotg_hcd->dma_en) {
                reg->global_regs->gahbcfg |= USB_OTG_GAHBCFG_DMAEN | USB_OTG_GAHBCFG_HBSTLEN_1 | USB_OTG_GAHBCFG_HBSTLEN_2;
            }

            // Force Host Mode
            reg->global_regs->gusbcfg &= ~USB_OTG_GUSBCFG_FDMOD;
            reg->global_regs->gusbcfg |= USB_OTG_GUSBCFG_FHMOD;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WAIT_ENABLE;
            vsf_teda_set_timer_ms(50);
            break;
        case DWCOTG_HCD_STATE_WAIT_ENABLE:
            // USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
            reg->global_regs->gccfg |= USB_OTG_GCCFG_VBDEN;
            reg->host.global_regs->hcfg &= ~USB_OTG_HCFG_FSLSPCS;
            if (dwcotg_hcd->speed == USB_SPEED_HIGH) {
                reg->host.global_regs->hcfg &= ~USB_OTG_HCFG_FSLSS;
            } else if (dwcotg_hcd->speed == USB_SPEED_FULL) {
                reg->host.global_regs->hcfg |= USB_OTG_HCFG_FSLSS | USB_OTG_HCFG_FSLSPCS_0;
            } else {
                VSF_USB_ASSERT(false);
            }
            // TODO: set reg->host.global_regs->hfir if SOF interval is not 1ms

            // Flush FIFO
            reg->global_regs->grstctl = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM_4;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WAIT_FLUSH_TXFIFO;
            // fall through
        case DWCOTG_HCD_STATE_WAIT_FLUSH_TXFIFO:
            if (reg->global_regs->grstctl & USB_OTG_GRSTCTL_TXFFLSH) {
                vsf_teda_set_timer_ms(2);
                break;
            }

            reg->global_regs->grstctl = USB_OTG_GRSTCTL_RXFFLSH;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WAIT_FLUSH_RXFIFO;
            // fall through
        case DWCOTG_HCD_STATE_WAIT_FLUSH_RXFIFO:
            if (reg->global_regs->grstctl & USB_OTG_GRSTCTL_RXFFLSH) {
                vsf_teda_set_timer_ms(2);
                break;
            }

            // Clear all pending HC Interrupts
            for (int i = 0; i < dwcotg_hcd->ep_num; i++) {
                reg->host.hc_regs[i].hcintmsk = 0;
                reg->host.hc_regs[i].hcint = 0xFFFFFFFF;
            }
            reg->global_regs->gintmsk = 0;
            reg->global_regs->gintsts = 0xFFFFFFFF;

            {
                uint16_t rx_fifo_size;
                uint16_t non_periodic_tx_fifo_size;
                uint16_t periodic_tx_fifo_size;

                vsf_dwcotg_hcd_get_fifo_size(hcd,
                    &rx_fifo_size, &non_periodic_tx_fifo_size, &periodic_tx_fifo_size);

                reg->global_regs->grxfsiz = rx_fifo_size >> 2;
                reg->global_regs->gnptxfsiz = ((non_periodic_tx_fifo_size >> 2) << 16) | (reg->global_regs->grxfsiz & 0xffff);
                reg->global_regs->hptxfsiz = ((periodic_tx_fifo_size >> 2) << 16) | (reg->global_regs->gnptxfsiz & 0xffff);
            }

            if (!dwcotg_hcd->dma_en) {
                reg->global_regs->gintmsk |= USB_OTG_GINTMSK_RXFLVLM;
            }
            // TODO: add back USB_OTG_GINTMSK_PXFRM_IISOOXFRM if know how to process
            reg->global_regs->gintmsk |= USB_OTG_GINTMSK_HCIM | USB_OTG_GINTMSK_SOFM;
            reg->global_regs->gahbcfg |= USB_OTG_GAHBCFG_GINT;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WORKING;

            dwcotg_hcd->task.fn.evthandler = __vk_dwcotg_hcd_evthandler;
            vsf_eda_init(&dwcotg_hcd->task, vsf_prio_inherit, false);
            return VSF_ERR_NONE;
        }
    }

    return VSF_ERR_NOT_READY;
}

static vsf_err_t __vk_dwcotg_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_dwcotg_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_dwcotg_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __vk_dwcotg_hcd_get_frame_number(vk_usbh_hcd_t *hcd)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    vk_dwcotg_reg_t *reg = &dwcotg_hcd->reg;
    return reg->host.global_regs->hfnum & 0xFFFF;
}

static vk_usbh_hcd_urb_t * __vk_dwcotg_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    uint_fast32_t size = sizeof(vk_usbh_hcd_urb_t) + sizeof(vk_dwcotg_hcd_urb_t);
    vk_usbh_hcd_urb_t *urb = vsf_usbh_malloc(size);
    if (urb != NULL) {
        memset(urb, 0, size);
    }
    return urb;
}

static void __vk_dwcotg_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    vsf_protect_t orig = vsf_protect_int();
    if (dwcotg_urb->is_alloced) {
        dwcotg_urb->is_discarded = true;
        vsf_unprotect_int(orig);
//        __vk_dwcotg_hcd_halt_channel((vk_dwcotg_hcd_t *)hcd->priv, dwcotg_urb->channel_idx);
    } else {
        vsf_unprotect_int(orig);
        __vk_dwcotg_hcd_free_urb_do(urb);
    }
}

static bool __vk_dwcotg_hcd_is_period_hit(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb)
{
    uint_fast32_t interval = urb->interval;
    if (!interval) {
        return true;
    }
    if (USB_SPEED_HIGH == dwcotg_hcd->speed) {
        interval <<= 3;
    }
    return dwcotg_hcd->softick % interval == 0;
}

static void __vk_dwcotg_hcd_urb_fsm(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    vk_usbh_pipe_t pipe = urb->pipe;

    if (!dwcotg_urb->is_alloced) {
        int_fast8_t index;
        vsf_protect_t orig = vsf_protect_int();
            index = vsf_ffz(dwcotg_hcd->ep_mask);
            if (index >= 0) {
                dwcotg_hcd->ep_mask |= 1 << index;
                dwcotg_urb->is_alloced = true;
                dwcotg_hcd->urb[index] = urb;
                dwcotg_urb->channel_idx = index;
            } else {
                vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->ready_queue, dwcotg_urb);
            }
        vsf_unprotect_int(orig);

        if (index < 0) {
            return;
        }
    }

    switch (dwcotg_urb->phase) {
    case VSF_DWCOTG_HCD_PHASE_PERIOD:
        VSF_USB_ASSERT(false);
        break;
    case VSF_DWCOTG_HCD_PHASE_SETUP:
        __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, DWCOTG_HCD_DPIP_SETUP, 0, (uint8_t *)&urb->setup_packet, sizeof(urb->setup_packet));
        break;
    case VSF_DWCOTG_HCD_PHASE_DATA: {
            uint_fast8_t dpid;
            switch (pipe.type) {
            case USB_ENDPOINT_XFER_CONTROL:
                dpid = DWCOTG_HCD_DPIP_DATA1;
                break;
            case USB_ENDPOINT_XFER_ISOC:
                dpid = DWCOTG_HCD_DPIP_DATA0;
                break;
            case USB_ENDPOINT_XFER_BULK:
            case USB_ENDPOINT_XFER_INT:
                dpid = dwcotg_urb->toggle ? DWCOTG_HCD_DPIP_DATA1 : DWCOTG_HCD_DPIP_DATA0;
                break;
            }
            __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, dpid, pipe.dir_in1out0, (uint8_t *)urb->buffer, urb->transfer_length);
        }
        break;
    case VSF_DWCOTG_HCD_PHASE_STATUS:
        __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, DWCOTG_HCD_DPIP_DATA1, !pipe.dir_in1out0, NULL, 0);
        break;
    default:
        VSF_USB_ASSERT(false);
        break;
    }
}

static void __vk_dwcotg_hcd_channel_interrupt(vk_dwcotg_hcd_t *dwcotg_hcd, uint_fast8_t channel_idx)
{
    vk_usbh_hcd_urb_t *urb = dwcotg_hcd->urb[channel_idx];
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    struct dwcotg_hc_regs_t *channel_regs = &dwcotg_hcd->reg.host.hc_regs[channel_idx];
    uint_fast32_t channel_intsts = channel_regs->hcint;
    bool is_in = channel_regs->hcchar & USB_OTG_HCCHAR_EPDIR;
    vsf_protect_t orig;

    if (channel_intsts & USB_OTG_HCINT_CHH) {
        orig = vsf_protect_int();
            dwcotg_hcd->reg.host.global_regs->haintmsk &= ~(1 << channel_idx);
        vsf_unprotect_int(orig);

        channel_regs->hcintmsk = 0;
        channel_regs->hcint = channel_regs->hcint;

        if (dwcotg_urb->is_discarded) {
            goto free_channel;
        }

        channel_intsts &= ~USB_OTG_HCINT_CHH;
        if (channel_intsts & (USB_OTG_HCINT_XFRC | USB_OTG_HCINT_STALL)) {
            bool is_stall = channel_intsts & USB_OTG_HCINT_STALL;
            if (is_in) {
                dwcotg_urb->current_size -= channel_regs->hctsiz & USB_OTG_HCTSIZ_XFRSIZ;
            }

            switch (urb->pipe.type) {
            case USB_ENDPOINT_XFER_CONTROL:
                if (is_stall) {
                    goto urb_done_check_stall;
                }
                switch (dwcotg_urb->phase) {
                case VSF_DWCOTG_HCD_PHASE_SETUP:
                    if (0 == urb->transfer_length) {
                        // skip data phase
                        dwcotg_urb->phase++;
                    }
                    break;
                case VSF_DWCOTG_HCD_PHASE_DATA:
                    urb->actual_length = dwcotg_urb->current_size;
                    break;
                }
                dwcotg_urb->phase++;
                if (dwcotg_urb->phase != VSF_DWCOTG_HCD_PHASE_DONE) {
                    __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
                    return;
                } else {
                urb_done_check_stall:
                    urb->status = is_stall ? URB_FAIL : URB_OK;
                urb_done:
                    vsf_eda_post_msg(urb->eda_caller, urb);
                }
                break;
            case USB_ENDPOINT_XFER_INT:
            case USB_ENDPOINT_XFER_BULK: {
                    int toggle = 0;
                    switch ((channel_regs->hctsiz >> 29) & 3) {
                    case 0: toggle = 0; break;  // DATA0
                    case 2: toggle = 1; break;  // DATA1
                    }
                    usb_settoggle(urb->dev_hcd, urb->pipe.endpoint, !urb->pipe.dir_in1out0, toggle);
                }
                // fall through
            case USB_ENDPOINT_XFER_ISOC:
                urb->actual_length = dwcotg_urb->current_size;
                goto urb_done_check_stall;
            }
        } else if (channel_intsts & USB_OTG_HCINT_NAK) {
        urb_retry:
            switch (urb->pipe.type) {
            case USB_ENDPOINT_XFER_INT:
                orig = vsf_protect_int();
                    vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, dwcotg_urb);
                vsf_unprotect_int(orig);
                break;
            case USB_ENDPOINT_XFER_CONTROL:
                if (VSF_DWCOTG_HCD_PHASE_SETUP == dwcotg_urb->phase) {
                    __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
                    break;
                }
                // fall through
            case USB_ENDPOINT_XFER_BULK:
                // re-activate the channel, no need to halt and re-submit
                channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHDIS;
                channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
                break;
            }
            return;
        } else if (channel_intsts & (USB_OTG_HCINT_TXERR | USB_OTG_HCINT_BBERR | USB_OTG_HCINT_DTERR)) {
            // data toggle error, babble error, usb bus error
            urb->status = URB_FAIL;
            goto urb_done;
        } else if (channel_intsts & USB_OTG_HCINT_FRMOR) {
            // frame error
            // TODO: update err_cnt
            goto urb_retry;
        }

    free_channel: {
            bool is_discarded;

            orig = vsf_protect_int();
                dwcotg_hcd->urb[channel_idx] = NULL;
                channel_regs->hctsiz = 0;

                is_discarded = dwcotg_urb->is_discarded;
                dwcotg_urb->is_alloced = false;
                vsf_slist_queue_dequeue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->ready_queue, dwcotg_urb);
                if (NULL == dwcotg_urb) {
                    dwcotg_hcd->ep_mask &= ~(1UL << channel_idx);
                } else {
                    urb = container_of(dwcotg_urb, vk_usbh_hcd_urb_t, priv);
                    dwcotg_hcd->urb[channel_idx] = urb;
                    dwcotg_urb->channel_idx = channel_idx;
                }
            vsf_unprotect_int(orig);

            if (is_discarded) {
                vsf_eda_post_msg((vsf_eda_t *)&dwcotg_hcd->task, urb);
            }
            if (dwcotg_urb != NULL) {
                __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
            }
        }
    }
}

static void __vk_dwcotg_hcd_interrupt(void *param)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = param;
    uint_fast32_t intsts = dwcotg_hcd->reg.global_regs->gintmsk;
    volatile uint32_t *intsts_reg = &dwcotg_hcd->reg.global_regs->gintsts;
    intsts &= *intsts_reg;
    vsf_protect_t orig;

    if (!intsts) {
        VSF_USB_ASSERT(false);
        return;
    }

    if (intsts & USB_OTG_GINTSTS_SOF) {
        vk_usbh_hcd_urb_t *urb;
        vsf_slist_t queue;

        *intsts_reg = USB_OTG_GINTSTS_SOF;
        dwcotg_hcd->softick++;

        orig = vsf_protect_int();
            queue.head = dwcotg_hcd->pending_queue.head.next;
            vsf_slist_queue_init(&dwcotg_hcd->pending_queue);
        vsf_unprotect_int(orig);

        // after device connecting to roothub is removed, there is possibility that the channel
        //  can not be halted, so check in SOF, if the channel need to be halted
        for (uint_fast8_t i = 0; (dwcotg_hcd->ep_mask != 0) && (i < dwcotg_hcd->ep_num); i++) {
            if (dwcotg_hcd->ep_mask & (1 << i)) {
                VSF_USB_ASSERT(dwcotg_hcd->urb[i] != NULL);
                vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&dwcotg_hcd->urb[i]->priv;
                if (dwcotg_urb->is_discarded) {
                    __vk_dwcotg_hcd_halt_channel(dwcotg_hcd, i);
                }
            }
        }

        __vsf_slist_foreach_next_unsafe(vk_dwcotg_hcd_urb_t, node, &queue) {
            urb = container_of(_, vk_usbh_hcd_urb_t, priv);
            if (__vk_dwcotg_hcd_is_period_hit(dwcotg_hcd, urb)) {
                if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.type) {
                    _->phase = VSF_DWCOTG_HCD_PHASE_SETUP;
                } else {
                    _->phase = VSF_DWCOTG_HCD_PHASE_DATA;
                }
                __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
            } else {
                vsf_slist_init_node(vk_dwcotg_hcd_urb_t, node, _);
                orig = vsf_protect_int();
                    vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, _);
                vsf_unprotect_int(orig);
            }
        }
    }
    if (intsts & USB_OTG_GINTSTS_RXFLVL) {
        *intsts_reg = USB_OTG_GINTSTS_RXFLVL;
    }
    if (intsts & USB_OTG_GINTSTS_NPTXFE) {
        *intsts_reg = USB_OTG_GINTSTS_NPTXFE;
    }
    if (intsts & USB_OTG_GINTSTS_HPRTINT) {
        *intsts_reg = USB_OTG_GINTSTS_HPRTINT;
    }
    if (intsts & USB_OTG_GINTSTS_PTXFE) {
        *intsts_reg = USB_OTG_GINTSTS_PTXFE;
    }

    if (intsts & USB_OTG_GINTSTS_HCINT) {
        *intsts_reg = USB_OTG_GINTSTS_HCINT;

        uint_fast32_t haint = dwcotg_hcd->reg.host.global_regs->haint;
        haint &= dwcotg_hcd->reg.host.global_regs->haintmsk;

        for (uint_fast8_t i = 0; i < dwcotg_hcd->ep_num; i++) {
            if (haint & (1UL << i)) {
                __vk_dwcotg_hcd_channel_interrupt(dwcotg_hcd, i);
            }
        }
    }
}

static vsf_err_t __vk_dwcotg_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    vk_usbh_pipe_t pipe = urb->pipe;
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;

    if (!dwcotg_hcd->is_connected) {
        return VSF_ERR_FAIL;
    }

    memset(dwcotg_urb, 0, sizeof(*dwcotg_urb));
    switch (pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        dwcotg_urb->phase = VSF_DWCOTG_HCD_PHASE_SETUP;
        break;
    case USB_ENDPOINT_XFER_BULK:
        dwcotg_urb->phase = VSF_DWCOTG_HCD_PHASE_DATA;
        goto init_toggle;
    case USB_ENDPOINT_XFER_INT:
        dwcotg_urb->phase = VSF_DWCOTG_HCD_PHASE_PERIOD;
        if (urb->transfer_length > pipe.size) {
            /* IMPORTANT PATCH:
                dwcotg_hcd does not support this kind of transfer,
                patch the endpoint to act as a bulk with period support.
            */
            // TODO: simply path to bulk type will make NAK be operated as bulk type
            urb->pipe.type = USB_ENDPOINT_XFER_BULK;
        }
    init_toggle:
        dwcotg_urb->toggle = usb_gettoggle(urb->dev_hcd, pipe.endpoint, !pipe.dir_in1out0);
        dwcotg_urb->do_ping = dwcotg_hcd->dma_en ? false : (pipe.speed == USB_SPEED_HIGH);
        break;
    case USB_ENDPOINT_XFER_ISOC:
        dwcotg_urb->phase = VSF_DWCOTG_HCD_PHASE_PERIOD;
        break;
    default:
        VSF_USB_ASSERT(false);
        return VSF_ERR_INVALID_PARAMETER;
    }

    if (dwcotg_urb->phase == VSF_DWCOTG_HCD_PHASE_PERIOD) {
        if (__vk_dwcotg_hcd_is_period_hit(dwcotg_hcd, urb)) {
            dwcotg_urb->phase = VSF_DWCOTG_HCD_PHASE_DATA;
        } else {
            vsf_protect_t orig = vsf_protect_int();
                vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, dwcotg_urb);
            vsf_unprotect_int(orig);
            return VSF_ERR_NONE;
        }
    }
    __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_dwcotg_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_dwcotg_hcd_submit_urb(hcd, urb);
}

static uint_fast32_t __vk_dwcotg_hcd_rh_portstat(vk_dwcotg_hcd_t *dwcotg_hcd, uint_fast32_t hprt0)
{
    uint_fast32_t value = 0;
    bool is_connected = false;

    if ((hprt0 & USB_OTG_HPRT_PSPD) == 0) {
        value |= USB_PORT_STAT_HIGH_SPEED;
    } else if ((hprt0 & USB_OTG_HPRT_PSPD) == USB_OTG_HPRT_PSPD_1) {
        value |= USB_PORT_STAT_LOW_SPEED;
    }

    if (hprt0 & USB_OTG_HPRT_PENA) {
        value |= USB_PORT_STAT_ENABLE;
    }

    if (hprt0 & USB_OTG_HPRT_PCSTS) {
        value |= USB_PORT_STAT_CONNECTION;
    }

    is_connected = !!(value & USB_PORT_STAT_CONNECTION);
    if (dwcotg_hcd->is_connected != is_connected) {
        dwcotg_hcd->is_connected = is_connected;
        dwcotg_hcd->is_port_changed = true;
    }
    if (dwcotg_hcd->is_port_changed) {
        value |= (USB_PORT_STAT_C_CONNECTION << 16);
    }

    return value;
}

static int __vk_dwcotg_hcd_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    vk_dwcotg_reg_t *reg = &dwcotg_hcd->reg;

    uint16_t wRequest, wValue, wLength;
    struct usb_ctrlrequest_t *req = &urb->setup_packet;
    uint32_t datadw[4];
    uint8_t *data = (uint8_t*)datadw;
    uint8_t len = 0;

    wRequest = (req->bRequestType << 8) | req->bRequest;
    wValue = req->wValue;
//    wIndex = req->wIndex;
    wLength = req->wLength;

    switch (wRequest) {
    case GetHubStatus:
        datadw[0] = 0;
        len = 4;
        break;
    case GetPortStatus:
        datadw[0] = __vk_dwcotg_hcd_rh_portstat(dwcotg_hcd, (uint_fast32_t)*reg->host.hprt0);
        len = 4;
        break;
    case SetPortFeature:
        switch (wValue) {
        case USB_PORT_FEAT_ENABLE:
            *reg->host.hprt0 |= USB_OTG_HPRT_PENA;
            len = 0;
            break;
        case USB_PORT_FEAT_RESET:
            if (*reg->host.hprt0 & USB_OTG_HPRT_PCSTS) {
                *reg->host.hprt0 |= USB_OTG_HPRT_PRST;
            }
            len = 0;
            break;
        case USB_PORT_FEAT_POWER:
            *reg->host.hprt0 |= USB_OTG_HPRT_PPWR;
            len = 0;
            break;
        default:
            goto error;
        }
        break;
    case ClearPortFeature:
        switch (wValue) {
        case USB_PORT_FEAT_ENABLE:
            *reg->host.hprt0 &= ~USB_OTG_HPRT_PENA;
            len = 0;
            break;
        case USB_PORT_FEAT_C_RESET:
            *reg->host.hprt0 &= ~USB_OTG_HPRT_PRST;
            len = 0;
            break;
        case USB_PORT_FEAT_C_CONNECTION:
            dwcotg_hcd->is_port_changed = false;
//            *reg->host.hprt0 |= USB_OTG_HPRT_PCDET;
            len = 0;
            break;
        case USB_PORT_FEAT_C_ENABLE:
            *reg->host.hprt0 &= ~USB_OTG_HPRT_PENCHNG;
            len = 0;
            break;
        case USB_PORT_FEAT_C_SUSPEND:
            *reg->host.hprt0 &= ~USB_OTG_HPRT_PSUSP;
            len = 0;
            break;
        case USB_PORT_FEAT_C_OVER_CURRENT:
            *reg->host.hprt0 &= ~USB_OTG_HPRT_POCA;
            len = 0;
            break;
        default:
            goto error;
        }
        break;
    case GetHubDescriptor:
        data[0] = 9;            // min length;
        data[1] = 0x29;
        data[2] = 1;
        data[3] = 0x8;
        datadw[1] = 0;
        data[5] = 0;
        data[8] = 0xff;
        len = min(data[0], wLength);
        break;
    default:
        goto error;
    }

    if (len) {
        if (urb->transfer_length < len) {
            len = urb->transfer_length;
        }
        urb->actual_length = len;
        memcpy(urb->buffer, data, len);
    }
    return len;

error:
    urb->status = URB_FAIL;
    return -1;
}

#endif
