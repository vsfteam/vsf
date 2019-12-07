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

#ifndef __VSF_USBD_CDCACM_H__
#define __VSF_USBD_CDCACM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USE_USB_DEVICE_CDCACM == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDCACM.h"
#include "./vsf_usbd_CDCACM_desc.h"

#if     defined(VSF_USBD_CDCACM_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_CDCACM_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_usbd_cdcacm_t)

def_simple_class(vk_usbd_cdcacm_t) {

    public_member(
        implement(vk_usbd_cdc_t)

        usb_cdcacm_line_coding_t line_coding;
        uint8_t control_line;

        struct {
            vsf_err_t (*set_line_coding)(usb_cdcacm_line_coding_t *line_coding);
            vsf_err_t (*set_control_line)(uint8_t control_line);
            vsf_err_t (*get_control_line)(uint8_t *control_line);
            vsf_err_t (*send_break)(void);
        } callback;
    )
};

struct vk_usbd_cdcacm_cfg_t {
    implement_ex(vk_usbd_ep_cfg_t, ep)
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_stream_t *tx_stream;
    vsf_stream_t *rx_stream;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
    implement_ex(vsf_stream_usr_cfg_t, stream_usr);
    implement_ex(vsf_stream_src_cfg_t, stream_src);
#endif
};
typedef struct vk_usbd_cdcacm_cfg_t vk_usbd_cdcacm_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_usbd_class_op_t vk_usbd_cdcacm_control;
extern const vk_usbd_class_op_t vk_usbd_cdcacm_data;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_usbd_cdcacm_init(vk_usbd_cdcacm_t *obj, const vk_usbd_cdcacm_cfg_t *cfg);

#endif  // VSF_USE_USB_DEVICE && VSF_USE_USB_DEVICE_CDCACM
#endif	// __VSF_USBD_CDCACM_H__
