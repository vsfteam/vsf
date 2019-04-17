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

#ifndef __VSF_USBD_HID_H__
#define __VSF_USBD_HID_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#include "../../../common/class/HID/vsf_usb_HID.h"

#if     defined(VSF_USBD_HID_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_USBD_HID_IMPLEMENT
#elif   defined(VSF_USBD_HID_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSF_USBD_HID_INHERIT
#endif
#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#define VSF_USBD_DESC_HID_REPORT(__ptr, __size)                                 \
    {USB_HID_DT_REPORT, 0, 0, (__size), (uint8_t*)(__ptr)}

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_usbd_HID_t)
declare_simple_class(vsf_usbd_HID_report_t)

def_simple_class(vsf_usbd_HID_report_t) {

    public_member(
        usb_HID_report_type_t type;
        vsf_mem_t mem;
        uint8_t id;
        uint8_t idle;
    )

    protected_member(
        bool changed;
        uint8_t idle_cnt;
    )
};

enum vsf_usbd_HID_output_state_t {
    HID_OUTPUT_STATE_WAIT,
    HID_OUTPUT_STATE_RECEIVING,
};
typedef enum vsf_usbd_HID_output_state_t vsf_usbd_HID_output_state_t;

def_simple_class(vsf_usbd_HID_t) {

    public_member(
        uint8_t ep_out;
        uint8_t ep_in;
        uint8_t num_of_report;

        uint8_t has_report_id   : 1;
        uint8_t notify_eda      : 1;

        vsf_usbd_HID_report_t *reports;

        vsf_usbd_desc_t *desc;

        union {
            vsf_err_t (*on_report)(vsf_usbd_HID_t *hid, vsf_usbd_HID_report_t *report);
            vsf_eda_t *eda;
        };
    )

    private_member(
        uint8_t protocol;
        uint8_t cur_report;
        uint8_t cur_OUT_id;
        uint8_t cur_IN_id;

        vsf_usbd_HID_output_state_t output_state;
        bool busy;
        uint16_t pos_out;

        vsf_usbd_trans_t transact_in;
        vsf_usbd_trans_t transact_out;
        vsf_teda_t teda;

        vsf_usbd_dev_t *dev;
        vsf_usbd_ifs_t *ifs;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_usbd_class_op_t vsf_usbd_HID;

/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_usbd_HID_IN_report_changed(vsf_usbd_HID_t *hid, vsf_usbd_HID_report_t *report);

#endif      // VSF_USE_USB_DEVICE
#endif      // __VSF_USBD_HID_H__
