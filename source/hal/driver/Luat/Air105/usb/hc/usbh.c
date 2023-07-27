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

#include "../../__common.h"
#include "./usbh.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_HAL_USE_USBH == ENABLED

#include "component/usb/driver/otg/musb/fdrc/vsf_musb_fdrc_hw.h"

#ifdef FALSE
#   undef FALSE
#endif
#ifdef TRUE
#   undef TRUE
#endif
#include "air105.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __vsf_hw_usb_init_interrupt(vsf_hw_usb_t *usb, vsf_arch_prio_t priority);
extern void __vsf_hw_usb_irq(vsf_hw_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usbh_init(vsf_hw_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
    int idx = hc->param->idx;
    JL_USB_TypeDef *usb_reg = (JL_USB_TypeDef *)hc->param->reg.usb_reg;
    JL_USB_IO_TypeDef *usb_io_reg = (JL_USB_IO_TypeDef *)hc->param->reg.io_reg;

#if VSF_HAL_USE_USBD == ENABLED
    // hc->is_host only exists when both host and device modes are enabled
    hc->is_host = true;
#endif
    hc->callback.irqhandler = cfg->irqhandler;
    hc->callback.param = cfg->param;

    __vsf_hw_usb_fifo_reset(hc);
    __vsf_hw_usb_init_interrupt(hc, cfg->priority);

    return VSF_ERR_NONE;
}

void vsf_hw_usbh_get_info(vsf_hw_usb_t *hc, usb_hc_ip_info_t *info)
{
    VSF_ASSERT(info != NULL);
    vsf_hw_usb_const_t *musb_param = hc->param;
    vk_musb_fdrc_hc_ip_info_t *musb_hw_info = (vk_musb_fdrc_hc_ip_info_t *)info;

    musb_hw_info->Common = musb_param->reg.sie_reg;
    musb_hw_info->EP = musb_param->reg.ep_reg;
    musb_hw_info->FIFO = NULL;
    musb_hw_info->param = hc;
    musb_hw_info->get_ep_reg = musb_param->fn.get_ep_reg;
    musb_hw_info->set_ep_fifo = musb_param->fn.set_ep_fifo;
    musb_hw_info->write_ep_fifo = musb_param->fn.write_ep_fifo;
    musb_hw_info->ep_num = vsf_hw_usbh_ep_number;
    musb_hw_info->is_dma = vsf_hw_usbh_ep_is_dma;
}

void vsf_hw_usbh_irq(vsf_hw_usb_t *hc)
{
    __vsf_hw_usb_irq(hc);
}

#endif      // VSF_HAL_USE_USBH
