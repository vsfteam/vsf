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



#ifndef __VSF_USB_CFG_H__
#define __VSF_USB_CFG_H__

/*============================ MACROS ========================================*/
/*============================ INCLUDES ======================================*/
#include "vsf_cfg.h"

/*============================ MACROS ========================================*/

#ifndef VSF_USBD_CFG_STREAM_EN
#   define VSF_USBD_CFG_STREAM_EN       ENABLED
#endif

#if VSF_USE_USB_HOST_ECM == ENABLED
#   undef VSF_USE_USB_HOST_CDC
#   define VSF_USE_USB_HOST_CDC         ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



#endif
/* EOF */