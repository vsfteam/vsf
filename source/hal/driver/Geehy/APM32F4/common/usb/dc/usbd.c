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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority,
                bool is_fs_phy, usb_ip_irqhandler_t handler, void *param);
extern void __vsf_hw_usb_irq_enable(vsf_hw_usb_t *usb);
extern void __vsf_hw_usb_irq_disable(vsf_hw_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usbd_init(vsf_hw_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    bool is_fs_phy = cfg->speed == USB_DC_SPEED_FULL;
    dc->is_host = false;
    return __vsf_hw_usb_init(dc, cfg->priority, is_fs_phy, cfg->irqhandler, cfg->param);
}

void vsf_hw_usbd_fini(vsf_hw_usb_t *dc)
{
}

void vsf_hw_usbd_irq_enable(vsf_hw_usb_t *dc)
{
    __vsf_hw_usb_irq_enable(dc);
}

void vsf_hw_usbd_irq_disable(vsf_hw_usb_t *dc)
{
    __vsf_hw_usb_irq_disable(dc);
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
}

void vsf_hw_usbd_connect(vsf_hw_usb_t *dc)
{
}

void vsf_hw_usbd_disconnect(vsf_hw_usb_t *dc)
{
}

void vsf_hw_usbd_irq(vsf_hw_usb_t *dc)
{
    VSF_HAL_ASSERT(false);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_DWCOTG
