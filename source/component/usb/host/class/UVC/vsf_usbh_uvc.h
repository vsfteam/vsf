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

#ifndef __VSF_USBH_UVC_H__
#define __VSF_USBH_UVC_H__


/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_UVC == ENABLED

#include "component/usb/common/class/UVC/vsf_usb_UVC.h"

#undef PUBLIC_CONST
#if     defined(__VSF_USBH_UVC_CLASS_IMPLEMENT)
#   undef __VSF_USBH_UVC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#   define PUBLIC_CONST
#else
#   define PUBLIC_CONST                         const
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// for some hcd, one urb will take 2ms(eg. ohci), so need 2 urbs to implement 1ms interval transaction
#ifndef VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM
#   define VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM  1
#endif
#if VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM > 8
#   error VSF_USBH_UVC_CFG_URB_NUM_PER_STREAM MUST be <= 8
#endif

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM is needed for USBH UVC driver
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_uvc_drv;

/*============================ PROTOTYPES ====================================*/

extern int16_t vsf_usbh_uvc_get_desc(uint8_t *buf, uint_fast16_t size, uint_fast8_t subtype, void **ptr);
extern usb_uvc_format_desc_t * vsf_usbh_uvc_get_format(void *param, uint_fast8_t format_idx);
extern vsf_err_t vsf_usbh_uvc_connect_stream(void *param, uint_fast8_t format_idx, uint_fast8_t frame_idx, vsf_stream_t *stream);
extern void vsf_usbh_uvc_disconnect_stream(void *param);
extern vsf_err_t __vsf_usbh_uvc_submit_req(void *uvc_ptr, void *data, struct usb_ctrlrequest_t *req);

#ifdef __cplusplus
}
#endif

#endif
#endif
