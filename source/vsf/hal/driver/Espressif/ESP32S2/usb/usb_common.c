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

vsf_err_t __esp32s2_usb_init(esp32s2_usb_t *usb, vsf_arch_prio_t priority, usb_ip_irq_handler_t handler, void *param)
{
    usb->callback.irq_handler = handler;
    usb->callback.param = param;
    esp_intr_alloc(usb->param->intr_source, ESP_INTR_FLAG_LOWMED, (intr_handler_t)__esp32s2_usb_irqhandler, usb, (intr_handle_t *)&usb->intr_handle);
    return VSF_ERR_NONE;
}

#endif
