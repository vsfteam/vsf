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

#ifndef __VSF_USBD_CDC_H__
#define __VSF_USBD_CDC_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDC.h"

#if     defined(VSF_USBD_CDC_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_CDC_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_STREAM_EN != ENABLED
#   error "CDC need VSF_USBD_CFG_STREAM_EN!!!"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbd_CDC_t)

typedef struct vsf_usbd_encapsulate_t vsf_usbd_encapsulate_t;
struct vsf_usbd_encapsulate_t {
    uint8_t *buffer;
    uint32_t size;
    union {
        vsf_err_t (*on_cmd)(vsf_usbd_encapsulate_t *cmd);
        vsf_err_t (*on_resp)(vsf_usbd_encapsulate_t *resp);
    };
};

def_simple_class(vsf_usbd_CDC_t) {

    public_member(
        implement_ex(vsf_usbd_ep_cfg_t, ep)
        /*uint8_t ep_notify;
        uint8_t ep_out;
        uint8_t ep_in;*/

        struct {
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            // stream.tx is used for data stream send to USB host
            // stream.rx is used for data stream receive from USB host
            vsf_usbd_ep_stream_t tx;
            vsf_usbd_ep_stream_t rx;
#elif VSF_USE_SERVICE_STREAM == ENABLED
            implement (vsf_usbd_ep_stream_t);
#endif
        } stream;

        // no need to initialize below if encapsulate command/response is not used
        vsf_usbd_encapsulate_t cmd, resp;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const struct vsf_usbd_class_op_t vsf_usbd_CDC_control;
extern const struct vsf_usbd_class_op_t vsf_usbd_CDC_data;

/*============================ PROTOTYPES ====================================*/

// helper functions
extern void vsf_usbd_CDC_data_connect(vsf_usbd_CDC_t *cdc);

#endif  // VSF_USE_USB_DEVICE
#endif	// __VSF_USBD_CDC_H__
