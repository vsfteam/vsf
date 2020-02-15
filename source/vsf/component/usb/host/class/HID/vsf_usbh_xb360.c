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

#if VSF_USE_USB_HOST == ENABLED && VSF_USE_USB_HOST_XB360 == ENABLED

#define VSF_USBH_IMPLEMENT_CLASS
#define VSF_USBH_XB360_IMPLEMENT
#define VSF_USBH_HID_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error "VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE is required"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vk_usbh_dev_id_t vk_usbh_xb360_dev_id[] = {
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_VENDOR_SPEC, 93, 1) },
    { VSF_USBH_MATCH_IFS_CLASS(USB_CLASS_VENDOR_SPEC, 93, 129) },
};

/*============================ PROTOTYPES ====================================*/

static void *vk_usbh_xb360_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs);
static void vk_usbh_xb360_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_usbh_class_drv_t vk_usbh_xb360_drv = {
    .name       = "xb360",
    .dev_id_num = dimof(vk_usbh_xb360_dev_id),
    .dev_ids    = vk_usbh_xb360_dev_id,
    .probe      = vk_usbh_xb360_probe,
    .disconnect = vk_usbh_xb360_disconnect,
};

/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_USBH_XB360_ON_REPORT_INPUT_EXTERN)                     \
    &&  defined(WEAK_VSF_USBH_XB360_ON_REPORT_INPUT)
WEAK_VSF_USBH_XB360_ON_REPORT_INPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT_EXTERN)                    \
    &&  defined(WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT)
WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB360_ON_NEW_EXTERN)                              \
    &&  defined(WEAK_VSF_USBH_XB360_ON_NEW)
WEAK_VSF_USBH_XB360_ON_NEW_EXTERN
#endif

#if     defined(WEAK_VSF_USBH_XB360_ON_FREE_EXTERN)                             \
    &&  defined(WEAK_VSF_USBH_XB360_ON_FREE)
WEAK_VSF_USBH_XB360_ON_FREE_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_USBH_XB360_ON_REPORT_INPUT
WEAK(vsf_usbh_xb360_on_report_input)
void vsf_usbh_xb360_on_report_input(vk_usbh_xb360_t *xb360, vsf_usb_xb360_gamepad_in_report_t *report)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_XB360 == ENABLED
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
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_XB360 == ENABLED
    vk_xb360_new_dev(&xb360->use_as__vk_input_xb360_t);
#   endif
}
#endif

#ifndef WEAK_VSF_USBH_XB360_ON_FREE
WEAK(vsf_usbh_xb360_on_free)
void vsf_usbh_xb360_on_free(vk_usbh_xb360_t *xb360)
{
#   if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_XB360 == ENABLED
    vk_xb360_free_dev(&xb360->use_as__vk_input_xb360_t);
#   endif
}
#endif

static void vk_usbh_xb360_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_usbh_xb360_t *xb360 = (vk_usbh_xb360_t *)container_of(eda, vk_usbh_hid_eda_t, use_as__vsf_eda_t);

    switch (evt) {
    case VSF_EVT_INIT:
        vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&xb360->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_xb360_gamepad_in_report_t));
        xb360->gamepad_out_buf.buffer[1] = 0x08;
        vk_usbh_hid_send_report((vk_usbh_hid_eda_t *)&xb360->use_as__vk_usbh_hid_teda_t, (uint8_t *)&xb360->gamepad_out_buf, 8);

        // TODO: use timer?
        break;
    case VSF_EVT_MESSAGE: {
            vk_usbh_urb_t urb = { .urb_hcd = vsf_eda_get_cur_msg() };
            vk_usbh_pipe_t pipe;

            pipe = vk_usbh_urb_get_pipe(&urb);
            if (pipe.dir_in1out0) {
                if (    (URB_OK == vk_usbh_urb_get_status(&urb))
                    &&  (sizeof(vsf_usb_xb360_gamepad_in_report_t) == vk_usbh_urb_get_actual_length(&urb))) {
#ifndef WEAK_VSF_USBH_XB360_ON_REPORT_INPUT
                    vsf_usbh_xb360_on_report_input(xb360, (vsf_usb_xb360_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
#else
                    WEAK_VSF_USBH_XB360_ON_REPORT_INPUT(xb360, (vsf_usb_xb360_gamepad_in_report_t *)vk_usbh_urb_peek_buffer(&urb));
#endif
                }
                vk_usbh_hid_recv_report((vk_usbh_hid_eda_t *)&xb360->use_as__vk_usbh_hid_teda_t, NULL, sizeof(vsf_usb_xb360_gamepad_in_report_t));
            } else {
                xb360->out_idle = true;
#ifndef WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT
                vsf_usbh_xb360_on_report_output(xb360);
#else
                WEAK_VSF_USBH_XB360_ON_REPORT_OUTPUT(xb360);
#endif
            }
        }
        break;
    }
    return;
}

static void *vk_usbh_xb360_probe(vk_usbh_t *usbh, vk_usbh_dev_t *dev, vk_usbh_ifs_parser_t *parser_ifs)
{
    vk_usbh_ifs_t *ifs = parser_ifs->ifs;
    vk_usbh_ifs_alt_parser_t *parser_alt = &parser_ifs->parser_alt[ifs->cur_alt];
    struct usb_interface_desc_t *desc_ifs = parser_alt->desc_ifs;

    if (0 == desc_ifs->bInterfaceNumber) {
        vk_usbh_xb360_t *xb360 = vk_usbh_hid_probe(usbh, dev, parser_ifs, sizeof(vk_usbh_xb360_t), true);
        if (xb360 != NULL) {
            xb360->user_evthandler = vk_usbh_xb360_evthandler;
#ifndef WEAK_VSF_USBH_XB360_ON_NEW
            vsf_usbh_xb360_on_new(xb360);
#else
            WEAK_VSF_USBH_XB360_ON_NEW(xb360);
#endif
        }
        return xb360;
    }
    return NULL;
}

static void vk_usbh_xb360_disconnect(vk_usbh_t *usbh, vk_usbh_dev_t *dev, void *param)
{
#ifndef WEAK_VSF_USBH_XB360_ON_FREE
    vsf_usbh_xb360_on_free((vk_usbh_xb360_t *)param);
#else
    WEAK_VSF_USBH_XB360_ON_FREE(xb360);
#endif
    vk_usbh_hid_disconnect((vk_usbh_hid_eda_t *)param);
}

#endif
