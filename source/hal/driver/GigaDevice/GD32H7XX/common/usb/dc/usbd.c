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
#include "../usb_priv.h"

#include "component/usb/driver/otg/dwcotg/dwcotg_regs.h"

/*============================ MACROS ========================================*/

// copied from drv_usb_regs.h
#define GUSBCS_EMBPHY_FS          (1 << 6)              /*!< embedded FS PHY selected */
#define GUSBCS_EMBPHY_HS          (1 << 5)              /*!< embedded HS PHY selected */

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vsf_hw_usb_init(vsf_hw_usb_t *usb, vsf_arch_prio_t priority);
extern void __vsf_hw_usb_irq_enable(vsf_hw_usb_t *usb);
extern void __vsf_hw_usb_irq_disable(vsf_hw_usb_t *usb);
extern void __vsf_hw_usb_irq(vsf_hw_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

static void __vsf_hw_usbd_phy_init(void *param, vk_dwcotg_dcd_param_t *dcd_param)
{
    vsf_hw_usb_t *dc = (vsf_hw_usb_t *)param;
    struct dwcotg_core_global_regs_t *global_regs = dc->param->reg;

    if (USB_SPEED_FULL == dcd_param->speed) {
        global_regs->gusbcfg |= GUSBCS_EMBPHY_FS;
    } else if (USB_SPEED_HIGH == dcd_param->speed) {
        global_regs->gusbcfg |= GUSBCS_EMBPHY_HS;
    } else {
        VSF_HAL_ASSERT(false);
    }
    global_regs->gotgctl |= USB_OTG_GOTGCTL_BVALOEN | USB_OTG_GOTGCTL_BVALOVAL;
    // PWRCLKCTL = 0
    ((uint32_t *)global_regs)[0x0E00 / 4] = 0;
    // Embedded PHY Power On
    global_regs->gccfg |= USB_OTG_GCCFG_PWRDWN;
}

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
    dwcotg_info->ep_num = param->ep_num;
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
