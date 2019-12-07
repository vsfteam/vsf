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

//#include "../../common.h"
#include "./usbh.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_HCD_DWCOTG == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t gd32vf103_usb_init(gd32vf103_usb_t *usb, vsf_arch_prio_t priority);
extern void gd32vf103_usb_irq(gd32vf103_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t gd32vf103_usbh_init(gd32vf103_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
    hc->is_host = true;
    hc->callback.irq_handler = cfg->irq_handler;
    hc->callback.param = cfg->param;
    return gd32vf103_usb_init(hc, cfg->priority);
}

void gd32vf103_usbh_get_info(gd32vf103_usb_t *hc, usb_hc_ip_info_t *info)
{
    const gd32vf103_usb_const_t *param = dc->param;
    vsf_dwcotg_hc_ip_info_t *dwcotg_info = (vsf_dwcotg_hc_ip_info_t *)info;

    VSF_HAL_ASSERT(info != NULL);
    info->regbase = hc->param->reg;
    info->ep_num = param->ep_num;
    info->is_dma = true;
}

void gd32vf103_usbh_irq(gd32vf103_usb_t *hc)
{
    gd32vf103_usb_irq(hc);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_HCD_DWCOTG
