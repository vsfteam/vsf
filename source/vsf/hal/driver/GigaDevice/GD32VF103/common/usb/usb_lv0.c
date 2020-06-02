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

//#include "../common.h"
#include "./usb.h"

// for usb_device_speed_t
#include "component/usb/common/usb_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

// TODO: add gd32vf103_usbh_irq
#if VSF_USE_USB_DEVICE == ENABLED || VSF_USE_USB_HOST == ENABLED
#define __USB_OTG_IMPLEMENT(__N, __VALUE)                                       \
static const gd32vf103_usb_const_t __USB_OTG##__N##_const = {                   \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
gd32vf103_usb_t USB_OTG##__N##_IP = {                                           \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
ROOT void USB_OTG##__N##_IRQHandler(void)                                       \
{                                                                               \
    if (USB_OTG##__N##_IP.is_host) {                                            \
        gd32vf103_usbh_irq(&USB_OTG##__N##_IP);                                 \
    } else {                                                                    \
        gd32vf103_usbd_irq(&USB_OTG##__N##_IP);                                 \
    }                                                                           \
}
#else
#define __USB_OTG_IMPLEMENT(__N, __VALUE)                                       \
static const gd32vf103_usb_const_t __USB_OTG##__N##_const = {                   \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
gd32vf103_usb_t USB_OTG##__N##_IP = {                                           \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
void USB_OTG##__N##_IRQHandler(void)                                            \
{                                                                               \
    if (USB_OTG##__N##_IP.is_host) {                                            \
        gd32vf103_usbh_irq(&USB_OTG##__N##_IP);                                 \
    } else {                                                                    \
        gd32vf103_usbd_irq(&USB_OTG##__N##_IP);                                 \
    }                                                                           \
}
#endif

#define _USB_OTG_IMPLEMENT(__N, __VALUE)    __USB_OTG_IMPLEMENT(__N, __VALUE)
#define USB_OTG_IMPLEMENT(__N, __VALUE)     _USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK(gd32vf103_usbd_irq)
void gd32vf103_usbd_irq(gd32vf103_usb_t *dc) { ASSERT(false); }
WEAK(gd32vf103_usbh_irq)
void gd32vf103_usbh_irq(gd32vf103_usb_t *dc) { ASSERT(false); }

REPEAT_MACRO(USB_OTG_COUNT, USB_OTG_IMPLEMENT, NULL)
