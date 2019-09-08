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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_DS4 == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_HID_INHERIT
#define VSF_USBH_DS4_IMPLEMENT
#include "vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_usbh_dev_id_t vsf_usbh_ds4_id_table[] = {
    {
        .match_vendor = true,
        .match_product = true,
        .match_int_class = true,
        .idVendor = 0x054c,
        .idProduct = 0x05c4,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        .match_vendor = true,
        .match_product = true,
        .match_int_class = true,
        .idVendor = 0x054c,
        .idProduct = 0x09cc,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        .match_vendor = true,
        .match_product = true,
        .match_int_class = true,
        .idVendor = 0x054c,
        .idProduct = 0x0ba0,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        .match_vendor = true,
        .match_product = true,
        .match_int_class = true,
        .idVendor = 0x0738,
        .idProduct = 0x8250,
        .bInterfaceClass = USB_CLASS_HID,
    },
};

/*============================ PROTOTYPES ====================================*/

static void * vsf_usbh_ds4_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
            vsf_usbh_ifs_parser_t *parser_ifs);
static void vsf_usbh_ds4_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param);

#if     defined(WEAK_VSF_USBH_DS4_ON_REPORT_INPUT_EXTERN)                       \
    &&  defined(WEAK_VSF_USBH_DS4_ON_REPORT_INPUT)
WEAK_VSF_USBH_DS4_ON_REPORT_INPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_DS4_ON_NEW_EXTERN)                                \
    &&  defined(WEAK_VSF_USBH_DS4_ON_NEW)
WEAK_VSF_USBH_DS4_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_DS4_ON_FREE_EXTERN)                               \
    &&  defined(WEAK_VSF_USBH_DS4_ON_FREE)
WEAK_VSF_USBH_DS4_ON_FREE_EXTERN
#endif

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_usbh_class_drv_t vsf_usbh_ds4_drv = {
    .name       = "ds4",
    .dev_id_num = dimof(vsf_usbh_ds4_id_table),
    .dev_ids    = vsf_usbh_ds4_id_table,
    .probe      = vsf_usbh_ds4_probe,
    .disconnect = vsf_usbh_ds4_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
WEAK(vsf_usbh_ds4_on_report_input)
void vsf_usbh_ds4_on_report_input(vsf_usbh_ds4_t *ds4, vsf_usb_ds4_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT_DS4 == ENABLED
    vsf_ds4u_process_input(&ds4->use_as__vsf_input_ds4u_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_NEW
WEAK(vsf_usbh_ds4_on_new)
void vsf_usbh_ds4_on_new(vsf_usbh_ds4_t *ds4)
{
#   if VSF_USE_INPUT_DS4 == ENABLED
    vsf_ds4u_new_dev(&ds4->use_as__vsf_input_ds4u_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_FREE
WEAK(vsf_usbh_ds4_on_free)
void vsf_usbh_ds4_on_free(vsf_usbh_ds4_t *ds4)
{
#   if VSF_USE_INPUT_DS4 == ENABLED
    vsf_ds4u_free_dev(&ds4->use_as__vsf_input_ds4u_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_GET_OUTPUT_REPORT
WEAK(vsf_usbh_ds4_get_output_report)
int_fast32_t vsf_usbh_ds4_get_output_report(vsf_usbh_ds4_t *ds4, vsf_usb_ds4_gamepad_out_report_t *report)
{
    return 0;
}
#endif

static void vsf_usbh_ds4_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_ds4_t *ds4 = (vsf_usbh_ds4_t *)container_of(eda, vsf_usbh_hid_eda_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        {
            vsf_usbh_ep0_t *ep0 = ds4->ep0;
            vsf_usbh_urb_t *urb = &ep0->urb;

            vsf_usbh_urb_free_buffer(urb);
            vsf_usbh_hid_set_idle(ds4, 0, 0);

            vsf_teda_set_timer_ms(1000);
        }
        break;
    case VSF_EVT_MESSAGE:
        {
            vsf_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vsf_usbh_eppipe_t pipe;

            pipe = vsf_usbh_urb_get_pipe(&urb);
            if (USB_ENDPOINT_XFER_CONTROL == pipe.type) {
                vsf_usbh_hid_recv_report((vsf_usbh_hid_eda_t *)&ds4->use_as__vsf_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
                vsf_usbh_hid_send_report((vsf_usbh_hid_eda_t *)&ds4->use_as__vsf_usbh_hid_teda_t, (uint8_t *)&ds4->gamepad_out_buf, sizeof(ds4->gamepad_out_buf));
            } else /* if (USB_ENDPOINT_XFER_INT == pipe.type) */ {
                if (pipe.dir_in1out0) {
                    if (    (URB_OK == vsf_usbh_urb_get_status(&urb))
                        &&    (sizeof(vsf_usb_ds4_gamepad_in_report_t) == vsf_usbh_urb_get_actual_length(&urb))) {
#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
                        vsf_usbh_ds4_on_report_input(ds4, (vsf_usb_ds4_gamepad_in_report_t *)vsf_usbh_urb_peek_buffer(&urb));
#else
                        WEAK_VSF_USBH_DS4_ON_REPORT_INPUT(ds4, (vsf_usb_ds4_gamepad_in_report_t *)vsf_usbh_urb_peek_buffer(&urb));
#endif
                    }
                    vsf_usbh_hid_recv_report((vsf_usbh_hid_eda_t *)&ds4->use_as__vsf_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
                } else {
                    ds4->out_idle = true;
                }
            }

            if (ds4->out_idle && (vsf_usbh_ds4_get_output_report(ds4, &ds4->gamepad_out_buf) > 0)) {
                ds4->out_idle = false;
                vsf_usbh_hid_send_report((vsf_usbh_hid_eda_t *)&ds4->use_as__vsf_usbh_hid_teda_t, (uint8_t *)&ds4->gamepad_out_buf, sizeof(ds4->gamepad_out_buf));
            }
        }
        break;
    }
}

static void * vsf_usbh_ds4_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev,
            vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_ds4_t *ds4 = vsf_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vsf_usbh_ds4_t));
    if (ds4 != NULL) {
        ds4->gamepad_out_buf.id = 0x05;
        ds4->gamepad_out_buf.dummy1[0] = 0xff;
        ds4->gamepad_out_buf.led_r = 0x01;
        ds4->gamepad_out_buf.led_g = 0x00;
        ds4->gamepad_out_buf.led_b = 0x01;
        ds4->user_evthandler = vsf_usbh_ds4_evthandler;
    }
    return ds4;
}

static void vsf_usbh_ds4_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
    vsf_usbh_ds4_on_report_input(param, NULL);
#else
    WEAK_VSF_USBH_DS4_ON_REPORT_INPUT(param, NULL);
#endif

    vsf_usbh_hid_disconnect((vsf_usbh_hid_eda_t *)param);
    VSF_USBH_FREE(param);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_DS4
