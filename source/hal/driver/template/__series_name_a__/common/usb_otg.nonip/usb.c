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

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

/*============================ MACROS ========================================*/

#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
#   define ____VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)                           \
__USB_DC_BODY(__N, __VALUE##_usbd)                                              \
const i_usb_dc_t VSF_USB_DC##__N = __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE##_usbd);\
vsf_hw_usb_t VSF_HW_USB_OTG##__N = {                                            \
    .irq        = VSF_HW_USB_OTG##__N##_IRQN,                                   \
    .reg        = (void *)VSF_HW_USB_OTG##__N##_REG,                            \
};                                                                              \
VSF_CAL_ROOT void VSF_HW_USB_OTG##__N##_IRQHandler(void)                        \
{                                                                               \
    if (VSF_HW_USB_OTG##__N.is_host) {                                          \
        vsf_hw_usbh_irq(&VSF_HW_USB_OTG##__N);                                  \
    } else {                                                                    \
        vsf_hw_usbd_irq(&VSF_HW_USB_OTG##__N);                                  \
    }                                                                           \
}
#elif VSF_HAL_USE_USBD == ENABLED
#   define ____VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)                           \
__USB_DC_BODY(__N, __VALUE##_usbd)                                              \
const i_usb_dc_t VSF_USB_DC##__N = __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE##_usbd);\
vsf_hw_usb_t VSF_HW_USB_OTG##__N = {                                            \
    .irq        = VSF_HW_USB_OTG##__N##_IRQN,                                   \
    .reg        = (void *)VSF_HW_USB_OTG##__N##_REG,                            \
};                                                                              \
VSF_CAL_ROOT void VSF_HW_USB_OTG##__N##_IRQHandler(void)                        \
{                                                                               \
    vsf_hw_usbd_irq(&VSF_HW_USB_OTG##__N);                                      \
}
#elif VSF_HAL_USE_USBH == ENABLED
#   define ____VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)                           \
vsf_hw_usb_t VSF_HW_USB_OTG##__N = {                                            \
    .irq        = VSF_HW_USB_OTG##__N##_IRQN,                                   \
    .reg        = (void *)VSF_HW_USB_OTG##__N##_REG,                            \
};                                                                              \
VSF_CAL_ROOT void VSF_HW_USB_OTG##__N##_IRQHandler(void)                        \
{                                                                               \
    vsf_hw_usbh_irq(&VSF_HW_USB_OTG##__N);                                      \
}
#endif

#define __VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)    ____VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)
#define VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)      __VSF_HW_USB_OTG_IMPLEMENT(__N, __VALUE)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_HW_USB_OTG_COUNT > 0
VSF_CAL_WEAK(vsf_hw_usbd_irq)
void vsf_hw_usbd_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }
VSF_CAL_WEAK(vsf_hw_usbh_irq)
void vsf_hw_usbh_irq(vsf_hw_usb_t *dc) { VSF_HAL_ASSERT(false); }

VSF_MREPEAT(VSF_HW_USB_OTG_COUNT, VSF_HW_USB_OTG_IMPLEMENT, vsf_hw)
#endif

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
