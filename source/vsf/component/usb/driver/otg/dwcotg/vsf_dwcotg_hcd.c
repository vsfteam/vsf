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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_DWCOTG == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_DWCOTG_HCD_CLASS_IMPLEMENT

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

typedef struct vk_dwcotg_hcd_urb_t {
    int8_t idx;

    uint8_t phase           : 3;
    uint8_t state           : 3;

    uint8_t toggle_start    : 1;
    uint8_t toggle_next     : 1;
    uint8_t do_ping         : 1;
    uint8_t discarded       : 1;

    uint16_t current_size;
} vk_dwcotg_hcd_urb_t;

typedef struct vk_dwcotg_hcd_t {
    implement(vk_dwcotg_t)
    enum {
        DWCOTG_HCD_STATE_WAIT_ABH_IDLE,
        DWCOTG_HCD_STATE_WAIT_RESET,
        DWCOTG_HCD_STATE_WAIT_ENABLE,
        DWCOTG_HCD_STATE_WAIT_FLUSH_TXFIFO,
        DWCOTG_HCD_STATE_WAIT_FLUSH_RXFIFO,
        DWCOTG_HCD_STATE_WORKING,
    } state;

    uint16_t ep_mask;
    vsf_teda_t teda;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;
    implement(vk_dwcotg_param_t)

    vk_dwcotg_hcd_urb_t *urb[0];
} vk_dwcotg_hcd_t;

/*============================ PROTOTYPES ====================================*/

extern void vsf_dwcotg_hcd_get_fifo_size(vk_usbh_hcd_t *hcd,
        uint16_t *rx_fifo_size, uint16_t *non_periodic_tx_fifo_size, uint16_t *periodic_tx_fifo_size);

static vsf_err_t __vk_dwcotg_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_resume(vk_usbh_hcd_t *hcd);
static vk_usbh_hcd_urb_t * __vk_dwcotg_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_dwcotg_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_dwcotg_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_dwcotg_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static int __vk_dwcotg_hcd_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_dwcotg_hcd_drv = {
    .init_evthandler    = __vk_dwcotg_hcd_init_evthandler,
    .fini               = __vk_dwcotg_hcd_fini,
    .suspend            = __vk_dwcotg_hcd_suspend,
    .resume             = __vk_dwcotg_hcd_resume,
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

    for (uint_fast8_t i = 0; i < ep_num; i++) {
        dwcotg_hcd->reg.dfifo[i] = (void *)((uint8_t *)regbase + (i + 1) * 0x1000);
    }
}

static vsf_err_t __vk_dwcotg_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_dwcotg_hcd_t *dwcotg_hcd;
    vk_dwcotg_hcd_param_t *param;
    vk_dwcotg_reg_t *reg;

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
            info.ep_num >>= 1£»

            dwcotg_hcd = hcd->priv = vsf_usbh_malloc(sizeof(*dwcotg_hcd) + info.ep_num * sizeof(vk_dwcotg_hcd_urb_t *));
            if (NULL == dwcotg_hcd) {
                VSF_USB_ASSERT(false);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }
            memset(dwcotg_hcd, 0, sizeof(*dwcotg_hcd));
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
            dwcotg_hcd->ep_num = info.ep_num >> 1;
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
            if (dwcotg_hcd->speed == USB_SPEED_HIGH) {
                reg->host.global_regs->hcfg &= ~USB_OTG_HCFG_FSLSS;
            } else {
                reg->host.global_regs->hcfg |= USB_OTG_HCFG_FSLSS;
            }

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
            reg->global_regs->gintmsk |= USB_OTG_GINTMSK_HCIM | USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_PXFRM_IISOOXFRM;
            reg->global_regs->gahbcfg |= USB_OTG_GAHBCFG_GINT;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WORKING;
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
    vsf_usbh_free(urb);
}

static vsf_err_t __vk_dwcotg_hcd_commit_urb(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb,
                        uint_fast8_t dpid, bool dir_in1out0, uint8_t *buffer, uint_fast16_t size)
{
    const uint8_t eptype_to_dwctype[4] = {
        [USB_ENDPOINT_XFER_CONTROL] = 1,
        [USB_ENDPOINT_XFER_ISOC]    = 3,
        [USB_ENDPOINT_XFER_BULK]    = 0,
        [USB_ENDPOINT_XFER_INT]     = 2
    };

    vk_dwcotg_hcd_urb_t *urb_priv = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    struct dwcotg_hc_regs_t *hc_regs = &dwcotg_hcd->reg.host.hc_regs[urb_priv->idx];
    vk_usbh_pipe_t pipe = urb->pipe;
    uint_fast32_t tmp;

    switch (pipe.type) {
    case USB_ENDPOINT_XFER_ISOC:
        hc_regs->hcintmsk = USB_OTG_HCINTMSK_XFRCM | USB_OTG_HCINTMSK_ACKM |
                            USB_OTG_HCINTMSK_AHBERR | USB_OTG_HCINTMSK_FRMORM ;
        if (dir_in1out0) {
            hc_regs->hcintmsk |= USB_OTG_HCINTMSK_TXERRM;
        }
        break;
    case USB_ENDPOINT_XFER_INT:
        hc_regs->hcintmsk = USB_OTG_HCINTMSK_XFRCM | USB_OTG_HCINTMSK_STALLM |
                            USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_DTERRM|
                            USB_OTG_HCINTMSK_NAKM | USB_OTG_HCINTMSK_AHBERR |
                            USB_OTG_HCINTMSK_FRMORM;
        break;
    case USB_ENDPOINT_XFER_CONTROL:
    case USB_ENDPOINT_XFER_BULK:
        hc_regs->hcintmsk = USB_OTG_HCINTMSK_XFRCM  | USB_OTG_HCINTMSK_STALLM |
                            USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_DTERRM |
                            USB_OTG_HCINTMSK_AHBERR | USB_OTG_HCINTMSK_NAKM;
        if (!dir_in1out0) {
            hc_regs->hcintmsk |= USB_OTG_HCINTMSK_NYET;
        }
        break;
    }
    if (hc->dir_o0_i1) {
        reg->hcintmsk |= USB_OTG_HCINTMSK_BBERRM;
    }

    dwcotg_hcd->reg.host.global_regs->haintmsk |= 1 << urb_priv->idx;
    hc_regs->hcchar =   (pipe.address << 22) | (pipe.endpoint << 11) |
                        (dir_in1out0 ? USB_OTG_HCCHAR_EPDIR : 0) |
                        ((pipe.speed == USB_SPEED_LOW) << 17) |
                        ((uint32_t)eptype_to_dwctype[pipe.type] << 18) |
                        (pipe.size & USB_OTG_HCCHAR_MPSIZ);
    if (urb_priv->type == USB_ENDPOINT_XFER_INT) {
        reg->hcchar |= USB_OTG_HCCHAR_ODDFRM;
    }

    if (size > 0) {
        tmp = (size + pipe.size - 1) / pipe.size;
        if (tmp > 256) {
            tmp = 256;
            size = pipe.size * 256;
        }
    } else {
        tmp = 1;
    }
    if (dir_in1out0) {
        size = pipe.size * tmp;
    }
    urb_priv->current_size = size;
    hc_regs->hctsiz = ((tmp << 19) & USB_OTG_HCTSIZ_PKTCNT) | ((uint32_t)dpid << 29) | size;
    urb_priv->toggle_next = urb_priv->toggle_start ^ (tmp & 1);

    if (dwcotg_hcd->dma_en) {
        hc_regs->hcdma = (uint32_t)buffer;
    }

    hc_regs->hcchar |= (dwcotg_hcd->reg.host.global_regs->hfnum & 1) << 29;
    hc_regs->hcchar = (hc_regs->hcchar & ~USB_OTG_HCCHAR_CHDIS) | USB_OTG_HCCHAR_CHENA;

    if (!dwcotg_hcd->dma_en && !dir_in1out0 && (urb_priv->current_size > 0)) {
        
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_dwcotg_hcd_urb_handler(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_urb_t *urb_priv = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    vk_usbh_pipe_t pipe = urb->pipe;

    switch (urb_priv->phase) {
    case VSF_DWCOTG_HCD_PHASE_PERIOD: {
            uint_fast32_t interval = urb->interval;
            if (USB_SPEED_HIGH == dwcotg_hcd->speed) {
                interval <<= 3;
            }
            if (!(dwcotg_hcd->softtick & interval)) {
                urb_priv->state = VSF_DWCOTG_HCD_STATE_START;
                urb_priv->phase = VSF_DWCOTG_HCD_PHASE_DATA;
                goto data_phase;
            } else {
                urb_priv->state = VSF_DWCOTG_HCD_STATE_WAIT;
            }
        }
        break;
    case VSF_DWCOTG_HCD_PHASE_SETUP:
        return __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, DWCOTG_HCD_DPIP_SETUP, 0, &urb->setup_packet, sizeof(urb->setup_packet));
    case VSF_DWCOTG_HCD_PHASE_DATA:
    data_phase: {
            uint_fast8_t dpid;
            switch (pipe.type) {
            case USB_ENDPOINT_XFER_CONTROL:
                dpid = DWCOTG_HCD_DPIP_DATA1;
                break;
            case USB_ENDPOINT_XFER_ISO:
                dpid = DWCOTG_HCD_DPIP_DATA0;
                break;
            case USB_ENDPOINT_XFER_BULK:
            case USB_ENDPOINT_XFER_INT:
                dpid = urb_priv->toggle_start ? DWCOTG_HCD_DPIP_DATA1 : DWCOTG_HCD_DPIP_DATA0;
                break;
            }
            return __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, dpid, pipe.dir_in1out0, urb_priv->transfer_buffer, urb_priv->transfer_length);
        }
    case VSF_DWCOTG_HCD_PHASE_STATE:
        return __vk_dwcotg_hcd_commit_urb(dwcotg_hcd, urb, HC_DPID_DATA1, NULL, 0);
    default:
        VSF_USB_ASSERT(false);
        return VSF_ERR_FAIL;
    }
}

static void __vk_dwcotg_hcd_interrupt(void *param)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = param;
    uint_fast32_t intsts = dwcotg_hcd->reg.global_regs->gintmsk;
    volatile uint32_t *intsts_reg = &dwcotg_hcd->reg.global_regs->gintsts;
    intsts &= *intsts_reg;

    if (intsts != 0) {
        if (intsts & USB_OTG_GINTSTS_SOF) {
            *intsts_reg = USB_OTG_GINTSTS_SOF;
            dwcotg_hcd->softtick++;
            // TODO: commit urb in wait state
        }
        if (intsts & USB_OTG_GINTSTS_RXFLVL) {
        }
        if (intsts & USB_OTG_GINTSTS_NPTXFE) {
        }
        if (intsts & USB_OTG_GINTSTS_HPRTINT) {
        }
        if (intsts & USB_OTG_GINTSTS_PTXFE) {
        }
        if (intsts & USB_OTG_GINTSTS_HCINT) {
            *intsts_reg = USB_OTG_GINTSTS_HCINT;

            uint_fast32_t haint = dwcotg_hcd->reg.host.global_regs->haint;
            vk_dwcotg_hcd_urb_t *dwcotg_urb;
            struct dwcotg_hc_regs_t *hc_regs;
            vk_usbh_urb_t *urb;
            uint_fast32_t hc_intsts;

            for (uint_fast8_t i = 0; i < dwcotg_hcd->ep_num; i++) {
                if (haint & (1UL << i)) {
                    dwcotg_urb = dwcotg_hcd->urb[i];
                    urb = container_of(dwcotg_urb, vk_usbh_urb_t, priv);
                    hc_regs = &dwcotg_hcd->reg.host.hc_regs[i];
                    hc_intsts = hc_regs->hcintmsk;
                    hc_intsts &= hc_regs->hcint;

                    if (hc_intsts & USB_OTG_HCINT_AHBERR) {
                        hc_regs->hcint = USB_OTG_HCINT_AHBERR;
                        hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                    } else if (hc_intsts & USB_OTG_HCINT_STALL) {
                        dwcotg_urb->state = DWCOTG_HCD_URB_STATE_STALL;
                        hc_regs->hcint = USB_OTG_HCINT_NAK | USB_OTG_HCINT_STALL;
                        hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                        __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                    } else if (hc_intsts & USB_OTG_HCINT_DTERR) {
                        dwcotg_urb->state = DWCOTG_HCD_URB_STATE_DTERR;
                        hc_regS->hcint = USB_OTG_HCINT_NAK | USB_OTG_HCINT_DTERR;
                        hc_regS->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                        __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                    } else if (hc_intsts & USB_OTG_HCINT_FRMOR) {
                        hc_regs->hcint = USB_OTG_HCINT_FRMOR;
                        hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                        __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                    } else if (hc_intsts & USB_OTG_HCINT_TXERR) {
                        dwcotg_urb->state = DWCOTG_HCD_URB_STATE_XACTERR;
                        hc_regs->hcint = USB_OTG_HCINT_TXERR;
                        hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                        __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                    } else if (hc_intsts & USB_OTG_HCINT_ACK) {
                        hc_regs->hcint = USB_OTG_HCINT_ACK;

                        if (dwcotg_urb->do_ping) {
                            dwcotg_urb->state = DWCOTG_HCD_URB_STATE_NYET;
                            hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                            __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                            // TODO
                            dwcotg_urb->state = URB_PRIV_STATE_NOTREADY;
                        }
                    }

                    if (hc_regs->hcchar & USB_OTG_HCCHAR_EPDIR) {
                        if (hc_intsts & USB_OTG_HCINT_XFRC) {
                            if (dwcotg_hcd->dma_en) {
                                urb->actual_length = dwcotg_urb->transfer_size - (hc_regs->hctsiz & USB_OTG_HCTSIZ_XFRSIZ);
                            }

                            dwcotg_urb->state = DWCOTG_HCD_URB_STATE_XFRC;
                            hc_regs->hcint = USB_OTG_HCINT_XFRC;
                            hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;

                            switch (urb->pipe.type) {
                            case USB_ENDPOINT_XFER_CONTROL:
                            case USB_ENDPOINT_XFER_BULK:
                                __vk_dwcotg_hcd_hc_halt(dwcotg_hcd, i);
                                hc_regs->hcint = USB_OTG_HCINT_NAK;
                                break;
                            case USB_ENDPOINT_XFER_INT:
                                hc_regs->hcchar |= USB_OTG_HCCHAR_ODDFRM;
                                urb_priv->state = URB_PRIV_STATE_DONE;
                                break;
                            }
                        } else if (hc_intsts & USB_OTG_HCINT_NAK) {
                            dwcotg_urb->state = HC_NAK;
                            hc_regs->hcint = USB_OTG_HCINT_NAK;

                            switch (urb->pipe.type) {
                            case USB_ENDPOINT_XFER_CONTROL:
                            case USB_ENDPOINT_XFER_BULK:
                                hc_regs->hcchar &= ~USB_OTG_HCCHAR_CHDIS;
                                hc_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
                                break;
                            case USB_ENDPOINT_XFER_INT:
                                hc_regs->hcintmsk |= USB_OTG_HCINTMSK_CHHM;
                                __vk_dwcotg_hcd_hc_halt(otgh, hc_num);
                                break;
                            }
                        }

                        hc_intsts = hc_regs->hcintmsk;
                        hc_intsts &= hc_regs->hcint;

                        if (hc_intsts & USB_OTG_HCINT_CHH) {
                            hc_regs->hcintmsk &= ~USB_OTG_HCINTMSK_CHHM;
                            hc_regs->hcint = USB_OTG_HCINT_CHH;

                            switch (hc->hc_state) {
                            case HC_XFRC:
                                break;
                            }
                        }
                    } else {
                        
                    }
                }
            }
        }
    }
}

static vsf_err_t __vk_dwcotg_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    vk_usbh_dev_t *dev = dwcotg_hcd->dev;
    vk_usbh_pipe_t pipe = urb->pipe;
    vk_dwcotg_hcd_urb_t *urb_priv = (vk_dwcotg_hcd_urb_t *)&urb->priv;

    memset(urb_priv, 0, sizeof(*urb_priv));
    switch (pipe.type) {
    case USB_ENDPOINT_XFER_CONTROL:
        urb_priv->phase = VSF_DWCOTG_HCD_PHASE_SETUP;
        break;
    case USB_ENDPOINT_XFER_BULK:
        urb_priv->phase = VSF_DWCOTG_HCD_PHASE_DATA;
        // fall through
    case USB_ENDPOINT_XFER_INT:
        urb_priv->toggle_start = usb_gettoggle(urb->dev_hcd, pipe.endpoint, !pipe.dir_in1out0);
        urb_priv->do_ping = dwcotg_hcd->dma_en ? 0 : (pipe.speed == USB_SPEED_HIGH);
        break;
    default:
        urb_priv->phase = VSF_DWCOTG_HCD_PHASE_PERIOD;
        break;
    }

    return __vk_dwcotg_hcd_urb_handler(dwcotg_hcd, urb);
}

static vsf_err_t __vk_dwcotg_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_dwcotg_hcd_urb_handler(hcd->priv, urb);
}

static uint_fast32_t __vk_dwcotg_hcd_rh_portstat(uint_fast32_t hprt0)
{
    uint_fast32_t value = 0;

    if ((hprt0 & USB_OTG_HPRT_PSPD) == 0) {
        value |= USB_PORT_STAT_HIGH_SPEED;
    } else if ((hprt0 & USB_OTG_HPRT_PSPD) == USB_OTG_HPRT_PSPD_1) {
        value |= USB_PORT_STAT_LOW_SPEED;
    }

    if (hprt0 & USB_OTG_HPRT_PCDET) {
        value |= (USB_PORT_STAT_C_CONNECTION << 16);
    }

    if (hprt0 & USB_OTG_HPRT_PENA) {
        value |= USB_PORT_STAT_ENABLE;
    }

    if (hprt0 & USB_OTG_HPRT_PCSTS) {
        value |= USB_PORT_STAT_CONNECTION;
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
        datadw[0] = __vk_dwcotg_hcd_rh_portstat((uint_fast32_t)*reg->host.hprt0);
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
            *reg->host.hprt0 |= USB_OTG_HPRT_PCDET;
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
