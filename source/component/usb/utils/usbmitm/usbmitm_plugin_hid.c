#include <vsf.h>
#include <component/usb/common/class/HID/vsf_usb_HID.h>

#include "./vsf_usbmitm.h"

struct usbmitm_plugin_hid_t {
	struct hid_param_t {
		uint8_t interface;
		uint8_t ep_out;
		uint8_t ep_in;
	} hid[8];
} static plugin_hid;

static struct hid_param_t * usbmitm_plugin_hid_get_param_byifs(uint8_t ifs)
{
	for (int i = 0; i < dimof(plugin_hid.hid); i++) {
		if (plugin_hid.hid[i].interface == ifs) {
			return &plugin_hid.hid[i];
        }
    }
	return NULL;
}

static void usbmitm_plugin_hid_parse_config(uint8_t *data, uint16_t len)
{
	uint16_t pos = USB_DT_CONFIG_SIZE;
	usb_interface_desc_t *if_desc;
	struct hid_param_t *param = plugin_hid.hid;

	for (int i = 0; i < dimof(plugin_hid.hid); i++, param++) {
		param->interface = 0xFF;
		param->ep_in = param->ep_out = 0;
	}

	while (len > pos) {
		switch (data[pos + 1]) {
		case USB_DT_INTERFACE:
			if_desc = (usb_interface_desc_t *)&data[pos];
			param = (if_desc->bInterfaceClass == USB_CLASS_HID) ? usbmitm_plugin_hid_get_param_byifs(0xFF) : NULL;
			if (param != NULL) {
				param->interface = if_desc->bInterfaceNumber;
            }
			break;
		case USB_DT_ENDPOINT:
			if (param != NULL) {
				uint8_t epaddr = data[pos + 2];
				if (epaddr & 0x80) {
					param->ep_in = epaddr & 0x0F;
				} else {
					param->ep_out = epaddr & 0x0F;
                }
			}
			break;
		}
		pos += data[pos];
	}

	param = plugin_hid.hid;
	for (int i = 0; i < dimof(plugin_hid.hid); i++, param++) {
		if (param->interface != 0xFF) {
			vsf_trace_info("HID: interface(%d)", param->interface);
			if (param->ep_in != 0) {
				vsf_trace_info(", ep_in(%d)", param->ep_in);
            }
			if (param->ep_out != 0) {
				vsf_trace_info(", ep_out(%d)", param->ep_out);
            }
			vsf_trace_info(VSF_TRACE_CFG_LINEEND);
		}
	}
}

static void usbmitm_plugin_hid_on_SETUP(struct usb_ctrlrequest_t *request,
							int16_t urb_status, uint8_t *data, uint16_t len)
{
	uint8_t recip = request->bRequestType & USB_RECIP_MASK;

	if (recip == USB_RECIP_INTERFACE) {
		uint16_t type = request->bRequestType & USB_TYPE_MASK;
		uint8_t ifs = (uint8_t)request->wIndex;
		uint8_t index;
		struct hid_param_t *param = usbmitm_plugin_hid_get_param_byifs(ifs);

		if (param != NULL) {
			if (type == USB_TYPE_STANDARD) {
				switch (request->bRequest) {
				case USB_REQ_GET_DESCRIPTOR:
					type = (request->wValue >> 8) & 0xFF;
					if ((type == USB_HID_DT_HID) || (type == USB_HID_DT_REPORT) || (type == USB_HID_DT_PHYSICAL)) {
						vsf_trace_info("HID: USB_REQ_GET_%s_DESCRIPTOR(%d)" VSF_TRACE_CFG_LINEEND,
								type == USB_HID_DT_HID ? "HID" :
								type == USB_HID_DT_REPORT ? "HID_REPORT" :
								type == USB_HID_DT_PHYSICAL ? "HID_PHYSICAL" : "Unknown", ifs);
					}
					break;
				}
			}
			else if (type == USB_TYPE_CLASS)
			{
				switch (request->bRequest)
				{
				case USB_HID_REQ_GET_REPORT:
					type = (request->wValue >> 8) & 0xFF;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_GET_%s_REPORT(%d)" VSF_TRACE_CFG_LINEEND,
								type == USB_HID_REPORT_INPUT ? "INPUT" :
								type == USB_HID_REPORT_OUTPUT ? "OUTPUT" :
								type == USB_HID_REPORT_FEATURE ? "FEATURE" : "Unknown", index);
					break;
				case USB_HID_REQ_GET_IDLE:
					type = (urb_status == URB_OK) ? data[0] : 0;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_GET_IDLE(%d): %d" VSF_TRACE_CFG_LINEEND,
								index, type);
					break;
				case USB_HID_REQ_GET_PROTOCOL:
					type = (urb_status == URB_OK) ? data[0] : 0;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_GET_PROTOCOL(%d): %s" VSF_TRACE_CFG_LINEEND,
								index, type == USB_HID_PROTOCOL_BOOT ? "BOOT" :
								type == USB_HID_PROTOCOL_REPORT ? "REPORT" : "Unknown");
					break;
				case USB_HID_REQ_SET_REPORT:
					type = (request->wValue >> 8) & 0xFF;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_SET_%s_REPORT(%d)" VSF_TRACE_CFG_LINEEND,
								type == USB_HID_REPORT_INPUT ? "INPUT" :
								type == USB_HID_REPORT_OUTPUT ? "OUTPUT" :
								type == USB_HID_REPORT_FEATURE ? "FEATURE" : "Unknown", index);
					break;
				case USB_HID_REQ_SET_IDLE:
					type = (request->wValue >> 8) & 0xFF;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_SET_IDLE(%d, %d)" VSF_TRACE_CFG_LINEEND,
								index, type);
					break;
				case USB_HID_REQ_SET_PROTOCOL:
					type = (request->wValue >> 8) & 0xFF;
					index = (request->wValue >> 0) & 0xFF;
					vsf_trace_info("HID: USB_HIDREQ_SET_PROTOCOL(%d): %s" VSF_TRACE_CFG_LINEEND,
								index, type == USB_HID_PROTOCOL_BOOT ? "BOOT" :
								type == USB_HID_PROTOCOL_REPORT ? "REPORT" : "Unknown");
					break;
				}
			}
		}
	}
}

const usbmitm_plugin_op_t usbmitm_plugin_hid_op = {
	.parse_config = usbmitm_plugin_hid_parse_config,
	.on_SETUP = usbmitm_plugin_hid_on_SETUP,
	.on_IN = NULL,
	.on_OUT = NULL,
};

usbmitm_plugin_t usbmitm_plugin_hid = {
	.op = &usbmitm_plugin_hid_op,
};
