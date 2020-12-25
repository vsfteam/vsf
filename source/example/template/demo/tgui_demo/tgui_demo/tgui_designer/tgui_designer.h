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

#ifndef __TGUI_DESIGNER_H__
#define __TGUI_DESIGNER_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum tgui_designer_op_type_t {
    VSF_TGUI_CREATE_NEW_ITEM    = 1,

    VSF_TGUI_OP_INCLUDE_VALUE   = 0x8000,
    VSF_TGUI_UPDATE_LOCATION    = VSF_TGUI_OP_INCLUDE_VALUE,
    VSF_TGUI_UPDATE_SIZE,

    //VSF_TGUI_UPDATE_SIZE,
} tgui_designer_op_type_t;

typedef struct tgui_designer_op_t {
    vsf_tgui_control_t* control_ptr;
    tgui_designer_op_type_t op_type;
    struct {
        int32_t old_value;
        int32_t new_value;
    };
} tgui_designer_op_t;

typedef struct tgui_designer_ops_t {
    int current;
    tgui_designer_op_t items[1024];
} tgui_designer_ops_t ;

typedef struct tgui_designer_int_label_t {
    union {
        vsf_tgui_label_t label;
        vsf_tgui_label_t;
    };
    int32_t min_value;
    int32_t max_value;
    int32_t cur_value;

    struct {
        uint16_t offset;
        uint16_t item_size;
    };
    char buffer[sizeof("xxxxxx:4294967296")];
} tgui_designer_int_label_t;

typedef struct tgui_designer_log_label_t {
    union {
        vsf_tgui_label_t label;
        vsf_tgui_label_t;
    };
    uint8_t max_line;
    char buffer[1024 * 4];
} tgui_designer_log_label_t;

typedef struct tgui_designer_msg_button_t {
    union {
        vsf_tgui_button_t button;
        vsf_tgui_button_t;
    };
    vsf_evt_t desinger_msg;
} tgui_designer_msg_button_t;

typedef struct tgui_designer_msgmap_buttons {
    tgui_designer_msg_button_t low_level_ready_to_refresh;
    tgui_designer_msg_button_t control_evt;
    tgui_designer_msg_button_t on_set_top_container;
    tgui_designer_msg_button_t on_load;
    tgui_designer_msg_button_t on_depose;
    tgui_designer_msg_button_t update;
    tgui_designer_msg_button_t update_tree;
    tgui_designer_msg_button_t refresh;
    tgui_designer_msg_button_t post_refresh;
    tgui_designer_msg_button_t get_active;
    tgui_designer_msg_button_t lost_active;
    tgui_designer_msg_button_t on_time;
    tgui_designer_msg_button_t pointer_down;
    tgui_designer_msg_button_t pointer_hold;
    tgui_designer_msg_button_t pointer_up;
    tgui_designer_msg_button_t pointer_click;
    tgui_designer_msg_button_t pointer_double_click;
    tgui_designer_msg_button_t pointer_enter;
    tgui_designer_msg_button_t pointer_left;
    tgui_designer_msg_button_t pointer_hover;
    tgui_designer_msg_button_t pointer_move;
    tgui_designer_msg_button_t key_down;
    tgui_designer_msg_button_t key_repeate;
    tgui_designer_msg_button_t key_up;
    tgui_designer_msg_button_t key_pressed;
    tgui_designer_msg_button_t key_long_pressed;
    tgui_designer_msg_button_t key_double_click;
    tgui_designer_msg_button_t gesture_slide;
    tgui_designer_msg_button_t gesture_wheel;
    tgui_designer_msg_button_t gesture_zoom_in;
    tgui_designer_msg_button_t gesture_zoom_out;
    tgui_designer_msg_button_t gesture_rotate_clockwise;
    tgui_designer_msg_button_t gesture_rotate_anticlockwise;
    tgui_designer_msg_button_t list_selection_changed;
    tgui_designer_msg_button_t list_sliding_started;
    tgui_designer_msg_button_t list_sliding_stopped;
} tgui_designer_msgmap_buttons;

declare_tgui_panel(tgui_designer_t)

def_tgui_panel(tgui_designer_t,
    tgui_contains(
        //use_tgui_panel(design_panel)

        use_tgui_panel(tools,
            tgui_contains(
                use_tgui_container(ctrl_attrs,
                    vsf_tgui_text_list_t component_ids;

                    use_tgui_container(tile_container,
                        vsf_tgui_text_list_t tiles;
                        vsf_tgui_text_list_t tile_align;
                    )

                    use_tgui_container(colors,
                        tgui_designer_int_label_t red;
                        tgui_designer_int_label_t green;
                        tgui_designer_int_label_t blue;
                        tgui_designer_int_label_t alpha;
                    )

                    use_tgui_container(regions,
                        tgui_designer_int_label_t x;
                        tgui_designer_int_label_t y;
                        tgui_designer_int_label_t width;
                        tgui_designer_int_label_t height;
                    )

                    use_tgui_container(margins,
                        tgui_designer_int_label_t left;
                        tgui_designer_int_label_t top;
                        tgui_designer_int_label_t right;
                        tgui_designer_int_label_t bottom;
                    )

                    use_tgui_container(msgmaps,
                        union {
                            tgui_designer_msgmap_buttons btns;
                            tgui_designer_msg_button_t btn_array[sizeof(tgui_designer_msgmap_buttons) / sizeof(vsf_tgui_button_t)];
                        };
                    )
                )

                use_tgui_container(label_attrs,
                    vsf_tgui_text_list_t font;
                    vsf_tgui_text_list_t label_align;

                    use_tgui_container(font_colors,
                        tgui_designer_int_label_t red;
                        tgui_designer_int_label_t green;
                        tgui_designer_int_label_t blue;
                        tgui_designer_int_label_t alpha;
                    )

                    vsf_tgui_label_t content;
                    tgui_designer_int_label_t line_height;
                    vsf_tgui_button_t raw_view;
                    vsf_tgui_button_t auto_size;
                )

                use_tgui_container(btn_attrs,
                    vsf_tgui_button_t check_btn;
                )

                use_tgui_container(cont_attrs,
                    vsf_tgui_button_t auto_size;
                    vsf_tgui_button_t show_corner;
                    vsf_tgui_button_t refresh_whole;
                    vsf_tgui_text_list_t type;

                    use_tgui_container(padding,
                        tgui_designer_int_label_t left;
                        tgui_designer_int_label_t top;
                        tgui_designer_int_label_t right;
                        tgui_designer_int_label_t bottom;
                    )
                )

                use_tgui_container(list_attrs)
                use_tgui_container(text_list_attrs)
                use_tgui_container(panel_attrs)

            )
        )

        vsf_tgui_button_t code_gen;
        tgui_designer_log_label_t log;
    ))

    vsf_tgui_control_t* cur_ctrl_ptr;
    vsf_tgui_control_t* design_panel_ptr;

    tgui_designer_ops_t ops;

end_def_tgui_panel(tgui_designer_t)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

extern
tgui_designer_t* tgui_designer_init(tgui_designer_t* ptPanel, vsf_tgui_t *gui_ptr);

#endif
#endif


/* EOF */
