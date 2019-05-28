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

#ifndef __VSF_USBD_UVC_H__
#define __VSF_USBD_UVC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../../../common/class/UVC/vsf_usb_UVC.h"

#if     defined(VSF_USBD_UVC_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_UVC_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbd_UVC_t)

typedef struct vsf_usbd_UVC_control_t vsf_usbd_UVC_control_t;

struct vsf_usbd_UVC_control_info_t {
    uint8_t selector;
    uint16_t size;

    vsfav_control_value_t min;
    vsfav_control_value_t max;
    vsfav_control_value_t def;

    void (*on_set)(vsf_usbd_UVC_control_t *control);
};
typedef struct vsf_usbd_UVC_control_info_t vsf_usbd_UVC_control_info_t;

struct vsf_usbd_UVC_control_t {
    const vsf_usbd_UVC_control_info_t *info;
    vsfav_control_value_t cur;
};

struct vsf_usbd_UVC_entity_t {
    uint8_t id;
    uint8_t control_num;
    vsf_usbd_UVC_control_t *control;
};
typedef struct vsf_usbd_UVC_entity_t vsf_usbd_UVC_entity_t;

def_simple_class(vsf_usbd_UVC_t) {

    public_member(
        uint8_t ep_in;
        uint16_t control_num;

        uint8_t entity_num;
        vsf_usbd_UVC_entity_t *entity;

        vsf_usbd_desc_t *desc;
//        vsf_stream_t *video_stream;
    )

    private_member(
        vsf_usbd_trans_t trans_in;
        vsf_usbd_dev_t *dev;
        vsf_usbd_ifs_t *ifs;
#if VSF_USBD_UVC_TRACE_EN == ENABLED
        uint_fast32_t cur_size;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbd_class_op_t vsf_usbd_UVC_control_class;
extern const vsf_usbd_class_op_t vsf_usbd_UVC_stream_class;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_usbd_UVC_send_packet(vsf_usbd_UVC_t *uvc, uint8_t *buffer, uint_fast32_t size);

#endif      // VSF_USE_USB_DEVICE
#endif      // __VSF_USBD_UVC_H__
