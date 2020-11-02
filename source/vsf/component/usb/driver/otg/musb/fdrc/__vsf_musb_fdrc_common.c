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
    reg->Common.IntrTx1E = 0;
    reg->Common.IntrTx2E = 0;
    reg->Common.IntrRx1E = 0;
    reg->Common.IntrRx2E = 0;
    reg->Common.IntrUSBE = 0;
}

uint_fast16_t vk_musb_fdrc_rx_fifo_size(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = vk_musb_fdrc_set_ep(reg, ep);
        uint_fast16_t result;
        if (!ep) {
            result = reg->EP0.Count0;
        } else {
            result = ((reg->EPN.RxCount2 & 7) << 8) | reg->EPN.RxCount1;
        }
    vk_musb_fdrc_set_ep(reg, ep_orig);
    return result;
}

void vk_musb_fdrc_read_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    VSF_USB_ASSERT(size <= vk_musb_fdrc_rx_fifo_size(reg, ep));

    fifo = (uint8_t *)((uint32_t)&reg->FIFO0 + (ep << 2));
    while (size--) {
        *buffer++ = *fifo;
    }
}

void vk_musb_fdrc_write_fifo(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    ep &= 0x0F;
    fifo = (uint8_t *)((uint32_t)&reg->FIFO0 + (ep << 2));
    while (size--) {
        *fifo = *buffer++;
    }
}

uint_fast8_t vk_musb_fdrc_set_ep(vk_musb_fdrc_reg_t *reg, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = reg->Common.Index;
    reg->Common.Index = ep;
    return ep_orig;
}

#endif
