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
#ifndef __VSF_DISP_USBD_UVC_H__
#define __VSF_DISP_USBD_UVC_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_USBD_UVC == ENABLED

#include "kernel/vsf_kernel.h"

#if     defined(__VSF_DISP_USBD_UVC_CLASS_IMPLEMENT)
#   undef __VSF_DISP_USBD_UVC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_USE_EDA != ENABLED
#   error "need VSF_USBD_CFG_USE_EDA"
#endif

#if VSF_USE_USB_DEVICE != ENABLED || VSF_USBD_USE_UVC != ENABLED
#   error "need VSF_USE_USB_DEVICE and VSF_USBD_USE_UVC"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_usbd_uvc_t)

def_simple_class(vk_disp_usbd_uvc_t) {
    public_member(
        implement(vk_disp_t)
    )

    public_member(
        vk_usbd_uvc_t  *uvc;
        uint16_t        frame_cnt;
    )
    private_member(
        vsf_eda_t       eda;
        uint8_t         *cur_buffer;
        uint8_t         header[2];
        uint16_t        line_cnt;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_usbd_uvc;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_USBD_UVC_H__
