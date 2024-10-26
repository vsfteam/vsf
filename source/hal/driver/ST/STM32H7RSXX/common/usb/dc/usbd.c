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

#include "./usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED

// for vk_dwcotg_dc_ip_info_t
#include "component/vsf_component.h"

#include "hal/driver/ST/STM32H7RSXX/common/vendor/cmsis_device_h7rs/Include/stm32h7rsxx.h"
#include "../usb_priv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority);
extern void __vsf_hw_usb_irq(vsf_hw_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usbd_init(vsf_hw_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    dc->is_host = false;
    dc->callback.irqhandler = cfg->irqhandler;
    dc->callback.param = cfg->param;
    return __vsf_hw_usb_init(dc, cfg->priority);
}

void vsf_hw_usbd_fini(vsf_hw_usb_t *dc)
{
}

static void __vsf_hw_usbd_phy_init(void *usb, vk_dwcotg_dcd_param_t *param)
{
    vsf_hw_usb_t *dc = (vsf_hw_usb_t *)usb;

    VSF_HAL_ASSERT(!param->ulpi_en && param->utmi_en);
    USB_OTG_GlobalTypeDef *USBx = (USB_OTG_GlobalTypeDef *)dc->param->reg;
    USBx->GUSBCFG &= ~USB_OTG_GUSBCFG_TSDPS;

    if ((USBx->GUSBCFG & USB_OTG_GUSBCFG_PHYSEL) == 0U) {
        /* Disable USB PHY pulldown resistors */
        USBx->GCCFG &= ~USB_OTG_GCCFG_PULLDOWNEN;
    }

    if ((USBx->GUSBCFG & USB_OTG_GUSBCFG_PHYSEL) == 0U) {
        USBx->GCCFG |= USB_OTG_GCCFG_VBVALEXTOEN;
        USBx->GCCFG |= USB_OTG_GCCFG_VBVALOVAL;
    } else {
        USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
        USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
    }
}

void vsf_hw_usbd_get_info(vsf_hw_usb_t *dc, usb_dc_ip_info_t *info)
{
    const vsf_hw_usb_const_t *param = dc->param;
    vk_dwcotg_dc_ip_info_t *dwcotg_info = (vk_dwcotg_dc_ip_info_t *)info;

    VSF_HAL_ASSERT(dwcotg_info != NULL);
    dwcotg_info->regbase = param->reg;
    dwcotg_info->ep_num = param->dc_ep_num;
    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = param->feature;
    dwcotg_info->vendor.param = dc;
    dwcotg_info->vendor.phy_init = __vsf_hw_usbd_phy_init;
}

void vsf_hw_usbd_connect(vsf_hw_usb_t *dc)
{
}

void vsf_hw_usbd_disconnect(vsf_hw_usb_t *dc)
{
}

void vsf_hw_usbd_irq(vsf_hw_usb_t *dc)
{
    __vsf_hw_usb_irq(dc);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_DWCOTG
