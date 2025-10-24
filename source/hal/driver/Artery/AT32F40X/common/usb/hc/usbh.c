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

//#include "../../common.h"
#include "./usbh.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

// for vk_dwcotg_hc_ip_info_t and USB_SPEED_XXX
#include "component/vsf_component.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority,
                bool is_fs_phy, usb_ip_irqhandler_t handler, void *param);
extern void __vsf_hw_usb_irq_enable(vsf_hw_usb_t *usb);
extern void __vsf_hw_usb_irq_disable(vsf_hw_usb_t *usb);

static uint_fast32_t __vsf_hw_usbh_workaround_reset_port(void *param);
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_dwcotg_hcd_workaround_t __vsf_hw_usbh_workaround = {
    .reset_port         = __vsf_hw_usbh_workaround_reset_port,
};

/*============================ IMPLEMENTATION ================================*/

static uint_fast32_t __vsf_hw_usbh_workaround_reset_port(void *param)
{
    return 200;
}

vsf_err_t vsf_hw_usbh_init(vsf_hw_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
    bool is_fs_phy = hc->param->speed == USB_SPEED_FULL;
    hc->is_host = true;

    return __vsf_hw_usb_init(hc, cfg->priority, is_fs_phy, cfg->irqhandler, cfg->param);
}

void vsf_hw_usbh_irq_enable(vsf_hw_usb_t *dc)
{
    __vsf_hw_usb_irq_enable(dc);
}

void vsf_hw_usbh_irq_disable(vsf_hw_usb_t *dc)
{
    __vsf_hw_usb_irq_disable(dc);
}

void vsf_hw_usbh_get_info(vsf_hw_usb_t *hc, usb_hc_ip_info_t *info)
{
    const vsf_hw_usb_const_t *param = hc->param;
    vk_dwcotg_hc_ip_info_t *dwcotg_info = (vk_dwcotg_hc_ip_info_t *)info;

    VSF_HAL_ASSERT(info != NULL);
    dwcotg_info->regbase = param->reg;
    dwcotg_info->ep_num = param->hc_ep_num;
    dwcotg_info->is_dma = true;
    dwcotg_info->use_as__vk_dwcotg_hw_info_t = param->use_as__vk_dwcotg_hw_info_t;
    dwcotg_info->workaround = (vk_dwcotg_hcd_workaround_t *)&__vsf_hw_usbh_workaround;
}

void vsf_hw_usbh_irq(vsf_hw_usb_t *dc)
{
    VSF_HAL_ASSERT(false);
}

#endif
