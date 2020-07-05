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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

uint_fast8_t __f1cx00s_usb_set_ep(f1cx00s_usb_otg_t *usb, uint_fast8_t ep)
{
    uint_fast8_t ep_orig = MUSB_BASE->Common.Index;
    MUSB_BASE->Common.Index = ep;
    return ep_orig;
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
    return VSF_ERR_NONE;
}

void f1cx00s_usb_irq(f1cx00s_usb_otg_t *usb)
{
//    if (usb->callback.irq_handler != NULL) {
//        usb->callback.irq_handler(usb->callback.param);
//    }
}
