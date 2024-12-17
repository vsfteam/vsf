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

#include "./usb.h"

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

#include "./usb_priv.h"

#include "../vendor/Include/gd32h7xx_pmu.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority)
{
    const vsf_hw_usb_const_t *param = usb->param;

    PMU_CTL2 |= PMU_CTL2_USBSEN;
    PMU_CTL2 |= PMU_CTL2_VUSB33DEN;
    while (!(PMU_CTL2 & PMU_CTL2_USB33RF));
    vsf_hw_peripheral_enable(param->en);

    if (priority >= 0) {
        NVIC_SetPriority(param->irq, priority);
        NVIC_EnableIRQ(param->irq);
    } else {
        NVIC_DisableIRQ(param->irq);
    }
    return VSF_ERR_NONE;
}

void __vsf_hw_usb_irq(vsf_hw_usb_t *usb)
{
    if (usb->callback.irqhandler != NULL) {
        usb->callback.irqhandler(usb->callback.param);
    }
}

#endif
