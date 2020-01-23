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

static const vk_usbh_dev_id_t vk_usbh_ds4_id_table[] = {
    {
        VSF_USBH_MATCH_VID_PID(0x054C, 0x05C4)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        VSF_USBH_MATCH_VID_PID(0x054C, 0x05C5)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        VSF_USBH_MATCH_VID_PID(0x054C, 0x09CC)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        VSF_USBH_MATCH_VID_PID(0x054C, 0x0BA0)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
    {
        VSF_USBH_MATCH_VID_PID(0x0738, 0x8250)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
};

/*============================ PROTOTYPES ====================================*/

static void * vk_usbh_ds4_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs);
static void vk_usbh_ds4_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

#if     defined(WEAK_VSF_USBH_DS4_ON_REPORT_INPUT_EXTERN)                       \
    &&  defined(WEAK_VSF_USBH_DS4_ON_REPORT_INPUT)
WEAK_VSF_USBH_DS4_ON_REPORT_INPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT_EXTERN)                      \
    &&  defined(WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT)
WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT_EXTERN
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

const vk_usbh_class_drv_t vk_usbh_ds4_drv = {
    .name       = "ds4",
    .dev_id_num = dimof(vk_usbh_ds4_id_table),
    .dev_ids    = vk_usbh_ds4_id_table,
    .probe      = vk_usbh_ds4_probe,
    .disconnect = vk_usbh_ds4_disconnect,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
WEAK(vsf_usbh_ds4_on_report_input)
void vsf_usbh_ds4_on_report_input(vk_usbh_ds4_t *ds4, vsf_usb_ds4_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_DS4 == ENABLED
    vk_ds4u_process_input(&ds4->use_as__vk_input_ds4u_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT
WEAK(vsf_usbh_ds4_on_report_output)
void vsf_usbh_ds4_on_report_output(vk_usbh_ds4_t *ds4)
{
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_NEW
WEAK(vsf_usbh_ds4_on_new)
void vsf_usbh_ds4_on_new(vk_usbh_ds4_t *ds4)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_DS4 == ENABLED
    vk_ds4u_new_dev(&ds4->use_as__vk_input_ds4u_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_FREE
WEAK(vsf_usbh_ds4_on_free)
void vsf_usbh_ds4_on_free(vk_usbh_ds4_t *ds4)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_DS4 == ENABLED
    vk_ds4u_free_dev(&ds4->use_as__vk_input_ds4u_t);
#   endif
}
#endif

static void vk_usbh_ds4_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_ds4_t *ds4 = (vk_usbh_ds4_t *)container_of(eda, vk_usbh_hid_eda_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT: {
            vk_usbh_ep0_t *ep0 = ds4->ep0;
            vk_usbh_urb_t *urb = &ep0->urb;

            vk_usbh_urb_free_buffer(urb);
            vk_usbh_hid_set_idle(ds4, 0, 0);

            // TODO: remove timer and use eda instead of teda
            vsf_teda_set_timer_ms(1000);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (USB_ENDPOINT_XFER_CONTROL == pipe.type) {
                vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&ds4->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
                vk_usbh_hid_send_report((vk_usbh_hid_eda_t *)&ds4->use_as__vk_usbh_hid_teda_t, (uint8_t *)&ds4->gamepad_out_buf, sizeof(ds4->gamepad_out_buf));
            } else /* if (USB_ENDPOINT_XFER_INT == pipe.type) */ {
                if (pipe.dir_in1out0) {
                    if (    (URB_OK == vk_usbh_urb_get_status(&urb))
                        &&  (sizeof(vsf_usb_ds4_gamepad_in_report_t) == vk_usbh_urb_get_actual_length(&urb))) {
#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
                        vsf_usbh_ds4_on_report_input(ds4, (vsf_usb_ds4_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
#else
                        WEAK_VSF_USBH_DS4_ON_REPORT_INPUT(ds4, (vsf_usb_ds4_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
#endif
                    }
                    vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&ds4->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
                } else {
                    ds4->out_idle = true;
#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT
                    vsf_usbh_ds4_on_report_output(ds4);
#else
                    WEAK_VSF_USBH_DS4_ON_REPORT_OUTPUT(ds4);
#endif
                }
            }
        }
        break;
    }
}

static void * vk_usbh_ds4_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ds4_t *ds4 = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_ds4_t), true);
    if (ds4 != NULL) {
        ds4->gamepad_out_buf.id = 0x05;
        ds4->gamepad_out_buf.dummyFF = 0xff;
        ds4->gamepad_out_buf.led_r = 0x01;
        ds4->gamepad_out_buf.led_g = 0x00;
        ds4->gamepad_out_buf.led_b = 0x01;
        ds4->user_evthandler = vk_usbh_ds4_evthandler;

#ifndef WEAK_VSF_USBH_DS4_ON_NEW
        vsf_usbh_ds4_on_new(ds4);
#else
        WEAK_VSF_USBH_DS4_ON_NEW(ds4);
#endif
    }
    return ds4;
}

static void vk_usbh_ds4_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
#ifndef WEAK_VSF_USBH_DS4_ON_FREE
    vsf_usbh_ds4_on_free((vk_usbh_ds4_t *)param);
#else
    WEAK_VSF_USBH_DS4_ON_FREE(ds4);
#endif
    vk_usbh_hid_disconnect((vk_usbh_hid_eda_t *)param);
}

bool vk_usbh_ds4_can_output(vk_usbh_ds4_t *ds4)
{
    return ds4->out_idle;
}

static void vk_usbd_ds4_output(vk_usbh_ds4_t *ds4)
{
    ds4->out_idle = false;
    vk_usbh_hid_send_report((vk_usbh_hid_eda_t *)&ds4->use_as__vk_usbh_hid_teda_t, (uint8_t *)&ds4->gamepad_out_buf, sizeof(ds4->gamepad_out_buf));
}

void vk_usbh_ds4_set_rumble(vk_usbh_ds4_t *ds4, uint_fast8_t left, uint_fast8_t right)
{
    VSF_USB_ASSERT(ds4->out_idle);
    ds4->gamepad_out_buf.rumble_r = right;
    ds4->gamepad_out_buf.rumble_l = left;
    vk_usbd_ds4_output(ds4);
}

void vk_usbh_ds4_set_led(vk_usbh_ds4_t *ds4, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b)
{
    VSF_USB_ASSERT(ds4->out_idle);
    ds4->gamepad_out_buf.led_r = r;
    ds4->gamepad_out_buf.led_g = g;
    ds4->gamepad_out_buf.led_b = b;
    vk_usbd_ds4_output(ds4);
}

#endif      // VSF_USE_USB_HOST && VSF_USE_USB_HOST_DS4
