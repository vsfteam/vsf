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

/*============================ MACROS ========================================*/

#define __USB_HC_INTERFACE_DEF(__N, __VALUE)                                    \
const i_usb_hc_ip_t VSF_USB_HC##__N##_IP = __USB_HC_IP_INTERFACE_FUNC_DEF(__N, __VALUE);

#define __USB_DC_INTERFACE_DEF(__N, __VALUE)                                    \
const i_usb_dc_t VSF_USB_DC##__N = __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(VSF_HW_USB_HC_COUNT, __USB_HC_IP_FUNC_DEF, NULL)
VSF_MREPEAT(VSF_HW_USB_DC_COUNT, __USB_DC_FUNC_DEF, NULL)

#if VSF_HW_USB_HC_COUNT > 0
VSF_MREPEAT(VSF_HW_USB_HC_OHCI_COUNT, __USB_HC_INTERFACE_DEF, NULL)
#endif

#if VSF_HW_USB_DC_COUNT > 0
VSF_MREPEAT(VSF_HW_USB_DC_COUNT, __USB_DC_INTERFACE_DEF, NULL)
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_HW_USB_HC_OHCI_COUNT > 0
VSF_MREPEAT(VSF_HW_USB_HC_COUNT, __USB_HC_IP_BODY, m480_ohci)
#endif

#if VSF_HW_USB_DC_HS_COUNT > 0
VSF_MREPEAT(VSF_HW_USB_DC_HS_COUNT, __USB_DC_BODY, m480_usbd_hs)
#endif

#if VSF_HW_USB_DC_FS_COUNT > 0
VSF_MREPEAT(VSF_HW_USB_DC_FS_COUNT, __USB_DC_BODY, m480_usbd_fs)
#endif
