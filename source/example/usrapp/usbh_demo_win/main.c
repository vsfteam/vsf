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

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "component/3rd-party/littlevgl/6.0/port/vsf_lvgl_port.h"

/*============================ MACROS ========================================*/

#define GENERATE_HEX(value)                TPASTE2(0x, value)

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

    vsf_disp_sdl2_t disp;

    struct {
        lv_disp_buf_t disp_buf;
        lv_disp_drv_t disp_drv;
        lv_color_t color[LV_VER_RES_MAX][LV_HOR_RES_MAX];
    } ui;
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

    .disp                       = {
        .param                  = {
            .height             = LV_VER_RES_MAX,
            .width              = LV_HOR_RES_MAX,
            .drv                = &vsf_disp_drv_sdl2,
            .color              = VSF_DISP_COLOR_RGB565,
        },
        .amplifier              = 2,
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// lv demo
void lv_refr_task(struct _lv_task_t *task)
{
    static lv_coord_t y;
    lv_obj_t *label1 = (lv_obj_t *)task->user_data;

    y = (y + 10) % LV_VER_RES;
  
    lv_obj_set_y(label1, y);
}

void lvgl_create_demo(void)
{
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");
    /* Align the Label to the center
    * NULL means align on parent (which is the screen now)
    * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_task_create(lv_refr_task, 100, LV_TASK_PRIO_LOWEST, label1);
}

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

#if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#endif
    lv_init();

    lv_disp_buf_init(   &usrapp.ui.disp_buf,
                        &usrapp.ui.color,
                        NULL,
                        LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_init(&usrapp.ui.disp_drv);

    usrapp.ui.disp_drv.hor_res = LV_HOR_RES_MAX;
    usrapp.ui.disp_drv.ver_res = LV_VER_RES_MAX;
    usrapp.ui.disp_drv.flush_cb = vsf_lvgl_disp_flush;
    usrapp.ui.disp_drv.buffer = &usrapp.ui.disp_buf;
    vsf_lvgl_bind(&usrapp.disp.use_as__vsf_disp_t, &usrapp.ui.disp_drv);
    lv_disp_drv_register(&usrapp.ui.disp_drv);

    lvgl_create_demo();
    while (1) {
        lv_task_handler();
    }
    return 0;
}

/* EOF */
