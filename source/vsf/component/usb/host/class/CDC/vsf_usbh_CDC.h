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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_CDC == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDC.h"

#if     defined(VSF_USBH_CDC_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(VSF_USBH_CDC_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbh_cdc_t)

enum vsf_usbh_cdc_evt_t {
    VSF_USBH_CDC_ON_INIT,
    VSF_USBH_CDC_ON_DESC,
    VSF_USBH_CDC_ON_EVENT,
    VSF_USBH_CDC_ON_RX,
    VSF_USBH_CDC_ON_TX,
};
typedef enum vsf_usbh_cdc_evt_t vsf_usbh_cdc_evt_t;

typedef vsf_err_t (*vsf_usbh_cdc_evthandler_t)(vsf_usbh_cdc_t *cdc,
                vsf_usbh_cdc_evt_t evt, void *param);

def_simple_class(vsf_usbh_cdc_t) {

    protected_member(
        vsf_usbh_t *usbh;
        vsf_usbh_dev_t *dev;
        vsf_usbh_ifs_t *ifs;

        vsf_usbh_cdc_evthandler_t evthandler;
        uint8_t *evt_buffer;
        uint32_t evt_size;

        int8_t ctrl_ifs;
        int8_t data_ifs;

        vsf_eda_t eda;
    )

    private_member(
        vsf_usbh_urb_t urb_evt;
        vsf_usbh_eppipe_t pipe_tx;
        vsf_usbh_eppipe_t pipe_rx;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if defined(VSF_USBH_CDC_IMPLEMENT) || defined(VSF_USBH_CDC_INHERIT)
extern vsf_err_t vsf_usbh_cdc_init(vsf_usbh_cdc_t *pthis, vsf_usbh_t *usbh,
        vsf_usbh_dev_t *dev, vsf_usbh_ifs_parser_t *parser_ifs);
extern void vsf_usbh_cdc_fini(vsf_usbh_cdc_t *pthis);
extern void vsf_usbh_cdc_evthandler(vsf_eda_t *eda, vsf_evt_t evt);
extern vsf_err_t vsf_usbh_cdc_prepare_urb(vsf_usbh_cdc_t *pthis, bool tx, vsf_usbh_urb_t *urb);
extern vsf_err_t vsf_usbh_cdc_submit_urb(vsf_usbh_cdc_t *pthis, vsf_usbh_urb_t *urb);
extern void vsf_usbh_cdc_free_urb(vsf_usbh_cdc_t *pthis, vsf_usbh_urb_t *urb);
#endif

#undef VSF_USBH_CDC_IMPLEMENT
#undef VSF_USBH_CDC_IMHERIT

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_CDC
#endif      // __VSF_USBH_CDC_H__
