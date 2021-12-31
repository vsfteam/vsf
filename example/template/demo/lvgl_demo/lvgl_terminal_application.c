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

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_LVGL == ENABLED && APP_LVGL_DEMO_USE_TERMINAL == ENABLED

// for stdlib.h
#include "utilities/vsf_utilities.h"
#include "../common/usrapp_common.h"
#include "lvgl/lvgl.h"
#include "lv_conf.h"

#include "shell/sys/linux/port/busybox/config.h"

/*============================ MACROS ========================================*/

#ifndef LV_TERMIAL_APP_STREAM_CFG_RX_BUF_SIZE
#   define LV_TERMIAL_APP_STREAM_CFG_RX_BUF_SIZE            32
#endif

#ifndef LV_TERMIAL_APP_STREAM_CFG_TX_BUF_SIZE
#   define LV_TERMIAL_APP_STREAM_CFG_TX_BUF_SIZE            (1024)
#endif

#ifndef LV_TERMIAL_APP_STREAM_TX_PRIO
#   define LV_TERMIAL_APP_STREAM_TX_PRIO                    vsf_prio_0
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct lvgl_terminal_app_const_t {
    vsf_stream_op_t tx_op;

    struct {
        const char **map;
        const lv_btnmatrix_ctrl_t *const ctrl_map;
    } keyboard_maps[3];

    const char **apps_btnm_map;
};

struct lvgl_termial_app_t {
    lv_obj_t *tabview;

    struct {
        lv_style_t tabview;
        lv_style_t terminal;
    } styles;

    struct {
        vsf_eda_t eda;
    } apps;

    struct {
        lv_obj_t *textarea;
        lv_obj_t *keyboard;

        struct {
            uint8_t buf[LV_TERMIAL_APP_STREAM_CFG_TX_BUF_SIZE];
            vsf_byte_fifo_t fifo;
        } tx;

        struct {
            uint8_t buf[LV_TERMIAL_APP_STREAM_CFG_RX_BUF_SIZE];
            uint16_t offset;
        } rx;
    } terminal;

    vsf_callback_timer_t timer;
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __lvgl_terminal_stream_init(vsf_stream_t *stream);
static uint_fast32_t __lvgl_terminal_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __lvgl_terminal_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __lvgl_terminal_stream_get_avail_length(vsf_stream_t *stream);

/*============================ LOCAL VARIABLES ===============================*/

static const char *__keyboard_lower_map[] = {
    "1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
    "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
    "_", "-", "z", "x", "c", "v", "b", "n", "m", " ", LV_SYMBOL_UP, " ", "\n",
    LV_SYMBOL_CLOSE, " ", LV_SYMBOL_LEFT, LV_SYMBOL_DOWN,  LV_SYMBOL_RIGHT, ""
};
static const lv_btnmatrix_ctrl_t __keyboard_lower_ctrl_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, LV_BTNMATRIX_CTRL_HIDDEN | 1, 1, LV_BTNMATRIX_CTRL_HIDDEN | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 7, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1
};
static const char *__keyboard_up_map[] = {
    "1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
    "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
    "_", "-", "Z", "X", "C", "V", "B", "N", "M", " ", LV_SYMBOL_UP, " ", "\n",
    LV_SYMBOL_CLOSE, " ", LV_SYMBOL_LEFT, LV_SYMBOL_DOWN,  LV_SYMBOL_RIGHT, ""
};
static const lv_btnmatrix_ctrl_t __keyboard_up_ctrl_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, LV_BTNMATRIX_CTRL_HIDDEN | 1, 1, LV_BTNMATRIX_CTRL_HIDDEN | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 7, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1
};
static const char *__keyboard_spec_map[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
    "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
    "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
    LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
};
static const lv_btnmatrix_ctrl_t __keyboard_spec_ctrl_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char *__demo_apps_name[] = {
#if APP_USE_LUA_DEMO == ENABLED
    "love",
#endif
    "",
};

static const struct lvgl_terminal_app_const_t __demo_const = {
    .tx_op = {
        .init = __lvgl_terminal_stream_init,
        .get_data_length = __lvgl_terminal_stream_get_data_length,
        .get_avail_length = __lvgl_terminal_stream_get_avail_length,
        .write = __lvgl_terminal_stream_write,
    },

    .keyboard_maps = {
        [LV_KEYBOARD_MODE_TEXT_LOWER] = {
            .map = __keyboard_lower_map,
            .ctrl_map = __keyboard_lower_ctrl_map,
        },
        [LV_KEYBOARD_MODE_TEXT_UPPER] = {
            .map = __keyboard_up_map,
            .ctrl_map = __keyboard_up_ctrl_map,
        },
        [LV_KEYBOARD_MODE_SPECIAL] = {
            .map = __keyboard_spec_map,
            .ctrl_map = __keyboard_spec_ctrl_map,
        },
    },

    .apps_btnm_map = __demo_apps_name,
};

static struct lvgl_termial_app_t __demo = {
    .terminal.tx.fifo = {
        .buffer = __demo.terminal.tx.buf,
        .size = dimof(__demo.terminal.tx.buf),
    },
};

vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op = &__demo_const.tx_op,
};

vsf_mem_stream_t VSF_DEBUG_STREAM_RX = {
    .op = &vsf_mem_stream_op,
    .buffer = __demo.terminal.rx.buf,
    .size = sizeof(__demo.terminal.rx.buf),
};

/*============================ IMPLEMENTATION ================================*/

static bool __terminal_add_char(void)
{
    lv_obj_t *textarea = __demo.terminal.textarea;
    if (textarea == NULL) {
        return false;
    }

    // TODO: support utf-8
    static uint8_t __last_ch;
    uint8_t ch;
    while (1 == vsf_byte_fifo_read(&__demo.terminal.tx.fifo, &ch, 1)) {
        if (ch == '\b' || ch == 0x7F) {
            lv_textarea_del_char(textarea);
        } else if (ch == '\r') {
            lv_textarea_add_char(textarea, '\n');
        } else if (ch == '\n') {
            if (__last_ch != '\r') {
                lv_textarea_add_char(textarea, '\n');
            }
        } else {
            lv_textarea_add_char(textarea, ch);
        }
        __last_ch = ch;
    }
    return true;
}

static void __lvgl_all_in_one_thread(vsf_eda_t *eda, vsf_evt_t evt)
{
    switch (evt) {
    case VSF_EVT_INIT:
    case VSF_EVT_USER:
        __terminal_add_char();
    }
}

static void __lvgl_vsh_execute_cmd(char *cmd)
{
    char ch = VSH_ENTER_CHAR;
    vsf_stream_write(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t, (uint8_t *)cmd, strlen(cmd));
    vsf_stream_write(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t, (uint8_t *)&ch, 1);
}

static void __lvgl_terminal_on_timer(vsf_callback_timer_t *timer)
{
    if (usrapp_ui_common.disp != NULL) {
        // run current lvgl demo
        __lvgl_vsh_execute_cmd("lvgl");
        return;
    }
    vsf_callback_timer_add_ms(timer, 100);
}

static void __lvgl_terminal_stream_init(vsf_stream_t *stream)
{
    vsf_byte_fifo_init(&__demo.terminal.tx.fifo);
    vsf_callback_timer_init(&__demo.timer);
    __demo.timer.on_timer = __lvgl_terminal_on_timer;
    vsf_callback_timer_add_ms(&__demo.timer, 100);

    vsf_eda_cfg_t cfg = {
        .fn.evthandler = __lvgl_all_in_one_thread,
        .priority = LV_TERMIAL_APP_STREAM_TX_PRIO,
    };

    vsf_eda_start(&__demo.apps.eda, &cfg);
}

static uint_fast32_t __lvgl_terminal_stream_write(vsf_stream_t *stream,
    uint8_t *buf, uint_fast32_t size)
{
    vsf_byte_fifo_write(&__demo.terminal.tx.fifo, buf, size);
    vsf_eda_post_evt(&__demo.apps.eda, VSF_EVT_USER);

    return size;
}

static uint_fast32_t __lvgl_terminal_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}

static uint_fast32_t __lvgl_terminal_stream_get_avail_length(vsf_stream_t *stream)
{
    return -1;
}

static void __keyboard_event_cb(lv_obj_t *keyboard, lv_event_t event)
{
    //lv_keyboard_def_event_cb(keyboard, event);

    if (event != LV_EVENT_VALUE_CHANGED)
        return;

    lv_keyboard_ext_t *ext = lv_obj_get_ext_attr(keyboard);
    uint16_t btn_id = lv_btnmatrix_get_active_btn(keyboard);
    if (btn_id == LV_BTNMATRIX_BTN_NONE) {
        return;
    }
    if (lv_btnmatrix_get_btn_ctrl(keyboard, btn_id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED)) {
        return;
    }
    if (lv_btnmatrix_get_btn_ctrl(keyboard, btn_id, LV_BTNMATRIX_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) {
        return;
    }

    const char *txt = lv_btnmatrix_get_active_btn_text(keyboard);
    if (txt == NULL) {
        return;
    }

    /*Do the corresponding action according to the text of the button*/
    if (strcmp(txt, "abc") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_btnmatrix_set_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_LOWER].map);
        lv_btnmatrix_set_ctrl_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_LOWER].ctrl_map);
        return;
    } else if (strcmp(txt, "ABC") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_btnmatrix_set_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_UPPER].map);
        lv_btnmatrix_set_ctrl_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_UPPER].ctrl_map);
        return;
    } else if (strcmp(txt, "1#") == 0) {
        ext->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_btnmatrix_set_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_SPECIAL].map);
        lv_btnmatrix_set_ctrl_map(keyboard, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_SPECIAL].ctrl_map);
        return;
    } else if (strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        // TODO
        return;
    } else if (strcmp(txt, LV_SYMBOL_OK) == 0) {
        // TODO
        return;
    }

    uint8_t ch;
    uint8_t escape_code[] = { "\033[ " };
    if (strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
        ch = VSH_ENTER_CHAR;
    } else if (strcmp(txt, LV_SYMBOL_LEFT) == 0) {
        //lv_textarea_cursor_left(ext->ta);
        return;
    } else if (strcmp(txt, LV_SYMBOL_RIGHT) == 0) {
        //lv_textarea_cursor_right(ext->ta);
        return;
    } else if (strcmp(txt, LV_SYMBOL_UP) == 0) {
        escape_code[2] = 'A';
        vsf_stream_write(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t, escape_code, 3);
        return;
    } else if (strcmp(txt, LV_SYMBOL_DOWN) == 0) {
        escape_code[2] = 'B';
        vsf_stream_write(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t, escape_code, 3);
        return;
    } else if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
        ch = '\b';
    } else if (strcmp(txt, "+/-") == 0) {
        return;
    } else {
        ch = *txt;
    }

    vsf_stream_write(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t, &ch, 1);
}

static void __keyborad_create(void)
{
    lv_obj_t *textarea = __demo.terminal.textarea;
    lv_obj_t *parent = lv_obj_get_parent(textarea);

    lv_coord_t height = lv_obj_get_height(parent);

    lv_obj_t *keyboard = lv_keyboard_create(parent, NULL);
    lv_keyboard_set_cursor_manage(keyboard, true);

    lv_obj_set_height(keyboard, height * 1 / 3);
    lv_obj_align(keyboard, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_event_cb(keyboard, __keyboard_event_cb);

    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_LOWER].map);
    lv_keyboard_set_ctrl_map(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER, __demo_const.keyboard_maps[LV_KEYBOARD_MODE_TEXT_LOWER].ctrl_map);

    __demo.terminal.keyboard = keyboard;
}

lv_obj_t *__terminal_create(lv_obj_t *parent)
{
    lv_coord_t width = lv_obj_get_width(parent);
    lv_coord_t height = lv_obj_get_height(parent);

    lv_style_init(&__demo.styles.terminal);
    lv_style_set_text_line_space(&__demo.styles.terminal, LV_STATE_DEFAULT, LV_DPX(2));
    lv_style_set_text_opa(&__demo.styles.terminal, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_text_color(&__demo.styles.terminal, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&__demo.styles.terminal, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&__demo.styles.terminal, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    lv_obj_t *textarea = lv_textarea_create(parent, NULL);
    lv_obj_add_style(textarea, LV_LABEL_PART_MAIN, &__demo.styles.terminal);
    lv_obj_set_size(textarea, width, height * 2 / 3);
    lv_obj_align(textarea, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_textarea_set_text(textarea, "");
    lv_textarea_set_cursor_click_pos(textarea, false);
    lv_textarea_set_scrollbar_mode(textarea, LV_SCROLLBAR_MODE_DRAG);

    __demo.terminal.textarea = textarea;

    __keyborad_create();

    return textarea;
}

static void __apps_btnm_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        extern void __lvgl_stop_looping(void);
        __lvgl_stop_looping();

        const char *text = lv_btnmatrix_get_active_btn_text(obj);
        __lvgl_vsh_execute_cmd((char *)text);
        lv_tabview_set_tab_act(__demo.tabview, 0, LV_ANIM_ON);
    }
}

static lv_obj_t *__apps_create(lv_obj_t *parent)
{
    lv_coord_t height = lv_obj_get_height(parent);
    lv_coord_t width = lv_obj_get_width(parent);

    lv_obj_t *apps = lv_btnmatrix_create(parent, NULL);
    lv_btnmatrix_set_map(apps, __demo_const.apps_btnm_map);
    lv_obj_align(apps, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_size(apps, width, height);     /*Circular scroll*/
    lv_obj_set_event_cb(apps, __apps_btnm_event_handler);

    return apps;
}

void lvgl_terminal_application(void)
{
    lv_style_init(&__demo.styles.tabview);
    lv_style_set_pad_top(&__demo.styles.tabview, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_bottom(&__demo.styles.tabview, LV_STATE_DEFAULT, 6);

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_add_style(tabview, LV_TABVIEW_PART_TAB_BTN, &__demo.styles.tabview);
    lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_TOP);

    __demo.tabview = tabview;

    lv_obj_t *terminal_tab = lv_tabview_add_tab(tabview, "Terminal");
    lv_page_set_scrlbar_mode(terminal_tab, LV_SCROLLBAR_MODE_OFF);
    __terminal_create(terminal_tab);

    lv_obj_t *applications = lv_tabview_add_tab(tabview, "Applications");
    lv_page_set_scrlbar_mode(applications, LV_SCROLLBAR_MODE_OFF);
    __apps_create(applications);
}


#endif
