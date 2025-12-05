#include <vsf.h>
#include <component/usb/common/class/MSC/vsf_usb_MSC.h>

#include "./vsf_usbmitm.h"

struct usbmitm_plugin_msc_t {
	struct msc_param_t {
		uint8_t interface;
		uint8_t subclass;
		uint8_t protocol;
		uint8_t ep_out;
		uint8_t ep_in;
		uint8_t ep_size;
		uint8_t maxlun;
		enum {
			STATE_IDLE = 0, STATE_CBW, STATE_DATA, STATE_CSW,
		} state;
		uint32_t curlen;
		usb_msc_cbw_t cbw;
	} msc[8];
} static plugin_msc;

static struct msc_param_t * usbmitm_plugin_msc_get_param_byifs(uint8_t ifs)
{
	for (int i = 0; i < dimof(plugin_msc.msc); i++) {
		if (plugin_msc.msc[i].interface == ifs) {
			return &plugin_msc.msc[i];
        }
    }
	return NULL;
}

static struct msc_param_t * usbmitm_plugin_msc_get_param_byepin(uint8_t ep)
{
	for (int i = 0; i < dimof(plugin_msc.msc); i++) {
		if (plugin_msc.msc[i].ep_in == ep) {
			return &plugin_msc.msc[i];
        }
    }
	return NULL;
}

static struct msc_param_t * usbmitm_plugin_msc_get_param_byepout(uint8_t ep)
{
	for (int i = 0; i < dimof(plugin_msc.msc); i++) {
		if (plugin_msc.msc[i].ep_out == ep) {
			return &plugin_msc.msc[i];
        }
    }
	return NULL;
}

static void usbmitm_plugin_msc_parse_config(uint8_t *data, uint16_t len)
{
	uint16_t pos = USB_DT_CONFIG_SIZE;
	usb_interface_desc_t *if_desc;
	struct msc_param_t *param = plugin_msc.msc;

	for (int i = 0; i < dimof(plugin_msc.msc); i++, param++) {
		param->interface = 0xFF;
		param->ep_in = param->ep_out = param->ep_size = 0;
	}

	while (len > pos) {
		switch (data[pos + 1]) {
		case USB_DT_INTERFACE:
			if_desc = (usb_interface_desc_t *)&data[pos];
			param = (if_desc->bInterfaceClass == USB_CLASS_MASS_STORAGE) ?
				usbmitm_plugin_msc_get_param_byifs(0xFF) : NULL;
			if (param != NULL) {
				param->state = STATE_IDLE;
				param->interface = if_desc->bInterfaceNumber;
				param->subclass = if_desc->bInterfaceSubClass;
				param->protocol = if_desc->bInterfaceProtocol;
			}
			break;
		case USB_DT_ENDPOINT:
			if (param != NULL) {
				uint8_t epaddr = data[pos + 2];
				if (!param->ep_size) {
					param->ep_size = (uint8_t)get_unaligned_le16(&data[pos + 4]);
                }
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

	param = plugin_msc.msc;
	for (int i = 0; i < dimof(plugin_msc.msc); i++, param++) {
		if (param->interface != 0xFF) {
			vsf_trace_info("MSC: interface(%d), subclass(%d), protocol(%02X), ep_in(%d), ep_out(%d)" VSF_TRACE_CFG_LINEEND,
							param->interface, param->subclass, param->protocol, param->ep_in, param->ep_out);
		}
	}
}

static void usbmitm_plugin_msc_on_SETUP(struct usb_ctrlrequest_t *request,
							int16_t urb_status, uint8_t *data, uint16_t len)
{
	uint8_t recip = request->bRequestType & USB_RECIP_MASK;

	if (recip == USB_RECIP_INTERFACE) {
		uint16_t type = request->bRequestType & USB_TYPE_MASK;
		uint8_t ifs = (uint8_t)request->wIndex;
		struct msc_param_t *param = usbmitm_plugin_msc_get_param_byifs(ifs);

		if (param != NULL) {
			if (type == USB_TYPE_CLASS) {
				switch (request->bRequest) {
				case USB_MSC_REQ_GET_MAX_LUN:
					param->maxlun = (urb_status == URB_OK) ? data[0] : 0;
					vsf_trace_info("MSC: USB_MSCBOTREQ_GET_MAX_LUN: %d" VSF_TRACE_CFG_LINEEND, param->maxlun);
					break;
				case USB_MSC_REQ_RESET:
					vsf_trace_info("MSC: USB_MSCBOTREQ_RESET" VSF_TRACE_CFG_LINEEND);
					break;
				}
			}
		}
	}
}

static void usbmitm_plugin_msc_parse_cbw(usb_msc_cbw_t *cbw)
{
	uint8_t *cdb = cbw->CBWCB;
	uint8_t gcode = cdb[0] & 0xE0, ccode = cdb[0] & 0x1F;

	switch (gcode) {
	case SCSI_GROUPCODE6:
		switch (ccode) {
		case SCSI_CMDCODE_MODE_SELECT: vsf_trace_info("MODE_SELECT6"); break;
		case SCSI_CMDCODE_TEST_UNIT_READY: vsf_trace_info("TEST_UNIT_READY"); break;
		case SCSI_CMDCODE_VERIFY: vsf_trace_info("VERIFY"); break;
		case SCSI_CMDCODE_FORMAT_UNIT: vsf_trace_info("FORMAT_UNITY"); break;
		case SCSI_CMDCODE_START_STOP_UNIT: vsf_trace_info("START_STOP_UNIT"); break;
		case SCSI_CMDCODE_ALLOW_MEDIUM_REMOVAL: vsf_trace_info("ALLOW_MEDIUM_REMOVAL"); break;
		case SCSI_CMDCODE_REQUEST_SENSE: vsf_trace_info("REQUEST_SENSE"); break;
		case SCSI_CMDCODE_READ: vsf_trace_info("READ6"); break;
		case SCSI_CMDCODE_WRITE: vsf_trace_info("WRITE6"); break;
		case SCSI_CMDCODE_INQUIRY: vsf_trace_info("INQUIRY"); break;
		case SCSI_CMDCODE_MODE_SENSE: vsf_trace_info("MODE_SENSE6"); break;
		default: vsf_trace_info("UNKNOWN"); break;
		}
		break;
	case SCSI_GROUPCODE10_1:
		switch (ccode) {
		case SCSI_CMDCODE_READ_FORMAT_CAPACITIES: vsf_trace_info("READ_FORMAT_CAPACITIES"); break;
		case SCSI_CMDCODE_READ_CAPACITY: vsf_trace_info("READ_CAPACITY"); break;
		case SCSI_CMDCODE_READ: vsf_trace_info("READ10"); break;
		case SCSI_CMDCODE_WRITE: vsf_trace_info("WRITE10"); break;
		default: vsf_trace_info("UNKNOWN"); break;
		}
		break;
	case SCSI_GROUPCODE10_2:
		switch (ccode) {
		case SCSI_CMDCODE_MODE_SELECT: vsf_trace_info("MODE_SELECT10"); break;
		case SCSI_CMDCODE_MODE_SENSE: vsf_trace_info("MODE_SENSE10"); break;
		default: vsf_trace_info("UNKNOWN"); break;
		}
		break;
	case SCSI_GROUPCODE16:
		switch (ccode) {
		case SCSI_CMDCODE_READ: vsf_trace_info("READ16"); break;
		case SCSI_CMDCODE_WRITE: vsf_trace_info("WRITE16"); break;
		default: vsf_trace_info("UNKNOWN"); break;
		}
		break;
	case SCSI_GROUPCODE12:
		switch (ccode) {
		case SCSI_CMDCODE_READ: vsf_trace_info("READ12"); break;
		case SCSI_CMDCODE_WRITE: vsf_trace_info("WRITE12"); break;
		default: vsf_trace_info("UNKNOWN"); break;
		}
		break;
	default:
		vsf_trace_info("Unknown group code: %d", gcode);
		break;
	}
}

static void usbmitm_plugin_msc_parse_csw(usb_msc_cbw_t *cbw, usb_msc_csw_t *csw)
{
	if (csw->dCSWSignature != USB_MSC_CSW_SIGNATURE) {
		vsf_trace_info("Invalid CSW Signature: %04X", csw->dCSWSignature);
		return;
	}
	if (csw->dCSWTag != cbw->dCBWTag) {
		vsf_trace_info("Invalid CSW Tag: %04X", csw->dCSWTag);
		return;
	}
	vsf_trace_info("CSW Result: %d", csw->dCSWStatus);
}

static void usbmitm_plugin_msc_parse_data(usb_msc_cbw_t *cbw, uint8_t *data, uint16_t len)
{
	
}

void usbmitm_plugin_msc_on_IN(uint8_t ep, int16_t urb_status, uint8_t *data, uint16_t len)
{
	struct msc_param_t *param = usbmitm_plugin_msc_get_param_byepin(ep);

	if (param) {
		vsf_trace_info("MSC: ");
		switch (param->state) {
		case STATE_CBW:
			param->curlen += len;
			if (    (len < param->ep_size)
                ||  (param->curlen >= param->cbw.dCBWDataTransferLength)
                ||  (urb_status != URB_OK)) {
                if ((param->curlen <= param->ep_size) && (urb_status == URB_OK)) {
					usbmitm_plugin_msc_parse_data(&param->cbw, data, len);
                }
				param->state++;
			}
			break;
		case STATE_DATA:
			usbmitm_plugin_msc_parse_csw(&param->cbw, (usb_msc_csw_t *)data);
			param->state = STATE_IDLE;
			break;
		default:
			vsf_trace_info("Invalid Sequence");
			break;
		}
		vsf_trace_info(VSF_TRACE_CFG_LINEEND);
	}
}

void usbmitm_plugin_msc_on_OUT(uint8_t ep, int16_t urb_status, uint8_t *data, uint16_t len)
{
	struct msc_param_t *param = usbmitm_plugin_msc_get_param_byepout(ep);

	if (param) {
		vsf_trace_info("MSC: ");
		switch (param->state) {
		case STATE_IDLE:
			if (len != sizeof(param->cbw)) {
				vsf_trace_info("Invalid CBW Length");
				break;
			}
			param->cbw = *(usb_msc_cbw_t *)data;
			param->curlen = 0;
			if (param->cbw.dCBWSignature != USB_MSC_CBW_SIGNATURE) {
				vsf_trace_info("Invalid CBW Signature: %04X", param->cbw.dCBWSignature);
				break;
			}
			if ((param->cbw.bCBWCBLength < 1) || (param->cbw.bCBWCBLength > 16)) {
				vsf_trace_info("Invalid CBW Length: %d", param->cbw.bCBWCBLength);
				break;
			}
			if (param->cbw.bCBWLUN > param->maxlun) {
				vsf_trace_info("Invalid CBW Lun: %d", param->cbw.bCBWLUN);
				break;
			}

			param->state++;
			vsf_trace_info("CBW(");
			usbmitm_plugin_msc_parse_cbw(&param->cbw);
			vsf_trace_info("): ");
			vsf_trace_buffer(VSF_TRACE_INFO, param->cbw.CBWCB, param->cbw.bCBWCBLength);

			if (!param->cbw.dCBWDataTransferLength)
				param->state++;
			break;
		case STATE_CBW:
			param->curlen += len;
			if (    (len < param->ep_size)
                ||  (param->curlen >= param->cbw.dCBWDataTransferLength)) {
                if (param->curlen <= param->ep_size) {
					usbmitm_plugin_msc_parse_data(&param->cbw, data, len);
                }
				param->state++;
			}
			break;
		default:
			vsf_trace_info("Invalid Sequence");
			break;
		}
		vsf_trace_info(VSF_TRACE_CFG_LINEEND);
	}
}

const usbmitm_plugin_op_t usbmitm_plugin_msc_op = {
	.parse_config = usbmitm_plugin_msc_parse_config,
	.on_SETUP = usbmitm_plugin_msc_on_SETUP,
	.on_IN = usbmitm_plugin_msc_on_IN,
	.on_OUT = usbmitm_plugin_msc_on_OUT,
};

usbmitm_plugin_t usbmitm_plugin_msc = {
	.op = &usbmitm_plugin_msc_op,
};
