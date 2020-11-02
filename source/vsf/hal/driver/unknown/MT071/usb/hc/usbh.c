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
#include "./usbh.h"

#if VSF_HAL_USE_USBH == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __mt071_usb_init_interrupt(mt071_usb_t *usb, vsf_arch_prio_t priority);
extern void __mt071_usb_irq(mt071_usb_t *usb);

/*============================ IMPLEMENTATION ================================*/

vsf_err_t mt071_usbh_init(mt071_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
#if VSF_HAL_USE_USBD == ENABLED
    // hc->is_host only exists when both host and device modes are enabled
    hc->is_host = true;
#endif
    hc->callback.irq_handler = cfg->irq_handler;
    hc->callback.param = cfg->param;

    // TODO: use pm to config clock
    RCC->USBCLKSEL = 0;
    RCC->USBCLKUEN = 0;
    RCC->USBCLKUEN = 1;
    RCC->PDRUNCFG &= ~RCC_PDRUNCFG_USB;
    RCC->AHBCLKCTRL0_SET = SCLK_USB_MSK;

    // no info in doc, do same as sample code in official sdk
    RCC->USBCTRL = 0x1170;

    __mt071_usb_init_interrupt(hc, cfg->priority);
    return VSF_ERR_NONE;
}

void mt071_usbh_get_info(mt071_usb_t *hc, usb_hc_ip_info_t *info)
{
    ASSERT(info != NULL);
    info->regbase = hc->param->reg;
    info->ep_num = mt071_usbh_ep_number;
    info->is_dma = mt071_usbh_ep_is_dma;
}

void mt071_usbh_irq(mt071_usb_t *hc)
{
    __mt071_usb_irq(hc);
}

#endif      // VSF_HAL_USE_USBH
