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

#include "./usb.h"

#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED

#include "esp_intr_alloc.h"
#include "esp_rom_gpio.h"
#include "hal/usb_hal.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __esp32s2_usb_irqhandler(void *param)
{
    esp32s2_usb_t *usb = param;
    __vsf_arch_irq_enter();
        if (usb->callback.irq_handler != NULL) {
            usb->callback.irq_handler(usb->callback.param);
        }
    __vsf_arch_irq_leave();
}

void __esp32s2_usb_io_configure(int func, int value)
{
    value = value ? GPIO_MATRIX_CONST_ONE_INPUT : GPIO_MATRIX_CONST_ZERO_INPUT;

    esp_rom_gpio_pad_select_gpio(value);
        esp_rom_gpio_connect_in_signal(value, func, false);
    esp_rom_gpio_pad_unhold(value);
}

void __esp32s2_usb_extphy_io_init(void)
{
    // not supported yet
    // TODO: configure VP/VM/RCV/OEN/VPO/VMO
    VSF_HAL_ASSERT(false);
}

void __esp32s2_usb_phy_init(bool use_external_phy)
{
    usb_hal_context_t hal = {
        .use_external_phy = use_external_phy,
    };
    usb_hal_init(&hal);
}

vsf_err_t __esp32s2_usb_init(esp32s2_usb_t *usb, vsf_arch_prio_t priority, usb_ip_irq_handler_t handler, void *param)
{
    usb->callback.irq_handler = handler;
    usb->callback.param = param;
    esp_intr_alloc(usb->param->intr_source, ESP_INTR_FLAG_LOWMED, (intr_handler_t)__esp32s2_usb_irqhandler, usb, (intr_handle_t *)&usb->intr_handle);
    return VSF_ERR_NONE;
}

#endif
