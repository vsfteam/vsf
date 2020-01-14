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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_MUSB_FDRC == ENABLED

#define VSF_MUSB_FDRC_DCD_IMPLEMENT
// TODO: use dedicated include
#include "vsf.h"
#include "./__vsf_musb_fdrc_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void vk_musb_fdrc_usbd_reset_do(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;

    reg->Common.Power = 0;
    vk_musb_fdrc_interrupt_init(reg);
    reg->Common.IntrUSBE = MUSB_INTRUSBE_RESET;
    usbd->ep_buf_ptr = 0;
    usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
}

vsf_err_t vk_musb_fdrc_usbd_init(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    VSF_USB_ASSERT((usbd != NULL) && (cfg != NULL));
    VSF_USB_ASSERT((usbd->param != NULL) && (usbd->param->op != NULL));

    usb_dc_ip_info_t info;
    usbd->param->op->GetInfo(&info);
    usbd->reg = info.regbase;
    usbd->ep_num = info.ep_num;
    usbd->is_dma = info.is_dma;

    usbd->callback.evt_handler = cfg->evt_handler;
    usbd->callback.param = cfg->param;

    {
        usb_dc_ip_cfg_t ip_cfg = {
            .priority       = cfg->priority,
            .irq_handler    = (usb_ip_irq_handler_t)vk_musb_fdrc_usbd_irq,
            .param          = usbd,
        };
        usbd->param->op->Init(&ip_cfg);
    }

    vk_musb_fdrc_usbd_reset_do(usbd);
    return VSF_ERR_NONE;
}

void vk_musb_fdrc_usbd_fini(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common.IntrUSBE = 0;
    usbd->param->op->Fini();
}

void vk_musb_fdrc_usbd_reset(vk_musb_fdrc_dcd_t *usbd, usb_dc_cfg_t *cfg)
{
    vk_musb_fdrc_usbd_init(usbd, cfg);
}

void vk_musb_fdrc_usbd_connect(vk_musb_fdrc_dcd_t *usbd)
{
    usbd->param->op->Connect();
}

void vk_musb_fdrc_usbd_disconnect(vk_musb_fdrc_dcd_t *usbd)
{
    usbd->param->op->Disconnect();
}

void vk_musb_fdrc_usbd_wakeup(vk_musb_fdrc_dcd_t *usbd)
{
}

void vk_musb_fdrc_usbd_set_address(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t addr)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    reg->Common.FAddr = addr;
}

uint_fast8_t vk_musb_fdrc_usbd_get_address(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    return reg->Common.FAddr;
}

uint_fast16_t vk_musb_fdrc_usbd_get_frame_number(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    return ((reg->Common.Frame2 & 0x07) << 8) | reg->Common.Frame1;
}

extern uint_fast8_t vk_musb_fdrc_usbd_get_mframe_number(vk_musb_fdrc_dcd_t *usbd)
{
    return 0;
}

void vk_musb_fdrc_usbd_get_setup(vk_musb_fdrc_dcd_t *usbd, uint8_t *buffer)
{
    vk_musb_fdrc_read_fifo(usbd->reg, 0, buffer, 8);

    VSF_USB_ASSERT(MUSB_FDRC_USBD_EP0_IDLE == usbd->ep0_state);
    usbd->has_data_stage = false;
    if (!(buffer[0] & 0x80)) {
        usbd->ep0_state = MUSB_FDRC_USBD_EP0_DATA_OUT;
    }
}

void vk_musb_fdrc_usbd_status_stage(vk_musb_fdrc_dcd_t *usbd, bool is_in)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;

    if (usbd->has_data_stage) {
        uint_fast8_t ep_orig = vk_musb_fdrc_set_ep(reg, 0);
            reg->EP0.CSR0 |= MUSBD_CSR0_DATAEND;
        vk_musb_fdrc_set_ep(reg, ep_orig);
    }
}

uint_fast8_t vk_musb_fdrc_usbd_ep_get_feature(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    return usbd->is_dma ? USB_DC_FEATURE_TRANSFER : 0;
}

vsf_err_t vk_musb_fdrc_usbd_ep_add(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, usb_ep_type_t type, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast16_t addr = usbd->ep_buf_ptr >> 3;
    uint_fast8_t size_msk;
    uint_fast8_t ep_orig;

    if (size == 1023) { size = 1024; }
    VSF_USB_ASSERT(!(size & (size - 1)) && (size >= 8) && (size <= 1024));

    size_msk = ffz(~size) - 3;
    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);

    if (is_in) {
        reg->EPN.RxFIFO1 = addr & 0xFF;
        reg->EPN.RxFIFO2 = ((addr >> 8) & 0x0F) | (size_msk << 5);
        if (!ep) {
            VSF_USB_ASSERT(64 == size);
        } else {
            reg->EPN.TxMAXP = size;
            reg->EPN.TxCSR2 |= MUSB_TXCSR2_MODE;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                VSF_USB_ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EPN.TxCSR2 |= MUSBD_TXCSR2_ISO;
                reg->EPN.TxCSR2 &= ~MUSB_TXCSR2_FRCDATATOG;
                break;
            case USB_EP_TYPE_BULK:
                reg->EPN.TxCSR2 &= ~(MUSBD_TXCSR2_ISO | MUSB_TXCSR2_FRCDATATOG);
                break;
            case USB_EP_TYPE_INTERRUPT:
                reg->EPN.TxCSR2 &= ~MUSBD_TXCSR2_ISO;
                reg->EPN.TxCSR2 |= MUSB_TXCSR2_FRCDATATOG;
                break;
            }
        }
        vk_musb_fdrc_set_mask(&reg->Common.IntrTx1E, ep);
    } else {
        reg->EPN.TxFIFO1 = addr & 0xFF;
        reg->EPN.TxFIFO2 = ((addr >> 8) & 0x0F) | (size_msk << 5);
        if (ep != 0) {
            reg->EPN.RxMAXP = size;
            vk_musb_fdrc_set_mask(&reg->Common.IntrRx1E, ep);
            reg->EPN.RxCSR1 |= MUSBD_RXCSR1_CLRDATATOG;
            switch (type) {
            case USB_EP_TYPE_CONTROL:
                VSF_USB_ASSERT(false);
                return VSF_ERR_FAIL;
            case USB_EP_TYPE_ISO:
                reg->EPN.RxCSR2 |= MUSBD_TXCSR2_ISO;
                break;
            default:
                reg->EPN.RxCSR2 &= ~MUSBD_TXCSR2_ISO;
                break;
            }
            reg->EPN.RxCSR1 |= MUSBD_RXCSR1_FLUSHFIFO;
        }
    }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    usbd->ep_buf_ptr += size;
    return VSF_ERR_NONE;
}

uint_fast16_t vk_musb_fdrc_usbd_ep_get_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast16_t result;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    if (!ep) {
        return 64;
    }
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        result = reg->EPN.TxMAXP;
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return result;
}

vsf_err_t vk_musb_fdrc_usbd_ep_set_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            reg->EP0.CSR0 |= MUSBD_CSR0_SENDSTALL;
        } else {
            if (is_in) {
                reg->EPN.TxCSR1 |= MUSBD_TXCSR1_SENDSTALL;
            } else {
                reg->EPN.RxCSR1 |= MUSBD_RXCSR1_SENDSTALL;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

bool vk_musb_fdrc_usbd_ep_is_stalled(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80, is_stall;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            is_stall = reg->EP0.CSR0 & MUSBD_CSR0_SENDSTALL;
        } else {
            if (is_in) {
                is_stall = reg->EPN.TxCSR1 & MUSBD_TXCSR1_SENDSTALL;
            } else {
                is_stall = reg->EPN.RxCSR1 & MUSBD_RXCSR1_SENDSTALL;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return is_stall > 0;
}

vsf_err_t vk_musb_fdrc_usbd_ep_clear_stall(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t is_in = ep & 0x80;
    uint_fast8_t ep_orig;

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            reg->EP0.CSR0 &= ~(MUSBD_CSR0_SENTSTALL | MUSBD_CSR0_SENDSTALL);
        } else {
            if (is_in) {
                reg->EPN.TxCSR1 &= ~(MUSBD_TXCSR1_SENTSTALL | MUSBD_TXCSR1_SENDSTALL);
                reg->EPN.TxCSR1 |= MUSBD_TXCSR1_CLRDATATOG | MUSBD_TXCSR1_FLUSHFIFO;
            } else {
                reg->EPN.RxCSR1 &= ~(MUSBD_RXCSR1_SENTSTALL | MUSBD_RXCSR1_SENDSTALL);
                reg->EPN.RxCSR1 |= MUSBD_RXCSR1_CLRDATATOG | MUSBD_RXCSR1_FLUSHFIFO;
            }
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

uint_fast32_t vk_musb_fdrc_usbd_ep_get_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;

    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    return vk_musb_fdrc_rx_fifo_size(reg, ep);
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_read_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg;
    uint_fast8_t ep_orig;

    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    reg = usbd->reg;
    vk_musb_fdrc_read_fifo(reg, ep, buffer, size);

    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            reg->EP0.CSR0 |= MUSBD_CSR0_SERVICEDRXPKGRDY;
        } else {
            reg->EPN.RxCSR1 &= ~MUSBD_RXCSR1_RXPKTRDY;
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_enable_out(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep)
{
    VSF_USB_ASSERT(!(ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_set_data_size(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t ep_orig;

    VSF_USB_ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    ep &= 0x0F;
    ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        if (!ep) {
            uint_fast8_t ep0_int_en = reg->Common.IntrTx1E & 1;
            reg->Common.IntrTx1E &= ~1;
                usbd->ep0_state = MUSB_FDRC_USBD_EP0_DATA_IN;
                reg->EP0.CSR0 |= MUSBD_CSR0_TXPKTRDY;
            reg->Common.IntrTx1E |= ep0_int_en;
        } else {
            reg->EPN.TxCSR1 |= MUSBD_TXCSR1_TXPKTRDY;
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transaction_write_buffer(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    vk_musb_fdrc_reg_t *reg;

    VSF_USB_ASSERT((ep & 0x80) && ((ep & 0x0F)) < (usbd->ep_num / 2));

    reg = usbd->reg;
    vk_musb_fdrc_write_fifo(reg, ep, buffer, size);
    return VSF_ERR_NONE;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transfer_recv(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

vsf_err_t vk_musb_fdrc_usbd_ep_transfer_send(vk_musb_fdrc_dcd_t *usbd, uint_fast8_t ep, uint8_t *buffer, uint_fast32_t size, bool zlp)
{
    VSF_USB_ASSERT(false);
    return VSF_ERR_NOT_SUPPORT;
}

static void vk_musb_fdrc_usbd_notify(vk_musb_fdrc_dcd_t *usbd, usb_evt_t evt, uint_fast8_t value)
{
    if (usbd->callback.evt_handler != NULL) {
        usbd->callback.evt_handler(usbd->callback.param, evt, value);
    }
}

void vk_musb_fdrc_usbd_irq(vk_musb_fdrc_dcd_t *usbd)
{
    vk_musb_fdrc_reg_t *reg = usbd->reg;
    uint_fast8_t status, csr1;
    uint_fast16_t status_rx, status_tx;

    status = reg->Common.IntrUSB;
    status &= reg->Common.IntrUSBE;

    // USB interrupt
    if (status & MUSB_INTRUSB_RESET) {
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_RESET, 0);
        reg->Common.Power |= MUSB_POWER_ENABLESUSPEND;
    }
    if (status & MUSB_INTRUSB_SOF) {
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_SOF, 0);
    }
    if (status & MUSB_INTRUSB_RESUME) {
        reg->Common.Power |= MUSB_POWER_ENABLESUSPEND;
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_RESUME, 0);
    }
    if (status & MUSB_INTRUSB_SUSPEND) {
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_SUSPEND, 0);
    }

    status_rx = vk_musb_fdrc_get_mask(&reg->Common.IntrRx1);
    status_rx &= vk_musb_fdrc_get_mask(&reg->Common.IntrRx1E);
    status_tx = vk_musb_fdrc_get_mask(&reg->Common.IntrTx1);
    status_tx &= vk_musb_fdrc_get_mask(&reg->Common.IntrTx1E);

    // EP0 interrupt
    if (status_tx & 1) {
        uint_fast8_t ep_orig;

        status_tx &= ~1;
        ep_orig = vk_musb_fdrc_set_ep(reg, 0);
        csr1 = reg->EP0.CSR0;

        if (csr1 & MUSBD_CSR0_SENTSTALL) {
            reg->EP0.CSR0 &= ~MUSBD_CSR0_SENTSTALL;
            vk_musb_fdrc_set_ep(reg, ep_orig);
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
        }

        if (csr1 & MUSBD_CSR0_SETUPEND) {
            reg->EP0.CSR0 |= MUSBD_CSR0_SERVICEDSETUPEND;
            vk_musb_fdrc_set_ep(reg, ep_orig);

            vk_musb_fdrc_usbd_notify(usbd, USB_ON_STATUS, 0);
            usbd->ep0_state = MUSB_FDRC_USBD_EP0_IDLE;
        }

        if (csr1 & MUSBD_CSR0_RXPKTRDY) {
            switch (usbd->ep0_state) {
            case MUSB_FDRC_USBD_EP0_IDLE:
                vk_musb_fdrc_usbd_notify(usbd, USB_ON_SETUP, 0);
                break;
            case MUSB_FDRC_USBD_EP0_DATA_OUT:
                usbd->has_data_stage = true;
                vk_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, 0);
                break;
            default:
                VSF_USB_ASSERT(false);
            }
        }
        if (    (MUSB_FDRC_USBD_EP0_DATA_IN == usbd->ep0_state)
            &&  !(csr1 & MUSBD_CSR0_TXPKTRDY)) {
            usbd->has_data_stage = true;
            vk_musb_fdrc_usbd_notify(usbd, USB_ON_IN, 0);
        }
    }

    // EPN interrupt
    status_rx &= ~1;
    while (status_rx) {
        uint_fast8_t ep = ffz(~status_rx);
        status_rx &= ~(1 << ep);
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_OUT, ep);
    }

    while (status_tx) {
        uint_fast8_t ep = ffz(~status_tx);
        status_tx &= ~(1 << ep);
        vk_musb_fdrc_usbd_notify(usbd, USB_ON_IN, ep);
    }
}

#endif
