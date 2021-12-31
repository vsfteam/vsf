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

#include "cmem7_gpio.h"
#include "cmem7_misc.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __cmem7_usb_init(cmem7_usb_t *usb, vsf_arch_prio_t priority,
                bool is_fs_phy, usb_ip_irqhandler_t handler, void *param);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t cmem7_usbd_init(cmem7_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    bool is_fs_phy = cfg->speed == USB_DC_SPEED_FULL;
    dc->is_host = false;
    return __cmem7_usb_init(dc, cfg->priority, is_fs_phy, cfg->irqhandler, cfg->param);
}

void cmem7_usbd_fini(cmem7_usb_t *dc)
{
}

static void __cmem7_usbd_phy_init(void *param)
{
//	// 2.13	usb_phy_control in glb_ctrl_spec.doc, GLBC_RW_REG_4, Address: 0x41007410
//	GLOBAL_CTRL->USB_PHY_CTRL_b.CKISEL = 1;		// 0:Crystal   1:PLL
//	udelay(0x100);
//	GLOBAL_CTRL->USB_PHY_CTRL_b.RSTPRT = 1;
//	udelay(0x100);
//	GLOBAL_CTRL->USB_PHY_CTRL_b.RSTPRT = 0;
//	udelay(0x100);
}

void cmem7_usbd_get_info(cmem7_usb_t *dc, usb_dc_ip_info_t *info)
{
    const cmem7_usb_const_t *param = dc->param;
    vk_dwcotg_dc_ip_info_t *dwcotg_info = (vk_dwcotg_dc_ip_info_t *)info;

    VSF_HAL_ASSERT(dwcotg_info != NULL);
    dwcotg_info->regbase = param->reg;
    dwcotg_info->ep_num = param->dc_ep_num;
    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = param->feature;

    dwcotg_info->vendor.param = dc;
    dwcotg_info->vendor.phy_init = __cmem7_usbd_phy_init;
}

void cmem7_usbd_connect(cmem7_usb_t *dc)
{
}

void cmem7_usbd_disconnect(cmem7_usb_t *dc)
{
}

void cmem7_usbd_irq(cmem7_usb_t *dc)
{
    VSF_HAL_ASSERT(false);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_DWCOTG
