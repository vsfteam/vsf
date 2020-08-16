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

    vsf_teda_t teda;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;
    implement(vk_dwcotg_param_t)
} vk_dwcotg_hcd_t;

typedef struct vk_dwcotg_hcd_urb_t {
    uint8_t type    : 2;
    uint8_t phase   : 3;
    uint8_t state   : 3;

    uint8_t toggle_start    : 1;
} vk_dwcotg_hcd_urb_t;

typedef struct vk_dwcotg_hcd_dev_t {
    uint8_t hc_num;
    vk_usbh_hcd_dev_t *dev;
} vk_dwcotg_hcd_dev_t;

/*============================ PROTOTYPES ====================================*/

extern void vsf_dwcotg_hcd_get_fifo_size(vk_usbh_hcd_t *hcd,
        uint16_t *rx_fifo_size, uint16_t *non_periodic_tx_fifo_size, uint16_t *periodic_tx_fifo_size);

static vsf_err_t __vk_dwcotg_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_resume(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_dwcotg_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __vk_dwcotg_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
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
    .alloc_device       = __vk_dwcotg_hcd_alloc_device,
    .free_device        = __vk_dwcotg_hcd_free_device,
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

static void __vk_dwcotg_hcd_interrupt(void *param)
{
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
    case VSF_EVT_INIT:
        dwcotg_hcd = hcd->priv = vsf_usbh_malloc(sizeof(*dwcotg_hcd));
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
        {
            vk_dwcotg_hc_ip_info_t info;
            param->op->GetInfo(&info.use_as__usb_hc_ip_info_t);
            dwcotg_hcd->dma_en = info.dma_en;
            hcd->rh_speed = dwcotg_hcd->speed = info.speed;
            dwcotg_hcd->ep_num = info.ep_num >> 1;
            __vk_dwcotg_hcd_init_regs(dwcotg_hcd, info.regbase, dwcotg_hcd->ep_num);
            reg = &dwcotg_hcd->reg;
        }

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

static vsf_err_t __vk_dwcotg_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_dwcotg_hcd_dev_t *dwcotg_hcd_dev = vsf_usbh_malloc(sizeof(vk_dwcotg_hcd_dev_t));
    if (dwcotg_hcd_dev != NULL) {
        memset(dwcotg_hcd_dev, 0, sizeof(vk_dwcotg_hcd_dev_t));
        dev->dev_priv = dwcotg_hcd_dev;
        dwcotg_hcd_dev->dev = dev;
        return VSF_ERR_NONE;
    }
    return VSF_ERR_NOT_ENOUGH_RESOURCES;
}

static void __vk_dwcotg_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_dwcotg_hcd_dev_t *dwcotg_hcd_dev = dev->dev_priv;
    if (dwcotg_hcd_dev != NULL) {
        vsf_usbh_free(dwcotg_hcd_dev);
        dev->dev_priv = NULL;
    }
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

static vsf_err_t __vk_dwcotg_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_dwcotg_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __vk_dwcotg_hcd_submit_urb(hcd, urb);
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
