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

// for stdlib.h
#include "utilities/vsf_utilities.h"
#include "lvgl/lvgl.h"
#include "lv_conf.h"

/*============================ MACROS ========================================*/

// __R, __G, __B in [0, 255]
#if LV_COLOR_DEPTH == 16
#   define UI_COLOR(__R, __G, __B)                                              \
            (((uint16_t)(__R) >> 3) << 11)                                      \
        |   (((uint16_t)(__G) >> 2) << 5)                                       \
        |   (((uint16_t)(__B) >> 3) << 0)
#endif

#define UI_COLOR_BLACK              UI_COLOR(0, 0, 0)
#define UI_COLOR_WHITE              UI_COLOR(255, 255, 255)
#define UI_COLOR_GRAY               UI_COLOR(128, 128, 128)
#define UI_COLOR_DARK_GRAY          UI_COLOR(169, 169, 169)
#define UI_COLOR_LIGHT_GRAY         UI_COLOR(211, 211, 211)

#define UI_COLOR_BACKGROUND         UI_COLOR_BLACK
#define UI_COLOR_TGL_PR             UI_COLOR_WHITE
#define UI_COLOR_EDGE_TGL_PR        UI_COLOR_LIGHT_GRAY
#define UI_COLOR_TGL_REL            UI_COLOR_DARK_GRAY
#define UI_COLOR_EDGE_TGL_REL       UI_COLOR_GRAY
#define UI_COLOR_FONT               UI_COLOR_BLACK

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct lvgl_demo_pad_t {
    lv_obj_t *cont;
        lv_obj_t *btn_up;
        lv_obj_t *btn_down;
        lv_obj_t *btn_left;
        lv_obj_t *btn_right;
};
typedef struct lvgl_demo_pad_t lvgl_demo_pad_t;

struct lvgl_demo_joystick_t {
    lv_obj_t *cont;
        lv_obj_t *btn;
};
typedef struct lvgl_demo_joystick_t lvgl_demo_joystick_t;

struct lvgl_demo_gamepad_t {
    lv_coord_t margin;
    void *dev;

    lvgl_demo_pad_t pad_left, pad_right;
    lvgl_demo_joystick_t joystick_left, joystick_right;
    lv_obj_t *btn_lt;       // left trigger
        lv_obj_t *lbl_lt;
        char lbl_lt_text[10];
    lv_obj_t *btn_rt;       // right trigger
        lv_obj_t *lbl_rt;
        char lbl_rt_text[10];
    lv_obj_t *btn_lb;       // left shoulder
    lv_obj_t *btn_rb;       // right shoulder
    lv_obj_t *btn_lm;       // left menu
    lv_obj_t *btn_rm;       // right menu
    lv_obj_t *btn_mm;       // main menu
};
typedef struct lvgl_demo_gamepad_t lvgl_demo_gamepad_t;

struct lvgl_demo_t {
    vk_input_notifier_t notifier;
    lv_obj_t *cont[4];
        lvgl_demo_gamepad_t gamepad[4];
};
typedef struct lvgl_demo_t lvgl_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __lvgl_input_on_gamepad(vk_input_type_t type, vk_gamepad_evt_t *gamepad_evt);

/*============================ LOCAL VARIABLES ===============================*/

static lvgl_demo_t __lvgl_demo = {
    .notifier   = {
        .mask   = 1 << VSF_INPUT_TYPE_GAMEPAD,
        .on_evt = (vk_input_on_evt_t)__lvgl_input_on_gamepad,
    },
};

const static uint8_t __hid_dpad_to_mask[16] = {
	0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5,
	0x0, 0x0, 0x0, 0x0,	0x0, 0x0, 0x0, 0x0
};

/*============================ IMPLEMENTATION ================================*/

static float __lvgl_demo_calc_joystick_offset(vk_gamepad_evt_t *gamepad_evt)
{
    uint32_t mask = (1UL << gamepad_evt->bitlen) - 1;
    uint32_t cur = gamepad_evt->cur.valu32 & mask;
    int cur_i32;
    float result;

    if (gamepad_evt->is_signed) {
        cur_i32 = cur;
        cur_i32 <<= (32 - gamepad_evt->bitlen);
        cur_i32 >>= (32 - gamepad_evt->bitlen);
    } else {
        cur_i32 = cur - (1 << (gamepad_evt->bitlen - 1));
    }

    ASSERT(mask != 0);
    result = (float)cur_i32 / (float)mask;
    if (gamepad_evt->config) {
        result = -result;
    }
    return result;
}

static lvgl_demo_gamepad_t * __lvgl_demo_get_gamepad(void *dev)
{
    int empty_idx = -1;
    for (int i = 0; i < dimof(__lvgl_demo.gamepad); i++) {
        if ((NULL == __lvgl_demo.gamepad[i].dev) && (empty_idx < 0)) {
            empty_idx = i;
        }
        if (__lvgl_demo.gamepad[i].dev == dev) {
            return &__lvgl_demo.gamepad[i];
        }
    }
    if (empty_idx < 0) {
        return NULL;
    }
    __lvgl_demo.gamepad[empty_idx].dev = dev;
    return &__lvgl_demo.gamepad[empty_idx];
}

static void __lvgl_input_on_gamepad(vk_input_type_t type, vk_gamepad_evt_t *gamepad_evt)
{
    lvgl_demo_gamepad_t *gamepad = __lvgl_demo_get_gamepad(gamepad_evt->dev);
    ASSERT(gamepad != NULL);

    lv_coord_t radius = gamepad->margin + (gamepad->margin >> 1);
    uint32_t mask = (1UL << gamepad_evt->bitlen) - 1;
    uint32_t cur = gamepad_evt->cur.valu32 & mask;
    uint32_t prev = gamepad_evt->pre.valu32 & mask;
    lv_coord_t margin = gamepad->margin;
    float offset;

    switch (gamepad_evt->id) {
    case GAMEPAD_ID_DUMMY:                                                      break;
    case GAMEPAD_ID_L_UP:       lv_btn_toggle(gamepad->pad_left.btn_up);        break;
    case GAMEPAD_ID_L_DOWN:     lv_btn_toggle(gamepad->pad_left.btn_down);      break;
    case GAMEPAD_ID_L_LEFT:     lv_btn_toggle(gamepad->pad_left.btn_left);      break;
    case GAMEPAD_ID_L_RIGHT:    lv_btn_toggle(gamepad->pad_left.btn_right);     break;
    case GAMEPAD_ID_R_UP:       lv_btn_toggle(gamepad->pad_right.btn_up);       break;
    case GAMEPAD_ID_R_DOWN:     lv_btn_toggle(gamepad->pad_right.btn_down);     break;
    case GAMEPAD_ID_R_LEFT:     lv_btn_toggle(gamepad->pad_right.btn_left);     break;
    case GAMEPAD_ID_R_RIGHT:    lv_btn_toggle(gamepad->pad_right.btn_right);    break;
    case GAMEPAD_ID_LB:         lv_btn_toggle(gamepad->btn_lb);                 break;
    case GAMEPAD_ID_RB:         lv_btn_toggle(gamepad->btn_rb);                 break;
    case GAMEPAD_ID_LS:         lv_btn_toggle(gamepad->joystick_left.btn);      break;
    case GAMEPAD_ID_RS:         lv_btn_toggle(gamepad->joystick_right.btn);     break;
    case GAMEPAD_ID_MENU_LEFT:  lv_btn_toggle(gamepad->btn_lm);                 break;
    case GAMEPAD_ID_MENU_RIGHT: lv_btn_toggle(gamepad->btn_rm);                 break;
    case GAMEPAD_ID_MENU_MAIN:  lv_btn_toggle(gamepad->btn_mm);                 break;
    case GAMEPAD_ID_SPECIAL:                                                    break;
    case GAMEPAD_ID_LX:
        offset = __lvgl_demo_calc_joystick_offset(gamepad_evt);
        lv_obj_set_x(   gamepad->joystick_left.btn,
                        1 * margin + (margin >> 1) + (int32_t)((float)radius * offset));
        break;
    case GAMEPAD_ID_LY:
        offset = __lvgl_demo_calc_joystick_offset(gamepad_evt);
        lv_obj_set_y(   gamepad->joystick_left.btn,
                        1 * margin + (margin >> 1) - (int32_t)((float)radius * offset));
        break;
    case GAMEPAD_ID_RX:
        offset = __lvgl_demo_calc_joystick_offset(gamepad_evt);
        lv_obj_set_x(   gamepad->joystick_right.btn,
                        1 * margin + (margin >> 1) + (int32_t)((float)radius * offset));
        break;
    case GAMEPAD_ID_RY:
        offset = __lvgl_demo_calc_joystick_offset(gamepad_evt);
        lv_obj_set_y(   gamepad->joystick_right.btn,
                        1 * margin + (margin >> 1) - (int32_t)((float)radius * offset));
        break;
    case GAMEPAD_ID_LT:
        itoa(cur, gamepad->lbl_lt_text, 10);
        lv_label_set_text(gamepad->lbl_lt, gamepad->lbl_lt_text);
        break;
    case GAMEPAD_ID_RT:
        itoa(cur, gamepad->lbl_rt_text, 10);
        lv_label_set_text(gamepad->lbl_rt, gamepad->lbl_rt_text);
        break;
    case GAMEPAD_ID_DPAD: {
            uint8_t dpad_diff = __hid_dpad_to_mask[prev] ^ __hid_dpad_to_mask[cur];

            if (dpad_diff & 1) {
                lv_btn_toggle(gamepad->pad_left.btn_up);
            }
            if (dpad_diff & 2) {
                lv_btn_toggle(gamepad->pad_left.btn_down);
            }
            if (dpad_diff & 4) {
                lv_btn_toggle(gamepad->pad_left.btn_left);
            }
            if (dpad_diff & 8) {
                lv_btn_toggle(gamepad->pad_left.btn_right);
            }
        }
        break;
    }
}

static bool lv_btn_is_down(lv_obj_t *obj)
{
    lv_btn_state_t state = lv_btn_get_state(obj);
    return state == LV_BTN_STATE_TGL_REL;
}

static void lvgl_demo_event_cb(lv_obj_t *obj, lv_event_t event)
{
    lvgl_demo_gamepad_t *gamepad = obj->user_data;
    ASSERT(gamepad != NULL);

    if (event != LV_EVENT_CLICKED) {
        return;
    }

    // TODO: add event handler
    if (obj == gamepad->pad_left.btn_up) {
        if (lv_btn_is_down(obj)) {
            vsf_trace(VSF_TRACE_DEBUG, "btn_lu down\r\n");
        } else {
            vsf_trace(VSF_TRACE_DEBUG, "btn_lu up\r\n");
        }
    } else if (obj == gamepad->pad_left.btn_down) {
    } else if (obj == gamepad->pad_left.btn_left) {
    } else if (obj == gamepad->pad_left.btn_right) {
    } else if (obj == gamepad->pad_right.btn_up) {
    } else if (obj == gamepad->pad_right.btn_down) {
    } else if (obj == gamepad->pad_right.btn_left) {
    } else if (obj == gamepad->pad_right.btn_right) {
    } else if (obj == gamepad->btn_lb) {
    } else if (obj == gamepad->btn_rb) {
    } else if (obj == gamepad->btn_lt) {
    } else if (obj == gamepad->btn_rt) {
    } else if (obj == gamepad->btn_lm) {
    } else if (obj == gamepad->btn_rm) {
    } else if (obj == gamepad->btn_mm) {
    } else if (obj == gamepad->joystick_left.btn) {
    } else if (obj == gamepad->joystick_right.btn) {
    }
}

static lv_obj_t * lvgl_demo_create_btn(lv_obj_t *cont, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t *obj = lv_btn_create(cont, NULL);
    if (obj != NULL) {
        lv_obj_set_pos(obj, x, y);
        lv_obj_set_size(obj, w, h);
        lv_btn_set_toggle(obj, true);
        lv_obj_set_event_cb(obj, lvgl_demo_event_cb);
        obj->user_data = cont->user_data;
    }
    return obj;
}

static void lvgl_demo_create_pad(lvgl_demo_pad_t *pad, lv_obj_t *cont, lv_coord_t x, lv_coord_t y, lv_coord_t margin)
{
    pad->cont = lv_cont_create(cont, NULL);
    if (pad->cont != NULL) {
        lv_obj_set_pos(pad->cont, x, y);
        lv_obj_set_size(pad->cont, 6 * margin, 6 * margin);
        pad->cont->user_data = cont->user_data;

        pad->btn_up = lvgl_demo_create_btn(pad->cont,
                        2 * margin, margin >> 1,
                        2 * margin, 2 * margin);

        pad->btn_down = lvgl_demo_create_btn(pad->cont,
                        2 * margin, 4 * margin - (margin >> 1),
                        2 * margin, 2 * margin);

        pad->btn_left = lvgl_demo_create_btn(pad->cont,
                        margin >> 1, 2 * margin,
                        2 * margin, 2 * margin);

        pad->btn_right = lvgl_demo_create_btn(pad->cont,
                        4 * margin - (margin >> 1), 2 * margin,
                        2 * margin, 2 * margin);
    }
}

static void lvgl_demo_create_joystick(lvgl_demo_joystick_t *joystick, lv_obj_t *cont, lv_coord_t x, lv_coord_t y, lv_coord_t margin)
{
    joystick->cont = lv_cont_create(cont, NULL);
    if (joystick->cont != NULL) {
        lv_obj_set_pos(joystick->cont, x, y);
        lv_obj_set_size(joystick->cont, 6 * margin, 6 * margin);
        joystick->cont->user_data = cont->user_data;

        joystick->btn = lvgl_demo_create_btn(joystick->cont,
                        1 * margin + (margin >> 1), 1 * margin + (margin >> 1),
                        3 * margin, 3 * margin);
    }
}

static void lvgl_demo_create_gamepad(lvgl_demo_gamepad_t *gamepad, lv_obj_t *cont, lv_coord_t margin)
{
    memset(gamepad, 0, sizeof(*gamepad));
    gamepad->margin = margin;
    cont->user_data = gamepad;

    lv_obj_set_size(cont, margin * 30, margin * 15);
    lvgl_demo_create_pad(&gamepad->pad_left, cont, 1 * margin, 1 * margin, margin);

    gamepad->btn_lt = lvgl_demo_create_btn(cont,
                        8 * margin, 1 * margin,
                        6 * margin, 2 * margin);
    if (gamepad->btn_lt != NULL) {
        gamepad->lbl_lt = lv_label_create(gamepad->btn_lt, NULL);
        itoa(0, gamepad->lbl_lt_text, 10);
        lv_label_set_text(gamepad->lbl_lt, gamepad->lbl_lt_text);
    }

    gamepad->btn_rt = lvgl_demo_create_btn(cont,
                        16 * margin, 1 * margin,
                        6 * margin, 2 * margin);
    if (gamepad->btn_rt != NULL) {
        gamepad->lbl_rt = lv_label_create(gamepad->btn_rt, NULL);
        itoa(0, gamepad->lbl_rt_text, 10);
        lv_label_set_text(gamepad->lbl_rt, gamepad->lbl_rt_text);
    }

    gamepad->btn_lb = lvgl_demo_create_btn(cont,
                        8 * margin, 4 * margin,
                        6 * margin, 2 * margin);

    gamepad->btn_rb = lvgl_demo_create_btn(cont,
                        16 * margin, 4 * margin,
                        6 * margin, 2 * margin);

    lvgl_demo_create_pad(&gamepad->pad_right, cont, 23 * margin, 1 * margin, margin);

    lvgl_demo_create_joystick(&gamepad->joystick_left, cont, 4 * margin, 8 * margin, margin);

    gamepad->btn_lm = lvgl_demo_create_btn(cont,
                        11 * margin, 8 * margin,
                        2 * margin, 2 * margin);

    gamepad->btn_mm = lvgl_demo_create_btn(cont,
                        14 * margin, 8 * margin,
                        2 * margin, 2 * margin);

    gamepad->btn_rm = lvgl_demo_create_btn(cont,
                        17 * margin, 8 * margin,
                        2 * margin, 2 * margin);

    lvgl_demo_create_joystick(&gamepad->joystick_right, cont, 20 * margin, 8 * margin, margin);
}

void lvgl_application(uint_fast8_t gamepad_num)
{
    lv_obj_t *obj, *screen;
    lv_coord_t margin_x, margin_y, margin, top_margin;

    ASSERT((gamepad_num > 0) && (gamepad_num <= dimof(__lvgl_demo.gamepad)));

    screen = lv_scr_act();
    margin_x = lv_obj_get_width(screen) / 30;
    margin_y = lv_obj_get_height(screen) / (16 * gamepad_num);
    margin = min(margin_x, margin_y);
    top_margin = (lv_obj_get_height(screen) - 16 * gamepad_num * margin) / 2;

    for (uint_fast8_t i = 0; i < gamepad_num; i++) {
        obj = lv_cont_create(screen, NULL);
        // gamepad size is 15 X 30
        lv_obj_set_pos(obj, (lv_obj_get_width(screen) - 30 * margin) / 2, top_margin);
        top_margin += 16 * margin;
        __lvgl_demo.cont[i] = obj;
        lvgl_demo_create_gamepad(&__lvgl_demo.gamepad[i], __lvgl_demo.cont[i], margin);
    }

    vk_input_notifier_register(&__lvgl_demo.notifier);
}

#endif
