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
#include "./usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_DCD_DWCOTG == ENABLED

// for vsf_dwcotg_dc_ip_info_t
#include "component/usb/driver/otg/dwcotg/vsf_dwcotg_dcd.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t gd32vf103_usb_init(gd32vf103_usb_t *usb, vsf_arch_prio_t priority);
extern void gd32vf103_usb_irq(gd32vf103_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t gd32vf103_usbd_init(gd32vf103_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    dc->is_host = false;
    dc->callback.irq_handler = cfg->irq_handler;
    dc->callback.param = cfg->param;
    return gd32vf103_usb_init(dc, cfg->priority);
}

void gd32vf103_usbd_fini(gd32vf103_usb_t *dc)
{
}

void gd32vf103_usbd_get_info(gd32vf103_usb_t *dc, usb_dc_ip_info_t *info)
{
    const gd32vf103_usb_const_t *param = dc->param;
    vsf_dwcotg_dc_ip_info_t *dwcotg_info = (vsf_dwcotg_dc_ip_info_t *)info;

    VSF_HAL_ASSERT(dwcotg_info != NULL);
    dwcotg_info->regbase = dc->param->reg;
    dwcotg_info->ep_num = param->ep_num;
    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = param->feature;
}

void gd32vf103_usbd_connect(gd32vf103_usb_t *dc)
{
}

void gd32vf103_usbd_disconnect(gd32vf103_usb_t *dc)
{
}

void gd32vf103_usbd_irq(gd32vf103_usb_t *dc)
{
    gd32vf103_usb_irq(dc);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_DCD_DWCOTG
