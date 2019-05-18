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

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if VSF_USE_USB_DEVICE == ENABLED

#define VSF_USBD_INHERIT
#define VSF_USBD_CDC_INHERIT
#define VSF_USBD_CDCACM_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
#undef this
#define this        (*obj)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_usbd_CMDACM_init( vsf_usbd_CDCACM_t *obj, 
                                const vsf_usbd_CDCACM_cfg_t *cfg)
{
    ASSERT(NULL != obj && NULL != cfg);

    memset(obj, 0, sizeof(vsf_usbd_CDCACM_t));
    this.ep_cfg = cfg->ep_cfg;


    this.line_coding = (usb_CDCACM_line_coding_t){
                .bitrate        = 115200,
                .stop           = 0,
                .parity         = 0,
                .datalen        = 8,
            };
#if     VSF_USE_SERVICE_VSFSTREAM == ENABLED
    ASSERT(     (NULL != cfg->rx_stream) 
            &&  (NULL != cfg->tx_stream));

    this.stream.rx.stream = cfg->rx_stream;
    this.stream.tx.stream = cfg->tx_stream;

#elif   VSF_USE_SERVICE_STREAM == ENABLED
    vsf_stream_usr_init(&(this.stream.use_as__vsf_stream_usr_t),
                        &cfg->stream_usr);
    vsf_stream_src_init(&(this.stream.use_as__vsf_stream_src_t),
                        &cfg->stream_src);

    do {
        vsf_usbd_ep_stream_cfg_t cfg = {
            .rx_ep = this.ep.out,
            .tx_ep = this.ep.in,
        };
        vsf_usbd_ep_stream_init(&this.stream.use_as__vsf_usbd_ep_stream_t, &cfg);
        
    } while(0);
#endif
    return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_CDCACM_data_init(vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
	vsf_usbd_CDCACM_t *acm = (vsf_usbd_CDCACM_t *)ifs->class_param;

	acm->control_line = 0;
	if (    (acm->callback.set_line_coding != NULL)
        &&  (VSF_ERR_NONE != acm->callback.set_line_coding(&acm->line_coding))) {
		return VSF_ERR_FAIL;
	}
	return vsf_usbd_CDC_data.init(dev, ifs);
}

static vsf_err_t vsf_usbd_CDCACM_control_request_prepare(
		vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
	vsf_usbd_CDCACM_t *acm = (vsf_usbd_CDCACM_t *)ifs->class_param;
	vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
	struct usb_ctrlrequest_t *request = &ctrl_handler->request;
	usb_CDCACM_line_coding_t *line_coding = &acm->line_coding;
    uint8_t *buffer = NULL;
    uint_fast32_t size = 0;

	switch (request->bRequest) {
	case USB_CDCACMREQ_SET_LINE_CODING:
		if ((request->wLength != 7) || (request->wValue != 0)) {
			return VSF_ERR_FAIL;
		}

        buffer = (uint8_t *)line_coding;
        size = 7;
		break;
	case USB_CDCACMREQ_GET_LINE_CODING:
		if ((request->wLength != 7) || (request->wValue != 0)) {
			return VSF_ERR_FAIL;
		}

        cpu_to_le32s(&acm->line_coding.bitrate);
		buffer = (uint8_t *)line_coding;
        size = 7;
		break;
	case USB_CDCACMREQ_SET_CONTROL_LINE_STATE:
		if (    (request->wLength != 0)
            ||  (request->wValue & ~USB_CDCACM_CONTROLLINE_MASK)) {
			return VSF_ERR_FAIL;
		}

		acm->control_line = (uint8_t)request->wValue;
		if (    (acm->callback.set_control_line != NULL)
            &&  (VSF_ERR_NONE != acm->callback.set_control_line(acm->control_line))) {
			return VSF_ERR_FAIL;
		}
		break;
	case USB_CDCACMREQ_SEND_BREAK:
		if (    (request->wLength != 0)
            ||  (   (acm->callback.send_break != NULL)
                &&  acm->callback.send_break())) {
			return VSF_ERR_FAIL;
		}
		break;
	default:
		return vsf_usbd_CDC_control.request_prepare(dev, ifs);
	}

    ctrl_handler->trans.pchBuffer = buffer;
    ctrl_handler->trans.nSize = size;
	return VSF_ERR_NONE;
}

static vsf_err_t vsf_usbd_CDCACM_control_request_process(
		vsf_usbd_dev_t *dev, vsf_usbd_ifs_t *ifs)
{
	vsf_usbd_CDCACM_t *acm = (vsf_usbd_CDCACM_t *)ifs->class_param;
	vsf_usbd_ctrl_handler_t *ctrl_handler = &dev->ctrl_handler;
	struct usb_ctrlrequest_t *request = &ctrl_handler->request;

    switch (request->bRequest) {
	case USB_CDCACMREQ_SET_LINE_CODING:
        le32_to_cpus(&acm->line_coding.bitrate);
        if (    (acm->callback.set_line_coding != NULL)
            &&  (VSF_ERR_NONE != acm->callback.set_line_coding(&acm->line_coding))) {
            return VSF_ERR_FAIL;
        }
        vsf_usbd_CDC_data_connect(&acm->use_as__vsf_usbd_CDC_t);
        break;
    case USB_CDCACMREQ_GET_LINE_CODING:
        le32_to_cpus(&acm->line_coding.bitrate);
	case USB_CDCACMREQ_SET_CONTROL_LINE_STATE:
    case USB_CDCACMREQ_SEND_BREAK:
        break;
    default:
		return vsf_usbd_CDC_control.request_process(dev, ifs);
    }
	return VSF_ERR_NONE;
}

const vsf_usbd_class_op_t vsf_usbd_CDCACM_control =
{
	.request_prepare = vsf_usbd_CDCACM_control_request_prepare,
	.request_process = vsf_usbd_CDCACM_control_request_process,
};

const vsf_usbd_class_op_t vsf_usbd_CDCACM_data =
{
	.init = vsf_usbd_CDCACM_data_init,
};

#endif  // VSF_USE_USB_DEVICE
