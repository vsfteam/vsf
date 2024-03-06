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
#if VSF_USE_INPUT == ENABLED
#   include "component/input/vsf_input.h"
#endif

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
VSF_CAL_NO_INIT static vsf_eda_t * eda_pending;
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
    VSF_UI_ASSERT(eda_pending != NULL);
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

#if VSF_USE_INPUT == ENABLED
uint32_t vsf_input_keycode_to_lvgl_keycode(uint16_t keycode)
{
    if ((VSF_KB_a <= keycode) && (keycode <= VSF_KB_z)) {
        return 'a' + (keycode - VSF_KB_a);
    } else if ((VSF_KB_A <= keycode) && (keycode <= VSF_KB_Z)) {
        return 'A' + (keycode - VSF_KB_A);
    } else if ((VSF_KB_1 <= keycode) && (keycode <= VSF_KB_9)) {
        return '1' + (keycode - VSF_KB_1);
    } else if ((VSF_KP_1 <= keycode) && (keycode <= VSF_KP_9)) {
        return '1' + (keycode - VSF_KP_1);
    }

    switch (keycode) {
    case VSF_KB_0:              return '0';
    case VSF_KB_ENTER:          return LV_KEY_ENTER;
    case VSF_KB_ESCAPE:         return LV_KEY_ESC;
    case VSF_KB_BACKSPACE:      return LV_KEY_BACKSPACE;
    case VSF_KB_SPACE:          return ' ';
    case VSF_KB_MINUS:          return '-';
    case VSF_KB_EQUAL:          return '=';
    case VSF_KB_LEFT_BRACKET:   return '[';
    case VSF_KB_RIGHT_BRACKET:  return ']';
    case VSF_KB_BACKSLASH:      return '\\';
    case VSF_KB_SEMICOLON:      return ';';
    case VSF_KB_SINGLE_QUOTE:   return '\'';
    case VSF_KB_GRAVE:          return '`';
    case VSF_KB_COMMA:          return ',';
    case VSF_KB_DOT:            return '.';
    case VSF_KB_SLASH:          return '/';
    case VSF_KB_HOME:           return LV_KEY_HOME;
    case VSF_KB_DELETE:         return LV_KEY_DEL;
    case VSF_KB_END:            return LV_KEY_END;
    case VSF_KB_RIGHT:          return LV_KEY_RIGHT;
    case VSF_KB_LEFT:           return LV_KEY_LEFT;
    case VSF_KB_DOWN:           return LV_KEY_DOWN;
    case VSF_KB_UP:             return LV_KEY_UP;
    case VSF_KP_DIVIDE:         return '/';
    case VSF_KP_MULTIPLY:       return '*';
    case VSF_KP_MINUS:          return '-';
    case VSF_KP_PLUS:           return '+';
    case VSF_KP_ENTER:          return LV_KEY_ENTER;
    case VSF_KP_0:              return '0';
    case VSF_KP_DOT:            return '.';
    case VSF_KP_EQUAL:          return '=';
    case VSF_KB_EXCLAM:         return '!';
    case VSF_KB_AT:             return '@';
    case VSF_KB_POUND:          return '#';
    case VSF_KB_DOLLAR:         return '$';
    case VSF_KB_PERCENT:        return '%';
    case VSF_KB_CARET:          return '^';
    case VSF_KB_AMPERSAND:      return '&';
    case VSF_KB_ASTERISK:       return '*';
    case VSF_KB_LEFT_PAREN:     return '(';
    case VSF_KB_RIGHT_PAREN:    return ')';
    case VSF_KB_UNDERSCORE:     return '_';
    case VSF_KB_PLUS:           return '+';
    case VSF_KB_LEFT_BRACE:     return '{';
    case VSF_KB_RIGHT_BRACE:    return '}';
    case VSF_KB_BAR:            return '|';
    case VSF_KB_COLON:          return ':';
    case VSF_KB_DOUBLE_QUOTE:   return '"';
    case VSF_KB_TIDE:           return '~';
    case VSF_KB_LESS:           return '<';
    case VSF_KB_GREATER:        return '>';
    case VSF_KB_QUESTION:       return '?';
    default:                    return 0;
    }
}
#endif

#endif
