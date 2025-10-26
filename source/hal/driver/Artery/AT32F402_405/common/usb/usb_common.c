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

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED)

#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority,
            bool is_fs_phy, usb_ip_irqhandler_t handler, void *param)
{
    const vsf_hw_usb_const_t *usb_hw_param = usb->param;

    usb->callback.irqhandler = handler;
    usb->callback.param = param;

    if (vsf_hw_clk_get_freq_hz(usb_hw_param->clk) != usb_hw_param->clk_freq_required) {
        VSF_HAL_ASSERT(false);
        return VSF_ERR_INVALID_PARAMETER;
    }

    vsf_hw_peripheral_enable(usb_hw_param->en);
    NVIC_SetPriority(usb_hw_param->irq, priority);
    NVIC_ClearPendingIRQ(usb_hw_param->irq);
    NVIC_EnableIRQ(usb_hw_param->irq);
    return VSF_ERR_NONE;
}

void __vsf_hw_usb_irq_enable(vsf_hw_usb_t *usb)
{
    NVIC_EnableIRQ(usb->param->irq);
}

void __vsf_hw_usb_irq_disable(vsf_hw_usb_t *usb)
{
    NVIC_DisableIRQ(usb->param->irq);
}

#endif
