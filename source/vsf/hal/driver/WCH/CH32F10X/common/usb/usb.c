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

/*============================ MACROS ========================================*/

#define __USB_DC_INTERFACE_DEF(__N, __VALUE)                                    \
const i_usb_dc_t VSF_USB_DC##__N = __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

REPEAT_MACRO(USB_DC_COUNT, __USB_DC_FUNC_DEF, NULL)
REPEAT_MACRO(USB_DC_COUNT, __USB_DC_INTERFACE_DEF, NULL)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

REPEAT_MACRO(USB_DC_COUNT, __USB_DC_BODY, ch32f10x_usbd)
