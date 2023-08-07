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

#ifndef __VSF_USBD_HIDMSC_H__
#define __VSF_USBD_HIDMSC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_HID == ENABLED && VSF_USBD_USE_MSC == ENABLED

#include "../MSC/vsf_usbd_MSC.h"

#if     defined(__VSF_USBD_HIDMSC_CLASS_IMPLEMENT)
#   undef __VSF_USBD_HIDMSC_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_STREAM_EN != ENABLED
#   error hidmsc need stream
#endif



#define __USB_HIDMSC_REPORT_DESC_SIZE       29

#define USB_HIDMSC_PARAM(__INT_IN_EP, __INT_OUT_EP, __SCSI_DEV, __STREAM)       \
            .msc                = {                                             \
                .ep_out         = (__INT_OUT_EP),                               \
                .ep_in          = (__INT_IN_EP),                                \
                .scsi           = (__SCSI_DEV),                                 \
                .stream         = (__STREAM),                                   \
            },

#define USB_HIDMSC_IFS_NUM          USB_HID_IFS_NUM
#define USB_DESC_HIDMSC_LEN         USB_DESC_HID_LEN
#define USB_DESC_HIDMSC_IAD_LEN     USB_DESC_HID_IAD_LEN
#define USB_HIDMSC_IFS(__PARAM)USB_IFS(&vk_usbd_hidmsc, &(__PARAM))


#define __usbd_hidmsc_desc(__name, __ifs, __i_func, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size)\
            USB_DESC_MSCBOT((__ifs), 4 + (__i_func), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size))
#define __usbd_hidmsc_desc_iad(__name, __ifs, __i_func, __bulk_in_ep, __bulk_out_ep, __bulk_ep_size)\
            USB_DESC_MSCBOT_IAD((__ifs), 4 + (__i_func), (__bulk_in_ep), (__bulk_out_ep), (__bulk_ep_size))

#define __usbd_hidmsc_func(__name, __func_id, __int_in_ep, __int_out_ep, __scsi_dev, __stream)\
            vk_usbd_hidmsc_t __##__name##_hidmsc##__func_id = {               \
                USB_HIDMSC_PARAM((__int_in_ep), (__int_out_ep), (__scsi_dev), (__stream))\
            };

#define __usbd_hidmsc_ifs(__name, __func_id)                                   \
            USB_HIDMSC_IFS(__##__name##_hidmsc##__func_id)

#define usbd_hidmsc_desc(__name, __ifs, __i_func, __int_in_ep, __int_out_ep)   \
            usbd_hid_desc(__name, (__ifs), (__i_func), 0, 0, 0x0111, 0, __USB_HIDMSC_REPORT_DESC_SIZE, (__int_in_ep), 512, 1, (__int_out_ep), 512, 1)
#define usbd_hidmsc_desc_iad(__name, __ifs, __i_func, __int_in_ep, __int_out_ep)\
            usbd_hid_desc_iad(__name, (__ifs), (__i_func), 0, 0, 0x0111, 0, __USB_HIDMSC_REPORT_DESC_SIZE, (__int_in_ep), 512, 1, (__int_out_ep), 512, 1)
#define usbd_hidmsc_func(__name, __func_id, __int_in_ep, __int_out_ep, __scsi_dev, __stream)\
            __usbd_hidmsc_func(__name, __func_id, (__int_in_ep), (__int_out_ep), (__scsi_dev), (__stream))
#define usbd_hidmsc_ifs(__name, __func_id)                                      \
            __usbd_hidmsc_ifs(__name, __func_id)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vk_usbd_hidmsc_t) {
    public_member(
        implement(vk_usbd_hid_t)
        vk_usbd_msc_t msc;
    )
    protected_member(
        vk_usbd_ifs_t msc_ifs;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_hidmsc;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_USB_DEVICE && VSF_USBD_USE_HID && VSF_USBD_USE_MSC
#endif      // __VSF_USBD_HIDMSC_H__
