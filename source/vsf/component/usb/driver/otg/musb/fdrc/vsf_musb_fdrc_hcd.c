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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_MUSB_FDRC == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
// musbfdrc has no roothub, so need VSF_USBH_IMPLEMENT_HUB for
//  vsf_usbh_new_device and vsf_usbh_disconnect_device
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__

#include "./vsf_musb_fdrc_hcd.h"
#include "./__vsf_musb_fdrc_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vk_musb_fdrc_hcd_t {
    vk_musb_fdrc_reg_t *reg;
    enum vk_musb_fdrc_hcd_state_t {
        MUSB_FDRC_HCD_STATE_WAIT_HOSTMODE,
        MUSB_FDRC_HCD_STATE_WAIE_CONNECT,
        MUSB_FDRC_HCD_STATE_WAIT_RESET,
        MUSB_FDRC_HCD_STATE_WAIT_RESET_CLEAR,
        MUSB_FDRC_HCD_STATE_CONNECTED,
    } state;
    uint8_t epidx_cur;
    uint8_t errcode_cur;

    // ep_in_mask and ep_out_mask is used to mask the ep_idx used
    uint16_t ep_in_mask;
    uint16_t ep_out_mask;

    vsf_teda_t teda;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;
    vk_usbh_hcd_urb_t *urb_cur;

    // singleton device
    union {
        struct {
            // ep_out to ep_idx
            int8_t ep_out_idx[16];
            // ep_in to ep_idx
            int8_t ep_in_idx[16];
        };
        int8_t ep_idx[32];
    } dev_priv;
} vk_musb_fdrc_hcd_t;

typedef struct vk_musb_fdrc_urb_t {
    vsf_dlist_node_t urb_node;
    uint8_t epidx;
    enum vk_musb_fdrc_urb_state_t {
        MUSB_FDRC_URB_STATE_IDLE,
        MUSB_FDRC_URB_STATE_START_SUBMITTING,
        MUSB_FDRC_URB_STATE_SUBMITTING,
        MUSB_FDRC_URB_STATE_TO_FREE,
    } state;
    enum vk_musb_fdrc_ep0_state_t {
        MUSB_FDRC_USBH_EP0_IDLE,
        MUSB_FDRC_USBH_EP0_SETUP,
        MUSB_FDRC_USBH_EP0_DATA,
        MUSB_FDRC_USBH_EP0_STATUS,
    } ep0_state;
    uint16_t cur_size;
} vk_musb_fdrc_urb_t;

typedef enum vk_musb_fdrc_hcd_evt_t {
    VSF_MUSB_FDRC_HCD_EVT_EP        = VSF_EVT_USER + 0,
    VSF_MUSB_FDRC_HCD_EVT_CONN      = VSF_EVT_USER + 1,
    VSF_MUSB_FDRC_HCD_EVT_DISCONN   = VSF_EVT_USER + 2,
} vk_musb_fdrc_hcd_evt_t;

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_musb_fdrc_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_musb_fdrc_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_musb_fdrc_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __vk_musb_fdrc_hcd_resume(vk_usbh_hcd_t *hcd);
static void __vk_musb_fdrc_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __vk_musb_fdrc_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __vk_musb_fdrc_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_musb_fdrc_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __vk_musb_fdrc_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vk_musb_fdrc_hcd_drv = {
    .init_evthandler    = __vk_musb_fdrc_hcd_init_evthandler,
    .fini               = __vk_musb_fdrc_hcd_fini,
    .suspend            = __vk_musb_fdrc_hcd_suspend,
    .resume             = __vk_musb_fdrc_hcd_resume,
    .free_device        = __vk_musb_fdrc_hcd_free_device,
    .alloc_urb          = __vk_musb_fdrc_hcd_alloc_urb,
    .free_urb           = __vk_musb_fdrc_hcd_free_urb,
    .submit_urb         = __vk_musb_fdrc_hcd_submit_urb,
    .relink_urb         = __vk_musb_fdrc_hcd_relink_urb,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_musb_fdrc_hcd_free_ep(vk_musb_fdrc_hcd_t *musb, uint_fast8_t idx, bool is_in)
{
    VSF_USB_ASSERT(idx != 0);
    vk_musb_fdrc_reg_t *reg = musb->reg;
    vsf_protect_t orig = vsf_protect_sched();
        if (is_in) {
            vk_musb_fdrc_clear_mask(&reg->Common.IntrRx1E, idx);
            musb->ep_in_mask &= ~(1 << idx);
        } else {
            vk_musb_fdrc_clear_mask(&reg->Common.IntrTx1E, idx);
            musb->ep_out_mask &= ~(1 << idx);
        }
    vsf_unprotect_sched(orig);
}

static void __vk_musb_fdrc_hcd_interrupt(void *param)
{
    vk_musb_fdrc_hcd_t *musb = param;
    vk_musb_fdrc_reg_t *reg = musb->reg;

    uint_fast32_t status = reg->Common.IntrUSB;
    status &= reg->Common.IntrUSBE;

    if (status & MUSB_INTRUSB_RESUME) {
    }
    if (status & MUSB_INTRUSB_SESSREQ) {
    }
    if (status & MUSB_INTRUSB_VBUSERROR) {
    }
    if (status * MUSB_INTRUSB_SUSPEND) {
    }
    if (status & MUSB_INTRUSB_CONN) {
        vsf_eda_post_evt(&musb->teda.use_as__vsf_eda_t, VSF_MUSB_FDRC_HCD_EVT_CONN);
    }
    if (status & MUSB_INTRUSB_DISCON) {
        vsf_eda_post_evt(&musb->teda.use_as__vsf_eda_t, VSF_MUSB_FDRC_HCD_EVT_DISCONN);
    }
    if (status & MUSB_INTRUSB_RESET) {
        // Babble for host
    }
    if (status & MUSB_INTRUSB_SOF) {
    }

    // EP interrupt
    // lower 16-bit is ep_out, higher 16-bit is ep_in
    status =    (vk_musb_fdrc_get_mask(&reg->Common.IntrTx1) << 0)
            |   (vk_musb_fdrc_get_mask(&reg->Common.IntrRx1) << 16);
    status &=   (vk_musb_fdrc_get_mask(&reg->Common.IntrTx1E) << 0)
            |   (vk_musb_fdrc_get_mask(&reg->Common.IntrRx1E) << 16);

    if (status > 0) {
        uint_fast8_t epidx = musb->epidx_cur & 0x0F;
        bool is_in = (musb->epidx_cur & 0x80) > 0;
        uint_fast32_t mask = (1 << epidx) << (is_in ? 16 : 0);

        VSF_USB_ASSERT(status == mask);
        if (0 == epidx) {
            musb->errcode_cur = reg->EP0.CSR0 & MUSBH_CSR0_ERRMASK;
            reg->EP0.CSR0 &= ~MUSBH_CSR0_ERRMASK;
        } else if (is_in) {
            musb->errcode_cur = reg->EPN.RxCSR1 & MUSBH_RXCSR1_ERRMASK;
            reg->EPN.RxCSR1 &= ~MUSBH_RXCSR1_ERRMASK;
        } else {
            musb->errcode_cur = reg->EPN.TxCSR1 & MUSBH_TXCSR1_ERRMASK;
            reg->EPN.TxCSR1 &= ~MUSBH_TXCSR1_ERRMASK;
        }
        vsf_eda_post_evt(&musb->teda.use_as__vsf_eda_t, VSF_MUSB_FDRC_HCD_EVT_EP);
    }
}

static void __vk_musb_fdrc_hcd_free_urb_do(vk_musb_fdrc_hcd_t *musb, vk_usbh_hcd_urb_t *urb)
{
    vsf_usbh_free(urb);
}

// TODO: verify ZLP indicated by urb->transfer_flags with URB_ZERO_PACKET
static vsf_err_t __vk_musb_fdrc_hcd_urb_fsm(vk_musb_fdrc_hcd_t *musb)
{
    vk_musb_fdrc_reg_t *reg = musb->reg;
    vk_usbh_hcd_urb_t *urb = musb->urb_cur;
    vk_musb_fdrc_urb_t *musb_urb = (vk_musb_fdrc_urb_t *)urb->priv;

    vk_usbh_pipe_t pipe = urb->pipe;
    bool is_iso = pipe.type == USB_ENDPOINT_XFER_ISOC;
    bool is_in = pipe.dir_in1out0 > 0;
    uint_fast16_t epsize = pipe.size + (is_iso ? 1 : 0);
    uint8_t *buffer = urb->buffer;

    switch (musb_urb->state) {
    case MUSB_FDRC_URB_STATE_START_SUBMITTING:
        musb_urb->state = MUSB_FDRC_URB_STATE_SUBMITTING;

        urb->actual_length = 0;
        urb->status = VSF_ERR_NONE;
        VSF_USB_ASSERT(reg->Common.FAddr == pipe.address);
        vk_musb_fdrc_set_ep(reg, musb_urb->epidx);
        musb->epidx_cur = musb_urb->epidx;

        if (0 == pipe.endpoint) {
            musb_urb->ep0_state = MUSB_FDRC_USBH_EP0_SETUP;
            vk_musb_fdrc_write_fifo(reg, 0, (uint8_t *)&urb->setup_packet, sizeof(urb->setup_packet));
            reg->EP0.CSR0 &= ~(MUSBH_CSR0_SETUPPKT | MUSBH_CSR0_STATUSPKT);
            reg->EP0.CSR0 |= MUSBH_CSR0_TXPKTRDY | MUSBH_CSR0_SETUPPKT;
            break;
        } else {
            if (is_in) {
                musb->epidx_cur |= 0x80;

                reg->EPN.RxType = (pipe.type << 4) | pipe.endpoint;
                reg->EPN.RxMAXP = (epsize + 7) >> 3;
                if (is_iso) {
                    reg->EPN.RxInterval = 1;
                } else {
                    reg->EPN.RxInterval = 0;
                }

                // FIFO start from 128 with 1024 size
                reg->EPN.RxFIFO1 = 128 >> 3;
                reg->EPN.RxFIFO2 = 3 << 5;
                reg->EPN.RxCSR1 |= MUSBH_RXCSR1_FLUSHFIFO;
            } else {
                reg->EPN.TxType = (pipe.type << 4) | pipe.endpoint;
                reg->EPN.TxMAXP = (epsize + 7) >> 3;
                if (is_iso) {
                    reg->EPN.TxInterval = 1;
                } else {
                    reg->EPN.TxInterval = 0;
                }

                // FIFO start from 128 with 1024 size
                reg->EPN.TxFIFO1 = 128 >> 3;
                reg->EPN.TxFIFO2 = 3 << 5;
            }
            goto do_tx_rx;
        }
        // fall through
    case MUSB_FDRC_URB_STATE_SUBMITTING:
        if (0 == pipe.endpoint) {
            switch (musb_urb->ep0_state) {
            case MUSB_FDRC_USBH_EP0_SETUP:
                musb_urb->ep0_state = MUSB_FDRC_USBH_EP0_DATA;
                goto do_ep0_data_stage;
            case MUSB_FDRC_USBH_EP0_DATA:
                if ((musb_urb->cur_size > 0) && is_in) {
                    musb_urb->cur_size = vk_musb_fdrc_rx_fifo_size(reg, 0);
                    if (musb_urb->cur_size > 0) {
                        vk_musb_fdrc_read_fifo(reg, 0, &buffer[urb->actual_length], musb_urb->cur_size);
                        reg->EP0.CSR0 &= ~MUSBH_CSR0_RXPKTRDY;
                    }
                }
                urb->actual_length += musb_urb->cur_size;
                if (is_in && (musb_urb->cur_size < epsize)) {
                    // short packet
                    goto status_stage;
                }

            do_ep0_data_stage:
                musb_urb->cur_size = min(epsize, (urb->transfer_length - urb->actual_length));

                if (urb->actual_length >= urb->transfer_length) {
                status_stage:
                    musb_urb->ep0_state = MUSB_FDRC_USBH_EP0_STATUS;
                    reg->EP0.CSR0 &= ~(MUSBH_CSR0_SETUPPKT | MUSBH_CSR0_STATUSPKT);
                    if (is_in) {
                        reg->EP0.CSR0 |= MUSBH_CSR0_TXPKTRDY | MUSBH_CSR0_STATUSPKT;
                    } else {
                        reg->EP0.CSR0 |= MUSBH_CSR0_REQPKT | MUSBH_CSR0_STATUSPKT;
                    }
                } else {
                    if (is_in) {
                        reg->EP0.CSR0 |= MUSBH_CSR0_REQPKT;
                    } else {
                        vk_musb_fdrc_write_fifo(reg, 0, &buffer[urb->actual_length], musb_urb->cur_size);
                        reg->EP0.CSR0 |= MUSBH_CSR0_TXPKTRDY;
                    }
                }
                break;
            case MUSB_FDRC_USBH_EP0_STATUS:
                goto urb_finished;
            }
        } else {
            if ((musb_urb->cur_size > 0) && is_in) {
                musb_urb->cur_size = vk_musb_fdrc_rx_fifo_size(reg, musb_urb->epidx);
                if (musb_urb->cur_size > 0) {
                    vk_musb_fdrc_read_fifo(reg, musb_urb->epidx, &buffer[urb->actual_length], musb_urb->cur_size);
                    reg->EPN.RxCSR1 &= ~MUSBH_RXCSR1_RXPKTRDY;
                }
            }
            urb->actual_length += musb_urb->cur_size;
            if (is_in) {
                if (musb_urb->cur_size < epsize) {
                    // short packet
                    goto urb_finished;
                }
            }

            if (urb->actual_length >= urb->transfer_length) {
                goto urb_finished;
            } else {
            do_tx_rx:
                musb_urb->cur_size = min(epsize, (urb->transfer_length - urb->actual_length));
                if (is_in) {
                    reg->EPN.RxCSR1 |= MUSBH_RXCSR1_REQPKT;
                } else {
                    vk_musb_fdrc_write_fifo(reg, musb_urb->epidx, &buffer[urb->actual_length], musb_urb->cur_size);
                    reg->EPN.TxCSR1 |= MUSBH_TXCSR1_TXPKTRDY;
                }
            }
        }
        break;
    }
    return VSF_ERR_NOT_READY;

urb_finished:
    // for set address request, set FAddr
    if ((0 == pipe.endpoint) && (0 == urb->setup_packet.bRequestType) && (0x05 == urb->setup_packet.bRequest)) {
        reg->Common.FAddr = urb->setup_packet.wValue & 0xFF;
    }
    musb_urb->state = MUSB_FDRC_URB_STATE_IDLE;
    vsf_eda_post_msg(urb->eda_caller, urb);
    musb->urb_cur = NULL;
    return VSF_ERR_NONE;
}

static void __vk_musb_fdrc_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_musb_fdrc_hcd_t *musb = container_of(eda, vk_musb_fdrc_hcd_t, teda);
    vk_musb_fdrc_reg_t *reg = musb->reg;

    switch (evt) {
    case VSF_EVT_MESSAGE: {
            vk_usbh_hcd_urb_t *urb = vsf_eda_get_cur_msg();
            VSF_USB_ASSERT(urb != NULL);
            vk_musb_fdrc_urb_t *musb_urb = (vk_musb_fdrc_urb_t *)urb->priv;

            musb_urb->state = MUSB_FDRC_URB_STATE_START_SUBMITTING;
            musb->urb_cur = urb;
            musb->errcode_cur = 0;
        }
        // fall through
    case VSF_MUSB_FDRC_HCD_EVT_EP: {
            vk_usbh_hcd_urb_t *urb = musb->urb_cur;
            vk_musb_fdrc_urb_t *musb_urb = (vk_musb_fdrc_urb_t *)urb->priv;

            if (MUSB_FDRC_URB_STATE_TO_FREE == musb_urb->state) {
                __vk_musb_fdrc_hcd_free_urb_do(musb, urb);
            } else if (MUSB_FDRC_HCD_STATE_CONNECTED == musb->state) {
                if (musb->errcode_cur != 0) {
                    goto urb_fail;
                }

                vsf_err_t err = __vk_musb_fdrc_hcd_urb_fsm(musb);
                if (err < 0) {
                    goto urb_fail;
                }
            } else {
            urb_fail:
                musb_urb->state = MUSB_FDRC_URB_STATE_IDLE;
                urb->status = VSF_ERR_FAIL;
                vsf_eda_post_msg(urb->eda_caller, urb);
                musb->urb_cur = NULL;
            }
        }
        break;
    case VSF_MUSB_FDRC_HCD_EVT_CONN:
        if (MUSB_FDRC_HCD_STATE_WAIE_CONNECT == musb->state) {
            reg->Common.FAddr = 0;
            reg->Common.Power |= MUSB_POWER_RESET;
            vsf_teda_set_timer_ms(100);
            musb->state = MUSB_FDRC_HCD_STATE_WAIT_RESET;
        }
        break;
    case VSF_MUSB_FDRC_HCD_EVT_DISCONN:
        if (MUSB_FDRC_HCD_STATE_CONNECTED == musb->state) {
            vk_usbh_disconnect_device((vk_usbh_t *)musb->hcd, musb->dev);
            reg->Common.FAddr = 0;
            musb->dev = NULL;
            musb->state = MUSB_FDRC_HCD_STATE_WAIE_CONNECT;
        }
        reg->Common.IntrUSBE = MUSB_INTRUSBE_CONN;
        break;
    case VSF_EVT_TIMER:
        if (MUSB_FDRC_HCD_STATE_WAIT_RESET == musb->state) {
            reg->Common.Power &= ~MUSB_POWER_RESET;
            vsf_teda_set_timer_ms(100);
            musb->state = MUSB_FDRC_HCD_STATE_WAIT_RESET_CLEAR;
        } else if (MUSB_FDRC_HCD_STATE_WAIT_RESET_CLEAR == musb->state) {
            enum usb_device_speed_t speed;
            if (reg->Common.DevCtl & MUSB_DEVCTL_LSDEV) {
                speed = USB_SPEED_LOW;
            } else if (reg->Common.DevCtl & MUSB_DEVCTL_FSDEV) {
                speed = USB_SPEED_FULL;
            } else {
                VSF_USB_ASSERT(false);
                speed = USB_SPEED_UNKNOWN;
            }

            musb->state = MUSB_FDRC_HCD_STATE_CONNECTED;
            musb->dev = vk_usbh_new_device((vk_usbh_t *)musb->hcd, speed, NULL, 0);
            reg->Common.IntrUSBE = MUSB_INTRUSBE_DISCON;
        }
        break;
    }
}

static vsf_err_t __vk_musb_fdrc_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    vk_musb_fdrc_hcd_t *musb;
    vk_musb_fdrc_hcd_param_t *param;
    vk_musb_fdrc_reg_t *reg;

    VSF_USB_ASSERT(hcd != NULL);
    if (hcd->priv != NULL) {
        musb = hcd->priv;
        reg = musb->reg;
    }
    param = hcd->param;

    switch (evt) {
    case VSF_EVT_INIT:
        hcd->rh_speed = USB_SPEED_FULL;
        musb = hcd->priv = vsf_usbh_malloc(sizeof(*musb));
        if (NULL == musb) {
            VSF_USB_ASSERT(false);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
        memset(musb, 0, sizeof(*musb));
        musb->hcd = hcd;

        // initialize all members in musb->dev
        memset(musb->dev_priv.ep_idx, -1, sizeof(musb->dev_priv.ep_idx));
        // IN0 and OUT0 uses shared ep0
        musb->dev_priv.ep_in_idx[0] = 0;
        musb->dev_priv.ep_out_idx[0] = 0;

        {
            usb_hc_ip_cfg_t cfg = {
                .priority       = param->priority,
                .irq_handler    = __vk_musb_fdrc_hcd_interrupt,
                .param          = musb,
            };
            param->op->Init(&cfg);
        }
        {
            usb_hc_ip_info_t info;
            param->op->GetInfo(&info);
            reg = musb->reg = info.regbase;
            // shared ep0 is reserved for control transfer
            musb->ep_out_mask = musb->ep_in_mask = (0xFFFF & ~((1 << info.ep_num) - 1)) | 1;
        }

        // POWER MUST be cleared in param->op->Init
//        reg->Common.Power = 0;
        vk_musb_fdrc_interrupt_init(reg);
        reg->Common.IntrUSBE = MUSB_INTRUSBE_CONN;
        reg->Common.IntrTx1E = 1 << 0;      // enable shared ep0 interrupt
        reg->Common.DevCtl = MUSB_DEVCTL_SESSION;
        musb->state = MUSB_FDRC_HCD_STATE_WAIT_HOSTMODE;
        // fall through
    case VSF_EVT_TIMER:
        VSF_USB_ASSERT(MUSB_FDRC_HCD_STATE_WAIT_HOSTMODE == musb->state);
        if (!(reg->Common.DevCtl & MUSB_DEVCTL_HOSTMODE)) {
            vsf_teda_set_timer_ms(1);
        } else {
            musb->state = MUSB_FDRC_HCD_STATE_WAIE_CONNECT;
            musb->teda.fn.evthandler = __vk_musb_fdrc_hcd_evthandler;
            vsf_teda_init(&musb->teda, vsf_prio_inherit, false);
            return VSF_ERR_NONE;
        }
        break;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __vk_musb_fdrc_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_musb_fdrc_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_musb_fdrc_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static void __vk_musb_fdrc_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    vk_musb_fdrc_hcd_t *musb = hcd->priv;

    // DO NOT free ep0, which is shared by all devices
    // ep resources can only be freed here,
    //  because hardware maintain the data toggle status
    for (int i = 1; i < dimof(musb->dev_priv.ep_in_idx); i++) {
        if (musb->dev_priv.ep_in_idx[i] > 0) {
            __vk_musb_fdrc_hcd_free_ep(musb, i, true);
        }
    }
    for (int i = 1; i < dimof(musb->dev_priv.ep_out_idx); i++) {
        if (musb->dev_priv.ep_out_idx[i] > 0) {
            __vk_musb_fdrc_hcd_free_ep(musb, i, false);
        }
    }
}

static vk_usbh_hcd_urb_t * __vk_musb_fdrc_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    vk_usbh_hcd_urb_t *urb;
    uint_fast32_t size;

    VSF_USB_ASSERT(hcd != NULL);

    size = sizeof(vk_usbh_hcd_urb_t) + sizeof(vk_musb_fdrc_urb_t);
    urb = vsf_usbh_malloc(size);
    if (urb != NULL) {
        memset(urb, 0, size);
    }
    return urb;
}

static void __vk_musb_fdrc_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_musb_fdrc_hcd_t *musb = hcd->priv;
    vk_musb_fdrc_urb_t *musb_urb = (vk_musb_fdrc_urb_t *)urb->priv;

    if (MUSB_FDRC_URB_STATE_IDLE != musb_urb->state) {
        musb_urb->state = MUSB_FDRC_URB_STATE_TO_FREE;
    } else {
        __vk_musb_fdrc_hcd_free_urb_do(musb, urb);
    }
}

static vsf_err_t __vk_musb_fdrc_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_musb_fdrc_hcd_t *musb = hcd->priv;
    return vsf_eda_post_msg(&musb->teda.use_as__vsf_eda_t, urb);
}

static vsf_err_t __vk_musb_fdrc_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_musb_fdrc_hcd_t *musb = hcd->priv;
    vk_musb_fdrc_urb_t *musb_urb = (vk_musb_fdrc_urb_t *)urb->priv;

    // allocate ep resources
    vk_usbh_pipe_t pipe = urb->pipe;
    int8_t *ep_idx_arr;
    uint16_t *ep_mask;
    uint8_t *ep_inten;

    if (pipe.dir_in1out0) {
        ep_idx_arr = musb->dev_priv.ep_in_idx;
        ep_mask = &musb->ep_in_mask;
        ep_inten = (uint8_t *)&musb->reg->Common.IntrRx1E;
    } else {
        ep_idx_arr = musb->dev_priv.ep_out_idx;
        ep_mask = &musb->ep_out_mask;
        ep_inten = (uint8_t *)&musb->reg->Common.IntrTx1E;
    }

    if (ep_idx_arr[pipe.endpoint] < 0) {
        bool alloced = false;
        // allocate new ep
        vsf_protect_t orig = vsf_protect_sched();
            int idx = ffz(*ep_mask);
            if (idx < 16) {
                *ep_mask |= 1 << idx;
                ep_idx_arr[pipe.endpoint] = idx;
                vk_musb_fdrc_set_mask(ep_inten, idx);
                alloced = true;
            }
        vsf_unprotect_sched(orig);
        if (!alloced) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
    }
    musb_urb->epidx = ep_idx_arr[pipe.endpoint];

    return __vk_musb_fdrc_hcd_relink_urb(hcd, urb);
}

#endif
