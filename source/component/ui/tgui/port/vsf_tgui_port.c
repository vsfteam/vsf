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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED

#include "./vsf_tgui_port.h"

#if VSF_TGUI_CFG_FONT_USE_LVGL == ENABLED
#   ifdef VSF_TGUI_CFG_LVGL_HEADER
#       include VSF_TGUI_CFG_LVGL_HEADER
#   else
#       include <lvgl.h>
#   endif
#   ifdef VSF_TGUI_LVGL_FONT_DECLARE
VSF_TGUI_LVGL_FONT_DECLARE
#   endif
#endif

/*============================ MACROS ========================================*/

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   define __TGUI_FONT_NAME(__NAME)             .name_ptr = #__NAME,
#else
#   define __TGUI_FONT_NAME(__NAME)
#endif

#undef __TGUI_FONT_DEF
#define __TGUI_FONT_DEF(__NAME, __HEIGHT, ...)                                  \
    [__NAME] = {                                                                \
        __TGUI_FONT_NAME(__NAME)                                                \
        .height = (__HEIGHT),                                                   \
        __VA_ARGS__                                                             \
    }

#undef TGUI_FONT_DEF
#define TGUI_FONT_DEF(__NAME, __HEIGHT)                                         \
    __TGUI_FONT_DEF(__NAME, (__HEIGHT))

#if VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED
#   define __TGUI_FT2_FONT_INIT(__PATH, __SIZE)                                 \
    .type = VSF_TGUI_FONT_FT2,                                                  \
    .ft2.font_path_ptr = (__PATH),                                              \
    .ft2.font_size = (__SIZE),

#   undef TGUI_FT2_FONT_DEF
#   define TGUI_FT2_FONT_DEF(__NAME, __PATH, __SIZE)                            \
    __TGUI_FONT_DEF(__NAME, 0, __TGUI_FT2_FONT_INIT((__PATH), (__SIZE)))
#elif VSF_TGUI_CFG_FONT_USE_LVGL == ENABLED
#   define __TGUI_LVGL_FONT_INIT(__FONT, __SIZE)                                \
    .type = VSF_TGUI_FONT_LVGL,                                                 \
    .font = (__FONT),

#   undef TGUI_LVGL_FONT_DEF
#   define TGUI_LVGL_FONT_DEF(__NAME, __FONT, __SIZE)                           \
    __TGUI_FONT_DEF(__NAME, __SIZE, __TGUI_LVGL_FONT_INIT((__FONT), (__SIZE)))
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#ifdef VSF_TGUI_FONTS
#   if VSF_TGUI_CFG_FONT_USE_FREETYPE != ENABLED
const
#   endif
static vsf_tgui_font_t __tgui_fonts[] = {
    VSF_TGUI_FONTS
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_INPUT == ENABLED
static void __vsf_tgui_on_keyboard_evt(vsf_tgui_t *tgui_ptr, vk_keyboard_evt_t* evt)
{
    vsf_tgui_evt_t event = {
        .tKeyEvt = {
            .msg = vsf_input_keyboard_is_down(evt)
                                ? VSF_TGUI_EVT_KEY_DOWN
                                : VSF_TGUI_EVT_KEY_UP,
            .hwKeyValue = vsf_input_keyboard_get_keycode(evt),
        },
    };

    vk_tgui_send_message(tgui_ptr, event);

    if (!vsf_input_keyboard_is_down(evt)) {
        event.tKeyEvt.msg = VSF_TGUI_EVT_KEY_PRESSED;
        vk_tgui_send_message(tgui_ptr, event);
    }
}


static void __vsf_tgui_on_touchscreen_evt(vsf_tgui_t *tgui_ptr, vk_touchscreen_evt_t* ts_evt)
{
    vsf_err_t result =
        vsf_tgui_send_touch_evt(tgui_ptr,
                                0,  /* only one finger is used for now*/
                                (vsf_tgui_location_t) {
                                    vsf_input_touchscreen_get_x(ts_evt),
                                    vsf_input_touchscreen_get_y(ts_evt)},
                                vsf_input_touchscreen_is_down(ts_evt),
                                ts_evt->use_as__vk_input_evt_t.duration
                                );

    VSF_ASSERT(result == VSF_ERR_NONE);
}

#if (VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS == ENABLED) && defined(VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS)
static void __vsf_tgui_on_mouse_evt(vsf_tgui_t *tgui_ptr, vk_mouse_evt_t *mouse_evt)
{
    int_fast8_t button_id = vsf_input_mouse_evt_button_get(mouse_evt) == 0 ? 0 : 1;
    static uint_fast8_t __button_status = 0;
    vsf_err_t result = VSF_ERR_NONE;

    switch (vsf_input_mouse_evt_get(mouse_evt)) {
        case VSF_INPUT_MOUSE_EVT_BUTTON: {

            result =
                vsf_tgui_send_touch_evt(tgui_ptr,
                                        button_id,
                                        (vsf_tgui_location_t) {
                                            vsf_input_mouse_evt_get_x(mouse_evt),
                                            vsf_input_mouse_evt_get_y(mouse_evt)},
                                        vsf_input_mouse_evt_button_is_down(mouse_evt),
                                        mouse_evt->use_as__vk_input_evt_t.duration
                                        );

            if (vsf_input_mouse_evt_button_is_down(mouse_evt)) {
                __button_status |= VSF_BIT(button_id);
            } else {
                __button_status &= ~VSF_BIT(button_id);
            }


            VSF_ASSERT(result == VSF_ERR_NONE);
            break;
        }

        case VSF_INPUT_MOUSE_EVT_MOVE: {
            if (0 == __button_status) {
                //! pure mouse move
                result =
                    vsf_tgui_send_touch_evt(tgui_ptr,
                                            button_id,
                                            (vsf_tgui_location_t) {
                                                vsf_input_mouse_evt_get_x(mouse_evt),
                                                vsf_input_mouse_evt_get_y(mouse_evt)},
                                            __button_status & VSF_BIT(button_id),
                                            mouse_evt->use_as__vk_input_evt_t.duration
                                            );
            } else {
                if (__button_status & VSF_BIT(0)) {
                    //! simulate finger 0 move
                    result =
                    vsf_tgui_send_touch_evt(tgui_ptr,
                                            0,
                                            (vsf_tgui_location_t) {
                                                vsf_input_mouse_evt_get_x(mouse_evt),
                                                vsf_input_mouse_evt_get_y(mouse_evt)},
                                            __button_status & VSF_BIT(0),
                                            mouse_evt->use_as__vk_input_evt_t.duration
                                            );
                }
                if (__button_status & VSF_BIT(1)) {
                    //! simulate finger 1 move
                    result =
                    vsf_tgui_send_touch_evt(tgui_ptr,
                                            1,
                                            (vsf_tgui_location_t) {
                                                vsf_input_mouse_evt_get_x(mouse_evt),
                                                vsf_input_mouse_evt_get_y(mouse_evt)},
                                            __button_status & VSF_BIT(1),
                                            mouse_evt->use_as__vk_input_evt_t.duration
                                            );
                }
            }
            VSF_ASSERT(result == VSF_ERR_NONE);
            break;
        }

        case VSF_INPUT_MOUSE_EVT_WHEEL:  {
                vsf_tgui_evt_t event = {
                    .tGestureEvt = {
                        .msg = VSF_TGUI_EVT_GESTURE_WHEEL,
                        .delta = {
                            .iX = 0,
                            .iY = -vsf_input_mouse_evt_get_y(mouse_evt),
                            .ms = 20,            //! 50Hz
                        },
                    },
                };

                vsf_tgui_control_t *control_ptr = (vsf_tgui_control_t *)vsf_tgui_pointed_control_get(tgui_ptr);
                if (control_ptr != NULL) {
                    vsf_tgui_control_set_active(control_ptr);
                    vk_tgui_send_message(tgui_ptr, event);
                }
                break;
            }

        default:
            break;
    }
}
#endif

static void __vsf_tgui_on_input_evt(vk_input_notifier_t *notifier, vk_input_type_t type, vk_input_evt_t *evt)
{
    vsf_tgui_t *tgui_ptr = (vsf_tgui_t *)notifier->param;
    switch (type) {
    case VSF_INPUT_TYPE_KEYBOARD:
        __vsf_tgui_on_keyboard_evt(tgui_ptr, (vk_keyboard_evt_t *)evt);
        break;
    case VSF_INPUT_TYPE_TOUCHSCREEN:
        __vsf_tgui_on_touchscreen_evt(tgui_ptr, (vk_touchscreen_evt_t *)evt);
        break;
#if VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS == ENABLED
    case VSF_INPUT_TYPE_MOUSE:
        __vsf_tgui_on_mouse_evt(tgui_ptr, (vk_mouse_evt_t *)evt);
        break;
#endif
    }
}

void vsf_tgui_input_init(vsf_tgui_t *gui_ptr, vk_input_notifier_t *notifier)
{
    notifier->on_evt = __vsf_tgui_on_input_evt;
    notifier->param = gui_ptr;
    vk_input_notifier_register(notifier);
}
#endif

VSF_CAL_WEAK(vsf_tgui_font_get)
const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t font_index)
{
#ifdef VSF_TGUI_FONTS
    VSF_TGUI_ASSERT(font_index < dimof(__tgui_fonts));
    return &__tgui_fonts[font_index];
#else
    return NULL;
#endif
}

VSF_CAL_WEAK(vsf_tgui_font_number)
uint8_t vsf_tgui_font_number(void)
{
#ifdef VSF_TGUI_FONTS
    return dimof(__tgui_fonts);
#else
    return 0;
#endif
}

#endif
/* EOF */
