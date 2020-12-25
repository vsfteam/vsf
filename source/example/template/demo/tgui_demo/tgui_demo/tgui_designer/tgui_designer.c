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
#include <stdio.h>

#if (VSF_USE_TINY_GUI == ENABLED) && (APP_USE_TGUI_DESIGNER_DEMO == ENABLED)

#include "./tgui_designer_common.h"
#include "./tgui_designer.h"
#include "./tgui_control_tree_dump.h"
#include "./tgui_control_create.h"
#include "../images/demo_images.h"
#include "../tgui_custom.h"

/*============================ MACROS ========================================*/
#ifndef APP_TGUI_DESIGNER_WHELL_HALF_UNIT
#   define APP_TGUI_DESIGNER_WHELL_HALF_UNIT        2
#endif

#ifndef APP_TGUI_DESIGNER_PADDING_WIDTH
#   define APP_TGUI_DESIGNER_PADDING_WIDTH          1
#endif

#ifndef APP_TGUI_DESIGNER_DUMP_ALL_COMPONMENT
#   define APP_TGUI_DESIGNER_DUMP_ALL_COMPONMENT    DISABLED
#endif

#ifndef APP_TGUI_DESIGNER_DUMP_ALL_ATTRIBUTES
#   define APP_TGUI_DESIGNER_DUMP_ALL_ATTRIBUTES    DISABLED
#endif

#if APP_TGUI_DESIGNER_DUMP_ALL_COMPONMENT == ENABLED
#   define APP_TGUI_PANEL       &__designer_ptr->use_as__vsf_tgui_panel_t
#else
#   define APP_TGUI_PANEL       __designer_ptr->design_panel_ptr
#endif

#if APP_TGUI_DESIGNER_DUMP_ALL_ATTRIBUTES == ENABLED
#   define APP_TGUI_ALL_ATTRS   true
#else
#   define APP_TGUI_ALL_ATTRS   false
#endif

#ifndef size_of_struct_member
#   define size_of_struct_member(s, m)          (sizeof(((s *)0)->m))
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/*============================ PROTOTYPES ====================================*/
static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);

static fsm_rt_t __on_text_list_post_refresh(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_text_list_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);

static fsm_rt_t __on_design_pointer_down(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_gesture_slide(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_gesture_wheel(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_pointer_up(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_pointer_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_panel_post_refresh(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_design_default_msg_handler(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);

static fsm_rt_t __on_tool_selection_changed(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_tool_button_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
static fsm_rt_t __on_tool_lable_gesture_wheel(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_tgui_align_mode_t __align_modes[] = {
    VSF_TGUI_ALIGN_CENTER,
    VSF_TGUI_ALIGN_LEFT,
    VSF_TGUI_ALIGN_RIGHT,
    VSF_TGUI_ALIGN_TOP,
    VSF_TGUI_ALIGN_BOTTOM,
    VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP,
    VSF_TGUI_ALIGN_RIGHT | VSF_TGUI_ALIGN_TOP,
    VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_BOTTOM,
    VSF_TGUI_ALIGN_RIGHT | VSF_TGUI_ALIGN_BOTTOM,
    //VSF_TGUI_ALIGN_FILL, // fill is todo
};

static const vsf_tgui_container_type_t __container_types[] = {
    VSF_TGUI_CONTAINER_TYPE_PLANE,
    VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL,
    VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL,
    VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL,
    VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL,
};

static tgui_designer_t * __designer_ptr;

static describe_tgui_msgmap(__panel_msg,
    tgui_msg_handler(VSF_TGUI_EVT_ON_LOAD, __on_top_panel_load),
);

static describe_tgui_msgmap(__on_design_msg,
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_DOWN, __on_design_pointer_down),
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_UP, __on_design_pointer_up),
    tgui_msg_handler(VSF_TGUI_EVT_GESTURE_SLIDE, __on_design_gesture_slide),
    tgui_msg_handler(VSF_TGUI_EVT_GESTURE_WHEEL, __on_design_gesture_wheel),
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK, __on_design_pointer_click),
    tgui_msg_handler(VSF_TGUI_EVT_POST_REFRESH, __on_design_panel_post_refresh),
);

static describe_tgui_msgmap(__on_tool_text_list_msg,
    tgui_msg_handler(VSF_TGUI_EVT_POST_REFRESH, __on_text_list_post_refresh),
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK, __on_text_list_click),
    tgui_msg_handler(VSF_TGUI_EVT_LIST_SELECTION_CHANGED, __on_tool_selection_changed),
);

static describe_tgui_msgmap(__on_tool_button_click_msg,
    tgui_msg_handler(VSF_TGUI_EVT_POINTER_CLICK, __on_tool_button_click),
);

static describe_tgui_msgmap(__on_tool_label_wheel_msg,
    tgui_msg_handler(VSF_TGUI_EVT_GESTURE_WHEEL, __on_tool_lable_gesture_wheel),
);

/*============================ IMPLEMENTATION ================================*/
static void __control_set_msgmap(vsf_tgui_control_t* control, const vsf_tgui_user_evt_handler* items, uint8_t counter, char *msgmap_name)
{
    control->tMSGMap.ptItems = items;
    control->tMSGMap.chCount = counter;
    control->tMSGMap.name_ptr = msgmap_name;
}

static bool __control_is_support_child(vsf_tgui_control_t* control_ptr)
{
    if ((control_ptr->Attribute._.is_container) && (control_ptr->id != VSF_TGUI_COMPONENT_ID_TEXT_LIST)) {
        return true;
    } else {
        return false;
    }
}

static void __container_clean_all_child(vsf_tgui_container_t* container_ptr)
{
    ASSERT(tgui_designer_is_container((vsf_tgui_control_t *)container_ptr));
    container_ptr->node_ptr = NULL;
}

static void __tgui_designer_text_set(vsf_tgui_label_t* label)
{
    vsf_tgui_text_set(&label->tLabel, &label->tLabel.tString);
}

static void __tgui_designer_log_init(tgui_designer_log_label_t* log_ptr)
{
    ASSERT(tgui_designer_is_label((vsf_tgui_control_t *)&log_ptr->label));

    memset(log_ptr->buffer, 0, sizeof(log_ptr->buffer));

    uint8_t line_height = vsf_tgui_font_get_char_height(log_ptr->chFontIndex) + log_ptr->tLabel.chInterLineSpace;
    int16_t max_ver = log_ptr->tRegion.tSize.iHeight + log_ptr->tLabel.chInterLineSpace;
    log_ptr->max_line = max_ver / line_height;
}

static void __tgui_designer_append_log(char* new_log)
{
    static int __cnt = 0;
    tgui_designer_log_label_t* log_lable_ptr = &__designer_ptr->log;

    if (__cnt++ >= log_lable_ptr->max_line) {
        __cnt = 0;
        log_lable_ptr->buffer[0] = '\0';
    }

    strcat(log_lable_ptr->buffer, new_log);
    __tgui_designer_text_set(&__designer_ptr->log.label);
    vk_tgui_refresh_ex(__designer_ptr->gui_ptr, (vsf_tgui_control_t*)&__designer_ptr->log, NULL);
}

static void __tgui_control_draw_border(vsf_tgui_control_t * draw_control, vsf_tgui_control_t *mask_control, const vsf_tgui_region_t* dirty_region_ptr, vsf_tgui_sv_color_t color)
{
    vsf_tgui_region_t region = { 0 };
    vsf_tgui_region_t absolute_mask_region = { 0 };
    vsf_tgui_region_t absolute_draw_region = { 0 };
    vsf_tgui_region_t draw_region = { 0 };

    vsf_tgui_get_absolute_control_region(draw_control, &absolute_draw_region);
    vsf_tgui_get_absolute_control_region(mask_control, &absolute_mask_region);

    if (!vsf_tgui_region_intersect(&draw_region, &absolute_draw_region, &absolute_mask_region)) {
        return ;
    }

    draw_region.tLocation.iX -= absolute_draw_region.tLocation.iX;
    draw_region.tLocation.iY -= absolute_draw_region.tLocation.iY;

    region.tLocation.iX = draw_region.tLocation.iX;
    region.tLocation.iY = draw_region.tLocation.iY;
    region.tSize.iWidth = draw_region.tSize.iWidth;
    region.tSize.iHeight = APP_TGUI_DESIGNER_PADDING_WIDTH;
    vsf_tgui_control_v_draw_rect(draw_control, dirty_region_ptr, &region, color);

    region.tLocation.iX = draw_region.tLocation.iX;
    region.tLocation.iY = draw_region.tLocation.iY + draw_region.tSize.iHeight - APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tSize.iWidth = draw_region.tSize.iWidth;
    region.tSize.iHeight = APP_TGUI_DESIGNER_PADDING_WIDTH;
    vsf_tgui_control_v_draw_rect(draw_control, dirty_region_ptr, &region, color);

    region.tLocation.iX = draw_region.tLocation.iX;
    region.tLocation.iY = draw_region.tLocation.iY + APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tSize.iWidth = APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tSize.iHeight = draw_region.tSize.iHeight - 2 * APP_TGUI_DESIGNER_PADDING_WIDTH;
    vsf_tgui_control_v_draw_rect(draw_control, dirty_region_ptr, &region, color);

    region.tLocation.iX = draw_region.tLocation.iX + draw_region.tSize.iWidth - APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tLocation.iY = draw_region.tLocation.iY + APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tSize.iWidth = APP_TGUI_DESIGNER_PADDING_WIDTH;
    region.tSize.iHeight = draw_region.tSize.iHeight - 2 * APP_TGUI_DESIGNER_PADDING_WIDTH;
    vsf_tgui_control_v_draw_rect(draw_control, dirty_region_ptr, &region, color);
}

static int __label_update_int_value(tgui_designer_int_label_t* label_ptr, int new_value)
{
    char name[128];
    strcpy(name, vsf_tgui_control_get_var_name((vsf_tgui_control_t*)label_ptr));

    new_value = max(label_ptr->min_value, new_value);
    new_value = min(label_ptr->max_value, new_value);
    label_ptr->cur_value = new_value;

    snprintf(label_ptr->buffer, sizeof(label_ptr->buffer), "%s:%d", name, new_value);
    __tgui_designer_text_set(&label_ptr->label);

    return new_value;
}

static int __label_update_offset(tgui_designer_int_label_t* label_ptr, int offset)
{
    return __label_update_int_value(label_ptr, label_ptr->cur_value + offset);
}

static int __align_mode_get_offset(const vsf_tgui_align_mode_t mode)
{
    for (int i = 0; i < dimof(tiles_list); i++) {
        if (mode == __align_modes[i]) {
            return i;
        }
    }

    ASSERT(0);
    return -1;
}

static int __tile_get_offset(const vsf_tgui_tile_t *tile_ptr)
{
    if (tile_ptr == NULL) {
        return dimof(tiles_list);
    } else {
        int i;
        for (i = 0; i < dimof(tiles_list); i++) {
            if (tile_ptr == tiles_list[i]) {
                return i;
            }
        }

        ASSERT(0);
        return -1;
    }
}

static vsf_tgui_user_evt_handler* __designer_control_get_msgmaps(vsf_tgui_control_t* control_ptr)
{
    if (    ((uint8_t*)__designer_ptr < (uint8_t*)control_ptr)
        &&  ((uint8_t*)control_ptr < ((uint8_t*)__designer_ptr + sizeof(*__designer_ptr)))) {
        ASSERT(0);
    }

    int msgmap_size = dimof(__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array) * sizeof(vsf_tgui_user_evt_handler);
    vsf_tgui_user_evt_handler* msgmap_ptr = (vsf_tgui_user_evt_handler*)((uint8_t*)control_ptr - msgmap_size);

    return msgmap_ptr;
}

static uint32_t __designer_op_value_update(tgui_designer_op_t *op_ptr, uint32_t* new_value_ptr)
{
    ASSERT(op_ptr != NULL);
    if (!(op_ptr->op_type & VSF_TGUI_OP_INCLUDE_VALUE)) {
        return 0;
    }

    vsf_tgui_control_t* control_ptr = op_ptr->control_ptr;
    uint32_t current_value;

    switch (op_ptr->op_type & ~VSF_TGUI_OP_INCLUDE_VALUE) {
    case VSF_TGUI_UPDATE_LOCATION:
        memcpy(&current_value, &control_ptr->tRegion.tLocation, sizeof(control_ptr->tRegion.tLocation));
        if (new_value_ptr != NULL) {
            op_ptr->old_value = current_value;
            op_ptr->new_value = *new_value_ptr;
            memcpy(&control_ptr->tRegion.tLocation, new_value_ptr, sizeof(control_ptr->tRegion.tLocation));
        }
        break;

    case VSF_TGUI_UPDATE_SIZE:
        memcpy(&current_value, &control_ptr->tRegion.tSize, sizeof(control_ptr->tRegion.tSize));
        if (new_value_ptr != NULL) {
            op_ptr->old_value = current_value;
            op_ptr->new_value = *new_value_ptr;
            memcpy(&control_ptr->tRegion.tSize, new_value_ptr, sizeof(control_ptr->tRegion.tSize));
        }
        break;
    }

    return current_value;
}

static void __designer_op_add(uint8_t op_type, vsf_tgui_control_t* control_ptr, uint32_t new_value)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);

    tgui_designer_ops_t* ops_ptr = &__designer_ptr->ops;

#if 0
    // todo: same type op for one control can be merged
    if (   (control_ptr == ops_ptr->items[ops_ptr->current].control_ptr)
        && (op_type == ops_ptr->items[ops_ptr->current].op_type)) {
        ops_ptr->items[ops_ptr->current].new_value = new_value;
        // copy new value to control and backup new value
        return;
    }
#endif

    tgui_designer_op_t* op = &ops_ptr->items[ops_ptr->current];
    if (op->control_ptr != NULL) {
        ops_ptr->current++;
        op++;
        ASSERT(__designer_ptr->ops.current < dimof(__designer_ptr->ops.items));
    }

    op->control_ptr = control_ptr;
    op->op_type = op_type;
    __designer_op_value_update(op, &new_value);
}

static void __tools_refresh_control(vsf_tgui_control_t* control_ptr)
{
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.colors.red, control_ptr->tBackgroundColor.tColor.chR);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.colors.green, control_ptr->tBackgroundColor.tColor.chG);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.colors.blue, control_ptr->tBackgroundColor.tColor.chB);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.colors.alpha, control_ptr->tBackgroundColor.tColor.chA);

    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.regions.x, control_ptr->tRegion.tLocation.iX);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.regions.y, control_ptr->tRegion.tLocation.iY);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.regions.width, control_ptr->tRegion.tSize.iWidth);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.regions.height, control_ptr->tRegion.tSize.iHeight);

    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.margins.left, control_ptr->tMargin.chLeft);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.margins.top, control_ptr->tMargin.chTop);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.margins.right, control_ptr->tMargin.chRight);
    __label_update_int_value(&__designer_ptr->tools.ctrl_attrs.margins.bottom, control_ptr->tMargin.chBottom);

    vsf_tgui_user_evt_handler* msgmap_ptr = __designer_control_get_msgmaps(control_ptr);
    for (int i = 0; i < dimof(__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array); i++) {
        __designer_ptr->tools.ctrl_attrs.msgmaps.btn_array[i].button._.bIsChecked = (msgmap_ptr[i].msg != 0);
    }
    vsf_tgui_text_list_select_set(&__designer_ptr->tools.ctrl_attrs.tile_container.tiles,
                                  __tile_get_offset(control_ptr->tBackground.ptTile));

    vsf_tgui_text_list_select_set(&__designer_ptr->tools.ctrl_attrs.tile_container.tile_align,
                                  __align_mode_get_offset(control_ptr->tBackground.tAlign));

}

static void __tools_refresh_label(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.label_attrs.bIsVisible = tgui_designer_is_label(control_ptr);
    if (__designer_ptr->tools.label_attrs.bIsVisible) {
        vsf_tgui_label_t* label_ptr = (vsf_tgui_label_t*)control_ptr;

        vsf_tgui_text_list_select_set(&__designer_ptr->tools.label_attrs.font, label_ptr->chFontIndex);
        vsf_tgui_text_list_select_set(&__designer_ptr->tools.label_attrs.label_align, __align_mode_get_offset(label_ptr->tLabel.u4Align));

        __label_update_int_value(&__designer_ptr->tools.label_attrs.font_colors.red, label_ptr->tFontColor.tColor.chR);
        __label_update_int_value(&__designer_ptr->tools.label_attrs.font_colors.green, label_ptr->tFontColor.tColor.chG);
        __label_update_int_value(&__designer_ptr->tools.label_attrs.font_colors.blue, label_ptr->tFontColor.tColor.chB);
        __label_update_int_value(&__designer_ptr->tools.label_attrs.font_colors.alpha, label_ptr->tFontColor.tColor.chA);
        __label_update_int_value(&__designer_ptr->tools.label_attrs.line_height, label_ptr->tLabel.chInterLineSpace);

        vsf_tgui_text_set(&__designer_ptr->tools.label_attrs.content.tLabel, &label_ptr->tLabel.tString);
        __designer_ptr->tools.label_attrs.auto_size._.bIsChecked = label_ptr->tLabel.bIsAutoSize;
        __designer_ptr->tools.label_attrs.raw_view._.bIsChecked = label_ptr->bIsUseRawView;
    }
}

static void __tools_refresh_button(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.btn_attrs.bIsVisible = tgui_designer_is_button(control_ptr);
    if (__designer_ptr->tools.btn_attrs.bIsVisible) {
        vsf_tgui_button_t* button_ptr = (vsf_tgui_button_t*)control_ptr;
        __designer_ptr->tools.btn_attrs.check_btn._.bIsChecked = button_ptr->_.bIsCheckButton;
    }
}

static void __tools_refresh_container(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.cont_attrs.bIsVisible = tgui_designer_is_container(control_ptr);
    if (__designer_ptr->tools.cont_attrs.bIsVisible) {
        vsf_tgui_container_t* container_ptr = (vsf_tgui_container_t*)control_ptr;
        __designer_ptr->tools.cont_attrs.auto_size._.bIsChecked = container_ptr->ContainerAttribute.bIsAutoSize;
        __designer_ptr->tools.cont_attrs.show_corner._.bIsChecked = container_ptr->bIsShowCornerTile;
        __designer_ptr->tools.cont_attrs.refresh_whole._.bIsChecked = container_ptr->ContainerAttribute.is_forced_to_refresh_whole_background;

        vsf_tgui_text_list_select_set(&__designer_ptr->tools.cont_attrs.type, container_ptr->ContainerAttribute.u5Type);

        __label_update_int_value(&__designer_ptr->tools.cont_attrs.padding.left, container_ptr->tContainerPadding.chLeft);
        __label_update_int_value(&__designer_ptr->tools.cont_attrs.padding.top, container_ptr->tContainerPadding.chTop);
        __label_update_int_value(&__designer_ptr->tools.cont_attrs.padding.right, container_ptr->tContainerPadding.chRight);
        __label_update_int_value(&__designer_ptr->tools.cont_attrs.padding.bottom, container_ptr->tContainerPadding.chBottom);
    }
}

static void __tools_refresh_text_list(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.text_list_attrs.bIsVisible = tgui_designer_is_text_list(control_ptr);
    if (__designer_ptr->tools.text_list_attrs.bIsVisible) {

    }
}

static void __tools_refresh_list(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.list_attrs.bIsVisible = tgui_designer_is_list(control_ptr);
    if (__designer_ptr->tools.list_attrs.bIsVisible) {

    }
}

static void __tools_refresh_panel(vsf_tgui_control_t* control_ptr)
{
    __designer_ptr->tools.panel_attrs.bIsVisible = tgui_designer_is_panel(control_ptr);
    if (__designer_ptr->tools.panel_attrs.bIsVisible) {

    }
}

static void __tools_refresh_current_active(void)
{
    vsf_tgui_control_t * control_ptr = __designer_ptr->cur_ctrl_ptr;

    __tools_refresh_control(control_ptr);
    __tools_refresh_label(control_ptr);
    __tools_refresh_button(control_ptr);
    __tools_refresh_container(control_ptr);
    __tools_refresh_text_list(control_ptr);
    __tools_refresh_list(control_ptr);
    __tools_refresh_panel(control_ptr);

    vk_tgui_update_tree(__designer_ptr->gui_ptr, (const vsf_tgui_control_t *)&__designer_ptr->tools);

    vk_tgui_refresh(__designer_ptr->gui_ptr);
}

static vsf_tgui_control_t* __tgui_design_create_with_id(vsf_tgui_container_t* parent, vsf_tree_msg_node_id_t id, vsf_tgui_location_t location)
{
    ASSERT(parent != NULL);

    char name_buffer[128];
    tgui_designer_gen_control_name(name_buffer, sizeof(name_buffer), id);

    // alloc memory before control, because control size is related to the type
    int msgmap_size = dimof(__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array) * sizeof(vsf_tgui_user_evt_handler);
    vsf_tgui_control_t * control_ptr = vsf_tgui_control_create(parent, id, name_buffer, true, msgmap_size);
    __control_set_msgmap(control_ptr, __on_design_msg, dimof(__on_design_msg), "__on_design_msg");

    vsf_tgui_location_t* location_ptr = vsf_tgui_control_get_location(control_ptr);
    *location_ptr = location;

    // todo: temp fixed
    if (parent->id == VSF_TGUI_COMPONENT_ID_LIST) {
        vsf_tgui_list_t* list = (vsf_tgui_list_t*)parent;
        list->ptList->ContainerAttribute.u5Type = VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL;
    }

    vsf_tgui_control_set_active(control_ptr);

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "create new %s\n", control_ptr->node_name_ptr);
    __tgui_designer_append_log(buffer);

    __designer_op_add(VSF_TGUI_CREATE_NEW_ITEM, control_ptr, 0);

    __designer_ptr->cur_ctrl_ptr = control_ptr;

    return control_ptr;
}

static void __tgui_design_create(vsf_tgui_container_t* parent, vsf_tgui_evt_t* event_ptr)
{
    ASSERT(parent != NULL);
    ASSERT(event_ptr != NULL);

    int id = vsf_tgui_text_list_select_get(&__designer_ptr->tools.ctrl_attrs.component_ids);

    vsf_tgui_location_t parent_absolute_location = { 0 };
    vsf_tgui_control_calculate_absolute_location((vsf_tgui_control_t*)parent, &parent_absolute_location);
    vsf_tgui_location_t click_location = {
        .iX = event_ptr->PointerEvt.iX - parent_absolute_location.iX - parent->tContainerPadding.chLeft,
        .iY = event_ptr->PointerEvt.iY - parent_absolute_location.iY - parent->tContainerPadding.chTop,
    };

    vsf_tgui_control_t* control_ptr = __tgui_design_create_with_id(parent, id, click_location);

    vsf_tgui_size_t* size_ptr = vsf_tgui_control_get_size(control_ptr);
    vsf_tgui_location_t* location_ptr = vsf_tgui_control_get_location(control_ptr);
    location_ptr->iX -= size_ptr->iWidth / 2;
    location_ptr->iY -= size_ptr->iHeight / 2;

    vsf_tgui_set_pointer_location(__designer_ptr->gui_ptr, 0, NULL);
}


tgui_designer_t* tgui_designer_init(tgui_designer_t* tgui_designer_ptr, vsf_tgui_t* gui_ptr)
{
    if (NULL == tgui_designer_ptr && NULL != gui_ptr) {
        VSF_TGUI_ASSERT(0);
        return NULL;
    }

    __designer_ptr = tgui_designer_ptr;

    describ_tgui_panel(tgui_designer_t, *tgui_designer_ptr,
        tgui_region(0, 0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX),
        tgui_msgmap(__panel_msg),
        tgui_attribute(bIsShowCornerTile, false),
        tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_WHITE),
        tgui_container_type(VSF_TGUI_CONTAINER_TYPE_PLANE),

        tgui_panel(tools, tgui_designer_ptr, tools, code_gen,
            tgui_region(APP_TGUI_DESIGNER_RIGHT_ATTRS_X, APP_TGUI_DESIGNER_RIGHT_ATTRS_Y, APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, APP_TGUI_DESIGNER_RIGHT_ATTRS_VER),
            tgui_background((const vsf_tgui_tile_t*)&bg2_RGB, VSF_TGUI_ALIGN_CENTER),
            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
            tgui_attribute(bIsShowCornerTile , false),
            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_PURPLE),

            tgui_contains(
                tgui_container(ctrl_attrs, &tgui_designer_ptr->tools, ctrl_attrs, label_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),

                    tgui_contains(
                        tgui_text_list(component_ids, &tgui_designer_ptr->tools.ctrl_attrs, component_ids, tile_container,
                            tgui_region(0, 0, 70, APP_TGUI_DESIGNER_VER_MAX / 4),
                            tgui_attribute(bIsAutoSize, true),
                            tgui_margin(0, 0, 8, 8),
                            tgui_msgmap(__on_tool_text_list_msg),

                            tgui_text_list_content(
                                tgui_size(70, 0),
                                tgui_text(tLabel, "control\n" "container\n" "label\n" "button\n" "panel\n" "list\n" "text_list", true, VSF_TGUI_ALIGN_LEFT),
                                tgui_line_space(tLabel, 8),
                                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            )

                        ),

                        tgui_container(tile_container, &tgui_designer_ptr->tools.ctrl_attrs, component_ids, colors,
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_RED),
                            tgui_region(0, 0, APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR - 80, 0),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                            tgui_contains(
                                tgui_text_list(tiles, &tgui_designer_ptr->tools.ctrl_attrs.tile_container, tiles, tile_align,
                                    tgui_region(0, 0, (APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR - 100 - 80 - 8), APP_TGUI_DESIGNER_VER_MAX / 4),
                                    tgui_margin(0, 0, 8, 8),
                                    tgui_msgmap(__on_tool_text_list_msg),

                                    tgui_text_list_content(
                                        tgui_size(100, 0),
                                        tgui_text(tLabel, TILES_TEXT_LIST, true, VSF_TGUI_ALIGN_LEFT),
                                        tgui_line_space(tLabel, 8),
                                        tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    )
                                ),

                                tgui_text_list(tile_align, &tgui_designer_ptr->tools.ctrl_attrs.tile_container, tiles, tile_align,
                                    tgui_region(0, 0, 100, APP_TGUI_DESIGNER_VER_MAX / 4),
                                    //tgui_attribute(bIsAutoSize, true),
                                    tgui_margin(0, 0, 0, 8),
                                    tgui_msgmap(__on_tool_text_list_msg),

                                    tgui_text_list_content(
                                        tgui_size(VSF_TGUI_HOR_MAX - APP_TGUI_DESIGNER_HOR_MAX, 0),
                                        tgui_text(tLabel, TGUI_ALIGN_MODE_TEXT, true, VSF_TGUI_ALIGN_LEFT),
                                        tgui_line_space(tLabel, 8),
                                        tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    )
                                ),

                            )
                        ),

                        tgui_container(colors, &tgui_designer_ptr->tools.ctrl_attrs, tile_container, regions,
                            tgui_region(0, 0, APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_GRAY),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                            tgui_margin(0, 0, 0, 8),
                            tgui_contains(
                                tgui_label(red, &tgui_designer_ptr->tools.ctrl_attrs.colors, red, green,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.colors.red.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tBackgroundColor.tColor.chR)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tBackgroundColor.tColor.chR)),
                                ),
                                tgui_label(green, &tgui_designer_ptr->tools.ctrl_attrs.colors, red, blue,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.colors.green.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tBackgroundColor.tColor.chG)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tBackgroundColor.tColor.chG)),
                                ),
                                tgui_label(blue, &tgui_designer_ptr->tools.ctrl_attrs.colors, green, alpha,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.colors.blue.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tBackgroundColor.tColor.chB)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tBackgroundColor.tColor.chB)),
                                ),
                                tgui_label(alpha, &tgui_designer_ptr->tools.ctrl_attrs.colors, blue, alpha,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.colors.alpha.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tBackgroundColor.tColor.chA)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tBackgroundColor.tColor.chA)),
                                ),
                            )
                        ),

                        tgui_container(regions, &tgui_designer_ptr->tools.ctrl_attrs, colors, margins,
                            tgui_margin(0, 0, 0, 8),
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_GRAY),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),
                            tgui_contains(
                                tgui_label(x, &tgui_designer_ptr->tools.ctrl_attrs.regions, x, y,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.regions.x.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, -100),
                                    tgui_attribute(max_value, APP_TGUI_DESIGNER_HOR_MAX),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tRegion.tLocation.iX)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tRegion.tLocation.iX)),
                                ),

                                tgui_label(y, &tgui_designer_ptr->tools.ctrl_attrs.regions, x, width,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.regions.y.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, -100),
                                    tgui_attribute(max_value, APP_TGUI_DESIGNER_VER_MAX),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tRegion.tLocation.iY)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tRegion.tLocation.iY)),
                                ),

                                tgui_label(width, &tgui_designer_ptr->tools.ctrl_attrs.regions, y, height,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.regions.width.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 1),
                                    tgui_attribute(max_value, APP_TGUI_DESIGNER_HOR_MAX),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tRegion.tSize.iWidth)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tRegion.tSize.iWidth)),
                                ),

                                tgui_label(height, &tgui_designer_ptr->tools.ctrl_attrs.regions, width, height,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.regions.height.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 1),
                                    tgui_attribute(max_value, APP_TGUI_DESIGNER_VER_MAX),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tRegion.tSize.iHeight)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tRegion.tSize.iHeight)),
                                ),
                            )
                        ),

                        tgui_container(margins, &tgui_designer_ptr->tools.ctrl_attrs, regions, msgmaps,
                            tgui_margin(0, 0, 0, 8),
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_GRAY),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL),
                            tgui_contains(
                                tgui_label(left, &tgui_designer_ptr->tools.ctrl_attrs.margins, left, top,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.margins.left.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tMargin.chLeft)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tMargin.chLeft)),
                                ),

                                tgui_label(top, &tgui_designer_ptr->tools.ctrl_attrs.margins, left, right,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.margins.top.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tMargin.chTop)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tMargin.chTop)),
                                ),

                                tgui_label(right, &tgui_designer_ptr->tools.ctrl_attrs.margins, top, bottom,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_margin(0, 0, 8, 0),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.margins.right.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tMargin.chRight)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tMargin.chRight)),
                                ),

                                tgui_label(bottom, &tgui_designer_ptr->tools.ctrl_attrs.margins, right, bottom,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.ctrl_attrs.margins.bottom.buffer, false, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_control_t, tMargin.chBottom)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_control_t, tMargin.chBottom)),
                                ),
                            )
                        ),

                        tgui_container(msgmaps, &tgui_designer_ptr->tools.ctrl_attrs, margins, msgmaps,
                            tgui_margin(0, 0, 0, 8),
                            tgui_region(0, 0, APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_GRAY),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                            tgui_contains(

#define __button(__current, __default_text, __pre, __next, __evt)                                              \
            tgui_button(btns.__current, &tgui_designer_ptr->tools.ctrl_attrs.msgmaps, btns.__pre, btns.__next, \
                tgui_region(0, 0, 200 - 2, 20),                                                                \
                tgui_margin(0, 0, 2, 2),                                                                       \
                tgui_attribute(bIsUseRawView, true),                                                           \
                tgui_attribute(bIsCheckButton, true),                                                          \
                tgui_msgmap(__on_tool_button_click_msg),                                                       \
                tgui_text(tLabel, __default_text, true, VSF_TGUI_ALIGN_LEFT),                                  \
                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),                                   \
                tgui_attribute(desinger_msg, __evt),                                                           \
            )

                                __button(low_level_ready_to_refresh,    "low leave refresh",            low_level_ready_to_refresh,   control_evt,                  VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH),
                                __button(control_evt,                   "control_evt",                  low_level_ready_to_refresh,   on_set_top_container,         VSF_TGUI_MSG_CONTROL_EVT),
                                __button(on_set_top_container,          "set top container",            control_evt,                  on_load,                      VSF_TGUI_EVT_ON_SET_TOP_CONTAINER),
                                __button(on_load,                       "on load",                      on_set_top_container,         on_depose,                    VSF_TGUI_EVT_ON_LOAD),
                                __button(on_depose,                     "on depose",                    on_load,                      update,                       VSF_TGUI_EVT_ON_DEPOSE),
                                __button(update,                        "update",                       on_depose,                    update_tree,                  VSF_TGUI_EVT_UPDATE),
                                __button(update_tree,                   "update tree",                  update,                       refresh,                      VSF_TGUI_EVT_UPDATE_TREE),
                                __button(refresh,                       "refresh",                      update_tree,                  post_refresh,                 VSF_TGUI_EVT_REFRESH),
                                __button(post_refresh,                  "post refresh",                 refresh,                      get_active,                   VSF_TGUI_EVT_POST_REFRESH),
                                __button(get_active,                    "get active",                   post_refresh,                 lost_active,                  VSF_TGUI_EVT_GET_ACTIVE),
                                __button(lost_active,                   "lost active",                  get_active,                   on_time,                      VSF_TGUI_EVT_LOST_ACTIVE),
                                __button(on_time,                       "on time",                      lost_active,                  pointer_down,                 VSF_TGUI_EVT_ON_TIME),
                                __button(pointer_down,                  "pointer down",                 on_time,                      pointer_hold,                 VSF_TGUI_EVT_POINTER_DOWN),
                                __button(pointer_hold,                  "pointer hold",                 pointer_down,                 pointer_up,                   VSF_TGUI_EVT_POINTER_HOLD),
                                __button(pointer_up,                    "pointer up",                   pointer_hold,                 pointer_click,                VSF_TGUI_EVT_POINTER_UP),
                                __button(pointer_click,                 "pointer click",                pointer_up,                   pointer_double_click,         VSF_TGUI_EVT_POINTER_CLICK),
                                __button(pointer_double_click,          "pointer double click",         pointer_click,                pointer_enter,                VSF_TGUI_EVT_POINTER_DOUBLE_CLICK),
                                __button(pointer_enter,                 "pointer enter",                pointer_double_click,         pointer_left,                 VSF_TGUI_EVT_POINTER_ENTER),
                                __button(pointer_left,                  "pointer left",                 pointer_enter,                pointer_hover,                VSF_TGUI_EVT_POINTER_LEFT),
                                __button(pointer_hover,                 "pointer hover",                pointer_left,                 pointer_move,                 VSF_TGUI_EVT_POINTER_HOVER),
                                __button(pointer_move,                  "pointer move",                 pointer_hover,                key_down,                     VSF_TGUI_EVT_POINTER_MOVE),
                                __button(key_down,                      "key down",                     pointer_move,                 key_repeate,                  VSF_TGUI_EVT_KEY_DOWN),
                                __button(key_repeate,                   "key repeate",                  key_down,                     key_up,                       VSF_TGUI_EVT_KEY_REPEATE),
                                __button(key_up,                        "key up",                       key_repeate,                  key_pressed,                  VSF_TGUI_EVT_KEY_UP),
                                __button(key_pressed,                   "key pressed",                  key_up,                       key_long_pressed,             VSF_TGUI_EVT_KEY_PRESSED),
                                __button(key_long_pressed,              "key long_pressed",             key_pressed,                  key_double_click,             VSF_TGUI_EVT_KEY_LONG_PRESSED),
                                __button(key_double_click,              "key double_click",             key_long_pressed,             gesture_slide,                VSF_TGUI_EVT_KEY_DOUBLE_CLICK),
                                __button(gesture_slide,                 "gesture slide",                key_double_click,             gesture_wheel,                VSF_TGUI_EVT_GESTURE_SLIDE),
                                __button(gesture_wheel,                 "gesture wheel",                gesture_slide,                gesture_zoom_in,              VSF_TGUI_EVT_GESTURE_WHEEL),
                                __button(gesture_zoom_in,               "gesture zoom_in",              gesture_wheel,                gesture_zoom_out,             VSF_TGUI_EVT_GESTURE_ZOOM_IN),
                                __button(gesture_zoom_out,              "gesture zoom out",             gesture_zoom_in,              gesture_rotate_clockwise,     VSF_TGUI_EVT_GESTURE_ZOOM_OUT),
                                __button(gesture_rotate_clockwise,      "gesture rotate clockwise",     gesture_zoom_out,             gesture_rotate_anticlockwise, VSF_TGUI_EVT_GESTURE_ROTATE_CLOCKWISE),
                                __button(gesture_rotate_anticlockwise,  "gesture rotate anticlockwise", gesture_rotate_clockwise,     list_selection_changed,       VSF_TGUI_EVT_GESTURE_ROTATE_ANTICLOCKWISE),
                                __button(list_selection_changed,        "list selection changed",       gesture_rotate_anticlockwise, list_sliding_started,         VSF_TGUI_EVT_LIST_SELECTION_CHANGED),
                                __button(list_sliding_started,          "list sliding started",         list_selection_changed,       list_sliding_stopped,         VSF_TGUI_EVT_LIST_SLIDING_STARTED),
                                __button(list_sliding_stopped,          "list sliding stopped",         list_sliding_started,         list_sliding_stopped,         VSF_TGUI_EVT_LIST_SLIDING_STOPPED),
                            )
                        ),
                    )
                ),

                tgui_container(label_attrs, &tgui_designer_ptr->tools, ctrl_attrs, btn_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),

                    tgui_contains(
                        tgui_text_list(font, &tgui_designer_ptr->tools.label_attrs, font, label_align,
                            tgui_region(0, 0, 200, APP_TGUI_DESIGNER_VER_MAX / 4),
                            tgui_margin(0, 0, 8, 8),
                            tgui_msgmap(__on_tool_text_list_msg),

                            tgui_text_list_content(
                                tgui_text(tLabel, VSF_TGUI_FONTS_TEXT, true, VSF_TGUI_ALIGN_LEFT),
                                tgui_line_space(tLabel, 8),
                                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            )
                        ),

                        tgui_text_list(label_align, &tgui_designer_ptr->tools.label_attrs, font, font_colors,
                            tgui_region(0, 0, 150, APP_TGUI_DESIGNER_VER_MAX / 4),
                            //tgui_attribute(bIsAutoSize, true),
                            tgui_margin(0, 0, 0, 8),
                            tgui_msgmap(__on_tool_text_list_msg),

                            tgui_text_list_content(
                                tgui_size(VSF_TGUI_HOR_MAX - APP_TGUI_DESIGNER_HOR_MAX, 0),
                                tgui_text(tLabel, TGUI_ALIGN_MODE_TEXT, true, VSF_TGUI_ALIGN_LEFT),
                                tgui_line_space(tLabel, 8),
                                tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
                                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            )
                        ),

                        tgui_container(font_colors, &tgui_designer_ptr->tools.label_attrs, label_align, content,
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_RED),
                            tgui_region(0, 0, APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                            tgui_margin(0, 0, 0, 8),
                            tgui_contains(
                                tgui_label(red, &tgui_designer_ptr->tools.label_attrs.font_colors, red, green,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.label_attrs.font_colors.red.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_label_t, tFontColor.tColor.chR)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_label_t, tFontColor.tColor.chR)),
                                ),
                                tgui_label(green, &tgui_designer_ptr->tools.label_attrs.font_colors, red, blue,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.label_attrs.font_colors.green.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_label_t, tFontColor.tColor.chG)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_label_t, tFontColor.tColor.chG)),
                                ),
                                tgui_label(blue, &tgui_designer_ptr->tools.label_attrs.font_colors, green, alpha,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.label_attrs.font_colors.blue.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_label_t, tFontColor.tColor.chB)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_label_t, tFontColor.tColor.chB)),
                                ),
                                tgui_label(alpha, &tgui_designer_ptr->tools.label_attrs.font_colors, blue, alpha,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.label_attrs.font_colors.alpha.buffer, true, VSF_TGUI_ALIGN_LEFT),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                    tgui_attribute(offset, offset_of(vsf_tgui_label_t, tFontColor.tColor.chA)),
                                    tgui_attribute(item_size, size_of_struct_member(vsf_tgui_label_t, tFontColor.tColor.chA)),
                                ),
                            )
                        ),

                        tgui_label(content, &tgui_designer_ptr->tools.label_attrs, font_colors, line_height,
                            tgui_region(0, 0, 80, 20),
                            tgui_margin(0, 0, 8, 8),
                            tgui_attribute(bIsUseRawView, true),
                            tgui_text(tLabel, "content", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                        ),

                        tgui_label(line_height, &tgui_designer_ptr->tools.label_attrs, content, auto_size,
                            tgui_region(0, 0, 80, 20),
                            tgui_margin(0, 0, 8, 8),
                            tgui_attribute(bIsUseRawView, true),
                            tgui_text(tLabel, tgui_designer_ptr->tools.label_attrs.line_height.buffer, true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            tgui_attribute(min_value, 0),
                            tgui_attribute(max_value, 255),
                            tgui_attribute(offset, offset_of(vsf_tgui_label_t, tLabel.chInterLineSpace)),
                            tgui_attribute(item_size, size_of_struct_member(vsf_tgui_label_t, tLabel.chInterLineSpace)),
                        ),

                        tgui_button(auto_size, &tgui_designer_ptr->tools.label_attrs, line_height, raw_view,
                            tgui_region(0, 0, 80, 20),
                            tgui_margin(0, 0, 8, 8),
                            tgui_attribute(bIsUseRawView, true),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_text(tLabel, "auto size", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                        ),

                        tgui_button(raw_view, &tgui_designer_ptr->tools.label_attrs, auto_size, raw_view,
                            tgui_region(0, 0, 100, 30),
                            tgui_margin(0, 0, 8, 8),
                            tgui_attribute(bIsUseRawView, true),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_text(tLabel, "raw view", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                        ),
                    )
                ),

                tgui_container(btn_attrs, &tgui_designer_ptr->tools, label_attrs, cont_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),

                    tgui_contains(
                        tgui_button(check_btn, &tgui_designer_ptr->tools.btn_attrs, check_btn, check_btn,
                            tgui_region(0, 0, 100, 32),
                            tgui_margin(0, 0, 8, 8),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_text(tLabel, "check button", true),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_attribute(bIsUseRawView, true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                        ),
                    )
                ),

                tgui_container(cont_attrs, &tgui_designer_ptr->tools, btn_attrs, cont_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),

                    tgui_contains(
                        tgui_button(auto_size, &tgui_designer_ptr->tools.cont_attrs, auto_size, show_corner,
                            tgui_region(0, 0, 100, 32),
                            tgui_margin(0, 0, 8, 8),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_text(tLabel, "auto size", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_attribute(bIsUseRawView, true),
                        ),

                        tgui_button(show_corner, &tgui_designer_ptr->tools.cont_attrs, auto_size, refresh_whole,
                            tgui_region(0, 0, 100, 32),
                            tgui_margin(0, 0, 8, 8),
                            tgui_text(tLabel, "show corner", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_attribute(bIsUseRawView, true),
                        ),

                        tgui_button(refresh_whole, &tgui_designer_ptr->tools.cont_attrs, show_corner, padding,
                            tgui_region(0, 0, 100, 32),
                            tgui_margin(0, 0, 8, 8),
                            tgui_text(tLabel, "refresh whole", true),
                            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            tgui_msgmap(__on_tool_button_click_msg),
                            tgui_attribute(bIsCheckButton, true),
                            tgui_attribute(bIsUseRawView, true),
                        ),

                        tgui_container(padding, &tgui_designer_ptr->tools.cont_attrs, refresh_whole, type,
                            tgui_margin(0, 0, 8, 8),
                            tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                            tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                            tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),

                            tgui_contains(
                                tgui_label(left, &tgui_designer_ptr->tools.cont_attrs.padding, left, top,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.cont_attrs.padding.left.buffer, false, VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                ),

                                tgui_label(top, &tgui_designer_ptr->tools.cont_attrs.padding, left, right,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.cont_attrs.padding.top.buffer, false, VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                ),

                                tgui_label(right, &tgui_designer_ptr->tools.cont_attrs.padding, top, bottom,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.cont_attrs.padding.right.buffer, false, VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                ),

                                tgui_label(bottom, &tgui_designer_ptr->tools.cont_attrs.padding, right, bottom,
                                    tgui_region(0, 0, 80, 20),
                                    tgui_text(tLabel, tgui_designer_ptr->tools.cont_attrs.padding.bottom.buffer, false, VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP),
                                    tgui_attribute(bIsUseRawView, true),
                                    tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                                    tgui_msgmap(__on_tool_label_wheel_msg),
                                    tgui_attribute(min_value, 0),
                                    tgui_attribute(max_value, 255),
                                ),
                            )
                        ),

                        tgui_text_list(type, &tgui_designer_ptr->tools.cont_attrs, padding, type,
                            tgui_region(0, 0, 150, APP_TGUI_DESIGNER_VER_MAX / 4),
                            tgui_margin(0, 0, 0, 8),
                            tgui_msgmap(__on_tool_text_list_msg),

                            tgui_text_list_content(
                                tgui_size(VSF_TGUI_HOR_MAX - APP_TGUI_DESIGNER_HOR_MAX, 0),
                                tgui_text(tLabel, TGUI_CONTAINER_TYPE_TEXT, true, VSF_TGUI_ALIGN_LEFT),
                                tgui_line_space(tLabel, 8),
                                tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
                            )
                        ),

                    )
                ),

                tgui_container(list_attrs, &tgui_designer_ptr->tools, cont_attrs, text_list_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                ),

                tgui_container(text_list_attrs, &tgui_designer_ptr->tools, list_attrs, panel_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                ),

                tgui_container(panel_attrs, &tgui_designer_ptr->tools, text_list_attrs, panel_attrs,
                    tgui_container_type(VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL),
                    tgui_attribute(tBackgroundColor, VSF_TGUI_COLOR_YELLOW),
                    tgui_size(APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR, 0),
                ),

            )
        ),

        tgui_button(code_gen, tgui_designer_ptr, tools, log,
            tgui_region(APP_TGUI_DESIGNER_CODE_GEN_X, APP_TGUI_DESIGNER_CODE_GEN_Y, APP_TGUI_DESIGNER_CODE_GEN_HOR, APP_TGUI_DESIGNER_CODE_GEN_VER),
            tgui_text(tLabel, "Generate Code", true),
            tgui_msgmap(__on_tool_button_click_msg),
        ),

        tgui_label(log, tgui_designer_ptr, code_gen, log,
            tgui_region(APP_TGUI_DESIGNER_BOTTOM_INFO_X, APP_TGUI_DESIGNER_BOTTOM_INFO_Y, APP_TGUI_DESIGNER_BOTTOM_INFO_HOR, APP_TGUI_DESIGNER_BOTTOM_INFO_VER),
            tgui_text(tLabel, __designer_ptr->log.buffer, false, VSF_TGUI_ALIGN_LEFT | VSF_TGUI_ALIGN_TOP),
            tgui_attribute(bIsUseRawView, true),
            tgui_attribute(chFontIndex, VSF_TGUI_FONT_WQY_MICROHEI_S16),
        ),
    );

    __container_clean_all_child((vsf_tgui_container_t *)&__designer_ptr->tools.list_attrs);
    __container_clean_all_child((vsf_tgui_container_t *)&__designer_ptr->tools.text_list_attrs);
    __container_clean_all_child((vsf_tgui_container_t *)&__designer_ptr->tools.panel_attrs);
    __tgui_designer_log_init(&__designer_ptr->log);

    vsf_tgui_create_init(gui_ptr);

    return __designer_ptr;
}

static fsm_rt_t __on_top_panel_load(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);
    ASSERT(__designer_ptr == (tgui_designer_t*)control_ptr);


    __designer_ptr->design_panel_ptr = __tgui_design_create_with_id((vsf_tgui_container_t*)__designer_ptr, VSF_TGUI_COMPONENT_ID_PANEL, (vsf_tgui_location_t) { 0, 0 });
    __designer_ptr->design_panel_ptr->tRegion.tSize.iWidth = APP_TGUI_DESIGNER_HOR_MAX;
    __designer_ptr->design_panel_ptr->tRegion.tSize.iHeight = APP_TGUI_DESIGNER_VER_MAX;
    __designer_ptr->design_panel_ptr->tBackground.ptTile = (vsf_tgui_tile_t*)&bg2_RGB;
    __control_set_msgmap(__designer_ptr->design_panel_ptr, __on_design_msg, dimof(__on_design_msg), "__on_design_msg");

    vsf_tgui_user_evt_handler * handler = __designer_control_get_msgmaps(__designer_ptr->design_panel_ptr);
    tgui_designer_msg_button_t* on_load = &__designer_ptr->tools.ctrl_attrs.msgmaps.btns.on_load;
    int offset = on_load - &__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array[0];

    handler[offset].msg = VSF_TGUI_EVT_ON_LOAD;
    handler[offset].u10EvtMask = 0x3FF;

    __tools_refresh_current_active();

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_pointer_down(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_pointer_up(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_gesture_slide(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    vsf_tgui_evt_t* event_ptr = (vsf_tgui_evt_t*)msg_ptr;
    vsf_tgui_location_t* location_ptr = vsf_tgui_control_get_location(control_ptr);

    location_ptr->iX += event_ptr->PointerEvt.iX;
    location_ptr->iY += event_ptr->PointerEvt.iY;
    //vk_tgui_refresh_ex(__designer_ptr->use_as__vsf_tgui_panel_t.gui_ptr, control_ptr->use_as__vsf_msgt_node_t.parent_ptr, NULL);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_gesture_wheel(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    if (control_ptr->id != VSF_TGUI_COMPONENT_ID_TEXT_LIST) {
        vsf_tgui_evt_t* event_ptr = (vsf_tgui_evt_t*)msg_ptr;
        int16_t offset = event_ptr->PointerEvt.iY * APP_TGUI_DESIGNER_WHELL_HALF_UNIT;

        vsf_tgui_location_t* location_ptr = vsf_tgui_control_get_location(control_ptr);
        location_ptr->iX -= offset;
        location_ptr->iY -= offset;

        vsf_tgui_size_t* size_ptr = vsf_tgui_control_get_size(control_ptr);
        size_ptr->iWidth += offset * 2;
        size_ptr->iHeight += offset * 2;

        __tools_refresh_current_active();
    }

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}


static fsm_rt_t __on_design_pointer_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    if (__designer_ptr->cur_ctrl_ptr != control_ptr) {
        __designer_ptr->cur_ctrl_ptr = control_ptr;
        __tools_refresh_current_active();
    } else {
        if (__control_is_support_child(control_ptr)) {
            __tgui_design_create((vsf_tgui_container_t *)control_ptr, (vsf_tgui_evt_t*)msg_ptr);
            __tools_refresh_current_active();
        }
    }

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_panel_post_refresh(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    if (control_ptr == (vsf_tgui_control_t *)__designer_ptr->design_panel_ptr) {
        vsf_tgui_refresh_evt_t* event_ptr = (vsf_tgui_refresh_evt_t*)msg_ptr;
        const vsf_tgui_region_t* dirty_region_ptr = (const vsf_tgui_region_t*)(event_ptr->region_ptr);

        vsf_tgui_sv_color_t color = VSF_TGUI_COLOR_RGBA(0xFF, 0x00, 0x00, 0xFF);
        __tgui_control_draw_border(control_ptr, __designer_ptr->cur_ctrl_ptr, dirty_region_ptr, color);
    }

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_text_list_post_refresh(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    vsf_tgui_refresh_evt_t* event_ptr = (vsf_tgui_refresh_evt_t*)msg_ptr;
    const vsf_tgui_region_t* dirty_region_ptr = (const vsf_tgui_region_t*)(event_ptr->region_ptr);

    vsf_tgui_sv_color_t color = VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR;
    vsf_tgui_region_t region = { 0 };

    region.tSize = *vsf_tgui_control_get_size(control_ptr);

    region.tLocation.iY = region.tSize.iHeight / 2 - 1;
    region.tLocation.iX = 4;
    region.tSize.iHeight = 2;
    region.tSize.iWidth -= 8;

    vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_text_list_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_design_default_msg_handler(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_tool_selection_changed(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);
    ASSERT(tgui_designer_is_text_list(control_ptr));

    vsf_tgui_text_list_t* text_list = (vsf_tgui_text_list_t*)control_ptr;
    int offset = vsf_tgui_text_list_select_get(text_list);
    ASSERT(0 <= offset);

    vsf_tgui_control_t * active_control_ptr = __designer_ptr->cur_ctrl_ptr;

    if (text_list == &__designer_ptr->tools.ctrl_attrs.tile_container.tiles) {
        ASSERT(offset <= dimof(tiles_list));
        active_control_ptr->tBackground.ptTile = (offset == dimof(tiles_list)) ? NULL : tiles_list[offset];
    }  else if (text_list == &__designer_ptr->tools.ctrl_attrs.tile_container.tile_align) {
        ASSERT(offset < dimof(__align_modes));
        active_control_ptr->tBackground.tAlign = __align_modes[offset];
    }  else if (text_list == &__designer_ptr->tools.ctrl_attrs.component_ids) {
        ASSERT(offset < __VSF_TGUI_COMPONENT_TYPE_NUM);
        // do nothing
    }  else if (text_list == &__designer_ptr->tools.label_attrs.font) {
        ASSERT(offset < vsf_tgui_font_number());
        ASSERT(tgui_designer_is_label(active_control_ptr));
        vsf_tgui_label_t* label_ptr = (vsf_tgui_label_t*)active_control_ptr;
        label_ptr->chFontIndex = offset;
    }  else if (text_list == &__designer_ptr->tools.label_attrs.label_align) {
        ASSERT(offset < dimof(__align_modes));
        ASSERT(tgui_designer_is_label(active_control_ptr));
        vsf_tgui_label_t * label_ptr = (vsf_tgui_label_t*)active_control_ptr;
        label_ptr->tLabel.u4Align = __align_modes[offset];
    }  else if (text_list == &__designer_ptr->tools.cont_attrs.type) {
        ASSERT(offset < dimof(__container_types));
        ASSERT(tgui_designer_is_container(active_control_ptr));
        vsf_tgui_container_t * container_ptr = (vsf_tgui_container_t*)active_control_ptr;
        container_ptr->ContainerAttribute.u5Type = __container_types[offset];
    } else {
        ASSERT(0);
    }

    vk_tgui_update(__designer_ptr->gui_ptr, active_control_ptr);
    vk_tgui_refresh(__designer_ptr->gui_ptr);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_tool_button_click(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    vsf_tgui_button_t* button_ptr = (vsf_tgui_button_t*)control_ptr;
    vsf_tgui_control_t* active_control_ptr = __designer_ptr->cur_ctrl_ptr;

    if (button_ptr == &__designer_ptr->tools.label_attrs.auto_size) {
        ASSERT(tgui_designer_is_label(active_control_ptr));
        vsf_tgui_label_t* active_label_ptr = (vsf_tgui_label_t*)active_control_ptr;
        active_label_ptr->tLabel.bIsAutoSize = !active_label_ptr->tLabel.bIsAutoSize;
        if (active_label_ptr->tLabel.bIsAutoSize) {
            active_label_ptr->tRegion.tSize.iWidth = 10;
            active_label_ptr->tRegion.tSize.iHeight = 10;
        }
    } else if (button_ptr == &__designer_ptr->tools.label_attrs.raw_view) {
        ASSERT(tgui_designer_is_label(active_control_ptr));
        vsf_tgui_label_t* active_label_ptr = (vsf_tgui_label_t*)active_control_ptr;
        active_label_ptr->bIsUseRawView = !active_label_ptr->bIsUseRawView;
        if (active_label_ptr->tLabel.bIsAutoSize) {
            active_label_ptr->tRegion.tSize.iWidth = 10;
            active_label_ptr->tRegion.tSize.iHeight = 10;
        }
    } else if (button_ptr == &__designer_ptr->tools.btn_attrs.check_btn) {
        ASSERT(tgui_designer_is_button(active_control_ptr));
        vsf_tgui_button_t* active_button_ptr = (vsf_tgui_button_t*)active_control_ptr;
        active_button_ptr->_.bIsCheckButton = button_ptr->_.bIsChecked;
    } else if (button_ptr == &__designer_ptr->tools.cont_attrs.auto_size) {
        ASSERT(tgui_designer_is_container(active_control_ptr));
        vsf_tgui_container_t* active_container_ptr = (vsf_tgui_container_t*)active_control_ptr;
        active_container_ptr->ContainerAttribute.bIsAutoSize = button_ptr->_.bIsChecked;

        vk_tgui_update_tree(__designer_ptr->gui_ptr, (vsf_tgui_control_t*)active_control_ptr->parent_ptr);
    } else if (button_ptr == &__designer_ptr->tools.cont_attrs.show_corner) {
        ASSERT(tgui_designer_is_container(active_control_ptr));
        vsf_tgui_container_t* active_container_ptr = (vsf_tgui_container_t*)active_control_ptr;
        active_container_ptr->use_as__vsf_tgui_v_container_t.bIsShowCornerTile = button_ptr->_.bIsChecked;
    } else if (button_ptr == &__designer_ptr->tools.cont_attrs.refresh_whole) {
        ASSERT(tgui_designer_is_container(active_control_ptr));
        vsf_tgui_container_t* active_container_ptr = (vsf_tgui_container_t*)active_control_ptr;
        active_container_ptr->ContainerAttribute.is_forced_to_refresh_whole_background = button_ptr->_.bIsChecked;
    } else if ( (&__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array[0].button <= button_ptr) &&
                (button_ptr <= &__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array[dimof(__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array) - 1].button)){
        tgui_designer_msg_button_t* msg_button_ptr = (tgui_designer_msg_button_t*)button_ptr;
        int offset = msg_button_ptr - &__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array[0];

        vsf_tgui_user_evt_handler* msgmap_ptr = __designer_control_get_msgmaps(active_control_ptr);
        msgmap_ptr[offset].msg = msg_button_ptr->desinger_msg;
        msgmap_ptr[offset].u10EvtMask = 0x3FF;
    } else if (button_ptr == &__designer_ptr->code_gen) {
        for (int i = 0; i <= __designer_ptr->ops.current; i++) {
            vsf_tgui_control_t* control_ptr = __designer_ptr->ops.items[i].control_ptr;
            control_ptr->tMSGMap.ptItems = __designer_control_get_msgmaps(control_ptr);
            control_ptr->tMSGMap.chCount = dimof(__designer_ptr->tools.ctrl_attrs.msgmaps.btn_array);
            control_ptr->tMSGMap.name_ptr = NULL;
        }


        static char __declaration_buffer[100 * 1024];
        static char __definition_buffer[100 * 1024];
        memset(__declaration_buffer, 0x0, sizeof(__declaration_buffer));
        memset(__definition_buffer, 0x0, sizeof(__definition_buffer));
        vsf_tgui_control_tree_dump(APP_TGUI_PANEL, APP_TGUI_ALL_ATTRS,
                                   __declaration_buffer, sizeof(__declaration_buffer),
                                   __definition_buffer, sizeof(__definition_buffer));

        printf("%s\n/****** declaration lenght: %8d bytes ******/\n", __declaration_buffer, strlen(__declaration_buffer));
        printf("%s\n/******  definition length: %8d bytes ******/\n", __definition_buffer, strlen(__definition_buffer));

        for (int i = 0; i <= __designer_ptr->ops.current; i++) {
            vsf_tgui_control_t* control_ptr = __designer_ptr->ops.items[i].control_ptr;
            control_ptr->tMSGMap.ptItems = __on_design_msg;
            control_ptr->tMSGMap.chCount = dimof(__on_design_msg);
            control_ptr->tMSGMap.name_ptr = "__on_design_msg";
        }

        return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
    } else {
        ASSERT(0);
    }

    vk_tgui_update(__designer_ptr->gui_ptr, (vsf_tgui_control_t*)active_control_ptr->parent_ptr);
    vk_tgui_refresh(__designer_ptr->gui_ptr);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}

static fsm_rt_t __on_tool_lable_gesture_wheel(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)
{
    ASSERT(control_ptr != NULL);
    ASSERT(msg_ptr != NULL);

    tgui_designer_int_label_t* label_ptr = (tgui_designer_int_label_t*)control_ptr;
    vsf_tgui_control_t* active_control_ptr = __designer_ptr->cur_ctrl_ptr;
    vsf_tgui_evt_t* event_ptr = (vsf_tgui_evt_t*)msg_ptr;
    int16_t offset = event_ptr->PointerEvt.iY * APP_TGUI_DESIGNER_WHELL_HALF_UNIT;

    int value = __label_update_offset(label_ptr, offset);
    uint8_t* buf = (uint8_t*)active_control_ptr + label_ptr->offset;
    memcpy(buf, &value, label_ptr->item_size);

    vk_tgui_update(__designer_ptr->gui_ptr, (vsf_tgui_control_t*)active_control_ptr->parent_ptr);
    vk_tgui_refresh(__designer_ptr->gui_ptr);

    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
}
#endif


/* EOF */
