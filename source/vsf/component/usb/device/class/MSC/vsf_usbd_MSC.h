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

extern const vk_usbd_class_op_t vk_usbd_msc_class;

/*============================ PROTOTYPES ====================================*/

#endif      // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_MSC
#endif      // __VSF_USBD_MSC_H__
