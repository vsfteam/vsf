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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_DS4 == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_HID_CLASS_INHERIT__
#define __VSF_USBH_DS4_CLASS_IMPLEMENT

#include "../../vsf_usbh.h"
#include "./vsf_usbh_ds4.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_ds4_id_table[] = {
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

static void * __vk_usbh_ds4_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_ds4_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
const vk_input_item_info_t vk_ds4u_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           47, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         45, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         44, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        46, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             48, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             49, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             54, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             55, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      52, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     53, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      56, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  SPECIAL,        57, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LX,     8,  8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LY,     16, 8,  false, true),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RX,     24, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RY,     32, 8,  false, true),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   LT,     64, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO_LINEAR(   RT,     72, 8,  false, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  DPAD,           40, 4,  false),
};

const vk_sensor_item_info_t vk_ds4u_sensor_item_info[6] = {
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_PITCH, 16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_YAW,   16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_ROLL,  16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Y,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Z,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_X,     16),
};
#endif

const vk_usbh_class_drv_t vk_usbh_ds4_drv = {
    .name       = "ds4",
    .dev_id_num = dimof(__vk_usbh_ds4_id_table),
    .dev_ids    = __vk_usbh_ds4_id_table,
    .probe      = __vk_usbh_ds4_probe,
    .disconnect = __vk_usbh_ds4_disconnect,
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_ds4_on_report_input(vk_usbh_ds4_t *ds4, vsf_usb_ds4_gamepad_in_report_t *report);
extern void vsf_usbh_ds4_on_report_output(vk_usbh_ds4_t *ds4);
extern void vsf_usbh_ds4_on_new(vk_usbh_ds4_t *ds4);
extern void vsf_usbh_ds4_on_free(vk_usbh_ds4_t *ds4);

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
extern void vsf_input_on_touchscreen(vk_touchscreen_evt_t *ts_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_sensor(vk_sensor_evt_t *sensor_evt);

extern void vsf_ds4u_on_new_dev(vk_input_ds4u_t *dev);
extern void vsf_ds4u_on_free_dev(vk_input_ds4u_t *dev);
extern void vsf_ds4u_on_sensor(vk_sensor_evt_t *sensor_evt);
extern void vsf_ds4u_on_report_input(vk_gamepad_evt_t *gamepad_evt);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
#ifndef WEAK_VSF_DS4U_ON_NEW_DEV
WEAK(vsf_ds4u_on_new_dev)
void vsf_ds4u_on_new_dev(vk_input_ds4u_t *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_DS4, dev);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_FREE_DEV
WEAK(vsf_ds4u_on_free_dev)
void vsf_ds4u_on_free_dev(vk_input_ds4u_t *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_DS4, dev);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_REPORT_INPUT
WEAK(vsf_ds4u_on_report_input)
void vsf_ds4u_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

#ifndef WEAK_VSF_DS4U_ON_SENSOR
WEAK(vsf_ds4u_on_sensor)
void vsf_ds4u_on_sensor(vk_sensor_evt_t *sensor_evt)
{
    vsf_input_on_sensor(sensor_evt);
}
#endif

void vk_ds4u_new_dev(vk_input_ds4u_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
    vsf_ds4u_on_new_dev(dev);
}

void vk_ds4u_free_dev(vk_input_ds4u_t *dev)
{
    vsf_ds4u_on_free_dev(dev);
}

void vk_ds4u_process_input(vk_input_ds4u_t *dev, vsf_usb_ds4_gamepad_in_report_t *data)
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
        struct {
            vk_touchscreen_evt_t evt;
        } touch;
    } parser;

    parser.gamepad.evt.duration     = vk_input_update_timestamp(&dev->timestamp);
    parser.gamepad.evt.dev          = dev;

    parser.gamepad.event_sent       = false;
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_ds4u_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_ds4u_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
            vsf_ds4u_on_report_input(&parser.gamepad.evt);
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
        vsf_ds4u_on_report_input(&parser.gamepad.evt);
    }

    // sensor
    parser.sensor.evt.desc.item_info    = (vk_sensor_item_info_t *)vk_ds4u_sensor_item_info;
    parser.sensor.evt.desc.item_num     = dimof(vk_ds4u_sensor_item_info);
    parser.sensor.evt.data              = (uint8_t *)&data->gyro_pitch;
    vsf_ds4u_on_sensor(&parser.sensor.evt);

    // touch
    if (data->touch[0] > 0) {
        uint_fast16_t x, y;

        // TODO: get the real resolution
        parser.touch.evt.info.width     = 1920;
        parser.touch.evt.info.height    = 942;

        if ((dev->data.touch[2] != data->touch[2]) || !(data->touch[2] & 0x80)) {
            x = data->touch[3] + ((data->touch[4] & 0x0F) << 8);
            y = ((data->touch[4] & 0xF0) >> 4) + (data->touch[5] << 4);
            vsf_input_touchscreen_set(&parser.touch.evt, 0, !(data->touch[2] & 0x80), 0, x, y);
            vsf_input_on_touchscreen(&parser.touch.evt);
        }

        if ((dev->data.touch[6] != data->touch[6]) || !(data->touch[6] & 0x80)) {
            x = data->touch[7] + ((data->touch[8] & 0x0F) << 8);
            y = ((data->touch[8] & 0xF0) >> 4) + (data->touch[9] << 4);
            vsf_input_touchscreen_set(&parser.touch.evt, 1, !(data->touch[6] & 0x80), 0, x, y);
            vsf_input_on_touchscreen(&parser.touch.evt);
        }
    }

    dev->data = *data;
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_REPORT_INPUT
WEAK(vsf_usbh_ds4_on_report_input)
void vsf_usbh_ds4_on_report_input(vk_usbh_ds4_t *ds4, vsf_usb_ds4_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
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
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
    vk_ds4u_new_dev(&ds4->use_as__vk_input_ds4u_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_DS4_ON_FREE
WEAK(vsf_usbh_ds4_on_free)
void vsf_usbh_ds4_on_free(vk_usbh_ds4_t *ds4)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_DS4 == ENABLED
    vk_ds4u_free_dev(&ds4->use_as__vk_input_ds4u_t);
#   endif
}
#endif

static void __vk_usbh_ds4_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_ds4_t *ds4 = (vk_usbh_ds4_t *)container_of(eda, vk_usbh_hid_teda_t, use_as__vsf_teda_t);

    switch (evt) {
    case VSF_EVT_INIT: {
            vk_usbh_ep0_t *ep0 = ds4->ep0;
            vk_usbh_urb_t *urb = &ep0->urb;

            vk_usbh_urb_free_buffer(urb);
            vk_usbh_hid_set_idle(ds4, 0, 0);
        }
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe = vk_usbh_urb_get_pipe(&urb);

            if (USB_ENDPOINT_XFER_CONTROL == pipe.type) {
                __vsf_eda_crit_npb_leave(&ds4->dev->ep0.crit);

                vsf_usbh_ds4_on_new(ds4);
                vk_usbh_hid_recv_report(&ds4->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
            } else /* if (USB_ENDPOINT_XFER_INT == pipe.type) */ {
                if (pipe.dir_in1out0) {
                    if (    (URB_OK == vk_usbh_urb_get_status(&urb))
                        &&  (sizeof(vsf_usb_ds4_gamepad_in_report_t) == vk_usbh_urb_get_actual_length(&urb))) {
                        vsf_usbh_ds4_on_report_input(ds4, (vsf_usb_ds4_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
                    }
                    vk_usbh_hid_recv_report(&ds4->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_ds4_gamepad_in_report_t));
                } else {
                    ds4->out_idle = true;
                    vsf_usbh_ds4_on_report_output(ds4);
                }
            }
        }
        break;
    }
}

static void * __vk_usbh_ds4_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ds4_t *ds4 = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_ds4_t), true);
    if (ds4 != NULL) {
        ds4->gamepad_out_buf.id = 0x05;
        ds4->gamepad_out_buf.dummyFF = 0xff;
        ds4->user_evthandler = __vk_usbh_ds4_evthandler;
    }
    return ds4;
}

static void __vk_usbh_ds4_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vsf_usbh_ds4_on_free((vk_usbh_ds4_t *)param);
    vk_usbh_hid_disconnect((vk_usbh_hid_teda_t *)param);
}

bool vk_usbh_ds4_can_output(vk_usbh_ds4_t *ds4)
{
    return ds4->out_idle;
}

static void __vk_usbd_ds4_output(vk_usbh_ds4_t *ds4)
{
    ds4->out_idle = false;
    vk_usbh_hid_send_report(&ds4->use_as__vk_usbh_hid_teda_t, (uint8_t *)&ds4->gamepad_out_buf, sizeof(ds4->gamepad_out_buf));
}

void vk_usbh_ds4_set_rumble(vk_usbh_ds4_t *ds4, uint_fast8_t left, uint_fast8_t right)
{
    VSF_USB_ASSERT(ds4->out_idle);
    ds4->gamepad_out_buf.rumble_r = right;
    ds4->gamepad_out_buf.rumble_l = left;
    __vk_usbd_ds4_output(ds4);
}

void vk_usbh_ds4_set_led(vk_usbh_ds4_t *ds4, uint_fast8_t r, uint_fast8_t g, uint_fast8_t b)
{
    VSF_USB_ASSERT(ds4->out_idle);
    ds4->gamepad_out_buf.led_r = r;
    ds4->gamepad_out_buf.led_g = g;
    ds4->gamepad_out_buf.led_b = b;
    __vk_usbd_ds4_output(ds4);
}

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_DS4
