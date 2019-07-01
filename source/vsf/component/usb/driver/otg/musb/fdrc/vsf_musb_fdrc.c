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

#if     (   (VSF_USE_USB_DEVICE == ENABLED)                                     \
        && (VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED))                       \
    ||  (   (VSF_USE_USB_HOST == ENABLED)                                       \
        && (VSF_USE_USB_DEVICE_HCD_MUSB_FDRC == ENABLED))

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_MUSB_FDRC == ENABLED
#   define VSF_USBH_IMPLEMENT_HCD
#endif
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_musb_fdrc_set_ep(__ep)                                              \
        do {                                                                    \
            reg->Common.Index = (__ep);                                         \
        } while (0)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// TODO: try 16-bit access to the reg
void vsf_musb_fdrc_set_mask(volatile uint8_t *reg, uint_fast8_t ep)
{
    ASSERT(ep < 16);
    if (ep < 8) {
        reg[0] = 1 << ep;
    } else {
        reg[1] = 1 << (ep - 8);
    }
}

uint16_t vsf_musb_fdrc_get_mask(volatile uint8_t *reg)
{
    return reg[0] | (reg[1] << 8);
}

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED

vsf_err_t vsf_musb_fdrc_usbd_init(vsf_musb_fdrc_usbd_t *usbd, usb_dc_cfg_t *cfg)
{
    ASSERT((usbd != NULL) && (cfg != NULL));
    ASSERT((usbd->param != NULL) && (usbd->param->dc_op != NULL));

    usbd->param->dc_op->GetInfo((void **)&usbd->reg, &usbd->ep_num, &usbd->is_dma);
    usbd->callback.evt_handler = cfg->evt_handler;
    usbd->callback.param = cfg->param;

    {
        usb_dc_ip_cfg_t ip_cfg = {
            .priority       = cfg->priority,
            .irq_handler    = (usb_ip_irq_handler_t)vsf_musb_fdrc_usbd_irq,
            .param          = usbd,
        };
        usbd->param->dc_op->Init(&ip_cfg);
    }

    vsf_musb_fdrc_usbd_reset(usbd);
    return VSF_ERR_NONE;
}

void vsf_musb_fdrc_usbd_fini(vsf_musb_fdrc_usbd_t *usbd)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common.IntrUSBE = 0;
    usbd->param->dc_op->Fini();
}

void vsf_musb_fdrc_usbd_reset(vsf_musb_fdrc_usbd_t *usbd)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;

    reg->Common.Power = 0;
    reg->Common.IntrUSBE = USB_INTR_RESET;
    usbd->ep_buf_ptr = 0;
    usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
}

void vsf_musb_fdrc_usbd_connect(vsf_musb_fdrc_usbd_t *usbd)
{
    usbd->param->dc_op->Connect();
}

void vsf_musb_fdrc_usbd_disconnect(vsf_musb_fdrc_usbd_t *usbd)
{
    usbd->param->dc_op->Disconnect();
}

void vsf_musb_fdrc_usbd_wakeup(vsf_musb_fdrc_usbd_t *usbd)
{
}

void vsf_musb_fdrc_usbd_set_address(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t addr)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common.FAddr = addr;
}

uint_fast8_t vsf_musb_fdrc_usbd_get_address(vsf_musb_fdrc_usbd_t *usbd)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    return reg->Common.FAddr;
}

uint_fast16_t vsf_musb_fdrc_usbd_get_frame_number(vsf_musb_fdrc_usbd_t *usbd)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    return ((reg->Common.Frame2 & 0x07) << 8) | reg->Common.Frame1;
}

extern uint_fast8_t vsf_musb_fdrc_usbd_get_mframe_number(vsf_musb_fdrc_usbd_t *usbd)
{
    return 0;
}

void vsf_musb_fdrc_usbd_get_setup(vsf_musb_fdrc_usbd_t *usbd, uint8_t *buffer)
{
    vsf_musb_fdrc_usbd_ep_read_buffer(usbd, 0, buffer, 8);

    ASSERT(MUSB_FDRC_USBD_EP0_IDLE == usbd->ep0_state);
    usbd->has_data_stage = false;
    if (!(buffer[0] & 0x80)) {
        usbd->ep0_state = MUSB_FDRC_USBD_EP0_DATA_OUT;
    }
}

void vsf_musb_fdrc_usbd_status_stage(vsf_musb_fdrc_usbd_t *usbd, bool is_in)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;

    if (usbd->has_data_stage) {
        vsf_musb_fdrc_set_ep(0);
        reg->EP0.CSR0 |= USB_CSR0_DATA_END;
    }
}

bool vsf_musb_fdrc_usbd_ep_is_dma(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    return usbd->is_dma;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_add(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast16_t addr = usbd->ep_buf_ptr >> 3;
    uint_fast8_t size_msk;

    if (size == 1023) { size = 1024; }
    ASSERT(!(size & (size - 1)) && (size >= 8) && (size <= 1024));

    size_msk = ffz(~size) - 3;
    ep &= 0x0F;
    vsf_musb_fdrc_set_ep(ep);

    if (is_in) {
        reg->EPN.RxFIFO1 = addr & 0xFF;
        reg->EPN.RxFIFO2 = ((addr >> 8) & 0x0F) | (size_msk << 5);
        if (!ep) {
            ASSERT(64 == size);
        } else {
            reg->EPN.TxMAXP = size;
            reg->EPN.TxCSR2 |= USB_TXCSR2_MODE;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EPN.TxCSR2 |= USB_TXCSR2_ISO;
                reg->EPN.TxCSR2 &= ~USB_TXCSR2_FRC_DATA_TOG;
                break;
            case USB_EP_TYPE_BULK:
                reg->EPN.TxCSR2 &= ~(USB_TXCSR2_ISO | USB_TXCSR2_FRC_DATA_TOG);
                break;
            case USB_EP_TYPE_INTERRUPT:
                reg->EPN.TxCSR2 &= ~USB_TXCSR2_ISO;
                reg->EPN.TxCSR2 |= USB_TXCSR2_FRC_DATA_TOG;
                break;
            }
        }
        vsf_musb_fdrc_set_mask(&reg->Common.IntrTx1E, ep);
    } else {
        reg->EPN.TxFIFO1 = addr & 0xFF;
        reg->EPN.TxFIFO2 = ((addr >> 8) & 0x0F) | (size_msk << 5);
        if (ep != 0) {
            reg->EPN.RxMAXP = size;
            vsf_musb_fdrc_set_mask(&reg->Common.IntrRx1E, ep);
            reg->EPN.RxCSR1 |= USB_RXCSR1_CLR_DATA_TOG;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EPN.RxCSR2 |= USB_TXCSR2_ISO;
                break;
            default:
                reg->EPN.RxCSR2 &= ~USB_TXCSR2_ISO;
                break;
            }
            reg->EPN.RxCSR1 |= USB_RXCSR1_FLUSH_FIFO;
        }
    }
    usbd->ep_buf_ptr += size;
    return VSF_ERR_NONE;
}

uint_fast16_t vsf_musb_fdrc_usbd_ep_get_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;

    ep &= 0x0F;
    if (!ep) {
        return 64;
    }
    vsf_musb_fdrc_set_ep(ep);
    return reg->EPN.TxMAXP;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_set_stall(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;

    ep &= 0x0F;
    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        reg->EP0.CSR0 |= USB_CSR0_SEND_STALL;
    } else {
        if (is_in) {
            reg->EPN.TxCSR1 |= USB_TXCSR1_SEND_STALL;
        } else {
            reg->EPN.RxCSR1 |= USB_RXCSR1_SEND_STALL;
        }
    }
    return VSF_ERR_NONE;
}

bool vsf_musb_fdrc_usbd_ep_is_stalled(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80, is_stall;

    ep &= 0x0F;
    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        is_stall = reg->EP0.CSR0 & USB_CSR0_SEND_STALL;
    } else {
        if (is_in) {
            is_stall = reg->EPN.TxCSR1 & USB_TXCSR1_SEND_STALL;
        } else {
            is_stall = reg->EPN.RxCSR1 & USB_RXCSR1_SEND_STALL;
        }
    }
    return is_stall > 0;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_clear_stall(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;

    ep &= 0x0F;
    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        reg->EP0.CSR0 &= ~(USB_CSR0_SENT_STALL | USB_CSR0_SEND_STALL);
    } else {
        if (is_in) {
            reg->EPN.TxCSR1 &= ~(USB_TXCSR1_SENT_STALL | USB_TXCSR1_SEND_STALL);
            reg->EPN.TxCSR1 |= USB_TXCSR1_CLR_DATA_TOG | USB_TXCSR1_FLUSH_FIFO;
        } else {
            reg->EPN.RxCSR1 &= ~(USB_RXCSR1_SENT_STALL | USB_RXCSR1_SEND_STALL);
            reg->EPN.RxCSR1 |= USB_RXCSR1_CLR_DATA_TOG | USB_RXCSR1_FLUSH_FIFO;
        }
    }
    return VSF_ERR_NONE;
}

uint_fast16_t vsf_musb_fdrc_usbd_ep_get_data_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;

    ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        return reg->EP0.Count0;
    } else {
        return ((reg->EPN.RxCount2 & 7) << 8) | reg->EPN.RxCount1;
    }
}

vsf_err_t vsf_musb_fdrc_usbd_ep_read_buffer(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vsf_musb_fdrc_reg_t *reg;
    uint8_t *fifo;

    ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    ASSERT(size <= vsf_musb_fdrc_usbd_ep_get_data_size(usbd, ep));

    reg = usbd->reg;
    fifo = (uint8_t *)((uint32_t)&reg->FIFO0 + (ep << 2));
    while (size--) {
        *buffer++ = *fifo;
    }

    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        reg->EP0.CSR0 |= USB_CSR0_SERVICED_RX_PKG_RDY;
    } else {
        reg->EPN.RxCSR1 &= ~USB_RXCSR1_RX_PKT_RDY;
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_enable_OUT(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep)
{
    ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    return VSF_ERR_NONE;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_set_data_size(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;

    ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    ep &= 0x0F;
    vsf_musb_fdrc_set_ep(ep);
    if (!ep) {
        uint_fast8_t ep0_int_en = reg->Common.IntrTx1E & 1;
        reg->Common.IntrTx1E &= ~1;
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_DATA_IN;
            reg->EP0.CSR0 |= USB_CSR0_TX_PKT_RDY;
        reg->Common.IntrTx1E |= ep0_int_en;
    } else {
        reg->EPN.TxCSR1 |= USB_TXCSR1_TX_PKT_RDY;
    }
    return VSF_ERR_NONE;
}

vsf_err_t vsf_musb_fdrc_usbd_ep_write_buffer(vsf_musb_fdrc_usbd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vsf_musb_fdrc_reg_t *reg;
    uint8_t *fifo;

    ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    ep &= 0x0F;
    reg = usbd->reg;
    fifo = (uint8_t *)((uint32_t)&reg->FIFO0 + (ep << 2));
    while (size--) {
        *fifo = *buffer++;
    }
    return VSF_ERR_NONE;
}

static void vsf_musb_fdrc_usbd_notify(vsf_musb_fdrc_usbd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evt_handler != NULL) {
        usbd->callback.evt_handler(usbd->callback.param, evt, value);
    }
}

void vsf_musb_fdrc_usbd_irq(vsf_musb_fdrc_usbd_t *usbd)
{
    vsf_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t status, csr1;
    uint_fast16_t status_rx, status_tx;

    status = reg->Common.IntrUSB;
    status &= reg->Common.IntrUSBE;

    // USB interrupt
    if (status & USB_INTR_RESET) {
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_RESET, 0);
        reg->Common.Power |= 1;
    }
    if (status & USB_INTR_SOF) {
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_SOF, 0);
    }
    if (status & USB_INTR_RESUME) {
        reg->Common.Power |= 1;
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_RESUME, 0);
    }
    if (status & USB_INTR_SUSPEND) {
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_SUSPEND, 0);
    }

    status_rx = vsf_musb_fdrc_get_mask(&reg->Common.IntrRx1);
    status_rx &= vsf_musb_fdrc_get_mask(&reg->Common.IntrRx1E);
    status_tx = vsf_musb_fdrc_get_mask(&reg->Common.IntrTx1);
    status_tx &= vsf_musb_fdrc_get_mask(&reg->Common.IntrTx1E);

    // EP0 interrupt
    if (status_tx & 1) {
        status_tx &= ~1;
        vsf_musb_fdrc_set_ep(0);
        csr1 = reg->EP0.CSR0;

        if (csr1 & USB_CSR0_SENT_STALL) {
            reg->EP0.CSR0 &= ~USB_CSR0_SENT_STALL;
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
        }

        if (csr1 & USB_CSR0_SETUP_END) {
            reg->EP0.CSR0 |= USB_CSR0_SERVICED_SETUP_END;
            vsf_musb_fdrc_usbd_notify(usbd, USB_ON_STATUS, 0);
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
        }

        if (csr1 & USB_CSR0_RX_PKT_RDY) {
            switch (usbd->ep0_state) {
            case MUSB_FDRC_USBD_EP0_IDLE:
                vsf_musb_fdrc_usbd_notify(usbd, USB_ON_SETUP, 0);
                break;
            case MUSB_FDRC_USBD_EP0_DATA_OUT:
                usbd->has_data_stage = true;
                vsf_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, 0);
                break;
            default:
                ASSERT(false);
            }
        }
        if (    (MUSB_FDRC_USBD_EP0_DATA_IN == usbd->ep0_state)
            &&  !(csr1 & USB_CSR0_TX_PKT_RDY)) {
            usbd->has_data_stage = true;
            vsf_musb_fdrc_usbd_notify(usbd, USB_ON_IN, 0);
        }
    }

    // EPN interrupt
    status_rx &= ~1;
    while (status_rx) {
        uint_fast8_t ep = ffz(~status_rx);
        status_rx &= ~(1 << ep);
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, ep);
    }

    while (status_tx) {
        uint_fast8_t ep = ffz(~status_tx);
        status_tx &= ~(1 << ep);
        vsf_musb_fdrc_usbd_notify(usbd, USB_ON_IN, ep);
    }
}
#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC

#endif
