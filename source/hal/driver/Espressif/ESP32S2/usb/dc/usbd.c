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

#include "./usbd.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED

// for vk_dwcotg_dc_ip_info_t
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

static void __esp32s2_usbd_configure_pins(bool use_external_phy)
{
    if (use_external_phy) {
        __esp32s2_usb_extphy_io_init();
    } else {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }

    // connect dwcotg ip pins
    __esp32s2_usb_io_configure(USB_OTG_IDDIG_IN_IDX, 1);
    __esp32s2_usb_io_configure(USB_SRP_BVALID_IN_IDX, 1);
    __esp32s2_usb_io_configure(USB_OTG_VBUSVALID_IN_IDX, 1);
    __esp32s2_usb_io_configure(USB_OTG_AVALID_IN_IDX, 0);
}

vsf_err_t esp32s2_usbd_init(esp32s2_usb_t *dc, usb_dc_ip_cfg_t *cfg)
{
    dc->is_host = false;

    periph_module_reset(dc->param->periph_module);
    periph_module_enable(dc->param->periph_module);
    __esp32s2_usb_phy_init(false);
    __esp32s2_usbd_configure_pins(false);

    return __esp32s2_usb_init(dc, cfg->priority, cfg->irq_handler, cfg->param);
}

void esp32s2_usbd_fini(esp32s2_usb_t *dc)
{
}

void esp32s2_usbd_get_info(esp32s2_usb_t *dc, usb_dc_ip_info_t *info)
{
    const esp32s2_usb_const_t *param = dc->param;
    vk_dwcotg_dc_ip_info_t *dwcotg_info = (vk_dwcotg_dc_ip_info_t *)info;

    VSF_HAL_ASSERT(dwcotg_info != NULL);
    dwcotg_info->regbase = param->reg;
    dwcotg_info->ep_num = param->dc_ep_num;
    dwcotg_info->buffer_word_size = param->buffer_word_size;
    dwcotg_info->feature = param->feature;
}

void esp32s2_usbd_connect(esp32s2_usb_t *dc)
{
}

void esp32s2_usbd_disconnect(esp32s2_usb_t *dc)
{
}

void esp32s2_usbd_irq(esp32s2_usb_t *dc)
{
    VSF_HAL_ASSERT(false);
}

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_DCD_DWCOTG
