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

//#include "../common.h"
#include "./usb.h"
#include "../intc/intc.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// TODO: add reg_base
struct f1cx00s_usb_otg_t {
    uint8_t ep_num;
    bool is_host;
    struct {
        void (*handler)(void *param);
        void *param;
    } irq;
};

/*============================ GLOBAL VARIABLES ==============================*/

f1cx00s_usb_otg_t USB_OTG0 = {
    .ep_num     = 3,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

uint_fast8_t __f1cx00s_usb_set_ep(f1cx00s_usb_otg_t *usb, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = MUSB_BASE->Common.Index;
    MUSB_BASE->Common.Index = ep;
    return ep_orig;
}

uint_fast8_t __f1cx00s_usb_get_ep_num(f1cx00s_usb_otg_t *usb)
{
    return usb->ep_num;
}

void __f1cx00s_usb_clear_interrupt(f1cx00s_usb_otg_t *usb)
{
    // disable all interrupts
    MUSB_BASE->Common.IntrUSBE = 0;
    MUSB_BASE->Common.IntrTxE = 0;
    MUSB_BASE->Common.IntrRxE = 0;
    MUSB_BASE->Common.IntrTx = 0xFFFF;
    MUSB_BASE->Common.IntrRx = 0xFFFF;
    MUSB_BASE->Common.IntrUSB = 0xFF;
}

void __f1cx00s_usb_register_irq(f1cx00s_usb_otg_t *usb, void (*irqhandler)(void *), void *param)
{
    usb->irq.handler = irqhandler;
    usb->irq.param = param;
    if (irqhandler != NULL) {
        intc_enable_irq(USBOTG_IRQn);
    } else {
        intc_disable_irq(USBOTG_IRQn);
    }
}

uint_fast16_t __f1cx00s_usb_rxfifo_size(f1cx00s_usb_otg_t *usb, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = __f1cx00s_usb_set_ep(usb, ep);
        uint_fast16_t result;
        if (!ep) {
            result = MUSB_BASE->Index.DC.EP0.Count0;
        } else {
            result = MUSB_BASE->Index.DC.EPN.RxCount;
        }
    __f1cx00s_usb_set_ep(usb, ep_orig);
    return result;
}

void __f1cx00s_usb_read_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    VSF_HAL_ASSERT(size <= __f1cx00s_usb_rxfifo_size(usb, ep));

    fifo = (uint8_t *)&MUSB_BASE->FIFO[ep];
    while (size--) {
        *buffer++ = *fifo;
    }
}

void __f1cx00s_usb_write_fifo(f1cx00s_usb_otg_t *usb, uint_fast8_t ep, uint8_t *buffer, uint_fast16_t size)
{
    uint8_t *fifo;

    ep &= 0x0F;
    fifo = (uint8_t *)&MUSB_BASE->FIFO[ep];
    while (size--) {
        *fifo = *buffer++;
    }
}

vsf_err_t __f1cx00s_usb_init(f1cx00s_usb_otg_t *usb, vsf_arch_prio_t priority)
{
    CCU_BASE->USBPHY_CLK |= USBPHY_CLK_SCLK_GATING;
    CCU_BASE->USBPHY_CLK |= USBPHY_CLK_USBPHY_RST;
    CCU_BASE->BUS_CLK_GATING0 |= BUS_CLK_GATING0_USB_OTG_GATING;
    CCU_BASE->BUS_SOFT_RST0 |= BUS_SOFT_RST0_USBOTG_RST;

    // __f1cx00s_usb_phy_config(usbd->otg);
    SYSCON_BASE->USB_CTRL = (SYSCON_BASE->USB_CTRL & ~USB_FIFO_MODE) | USB_FIFO_MODE_8KB;
    return VSF_ERR_NONE;
}

bool f1cx00s_usb_irq(f1cx00s_usb_otg_t *usb)
{
    if (usb->irq.handler != NULL) {
        usb->irq.handler(usb->irq.param);
        return false;
    }
    return true;
}

__arm void USBOTG_Handler(void) 
{
    f1cx00s_usb_irq(&USB_OTG0);
}
