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

#ifndef __VSF_USBMITM_H__
#define __VSF_USBMITM_H__

extern const vk_usbh_class_drv_t vsf_usbh_usbmitm_drv;

void usbmitm_init(const i_usb_dc_t *drv, int32_t int_priority);

typedef struct usbmitm_plugin_op_t {
	void (*parse_config)(uint8_t *data, uint16_t len);
	void (*on_SETUP)(struct usb_ctrlrequest_t *request, int16_t urb_status, uint8_t *data, uint16_t len);
	void (*on_IN)(uint8_t ep, int16_t urb_status, uint8_t *data, uint16_t len);
	void (*on_OUT)(uint8_t ep, int16_t urb_status, uint8_t *data, uint16_t len);
} usbmitm_plugin_op_t;

typedef struct usbmitm_plugin_t usbmitm_plugin_t;
struct usbmitm_plugin_t {
	const usbmitm_plugin_op_t *op;
	struct usbmitm_plugin_t *next;
};

extern usbmitm_plugin_t usbmitm_plugin_stdreq;
extern usbmitm_plugin_t usbmitm_plugin_hid;
extern usbmitm_plugin_t usbmitm_plugin_msc;
void usbmitm_register_plugin(usbmitm_plugin_t *plugin);

#endif // __VSF_USBMITM_H__
