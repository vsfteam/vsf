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

#ifndef __VSF_USBD_CDCACM_H__
#define __VSF_USBD_CDCACM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../../../common/class/CDC/vsf_usb_CDCACM.h"

#if     defined(VSF_USBD_CDCACM_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_CDCACM_IMPLEMENT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbd_CDCACM_t)

def_simple_class(vsf_usbd_CDCACM_t) {

    public_member(
        implement(vsf_usbd_CDC_t)

        usb_CDCACM_line_coding_t line_coding;
        uint8_t control_line;

        struct {
            vsf_err_t (*set_line_coding)(usb_CDCACM_line_coding_t *line_coding);
            vsf_err_t (*set_control_line)(uint8_t control_line);
            vsf_err_t (*get_control_line)(uint8_t *control_line);
            vsf_err_t (*send_break)(void);
        } callback;
    )
};

typedef struct {
    implement_ex(vsf_usbd_ep_cfg_t, ep);
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
    vsf_stream_t *tx_stream;
    vsf_stream_t *rx_stream;
#elif   VSF_USE_SERVICE_STREAM == ENABLED
    implement_ex(vsf_stream_usr_cfg_t, stream_usr);
    implement_ex(vsf_stream_src_cfg_t, stream_src);
#endif
}vsf_usbd_CDCACM_cfg_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern const vsf_usbd_class_op_t vsf_usbd_CDCACM_control;
extern const vsf_usbd_class_op_t vsf_usbd_CDCACM_data;

/*============================ PROTOTYPES ====================================*/
extern 
vsf_err_t vsf_usbd_CMDACM_init( vsf_usbd_CDCACM_t *obj, 
                                const vsf_usbd_CDCACM_cfg_t *cfg);


#endif  // VSF_USE_USB_DEVICE
#endif	// __VSF_USBD_CDCACM_H__
