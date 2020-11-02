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

#ifndef __VSF_USBH_CDC_H__
#define __VSF_USBH_CDC_H__

/*============================ INCLUDES ======================================*/
#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_CDC == ENABLED

#include "component/usb/common/class/CDC/vsf_usb_CDC.h"
#include "../../vsf_usbh.h"

#if     defined(__VSF_USBH_CDC_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_USBH_CDC_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_usbh_cdc_t)

typedef enum vk_usbh_cdc_evt_t {
    VSF_USBH_CDC_ON_INIT,
    VSF_USBH_CDC_ON_DESC,
    VSF_USBH_CDC_ON_EVENT,
    VSF_USBH_CDC_ON_RX,
    VSF_USBH_CDC_ON_TX,
} vk_usbh_cdc_evt_t;

typedef vsf_err_t (*vk_usbh_cdc_evthandler_t)(vk_usbh_cdc_t *cdc,
                vk_usbh_cdc_evt_t evt, void *param);

def_simple_class(vk_usbh_cdc_t) {

    protected_member(
        vk_usbh_t *usbh;
        vk_usbh_dev_t *dev;
        vk_usbh_ifs_t *ifs;

        vk_usbh_cdc_evthandler_t evthandler;
        uint8_t *evt_buffer;
        uint32_t evt_size;

        int8_t ctrl_ifs;
        int8_t data_ifs;

        vsf_eda_t eda;
    )

    private_member(
        vk_usbh_urb_t urb_evt;
        vk_usbh_pipe_t pipe_tx;
        vk_usbh_pipe_t pipe_rx;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(__VSF_USBH_CDC_CLASS_IMPLEMENT) || defined(__VSF_USBH_CDC_CLASS_INHERIT__)
extern vsf_err_t vk_usbh_cdc_init(vk_usbh_cdc_t *pthis, vk_usbh_t *usbh,
        vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
extern void vk_usbh_cdc_fini(vk_usbh_cdc_t *pthis);
extern void vk_usbh_cdc_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
extern vsf_err_t vk_usbh_cdc_prepare_urb(vk_usbh_cdc_t *pthis, bool tx, vk_usbh_urb_t *urb);
extern vsf_err_t vk_usbh_cdc_submit_urb(vk_usbh_cdc_t *pthis, vk_usbh_urb_t *urb);
extern void vk_usbh_cdc_free_urb(vk_usbh_cdc_t *pthis, vk_usbh_urb_t *urb);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_USBH_CDC_CLASS_IMPLEMENT
#undef __VSF_USBH_CDC_CLASS_INHERIT__

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_CDC
#endif      // __VSF_USBH_CDC_H__
