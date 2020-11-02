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

#include "../common.h"
#include "./usb.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
#define ____USB_OTG_IMPLEMENT(__N, __VALUE)                                     \
static const mt071_usb_const_t __USB_OTG##__N##_const = {                       \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
mt071_usb_t USB_OTG##__N##_IP = {                                               \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
ROOT void USB_OTG##__N##_IRQHandler(void)                                       \
{                                                                               \
    if (USB_OTG##__N##_IP.is_host) {                                            \
        mt071_usbh_irq(&USB_OTG##__N##_IP);                                     \
    } else {                                                                    \
        mt071_usbd_irq(&USB_OTG##__N##_IP);                                     \
    }                                                                           \
}
#elif VSF_HAL_USE_USBD == ENABLED
#define ____USB_OTG_IMPLEMENT(__N, __VALUE)                                     \
static const mt071_usb_const_t __USB_OTG##__N##_const = {                       \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
mt071_usb_t USB_OTG##__N##_IP = {                                               \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
ROOT void USB_OTG##__N##_IRQHandler(void)                                       \
{                                                                               \
    mt071_usbd_irq(&USB_OTG##__N##_IP);                                         \
}
#elif VSF_HAL_USE_USBH == ENABLED
#define ____USB_OTG_IMPLEMENT(__N, __VALUE)                                     \
static const mt071_usb_const_t __USB_OTG##__N##_const = {                       \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
mt071_usb_t USB_OTG##__N##_IP = {                                               \
    .param = &__USB_OTG##__N##_const,                                           \
};                                                                              \
ROOT void USB_OTG##__N##_IRQHandler(void)                                       \
{                                                                               \
    mt071_usbh_irq(&USB_OTG##__N##_IP);                                         \
}
#endif

#define __USB_OTG_IMPLEMENT(__N, __VALUE)   ____USB_OTG_IMPLEMENT(__N, __VALUE)
#define USB_OTG_IMPLEMENT(__N, __VALUE)     __USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK(mt071_usbd_irq)
void mt071_usbd_irq(mt071_usb_t *dc) { ASSERT(false); }
WEAK(mt071_usbh_irq)
void mt071_usbh_irq(mt071_usb_t *dc) { ASSERT(false); }

REPEAT_MACRO(USB_OTG_COUNT, USB_OTG_IMPLEMENT, NULL)

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
