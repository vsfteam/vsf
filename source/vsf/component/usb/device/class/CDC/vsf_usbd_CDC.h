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

#if VSF_USE_USB_DEVICE == ENABLED && VSF_USBD_USE_CDC == ENABLED

#include "component/usb/common/class/CDC/vsf_usb_CDC.h"

#if     defined(__VSF_USBD_CDC_CLASS_IMPLEMENT)
#   undef __VSF_USBD_CDC_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBD_CDC_CLASS_INHERIT__)
#   undef __VSF_USBD_CDC_CLASS_INHERIT__
#   define __PLOOC_CLASS_INHERIT__
#endif
#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_USBD_CFG_STREAM_EN != ENABLED
#   error "CDC need VSF_USBD_CFG_STREAM_EN!!!"
#endif

#define USB_CDC_IFS_NUM             2

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbd_cdc_t)

typedef union vk_usbd_cdc_ep_t {
    struct {
        uint8_t notify;
        uint8_t out;
        uint8_t in;
    };
    uint32_t ep_cfg;
} vk_usbd_cdc_ep_t;

typedef struct vk_usbd_cdc_encapsulate_t vk_usbd_cdc_encapsulate_t;
struct vk_usbd_cdc_encapsulate_t {
    uint8_t *buffer;
    uint32_t size;
    union {
        vsf_err_t (*on_cmd)(vk_usbd_cdc_encapsulate_t *cmd);
        vsf_err_t (*on_resp)(vk_usbd_cdc_encapsulate_t *resp);
    };
};

def_simple_class(vk_usbd_cdc_t) {

    public_member(
        implement_ex(vk_usbd_cdc_ep_t, ep)
        /*uint8_t ep_notify;
        uint8_t ep_out;
        uint8_t ep_in;*/

        struct {
#if VSF_USE_SIMPLE_STREAM == ENABLED
            // stream.tx is used for data stream send to USB host
            // stream.rx is used for data stream receive from USB host
            vk_usbd_ep_stream_t tx;
            vk_usbd_ep_stream_t rx;
#elif VSF_USE_STREAM == ENABLED
            implement (vk_usbd_ep_stream_t);
#endif
        } stream;

        // no need to initialize below if encapsulate command/response is not used
        vk_usbd_cdc_encapsulate_t cmd, resp;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const struct vk_usbd_class_op_t vk_usbd_cdc_control;
extern const struct vk_usbd_class_op_t vk_usbd_cdc_data;

/*============================ PROTOTYPES ====================================*/

// helper functions
extern void vk_usbd_cdc_data_connect(vk_usbd_cdc_t *cdc);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_USB_DEVICE && VSF_USBD_USE_CDC
#endif	// __VSF_USBD_CDC_H__
