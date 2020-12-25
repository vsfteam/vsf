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
#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
#define __VSF_TGUI_CLASS_IMPLEMENT

#include "vsf.h"
#include <stdio.h>

#if (VSF_USE_TINY_GUI == ENABLED) && (APP_USE_TGUI_DESIGNER_DEMO == ENABLED)
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

const vsf_tgui_root_container_t* vk_tgui_get_root_container(vsf_tgui_t* gui_ptr)
{
    VSF_TGUI_ASSERT(NULL != gui_ptr);
    return gui_ptr->consumer.param.root_node_ptr;
}

const char* vsf_tgui_color_to_string(vsf_tgui_sv_color_t color)
{
    static struct {
        vsf_tgui_sv_color_t color;
        const char* color_name;
    } __colors_name[] = {
        { VSF_TGUI_COLOR_WHITE,   "VSF_TGUI_COLOR_WHITE"},
        { VSF_TGUI_COLOR_SILVER,  "VSF_TGUI_COLOR_SILVER"},
        { VSF_TGUI_COLOR_GRAY,    "VSF_TGUI_COLOR_GRAY"},
        { VSF_TGUI_COLOR_BLACK,   "VSF_TGUI_COLOR_BLACK"},
        { VSF_TGUI_COLOR_RED,     "VSF_TGUI_COLOR_RED"},
        { VSF_TGUI_COLOR_MAROON,  "VSF_TGUI_COLOR_MAROON"},
        { VSF_TGUI_COLOR_YELLOW,  "VSF_TGUI_COLOR_YELLOW"},
        { VSF_TGUI_COLOR_OLIVE,   "VSF_TGUI_COLOR_OLIVE"},
        { VSF_TGUI_COLOR_LIME,    "VSF_TGUI_COLOR_LIME"},
        { VSF_TGUI_COLOR_GREEN,   "VSF_TGUI_COLOR_GREEN"},
        { VSF_TGUI_COLOR_AQUA,    "VSF_TGUI_COLOR_AQUA"},
        { VSF_TGUI_COLOR_TEAL,    "VSF_TGUI_COLOR_TEAL"},
        { VSF_TGUI_COLOR_BLUE,    "VSF_TGUI_COLOR_BLUE"},
        { VSF_TGUI_COLOR_NAVY,    "VSF_TGUI_COLOR_NAVY"},
        { VSF_TGUI_COLOR_FUCHSIA, "VSF_TGUI_COLOR_FUCHSIA"},
        { VSF_TGUI_COLOR_PURPLE,  "VSF_TGUI_COLOR_PURPLE"},
    };
    static char __color_buffer[128];

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    for (int i = 0; i < dimof(__colors_name); i++) {
        if (color.tColor.Value == __colors_name[i].color.tColor.Value) {
            return __colors_name[i].color_name;
        }
    }

    if (color.tColor.tChannel.chA != 0xFF) {
        sprintf(__color_buffer, "VSF_TGUI_COLOR_RGBA(0x%02X, 0x%02X, 0x%02X, 0x%02X)",
            color.tColor.tChannel.chR, color.tColor.tChannel.chG, color.tColor.tChannel.chB, color.tColor.tChannel.chA);
    } else {
        sprintf(__color_buffer, "VSF_TGUI_COLOR_RGB(0x%02X, 0x%02X, 0x%02X)",
            color.tColor.tChannel.chR, color.tColor.tChannel.chG, color.tColor.tChannel.chB);
    }
    return __color_buffer;
#else
#error "TODO"
#endif
}

const char* vsf_tgui_container_type_to_string(vsf_tgui_container_type_t type)
{
    static const char * __names[] = {
        [VSF_TGUI_CONTAINER_TYPE_PLANE] = "VSF_TGUI_CONTAINER_TYPE_PLANE",
        [VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL] = "VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL",
        [VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL] = "VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL",
        [VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL] = "VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL",
        [VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL] = "VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL",
    };

    VSF_TGUI_ASSERT(VSF_TGUI_CONTAINER_TYPE_PLANE <= type && type <= VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL);

    return __names[type];
}

const char* vsf_tgui_align_mode_to_string(vsf_tgui_align_mode_t mode)
{
    if (mode == VSF_TGUI_ALIGN_CENTER) {
        return "VSF_TGUI_ALIGN_CENTER";
    } else if (mode == VSF_TGUI_ALIGN_LEFT) {
        return "VSF_TGUI_ALIGN_LEFT";
    } else if (mode == VSF_TGUI_ALIGN_RIGHT) {
        return "VSF_TGUI_ALIGN_RIGHT";
    } else if (mode == VSF_TGUI_ALIGN_TOP) {
        return "VSF_TGUI_ALIGN_TOP";
    } else if (mode == VSF_TGUI_ALIGN_BOTTOM) {
        return "VSF_TGUI_ALIGN_BOTTOM";
    } else if (mode == (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT)) {
        return "VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT";
    } else if (mode == (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT)) {
        return "VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT";
    } else if (mode == (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT)) {
        return "VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT";
    } else if (mode == (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT)) {
        return "VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT";
    } else if (mode == VSF_TGUI_ALIGN_FILL) {
        return "VSF_TGUI_ALIGN_FILL";
    } else {
        VSF_TGUI_ASSERT(0);
        return "";
    }
}

const char* vsf_tgui_list_work_mode_to_string(vsf_tgui_list_work_mode_t mode)
{
    if (mode == VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM) {
        return "VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM";
    } else if (mode == VSF_TGUI_LIST_MODE_FREE_MOVE) {
        return "VSF_TGUI_LIST_MODE_FREE_MOVE";
    } else if (mode == VSF_TGUI_LIST_MODE_ITEM_SELECTION) {
        return "VSF_TGUI_LIST_MODE_ITEM_SELECTION";
    } else if (mode == VSF_TGUI_LIST_MODE_ITEM_SELECTION_CENTER_ALIGN) {
        return "VSF_TGUI_LIST_MODE_ITEM_SELECTION_CENTER_ALIGN";
    } else {
        VSF_TGUI_ASSERT(0);
        return "";
    }
}

const char* vsf_tgui_msg_evt_to_string(vsf_evt_t msg)
{
    static const char* __msg_strings[] = {
        [VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH",
        [VSF_TGUI_MSG_CONTROL_EVT - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_MSG_CONTROL_EVT",
        [VSF_TGUI_EVT_ON_SET_TOP_CONTAINER - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_ON_SET_TOP_CONTAINER",
        [VSF_TGUI_EVT_ON_LOAD - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_ON_LOAD",
        [VSF_TGUI_EVT_ON_DEPOSE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_ON_DEPOSE",
        [VSF_TGUI_EVT_UPDATE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_UPDATE",
        [VSF_TGUI_EVT_UPDATE_TREE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_UPDATE_TREE",
        [VSF_TGUI_EVT_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_REFRESH",
        [VSF_TGUI_EVT_POST_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POST_REFRESH",
        [VSF_TGUI_EVT_GET_ACTIVE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GET_ACTIVE",
        [VSF_TGUI_EVT_LOST_ACTIVE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_LOST_ACTIVE",
        [VSF_TGUI_EVT_ON_TIME - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_ON_TIME",
        [VSF_TGUI_EVT_POINTER_DOWN - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_DOWN",
        [VSF_TGUI_EVT_POINTER_HOLD - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_HOLD",
        [VSF_TGUI_EVT_POINTER_UP - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_UP",
        [VSF_TGUI_EVT_POINTER_CLICK - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_CLICK",
        [VSF_TGUI_EVT_POINTER_DOUBLE_CLICK - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_DOUBLE_CLICK",
        [VSF_TGUI_EVT_POINTER_ENTER - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_ENTER",
        [VSF_TGUI_EVT_POINTER_LEFT - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_LEFT",
        [VSF_TGUI_EVT_POINTER_HOVER - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_HOVER",
        [VSF_TGUI_EVT_POINTER_MOVE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_POINTER_MOVE",
        [VSF_TGUI_EVT_KEY_DOWN - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_DOWN",
        [VSF_TGUI_EVT_KEY_REPEATE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_REPEATE",
        [VSF_TGUI_EVT_KEY_UP - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_UP",
        [VSF_TGUI_EVT_KEY_PRESSED - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_PRESSED",
        [VSF_TGUI_EVT_KEY_LONG_PRESSED - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_LONG_PRESSED",
        [VSF_TGUI_EVT_KEY_DOUBLE_CLICK - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_KEY_DOUBLE_CLICK",
        [VSF_TGUI_EVT_GESTURE_SLIDE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_SLIDE",
        [VSF_TGUI_EVT_GESTURE_WHEEL - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_WHEEL",
        [VSF_TGUI_EVT_GESTURE_ZOOM_IN - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_ZOOM_IN",
        [VSF_TGUI_EVT_GESTURE_ZOOM_OUT - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_ZOOM_OUT",
        [VSF_TGUI_EVT_GESTURE_ROTATE_CLOCKWISE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_ROTATE_CLOCKWISE",
        [VSF_TGUI_EVT_GESTURE_ROTATE_ANTICLOCKWISE - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_GESTURE_ROTATE_ANTICLOCKWISE",
        [VSF_TGUI_EVT_LIST_SELECTION_CHANGED - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_LIST_SELECTION_CHANGED",
        [VSF_TGUI_EVT_LIST_SLIDING_STARTED - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_LIST_SLIDING_STARTED",
        [VSF_TGUI_EVT_LIST_SLIDING_STOPPED - VSF_TGUI_MSG_AVAILABLE] = "VSF_TGUI_EVT_LIST_SLIDING_STOPPED",
    };

    VSF_TGUI_ASSERT(msg >= VSF_TGUI_MSG_AVAILABLE);
    VSF_TGUI_ASSERT(msg - VSF_TGUI_MSG_AVAILABLE < dimof(__msg_strings));
    return __msg_strings[msg - VSF_TGUI_MSG_AVAILABLE];
}

const char* vsf_tgui_msg_evt_to_short_name(vsf_evt_t msg)
{
    static const char* __msg_strings[] = {
        [VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "low_level_ready_to_refresh",
        [VSF_TGUI_MSG_CONTROL_EVT - VSF_TGUI_MSG_AVAILABLE] = "control_evt",
        [VSF_TGUI_EVT_ON_SET_TOP_CONTAINER - VSF_TGUI_MSG_AVAILABLE] = "on_set_top_container",
        [VSF_TGUI_EVT_ON_LOAD - VSF_TGUI_MSG_AVAILABLE] = "on_load",
        [VSF_TGUI_EVT_ON_DEPOSE - VSF_TGUI_MSG_AVAILABLE] = "on_depose",
        [VSF_TGUI_EVT_UPDATE - VSF_TGUI_MSG_AVAILABLE] = "update",
        [VSF_TGUI_EVT_UPDATE_TREE - VSF_TGUI_MSG_AVAILABLE] = "update_tree",
        [VSF_TGUI_EVT_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "refresh",
        [VSF_TGUI_EVT_POST_REFRESH - VSF_TGUI_MSG_AVAILABLE] = "post_refresh",
        [VSF_TGUI_EVT_GET_ACTIVE - VSF_TGUI_MSG_AVAILABLE] = "get_active",
        [VSF_TGUI_EVT_LOST_ACTIVE - VSF_TGUI_MSG_AVAILABLE] = "lost_active",
        [VSF_TGUI_EVT_ON_TIME - VSF_TGUI_MSG_AVAILABLE] = "on_time",
        [VSF_TGUI_EVT_POINTER_DOWN - VSF_TGUI_MSG_AVAILABLE] = "pointer_down",
        [VSF_TGUI_EVT_POINTER_HOLD - VSF_TGUI_MSG_AVAILABLE] = "pointer_hold",
        [VSF_TGUI_EVT_POINTER_UP - VSF_TGUI_MSG_AVAILABLE] = "pointer_up",
        [VSF_TGUI_EVT_POINTER_CLICK - VSF_TGUI_MSG_AVAILABLE] = "pointer_click",
        [VSF_TGUI_EVT_POINTER_DOUBLE_CLICK - VSF_TGUI_MSG_AVAILABLE] = "pointer_double_click",
        [VSF_TGUI_EVT_POINTER_ENTER - VSF_TGUI_MSG_AVAILABLE] = "pointer_enter",
        [VSF_TGUI_EVT_POINTER_LEFT - VSF_TGUI_MSG_AVAILABLE] = "pointer_left",
        [VSF_TGUI_EVT_POINTER_HOVER - VSF_TGUI_MSG_AVAILABLE] = "pointer_hover",
        [VSF_TGUI_EVT_POINTER_MOVE - VSF_TGUI_MSG_AVAILABLE] = "pointer_move",
        [VSF_TGUI_EVT_KEY_DOWN - VSF_TGUI_MSG_AVAILABLE] = "key_down",
        [VSF_TGUI_EVT_KEY_REPEATE - VSF_TGUI_MSG_AVAILABLE] = "key_repeate",
        [VSF_TGUI_EVT_KEY_UP - VSF_TGUI_MSG_AVAILABLE] = "key_up",
        [VSF_TGUI_EVT_KEY_PRESSED - VSF_TGUI_MSG_AVAILABLE] = "key_pressed",
        [VSF_TGUI_EVT_KEY_LONG_PRESSED - VSF_TGUI_MSG_AVAILABLE] = "key_long_pressed",
        [VSF_TGUI_EVT_KEY_DOUBLE_CLICK - VSF_TGUI_MSG_AVAILABLE] = "key_double_click",
        [VSF_TGUI_EVT_GESTURE_SLIDE - VSF_TGUI_MSG_AVAILABLE] = "gesture_slide",
        [VSF_TGUI_EVT_GESTURE_WHEEL - VSF_TGUI_MSG_AVAILABLE] = "gesture_wheel",
        [VSF_TGUI_EVT_GESTURE_ZOOM_IN - VSF_TGUI_MSG_AVAILABLE] = "gesture_zoom_in",
        [VSF_TGUI_EVT_GESTURE_ZOOM_OUT - VSF_TGUI_MSG_AVAILABLE] = "gesture_zoom_out",
        [VSF_TGUI_EVT_GESTURE_ROTATE_CLOCKWISE - VSF_TGUI_MSG_AVAILABLE] = "gesture_rotate_clockwise",
        [VSF_TGUI_EVT_GESTURE_ROTATE_ANTICLOCKWISE - VSF_TGUI_MSG_AVAILABLE] = "gesture_rotate_anticlockwise",
        [VSF_TGUI_EVT_LIST_SELECTION_CHANGED - VSF_TGUI_MSG_AVAILABLE] = "list_selection_changed",
        [VSF_TGUI_EVT_LIST_SLIDING_STARTED - VSF_TGUI_MSG_AVAILABLE] = "list_sliding_started",
        [VSF_TGUI_EVT_LIST_SLIDING_STOPPED - VSF_TGUI_MSG_AVAILABLE] = "list_sliding_stopped",
    };

    VSF_TGUI_ASSERT(msg >= VSF_TGUI_MSG_AVAILABLE);
    VSF_TGUI_ASSERT(msg - VSF_TGUI_MSG_AVAILABLE < dimof(__msg_strings));
    return __msg_strings[msg - VSF_TGUI_MSG_AVAILABLE];
}

const char* vsf_tgui_evt_mask_to_string(uint32_t mask)
{
    static char __mask_buffer[sizeof(", 0x00000000")];
    if (mask == VSF_TGUI_MSG_MSK) {
        return ", VSF_TGUI_MSG_MSK";
    } else if (mask == VSF_TGUI_EVT_MSK) {
        return ", VSF_TGUI_EVT_MSK";
    } else if (mask == 0x3FF) {
        return "";
    } else {
        snprintf(__mask_buffer, sizeof(__mask_buffer), ", 0x%08X", mask);
        return __mask_buffer;
    }
}

const char* vsf_tgui_control_get_var_name(vsf_tgui_control_t* control)
{
    static char __name_buffer[128];
    vsf_msgt_node_t* node = &control->use_as__vsf_msgt_node_t;
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    const char* node_name_ptr = node->node_name_ptr + 1; // skip first [
    node_name_ptr = strchr(node_name_ptr, '['); // search second [
    ASSERT(node_name_ptr != NULL);
    node_name_ptr++; // skip second [

    if (*node_name_ptr == '*') {
        node_name_ptr++;
    }

    char* node_name_end_ptr;
    node_name_end_ptr = strchr(node_name_ptr, ']');
    ASSERT(node_name_end_ptr != NULL);
    int len = node_name_end_ptr - node_name_ptr;
    memcpy(__name_buffer, node_name_ptr, len);
    __name_buffer[len] = '\0';
    return __name_buffer;
#else
#error "TODO"
#endif
}


const char* vsf_tgui_font_get_var_name(uint8_t font_index)
{
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    const vsf_tgui_font_t* font = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font != NULL);
    VSF_TGUI_ASSERT(font->name_ptr != NULL);
    return font->name_ptr;
#else
#error "TODO"
#endif
}

const char* vsf_tgui_tile_get_var_name(const vsf_tgui_tile_t* tile)
{
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    VSF_TGUI_ASSERT(tile->_.tCore.name_ptr != NULL);
    return tile->_.tCore.name_ptr;
#else
#error "TODO"
#endif
}


static bool __is_char_printable(char ch)
{
    if (0x20 <= ch && ch <= 0x7E) {
        return true;
    }
    else {
        return false;
    }
}

void vsf_tgui_string_to_c_lang_format(char* dist, const char* src)
{
    VSF_TGUI_ASSERT(src != NULL);

    // escape sequence
    struct {
        char ch;
        char escape_ch;
    } static const __ch_list[] = {
        {0x07, 'a'},  // Bell
        {0x08, 'b'},  // Backspace
        {0x09, 't'},  // Horizontal Tab
        {0x0A, 'n'},  // Line Feed
        {0x0B, 'v'},  // Vertical Tab
        {0x0C, 'f'},  // Form Feed
        {0x0D, 'r'},  // Carriage Return
        {0x1B, 'e'},  // Escape
        {0x22, '\''}, //
        {0x27, '\"'}, //
        {0x5C, '\\'}, //
    };

    // TODO: support utf8
    while (*src != '\0') {
        int i = 0;
        for (; i < dimof(__ch_list); i++) {
            if (*src == __ch_list[i].ch) {
                char ch_buf[3] = { '\\' };
                ch_buf[1] = __ch_list[i].escape_ch;
                strcat(dist, ch_buf);
                src++;
                dist += 2;
                break;
            }
        }

        if (i >= dimof(__ch_list)) {
            if (__is_char_printable(*src)) {
                *dist++ = *src++;
            } else {
                char buf[sizeof("\x00")];
                snprintf(buf, sizeof(buf), "\\x%02x", *src++);
                strcat(dist, buf);
                dist += sizeof(buf) - 1;
            }
        }
    }
}


void tgui_designer_gen_control_name(char *buffer, size_t buffer_size, vsf_tree_msg_node_id_t id)
{
    static const char* __control_names[] = {
        [VSF_TGUI_COMPONENT_ID_CONTROL] = "control",
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = "container",
        [VSF_TGUI_COMPONENT_ID_LABEL] = "label",
        [VSF_TGUI_COMPONENT_ID_BUTTON] = "button",
        [VSF_TGUI_COMPONENT_ID_PANEL] = "panel",
        [VSF_TGUI_COMPONENT_ID_LIST] = "list",
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = "text_list",
    };
    static uint16_t __control_cnt[__VSF_TGUI_COMPONENT_TYPE_NUM];
    snprintf(buffer, buffer_size, "%s_%d", __control_names[id], __control_cnt[id]++);
}

bool tgui_designer_is_label(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);

    if ((control_ptr->id == VSF_TGUI_COMPONENT_ID_LABEL) || (control_ptr->id == VSF_TGUI_COMPONENT_ID_BUTTON)) {
        return true;
    } else {
        return false;
    }
}

bool tgui_designer_is_button(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);

    return (control_ptr->id == VSF_TGUI_COMPONENT_ID_BUTTON);
}

bool tgui_designer_is_container(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);

    static const vsf_tree_msg_node_id_t __container_ids[] = {
        VSF_TGUI_COMPONENT_ID_CONTAINER,
        VSF_TGUI_COMPONENT_ID_PANEL,
        VSF_TGUI_COMPONENT_ID_LIST,
        VSF_TGUI_COMPONENT_ID_TEXT_LIST,
    };

    for (int i = 0; i < dimof(__container_ids); i++) {
        if (control_ptr->id == __container_ids[i]) {
            return true;
        }
    }

    return false;
}

bool tgui_designer_is_text_list(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    return (control_ptr->id == VSF_TGUI_COMPONENT_ID_TEXT_LIST);
}

bool tgui_designer_is_list(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    return (control_ptr->id == VSF_TGUI_COMPONENT_ID_LIST);
}

bool tgui_designer_is_panel(vsf_tgui_control_t * control_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    return (control_ptr->id == VSF_TGUI_COMPONENT_ID_PANEL);
}
#endif


/* EOF */
