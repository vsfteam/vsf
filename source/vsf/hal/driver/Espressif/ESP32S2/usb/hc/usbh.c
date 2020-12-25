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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

// for vk_dwcotg_hc_ip_info_t and USB_SPEED_XXX
#include "component/vsf_component.h"

#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include "soc/usb_periph.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t __esp32s2_usb_init(esp32s2_usb_t *usb, vsf_arch_prio_t priority,
                usb_ip_irq_handler_t handler, void *param);
extern void __esp32s2_usb_extphy_io_init(void);
extern void __esp32s2_usb_io_configure(int func, int value);
extern void __esp32s2_usb_phy_init(bool use_external_phy);

/*============================ IMPLEMENTATION ================================*/

static void __esp32s2_usbh_configure_pins(bool use_external_phy)
{
    if (use_external_phy) {
        __esp32s2_usb_extphy_io_init();
    } else {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }

    // connect dwcotg ip pins
    __esp32s2_usb_io_configure(USB_OTG_IDDIG_IN_IDX, 0);
    __esp32s2_usb_io_configure(USB_SRP_BVALID_IN_IDX, 0);
    __esp32s2_usb_io_configure(USB_OTG_VBUSVALID_IN_IDX, 0);
    __esp32s2_usb_io_configure(USB_OTG_AVALID_IN_IDX, 1);
}

vsf_err_t esp32s2_usbh_init(esp32s2_usb_t *hc, usb_hc_ip_cfg_t *cfg)
{
    hc->is_host = true;

    periph_module_reset(hc->param->periph_module);
    periph_module_enable(hc->param->periph_module);
    __esp32s2_usb_phy_init(false);
    __esp32s2_usbh_configure_pins(false);

    return __esp32s2_usb_init(hc, cfg->priority, cfg->irq_handler, cfg->param);
}

void esp32s2_usbh_get_info(esp32s2_usb_t *hc, usb_hc_ip_info_t *info)
{
    const esp32s2_usb_const_t *param = hc->param;
    vk_dwcotg_hc_ip_info_t *dwcotg_info = (vk_dwcotg_hc_ip_info_t *)info;

    ASSERT(info != NULL);
    dwcotg_info->regbase = hc->param->reg;
    dwcotg_info->ep_num = hc->param->hc_ep_num;
    dwcotg_info->is_dma = false;

    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = 0;
    dwcotg_info->speed = USB_SPEED_FULL;
    dwcotg_info->dma_en = true;
}

void esp32s2_usbh_irq(esp32s2_usb_t *hc)
{
    VSF_HAL_ASSERT(false);
}

#endif
