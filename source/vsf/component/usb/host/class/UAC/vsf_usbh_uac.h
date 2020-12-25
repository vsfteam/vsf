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

#ifndef __VSF_USBH_UAC_H__
#define __VSF_USBH_UAC_H__


/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_UAC == ENABLED

#include "component/usb/common/class/UAC/vsf_usb_UAC.h"

#undef PUBLIC_CONST
#if     defined(__VSF_USBH_UAC_CLASS_IMPLEMENT)
#   undef __VSF_USBH_UAC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
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
#ifndef VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM
#   define VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM  1
#endif
#if VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM > 8
#   error VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM MUST be <= 8
#endif

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM is needed for USBH UAC driver
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbh_uac_stream_t)

def_simple_class(vk_usbh_uac_stream_t) {
    public_member(
        PUBLIC_CONST uint8_t is_in          : 1;
        PUBLIC_CONST uint8_t sample_size    : 3;
        PUBLIC_CONST uint8_t channel_num;
        PUBLIC_CONST uint16_t format;
        PUBLIC_CONST uint32_t sample_rate;
    )

    private_member(
        uint8_t idx                         : 6;
        uint8_t is_connected                : 1;
        uint8_t is_to_disconnect            : 1;
        uint8_t urb_mask;
        uint16_t next_frame;

        vk_usbh_urb_t urb[VSF_USBH_UAC_CFG_URB_NUM_PER_STREAM];
        vsf_stream_t *stream;
        void *param;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbh_class_drv_t vk_usbh_uac_drv;

/*============================ PROTOTYPES ====================================*/

extern vk_usbh_uac_stream_t * vsf_usbh_uac_get_stream_info(void *param, uint_fast8_t stream_idx);
extern vsf_err_t vsf_usbh_uac_connect_stream(void *param, uint_fast8_t stream_idx, vsf_stream_t *stream);
extern void vsf_usbh_uac_disconnect_stream(void *param, uint_fast8_t stream_idx);
extern vsf_err_t __vsf_usbh_uac_submit_req(void *uac_ptr, void *data, struct usb_ctrlrequest_t *req);

#ifdef __cplusplus
}
#endif

#endif
#endif
