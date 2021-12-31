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

extern vsf_err_t __aic8800_usb_init(aic8800_usb_t *usb, vsf_arch_prio_t priority,
                bool is_fs_phy, usb_ip_irqhandler_t handler, void *param);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t aic8800_usbd_init(aic8800_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    bool is_fs_phy = cfg->speed == USB_DC_SPEED_FULL;
    dc->is_host = false;
    return __aic8800_usb_init(dc, cfg->priority, is_fs_phy, cfg->irqhandler, cfg->param);
}

void aic8800_usbd_fini(aic8800_usb_t *dc)
{
}

static void __aic8800_usbd_phy_init(void *param)
{
    aic8800_usb_t *dc = (aic8800_usb_t *)param;
    volatile uint32_t *reg_base = dc->param->reg;

    // gpvndctl = 0x02440041;
    // while (!(gpvndctl & (1 <<27)));
    reg_base[13] = 0x02440041;
    while (!(reg_base[13] & (1 << 27)));
}

void aic8800_usbd_get_info(aic8800_usb_t *dc, usb_dc_ip_info_t *info)
{
    const aic8800_usb_const_t *param = dc->param;
    vk_dwcotg_dc_ip_info_t *dwcotg_info = (vk_dwcotg_dc_ip_info_t *)info;

    VSF_HAL_ASSERT(dwcotg_info != NULL);
    dwcotg_info->regbase = param->reg;
    dwcotg_info->ep_num = param->dc_ep_num;
    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = param->feature;

    dwcotg_info->vendor.param = dc;
    dwcotg_info->vendor.phy_init = __aic8800_usbd_phy_init;
}

void aic8800_usbd_connect(aic8800_usb_t *dc)
{
}

void aic8800_usbd_disconnect(aic8800_usb_t *dc)
{
}

void aic8800_usbd_irq(aic8800_usb_t *dc)
{
    VSF_HAL_ASSERT(false);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_DWCOTG
