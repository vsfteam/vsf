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

#ifndef __VSF_USBD_MSC_H__
#define __VSF_USBD_MSC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_MSC == ENABLED

#include "../../../common/class/MSC/vsf_usb_MSC.h"
#include "./vsf_usbd_MSC_desc.h"

#if     defined(VSF_USBD_MSC_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_MSC_IMPLEMENT
#elif   defined(VSF_USBD_MSC_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSF_USBD_MSC_INHERIT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USE_SCSI != ENABLED
#   error msc uses scsi!!!
#endif



#define USB_MSC_PARAM(__BULK_IN_EP, __BULK_OUT_EP, __MAX_LUN, __SCSI_DEV, __STREAM)\
            .ep_out             = (__BULK_OUT_EP),                              \
            .ep_in              = (__BULK_IN_EP),                               \
            .max_lun            = (__MAX_LUN),                                  \
            .scsi               = (__SCSI_DEV),                                 \
            .stream             = (__STREAM),

#define USB_MSC_IFS_NUM             1
#define USB_MSCBOT_IFS_NUM          USB_MSC_IFS_NUM
#define USB_MSC_IFS(__MSC_PARAM)                                                \
            {                                                                   \
                .class_op       = &vk_usbd_msc,                                 \
                .class_param  =  &(__MSC_PARAM),                                \
            },



#define __mscbot_desc(__NAME, __IFS, __I_FUNC, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE)\
            USB_DESC_MSCBOT_IAD((__IFS), 4 + (__I_FUNC), (__BULK_IN_EP), (__BULK_OUT_EP), (__BULK_EP_SIZE))

#define __mscbot_func(__NAME, __FUNC_ID, __BULK_IN_EP, __BULK_OUT_EP, __MAX_LUN, __SCSI_DEV, __STREAM)\
            vk_usbd_msc_t __##__NAME##_MSC##__FUNC_ID = {                       \
                USB_MSC_PARAM((__BULK_IN_EP), (__BULK_OUT_EP), (__MAX_LUN), (__SCSI_DEV), (__STREAM))\
            };

#define __msc_ifs(__NAME, __FUNC_ID)                                            \
                {                                                               \
                    USB_MSC_IFS(__##__NAME##_MSC##__FUNC_ID)                    \
                },

#define mscbot_desc(__NAME, __IFS, __I_FUNC, __BULK_IN_EP, __BULK_OUT_EP, __BULK_EP_SIZE)\
            __mscbot_desc(__NAME, (__IFS), (__I_FUNC), (__BULK_IN_EP), (__BULK_OUT_EP), (__BULK_EP_SIZE))
#define mscbot_func(__NAME, __FUNC_ID, __BULK_IN_EP, __BULK_OUT_EP, __MAX_LUN, __SCSI_DEV, __STREAM)\
            __mscbot_func(__NAME, __FUNC_ID, (__BULK_IN_EP), (__BULK_OUT_EP), (__MAX_LUN), (__SCSI_DEV), (__STREAM))
#define mscbot_ifs(__NAME, __FUNC_ID)                                           \
            __msc_ifs(__NAME, __FUNC_ID)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_class(vk_usbd_msc_t)

union vk_usbd_msc_scsi_ctx_t {
    usb_msc_cbw_t cbw;
    struct {
        usb_msc_csw_t csw;
        struct {
            uint32_t cur_size;
            uint32_t reply_size;
        };
    };
};
typedef union vk_usbd_msc_scsi_ctx_t vk_usbd_msc_scsi_ctx_t;

def_simple_class(vk_usbd_msc_t) {

    private_member(
        vsf_eda_t eda;
        vk_usbd_msc_scsi_ctx_t ctx;
        vk_usbd_dev_t *dev;
        vk_usbd_ep_stream_t ep_stream;
        uint8_t is_inited   : 1;
        uint8_t is_stream   : 1;
    )

    public_member(
        const uint8_t ep_out;
        const uint8_t ep_in;
        const uint8_t max_lun;
        vk_scsi_t *scsi;
        vsf_stream_t *stream;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_msc;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_MSC
#endif      // __VSF_USBD_MSC_H__
