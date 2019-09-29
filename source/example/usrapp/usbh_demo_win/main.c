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

#include "vsf.h"
#include "component/usb/driver/hcd/libusb_hcd/vsf_libusb_hcd.h"

#if VSF_USE_UI == ENABLED && VSF_USE_UI_LVGL == ENABLED
#   include "lvgl/lvgl.h"
#   include "lv_conf.h"
#   include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_const_t {
    vsf_libusb_hcd_param_t libusb_hcd_param;
};
typedef struct usrapp_const_t usrapp_const_t;

struct usrapp_t {
    vsf_usbh_t usbh;
#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vsf_usbh_class_t ecm;
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    vsf_usbh_class_t bthci;
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    vsf_usbh_class_t hid;
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    vsf_usbh_class_t ds4;
#endif

#if VSF_USE_UI == ENABLED
    struct {
        vsf_disp_sdl2_t disp;
        vsf_touchscreen_evt_t ts_evt;
        lv_disp_buf_t disp_buf;
        lv_color_t color[LV_VER_RES_MAX][LV_HOR_RES_MAX];
    } ui;
#endif
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usrapp_const_t usrapp_const = {
    .libusb_hcd_param           = {
        .priority               = vsf_arch_prio_0,
    },
};

static usrapp_t usrapp = {
    .usbh.drv                   = &vsf_libusb_hcd_drv,
    .usbh.param                 = (void *)&usrapp_const.libusb_hcd_param,

#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    .ecm.drv                    = &vsf_usbh_ecm_drv,
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    .bthci.drv                  = &vsf_usbh_bthci_drv,
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    .hid.drv                    = &vsf_usbh_hid_drv,
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    .ds4.drv                    = &vsf_usbh_ds4_drv,
#endif

#if VSF_USE_UI == ENABLED
    .ui.disp                    = {
        .param                  = {
            .height             = LV_VER_RES_MAX,
            .width              = LV_HOR_RES_MAX,
            .drv                = &vsf_disp_drv_sdl2,
            .color              = VSF_DISP_COLOR_RGB565,
        },
        .amplifier              = 2,
    },
#endif
};

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_UI == ENABLED
extern void ui_demo_start(void);
#endif

/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_UI == ENABLED
void vsf_input_on_touchscreen(vsf_touchscreen_evt_t *ts_evt)
{
    if (ts_evt->dev == &usrapp.ui.disp) {
        usrapp.ui.ts_evt = *ts_evt;
    }
}

static bool usrapp_touchscreen_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    data->state = VSF_INPUT_TOUCHSCREEN_IS_DOWN(&usrapp.ui.ts_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = VSF_INPUT_TOUCHSCREEN_GET_X(&usrapp.ui.ts_evt);
    data->point.y = VSF_INPUT_TOUCHSCREEN_GET_Y(&usrapp.ui.ts_evt);
//    vsf_trace(VSF_TRACE_DEBUG, "touchscreen: %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        data->state == LV_INDEV_STATE_PR ? "press" : "release",
//        data->point.x, data->point.y);
    return false;
}
#endif

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
    vsf_trace_init(NULL);
    vsf_stdio_init();

    vsf_usbh_init(&usrapp.usbh);
#if VSF_USE_TCPIP == ENABLED && VSF_USE_USB_HOST_ECM == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.ecm);
#endif
#if VSF_USE_USB_HOST_BTHCI == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.bthci);
#endif
#if VSF_USE_USB_HOST_HID == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.hid);
#endif
#if VSF_USE_USB_HOST_DS4 == ENABLED
    vsf_usbh_register_class(&usrapp.usbh, &usrapp.ds4);
#endif

#if VSF_USE_UI == ENABLED
#   if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#   endif
    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_t *disp;

    lv_disp_buf_init(   &usrapp.ui.disp_buf,
                        &usrapp.ui.color,
                        NULL,
                        LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = vsf_lvgl_disp_flush;
    disp_drv.buffer = &usrapp.ui.disp_buf;
    disp = lv_disp_drv_register(&disp_drv);
    vsf_lvgl_bind(&usrapp.ui.disp.use_as__vsf_disp_t, &disp->driver);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = usrapp_touchscreen_read;
    lv_indev_drv_register(&indev_drv);

    ui_demo_start();
    while (1) {
        lv_task_handler();
    }
#endif
    return 0;
}

/* EOF */
