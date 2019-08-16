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

#include "component/3rd-party/littlevgl/5.3/raw/lvgl/lvgl.h"
#include "lv_conf.h"
#include "vsf.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

WEAK void uvc_app_fill_line(void *line_buf, uint_fast16_t size, bool last_line);

static void lvgl_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color)
{
    uint_fast32_t pixel_number = LV_HOR_RES;

    if ((0 == x1) && ((LV_HOR_RES - 1) == x2) && (y1 == y2)) {
        uvc_app_fill_line((void *)color, pixel_number * LV_COLOR_DEPTH / 8, (y1 == (LV_VER_RES - 1)));
    } else {
        vsf_trace(0, "non-line disp area [%d,%d], [%d,%d]\r\n", x1, y1, x2, y2);
        ASSERT(false);
    }
}

void uvc_app_on_fill_line_cpl(bool frame_cpl)
{
    lv_flush_ready();
}

static void lv_refr_task(void * param)
{
    static lv_coord_t y;
    lv_obj_t *label1 = (lv_obj_t *)param;

    y = (y + 1) % LV_VER_RES;
  
    lv_obj_set_y(label1, y);
}

static void lvgl_create_demo(void)
{
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);
    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");
    /* Align the Label to the center
    * NULL means align on parent (which is the screen now)
    * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_task_create(lv_refr_task, 33, LV_TASK_PRIO_LOWEST, label1);
}

void uvc_app_init(void)
{
    static NO_INIT lv_disp_drv_t __disp_drv;

    lv_init();
    lv_disp_drv_init(&__disp_drv);
    __disp_drv.disp_flush = lvgl_disp_flush;
    lv_disp_drv_register(&__disp_drv);

    lvgl_create_demo();
}

void uvc_app_task(void)
{
    lv_task_handler();
}

/* EOF */
