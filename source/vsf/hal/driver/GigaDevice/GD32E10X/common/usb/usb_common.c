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

#include "./usb.h"

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __gd32e10x_usb_init(gd32e10x_usb_t *usb, vsf_arch_prio_t priority)
{
    const gd32e10x_usb_const_t *param = usb->param;
    volatile uint32_t *reg = param->reg;

    RCU_ADDAPB1EN |= RCU_ADDAPB1EN_CTCEN;
    CTC_CTL1 = CTC_REFSOURCE_USBSOF | CTL1_CKLIM(28) | (48000 - 1);
    CTC_CTL0 |= CTC_CTL0_AUTOTRIM | CTC_CTL0_CNTEN;
    RCU_ADDCTL |= RCU_ADDCTL_CK48MSEL;

    RCU_AHBEN |= RCU_AHBEN_USBFSEN;
    // USBFS_GAHBCS &= ~GINTEN;
    reg[2] &= ~1;

    if (priority >= 0) {
        NVIC_SetPriority(param->irq, priority);
        NVIC_EnableIRQ(param->irq);
    } else {
        NVIC_DisableIRQ(param->irq);
    }
    return VSF_ERR_NONE;
}

void __gd32e10x_usb_irq(gd32e10x_usb_t *usb)
{
    if (usb->callback.irq_handler != NULL) {
        usb->callback.irq_handler(usb->callback.param);
    }
}

#endif
