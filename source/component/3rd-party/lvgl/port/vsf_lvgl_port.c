/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#include "component/ui/vsf_ui.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
#   include "kernel/vsf_kernel.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
#   if VSF_USE_KERNEL != ENABLED || VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#       error VSF_LVGL_IMP_WAIT_CB need kernel and thread
#   endif
#   if LVGL_VERSION_MAJOR < 7
#       error current lvgl does not support wait_cb
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
// TODO: support multiple displays
NO_INIT static vsf_eda_t * eda_pending;
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if USE_LV_LOG
void vsf_lvgl_printf(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{
    static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error"};
    vsf_trace_debug("%s: %s \t(%s #%d)\r\n", lvl_prefix[level], dsc,  file, line);
}
#endif

static void __vsf_lvgl_flush_disp(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    lv_coord_t hres = disp_drv->rotated == 0 ? disp_drv->hor_res : disp_drv->ver_res;
    lv_coord_t vres = disp_drv->rotated == 0 ? disp_drv->ver_res : disp_drv->hor_res;

    VSF_UI_ASSERT(disp_drv->user_data != NULL);
    vk_disp_t *disp = disp_drv->user_data;
    vk_disp_area_t disp_area;

    if(area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    VSF_UI_ASSERT(area->y2 >= area->y1);
    VSF_UI_ASSERT(area->x2 >= area->x1);

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
    eda_pending = vsf_eda_get_cur();
#endif

    disp_area.pos.x = area->x1;
    disp_area.pos.y = area->y1;
    disp_area.size.x = area->x2 + 1 - area->x1;
    disp_area.size.y = area->y2 + 1 - area->y1;
#if VSF_LVGL_IMP_WAIT_CB == ENABLED
    disp_drv->user_data = NULL;
#endif
    vk_disp_refresh(disp, &disp_area, color_p);
}

static void __vsf_lvgl_disp_on_ready(vk_disp_t *disp)
{
    lv_disp_drv_t *disp_drv = (lv_disp_drv_t *)disp->ui_data;
    lv_disp_flush_ready(disp_drv);

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
    vsf_protect_t orig = vsf_protect_int();
        void *user_data = disp_drv->user_data;
        disp_drv->user_data = disp;
    vsf_unprotect_int(orig);

    if (user_data != NULL) {
        vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
    }
#endif
}

#if VSF_LVGL_IMP_WAIT_CB == ENABLED
static void __vsf_lvgl_disp_wait_cb(lv_disp_drv_t *disp_drv)
{
    vsf_protect_t orig = vsf_protect_int();
    if (NULL == disp_drv->user_data) {
        disp_drv->user_data = (void *)-1;
        vsf_unprotect_int(orig);

        vsf_thread_wfe(VSF_EVT_USER);
    } else {
        vsf_unprotect_int(orig);
    }
}
#endif

static void __vsf_lvgl_disp_on_inited(vk_disp_t *disp)
{
    lv_disp_drv_t *disp_drv = (lv_disp_drv_t *)disp->ui_data;
    void (*on_inited)(lv_disp_drv_t *) =
                    (void (*)(lv_disp_drv_t *))disp_drv->user_data;

    disp_drv->user_data = disp;
    disp->ui_on_ready = __vsf_lvgl_disp_on_ready;

    if (on_inited != NULL) {
        on_inited(disp_drv);
    }
}

void vsf_lvgl_bind_disp(vk_disp_t *disp, lv_disp_drv_t *lvgl_disp_drv,
            void (*on_inited)(lv_disp_drv_t *disp_drv))
{
    lvgl_disp_drv->flush_cb = __vsf_lvgl_flush_disp;
#if VSF_LVGL_IMP_WAIT_CB == ENABLED
    lvgl_disp_drv->wait_cb = __vsf_lvgl_disp_wait_cb;
    eda_pending = NULL;
#endif
    lvgl_disp_drv->user_data = (void *)on_inited;
    disp->ui_data = lvgl_disp_drv;
    disp->ui_on_ready = __vsf_lvgl_disp_on_inited;
    vk_disp_init(disp);
}

#endif
