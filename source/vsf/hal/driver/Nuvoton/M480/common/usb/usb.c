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

/*============================ MACROS ========================================*/

#define __USB_HC_INTERFACE_DEF(__N, __VALUE)                                    \
const i_usb_hc_t VSF_USB_HC##__N =                                              \
            {                                                                   \
                .Init           = &usb_hc##__N##_init,                          \
                .GetRegBase     = &usb_hc##__N##_get_regbase,                   \
                .Irq            = &usb_hc##__N##_irq,                           \
            };

#define __USB_DC_INTERFACE_DEF(__N, __VALUE)                                    \
const i_usb_dc_t VSF_USB_DC##__N = __USB_DC_INTERFACE_FUNC_DEF(__N, __VALUE);

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

MREPEAT(USB_HC_COUNT, __USB_HC_FUNC_DEF, NULL)
MREPEAT(USB_DC_COUNT, __USB_DC_FUNC_DEF, NULL)

#if USB_HC_COUNT > 0 || USB_DC_COUNT > 0
const i_usb_t VSF_USB = {
#if USB_HC_COUNT > 0
    .HC                 = {MREPEAT(USB_HC_COUNT, __USB_HC_INTERFACE_FUNC_DEF, NULL)},
#endif

#if USB_DC_COUNT > 0
    .DC                 = {MREPEAT(USB_DC_COUNT, __USB_DC_INTERFACE_FUNC_DEF, NULL)},
#endif
};
#endif

#if USB_HC_COUNT > 0
MREPEAT(USB_HC_COUNT, __USB_HC_INTERFACE_DEF, NULL)
#endif

#if USB_DC_COUNT > 0
MREPEAT(USB_DC_COUNT, __USB_DC_INTERFACE_DEF, NULL)
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

MREPEAT(USB_HC_COUNT, __USB_HC_BODY, NULL)
MREPEAT(USB_DC_COUNT, __USB_DC_BODY, NULL)
