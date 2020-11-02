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

#include "../../common.h"
#include "./usbd.h"

#if VSF_HAL_USE_USBD == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __mt071_usb_init_interrupt(mt071_usb_t *usb, vsf_arch_prio_t priority);
extern void __mt071_usb_irq(mt071_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t mt071_usbd_init(mt071_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
#if VSF_HAL_USE_USBH == ENABLED
    // hc->is_host only exists when both host and device modes are enabled
    dc->is_host = false;
#endif
    dc->callback.irq_handler = cfg->irq_handler;
    dc->callback.param = cfg->param;

    // TODO: use pm to config clock
    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_USB;
    RCC->AHBCLKCTRL0_SET = SCLK_USB_MSK;

    __mt071_usb_init_interrupt(dc, cfg->priority);
    return VSF_ERR_NONE;
}

void mt071_usbd_fini(mt071_usb_t *dc)
{
    NVIC_DisableIRQ(dc->param->irq);
    // TODO: use pm to config clock
    RCC->AHBCLKCTRL0_CLR = SCLK_USB_MSK;
}

void mt071_usbd_get_info(mt071_usb_t *dc, usb_dc_ip_info_t *info)
{
    ASSERT(info != NULL);
    info->regbase = dc->param->reg;
    info->ep_num = mt071_usbd_ep_number;
    info->is_dma = mt071_usbd_ep_is_dma;
}

void mt071_usbd_connect(mt071_usb_t *dc)
{
    RCC->USBCTRL |= 0xF0;
//    RCC->PRESETCTRL0_SET = AHBCLK_USB_msk;
}

void mt071_usbd_disconnect(mt071_usb_t *dc)
{
    RCC->USBCTRL &= 0xFFFFFF0F;
    RCC->USBCTRL |= 0x100;
//    RCC->PRESETCTRL0_CLR = AHBCLK_USB_msk;
}

void mt071_usbd_irq(mt071_usb_t *dc)
{
    __mt071_usb_irq(dc);
}

#endif      // VSF_HAL_USE_USBD
