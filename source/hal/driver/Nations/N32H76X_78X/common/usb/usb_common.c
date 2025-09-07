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

#include "hal/vsf_hal.h"
#include "hal/driver/vendor_driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __vsf_hw_usb_fini(vsf_hw_usb_t *usb)
{
    const vsf_hw_usb_const_t *usb_hw_param = usb->param;
    USB_Wrapper_Registers *wrap_reg = (USB_Wrapper_Registers *)usb_hw_param->wrap_reg;

    wrap_reg->WRPCFG &= ~USBHS_WRPCFG_PLLEN;
    vsf_hw_power_disable(usb_hw_param->pwr);
    vsf_hw_peripheral_disable(usb_hw_param->en);
}

vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority,
            bool is_fs_phy, usb_ip_irqhandler_t handler, void *param)
{
    const vsf_hw_usb_const_t *usb_hw_param = usb->param;
    USB_Wrapper_Registers *wrap_reg = (USB_Wrapper_Registers *)usb_hw_param->wrap_reg;
    uint32_t tmp32, refclk;

    usb->callback.irqhandler = handler;
    usb->callback.param = param;

    vsf_hw_peripheral_enable(usb_hw_param->en);
    vsf_hw_power_enable(usb_hw_param->pwr);

    tmp32 = wrap_reg->WRPCFG & ~USBHS_WRPCFG_PHYCLKSEL;
    refclk = vsf_hw_clk_get_freq_hz(&VSF_HW_CLK_USBREF);
    switch (refclk) {
    case 10000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_10M;    break;
    case 12000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_12M;    break;
    case 25000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_25M;    break;
    case 30000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_30M;    break;
    case 19200000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_19_2M;  break;
    case 24000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_24M;    break;
    case 27000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_27M;    break;
    case 40000000:  tmp32 |= USBHS_WRPCFG_PHYCLKSEL_40M;    break;
    default:        return VSF_ERR_FAIL;
    }
    tmp32 |= USBHS_WRPCFG_PLLEN;
    wrap_reg->WRPCFG = tmp32;

    // PWRCTRL = 0;
    ((uint32_t *)usb_hw_param->reg)[0xE00 >> 2] = 0;

    NVIC_SetPriority(usb_hw_param->irq, priority);
    NVIC_ClearPendingIRQ(usb_hw_param->irq);
    NVIC_EnableIRQ(usb_hw_param->irq);
    return VSF_ERR_NONE;
}

#endif
