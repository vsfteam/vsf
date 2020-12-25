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

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED && APP_USE_LVGL_DEMO == ENABLED

#include "../common/usrapp_common.h"

#include "lvgl/lvgl.h"
#include "lv_conf.h"
#include "component/3rd-party/littlevgl/6.1.2/port/vsf_lvgl_port.h"

#if APP_LVGL_DEMO_CFG_FREETYPE == ENABLED
#include "component/3rd-party/littlevgl/6.1.2/extension/lv_lib_freetype/raw/lv_freetype.h"
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
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void __lvgl_on_evt(vk_input_type_t type, vk_input_evt_t *evt)
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
    data->point.x = vsf_input_touchscreen_get_x(ts_evt) * LV_HOR_RES_MAX / ts_evt->info.width;
    data->point.y = vsf_input_touchscreen_get_y(ts_evt) * LV_VER_RES_MAX / ts_evt->info.height;
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

    // only support left key
    if (0 == vk_input_mouse_evt_button_get(mouse_evt)) {
        data->state = vk_input_mouse_evt_button_is_down(mouse_evt) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = vk_input_mouse_evt_get_x(mouse_evt);
    data->point.y = vk_input_mouse_evt_get_y(mouse_evt);

    return false;
}
#endif

#if LV_USE_GROUP == ENABLED
static uint32_t __input_keycode_to_lvgl_keycode(uint16_t keymod, uint16_t keycode)
{
    static const char __shift_num[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
    bool is_shift = keymod & (VSF_KM_LEFT_SHIFT | VSF_KM_RIGHT_SHIFT);

    if ((VSF_KB_a <= keycode) && (keycode <= VSF_KB_z)) {
        return (is_shift ? 'A' : 'a') + (keycode - VSF_KB_a);
    } else if ((VSF_KB_A <= keycode) && (keycode <= VSF_KB_Z)) {
        return (is_shift ? 'a' : 'A') + (keycode - VSF_KB_A);
    } else if ((VSF_KB_1 <= keycode) && (keycode <= VSF_KB_9)) {
        return  is_shift ? __shift_num[keycode - VSF_KB_1 + 1] : '1' + (keycode - VSF_KB_1);
    } else if ((VSF_KP_1 <= keycode) && (keycode <= VSF_KP_9)) {
        return '1' + (keycode - VSF_KP_1);
    } else if (VSF_KB_0 == keycode) {
        return  is_shift ? __shift_num[keycode - VSF_KB_0] : '0' + (keycode - VSF_KB_0);
    } else if (VSF_KP_0 == keycode) {
        return '0';
    } else if ((VSF_KB_RIGHT == keycode) || (VSF_KB_PLUS == keycode)) {
        return LV_KEY_RIGHT;
    } else if ((VSF_KB_LEFT == keycode) || (VSF_KB_MINUS == keycode)) {
		return LV_KEY_LEFT;
	} else if (VSF_KB_ENTER == keycode || '\r' == keycode) {
		return LV_KEY_ENTER;
	} else if (VSF_KB_UP == keycode) {
		return LV_KEY_UP;
	} else if (VSF_KB_DOWN == keycode) {
		return LV_KEY_DOWN;
	} else if (VSF_KB_ESCAPE == keycode) {
		return LV_KEY_ESC;
	} else if (VSF_KB_BACKSPACE == keycode) {
		return LV_KEY_BACKSPACE;
	} else if (VSF_KB_DELETE == keycode) {
		return LV_KEY_DEL;
	} else if (VSF_KB_HOME == keycode) {
		return LV_KEY_HOME;
	} else if (VSF_KB_END == keycode) {
		return LV_KEY_END;
	} else if (VSF_KB_EQUAL == keycode) {
		return is_shift ? '+' : '=';
	} else if (VSF_KB_RIGHT_BRACKET == keycode) {
		return is_shift ? '}' : ']';
	} else if (VSF_KB_BACKSLASH == keycode) {
	    return is_shift ? '|' : '\\';
    } else if (VSF_KB_SEMICOLON == keycode) {
	    return is_shift ? ':' : ';';
    } else if (VSF_KB_SINGLE_QUOTE == keycode) {
	    return is_shift ? '"' : '\'';
    } else if (VSF_KB_GRAVE == keycode) {
    	return is_shift ? '~' : '`';
    } else if (VSF_KB_COMMA == keycode) {
	    return is_shift ? '<' : ',';
    } else if (VSF_KB_DOT == keycode) {
		return is_shift ? '>' : '.';
	} else if (VSF_KB_SLASH == keycode) {
		return is_shift ? '?' : '/';
	} else if (VSF_KB_LEFT_BRACKET == keycode) {
		return is_shift ? '{' : '[';
    } else {
        // VSF_KB_TAB, VSF_KB_SPACE, VSF_KB_CAPSLOCK, VSF_KB_F1,
		// VSF_KB_F2, VSF_KB_F3, VSF_KB_F4, VSF_KB_F5,
		// VSF_KB_F6, VSF_KB_F7, VSF_KB_F8, VSF_KB_F9,
		// VSF_KB_F10, VSF_KB_F11, VSF_KB_F12, VSF_KB_PRINT_SCREEN,
		// VSF_KB_SCROLL_LOCK, VSF_KB_PAUSE, VSF_KB_INSERT, VSF_KB_PAGE_UP,
		// VSF_KB_PAGE_DOWN, VSF_KP_NUMLOCK, VSF_KP_DIVIDE, VSF_KP_MULTIPLY,
		// VSF_KP_MINUS, VSF_KP_PLUS, VSF_KP_ENTER, VSF_KP_DOT,
		// VSF_KB_EXCLAM, VSF_KB_AT, VSF_KB_POUND, VSF_KB_DOLLAR,
		// VSF_KB_PERCENT, VSF_KB_CARET, VSF_KB_AMPERSAND, VSF_KB_ASTERISK,
		// VSF_KB_LEFT_PAREN, VSF_KB_RIGHT_PAREN, VSF_KB_UNDERSCORE, VSF_KB_LEFT_BRACE,
		// VSF_KB_RIGHT_BRACE, VSF_KB_BAR, VSF_KB_COLON, VSF_KB_DOUBLE_QUOTE,
		// VSF_KB_TIDE, VSF_KB_LESS, VSF_KB_GREATER, VSF_KB_QUESTION,
        return 0;
    }
}

static bool __lvgl_keyboard_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    vk_keyboard_evt_t *kb_evt = &usrapp_ui_common.lvgl.kb_evt;
    bool is_down = vsf_input_keyboard_is_down(kb_evt);
    uint16_t keymod = vsf_input_keyboard_get_keymod(kb_evt);
    uint16_t keycode = vsf_input_keyboard_get_keycode(kb_evt);

    data->key = __input_keycode_to_lvgl_keycode(keymod, keycode);
    data->state = (is_down && data->key) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

    return false;
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
    usrapp_ui_common.lvgl.notifier.on_evt = (vk_input_on_evt_t)__lvgl_on_evt;
    vk_input_notifier_register(&usrapp_ui_common.lvgl.notifier);
}

#if APP_USE_LINUX_DEMO == ENABLED

void * __lvgl_thread(void *arg)
{
    usrapp_ui_common.lvgl.eda_poll = vsf_eda_get_cur();

    __lvgl_input_init();

    while (1) {
        lv_task_handler();
        vsf_thread_wfe(VSF_EVT_USER);
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

#   if USE_LV_LOG
    lv_log_register_print(vsf_lvgl_printf);
#   endif

    lv_init();

    lv_disp_drv_t disp_drv;
    lv_indev_drv_t indev_drv;
    lv_disp_t *disp;

    lv_disp_buf_init(   &usrapp_ui_common.lvgl.disp_buf,
                        &usrapp_ui_common.lvgl.color,
                        NULL,
                        LV_HOR_RES_MAX * LV_VER_RES_MAX);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = vsf_lvgl_disp_flush;
    disp_drv.buffer = &usrapp_ui_common.lvgl.disp_buf;
    disp = lv_disp_drv_register(&disp_drv);

    // insecure operation
    ((vk_disp_param_t *)&usrapp_ui_common.disp.param)->color = VSF_DISP_COLOR_RGB565;
    vsf_lvgl_disp_bind(&usrapp_ui_common.disp.use_as__vk_disp_t, &disp->driver);

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

    extern void lvgl_application(uint_fast8_t);
    lvgl_application(gamepad_num);

#if APP_USE_LINUX_DEMO == ENABLED
    pthread_t thread;
    pthread_create(&thread, NULL, __lvgl_thread, NULL);
#else
    __lvgl_input_init();

    while (1) {
        lv_task_handler();
    }
#endif
    return 0;
}

#endif
