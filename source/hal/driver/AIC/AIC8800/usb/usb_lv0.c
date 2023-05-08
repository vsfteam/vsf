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

#include "./usb.h"

#if     (VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_DCD_DWCOTG == ENABLED)   \
    ||  (VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED)

#include "../vendor/plf/aic8800/src/driver/chip.h"

// for usb_device_speed_t
#include "component/vsf_component.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __USB_OTG_IMPLEMENT(__N, __VALUE)                                       \
static const aic8800_usb_const_t __USB_OTG##__N##_const = {                     \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
aic8800_usb_t USB_OTG##__N##_IP = {                                             \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
ROOT ISR(USB_OTG##__N##_IRQHandler)                                             \
{                                                                               \
    aic8800_usb_t *usb = &USB_OTG##__N##_IP;                                    \
    if (usb->callback.irqhandler != NULL) {                                     \
        usb->callback.irqhandler(usb->callback.param);                          \
    }                                                                           \
}

#define _USB_OTG_IMPLEMENT(__N, __VALUE)    __USB_OTG_IMPLEMENT(__N, __VALUE)
#define USB_OTG_IMPLEMENT(__N, __VALUE)     _USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wint-conversion"
#endif

VSF_MREPEAT(USB_OTG_COUNT, USB_OTG_IMPLEMENT, NULL)

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif
