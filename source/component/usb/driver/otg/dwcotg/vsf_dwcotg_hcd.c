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

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB
//  seems roothub is not stable now
#   define VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB           DISABLED
#endif

#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB == ENABLED
#   error VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB support is not ready now
#endif

// If high speed bulk in transaction alway NAK, it will take all the USB bandwidth.
//  Define VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF to hold off these transactions.
#ifndef VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF
#   define VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF    0
#endif
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 7
#   error VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF MUST be in range [0 .. 7]
#endif

#ifndef VSF_DWCOTG_HCD_CFG_TRACE_PORT
#   define VSF_DWCOTG_HCD_CFG_TRACE_PORT                DISABLED
#endif
#ifndef VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL
#   define VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL             DISABLED
#endif

#ifndef __VSF_DWCOTG_HCD_CFG_PORT_RESET_CHECK
#   define __VSF_DWCOTG_HCD_CFG_PORT_RESET_CHECK        DISABLED
#endif

// VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT:
// use port disable as disconnect event, some MCU will falsely detect device
//  disconnect event IN HIGH SPEED MODE, but port disable event is stable.

#define USB_OTG_HPRT_W1C_MASK                                                   \
        (USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET | USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
enum {
    VSF_DWCOTG_HCD_EVT_CONN = VSF_EVT_USER + 0,
    VSF_DWCOTG_HCD_EVT_DISC = VSF_EVT_USER + 1,
    VSF_DWCOTG_HCD_EVT_RST  = VSF_EVT_USER + 2,
};
#endif

enum {
    DWCOTG_HCD_DPIP_DATA0   = 0,
    DWCOTG_HCD_DPIP_DATA1   = 2,
    DWCOTG_HCD_DPIP_SETUP   = 3,
};

enum vk_dwcotg_hcd_urb_channel_phase_t {
    VSF_DWCOTG_HCD_PHASE_IDLE = 0,
    VSF_DWCOTG_HCD_PHASE_PERIOD,
    VSF_DWCOTG_HCD_PHASE_SETUP,
    VSF_DWCOTG_HCD_PHASE_DATA,
    VSF_DWCOTG_HCD_PHASE_STATUS,
    VSF_DWCOTG_HCD_PHASE_DONE,
} vk_dwcotg_hcd_urb_channel_phase_t;

typedef struct vk_dwcotg_hcd_urb_t {
    vsf_slist_node_t node;

    uint16_t channel_idx    : 4;
    uint16_t is_alloced     : 1;
    uint16_t is_discarded   : 1;
    uint16_t phase          : 3;
    uint16_t do_ping        : 1;
#if VSF_USBH_USE_HUB == ENABLED
    uint16_t is_split       : 1;
#endif
    uint16_t is_timeout_en  : 1;
    uint16_t is_timeout     : 1;
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
    uint16_t holdoff_cnt    : 3;
#endif

    uint16_t current_size;

    uint32_t timeout;

#ifdef VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE
    uint32_t buffer[VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE / sizeof(uint32_t)];
    void *orig_buffer;
#endif
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
    uint8_t is_reset_issued : 1;
    uint8_t is_reset_pending: 1;
#if VSF_KERNEL_CFG_TRACE == ENABLED
    uint8_t is_isr_info_sent: 1;
#endif
    uint16_t ep_mask;
    volatile uint32_t softick;

    vsf_teda_t task;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;
    vsf_slist_queue_t ready_queue;
    vsf_slist_queue_t pending_queue;

    vk_dwcotg_hcd_workaround_t *workaround;

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
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB == ENABLED
static int __vk_dwcotg_hcd_rh_control(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
#else
static vsf_err_t __vk_dwcotg_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __vk_dwcotg_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
#endif

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
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB == ENABLED
    .rh_control         = __vk_dwcotg_hcd_rh_control,
#else
    .reset_dev          = __vk_dwcotg_hcd_reset_dev,
    .is_dev_reset       = __vk_dwcotg_hcd_is_dev_reset,
#endif
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

#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
    vsf_trace_debug("dwcotg_hcd.channel%d: halted" VSF_TRACE_CFG_LINEEND, channel_idx);
#endif
    channel_regs->hcchar |= USB_OTG_HCCHAR_CHDIS;
    if (    (urb->pipe.type == USB_ENDPOINT_XFER_BULK)
        ||  (urb->pipe.type == USB_ENDPOINT_XFER_CONTROL)) {
        // in DMA mode, no need to check request queue
        if (!dwcotg_hcd->dma_en && dwcotg_hcd->reg.global_regs->gnptxsts & 0xFFFF) {
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_EPDIR;
            // skip delay
        } else {
            // DO NOT enable channel, set CHDIS in DMA mode is enough to halt a channel.
            //  If CHENA is set here, there is possibility that the channel is enabled after halt,
            //  especially when VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF is enabled.
//            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
        }
    } else {
        // in DMA mode, no need to check request queue
        if (!dwcotg_hcd->dma_en && dwcotg_hcd->reg.host.global_regs->hptxsts & 0xFFFF) {
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
            channel_regs->hcchar &= ~USB_OTG_HCCHAR_EPDIR;
            // skip delay
        } else {
            // DO NOT enable channel, set CHDIS in DMA mode is enough to halt a channel.
            //  If CHENA is set here, there is possibility that the channel is enabled after halt,
            //  especially when VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF is enabled.
//            channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
        }
    }
}

static void __vk_dwcotg_hcd_commit_urb(vk_dwcotg_hcd_t *dwcotg_hcd, vk_usbh_hcd_urb_t *urb,
                        uint_fast8_t dpid, bool dir_in1out0, uint8_t *buffer, uint_fast16_t size)
{
    static const uint8_t eptype_to_dwctype[4] = {
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
                            ((USB_SPEED_LOW == pipe.speed) << 17) |
                            ((uint32_t)eptype_to_dwctype[pipe.type] << 18) |
                            (pipe.size & USB_OTG_HCCHAR_MPSIZ);

    if (size > 0) {
        pkt_num = (size + pipe.size - 1) / pipe.size;
        VSF_USB_ASSERT(pkt_num <= 256);
    } else {
        pkt_num = 1;
    }

    // not secure to increase size, beacuae the buffer provided by user maybe not fit
//    if (dir_in1out0) {
//        size = pipe.size * pkt_num;
//    }

    dwcotg_urb->current_size = size;
    channel_regs->hctsiz = ((pkt_num << 19) & USB_OTG_HCTSIZ_PKTCNT) | ((uint32_t)dpid << 29) | size;

#if VSF_USBH_USE_HUB == ENABLED
    vk_usbh_dev_t *dev = (vk_usbh_dev_t *)urb->dev_hcd;
    vk_usbh_dev_t *dev_parent = dev->dev_parent;
    uint32_t hcsplt;
    if (    (dev_parent != NULL)
#   if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB == ENABLED
        // devnum 1 is roothub
        && (dev_parent->devnum != 1)
#   endif
        &&  (USB_SPEED_HIGH == dev_parent->speed) && (dev_parent->speed > dev->speed)) {
        dwcotg_urb->is_split = true;
        hcsplt = USB_OTG_HCSPLT_SPLITEN | (dev_parent->devnum << 7) | (dev->index + 1);
    } else {
        hcsplt = 0;
    }
    channel_regs->hcsplt = hcsplt;
#endif

    if (dwcotg_hcd->dma_en && (size > 0)) {
        bool is_addr_valid;
        if ((dwcotg_hcd->workaround != NULL) && (dwcotg_hcd->workaround->check_dma_addr != NULL)) {
            is_addr_valid = dwcotg_hcd->workaround->check_dma_addr(dwcotg_hcd->workaround->param, (uintptr_t)buffer);
        } else {
            is_addr_valid = true;
        }
#ifndef VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE
        VSF_USB_ASSERT(!((uintptr_t)buffer & 0x03));
        VSF_USB_ASSERT(is_addr_valid);
        channel_regs->hcdma = (uint32_t)buffer;
#else
        if (((uintptr_t)buffer & 0x03) || !is_addr_valid) {
            VSF_USB_ASSERT(size <= sizeof(dwcotg_urb->buffer));
            if (!pipe.dir_in1out0) {
                memcpy(&dwcotg_urb->buffer, buffer, size);
            }
            dwcotg_urb->orig_buffer = buffer;
            channel_regs->hcdma = (uint32_t)dwcotg_urb->buffer;
        } else {
            channel_regs->hcdma = (uint32_t)buffer;
        }
#endif
    }

    channel_regs->hcchar |= ((dwcotg_hcd->reg.host.global_regs->hfnum & 1) ^ 1) << 29;
    channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;

#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
    vsf_trace_debug("dwcotg_hcd.channel%d: submit for urb %p" VSF_TRACE_CFG_LINEEND,
                        dwcotg_urb->channel_idx, urb);
#endif

    channel_regs->hcintmsk = USB_OTG_HCINTMSK_CHHM | USB_OTG_HCINTMSK_AHBERR;
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
    if (pipe.dir_in1out0 && (USB_ENDPOINT_XFER_BULK == pipe.type) && (pipe.speed == USB_SPEED_HIGH)) {
        dwcotg_urb->holdoff_cnt = 0;
        channel_regs->hcintmsk |= USB_OTG_HCINTMSK_NAKM;
    }
#endif

    vsf_protect_t orig = vsf_protect_int();
        dwcotg_hcd->reg.host.global_regs->haintmsk |= 1 << dwcotg_urb->channel_idx;
    vsf_unprotect_int(orig);

    if (!dwcotg_hcd->dma_en && !dir_in1out0 && (dwcotg_urb->current_size > 0)) {
        // TODO: check FIFO space
        uint32_t *fifo_reg = (uint32_t *)dwcotg_hcd->reg.dfifo[dwcotg_urb->channel_idx];
        for (uint_fast16_t i = 0; i < size; i += 4, buffer += 4) {
            *fifo_reg = get_unaligned_le32(buffer);
        }
    }
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

static enum usb_device_speed_t __vk_dwcotg_hcd_get_port_speed(uint32_t hprt0)
{
    enum usb_device_speed_t speed = USB_SPEED_FULL;
    if ((hprt0 & USB_OTG_HPRT_PSPD) == 0) {
        speed = USB_SPEED_HIGH;
    } else if ((hprt0 & USB_OTG_HPRT_PSPD) == USB_OTG_HPRT_PSPD_1) {
        speed = USB_SPEED_LOW;
    }
    return speed;
}

static void __vk_dwcotg_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
    vk_dwcotg_hcd_t *dwcotg_hcd = container_of(eda, vk_dwcotg_hcd_t, task);
    vk_dwcotg_reg_t *reg = &dwcotg_hcd->reg;
    uint32_t hprt0 = *reg->host.hprt0 & ~USB_OTG_HPRT_W1C_MASK;
#endif

    switch (evt) {
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
    case VSF_EVT_INIT:
        *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PPWR;
        break;
#endif
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
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
    case VSF_EVT_TIMER:
        if (!(hprt0 & USB_OTG_HPRT_PCSTS)) {
            break;
        }
    __do_reset_port:
        if (dwcotg_hcd->is_reset_pending) {
            dwcotg_hcd->is_reset_pending = false;
            *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PRST;
            // flush fifo
            reg->global_regs->grstctl = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM_4 | USB_OTG_GRSTCTL_RXFFLSH;
            vsf_teda_set_timer_ms(20);
        } else if (hprt0 & USB_OTG_HPRT_PRST) {
            *reg->host.hprt0 &= ~(USB_OTG_HPRT_PRST | USB_OTG_HPRT_W1C_MASK);
            // 2021.10.16: seems that PLSTS in hprt0 maybe different to the actual signal
            if (hprt0 & USB_OTG_HPRT_PLSTS) {
                vsf_trace_warning("dwcotg_hcd: reset failed, retry" VSF_TRACE_CFG_LINEEND);
#if __VSF_DWCOTG_HCD_CFG_PORT_RESET_CHECK == ENABLED
                goto __do_reset_issue;
#endif
            }

            dwcotg_hcd->speed = __vk_dwcotg_hcd_get_port_speed(hprt0);
            uint_fast32_t delay_ms = 20;
            if ((dwcotg_hcd->workaround != NULL) && (dwcotg_hcd->workaround->enable_port != NULL)) {
                delay_ms = dwcotg_hcd->workaround->enable_port(dwcotg_hcd->workaround->param, dwcotg_hcd->speed);
            }
            vsf_teda_set_timer_ms(delay_ms);
        } else if (NULL == dwcotg_hcd->dev) {
            // cast-align from gcc
            dwcotg_hcd->dev = vk_usbh_new_device((vk_usbh_t *)dwcotg_hcd->hcd, dwcotg_hcd->speed, NULL, 0);
        } else {
            dwcotg_hcd->is_reset_issued = false;
        }
        break;
    case VSF_DWCOTG_HCD_EVT_CONN:
        if (dwcotg_hcd->is_connected || !(hprt0 & USB_OTG_HPRT_PCSTS)) {
            break;
        }
        dwcotg_hcd->is_connected = true;
        vsf_trace_debug("dwcotg_hcd: dev connected" VSF_TRACE_CFG_LINEEND);
        // fall through
    case VSF_DWCOTG_HCD_EVT_RST:
        if ((hprt0 & USB_OTG_HPRT_PRST) || !(hprt0 & USB_OTG_HPRT_PCSTS)) {
            break;
        }

#if __VSF_DWCOTG_HCD_CFG_PORT_RESET_CHECK == ENABLED
    __do_reset_issue:
#endif
        dwcotg_hcd->is_reset_pending = true;
        if ((dwcotg_hcd->workaround != NULL) && (dwcotg_hcd->workaround->reset_port != NULL)) {
            uint_fast32_t delay_ms = dwcotg_hcd->workaround->reset_port(dwcotg_hcd->workaround->param);
            if (delay_ms > 0) {
                vsf_teda_set_timer_ms(delay_ms);
                break;
            }
        }

        goto __do_reset_port;
    case VSF_DWCOTG_HCD_EVT_DISC:
        if (dwcotg_hcd->is_connected) {
            vk_usbh_dev_t *dev = dwcotg_hcd->dev;
            dwcotg_hcd->dev = NULL;
            dwcotg_hcd->is_connected = false;
            *reg->host.hprt0 &= ~(USB_OTG_HPRT_PRST | USB_OTG_HPRT_W1C_MASK);

            vsf_teda_cancel_timer();
            vsf_trace_debug("dwcotg_hcd: dev disconnected" VSF_TRACE_CFG_LINEEND);
            if (dev != NULL) {
                vk_usbh_disconnect_device((vk_usbh_t *)dwcotg_hcd->hcd, dev);

                reg->global_regs->gintmsk &= ~USB_OTG_GINTMSK_SOFM;

                // halt all channel
                for (uint_fast8_t i = 0; (dwcotg_hcd->ep_mask != 0) && (i < dwcotg_hcd->ep_num); i++) {
                    if (dwcotg_hcd->ep_mask & (1 << i)) {
                        VSF_USB_ASSERT(dwcotg_hcd->urb[i] != NULL);
                        vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&dwcotg_hcd->urb[i]->priv;
                        // mark for halted, checked in SOF handler
                        dwcotg_urb->is_timeout = true;
                        __vk_dwcotg_hcd_halt_channel(dwcotg_hcd, i);
                        // but afterwards, maybe OSF is not interrupted, how to clear CH_HALTED in rx queue?
                    }
                }

                // reset pending queue, urb in pending_queue SHOULD be already free by __vk_dwcotg_hcd_free_urb
                vsf_slist_queue_init(&dwcotg_hcd->pending_queue);

                // enable USB_OTG_GINTMSK_RXFLVLM to process CH_HALTED event in rx queue, will be disabled in VSF_DWCOTG_HCD_EVT_CONN
                reg->global_regs->gintmsk |= USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_RXFLVLM;
            }
        }
        break;
#endif
    }
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

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
            vk_dwcotg_hc_ip_info_t info = { 0 };
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
            dwcotg_hcd->workaround = info.workaround;

            {
                usb_hc_ip_cfg_t cfg = {
                    .priority       = param->priority,
                    .irqhandler     = __vk_dwcotg_hcd_interrupt,
                    .param          = dwcotg_hcd,
                };
                // interrupt is disabled after init
                param->op->Init(&cfg);
            }

            dwcotg_hcd->feature = info.feature;
            hcd->rh_speed = info.speed;
            dwcotg_hcd->ep_num = info.ep_num;
            dwcotg_hcd->ep_mask = ~((1 << info.ep_num) - 1);
            __vk_dwcotg_hcd_init_regs(dwcotg_hcd, info.regbase, dwcotg_hcd->ep_num);
            reg = &dwcotg_hcd->reg;

            if (dwcotg_hcd->ulpi_en) {
                // TODO: test ulpi support
//                reg->global_regs->gusbcfg |= USB_OTG_GUSBCFG_ULPIAR | USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
//                reg->global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_PHYIF | USB_OTG_GUSBCFG_TRDT);

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

            reg->global_regs->grstctl |= USB_OTG_GRSTCTL_CSRST;
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
            reg->global_regs->gintmsk |= USB_OTG_GINTMSK_HCIM | USB_OTG_GINTMSK_SOFM
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
                                        | USB_OTG_GINTMSK_PRTIM
#   if VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT != ENABLED
                                        | USB_OTG_GINTMSK_DISCINT
#   endif
#endif
                                        ;
            reg->global_regs->gahbcfg |= USB_OTG_GAHBCFG_GINT;
            dwcotg_hcd->state = DWCOTG_HCD_STATE_WORKING;

            dwcotg_hcd->task.fn.evthandler = __vk_dwcotg_hcd_evthandler;
            vsf_teda_init(&dwcotg_hcd->task);
#if VSF_KERNEL_CFG_TRACE == ENABLED
            vsf_kernel_trace_eda_info(&dwcotg_hcd->task.use_as__vsf_eda_t, "dwcotg_hcd_task", NULL, 0);
#endif
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
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
    vsf_protect_t orig = vsf_protect_int();
    if (    dwcotg_urb->is_alloced
        ||  vsf_slist_queue_is_in(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, dwcotg_urb)
        ||  vsf_slist_queue_is_in(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->ready_queue, dwcotg_urb)) {
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
    vk_dwcotg_reg_t *reg = &dwcotg_hcd->reg;
    vk_usbh_pipe_t pipe = urb->pipe;
    uint32_t interval = pipe.interval;
    if (!interval) {
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
        vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
        if (dwcotg_urb->holdoff_cnt) {
            dwcotg_urb->holdoff_cnt--;
            return false;
        } else {
            return true;
        }
#else
        return true;
#endif
    }

    if ((USB_SPEED_HIGH == dwcotg_hcd->speed) && (pipe.speed < USB_SPEED_HIGH)) {
        interval <<= 3;
    }

    // corrent me:
    // interrupt transaction will be queued at the next sof by hardware,
    //  so if interval is 1, issue hit instantly
    if ((USB_ENDPOINT_XFER_INT == pipe.type) && (1 == interval)) {
        return true;
    }

    if ((reg->host.global_regs->hfnum & 0xFFFF) == pipe.last_frame) {
        return false;
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
        index = vsf_ffz32(dwcotg_hcd->ep_mask);
        if ((index >= 0) && (index < 16)) {
            dwcotg_hcd->ep_mask |= 1 << index;
            dwcotg_urb->is_alloced = true;
#if VSF_USBH_USE_HUB == ENABLED
            dwcotg_urb->is_split = false;
#endif
            dwcotg_hcd->urb[index] = urb;
            dwcotg_urb->channel_idx = index;
            vsf_unprotect_int(orig);
        } else {
            vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->ready_queue, dwcotg_urb);
            vsf_unprotect_int(orig);
            return;
        }
#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
        vsf_trace_debug("dwcotg_hcd.channel%d: alloc for urb %p" VSF_TRACE_CFG_LINEEND, index, urb);
#endif
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
                dpid = pipe.toggle ? DWCOTG_HCD_DPIP_DATA1 : DWCOTG_HCD_DPIP_DATA0;
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
    struct dwcotg_hc_regs_t *channel_regs = &dwcotg_hcd->reg.host.hc_regs[channel_idx];
    uint_fast32_t channel_intsts = channel_regs->hcint;
    vk_usbh_hcd_urb_t *urb = dwcotg_hcd->urb[channel_idx];
    VSF_USB_ASSERT(urb != NULL);
    vk_dwcotg_hcd_urb_t *dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
#if VSF_USBH_USE_HUB == ENABLED
    bool is_split = dwcotg_urb->is_split;
#endif
    bool is_to_notify = false;

#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
    vsf_trace_debug("dwcotg_hcd.channel%d: interrupt %08X" VSF_TRACE_CFG_LINEEND, channel_idx, channel_intsts);
#endif
    if (channel_intsts & USB_OTG_HCINT_CHH) {
        vsf_protect_t orig = vsf_protect_int();
            dwcotg_hcd->reg.host.global_regs->haintmsk &= ~(1 << channel_idx);
        vsf_unprotect_int(orig);

        channel_regs->hcintmsk = 0;
        channel_regs->hcint = channel_regs->hcint;

        // is_discarded MUST be handled first, because discard operatoin will use timeout as mark
        if (dwcotg_urb->is_discarded) {
            goto free_channel;
        }
        if (dwcotg_urb->is_timeout) {
#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
            vsf_trace_error("dwcotg_hcd.channel%d: timeout" VSF_TRACE_CFG_LINEEND, channel_idx);
#endif
            urb->status = URB_TIMEOUT;
            goto urb_done;
        }

        channel_intsts &= ~USB_OTG_HCINT_CHH;
        // IMPORTANT: process error first, because maybe some other flags will be set at the same time and errors ignored
        if (channel_intsts & (USB_OTG_HCINT_TXERR | USB_OTG_HCINT_BBERR | USB_OTG_HCINT_DTERR)) {
            // data toggle error, babble error, usb bus error
            vsf_trace_error("dwcotg_hcd.channel%d: failed 0x%08X" VSF_TRACE_CFG_LINEEND, channel_idx, channel_intsts);
        urb_fail:
            urb->status = URB_FAIL;
        urb_done:
#ifdef VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE
            if (urb->pipe.dir_in1out0 && (dwcotg_urb->orig_buffer != NULL)) {
                memcpy(dwcotg_urb->orig_buffer, dwcotg_urb->buffer, urb->transfer_length);
                dwcotg_urb->orig_buffer = NULL;
            }
#endif
            urb->pipe.last_frame = dwcotg_hcd->reg.host.global_regs->hfnum & 0xFFFF;
            is_to_notify = true;
        } else if (channel_intsts & (USB_OTG_HCINT_XFRC | USB_OTG_HCINT_STALL)) {
            bool is_stall = channel_intsts & USB_OTG_HCINT_STALL;
            bool is_in = channel_regs->hcchar & USB_OTG_HCCHAR_EPDIR;

            if (VSF_DWCOTG_HCD_PHASE_DATA == dwcotg_urb->phase) {
                if (is_in) {
                    uint16_t remain_size = channel_regs->hctsiz & USB_OTG_HCTSIZ_XFRSIZ;
                    dwcotg_urb->current_size -= remain_size;
                    urb->actual_length += dwcotg_urb->current_size;
#if VSF_USBH_USE_HUB == ENABLED
                    if (is_split) {
                        dwcotg_urb->current_size = remain_size;
                    }
#endif
                } else {
                    urb->actual_length += dwcotg_urb->current_size;
                }
            }

#if VSF_USBH_USE_HUB == ENABLED
            if (    !is_stall && is_split
                &&  (channel_regs->hctsiz & USB_OTG_HCTSIZ_PKTCNT)) {
                channel_regs->hcsplt &= ~USB_OTG_HCSPLT_COMPLSPLT;
                goto do_split_next;
            }
#endif

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
                    break;
                }
                dwcotg_urb->phase++;
                if (dwcotg_urb->phase != VSF_DWCOTG_HCD_PHASE_DONE) {
                    __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
                    return;
                } else {
                urb_done_check_stall:
                    urb->status = is_stall ? URB_FAIL : URB_OK;
#if VSF_USBH_USE_HUB == ENABLED
                    if (is_split && !is_in) {
                        urb->actual_length = urb->transfer_length;
                    }
#endif
                    goto urb_done;
                }
                break;
            case USB_ENDPOINT_XFER_INT:
            case USB_ENDPOINT_XFER_BULK: {
                    int toggle = 0;
                    switch ((channel_regs->hctsiz >> 29) & 3) {
                    case 0: toggle = 0; break;  // DATA0
                    case 2: toggle = 1; break;  // DATA1
                    }
                    urb->pipe.toggle = toggle;
                }
                // fall through
            case USB_ENDPOINT_XFER_ISOC:
                goto urb_done_check_stall;
            }
        } else if (channel_intsts & USB_OTG_HCINT_NAK) {
#if VSF_USBH_USE_HUB == ENABLED
            if (is_split) {
                channel_regs->hcsplt &= ~USB_OTG_HCSPLT_COMPLSPLT;
                goto do_split_next;
            }
#endif
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
                goto __reactivate_channel;
            case USB_ENDPOINT_XFER_BULK:
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
                VSF_USB_ASSERT(urb->pipe.dir_in1out0);
                if (USB_SPEED_HIGH == urb->pipe.speed) {
                    channel_regs->hcchar |= USB_OTG_HCCHAR_EPDIR;
                    orig = vsf_protect_int();
                        vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, dwcotg_urb);
                    vsf_unprotect_int(orig);
                } else
#endif
                {
                __reactivate_channel:
                    channel_regs->hcchar &= ~USB_OTG_HCCHAR_CHDIS;
                    channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
                }
                return;
            }
        } else if (channel_intsts & (USB_OTG_HCINT_ACK | USB_OTG_HCINT_NYET)) {
#if VSF_USBH_USE_HUB == ENABLED
            if (is_split) {
                if (channel_intsts & USB_OTG_HCINT_ACK) {
                    channel_regs->hcsplt |= USB_OTG_HCSPLT_COMPLSPLT;
                } else {
                    switch (urb->pipe.type) {
                    case USB_ENDPOINT_XFER_CONTROL:
                    case USB_ENDPOINT_XFER_BULK:
                        // retry complete split
                        channel_regs->hcsplt |= USB_OTG_HCSPLT_COMPLSPLT;
                        break;
                    case USB_ENDPOINT_XFER_INT:
                        // TODO: add retry count
                        // for last transaction, retry start split
                        //  for non-last transaction, retry complete split
                        channel_regs->hcsplt &= ~USB_OTG_HCSPLT_COMPLSPLT;
                        break;
                    case USB_ENDPOINT_XFER_ISOC:
                        // TODO:
                        VSF_USB_ASSERT(false);
                    }
                }

            do_split_next:
                orig = vsf_protect_int();
                    vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, dwcotg_urb);
                vsf_unprotect_int(orig);
            }
#endif
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
            else if (dwcotg_urb->holdoff_cnt != 0) {
                channel_regs->hcintmsk = USB_OTG_HCINTMSK_CHHM | USB_OTG_HCINTMSK_AHBERR;
                vsf_protect_t orig = vsf_protect_int();
                    dwcotg_hcd->reg.host.global_regs->haintmsk |= 1 << channel_idx;
                vsf_unprotect_int(orig);

                goto __reactivate_channel;
            }
#endif
            return;
        } else if (channel_intsts & USB_OTG_HCINT_FRMOR) {
            // frame error
            // TODO: update err_cnt
            goto urb_retry;
        } else {
            // no idea why no event
#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
            vsf_trace_error("dwcotg_hcd.channel%d: no event" VSF_TRACE_CFG_LINEEND, channel_idx);
#endif
            // seems disconnect dievice while urb commited will maybe run here
//            VSF_USB_ASSERT(false);
            goto urb_fail;
        }

    free_channel: {
            vk_usbh_hcd_urb_t *urb_orig = urb;
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

#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
            vsf_trace_debug("dwcotg_hcd.channel%d: free" VSF_TRACE_CFG_LINEEND, channel_idx);
#endif
            if (is_discarded) {
                vsf_eda_post_msg((vsf_eda_t *)&dwcotg_hcd->task, urb_orig);
            } else if (is_to_notify) {
                vsf_eda_post_msg(urb_orig->eda_caller, urb_orig);
            }
            if (dwcotg_urb != NULL) {
                if (dwcotg_urb->is_discarded) {
                    goto free_channel;
                } else {
                    __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
                }
            }
        }
    } else if (channel_intsts & USB_OTG_HCINT_AHBERR) {
        // for ABH error, make sure vendor provide check_dma_addr workaround.
        //  set VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE to use buffer in urb
        vsf_trace_error("dwcotg_hcd.channel%d: ahb fatal error" VSF_TRACE_CFG_LINEEND, channel_idx);
        VSF_USB_ASSERT(false);
    } else {
#if VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF > 0
        channel_intsts &= channel_regs->hcintmsk;
        if (channel_intsts & USB_OTG_HCINT_NAK) {
            switch (urb->pipe.type) {
            case USB_ENDPOINT_XFER_BULK:
                VSF_USB_ASSERT(urb->pipe.dir_in1out0);
                channel_regs->hcintmsk &= ~USB_OTG_HCINTMSK_NAKM;
                dwcotg_urb->holdoff_cnt = VSF_DWCOTG_HCD_CFG_HS_BULK_IN_NAK_HOLDOFF;
                __vk_dwcotg_hcd_halt_channel(dwcotg_hcd, channel_idx);
                break;
            }
        }
#endif
    }
}

static void __vk_dwcotg_hcd_interrupt(void *param)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = param;
    uint_fast32_t intsts = dwcotg_hcd->reg.global_regs->gintmsk;
    volatile uint32_t *intsts_reg = &dwcotg_hcd->reg.global_regs->gintsts;
    intsts &= *intsts_reg;

#if VSF_KERNEL_CFG_TRACE == ENABLED
    if (!dwcotg_hcd->is_isr_info_sent) {
        dwcotg_hcd->is_isr_info_sent = true;
        vsf_kernel_trace_isr_info(vsf_get_interrupt_id(), "dwcotg_hcd");
    }
    if (intsts & (USB_OTG_GINTSTS_HCINT | USB_OTG_GINTSTS_HPRTINT)) {
        vsf_kernel_trace_isr_enter(vsf_get_interrupt_id());
    }
#endif

    if (!intsts) {
        // in some cases assertion will fail, should be safe to ignore
//        VSF_USB_ASSERT(false);
        return;
    }

    if (intsts & USB_OTG_GINTSTS_SOF) {
        vk_usbh_hcd_urb_t *urb;
        vk_dwcotg_hcd_urb_t *dwcotg_urb;

        *intsts_reg = USB_OTG_GINTSTS_SOF;
        dwcotg_hcd->softick++;

        // after device connecting to roothub is removed, there is possibility that the channel
        //  can not be halted, so check in SOF, if the channel need to be halted or timeouted
        for (uint_fast8_t i = 0; (dwcotg_hcd->ep_mask != 0) && (i < dwcotg_hcd->ep_num); i++) {
            if (dwcotg_hcd->ep_mask & (1 << i)) {
                urb = dwcotg_hcd->urb[i];
                VSF_USB_ASSERT(urb != NULL);
                dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;
                if (dwcotg_urb->is_timeout) {
                    // channel is still active after timeout, need to read GRXSTSP according to
                    //  <DesignWare Cores USB 2.0 Hi-Speed On-The-Go(OTG)> Section 3.5:
                    //  Host Programming Overview -- Halting a Channel
                    // The core generates a RxFLv1 interrupt when there is an entry in the queue.
                    // The application must read/pop the GRXSTSP register to generate the Channel Halted interrupt.

                    if (dwcotg_hcd->reg.global_regs->grxfsiz > 0) {
                        // read rx queue first, if pktsts is CH_HALTED, then pop it
                        uint32_t pktsts = (dwcotg_hcd->reg.global_regs->grxstsr & USB_OTG_GRXSTSP_PKTSTS) >> 17;
                        if (pktsts == 7) {      // RXSTAT_CH_HALTED
                            // just do a read
                            volatile uint32_t grxstsp = dwcotg_hcd->reg.global_regs->grxstsp;
                            VSF_UNUSED_PARAM(grxstsp);
                        }
                    }
                } else if (dwcotg_urb->is_discarded) {
#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
                    vsf_trace_debug("dwcotg_hcd.sof.channel%d: discard" VSF_TRACE_CFG_LINEEND, i);
#endif
                    __vk_dwcotg_hcd_halt_channel(dwcotg_hcd, i);
                    // use timeout to check grxstsp
                    dwcotg_urb->is_timeout = true;
                } else if (dwcotg_urb->is_timeout_en && (dwcotg_urb->timeout == dwcotg_hcd->softick)) {
                    dwcotg_urb->is_timeout_en = false;
                    dwcotg_urb->is_timeout = true;
#if VSF_DWCOTG_HCD_CFG_TRACE_CHANNEL == ENABLED
                    vsf_trace_debug("dwcotg_hcd.sof.channel%d: timeout" VSF_TRACE_CFG_LINEEND, i);
#endif
                    __vk_dwcotg_hcd_halt_channel(dwcotg_hcd, i);
                }
            }
        }

        vsf_slist_t queue;
        vsf_protect_t orig = vsf_protect_int();
            queue.head = dwcotg_hcd->pending_queue.head.next;
            vsf_slist_queue_init(&dwcotg_hcd->pending_queue);
        vsf_unprotect_int(orig);
        __vsf_slist_foreach_next_unsafe(vk_dwcotg_hcd_urb_t, node, &queue) {
            urb = container_of(_, vk_usbh_hcd_urb_t, priv);
            dwcotg_urb = (vk_dwcotg_hcd_urb_t *)&urb->priv;

            vsf_slist_init_node(vk_dwcotg_hcd_urb_t, node, _);
#if VSF_USBH_USE_HUB == ENABLED
            if (dwcotg_urb->is_split) {
                struct dwcotg_hc_regs_t *channel_regs = &dwcotg_hcd->reg.host.hc_regs[dwcotg_urb->channel_idx];
                channel_regs->hcchar |= USB_OTG_HCCHAR_CHENA;
                channel_regs->hcintmsk = USB_OTG_HCINTMSK_CHHM | USB_OTG_HCINTMSK_AHBERR;

                vsf_protect_t orig = vsf_protect_int();
                    dwcotg_hcd->reg.host.global_regs->haintmsk |= 1 << dwcotg_urb->channel_idx;
                vsf_unprotect_int(orig);
            } else
#endif
            if (__vk_dwcotg_hcd_is_period_hit(dwcotg_hcd, urb)) {
                if (USB_ENDPOINT_XFER_CONTROL == urb->pipe.type) {
                    _->phase = VSF_DWCOTG_HCD_PHASE_SETUP;
                } else {
                    _->phase = VSF_DWCOTG_HCD_PHASE_DATA;
                }
                __vk_dwcotg_hcd_urb_fsm(dwcotg_hcd, urb);
            } else {
                orig = vsf_protect_int();
                    vsf_slist_queue_enqueue(vk_dwcotg_hcd_urb_t, node, &dwcotg_hcd->pending_queue, _);
                vsf_unprotect_int(orig);
            }
        }
    }
    if (intsts & USB_OTG_GINTSTS_RXFLVL) {
        *intsts_reg = USB_OTG_GINTSTS_RXFLVL;
        while (dwcotg_hcd->reg.global_regs->grxfsiz > 0) {
            // just do a read
            volatile uint32_t grxstsp = dwcotg_hcd->reg.global_regs->grxstsp;
            VSF_UNUSED_PARAM(grxstsp);
        }
    }
    if (intsts & USB_OTG_GINTSTS_NPTXFE) {
        *intsts_reg = USB_OTG_GINTSTS_NPTXFE;
    }
#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB != ENABLED
    if (intsts & USB_OTG_GINTSTS_HPRTINT) {
        uint32_t hprt0 = *dwcotg_hcd->reg.host.hprt0;
        uint32_t hprt0_masked = hprt0 & ~USB_OTG_HPRT_W1C_MASK;
#   if VSF_DWCOTG_HCD_CFG_TRACE_PORT == ENABLED
        vsf_trace_debug("dwcotg_hcd.hprt_isr: %08X" VSF_TRACE_CFG_LINEEND, hprt0);
#   endif
        if (hprt0 & USB_OTG_HPRT_PCDET) {
            *dwcotg_hcd->reg.host.hprt0 = hprt0_masked | USB_OTG_HPRT_PCDET;
            // disable rx queue interrupt which is used to clear CH_HALTED while device disconnected
            dwcotg_hcd->reg.global_regs->gintmsk &= ~USB_OTG_GINTMSK_RXFLVLM;
#   if VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT == ENABLED
            // if disconnect is not enabled, below timing will not be supported, so simply send a disconnect event first
            //  PCDET       -- connected
            //  DISCONNECT  -- disconnect, not detected, because port is not disabled
            //  PCDET       -- connected again, because previous disconnect event is missing, current connect event will not be processed
            vsf_eda_post_evt((vsf_eda_t *)&dwcotg_hcd->task, VSF_DWCOTG_HCD_EVT_DISC);
#   endif
            vsf_eda_post_evt((vsf_eda_t *)&dwcotg_hcd->task, VSF_DWCOTG_HCD_EVT_CONN);
        }
        if (hprt0 & USB_OTG_HPRT_PENCHNG) {
            *dwcotg_hcd->reg.host.hprt0 = hprt0_masked | USB_OTG_HPRT_PENCHNG;
#   if VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT == ENABLED
            if (!(hprt0 & USB_OTG_HPRT_PENA)) {
                vsf_eda_post_evt((vsf_eda_t *)&dwcotg_hcd->task, VSF_DWCOTG_HCD_EVT_DISC);
            }
#   endif
        }
        if (hprt0 & USB_OTG_HPRT_POCCHNG) {
            *dwcotg_hcd->reg.host.hprt0 = hprt0_masked | USB_OTG_HPRT_POCCHNG;
        }
        *intsts_reg = USB_OTG_GINTSTS_HPRTINT;
    }
#   if VSF_DWCOTG_HCD_WORKAROUND_PORT_DISABLE_AS_DISCONNECT != ENABLED
    if (intsts & USB_OTG_GINTSTS_DISCINT) {
        *intsts_reg = USB_OTG_GINTSTS_DISCINT;
        vsf_eda_post_evt((vsf_eda_t *)&dwcotg_hcd->task, VSF_DWCOTG_HCD_EVT_DISC);
    }
#   endif
#endif
    if (intsts & USB_OTG_GINTSTS_PTXFE) {
        *intsts_reg = USB_OTG_GINTSTS_PTXFE;
    }

    if (intsts & USB_OTG_GINTSTS_HCINT) {
        *intsts_reg = USB_OTG_GINTSTS_HCINT;

        uint_fast32_t haint = dwcotg_hcd->reg.host.global_regs->haint;
        haint &= dwcotg_hcd->reg.host.global_regs->haintmsk;

#if VSF_DWCOTG_HCD_CFG_TRACE_PORT == ENABLED
        vsf_trace_debug("dwcotg_hcd.channel_isr: hprt %08X" VSF_TRACE_CFG_LINEEND, *dwcotg_hcd->reg.host.hprt0);
#endif
        for (uint_fast8_t i = 0; i < dwcotg_hcd->ep_num; i++) {
            if (haint & (1UL << i)) {
                __vk_dwcotg_hcd_channel_interrupt(dwcotg_hcd, i);
            }
        }
    }

#if VSF_KERNEL_CFG_TRACE == ENABLED
    if (intsts & (USB_OTG_GINTSTS_HCINT | USB_OTG_GINTSTS_HPRTINT)) {
        vsf_kernel_trace_isr_leave(vsf_get_interrupt_id());
    }
#endif
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
    if (urb->timeout > 0) {
        dwcotg_urb->is_timeout_en = true;
        if (dwcotg_hcd->speed == USB_SPEED_HIGH) {
            dwcotg_urb->timeout = (urb->timeout << 3) + dwcotg_hcd->softick;
        } else {
            dwcotg_urb->timeout = urb->timeout + dwcotg_hcd->softick;
        }
    }
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

#if VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB == ENABLED
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
    uint32_t hprt0 = *reg->host.hprt0 & ~USB_OTG_HPRT_W1C_MASK;

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
            // port enabled is set by hw after reset
//            *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PENA;
            len = 0;
            break;
        case USB_PORT_FEAT_RESET:
            if (*reg->host.hprt0 & USB_OTG_HPRT_PCSTS) {
                *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PRST;
            }
            len = 0;
            break;
        case USB_PORT_FEAT_POWER:
            *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PPWR;
            len = 0;
            break;
        default:
            goto error;
        }
        break;
    case ClearPortFeature:
        switch (wValue) {
        case USB_PORT_FEAT_ENABLE:
            *reg->host.hprt0 |= hprt0 | USB_OTG_HPRT_PENA;
            len = 0;
            break;
        case USB_PORT_FEAT_C_RESET:
            *reg->host.hprt0 |= hprt0 | USB_OTG_HPRT_PRST;
            len = 0;
            break;
        case USB_PORT_FEAT_C_CONNECTION:
            dwcotg_hcd->is_port_changed = false;
//            *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PCDET;
            len = 0;
            break;
        case USB_PORT_FEAT_C_ENABLE:
            *reg->host.hprt0 = hprt0 | USB_OTG_HPRT_PENCHNG;
            len = 0;
            break;
        case USB_PORT_FEAT_C_SUSPEND:
            // suspend is cleared after remote wakeup
//            *reg->host.hprt0 = hprt0 & ~USB_OTG_HPRT_PSUSP;
            len = 0;
            break;
        case USB_PORT_FEAT_C_OVER_CURRENT:
            *reg->host.hprt0 |= hprt0 | USB_OTG_HPRT_POCCHNG;
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
        len = vsf_min(data[0], wLength);
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
#else       // VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB
static vsf_err_t __vk_dwcotg_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
//    vk_dwcotg_reg_t *reg = &dwcotg_hcd->reg;
//    uint32_t hprt0 = *reg->host.hprt0 & ~USB_OTG_HPRT_W1C_MASK;

    if (dwcotg_hcd->is_connected) {
        dwcotg_hcd->is_reset_issued = true;
        vsf_eda_post_evt((vsf_eda_t *)&dwcotg_hcd->task, VSF_DWCOTG_HCD_EVT_RST);
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

static bool __vk_dwcotg_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_dwcotg_hcd_t *dwcotg_hcd = hcd->priv;
    return !!dwcotg_hcd->is_reset_issued;
}
#endif      // VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB

#endif
