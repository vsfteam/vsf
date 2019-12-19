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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_NSPRO == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_HID_INHERIT
#define VSF_USBH_NSPRO_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t vk_usbh_nspro_id_table[] = {
    {
        VSF_USBH_MATCH_VID_PID(0x057E, 0x2009)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
};

/*============================ PROTOTYPES ====================================*/

static void * vk_usbh_nspro_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs);
static void vk_usbh_nspro_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

#if     defined(WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT_EXTERN)                     \
    &&  defined(WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT)
WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT_EXTERN)                    \
    &&  defined(WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT)
WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_NSPRO_ON_NEW_EXTERN)                              \
    &&  defined(WEAK_VSF_USBH_NSPRO_ON_NEW)
WEAK_VSF_USBH_NSPRO_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_NSPRO_ON_FREE_EXTERN)                             \
    &&  defined(WEAK_VSF_USBH_NSPRO_ON_FREE)
WEAK_VSF_USBH_NSPRO_ON_FREE_EXTERN
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_nspro_drv = {
    .name       = "nspro",
    .dev_id_num = dimof(vk_usbh_nspro_id_table),
    .dev_ids    = vk_usbh_nspro_id_table,
    .probe      = vk_usbh_nspro_probe,
    .disconnect = vk_usbh_nspro_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT
WEAK(vsf_usbh_nspro_on_report_input)
void vsf_usbh_nspro_on_report_input(vk_usbh_nspro_t *nspro, vsf_usb_nspro_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_NSPRO == ENABLED
    vk_nspro_process_input(&nspro->use_as__vk_input_nspro_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT
WEAK(vsf_usbh_nspro_on_report_output)
void vsf_usbh_nspro_on_report_output(vk_usbh_nspro_t *nspro)
{
}
#endif

#ifndef WEAK_VSF_USBH_NSPRO_ON_NEW
WEAK(vsf_usbh_nspro_on_new)
void vsf_usbh_nspro_on_new(vk_usbh_nspro_t *nspro)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_NSPRO == ENABLED
    vk_nspro_new_dev(&nspro->use_as__vk_input_nspro_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_NSPRO_ON_FREE
WEAK(vsf_usbh_nspro_on_free)
void vsf_usbh_nspro_on_free(vk_usbh_nspro_t *nspro)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_NSPRO == ENABLED
    vk_nspro_free_dev(&nspro->use_as__vk_input_nspro_t);
#   endif
}
#endif

static void vk_usbh_nspro_send_usb_cmd(vk_usbh_nspro_t *nspro, uint_fast8_t cmd)
{
    nspro->gamepad_out_buf.buffer[0] = 0x80;
    nspro->gamepad_out_buf.buffer[1] = cmd;
    vk_usbh_hid_send_report((vk_usbh_hid_eda_t *)&nspro->use_as__vk_usbh_hid_teda_t, (uint8_t *)&nspro->gamepad_out_buf, 2);
}

static void vk_usbh_nspro_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_nspro_t *nspro = (vk_usbh_nspro_t *)container_of(eda, vk_usbh_hid_eda_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT: {
            vk_usbh_ep0_t *ep0 = nspro->ep0;
            vk_usbh_urb_t *urb = &ep0->urb;

            vk_usbh_urb_free_buffer(urb);
            vk_usbh_hid_set_idle(nspro, 0, 0);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (USB_ENDPOINT_XFER_CONTROL == pipe.type) {
                nspro->start_state = VSF_USBH_NSPRO_GET_INFO;
                vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&nspro->use_as__vk_usbh_hid_teda_t, NULL, 64);
                vk_usbh_nspro_send_usb_cmd(nspro, 1);
            } else /* if (USB_ENDPOINT_XFER_INT == pipe.type) */ {
                if (pipe.dir_in1out0) {
                    uint8_t *buffer = vk_usbh_urb_peek_buffer(&urb);
                    if (buffer != NULL) {
                        switch (nspro->start_state) {
                        case VSF_USBH_NSPRO_GET_INFO:
                            if (    (URB_OK != vk_usbh_urb_get_status(&urb))
                                ||  (buffer[0] != 0x81) || (buffer[1] != 0x01)) {
                                break;
                            }

                            nspro->type = buffer[3];
                            memcpy(nspro->mac, &buffer[4], 6);

                            nspro->start_state++;
                            vk_usbh_nspro_send_usb_cmd(nspro, 2);
                            break;
                        case VSF_USBH_NSPRO_HANDSHAKE:
                            if (    (URB_OK != vk_usbh_urb_get_status(&urb))
                                ||  (buffer[0] != 0x81) || (buffer[1] != 0x02)) {
                                break;
                            }
                            nspro->start_state++;
                            vk_usbh_nspro_send_usb_cmd(nspro, 4);
                            break;
                        case VSF_USBH_NSPRO_RUNNING:
                            if (URB_OK == vk_usbh_urb_get_status(&urb)) {
#ifndef WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT
                                vsf_usbh_nspro_on_report_input(nspro, (vsf_usb_nspro_gamepad_in_report_t *)buffer);
#else
                                WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT(nspro, (vsf_usb_nspro_gamepad_in_report_t *)buffer);
#endif
                            }
                            break;
                        }
                        vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&nspro->use_as__vk_usbh_hid_teda_t, NULL, 64);
                    }
                } else {
                    if (VSF_USBH_NSPRO_RUNNING == nspro->start_state) {
                        nspro->out_idle = true;
#ifndef WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT
                        vsf_usbh_nspro_on_report_output(nspro);
#else
                        WEAK_VSF_USBH_NSPRO_ON_REPORT_OUTPUT(nspro);
#endif
                    }
                }
            }
        }
        break;
    }
}

static void * vk_usbh_nspro_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_nspro_t *nspro = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_nspro_t), true);
    if (nspro != NULL) {
        nspro->user_evthandler = vk_usbh_nspro_evthandler;
#ifndef WEAK_VSF_USBH_NSPRO_ON_NEW
        vsf_usbh_nspro_on_new(nspro);
#else
        WEAK_VSF_USBH_NSPRO_ON_NEW(nspro);
#endif
    }
    return nspro;
}

static void vk_usbh_nspro_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
#ifndef WEAK_VSF_USBH_NSPRO_ON_FREE
    vsf_usbh_nspro_on_free((vk_usbh_nspro_t *)param);
#else
    WEAK_VSF_USBH_NSPRO_ON_FREE(nspro);
#endif
    vk_usbh_hid_disconnect((vk_usbh_hid_eda_t *)param);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_NSPRO
