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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_XB1 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_HID_CLASS_INHERIT__
#define __VSF_USBH_XB1_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_usbh.h"
#include "./vsf_usbh_xb1.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_xb1_dev_id[] = {
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

static void *__vk_usbh_xb1_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_xb1_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
const vk_input_item_info_t vk_xb1_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           39, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         36, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         38, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        37, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             44, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             45, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             46, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             47, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      35, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     34, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      33, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LX,             80, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LY,             96, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RX,             112,16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RY,             128,16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LT,             48, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RT,             64, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_UP,           40, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_DOWN,         41, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_LEFT,         42, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_RIGHT,        43, 1,  false),
};
#endif

const vk_usbh_class_drv_t vk_usbh_xb1_drv = {
    .name       = "xb1",
    .dev_id_num = dimof(__vk_usbh_xb1_dev_id),
    .dev_ids    = __vk_usbh_xb1_dev_id,
    .probe      = __vk_usbh_xb1_probe,
    .disconnect = __vk_usbh_xb1_disconnect,
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_xb1_on_report_input(vk_usbh_xb1_t *xb1, vsf_usb_xb1_gamepad_in_report_t *report);
extern void vsf_usbh_xb1_on_report_output(vk_usbh_xb1_t *xb1);
extern void vsf_usbh_xb1_on_new(vk_usbh_xb1_t *xb1);
extern void vsf_usbh_xb1_on_free(vk_usbh_xb1_t *xb1);

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
#ifndef WEAK_VSF_XB1_ON_NEW_DEV
WEAK(vsf_xb1_on_new_dev)
void vsf_xb1_on_new_dev(vk_input_xb1_t *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_XB1, dev);
}
#endif

#ifndef WEAK_VSF_XB1_ON_FREE_DEV
WEAK(vsf_xb1_on_free_dev)
void vsf_xb1_on_free_dev(vk_input_xb1_t *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_XB1, dev);
}
#endif

#ifndef WEAK_VSF_XB1_ON_REPORT_INPUT
WEAK(vsf_xb1_on_report_input)
void vsf_xb1_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

void vk_xb1_new_dev(vk_input_xb1_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
#ifndef WEAK_VSF_XB1_ON_NEW_DEV
    vsf_xb1_on_new_dev(dev);
#else
    WEAK_VSF_XB1_ON_NEW_DEV(dev);
#endif
}

void vk_xb1_free_dev(vk_input_xb1_t *dev)
{
#ifndef WEAK_VSF_XB1_ON_FREE_DEV
    vsf_xb1_on_free_dev(dev);
#else
    WEAK_VSF_XB1_ON_FREE_DEV(dev);
#endif
}

void vk_xb1_process_input(vk_input_xb1_t *dev, vsf_usb_xb1_gamepad_in_report_t *data)
{
    union {
        struct {
            vk_gamepad_evt_t evt;
            vk_input_item_info_t *info;
            vk_input_parser_t parser;
            bool event_sent;
        } gamepad;
        struct {
            vk_sensor_evt_t evt;
        } sensor;
    } parser;

    parser.gamepad.evt.duration     = vk_input_update_timestamp(&dev->timestamp);
    parser.gamepad.evt.dev          = dev;

    parser.gamepad.event_sent       = false;
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_xb1_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_xb1_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
#ifndef WEAK_VSF_XB1_ON_REPORT_INPUT
            vsf_xb1_on_report_input(&parser.gamepad.evt);
#else
            WEAK_VSF_XB1_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
#ifndef WEAK_VSF_XB1_ON_REPORT_INPUT
        vsf_xb1_on_report_input(&parser.gamepad.evt);
#else
        WEAK_VSF_XB1_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
    }

    dev->data = *data;
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_INPUT
WEAK(vsf_usbh_xb1_on_report_input)
void vsf_usbh_xb1_on_report_input(vk_usbh_xb1_t *xb1, vsf_usb_xb1_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
    vk_xb1_process_input(&xb1->use_as__vk_input_xb1_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_REPORT_OUTPUT
WEAK(vsf_usbh_xb1_on_report_output)
void vsf_usbh_xb1_on_report_output(vk_usbh_xb1_t *xb1)
{
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_NEW
WEAK(vsf_usbh_xb1_on_new)
void vsf_usbh_xb1_on_new(vk_usbh_xb1_t *xb1)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
    vk_xb1_new_dev(&xb1->use_as__vk_input_xb1_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB1_ON_FREE
WEAK(vsf_usbh_xb1_on_free)
void vsf_usbh_xb1_on_free(vk_usbh_xb1_t *xb1)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB1 == ENABLED
    vk_xb1_free_dev(&xb1->use_as__vk_input_xb1_t);
#   endif
}
#endif

static void vk_usbh_xb1_output(vk_usbh_xb1_t *xb1, uint_fast8_t len)
{
    VSF_USB_ASSERT(xb1->out_idle);
    xb1->out_idle = false;
    vk_usbh_hid_send_report(&xb1->use_as__vk_usbh_hid_teda_t, (uint8_t *)&xb1->gamepad_out_buf, len);
}

static void vk_usbh_xb1_submit_cmd(vk_usbh_xb1_t *xb1, uint_fast8_t cmd, uint_fast8_t sub_cmd, uint_fast8_t seq, const uint8_t *data, uint8_t len)
{
    VSF_USB_ASSERT(xb1->out_idle && ((4 + len) < sizeof(xb1->gamepad_out_buf)));
    xb1->gamepad_out_buf.buffer[0] = cmd;
    xb1->gamepad_out_buf.buffer[1] = sub_cmd;
    xb1->gamepad_out_buf.buffer[2] = seq;
    xb1->gamepad_out_buf.buffer[3] = len;
    memcpy(&xb1->gamepad_out_buf.buffer[4], data, len);
    vk_usbh_xb1_output(xb1, 4 + len);
}

static void vk_usbh_xb1_ack_home(vk_usbh_xb1_t *xb1)
{
    const uint8_t ack_home_reply[] = {
        0x00, 0x07, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    vk_usbh_xb1_submit_cmd(xb1, 0x01, 0x20, xb1->home_seq, ack_home_reply, sizeof(ack_home_reply));
}

static void vk_usbh_xb1_start(vk_usbh_xb1_t *xb1)
{
    const uint8_t start[] = { 0x00 };
    vk_usbh_xb1_submit_cmd(xb1, 0x05, 0x20, 0x00, start, sizeof(start));
}

static void vk_usbh_xb1_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_xb1_t *xb1 = (vk_usbh_xb1_t *)container_of(eda, vk_usbh_hid_teda_t, use_as__vsf_teda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        xb1->out_idle = true;
        __vsf_eda_crit_npb_leave(&xb1->dev->ep0.crit);
        vk_usbh_hid_recv_report(&xb1->use_as__vk_usbh_hid_teda_t, NULL, 64);

        // TODO: use timer?
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (pipe.dir_in1out0) {
                if (URB_OK == vk_usbh_urb_get_status(&urb)) {
                    uint_fast32_t len = vk_usbh_urb_get_actual_length(&urb);
                    uint8_t *data = vk_usbh_urb_peek_buffer(&urb);

                    if (0 == len) {
                    } else if ((32 == len) && (data[0] == 0x02) && (data[1] == 0x20)) {
                        // Pairing request
                        vk_usbh_xb1_start(xb1);
                    } else if ((6 == len) && (data[0] == 0x07) && (data[1] == 0x30)) {
                        xb1->home_got = true;
                        xb1->home_seq = data[2];
                        if (xb1->out_idle) {
                            vk_usbh_xb1_ack_home(xb1);
                        }
                    } else if ((data[0] == 0x20) && (data[1] == 0x00)) {
                        if (len >= sizeof(vsf_usb_xb1_gamepad_in_report_t)) {
                            vsf_usbh_xb1_on_report_input(xb1, (vsf_usb_xb1_gamepad_in_report_t *)data);
                        }
                    }
                }
                vk_usbh_hid_recv_report(&xb1->use_as__vk_usbh_hid_teda_t, NULL, 64);
            } else {
                xb1->out_idle = true;
                if (xb1->home_got) {
                    vk_usbh_xb1_ack_home(xb1);
                } else {
                    vsf_usbh_xb1_on_report_output(xb1);
                }
            }
        }
        break;
    }
    return;
}

static void *__vk_usbh_xb1_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;

    if (0 == desc_ifs->bInterfaceNumber) {
        vk_usbh_xb1_t *xb1 = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_xb1_t), false);
        if (xb1 != NULL) {
            xb1->user_evthandler = vk_usbh_xb1_evthandler;
            vsf_usbh_xb1_on_new(xb1);
        }
        return xb1;
    }
    return NULL;
}

static void __vk_usbh_xb1_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vsf_usbh_xb1_on_free((vk_usbh_xb1_t *)param);
    vk_usbh_hid_disconnect((vk_usbh_hid_teda_t *)param);
}

#endif
