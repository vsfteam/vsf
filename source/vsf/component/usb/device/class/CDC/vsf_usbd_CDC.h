/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
