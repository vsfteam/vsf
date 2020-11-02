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
//! \note do not move this pre-processor statement to other places
#include "component/vsf_component_cfg.h"

#ifndef __VSF_USB_CFG_H__
#define __VSF_USB_CFG_H__

/*============================ MACROS ========================================*/

#ifndef VSF_USB_ASSERT
#   define VSF_USB_ASSERT                       ASSERT
#endif

#if VSF_USE_USB_DEVICE == ENABLED

#   if VSF_USBD_CFG_RAW_MODE == ENABLED
#       define VSF_USBD_CFG_AUTOSETUP           DISABLED
#   endif

#   if defined(VSF_USBD_CFG_DRV_LV0_OO)
#       undef __VSF_USBD_CFG_DRV_INTERFACE
#       if      defined(VSF_USBD_CFG_DRV_LV0_OO)                                \
            &&  (!defined(VSF_USBD_CFG_DRV_LV0_OO_PREFIX) || !defined(VSF_USBD_CFG_DRV_LV0_OO_OBJ))
#           error VSF_USBD_CFG_DRV_LV0_OO_PREFIX and VSF_USBD_CFG_DRV_LV0_OO_OBJ MUST be defined to use VSF_USBD_CFG_DRV_LV0_OO
#       endif
#   else
#       define __VSF_USBD_CFG_DRV_INTERFACE
#   endif

// check dependency here
#   if VSF_USBD_USE_CDCACM == ENABLED
#       ifndef VSF_USBD_USE_CDC
#           define VSF_USBD_USE_CDC             ENABLED
#       elif VSF_USBD_USE_CDC != ENABLED
#           #warning "VSF_USBD_USE_CDC MUST be enabled to use VSF_USBD_USE_CDCACM"
#           undef VSF_USBD_USE_CDC
#           define VSF_USBD_USE_CDC             ENABLED
#       endif
#   endif

#endif

#ifndef VSF_USBD_CFG_STREAM_EN
#   if VSF_USE_STREAM == ENABLED || VSF_USE_SIMPLE_STREAM == ENABLED
#       define VSF_USBD_CFG_STREAM_EN           ENABLED
#   else
#       define VSF_USBD_CFG_STREAM_EN           DISABLED
#   endif
#endif

#if     (VSF_USBD_CFG_STREAM_EN == ENABLED)                                     \
    &&  (VSF_USE_STREAM != ENABLED)                                     \
    &&  (VSF_USE_SIMPLE_STREAM != ENABLED)
#   error "VSF_USE_STREAM or VSF_USE_SIMPLE_STREAM must be enabled to use VSF_USBD_CFG_STREAM_EN"
#endif

#if VSF_USE_USB_HOST == ENABLED
#   if VSF_USBH_USE_ECM == ENABLED
#       undef VSF_USBH_USE_CDC
#       define VSF_USBH_USE_CDC                 ENABLED
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */