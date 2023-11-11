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

#if     (   (VSF_USE_USB_DEVICE == ENABLED)                                     \
        && (VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED))                             \
    ||  (   (VSF_USE_USB_HOST == ENABLED)                                       \
        && (VSF_USBH_USE_HCD_MUSB_FDRC == ENABLED))

#define VSF_MUSB_FDRC_IMPLEMENT
#include "./__vsf_musb_fdrc_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

// TODO: try 16-bit access to the reg
void vk_musb_fdrc_set_mask(volatile uint8_t *reg, uint_fast8_t ep)
{
    VSF_USB_ASSERT(ep < 16);
    if (ep < 8) {
        reg[0] |= 1 << ep;
    } else {
        reg[1] |= 1 << (ep - 8);
    }
}

void vk_musb_fdrc_clear_mask(volatile uint8_t *reg, uint_fast8_t ep)
{
    VSF_USB_ASSERT(ep < 16);
    if (ep < 8) {
        reg[0] &= ~(1 << ep);
    } else {
        reg[1] &= ~(1 << (ep - 8));
    }
}

uint16_t vk_musb_fdrc_get_mask(volatile uint8_t *reg)
{
    return reg[0] | (reg[1] << 8);
}

// Common API
void vk_musb_fdrc_interrupt_init(vk_musb_fdrc_reg_t *reg)
{
    reg->Common->IntrTx1E = 0;
    reg->Common->IntrTx2E = 0;
    reg->Common->IntrRx1E = 0;
    reg->Common->IntrRx2E = 0;
    reg->Common->IntrUSBE = 0;
}

uint_fast16_t vk_musb_fdrc_rx_fifo_size(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        uint_fast16_t result;
        if (!ep) {
#if VSF_MUSB_FDRC_WORKAROUND_WAIT_RXPKTRDY_BEFORE_READ_RX_COUNT == ENABLED
            while (!(reg->EP->EP0.CSR0 & MUSBD_CSR0_RXPKTRDY));
#else
            VSF_USB_ASSERT(reg->EP->EP0.CSR0 & MUSBD_CSR0_RXPKTRDY);
#endif
            result = reg->EP->EP0.Count0;
        } else {
#if VSF_MUSB_FDRC_WORKAROUND_WAIT_RXPKTRDY_BEFORE_READ_RX_COUNT == ENABLED
            while (!(reg->EP->EPN.RxCSR1 & MUSBD_RXCSR1_RXPKTRDY));
#else
            VSF_USB_ASSERT(reg->EP->EPN.RxCSR1 & MUSBD_RXCSR1_RXPKTRDY);
#endif
            result = ((reg->EP->EPN.RxCount2 & 7) << 8) | reg->EP->EPN.RxCount1;
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return result;
}

void vk_musb_fdrc_fifo_init(vk_musb_fdrc_reg_t *reg)
{
#if defined(VSF_MUSB_FDRC_NO_HWFIFO)
    memset(reg->__fifo_reg, 0, sizeof(reg->__fifo_reg));
    reg->FIFO = reg->__fifo_reg;
#endif
}

uint16_t vk_musb_fdrc_set_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, usb_ep_type_t type, uint16_t pos, uint_fast16_t size, uint_fast8_t size_msk)
{
    uint_fast8_t ep_no = ep & 0x0F, ep_dir = !!(ep & 0x80);
#if     defined(VSF_MUSB_FDRC_NO_HWFIFO)
    VSF_USB_ASSERT(ep_no < dimof(reg->FIFO->FIFO));
    void *fifo = (void *)&reg->__fifo[pos];
    if (0 == ep_no) {
        VSF_USB_ASSERT(0 == pos);
        reg->FIFO[0].FIFO[0] = fifo;
        reg->FIFO[1].FIFO[0] = fifo;
    } else {
        if (reg->Common->DevCtl & MUSB_DEVCTL_HOSTMODE) {
            ep_dir = !ep_dir;
        }
        reg->FIFO[ep_dir].FIFO[ep_no] = fifo;
    }
    reg->set_ep_fifo(reg->param, ep, fifo, size);
    return 1 << size_msk;
#elif   defined(VSF_MUSB_FDRC_DYNAMIC_FIFO)
    if (0 == ep_no) {
        // EP0 fifo should be at position 0
        VSF_USB_ASSERT(0 == pos);
    } else if (ep_dir) {
        reg->DynFIFO->TxFIFO1 = pos & 0xFF;
        reg->DynFIFO->TxFIFO2 = ((pos >> 8) & 0x0F) | (size_msk << 5);
    } else {
        reg->DynFIFO->RxFIFO1 = pos & 0xFF;
        reg->DynFIFO->RxFIFO2 = ((pos >> 8) & 0x0F) | (size_msk << 5);
    }
    return 1 << size_msk;
#elif   defined(VSF_MUSB_FDRC_DYNAMIC_FIFO2)
    if (0 == ep_no) {
        // EP0 fifo should be at position 0
        VSF_USB_ASSERT(0 == pos);
    } else if (ep_dir) {
        reg->DynFIFO->TxFIFOAdd = pos;
        reg->DynFIFO->TxFIFOSz = size_msk & 0x0F;
    } else {
        reg->DynFIFO->RxFIFOAdd = pos;
        reg->DynFIFO->RxFIFOSz = size_msk & 0x0F;
    }
    return 1 << size_msk;
#else
    #error unknown fifo
#endif
}

void vk_musb_fdrc_read_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    VSF_USB_ASSERT(size <= vk_musb_fdrc_rx_fifo_size(reg, ep));
    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dimof(reg->FIFO->FIFO));

#if defined(VSF_MUSB_FDRC_NO_HWFIFO)
    fifo = (uint8_t *)reg->FIFO[0].FIFO[ep];
    VSF_USB_ASSERT(fifo != NULL);
    memcpy(buffer, fifo, size);
#else
    fifo = (uint8_t *)&reg->FIFO->FIFO[ep];
    while (size--) {
        *buffer++ = *fifo;
    }
#endif
}

void vk_musb_fdrc_write_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    ep &= 0x0F;
    VSF_USB_ASSERT(ep < dimof(reg->FIFO->FIFO));

#if defined(VSF_MUSB_FDRC_NO_HWFIFO)
    fifo = (uint8_t *)reg->FIFO[1].FIFO[ep];
    VSF_USB_ASSERT(fifo != NULL);
    memcpy(fifo, buffer, size);
    reg->write_ep_fifo(reg->param, ep, size);
#else
    fifo = (uint8_t *)&reg->FIFO->FIFO[ep];
    while (size--) {
        *fifo = *buffer++;
    }
#endif
}

uint_fast8_t vk_musb_fdrc_set_ep(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep)
{
#ifdef VSF_MUSB_FDRC_NO_EP_IDX
    uint_fast8_t ep_orig = reg->__cur_ep;
    reg->EP = reg->get_ep_reg(reg->param, ep);
    reg->__cur_ep = ep;
    return ep_orig;
#else
    uint_fast8_t ep_orig = reg->Common->Index;
    reg->Common->Index = ep;
    return ep_orig;
#endif
}

#endif
