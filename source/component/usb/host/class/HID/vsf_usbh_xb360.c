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

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_XB360 == ENABLED

#define __VSF_USBH_CLASS_IMPLEMENT_CLASS__
#define __VSF_USBH_HID_CLASS_INHERIT__
#define __VSF_USBH_XB360_CLASS_IMPLEMENT

#include "../../vsf_usbh.h"
#include "./vsf_usbh_xb360.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t __vk_usbh_xb360_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_VENDOR_SPEC, 93, 1) },
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_VENDOR_SPEC, 93, 129) },
};

/*============================ PROTOTYPES ====================================*/

static void *__vk_usbh_xb360_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void __vk_usbh_xb360_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
const vk_input_item_info_t vk_xb360_gamepad_item_info[GAMEPAD_ID_NUM] = {
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_UP,           31, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_DOWN,         28, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_LEFT,         30, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  R_RIGHT,        29, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LB,             24, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RB,             25, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LS,             22, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RS,             23, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_LEFT,      21, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_RIGHT,     20, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  MENU_MAIN,      26, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LX,             48, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LY,             64, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RX,             80, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RY,             96, 16, true),
    VSF_GAMEPAD_DEF_ITEM_INFO(  LT,             32, 8,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  RT,             40, 8,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_UP,           16, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_DOWN,         17, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_LEFT,         18, 1,  false),
    VSF_GAMEPAD_DEF_ITEM_INFO(  L_RIGHT,        19, 1,  false),
};
#endif

const vk_usbh_class_drv_t vk_usbh_xb360_drv = {
    .name       = "xb360",
    .dev_id_num = dimof(__vk_usbh_xb360_dev_id),
    .dev_ids    = __vk_usbh_xb360_dev_id,
    .probe      = __vk_usbh_xb360_probe,
    .disconnect = __vk_usbh_xb360_disconnect,
};

/*============================ PROTOTYPES ====================================*/

extern void vsf_usbh_xb360_on_report_input(vk_usbh_xb360_t *xb360, vsf_usb_xb360_gamepad_in_report_t *report);
extern void vsf_usbh_xb360_on_report_output(vk_usbh_xb360_t *xb360);
extern void vsf_usbh_xb360_on_new(vk_usbh_xb360_t *xb360);
extern void vsf_usbh_xb360_on_free(vk_usbh_xb360_t *xb360);

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
extern void vsf_input_on_new_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_free_dev(vk_input_type_t type, void *dev);
extern void vsf_input_on_gamepad(vk_gamepad_evt_t *gamepad_evt);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
#ifndef WEAK_VSF_XB360_ON_NEW_DEV
WEAK(vsf_xb360_on_new_dev)
void vsf_xb360_on_new_dev(vk_input_xb360_t *dev)
{
    vsf_input_on_new_dev(VSF_INPUT_TYPE_XB360, dev);
}
#endif

#ifndef WEAK_VSF_XB360_ON_FREE_DEV
WEAK(vsf_xb360_on_free_dev)
void vsf_xb360_on_free_dev(vk_input_xb360_t *dev)
{
    vsf_input_on_free_dev(VSF_INPUT_TYPE_XB360, dev);
}
#endif

#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
WEAK(vsf_xb360_on_report_input)
void vsf_xb360_on_report_input(vk_gamepad_evt_t *gamepad_evt)
{
    vsf_input_on_gamepad(gamepad_evt);
}
#endif

void vk_xb360_new_dev(vk_input_xb360_t *dev)
{
    memset(&dev->data, 0, sizeof(dev->data));
#ifndef WEAK_VSF_XB360_ON_NEW_DEV
    vsf_xb360_on_new_dev(dev);
#else
    WEAK_VSF_XB360_ON_NEW_DEV(dev);
#endif
}

void vk_xb360_free_dev(vk_input_xb360_t *dev)
{
#ifndef WEAK_VSF_XB360_ON_FREE_DEV
    vsf_xb360_on_free_dev(dev);
#else
    WEAK_VSF_XB360_ON_FREE_DEV(dev);
#endif
}

void vk_xb360_process_input(vk_input_xb360_t *dev, vsf_usb_xb360_gamepad_in_report_t *data)
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
    parser.gamepad.parser.info      = (vk_input_item_info_t *)vk_xb360_gamepad_item_info;
    parser.gamepad.parser.num       = dimof(vk_xb360_gamepad_item_info);
    do {
        parser.gamepad.info = vk_input_parse(&parser.gamepad.parser, (uint8_t *)&dev->data, (uint8_t *)data);
        if (parser.gamepad.info != NULL) {
            parser.gamepad.evt.id = parser.gamepad.info->item;
            parser.gamepad.evt.info = *parser.gamepad.info;
            parser.gamepad.evt.pre = parser.gamepad.parser.pre;
            parser.gamepad.evt.cur = parser.gamepad.parser.cur;
#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
            vsf_xb360_on_report_input(&parser.gamepad.evt);
#else
            WEAK_VSF_XB360_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
            parser.gamepad.event_sent = true;
        }
    } while (parser.gamepad.info != NULL);
    if (parser.gamepad.event_sent) {
        parser.gamepad.evt.id = GAMEPAD_ID_DUMMY;
#ifndef WEAK_VSF_XB360_ON_REPORT_INPUT
        vsf_xb360_on_report_input(&parser.gamepad.evt);
#else
        WEAK_VSF_XB360_ON_REPORT_INPUT(&parser.gamepad.evt);
#endif
    }

    dev->data = *data;
}
#endif

#ifndef WEAK_VSF_USBH_XB360_ON_REPORT_INPUT
WEAK(vsf_usbh_xb360_on_report_input)
void vsf_usbh_xb360_on_report_input(vk_usbh_xb360_t *xb360, vsf_usb_xb360_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
    vk_xb360_process_input(&xb360->use_as__vk_input_xb360_t, report);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT
WEAK(vsf_usbh_xb360_on_report_output)
void vsf_usbh_xb360_on_report_output(vk_usbh_xb360_t *xb360)
{
}
#endif

#ifndef WEAK_VSF_USBH_XB360_ON_NEW
WEAK(vsf_usbh_xb360_on_new)
void vsf_usbh_xb360_on_new(vk_usbh_xb360_t *xb360)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
    vk_xb360_new_dev(&xb360->use_as__vk_input_xb360_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB360_ON_FREE
WEAK(vsf_usbh_xb360_on_free)
void vsf_usbh_xb360_on_free(vk_usbh_xb360_t *xb360)
{
#   if VSF_USE_INPUT == ENABLED && VSF_INPUT_USE_XB360 == ENABLED
    vk_xb360_free_dev(&xb360->use_as__vk_input_xb360_t);
#   endif
}
#endif

static void __vk_usbh_xb360_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_xb360_t *xb360 = (vk_usbh_xb360_t *)container_of(eda, vk_usbh_hid_teda_t, use_as__vsf_teda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        __vsf_eda_crit_npb_leave(&xb360->dev->ep0.crit);
        vk_usbh_hid_recv_report(&xb360->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_xb360_gamepad_in_report_t));
        xb360->gamepad_out_buf.buffer[1] = 0x08;
        vk_usbh_hid_send_report(&xb360->use_as__vk_usbh_hid_teda_t, (uint8_t *)&xb360->gamepad_out_buf, 8);

        // TODO: use timer?
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (pipe.dir_in1out0) {
                if (    (URB_OK == vk_usbh_urb_get_status(&urb))
                    &&  (sizeof(vsf_usb_xb360_gamepad_in_report_t) == vk_usbh_urb_get_actual_length(&urb))) {
                    vsf_usbh_xb360_on_report_input(xb360, (vsf_usb_xb360_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
                }
                vk_usbh_hid_recv_report(&xb360->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_xb360_gamepad_in_report_t));
            } else {
                xb360->out_idle = true;
                vsf_usbh_xb360_on_report_output(xb360);
            }
        }
        break;
    }
    return;
}

static void *__vk_usbh_xb360_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;

    if (0 == desc_ifs->bInterfaceNumber) {
        vk_usbh_xb360_t *xb360 = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_xb360_t), true);
        if (xb360 != NULL) {
            xb360->user_evthandler = __vk_usbh_xb360_evthandler;
            vsf_usbh_xb360_on_new(xb360);
        }
        return xb360;
    }
    return NULL;
}

static void __vk_usbh_xb360_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
    vsf_usbh_xb360_on_free((vk_usbh_xb360_t *)param);
    vk_usbh_hid_disconnect((vk_usbh_hid_teda_t *)param);
}

#endif
