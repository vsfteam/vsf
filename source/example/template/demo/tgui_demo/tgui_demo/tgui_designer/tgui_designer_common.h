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

#ifndef __TGUI_DESIGNER_COMMON_H__
#define __TGUI_DESIGNER_COMMON_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/

#define TGUI_ALIGN_MODE_TEXT \
    "center\n" \
    "left\n" \
    "right\n" \
    "top\n" \
    "bottom\n" \
    "left top\n" \
    "right top\n" \
    "left bottom\n" \
    "right bottom"

#define TGUI_CONTAINER_TYPE_TEXT \
    "Plane\n" \
    "Stream Horizontal\n" \
    "Stream Verical\n" \
    "Line Stream Horizontal\n" \
    "Line Stream Verical"


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
extern const vsf_tgui_root_container_t* vk_tgui_get_root_container(vsf_tgui_t* gui_ptr);
extern const char* vsf_tgui_color_to_string(vsf_tgui_sv_color_t color);
extern const char* vsf_tgui_container_type_to_string(vsf_tgui_container_type_t type);
extern const char* vsf_tgui_align_mode_to_string(vsf_tgui_align_mode_t mode);
extern const char* vsf_tgui_list_work_mode_to_string(vsf_tgui_list_work_mode_t mode);
extern const char* vsf_tgui_msg_evt_to_string(vsf_evt_t msg);
const char* vsf_tgui_msg_evt_to_short_name(vsf_evt_t msg);
extern const char* vsf_tgui_evt_mask_to_string(uint32_t mask);

extern const char* vsf_tgui_control_get_var_name(vsf_tgui_control_t* control);
extern const char* vsf_tgui_font_get_var_name(uint8_t font_index);
extern const char* vsf_tgui_tile_get_var_name(const vsf_tgui_tile_t* tile);

extern void vsf_tgui_string_to_c_lang_format(char* dist, const char* src);
extern void tgui_designer_gen_control_name(char *buffer, size_t buffer_size, vsf_tree_msg_node_id_t id);
extern bool tgui_designer_is_label(vsf_tgui_control_t *control_ptr);
extern bool tgui_designer_is_button(vsf_tgui_control_t *control_ptr);
extern bool tgui_designer_is_container(vsf_tgui_control_t* control_ptr);
extern bool tgui_designer_is_text_list(vsf_tgui_control_t* control_ptr);
extern bool tgui_designer_is_list(vsf_tgui_control_t* control_ptr);
extern bool tgui_designer_is_panel(vsf_tgui_control_t* control_ptr);

#endif
#endif


/* EOF */
