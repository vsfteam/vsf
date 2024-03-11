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

#ifndef __HAL_DRIVER_${SERIES}_USB_H__
#define __HAL_DRIVER_${SERIES}_USB_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USBD == ENABLED || VSF_HAL_USE_USBH == ENABLED

// for basic types/structures/compilers etc
#include "utilities/vsf_utilities.h"

#include "../../__device.h"
#include "./dc/usbd.h"
#include "./hc/usbh.h"

/*\note Refer to template/README.md for usage cases.
 *      For peripherial drivers, blackbox mode is recommended but not required, reimplementation part MUST be open.
 *      For IPCore drivers, class structure, MULTI_CLASS configuration, reimplementation and class APIs should be open to user.
 *      For emulated drivers, **** No reimplementation ****.
 *
 *      This template is designed only for peripherial drivers with host and device functionality.
 */

#include "hal/driver/common/template/vsf_template_usb.h"

/*\note Incudes CAN ONLY be put here. */

/*\note Use OOC because vsf_hw_usb_t should be open to dc/hc drivers, while should not open to users.
 */

#if     defined(__VSF_HAL_HW_USB_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_HAL_HW_USB_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/*\note IMPORTANT: ******** Add configurations below to vsf_board_cfg.h. ********
#define VSF_USBD_CFG_DRV_LV0_OO
#   define VSF_USBD_CFG_DRV_LV0_OO_PREFIX           vsf_hw_usbd
#   define VSF_USBD_CFG_DRV_LV0_OO_OBJ              VSF_HW_USB_OTG0
 */

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_HW_USB_OTG_DEF(__N, __VALUE)  extern vsf_hw_usb_t VSF_HW_USB_OTG##__N;
#define _VSF_HW_USB_OTG_DEF(__N, __VALUE)   __VSF_HW_USB_OTG_DEF(__N, __VALUE)
#define VSF_HW_USB_OTG_DEF(__N, __VALUE)    _VSF_HW_USB_OTG_DEF(__N, __VALUE)

/*============================ TYPES =========================================*/

vsf_class(vsf_hw_usb_t) {
    protected_member(
#if VSF_HAL_USE_USBD == ENABLED && VSF_HAL_USE_USBH == ENABLED
        bool is_host;
#endif

/*\note Add hc members to hc structure, dc members to dc structure.
 */

        IRQn_Type                           irq;
        void                                *reg;
        union {
#if VSF_HAL_USE_USBH == ENABLED
            struct {

            } hc;
#endif
#if VSF_HAL_USE_USBD == ENABLED
            struct {
                usb_dc_evthandler_t         evthandler;
                void                        *param;
            } dc;
#endif
        };
    )
};

/*============================ INCLUDES ======================================*/

#include "./hc/usbh.h"
#include "./dc/usbd.h"

/*============================ GLOBAL VARIABLES ==============================*/

VSF_MREPEAT(VSF_HW_USB_OTG_COUNT, VSF_HW_USB_OTG_DEF, NULL)

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_HAL_USE_USBD || VSF_HAL_USE_USBH
#endif      // __HAL_DRIVER_${SERIES}_USB_H__
/* EOF */
