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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_XB1 == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_XB1_IMPLEMENT
#define VSF_USBH_HID_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_usbh_dev_id_t vsf_usbh_xb1_dev_id[] = {
    { VSF_USBH_MATCH_VID_PID(0x045E, 0x02D1) },
    { VSF_USBH_MATCH_VID_PID(0x045E, 0x02DD) },
    { VSF_USBH_MATCH_VID_PID(0x045E, 0x02E3) },
    { VSF_USBH_MATCH_VID_PID(0x045E, 0x02EA) },
    { VSF_USBH_MATCH_VID_PID(0x0738, 0x4A01) },
    { VSF_USBH_MATCH_VID_PID(0x0E6F, 0x0139) },
    { VSF_USBH_MATCH_VID_PID(0x0E6F, 0x0146) },
    { VSF_USBH_MATCH_VID_PID(0x0F0D, 0x0067) },
    { VSF_USBH_MATCH_VID_PID(0x1532, 0x0A03) },
    { VSF_USBH_MATCH_VID_PID(0x24C6, 0x541A) },
    { VSF_USBH_MATCH_VID_PID(0x24C6, 0x542A) },
    { VSF_USBH_MATCH_VID_PID(0x24C6, 0x543A) },
};

/*============================ PROTOTYPES ====================================*/

static void *vsf_usbh_xb1_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, vsf_usbh_ifs_parser_t *parser_ifs);
static void vsf_usbh_xb1_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vsf_usbh_class_drv_t vsf_usbh_xb1_drv = {
    .name       = "xb1",
    .dev_id_num = dimof(vsf_usbh_xb1_dev_id),
    .dev_ids    = vsf_usbh_xb1_dev_id,
    .probe      = vsf_usbh_xb1_probe,
    .disconnect = vsf_usbh_xb1_disconnect,
};

/*============================ PROTOTYPES ====================================*/

SECTION(".text.vsf.kernel.eda")
vsf_err_t __vsf_eda_fini(vsf_eda_t *pthis);

#if     defined(WEAK_VSF_USBH_XB1_ON_REPORT_INPUT_EXTERN)                     \
    &&  defined(WEAK_VSF_USBH_XB1_ON_REPORT_INPUT)
WEAK_VSF_USBH_XB1_ON_REPORT_INPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT_EXTERN)                      \
    &&  defined(WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT)
WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB1_ON_NEW_EXTERN)                              \
    &&  defined(WEAK_VSF_USBH_XB1_ON_NEW)
WEAK_VSF_USBH_XB1_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB1_ON_FREE_EXTERN)                             \
    &&  defined(WEAK_VSF_USBH_XB1_ON_FREE)
WEAK_VSF_USBH_XB1_ON_FREE_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_INPUT
WEAK(vsf_usbh_xb1_on_report_input)
void vsf_usbh_xb1_on_report_input(vsf_usbh_xb1_t *xb1, vsf_usb_xb1_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT_XB1 == ENABLED
    vsf_xb1_process_input(&xb1->use_as__vsf_input_xb1_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT
WEAK(vsf_usbh_xb1_on_report_output)
void vsf_usbh_xb1_on_report_output(vsf_usbh_xb1_t *xb1)
{
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_NEW
WEAK(vsf_usbh_xb1_on_new)
void vsf_usbh_xb1_on_new(vsf_usbh_xb1_t *xb1)
{
#   if VSF_USE_INPUT_XB1 == ENABLED
    vsf_xb1_new_dev(&xb1->use_as__vsf_input_xb1_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_FREE
WEAK(vsf_usbh_xb1_on_free)
void vsf_usbh_xb1_on_free(vsf_usbh_xb1_t *xb1)
{
#   if VSF_USE_INPUT_XB1 == ENABLED
    vsf_xb1_free_dev(&xb1->use_as__vsf_input_xb1_t);
#   endif
}
#endif

static void vsf_usbh_xb1_output(vsf_usbh_xb1_t *xb1, uint_fast8_t len)
{
    VSF_USB_ASSERT(xb1->out_idle);
    xb1->out_idle = false;
    vsf_usbh_hid_send_report((vsf_usbh_hid_eda_t *)&xb1->use_as__vsf_usbh_hid_teda_t, (uint8_t *)&xb1->gamepad_out_buf, len);
}

static void vsf_usbh_xb1_submit_cmd(vsf_usbh_xb1_t *xb1, uint_fast8_t cmd, uint_fast8_t sub_cmd, uint_fast8_t seq, const uint8_t *data, uint8_t len)
{
    VSF_USB_ASSERT(xb1->out_idle && ((4 + len) < sizeof(xb1->gamepad_out_buf)));
    xb1->gamepad_out_buf.buffer[0] = cmd;
    xb1->gamepad_out_buf.buffer[1] = sub_cmd;
    xb1->gamepad_out_buf.buffer[2] = seq;
    xb1->gamepad_out_buf.buffer[3] = len;
    memcpy(&xb1->gamepad_out_buf.buffer[4], data, len);
    vsf_usbh_xb1_output(xb1, 4 + len);
}

static void vsf_usbh_xb1_ack_home(vsf_usbh_xb1_t *xb1)
{
    const uint8_t ack_home_reply[] = {
        0x00, 0x07, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    vsf_usbh_xb1_submit_cmd(xb1, 0x01, 0x20, xb1->home_seq, ack_home_reply, sizeof(ack_home_reply));
}

static void vsf_usbh_xb1_start(vsf_usbh_xb1_t *xb1)
{
    const uint8_t start[] = { 0x00 };
    vsf_usbh_xb1_submit_cmd(xb1, 0x05, 0x20, 0x00, start, sizeof(start));
}

static void vsf_usbh_xb1_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vsf_usbh_xb1_t *xb1 = (vsf_usbh_xb1_t *)container_of(eda, vsf_usbh_hid_eda_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        xb1->out_idle = true;
        vsf_usbh_hid_recv_report((vsf_usbh_hid_eda_t *)&xb1->use_as__vsf_usbh_hid_teda_t, NULL, 64);

        // TODO: use timer?
        break;
    case VSF_EVT_MESSAGE: {
            vsf_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vsf_usbh_eppipe_t pipe;

            pipe = vsf_usbh_urb_get_pipe(&urb);
            if (pipe.dir_in1out0) {
                if (URB_OK == vsf_usbh_urb_get_status(&urb)) {
                    uint_fast32_t len = vsf_usbh_urb_get_actual_length(&urb);
                    uint8_t *data = vsf_usbh_urb_peek_buffer(&urb);

                    if (0 == len) {
                    } else if ((32 == len) && (data[0] == 0x02) && (data[1] == 0x20)) {
                        // Pairing request
                        vsf_usbh_xb1_start(xb1);
                    } else if ((6 == len) && (data[0] == 0x07) && (data[1] == 0x30)) {
                        xb1->home_got = true;
                        xb1->home_seq = data[2];
                        if (xb1->out_idle) {
                            vsf_usbh_xb1_ack_home(xb1);
                        }
                    } else if ((data[0] == 0x20) && (data[1] == 0x00)) {
                        if (len >= sizeof(vsf_usb_xb1_gamepad_in_report_t)) {
#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_INPUT
                            vsf_usbh_xb1_on_report_input(xb1, (vsf_usb_xb1_gamepad_in_report_t *)data);
#else
                            WEAK_VSF_USBH_XB1_ON_REPORT_INPUT(xb1, (vsf_usb_xb1_gamepad_in_report_t *)data);
#endif
                        }
                    }
                }
                vsf_usbh_hid_recv_report((vsf_usbh_hid_eda_t *)&xb1->use_as__vsf_usbh_hid_teda_t, NULL, 64);
            } else {
                xb1->out_idle = true;
                if (xb1->home_got) {
                    vsf_usbh_xb1_ack_home(xb1);
                } else {
#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT
                    vsf_usbh_xb1_on_report_output(xb1);
#else
                    WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT(xb1);
#endif
                    }
            }
        }
        break;
    }
    return;
}

static void *vsf_usbh_xb1_probe(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, vsf_usbh_ifs_parser_t *parser_ifs)
{
    vsf_usbh_ifs_t *ifs = parser_ifs->ifs;
    vsf_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;

    if (0 == desc_ifs->bInterfaceNumber) {
        vsf_usbh_xb1_t *xb1 = vsf_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vsf_usbh_xb1_t), false);
        if (xb1 != NULL) {
            xb1->user_evthandler = vsf_usbh_xb1_evthandler;
#ifndef WEAK_VSF_USBH_XB1_ON_NEW
            vsf_usbh_xb1_on_new(xb1);
#else
            WEAK_VSF_USBH_XB1_ON_NEW(xb1);
#endif
        }
        return xb1;
    }
    return NULL;
}

static void vsf_usbh_xb1_disconnect(vsf_usbh_t *usbh, vsf_usbh_dev_t *dev, void *param)
{
#ifndef WEAK_VSF_USBH_XB1_ON_FREE
    vsf_usbh_xb1_on_free((vsf_usbh_xb1_t *)param);
#else
    WEAK_VSF_USBH_XB1_ON_FREE(xb1);
#endif
    vsf_usbh_hid_disconnect((vsf_usbh_hid_eda_t *)param);
}

#endif
