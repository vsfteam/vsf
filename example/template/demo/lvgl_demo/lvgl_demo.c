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

#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED && APP_USE_LVGL_DEMO == ENABLED

#include "../common/usrapp_common.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "component/3rd-party/lvgl/port/vsf_lvgl_port.h"

#if APP_LVGL_DEMO_CFG_FREETYPE == ENABLED
#include "component/3rd-party/lvgl/extension/lv_lib_freetype/raw/lv_freetype.h"
#endif

#if APP_USE_LINUX_DEMO == ENABLED
#   include <pthread.h>
#endif
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT bool __lvgl_is_looping;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static lv_coord_t __lvgl_touchscreen_get_width(void)
{
    return vsf_min(LV_HOR_RES_MAX, usrapp_ui_common.disp->param.width);
}

static lv_coord_t __lvgl_touchscreen_get_height(void)
{
    return vsf_min(LV_VER_RES_MAX, usrapp_ui_common.disp->param.height);
}

static void __lvgl_on_evt(vk_input_notifier_t *notifier, vk_input_type_t type, vk_input_evt_t *evt)
{
    switch (type) {
#if APP_LVGL_DEMO_USE_TOUCHSCREEN == ENABLED
    case VSF_INPUT_TYPE_TOUCHSCREEN: {
            vk_touchscreen_evt_t *ts_evt = (vk_touchscreen_evt_t *)evt;
            if (0 == vsf_input_touchscreen_get_id(ts_evt)) {
                usrapp_ui_common.lvgl.ts_evt = *ts_evt;
                if (usrapp_ui_common.lvgl.eda_poll != NULL) {
                    vsf_eda_post_evt(usrapp_ui_common.lvgl.eda_poll, VSF_EVT_USER);
                }
            }
        }
        break;
#else
    case VSF_INPUT_TYPE_MOUSE: {
            vk_mouse_evt_t* mouse_evt = (vk_mouse_evt_t*)evt;
            usrapp_ui_common.lvgl.mouse_evt = *mouse_evt;
            if (usrapp_ui_common.lvgl.eda_poll != NULL) {
                vsf_eda_post_evt(usrapp_ui_common.lvgl.eda_poll, VSF_EVT_USER);
            }
        }
        break;
#endif

#if LV_USE_GROUP == ENABLED
    case VSF_INPUT_TYPE_KEYBOARD: {
            vk_keyboard_evt_t *kb_evt = (vk_keyboard_evt_t *)evt;
            usrapp_ui_common.lvgl.kb_evt = *kb_evt;

            if (usrapp_ui_common.lvgl.eda_poll != NULL) {
                vsf_eda_post_evt(usrapp_ui_common.lvgl.eda_poll, VSF_EVT_USER);
            }
        }
        break;
#endif
    }
}

#if APP_LVGL_DEMO_USE_TOUCHSCREEN == ENABLED
static bool __lvgl_touchscreen_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    vk_touchscreen_evt_t *ts_evt = &usrapp_ui_common.lvgl.ts_evt;
    data->state = vsf_input_touchscreen_is_down(ts_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

#if APP_LVGL_DEMO_CFG_TOUCH_REMAP == ENABLED
    data->point.x = vsf_input_touchscreen_get_x(ts_evt) * __lvgl_touchscreen_get_width() / ts_evt->info.width;
    data->point.y = vsf_input_touchscreen_get_y(ts_evt) * __lvgl_touchscreen_get_height() / ts_evt->info.height;
#else
    data->point.x = vsf_input_touchscreen_get_x(ts_evt);
    data->point.y = vsf_input_touchscreen_get_y(ts_evt);
#endif

//    vsf_trace(VSF_TRACE_DEBUG, "touchscreen: %s x=%d, y=%d" VSF_TRACE_CFG_LINEEND,
//        data->state == LV_INDEV_STATE_PR ? "press" : "release",
//        data->point.x, data->point.y);
    return false;
}
#else
static bool __lvgl_mouse_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    vk_mouse_evt_t* mouse_evt = &usrapp_ui_common.lvgl.mouse_evt;
    lv_indev_state_t* state = &usrapp_ui_common.lvgl.state;

    uint8_t id = vk_input_mouse_evt_get(mouse_evt);
    if (id == VSF_INPUT_MOUSE_EVT_BUTTON) {
        // only support left key
        if (0 == vk_input_mouse_evt_button_get(mouse_evt)) {
            *state = vk_input_mouse_evt_button_is_down(mouse_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
        }
    }

    data->point.x = vk_input_mouse_evt_get_x(mouse_evt);
    data->point.y = vk_input_mouse_evt_get_y(mouse_evt);
    data->state = *state;

    return false;
}
#endif

#if LV_USE_GROUP == ENABLED
static uint32_t __input_keycode_to_lvgl_keycode(uint16_t keycode)
{
    if (VSF_KB_UNKNOWN == keycode) {
        return 0;
    } else if((VSF_KB_a <= keycode) && (keycode <= VSF_KB_z)) {
        return 'a' + (keycode - VSF_KB_a);
    } else if ((VSF_KB_1 <= keycode) && (keycode <= VSF_KB_9)) {
        return '1' + (keycode - VSF_KB_1);
    } else if (VSF_KB_0 == keycode) {
        return '0';
    } else if (VSF_KB_ENTER == keycode) {
		return LV_KEY_ENTER;
    } else if (VSF_KB_ESCAPE == keycode) {
		return LV_KEY_ESC;
    } else if (VSF_KB_BACKSPACE == keycode) {
		return LV_KEY_BACKSPACE;
    } else if (VSF_KB_TAB == keycode) {
        return 0;
    } else if (VSF_KB_SPACE == keycode) {
        return ' ';
    } else if (VSF_KB_MINUS == keycode) {
        return '-';
    } else if (VSF_KB_EQUAL == keycode) {
        return '=';
    } else if (VSF_KB_LEFT_BRACKET == keycode) {
		return '[';
    } else if (VSF_KB_RIGHT_BRACKET == keycode) {
        return ']';
    } else if (VSF_KB_BACKSLASH == keycode) {
	    return '\\';
    } else if (VSF_KB_SEMICOLON == keycode) {
	    return ';';
    } else if (VSF_KB_SINGLE_QUOTE == keycode) {
	    return '\'';
    } else if (VSF_KB_GRAVE == keycode) {
    	return '`';
    } else if (VSF_KB_COMMA == keycode) {
	    return ',';
    } else if (VSF_KB_DOT == keycode) {
		return '.';
    } else if (VSF_KB_SLASH == keycode) {
		return '/';
    } else if (VSF_KB_CAPSLOCK == keycode) {
        return 0;
    } else if ((VSF_KB_F1 <= keycode) && (keycode <= VSF_KB_F12)) {
        return 0;
    } else if (VSF_KB_PRINT_SCREEN == keycode) {
        return 0;
    } else if (VSF_KB_SCROLL_LOCK == keycode) {
        return 0;
    } else if (VSF_KB_PAUSE == keycode) {
        return 0;
    } else if (VSF_KB_INSERT == keycode) {
        return 0;
    } else if (VSF_KB_HOME == keycode) {
		return LV_KEY_HOME;
    } else if (VSF_KB_PAGE_UP == keycode) {
        return 0;
    } else if (VSF_KB_DELETE == keycode) {
		return LV_KEY_DEL;
    } else if (VSF_KB_END == keycode) {
		return LV_KEY_END;
    } else if (VSF_KB_PAGE_DOWN == keycode) {
        return 0;
    } else if (VSF_KB_RIGHT == keycode) {
        return LV_KEY_RIGHT;
    } else if (VSF_KB_LEFT == keycode) {
		return LV_KEY_LEFT;
    } else if (VSF_KB_DOWN == keycode) {
		return LV_KEY_DOWN;
    } else if (VSF_KB_UP == keycode) {
		return LV_KEY_UP;
    } else if (VSF_KP_NUMLOCK == keycode) {
        return 0;
    } else if (VSF_KP_DIVIDE == keycode) {
		return '/';
    } else if (VSF_KP_MULTIPLY == keycode) {
        return '*';
    } else if (VSF_KP_MINUS == keycode) {
        return '-';
    } else if (VSF_KP_PLUS == keycode) {
        return '+';
    } else if (VSF_KP_ENTER == keycode) {
		return LV_KEY_ENTER;
    } else if ((VSF_KP_1 <= keycode) && (keycode <= VSF_KP_9)) {
        return '1' + (keycode - VSF_KP_1);
    } else if (VSF_KP_0 == keycode) {
        return '0';
    } else if (VSF_KP_DOT == keycode) {
        return '.';
    } else if (VSF_KP_EQUAL == keycode) {
        return '=';
    } else if (VSF_KB_EXCLAM == keycode) {
        return '!';
    } else if (VSF_KB_AT == keycode) {
        return '@';
    } else if (VSF_KB_POUND == keycode) {
        return '#';
    } else if (VSF_KB_DOLLAR == keycode) {
        return '$';
    } else if (VSF_KB_PERCENT == keycode) {
        return '%';
    } else if (VSF_KB_CARET == keycode) {
        return '^';
    } else if (VSF_KB_AMPERSAND == keycode) {
        return '&';
    } else if (VSF_KB_ASTERISK == keycode) {
        return '*';
    } else if (VSF_KB_LEFT_PAREN == keycode) {
        return '(';
    } else if (VSF_KB_RIGHT_PAREN == keycode) {
        return ')';
    } else if (VSF_KB_UNDERSCORE == keycode) {
        return '_';
    } else if (VSF_KB_PLUS == keycode) {
        return '+';
    } else if (VSF_KB_LEFT_BRACE == keycode) {
        return '{';
    } else if (VSF_KB_RIGHT_BRACE == keycode) {
        return '}';
    } else if (VSF_KB_BAR == keycode) {
        return '|';
    } else if (VSF_KB_COLON == keycode) {
        return ':';
    } else if (VSF_KB_DOUBLE_QUOTE == keycode) {
        return '"';
    } else if (VSF_KB_TIDE == keycode) {
        return '~';
    } else if (VSF_KB_LESS == keycode) {
        return '<';
    } else if (VSF_KB_GREATER == keycode) {
        return '>';
    } else if (VSF_KB_QUESTION == keycode) {
        return '?';
    } else if ((VSF_KB_A <= keycode) && (keycode <= VSF_KB_Z)) {
        return 'A' + (keycode - VSF_KB_A);
    } else {
        //VSF_ASSERT(0);
        return 0;
    }
}

static bool __lvgl_keyboard_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static bool __last_state = LV_INDEV_STATE_REL;
    static bool __more_to_read = false;
    static uint16_t __last_key = 0;

    if (__more_to_read) {
        data->key = __last_key;
        data->state = __last_state;
        __more_to_read = false;
    } else {
        vk_keyboard_evt_t *kb_evt = &usrapp_ui_common.lvgl.kb_evt;
        uint16_t key = __input_keycode_to_lvgl_keycode(vsf_input_keyboard_get_keycode(kb_evt));
        bool state = vsf_input_keyboard_is_down(kb_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

        __more_to_read = (__last_state == LV_INDEV_STATE_PR) && (__last_key != key);
        data->key   = __more_to_read ? __last_key         : key;
        data->state = __more_to_read ? LV_INDEV_STATE_REL : state;

        __last_key = key;
        __last_state = state;
    }

    return __more_to_read;
}
#endif

static void __lvgl_input_init(void)
{
#if APP_LVGL_DEMO_USE_TOUCHSCREEN == ENABLED
    uint8_t mask = (1 << VSF_INPUT_TYPE_TOUCHSCREEN) | (1 << VSF_INPUT_TYPE_KEYBOARD);
#else
    uint8_t mask = (1 << VSF_INPUT_TYPE_MOUSE) | (1 << VSF_INPUT_TYPE_KEYBOARD);
#endif

    usrapp_ui_common.lvgl.notifier.mask = mask;
    usrapp_ui_common.lvgl.notifier.on_evt = __lvgl_on_evt;
    vk_input_notifier_register(&usrapp_ui_common.lvgl.notifier);
}

void __lvgl_stop_looping(void)
{
    __lvgl_is_looping = false;
}

#if APP_USE_LINUX_DEMO == ENABLED
void * __lvgl_thread(void *arg)
{
#if APP_LVGL_DEMO_CFG_ANIMINATION != ENABLED
    usrapp_ui_common.lvgl.eda_poll = vsf_eda_get_cur();
#endif

    __lvgl_input_init();

    while (__lvgl_is_looping) {
        lv_task_handler();
#if APP_LVGL_DEMO_CFG_ANIMINATION == ENABLED
        vsf_thread_delay_ms(10);
#else
        vsf_thread_wfe(VSF_EVT_USER);
#endif
    }
    return NULL;
}

static NO_INIT vsf_eda_t *__lvgl_demo_evt_to_notify;
void __lvgl_on_disp_drv_inited(lv_disp_drv_t *disp_drv)
{
    if (__lvgl_demo_evt_to_notify != NULL) {
        vsf_eda_post_evt(__lvgl_demo_evt_to_notify, VSF_EVT_USER);
    }
}

int lvgl_main(int argc, char *argv[])
{
    uint_fast8_t gamepad_num = 1;
    if (argc > 2) {
        printf("format: %s [GAMEPAD_NUM<1..4>]", argv[1]);
        return -1;
    } else if (argc > 1) {
        gamepad_num = strtoul(argv[1], NULL, 0);
    }

    if ((gamepad_num <= 0) || (gamepad_num > 4)) {
        printf("invalid gamepad_num %d", gamepad_num);
        return -1;
    }
#else
static NO_INIT bool __lvgl_is_disp_inited;
void __lvgl_on_disp_drv_inited(lv_disp_drv_t *disp_drv)
{
    __lvgl_is_disp_inited = true;
}

int VSF_USER_ENTRY(void)
{
    uint_fast8_t gamepad_num = 1;
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    vk_disp_t *vsf_disp = usrapp_ui_common.disp;
    if (NULL == vsf_disp) {
        return -1;
    }

    if (vsf_disp->param.color != VSF_DISP_COLOR_RGB565) {
        // insecure operation
        ((vk_disp_param_t *)&vsf_disp->param)->color = VSF_DISP_COLOR_RGB565;
    }

#   if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#   endif

    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_t *disp;

#ifdef APP_LVGL_DEMO_CFG_PIXEL_BUFFER_HEAP
    usrapp_ui_common.lvgl.color = vsf_heap_malloc(APP_LVGL_DEMO_CFG_PIXEL_BUFFER_SIZE
            *   sizeof(usrapp_ui_common.lvgl.color[0])
            *   ((APP_LVGL_DEMO_CFG_DOUBLE_BUFFER == ENABLED) ? 2 : 1));
    VSF_ASSERT(usrapp_ui_common.lvgl.color != NULL);
#endif
    lv_disp_buf_init(   &usrapp_ui_common.lvgl.disp_buf,
                        usrapp_ui_common.lvgl.color,
#if APP_LVGL_DEMO_CFG_DOUBLE_BUFFER == ENABLED
                        usrapp_ui_common.lvgl.color + APP_LVGL_DEMO_CFG_PIXEL_BUFFER_SIZE,
#else
                        NULL,
#endif
                        APP_LVGL_DEMO_CFG_PIXEL_BUFFER_SIZE);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = __lvgl_touchscreen_get_width();
    disp_drv.ver_res = __lvgl_touchscreen_get_height();
    disp_drv.buffer = &usrapp_ui_common.lvgl.disp_buf;
    disp = lv_disp_drv_register(&disp_drv);

#if APP_USE_LINUX_DEMO == ENABLED
    __lvgl_demo_evt_to_notify = vsf_eda_get_cur();
#else
    __lvgl_is_disp_inited = false;
#endif
    vsf_lvgl_bind_disp(vsf_disp, &disp->driver, __lvgl_on_disp_drv_inited);

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
#if APP_LVGL_DEMO_USE_TOUCHSCREEN == ENABLED
    indev_drv.read_cb = __lvgl_touchscreen_read;
#else
    indev_drv.read_cb = __lvgl_mouse_read;
#endif
    lv_indev_drv_register(&indev_drv);

#if LV_USE_GROUP == ENABLED
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.disp = disp;
    indev_drv.read_cb = __lvgl_keyboard_read;
    lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv);

    lv_group_t * group = lv_group_create();
    usrapp_ui_common.lvgl.group = group;
    lv_indev_set_group(kb_indev, group);
#endif

#if APP_LVGL_DEMO_CFG_FREETYPE == ENABLED
    lv_freetype_init(APP_LVGL_DEMO_CFG_FREETYPE_MAX_FACES);
#endif

#if APP_LVGL_DEMO_USE_TERMINAL == ENABLED
    extern void lvgl_terminal_application(void);
    lvgl_terminal_application();
#else
    extern void lvgl_application(uint_fast8_t);
    lvgl_application(gamepad_num);
#endif

    __lvgl_is_looping = true;
#if APP_USE_LINUX_DEMO == ENABLED
    // wait for disp_on_inited
    vsf_thread_wfe(VSF_EVT_USER);

    pthread_t thread;
    pthread_create(&thread, NULL, __lvgl_thread, NULL);
    pthread_join(thread, NULL);
#else
    __lvgl_input_init();

    while (!__lvgl_is_disp_inited);
    while (__lvgl_is_looping) {
        lv_task_handler();
    }
#endif

#ifdef APP_LVGL_DEMO_CFG_PIXEL_BUFFER_HEAP
    vsf_heap_free(usrapp_ui_common.lvgl.color);
    usrapp_ui_common.lvgl.color = NULL;
#endif
    return 0;
}

#endif
