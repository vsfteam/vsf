#include <vsf.h>

#include "./vsf_usbmitm.h"

static void usbmitm_plugin_stdreq_on_SETUP(struct usb_ctrlrequest_t *request,
							int16_t urb_status, uint8_t *data, uint16_t len)
{
	uint8_t recip = request->bRequestType & USB_RECIP_MASK;
	uint16_t value, type;

	if ((request->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		vsf_trace_info("StdReq: ");
		switch (request->bRequest) {
		case USB_REQ_GET_STATUS:
			value = (urb_status == URB_OK) ? get_unaligned_le16(data) : 0;
			switch (recip) {
			case USB_RECIP_DEVICE:
				vsf_trace_info("USB_REQ_GET_DEVICE_STATUS: %s%s",
						value & (1 << 0) ? "Self-powered" : "Bus-powered",
						value & (1 << 1) ? ", Remote wakeup" : "");
				break;
			case USB_RECIP_ENDPOINT:
				vsf_trace_info("USB_REQ_GET_ENDPOINT_STATUS: %s",
						value & (1 << 0) ? "Halted" : "");
				break;
			}
			break;
		case USB_REQ_CLEAR_FEATURE:
			switch (recip) {
			case USB_RECIP_DEVICE:
				vsf_trace_info("USB_REQ_CLEAR_DEVICE_FEATURE(%s)",
						request->wValue == 1 ? "Remote wakeup" :
						request->wValue == 2 ? "Test mode" : "Unknown");
				break;
			case USB_RECIP_ENDPOINT:
				vsf_trace_info("USB_REQ_CLEAR_ENDPOINT_FEATURE(%s)",
						request->wValue == 0 ? "Halt" : "Unknown");
				break;
			}
			break;
		case USB_REQ_SET_FEATURE:
			switch (recip) {
			case USB_RECIP_DEVICE:
				vsf_trace_info("USB_REQ_SET_DEVICE_FEATURE(%s)",
						request->wValue == 1 ? "Remote wakeup" :
						request->wValue == 2 ? "Test mode" : "Unknown");
				break;
			case USB_RECIP_ENDPOINT:
				vsf_trace_info("USB_REQ_SET_ENDPOINT_FEATURE(%s)",
						request->wValue == 0 ? "Halt" : "Unknown");
				break;
			}
			break;
		case USB_REQ_SET_ADDRESS:
			vsf_trace_info("USB_REQ_SET_ADDRESS(%d)", request->wValue);
			break;
		case USB_REQ_GET_DESCRIPTOR:
			type = (request->wValue >> 8) & 0xFF;
			value = (request->wValue >> 0) & 0xFF;
			vsf_trace_info("USB_REQ_GET_%s_DESCRIPTOR(%d)",
						type == USB_DT_DEVICE ? "DEVICE" :
						type == USB_DT_CONFIG ? "CONFIGURATION" :
						type == USB_DT_STRING ? "STRING" :
						type == USB_DT_DEVICE_QUALIFIER ? "QUALIFIER" : "Unknown", value);
			if ((type == USB_DT_STRING) && (data[0] == len)) {
				if (value == 0) {
					vsf_trace_info(": LanguageID: ");
					vsf_trace_buffer(VSF_TRACE_INFO, data + 2, len - 2);
				} else {
					char str[128];
					int i;
					for (i = 2; i < len; i += 2) {
						str[(i >> 1) - 1] = data[i];
                    }
					str[(i >> 1) - 1] = '\0';
					vsf_trace_info(": %s", str);
				}
			}
			break;
		case USB_REQ_SET_DESCRIPTOR:
			vsf_trace_info("USB_REQ_SET_DESCRIPTOR");
			break;
		case USB_REQ_GET_CONFIGURATION:
			value = (urb_status == URB_OK) ? data[0] : 0;
			vsf_trace_info("USB_REQ_GET_CONFIGURATION: %d", value);
			break;
		case USB_REQ_SET_CONFIGURATION:
			vsf_trace_info("USB_REQ_SET_CONFIGURATION(%d)", request->wValue);
			break;
		case USB_REQ_GET_INTERFACE:
			value = (urb_status == URB_OK) ? data[0] : 0;
			vsf_trace_info("USB_REQ_GET_INTERFACE: %d", value);
			break;
		case USB_REQ_SET_INTERFACE:
			vsf_trace_info("USB_REQ_SET_INTERFACE(%d)", request->wValue);
			break;
		case USB_REQ_SYNCH_FRAME:
			value = (urb_status == URB_OK) ? get_unaligned_le16(data) : 0;
			vsf_trace_info("USB_REQ_SYNCH_FRAME: %d", value);
			break;
		}
		vsf_trace_info(VSF_TRACE_CFG_LINEEND);
	}
}

static const usbmitm_plugin_op_t usbmitm_plugin_stdreq_op = {
	.parse_config = NULL,
	.on_SETUP = usbmitm_plugin_stdreq_on_SETUP,
	.on_IN = NULL,
	.on_OUT = NULL,
};

usbmitm_plugin_t usbmitm_plugin_stdreq = {
	.op = &usbmitm_plugin_stdreq_op,
};
