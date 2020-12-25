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

#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_USB_HOST == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_HCD__
// f1cx00s_usbh_hcd has no roothub, so need __VSF_USBH_CLASS_IMPLEMENT_HUB__ for
//  vsf_usbh_new_device and vsf_usbh_disconnect_device
#define __VSF_USBH_CLASS_IMPLEMENT_HUB__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define CH32F10X_usbh_CLASS_IMPLEMENT
#include "./usbh.h"

#include "../usb.h"

/*============================ MACROS ========================================*/

#define F1CX00S_USBH_TRACE_EN               DISABLED
#define F1CX00S_USBH_TRACE_BUFFER_EN        DISABLED

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct f1cx00s_usbh_hcd_t {
    f1cx00s_usb_otg_t *otg;

    enum {
        HCD_STATE_WAIT_HOSTMODE,
        HCD_STATE_WAIT_CONNECT,
        HCD_STATE_WAIT_RESET,
        HCD_STATE_WAIT_RESET_CLEAR,
        HCD_STATE_CONNECTED,
    } state;
    uint16_t unchained_ep_in_mask;

    vsf_teda_t teda;
    vk_usbh_hcd_t *hcd;
    vk_usbh_dev_t *dev;

    vsf_dlist_t urb_list;
    vsf_sem_t sem;
    // EP0 - EP1: chained urb, shared, urb_cur[0]
    // EP2 - EPn: unchained urb, urb_cur[n - 1]
    vk_usbh_hcd_urb_t *urb_cur[3];
} f1cx00s_usbh_hcd_t;

typedef struct f1cx00s_usbh_hcd_urb_t {
    vsf_dlist_node_t urb_node;
    uint8_t epidx;
    enum {
        URB_STATE_IDLE,
        URB_STATE_QUEUED,
        URB_STATE_START_SUBMITTING,
        URB_STATE_SUBMITTING,
        URB_STATE_TO_FREE,
    } state;
    enum {
        EP0_STATE_IDLE,
        EP0_STATE_SETUP,
        EP0_STATE_DATA,
        EP0_STATE_STATUS,
    } ep0_state;
    uint16_t cur_size;
} f1cx00s_usbh_hcd_urb_t;

typedef struct f1cx00s_usbh_hcd_dev_t {
    union {
        struct {
            // ep_out to ep_idx
            int8_t ep_out_idx[16];
            // ep_in to ep_idx
            int8_t ep_in_idx[16];
        };
        int8_t ep_idx[32];
    };
    uint16_t toggle[2];
} f1cx00s_usbh_hcd_dev_t;

enum {
    HCD_EVT_EP          = VSF_EVT_USER + 0x000,
    HCD_EVT_CONN        = VSF_EVT_USER + 0x100,
    HCD_EVT_DISCONN     = VSF_EVT_USER + 0x200,
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __f1cx00s_usbh_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd);
static vsf_err_t __f1cx00s_usbh_hcd_fini(vk_usbh_hcd_t *hcd);
static vsf_err_t __f1cx00s_usbh_hcd_suspend(vk_usbh_hcd_t *hcd);
static vsf_err_t __f1cx00s_usbh_hcd_resume(vk_usbh_hcd_t *hcd);
static uint_fast16_t __f1cx00s_usbh_hcd_get_frame_number(vk_usbh_hcd_t *hcd);
static vsf_err_t __f1cx00s_usbh_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __f1cx00s_usbh_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static vk_usbh_hcd_urb_t * __f1cx00s_usbh_hcd_alloc_urb(vk_usbh_hcd_t *hcd);
static void __f1cx00s_usbh_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __f1cx00s_usbh_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __f1cx00s_usbh_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb);
static vsf_err_t __f1cx00s_usbh_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static bool __f1cx00s_usbh_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev);
static void __f1cx00s_usbh_hcd_isr(f1cx00s_usbh_hcd_t *musb_hcd);

extern vsf_err_t __f1cx00s_usb_init(f1cx00s_usb_otg_t *usb, vsf_arch_prio_t priority);
extern void __f1cx00s_usb_clear_interrupt(f1cx00s_usb_otg_t *usb);
extern void __f1cx00s_usb_register_irq(f1cx00s_usb_otg_t *usb, void (*irqhandler)(void *), void *param);
extern uint_fast16_t __f1cx00s_usb_rxfifo_size(f1cx00s_usb_otg_t *usb, uint_fast8_t ep);
extern void __f1cx00s_usb_read_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern void __f1cx00s_usb_write_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size);
extern uint_fast8_t __f1cx00s_usb_set_ep(f1cx00s_usb_otg_t *usb, uint_fast8_t ep);
extern uint_fast8_t __f1cx00s_usb_get_ep_num(f1cx00s_usb_otg_t *usb);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_hcd_drv_t vsf_usb_hcd_drv = {
    .init_evthandler    = __f1cx00s_usbh_hcd_init_evthandler,
    .fini               = __f1cx00s_usbh_hcd_fini,
    .suspend            = __f1cx00s_usbh_hcd_suspend,
    .resume             = __f1cx00s_usbh_hcd_resume,
    .get_frame_number   = __f1cx00s_usbh_hcd_get_frame_number,
    .alloc_device       = __f1cx00s_usbh_hcd_alloc_device,
    .free_device        = __f1cx00s_usbh_hcd_free_device,
    .alloc_urb          = __f1cx00s_usbh_hcd_alloc_urb,
    .free_urb           = __f1cx00s_usbh_hcd_free_urb,
    .submit_urb         = __f1cx00s_usbh_hcd_submit_urb,
    .relink_urb         = __f1cx00s_usbh_hcd_relink_urb,

    .reset_dev          = __f1cx00s_usbh_hcd_reset_dev,
    .is_dev_reset       = __f1cx00s_usbh_hcd_is_dev_reset,
};

/*============================ LOCAL VARIABLES ===============================*/

// USB_HC0 is usb host controller hardware instance, so use upper-case
//  use singleton here to avoid allocation on heap
static NO_INIT f1cx00s_usbh_hcd_t USB_HC0;

/*============================ IMPLEMENTATION ================================*/

static void __f1cx00s_usbh_hcd_free_ep(f1cx00s_usbh_hcd_t *musb_hcd, uint_fast8_t idx, bool is_in)
{
    VSF_OSA_HAL_ASSERT(idx != 0);
    vsf_protect_t orig = vsf_protect_sched();
        if (is_in) {
            MUSB_BASE->Common.IntrRxE &= ~(1 << idx);
        } else {
            MUSB_BASE->Common.IntrTxE &= ~(1 << idx);
        }
    vsf_unprotect_sched(orig);
}

static void __f1cx00s_usbh_hcd_free_urb_imp(f1cx00s_usbh_hcd_t *musb_hcd, vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_hcd_urb_free_buffer(urb);
    vsf_usbh_free(urb);
}

static bool __f1cx00s_usbh_hcd_urb_for_queue(vk_usbh_hcd_urb_t *urb)
{
    vk_usbh_pipe_t pipe = urb->pipe;
    return (USB_ENDPOINT_XFER_CONTROL == pipe.type) || (USB_ENDPOINT_XFER_BULK == pipe.type);
}

static vk_usbh_hcd_urb_t * __f1cx00s_usbh_hcd_get_urb(f1cx00s_usbh_hcd_t *musb_hcd, uint_fast8_t ep_idx)
{
    ep_idx &= 0x0F;
    return musb_hcd->urb_cur[ep_idx < 2 ? 0 : ep_idx - 1];
}

static vsf_err_t __f1cx00s_usbh_hcd_enqueue_urb(f1cx00s_usbh_hcd_t *musb_hcd, f1cx00s_usbh_hcd_urb_t *musb_urb)
{
    vsf_dlist_init_node(f1cx00s_usbh_hcd_urb_t, urb_node, musb_urb);
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(f1cx00s_usbh_hcd_urb_t, urb_node, &musb_hcd->urb_list, musb_urb);
        musb_urb->state = URB_STATE_QUEUED;
    vsf_unprotect_sched(orig);
    vsf_eda_sem_post(&musb_hcd->sem);
    return VSF_ERR_NONE;
}

static uint_fast8_t __f1cx00s_usbh_hcd_get_speed(vk_usbh_pipe_t pipe)
{
    static const uint8_t speed_map[4] = {
        [USB_SPEED_LOW]     = 3,
        [USB_SPEED_FULL]    = 2,
        [USB_SPEED_HIGH]    = 1,
    };
    return speed_map[pipe.speed];
}

static vsf_err_t __f1cx00s_usbh_hcd_urb_fsm(f1cx00s_usbh_hcd_t *musb_hcd, vk_usbh_hcd_urb_t *urb)
{
    f1cx00s_usbh_hcd_urb_t *musb_urb = (f1cx00s_usbh_hcd_urb_t *)urb->priv;
    vk_usbh_hcd_dev_t *dev = urb->dev_hcd;
    f1cx00s_usbh_hcd_dev_t *musb_dev = dev->dev_priv;
    vk_usbh_pipe_t pipe = urb->pipe;
    bool is_in = pipe.dir_in1out0 > 0;
    uint_fast16_t epsize = pipe.size;
    uint8_t *buffer = urb->buffer;

    __f1cx00s_usb_set_ep(musb_hcd->otg, musb_urb->epidx);
    switch (musb_urb->state) {
    case URB_STATE_START_SUBMITTING: {
            bool is_int_iso = !__f1cx00s_usbh_hcd_urb_for_queue(urb);
#if VSF_USBH_USE_HUB == ENABLED
            vk_usbh_dev_t *dev = (vk_usbh_dev_t *)urb->dev_hcd;
            vk_usbh_dev_t *dev_parent = dev->dev_parent;
            uint_fast8_t hub_addr = 0, hub_port = 0;

            if (dev_parent != NULL) {
                hub_addr = dev_parent->devnum;
                hub_port = dev->index + 1;
#   if F1CX00S_USBH_TRACE_EN == ENABLED
                vsf_trace_debug("hub_addr: %d\r\n", hub_addr);
                vsf_trace_debug("hub_port: %d\r\n", hub_port);
#   endif
            }
#endif

            musb_urb->state = URB_STATE_SUBMITTING;

            urb->actual_length = 0;
            urb->status = VSF_ERR_NONE;

            if (!is_in || !pipe.endpoint) {
                // for normal musb, use FAddr
                MUSB_BASE->Index.TxFuncAddr = pipe.address;
#if VSF_USBH_USE_HUB == ENABLED
                MUSB_BASE->Index.TxHubAddr = hub_addr;
                MUSB_BASE->Index.TxHubPort = hub_port;
#endif
            } else {
                // for normal musb, use RAddr
                MUSB_BASE->Index.RxFuncAddr = pipe.address;
#if VSF_USBH_USE_HUB == ENABLED
                MUSB_BASE->Index.RxHubAddr = hub_addr;
                MUSB_BASE->Index.RxHubPort = hub_port;
#endif
            }

            if (0 == pipe.endpoint) {
                MUSB_BASE->Index.HC.EP0.Type0 = __f1cx00s_usbh_hcd_get_speed(pipe) << 6;

                musb_urb->ep0_state = EP0_STATE_SETUP;
                __f1cx00s_usb_write_fifo(musb_hcd->otg, 0, (uint8_t *)&urb->setup_packet, sizeof(urb->setup_packet));
                MUSB_BASE->Index.HC.EP0.CSR0 &= ~(MUSBH_CSR0_SetupPkt | MUSBH_CSR0_StatusPkt);
                MUSB_BASE->Index.HC.EP0.CSR0 |= MUSBH_CSR0_TxPktRdy | MUSBH_CSR0_SetupPkt;
                break;
            } else {
                if (is_in) {
                    MUSB_BASE->Index.HC.EPN.RxType = (__f1cx00s_usbh_hcd_get_speed(pipe) << 6) | (pipe.type << 4) | pipe.endpoint;
                    MUSB_BASE->Index.HC.EPN.RxMaxP = (epsize + 7) >> 3;

                    if (is_int_iso) {
                        MUSB_BASE->Index.HC.EPN.RxInterval = 1;
                    } else {
                        MUSB_BASE->Index.HC.EPN.RxInterval = 3;
                    }

                    MUSB_BASE->Index.HC.EPN.RxCSRL |= MUSBH_RxCSRL_FlushFIFO;
                    if (musb_dev->toggle[1] & (1 << pipe.endpoint)) {
                        MUSB_BASE->Index.HC.EPN.RxCSRH = (MUSB_BASE->Index.HC.EPN.RxCSRH | MUSBH_RxCSRH_DataToggle) | MUSBH_RxCSRH_DataToggleWrEnable;
                    } else {
                        MUSB_BASE->Index.HC.EPN.RxCSRH = (MUSB_BASE->Index.HC.EPN.RxCSRH & ~MUSBH_RxCSRH_DataToggle) | MUSBH_RxCSRH_DataToggleWrEnable;
                    }
                } else {
                    MUSB_BASE->Index.HC.EPN.TxType = (__f1cx00s_usbh_hcd_get_speed(pipe) << 6) | (pipe.type << 4) | pipe.endpoint;
                    MUSB_BASE->Index.HC.EPN.TxMaxP = (epsize + 7) >> 3;

                    if (is_int_iso) {
                        MUSB_BASE->Index.HC.EPN.TxInterval = 1;
                    } else {
                        MUSB_BASE->Index.HC.EPN.TxInterval = 3;
                    }

                    MUSB_BASE->Index.HC.EPN.TxCSRL |= MUSBH_TxCSRL_FlushFIFO;
                    if (musb_dev->toggle[0] & (1 << pipe.endpoint)) {
                        MUSB_BASE->Index.HC.EPN.TxCSRH = (MUSB_BASE->Index.HC.EPN.TxCSRH | MUSBH_TxCSRH_DataToggle) | MUSBH_TxCSRH_DataToggleWrEnable;
                    } else {
                        MUSB_BASE->Index.HC.EPN.TxCSRH = (MUSB_BASE->Index.HC.EPN.TxCSRH & ~MUSBH_TxCSRH_DataToggle) | MUSBH_TxCSRH_DataToggleWrEnable;
                    }
                }
                goto do_tx_rx;
            }
        }
        // fall through
    case URB_STATE_SUBMITTING:
        if (0 == pipe.endpoint) {
            switch (musb_urb->ep0_state) {
            case EP0_STATE_SETUP:
                musb_urb->ep0_state = EP0_STATE_DATA;
                goto do_ep0_data_stage;
            case EP0_STATE_DATA:
                if ((musb_urb->cur_size > 0) && is_in) {
                    musb_urb->cur_size = __f1cx00s_usb_rxfifo_size(musb_hcd->otg, 0);
                    if (musb_urb->cur_size > 0) {
                        __f1cx00s_usb_read_fifo(musb_hcd->otg, 0, &buffer[urb->actual_length], musb_urb->cur_size);
                        MUSB_BASE->Index.HC.EP0.CSR0 &= ~MUSBH_CSR0_RxPktRdy;
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
                    musb_urb->ep0_state = EP0_STATE_STATUS;
                    MUSB_BASE->Index.HC.EP0.CSR0 &= ~(MUSBH_CSR0_SetupPkt | MUSBH_CSR0_StatusPkt);
                    if (is_in) {
                        MUSB_BASE->Index.HC.EP0.CSR0 |= MUSBH_CSR0_TxPktRdy | MUSBH_CSR0_StatusPkt;
                    } else {
                        MUSB_BASE->Index.HC.EP0.CSR0 |= MUSBH_CSR0_ReqPkt | MUSBH_CSR0_StatusPkt;
                    }
                } else {
                    if (is_in) {
                        MUSB_BASE->Index.HC.EP0.CSR0 |= MUSBH_CSR0_ReqPkt;
                    } else {
                        __f1cx00s_usb_write_fifo(musb_hcd->otg, 0, &buffer[urb->actual_length], musb_urb->cur_size);
                        MUSB_BASE->Index.HC.EP0.CSR0 |= MUSBH_CSR0_TxPktRdy;
                    }
                }
                break;
            case EP0_STATE_STATUS:
                goto urb_finished;
            }
        } else {
            // NAKTimeout is not error, on NAKTimer, urb will be enqueue again
            if (is_in) {
                if (MUSB_BASE->Index.HC.EPN.RxCSRH & MUSBH_RxCSRH_DataToggle) {
                    musb_dev->toggle[1] |= 1 << pipe.endpoint;
                } else {
                    musb_dev->toggle[1] &= ~(1 << pipe.endpoint);
                }

                if (MUSB_BASE->Index.HC.EPN.RxCSRL & (MUSBH_RxCSRL_RxStall | MUSBH_RxCSRL_Error)) {
#if F1CX00S_USBH_TRACE_EN == ENABLED
                    vsf_trace_debug("rx urb failed: %08X %02X\r\n", urb, MUSB_BASE->Index.HC.EPN.RxCSRL);
#endif
                    MUSB_BASE->Index.HC.EPN.RxCSRL &= ~(MUSBH_RxCSRL_RxStall | MUSBH_RxCSRL_Error | MUSBH_RxCSRL_ReqPkt);
                    return VSF_ERR_FAIL;
                }
                if (MUSB_BASE->Index.HC.EPN.RxCSRL & MUSBH_RxCSRL_NAKTimeout) {
                    MUSB_BASE->Index.HC.EPN.RxCSRL &= ~(MUSBH_RxCSRL_NAKTimeout | MUSBH_RxCSRL_ReqPkt);
                    __f1cx00s_usbh_hcd_enqueue_urb(musb_hcd, musb_urb);
                    return VSF_ERR_NONE;
                }
            } else {
                if (MUSB_BASE->Index.HC.EPN.TxCSRH & MUSBH_TxCSRH_DataToggle) {
                    musb_dev->toggle[0] |= 1 << pipe.endpoint;
                } else {
                    musb_dev->toggle[0] &= ~(1 << pipe.endpoint);
                }

                if (MUSB_BASE->Index.HC.EPN.TxCSRL & (MUSBH_TxCSRL_RxStall | MUSBH_TxCSRL_Error)) {
#if F1CX00S_USBH_TRACE_EN == ENABLED
                    vsf_trace_debug("tx urb failed: %08X %02X\r\n", urb, MUSB_BASE->Index.HC.EPN.TxCSRL);
#endif
                    MUSB_BASE->Index.HC.EPN.TxCSRL &= ~(MUSBH_TxCSRL_RxStall | MUSBH_TxCSRL_Error | MUSBH_TxCSRL_TxPktRdy);
                    return VSF_ERR_FAIL;
                }
                if (MUSB_BASE->Index.HC.EPN.TxCSRL & MUSBH_TxCSRL_NAKTimeout) {
                    MUSB_BASE->Index.HC.EPN.TxCSRL &= ~(MUSBH_TxCSRL_NAKTimeout | MUSBH_TxCSRL_TxPktRdy);
                    __f1cx00s_usbh_hcd_enqueue_urb(musb_hcd, musb_urb);
                    return VSF_ERR_NONE;
                }
            }

            if ((musb_urb->cur_size > 0) && is_in) {
                musb_urb->cur_size = __f1cx00s_usb_rxfifo_size(musb_hcd->otg, musb_urb->epidx);
                if (musb_urb->cur_size > 0) {
                    __f1cx00s_usb_read_fifo(musb_hcd->otg, musb_urb->epidx, &buffer[urb->actual_length], musb_urb->cur_size);
                    MUSB_BASE->Index.HC.EPN.RxCSRL &= ~MUSBH_RxCSRL_RxPktRdy;
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
                    MUSB_BASE->Index.HC.EPN.RxCSRL |= MUSBH_RxCSRL_ReqPkt;
                } else {
                    __f1cx00s_usb_write_fifo(musb_hcd->otg, musb_urb->epidx, &buffer[urb->actual_length], musb_urb->cur_size);
                    MUSB_BASE->Index.HC.EPN.TxCSRL |= MUSBH_TxCSRL_TxPktRdy;
                }
            }
        }
        break;
    }
    return VSF_ERR_NOT_READY;

urb_finished:
#if F1CX00S_USBH_TRACE_EN == ENABLED
    vsf_trace_debug("urb_done: %08X %d %s%d %d\r\n", urb, pipe.address, pipe.dir_in1out0 ? "IN" : "OUT", pipe.endpoint, urb->actual_length);
#   if F1CX00S_USBH_TRACE_BUFFER_EN == ENABLED
    if (pipe.dir_in1out0) {
        vsf_trace_buffer(VSF_TRACE_DEBUG, urb->buffer, urb->actual_length);
    }
#   endif
#endif
    musb_urb->state = URB_STATE_IDLE;
    vsf_eda_post_msg(urb->eda_caller, urb);
    return VSF_ERR_NONE;
}

static void __f1cx00s_usbh_hcd_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    f1cx00s_usbh_hcd_t *musb_hcd = container_of(eda, f1cx00s_usbh_hcd_t, teda);

    switch (evt) {
    case VSF_EVT_INIT:
    wait_next_urb:
        if (vsf_eda_sem_pend(&musb_hcd->sem, -1)) {
            break;
        }
        // got sem, fall through
    case VSF_EVT_SYNC: {
            f1cx00s_usbh_hcd_urb_t *musb_urb;
            vsf_protect_t orig = vsf_protect_sched();
                vsf_dlist_remove_head(f1cx00s_usbh_hcd_urb_t, urb_node, &musb_hcd->urb_list, musb_urb);
            vsf_unprotect_sched(orig);
            if (NULL == musb_urb) {
                goto wait_next_urb;
            } else {
                musb_urb->state = URB_STATE_START_SUBMITTING;
            }

            vk_usbh_hcd_urb_t *urb = container_of(musb_urb, vk_usbh_hcd_urb_t, priv);
            musb_hcd->urb_cur[0] = urb;
        }
        // chained urb, use EP0
        evt = HCD_EVT_EP;
        goto on_ep;
    case VSF_EVT_TIMER:
        if (HCD_STATE_WAIT_RESET == musb_hcd->state) {
            MUSB_BASE->Common.Power &= ~MUSB_Power_Reset;
            vsf_teda_set_timer_ms(100);
            musb_hcd->state = HCD_STATE_WAIT_RESET_CLEAR;
        } else if (HCD_STATE_WAIT_RESET_CLEAR == musb_hcd->state) {
            enum usb_device_speed_t speed;
            if (MUSB_BASE->Common.DevCtl & MUSB_DevCtl_LSDev) {
                speed = USB_SPEED_LOW;
            } else if (MUSB_BASE->Common.DevCtl & MUSB_DevCtl_FSDev) {
                // TODO: check if full speed
                speed = USB_SPEED_HIGH;
            } else {
                VSF_OSA_HAL_ASSERT(false);
                speed = USB_SPEED_UNKNOWN;
            }

            musb_hcd->state = HCD_STATE_CONNECTED;
            musb_hcd->dev = vk_usbh_new_device((vk_usbh_t *)musb_hcd->hcd, speed, NULL, 0);
            MUSB_BASE->Common.IntrUSBE = MUSB_IntrUSBE_Discon;
        }
        break;
    case HCD_EVT_CONN:
        if (HCD_STATE_WAIT_CONNECT == musb_hcd->state) {
            MUSB_BASE->Common.Power |= MUSB_Power_Reset;
            vsf_teda_set_timer_ms(100);
            musb_hcd->state = HCD_STATE_WAIT_RESET;
        }
        break;
    case HCD_EVT_DISCONN:
        if (HCD_STATE_CONNECTED == musb_hcd->state) {
            vk_usbh_disconnect_device((vk_usbh_t *)musb_hcd->hcd, musb_hcd->dev);
            musb_hcd->dev = NULL;
            musb_hcd->state = HCD_STATE_WAIT_CONNECT;
        }
        MUSB_BASE->Common.IntrUSBE = MUSBH_IntrUSBE_Conn;
        break;
    default:
    on_ep: {
            evt -= VSF_EVT_USER;
            vk_usbh_hcd_urb_t *urb = __f1cx00s_usbh_hcd_get_urb(musb_hcd, evt & 0xFF);
            VSF_OSA_HAL_ASSERT(urb != NULL);
            evt &= 0xF00;

            if ((HCD_EVT_EP - VSF_EVT_USER) == evt) {
                f1cx00s_usbh_hcd_urb_t *musb_urb = (f1cx00s_usbh_hcd_urb_t *)urb->priv;
                bool is_in_queue = __f1cx00s_usbh_hcd_urb_for_queue(urb);

                if (URB_STATE_TO_FREE == musb_urb->state) {
                    __f1cx00s_usbh_hcd_free_urb_imp(musb_hcd, urb);
                } else if (HCD_STATE_CONNECTED == musb_hcd->state) {
                    vsf_err_t err = __f1cx00s_usbh_hcd_urb_fsm(musb_hcd, urb);
                    if (err < 0) {
                        goto urb_fail;
                    } else if (err > 0) {
                        break;
                    }
                } else {
                urb_fail:
                    musb_urb->state = URB_STATE_IDLE;
                    urb->status = VSF_ERR_FAIL;
#if F1CX00S_USBH_TRACE_EN == ENABLED
                    vsf_trace_debug("urb failed: %08X\r\n", urb);
#endif
                    vsf_eda_post_msg(urb->eda_caller, urb);
                }
                if (is_in_queue) {
                    goto wait_next_urb;
                }
            } else {
                VSF_OSA_HAL_ASSERT(false);
            }
        }
        break;
    }
}

static vsf_err_t __f1cx00s_usbh_hcd_init_evthandler(vsf_eda_t *eda, vsf_evt_t evt, vk_usbh_hcd_t *hcd)
{
    f1cx00s_usbh_hcd_t *musb_hcd;
    vsf_usb_hcd_param_t *param;

    VSF_OSA_HAL_ASSERT(hcd != NULL);
    if (hcd->priv != NULL) {
        musb_hcd = hcd->priv;
    }
    param = hcd->param;
    VSF_OSA_HAL_ASSERT(param != NULL);

    switch (evt) {
    case VSF_EVT_INIT: {
            uint_fast32_t reg_tmp;

            memset(&USB_HC0, 0, sizeof(USB_HC0));
            USB_HC0.otg = &USB_OTG0;
            musb_hcd = hcd->priv = &USB_HC0;
            musb_hcd->hcd = hcd;
            // mask EP0 and EP1, not usable for unchained urb
            musb_hcd->unchained_ep_in_mask = 0x03;

            __f1cx00s_usb_register_irq(musb_hcd->otg, (void (*)(void *))__f1cx00s_usbh_hcd_isr, musb_hcd);
            __f1cx00s_usb_init(musb_hcd->otg, param->priority);

            reg_tmp = MUSB_BASE->Vendor.ISCR;
            reg_tmp &= ~(   MUSB_ISCR_VBUS_CHANGE_DETECT | MUSB_ISCR_ID_CHANGE_DETECT | MUSB_ISCR_DPDM_CHANGE_DETECT
                        |   MUSB_ISCR_FORCE_ID | MUSB_ISCR_FORCE_VBUS_VALID);
            reg_tmp |= MUSB_ISCR_FORCE_ID_HOST | MUSB_ISCR_FORCE_VBUS_VALID_HIGH;
            MUSB_BASE->Vendor.ISCR = reg_tmp;

            for (uint_fast8_t i = 1; i < 16; i++) {
                MUSB_BASE->Common.Index = i;
                MUSB_BASE->Index.HC.EPN.TxCSRL = MUSBH_TxCSRL_FlushFIFO;
                MUSB_BASE->Index.HC.EPN.RxCSRL = MUSBH_RxCSRL_FlushFIFO;
            }
            MUSB_BASE->Common.Index = 0;
            MUSB_BASE->Index.HC.EP0.CSR0 = MUSBH_CSR0_FlushFIFO;
            // 1024 bytes from 0, for ep1, shared with ep0
            MUSB_BASE->Common.Index = 1;
            MUSB_BASE->Index.RxFIFOadd = 0 >> 3;
            MUSB_BASE->Index.RxFIFOsz = 7;
            MUSB_BASE->Index.TxFIFOadd = 0 >> 3;
            MUSB_BASE->Index.TxFIFOsz = 7;
            // 512 bytes from 1024, for ep2
            MUSB_BASE->Common.Index = 2;
            MUSB_BASE->Index.RxFIFOadd = 1024 >> 3;
            MUSB_BASE->Index.RxFIFOsz = 6;
            MUSB_BASE->Index.TxFIFOadd = 1024 >> 3;
            MUSB_BASE->Index.TxFIFOsz = 6;
            // 512 bytes from 1024 + 512, for ep3
            MUSB_BASE->Common.Index = 3;
            MUSB_BASE->Index.RxFIFOadd = (1024 + 512) >> 3;
            MUSB_BASE->Index.RxFIFOsz = 6;
            MUSB_BASE->Index.TxFIFOadd = (1024 + 512) >> 3;
            MUSB_BASE->Index.TxFIFOsz = 6;

            __f1cx00s_usb_clear_interrupt(musb_hcd->otg);
            MUSB_BASE->Common.IntrUSBE = MUSBH_IntrUSBE_Conn;
            // shared ep0 interrupt
            MUSB_BASE->Common.IntrTxE = 1 << 0;
            MUSB_BASE->Common.DevCtl = MUSB_DevCtl_Session;

            musb_hcd->state = HCD_STATE_WAIT_HOSTMODE;
        }
        // fall through
    case VSF_EVT_TIMER:
        VSF_OSA_HAL_ASSERT(HCD_STATE_WAIT_HOSTMODE == musb_hcd->state);
        if (!MUSB_BASE->Common.DevCtl & MUSB_DevCtl_HostMode) {
            vsf_teda_set_timer_ms(1);
        } else {
            musb_hcd->state = HCD_STATE_WAIT_CONNECT;
            vsf_eda_sem_init(&musb_hcd->sem, 0);
            musb_hcd->teda.fn.evthandler = __f1cx00s_usbh_hcd_evthandler;
            vsf_teda_init(&musb_hcd->teda, vsf_prio_inherit, false);

            // if device is already connected on startup, Connect interupt will not issue
            if (MUSB_BASE->Common.DevCtl & (MUSB_DevCtl_LSDev | MUSB_DevCtl_FSDev)) {
                vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_CONN);
            }
            return VSF_ERR_NONE;
        }
        break;
    }
    return VSF_ERR_NOT_READY;
}

static vsf_err_t __f1cx00s_usbh_hcd_fini(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __f1cx00s_usbh_hcd_suspend(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static vsf_err_t __f1cx00s_usbh_hcd_resume(vk_usbh_hcd_t *hcd)
{
    return VSF_ERR_NONE;
}

static uint_fast16_t __f1cx00s_usbh_hcd_get_frame_number(vk_usbh_hcd_t *hcd)
{
    return MUSB_BASE->Common.Frame;
}

static vsf_err_t __f1cx00s_usbh_hcd_alloc_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    f1cx00s_usbh_hcd_dev_t *musb_dev = vsf_usbh_malloc(sizeof(*musb_dev));
    if (NULL == musb_dev) { return VSF_ERR_NOT_ENOUGH_RESOURCES; }

    // initialize all members in musb_dev here if there is
    memset(musb_dev->ep_idx, -1, sizeof(musb_dev->ep_idx));
    // IN0 and OUT0 uses shared ep0
    musb_dev->ep_in_idx[0] = 0;
    musb_dev->ep_out_idx[0] = 0;
    musb_dev->toggle[0] = 0;
    musb_dev->toggle[1] = 0;
    dev->dev_priv = musb_dev;
    return VSF_ERR_NONE;
}

static void __f1cx00s_usbh_hcd_free_device(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    f1cx00s_usbh_hcd_dev_t *musb_dev = dev->dev_priv;

    if (musb_dev != NULL) {
        // DO NOT free ep0, which is shared by all devices
        // ep resources can only be freed here,
        //  because hardware maintain the data toggle status
        for (int i = 1; i < dimof(musb_dev->ep_in_idx); i++) {
            if (musb_dev->ep_in_idx[i] > 0) {
                __f1cx00s_usbh_hcd_free_ep((f1cx00s_usbh_hcd_t *)hcd->priv, i, true);
            }
        }
        for (int i = 1; i < dimof(musb_dev->ep_out_idx); i++) {
            if (musb_dev->ep_out_idx[i] > 0) {
                __f1cx00s_usbh_hcd_free_ep((f1cx00s_usbh_hcd_t *)hcd->priv, i, false);
            }
        }

        vsf_usbh_free(musb_dev);
        dev->dev_priv = NULL;
    }
}

static vk_usbh_hcd_urb_t * __f1cx00s_usbh_hcd_alloc_urb(vk_usbh_hcd_t *hcd)
{
    vk_usbh_hcd_urb_t *urb;
    uint_fast32_t size;

    VSF_OSA_HAL_ASSERT(hcd != NULL);

    size = sizeof(vk_usbh_hcd_urb_t) + sizeof(f1cx00s_usbh_hcd_urb_t);
    urb = vsf_usbh_malloc(size);
    if (urb != NULL) {
        memset(urb, 0, size);
    }
    return urb;
}

static void __f1cx00s_usbh_hcd_free_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    f1cx00s_usbh_hcd_t *musb_hcd = hcd->priv;
    f1cx00s_usbh_hcd_urb_t *musb_urb = (f1cx00s_usbh_hcd_urb_t *)urb->priv;

    if (musb_urb->state != URB_STATE_IDLE) {
        musb_urb->state = URB_STATE_TO_FREE;
    } else {
        __f1cx00s_usbh_hcd_free_urb_imp(musb_hcd, urb);
    }
}

static vsf_err_t __f1cx00s_usbh_hcd_submit_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    f1cx00s_usbh_hcd_t *musb_hcd = hcd->priv;
    f1cx00s_usbh_hcd_urb_t *musb_urb = (f1cx00s_usbh_hcd_urb_t *)urb->priv;

    // allocate ep resources
    vk_usbh_pipe_t pipe = urb->pipe;
    f1cx00s_usbh_hcd_dev_t *musb_dev = urb->dev_hcd->dev_priv;
    int8_t *ep_idx_arr;
    volatile uint16_t *ep_inten;

    if (pipe.dir_in1out0) {
#if F1CX00S_USBH_TRACE_EN
        vsf_trace_debug("submit_urb: %08x %d %s%d %d\r\n", urb, pipe.address, pipe.dir_in1out0 ? "IN" : "OUT", pipe.endpoint, urb->transfer_length);
#endif
        ep_idx_arr = musb_dev->ep_in_idx;
        ep_inten = &MUSB_BASE->Common.IntrRxE;
    } else {
#if F1CX00S_USBH_TRACE_EN == ENABLED
        vsf_trace_debug("submit_urb: %08x %d %s%d %d\r\n", urb, pipe.address, pipe.dir_in1out0 ? "IN" : "OUT", pipe.endpoint, urb->transfer_length);
#   if F1CX00S_USBH_TRACE_BUFFER_EN == ENABLED
        vsf_trace_buffer(VSF_TRACE_DEBUG, urb->buffer, urb->transfer_length);
#   endif
#endif
        ep_idx_arr = musb_dev->ep_out_idx;
        ep_inten = &MUSB_BASE->Common.IntrTxE;
    }

    if (!__f1cx00s_usbh_hcd_urb_for_queue(urb) && pipe.dir_in1out0) {
        // int and iso IN transfers are not put in queue
        uint_fast8_t ep_idx = ffz(musb_hcd->unchained_ep_in_mask);
        if (ep_idx <= __f1cx00s_usb_get_ep_num(musb_hcd->otg)) {
            vsf_protect_t orig = vsf_protect_sched();
                ep_idx_arr[pipe.endpoint] = ep_idx;
                *ep_inten |= 1 << ep_idx;
            vsf_unprotect_sched(orig);
            musb_hcd->urb_cur[ep_idx - 1] = urb;
            musb_urb->epidx = ep_idx_arr[pipe.endpoint];
            musb_urb->state = URB_STATE_START_SUBMITTING;
            vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_EP + ep_idx + 0x80);
            return VSF_ERR_NONE;
        } else {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
    }

    // ep_idx_arr[0] is initialized to use EP0 while allocating device
    // other chained urb use EP1
    if (ep_idx_arr[pipe.endpoint] < 0) {
        vsf_protect_t orig = vsf_protect_sched();
            ep_idx_arr[pipe.endpoint] = 1;
            *ep_inten |= 1 << 1;
        vsf_unprotect_sched(orig);
    }
    musb_urb->epidx = ep_idx_arr[pipe.endpoint];
    return __f1cx00s_usbh_hcd_enqueue_urb(musb_hcd, musb_urb);
}

static vsf_err_t __f1cx00s_usbh_hcd_relink_urb(vk_usbh_hcd_t *hcd, vk_usbh_hcd_urb_t *urb)
{
    return __f1cx00s_usbh_hcd_submit_urb(hcd, urb);
}

static vsf_err_t __f1cx00s_usbh_hcd_reset_dev(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    // TODO:
    return VSF_ERR_NONE;
}

static bool __f1cx00s_usbh_hcd_is_dev_reset(vk_usbh_hcd_t *hcd, vk_usbh_hcd_dev_t *dev)
{
    // TODO:
    return false;
}

static void __f1cx00s_usbh_hcd_isr(f1cx00s_usbh_hcd_t *musb_hcd)
{
    uint_fast16_t status = MUSB_BASE->Common.IntrUSB;
    status &= MUSB_BASE->Common.IntrUSBE;

    if (status & MUSBH_IntrUSB_Conn) {
        MUSB_BASE->Common.IntrUSB = MUSBH_IntrUSB_Conn;
        vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_CONN);
    }
    if (status & MUSB_IntrUSB_Discon) {
        MUSB_BASE->Common.IntrUSB = MUSB_IntrUSB_Discon;
        vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_DISCONN);
    }
    if (status & MUSBH_IntrUSB_Babble) {
        MUSB_BASE->Common.IntrUSB = MUSBH_IntrUSB_Babble;
        // Babble for host
    }
    if (status & MUSB_IntrUSB_SOF) {
        MUSB_BASE->Common.IntrUSB = MUSB_IntrUSB_SOF;
    }

    // EP interrupt
    status = MUSB_BASE->Common.IntrTx & MUSB_BASE->Common.IntrTxE;
    while (status) {
        uint_fast8_t ep_idx = ffz(~status);
        status &= ~(1 << ep_idx);
        MUSB_BASE->Common.IntrTx = 1 << ep_idx;
        vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_EP + ep_idx);
    }

    status = MUSB_BASE->Common.IntrRx & MUSB_BASE->Common.IntrRxE;
    while (status) {
        uint_fast8_t ep_idx = ffz(~status);
        status &= ~(1 << ep_idx);
        MUSB_BASE->Common.IntrRx = 1 << ep_idx;
        vsf_eda_post_evt(&musb_hcd->teda.use_as__vsf_eda_t, HCD_EVT_EP + ep_idx + 0x80);
    }
}

#endif      // VSF_USE_USB_HOST
