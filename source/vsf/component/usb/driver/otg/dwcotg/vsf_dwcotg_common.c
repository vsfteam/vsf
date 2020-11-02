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

#include "component/usb/vsf_usb_cfg.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED)

#include "./vsf_dwcotg_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vk_dwcotg_reset(vk_dwcotg_t *dwcotg)
{
    dwcotg->reg.global_regs->grstctl |= USB_OTG_GRSTCTL_CSRST;
    while (dwcotg->reg.global_regs->grstctl & USB_OTG_GRSTCTL_CSRST);
    while ((dwcotg->reg.global_regs->grstctl & USB_OTG_GRSTCTL_AHBIDL) == 0U);
}

void vk_dwcotg_phy_init(vk_dwcotg_t *dwcotg,
                        const vk_dwcotg_param_t *param,
                        vk_dwcotg_hw_info_t *hw_info)
{
    struct dwcotg_core_global_regs_t *global_regs = dwcotg->reg.global_regs;

    VSF_USB_ASSERT(hw_info->speed >= param->speed);
    if (param->ulpi_en) {
        VSF_USB_ASSERT(hw_info->ulpi_en);

        global_regs->gccfg &= USB_OTG_GCCFG_PWRDWN;

        // init ulpi
        global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
        // select vbus source
        global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
        if (param->vbus_en) {
            global_regs->gusbcfg |= USB_OTG_GUSBCFG_ULPIEVBUSD;
        }

        __vk_dwcotg_reset(dwcotg);
    } else if (param->utmi_en) {
        VSF_USB_ASSERT(hw_info->utmi_en);

        global_regs->gccfg &= USB_OTG_GCCFG_PWRDWN;

        // init utmi
        global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_PHYSEL | USB_OTG_GUSBCFG_ULPI_UTMI_SEL |
                                        USB_OTG_GUSBCFG_PHYIF | USB_OTG_GUSBCFG_ULPIFSLS);
        // Select vbus source
        global_regs->gusbcfg &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);

        __vk_dwcotg_reset(dwcotg);
    } else {
        // init embedded phy
        global_regs->gusbcfg |= USB_OTG_GUSBCFG_PHYSEL;

        __vk_dwcotg_reset(dwcotg);

        global_regs->gccfg |= USB_OTG_GCCFG_PWRDWN | USB_OTG_GCCFG_VBUSACEN | USB_OTG_GCCFG_VBUSBCEN;
        if (!param->vbus_en) {
            global_regs->gccfg |= USB_OTG_GCCFG_VBDEN;
        }
    }
}

#endif
