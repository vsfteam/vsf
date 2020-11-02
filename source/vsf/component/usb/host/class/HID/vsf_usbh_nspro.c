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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_NSPRO == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_HID_CLASS_INHERIT__
#define __VSF_USBH_NSPRO_CLASS_IMPLEMENT

#include "../../vsf_usbh.h"
#include "./vsf_usbh_nspro.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_nspro_id_table[] = {
    {
        VSF_USBH_MATCH_VID_PID(0x057E, 0x2009)
        .match_ifs_class = true,
        .bInterfaceClass = USB_CLASS_HID,
    },
};

/*============================ PROTOTYPES ====================================*/

static void * __vk_usbh_nspro_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev,
            vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_nspro_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
const vk_input_item_info_t vk_nspro_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           25, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         26, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         24, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        27, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             46, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             30, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             35, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             34, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      32, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     33, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      36, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  SPECIAL,        37, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LX,             48, 12, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LY,             60, 12, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RX,             72, 12, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RY,             84, 12, false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LT,             47, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RT,             31, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_UP,           41, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_DOWN,         40, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_LEFT,         43, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_RIGHT,        42, 1,  false),
};

const vk_sensor_item_info_t vk_nspro_sensor_item_info[6] = {
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_X,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Y,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_ACC,  SENSOR_SUBID_Z,     16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_ROLL,  16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_PITCH, 16),
    VSF_SENSOR_DEF_ITEM_INFO(   SENSOR_ID_GYRO, SENSOR_SUBID_YAW,   16),
};
#endif

const vk_usbh_class_drv_t vk_usbh_nspro_drv = {
    .name       = "nspro",
    .dev_id_num = dimof(__vk_usbh_nspro_id_table),
    .dev_ids    = __vk_usbh_nspro_id_table,
    .probe      = __vk_usbh_nspro_probe,
    .disconnect = __vk_usbh_nspro_disconnect,
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_nspro_on_report_input(vk_usbh_nspro_t *nspro, vsf_usb_nspro_gamepad_in_report_t *report);
extern void vsf_usbh_nspro_on_report_output(vk_usbh_nspro_t *nspro);
extern void vsf_usbh_nspro_on_new(vk_usbh_nspro_t *nspro);
extern void vsf_usbh_nspro_on_free(vk_usbh_nspro_t *nspro);

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_sensor(vk_sensor_evt_t *sensor_evt);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
#ifndef WEAK_VSF_NSPRO_ON_NEW_DEV
WEAK(vsf_nspro_on_new_dev)
void vsf_nspro_on_new_dev(vk_input_nspro_t *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_NSPRO, dev);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_FREE_DEV
WEAK(vsf_nspro_on_free_dev)
void vsf_nspro_on_free_dev(vk_input_nspro_t *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_NSPRO, dev);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_REPORT_INPUT
WEAK(vsf_nspro_on_report_input)
void vsf_nspro_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

#ifndef WEAK_VSF_NSPRO_ON_SENSOR
WEAK(vsf_nspro_on_sensor)
void vsf_nspro_on_sensor(vk_sensor_evt_t *sensor_evt)
{
    vsf_input_on_sensor(sensor_evt);
}
#endif

void vk_nspro_new_dev(vk_input_nspro_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
#ifndef WEAK_VSF_NSPRO_ON_NEW_DEV
    vsf_nspro_on_new_dev(dev);
#else
    WEAK_VSF_NSPRO_ON_NEW_DEV(dev);
#endif
}

void vk_nspro_free_dev(vk_input_nspro_t *dev)
{
#ifndef WEAK_VSF_NSPRO_ON_FREE_DEV
    vsf_nspro_on_free_dev(dev);
#else
    WEAK_VSF_NSPRO_ON_FREE_DEV(dev);
#endif
}

void vk_nspro_process_input(vk_input_nspro_t *dev, vsf_usb_nspro_gamepad_in_report_t *data)
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
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_nspro_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_nspro_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
#ifndef WEAK_VSF_NSPRO_ON_REPORT_INPUT
            vsf_nspro_on_report_input(&parser.gamepad.evt);
#else
            WEAK_VSF_NSPRO_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
#ifndef WEAK_VSF_NSPRO_ON_REPORT_INPUT
        vsf_nspro_on_report_input(&parser.gamepad.evt);
#else
        WEAK_VSF_NSPRO_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
    }

    // sensor
    parser.sensor.evt.desc.item_info    = (vk_sensor_item_info_t *)vk_nspro_sensor_item_info;
    parser.sensor.evt.desc.item_num     = dimof(vk_nspro_sensor_item_info);
    parser.sensor.evt.data              = (uint8_t *)&data->gyro_acc[0];
#ifndef WEAK_VSF_NSPRO_ON_SENSOR
    vsf_nspro_on_sensor(&parser.sensor.evt);
#else
    WEAK_VSF_NSPRO_ON_SENSOR(&parser.sensor.evt);
#endif

    dev->data = *data;
}
#endif

#ifndef WEAK_VSF_USBH_NSPRO_ON_REPORT_INPUT
WEAK(vsf_usbh_nspro_on_report_input)
void vsf_usbh_nspro_on_report_input(vk_usbh_nspro_t *nspro, vsf_usb_nspro_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
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
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
    vk_nspro_new_dev(&nspro->use_as__vk_input_nspro_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_NSPRO_ON_FREE
WEAK(vsf_usbh_nspro_on_free)
void vsf_usbh_nspro_on_free(vk_usbh_nspro_t *nspro)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_NSPRO == ENABLED
    vk_nspro_free_dev(&nspro->use_as__vk_input_nspro_t);
#   endif
}
#endif

static void __vk_usbh_nspro_send_usb_cmd(vk_usbh_nspro_t *nspro, uint_fast8_t cmd)
{
    nspro->gamepad_out_buf.buffer[0] = 0x80;
    nspro->gamepad_out_buf.buffer[1] = cmd;
    vk_usbh_hid_send_report(&nspro->use_as__vk_usbh_hid_teda_t, (uint8_t *)&nspro->gamepad_out_buf, 2);
}

static void __vk_usbh_nspro_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_nspro_t *nspro = (vk_usbh_nspro_t *)container_of(eda, vk_usbh_hid_teda_t, use_as__vsf_teda_t);

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
                __vsf_eda_crit_npb_leave(&nspro->dev->ep0.crit);
                vk_usbh_hid_recv_report(&nspro->use_as__vk_usbh_hid_teda_t, NULL, 64);
                __vk_usbh_nspro_send_usb_cmd(nspro, 1);
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
                            __vk_usbh_nspro_send_usb_cmd(nspro, 2);
                            break;
                        case VSF_USBH_NSPRO_HANDSHAKE:
                            if (    (URB_OK != vk_usbh_urb_get_status(&urb))
                                ||  (buffer[0] != 0x81) || (buffer[1] != 0x02)) {
                                break;
                            }
                            nspro->start_state++;
                            __vk_usbh_nspro_send_usb_cmd(nspro, 4);
                            break;
                        case VSF_USBH_NSPRO_RUNNING:
                            if (URB_OK == vk_usbh_urb_get_status(&urb)) {
                                vsf_usbh_nspro_on_report_input(nspro, (vsf_usb_nspro_gamepad_in_report_t *)buffer);
                            }
                            break;
                        }
                        vk_usbh_hid_recv_report(&nspro->use_as__vk_usbh_hid_teda_t, NULL, 64);
                    }
                } else {
                    if (VSF_USBH_NSPRO_RUNNING == nspro->start_state) {
                        nspro->out_idle = true;
                        vsf_usbh_nspro_on_report_output(nspro);
                    }
                }
            }
        }
        break;
    }
}

static void * __vk_usbh_nspro_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_nspro_t *nspro = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_nspro_t), true);
    if (nspro != NULL) {
        nspro->user_evthandler = __vk_usbh_nspro_evthandler;
        vsf_usbh_nspro_on_new(nspro);
    }
    return nspro;
}

static void __vk_usbh_nspro_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vsf_usbh_nspro_on_free((vk_usbh_nspro_t *)param);
    vk_usbh_hid_disconnect((vk_usbh_hid_teda_t *)param);
}

#endif      // VSF_USE_USB_HOST && VSF_USBH_USE_NSPRO
