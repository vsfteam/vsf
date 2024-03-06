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

#include "../__common.h"
#include "./usb.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
#   define __USB_OTG_IMPLEMENT_IRQHANDLER(__N)                                  \
    if (USB_OTG##__N##_IP.is_host) {                                            \
        vsf_hw_usbh_irq(&USB_OTG##__N##_IP);                                    \
    } else {                                                                    \
        vsf_hw_usbd_irq(&USB_OTG##__N##_IP);                                    \
    }
#elif VSF_HAL_USE_USBD == ENABLED
#   define __USB_OTG_IMPLEMENT_IRQHANDLER(__N)                                  \
        vsf_hw_usbd_irq(&USB_OTG##__N##_IP);
#elif VSF_HAL_USE_USBH == ENABLED
#   define __USB_OTG_IMPLEMENT_IRQHANDLER(__N)                                  \
        vsf_hw_usbh_irq(&USB_OTG##__N##_IP);
#endif

#define ____USB_OTG_IMPLEMENT(__N, __VALUE)                                     \
extern vsf_err_t USB_OTG##__N##_set_ep_fifo(vsf_hw_usb_t *usb, uint8_t ep, uint8_t *buffer, uint_fast16_t size);\
extern vsf_err_t USB_OTG##__N##_write_ep_fifo(vsf_hw_usb_t *usb, uint8_t ep, uint_fast16_t size);\
extern void * USB_OTG##__N##_get_ep_reg(void);                                  \
void USB_OTG##__N##_IRQHandler(void)                                            \
{                                                                               \
    __USB_OTG_IMPLEMENT_IRQHANDLER(__N)                                         \
}                                                                               \
static const vsf_hw_usb_const_t __USB_OTG##__N##_const = {                      \
    USB_OTG##__N##_CONFIG                                                       \
};                                                                              \
vsf_hw_usb_t USB_OTG##__N##_IP = {                                              \
    .param = &__USB_OTG##__N##_const,                                           \
};


#define __USB_OTG_IMPLEMENT(__N, __VALUE)   ____USB_OTG_IMPLEMENT(__N, __VALUE)
#define USB_OTG_IMPLEMENT(__N, __VALUE)     __USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(vsf_hw_usbd_irq)
void vsf_hw_usbd_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }
VSF_CAL_WEAK(vsf_hw_usbh_irq)
void vsf_hw_usbh_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }

VSF_MREPEAT(USB_OTG_COUNT, USB_OTG_IMPLEMENT, NULL)

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
