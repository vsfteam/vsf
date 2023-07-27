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
#include "./usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_HAL_USE_USBD == ENABLED

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

extern void __vsf_hw_usb_fifo_reset(vsf_hw_usb_t *usb);
extern vsf_err_t __vsf_hw_usb_init_interrupt(vsf_hw_usb_t *usb, vsf_arch_prio_t priority);
extern void __vsf_hw_usb_irq(vsf_hw_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_hw_usbd_init(vsf_hw_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
#if VSF_HAL_USE_USBH == ENABLED
    // hc->is_host only exists when both host and device modes are enabled
    dc->is_host = false;
#endif
    dc->callback.irqhandler = cfg->irqhandler;
    dc->callback.param = cfg->param;

    __vsf_hw_usb_fifo_reset(dc);
    __vsf_hw_usb_init_interrupt(dc, cfg->priority);

    return VSF_ERR_NONE;
}

void vsf_hw_usbd_fini(vsf_hw_usb_t *dc)
{

}

void vsf_hw_usbd_get_info(vsf_hw_usb_t *dc, usb_dc_ip_info_t *info)
{
    VSF_HAL_ASSERT(info != NULL);
    const vsf_hw_usb_const_t *musb_param = dc->param;
    vk_musb_fdrc_dc_ip_info_t *musb_hw_info = (vk_musb_fdrc_dc_ip_info_t *)info;

    musb_hw_info->Common = (void *)((uintptr_t)musb_param->reg + 0x00);
    musb_hw_info->EP = (void *)((uintptr_t)musb_param->reg + 0x10);
    musb_hw_info->FIFO = (void *)((uintptr_t)musb_param->reg + 0x20);
    musb_hw_info->ep_num = vsf_hw_usbd_ep_number;
    musb_hw_info->is_dma = vsf_hw_usbd_ep_is_dma;
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

#endif      // VSF_HAL_USE_USBD
