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

#if VSF_HAL_USE_USBH == ENABLED
#   define __USB_HC_INTERFACE_DEF(__N, __VALUE)                                 \
const i_usb_hc_ip_t VSF_USB_HC##__N##_IP = __USB_HC_IP_INTERFACE_FUNC_DEF(__N, __VALUE);
#endif

#if VSF_HAL_USE_USBD == ENABLED
#   define __USB_DC_INTERFACE_DEF(__N, __VALUE)                                 \
const i_usb_dc_ip_t VSF_USB_DC##__N##_IP = __USB_DC_IP_INTERFACE_FUNC_DEF(__N, __VALUE);
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if USB_OTG_COUNT > 0
#   if VSF_HAL_USE_USBH == ENABLED
REPEAT_MACRO(USB_OTG_COUNT, __USB_HC_IP_FUNC_DEF, NULL)
REPEAT_MACRO(USB_OTG_COUNT, __USB_HC_INTERFACE_DEF, NULL)
#   endif

#   if VSF_HAL_USE_USBD == ENABLED
REPEAT_MACRO(USB_OTG_COUNT, __USB_DC_IP_FUNC_DEF, NULL)
REPEAT_MACRO(USB_OTG_COUNT, __USB_DC_INTERFACE_DEF, NULL)
#   endif
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if USB_OTG_COUNT > 0
#   if VSF_HAL_USE_USBH == ENABLED
REPEAT_MACRO(USB_OTG_COUNT, __USB_OTG_HC_IP_BODY, mt071_usbh)
#   endif
#   if VSF_HAL_USE_USBD == ENABLED
REPEAT_MACRO(USB_OTG_COUNT, __USB_OTG_DC_IP_BODY, mt071_usbd)
#   endif
#endif

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
