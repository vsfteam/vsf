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

#if VSF_USE_TINY_GUI == ENABLED && APP_USE_TGUI_DESIGNER_DEMO == ENABLED
#include <stdio.h>
#include "./tgui_designer_common.h"
/*============================ MACROS ========================================*/
#define __DUMP_SNPRINTF(__buffer, __buffer_size, __offset_ptr, ...) \
    (*__offset_ptr) += snprintf(&__buffer[*__offset_ptr], \
                                     __buffer_size - *__offset_ptr, \
                                     __VA_ARGS__); \
    VSF_TGUI_ASSERT(*__offset_ptr < __buffer_size)

#define TOP_DEF_SNPRINTF(...)        \
    __DUMP_SNPRINTF(__tree_dump.panel_definition_code, sizeof(__tree_dump.panel_definition_code), &__tree_dump.panel_definition_offset, __VA_ARGS__)

#define TOP_DECL_SNPRINTF(...) \
    __DUMP_SNPRINTF(__tree_dump.panel_declaration_code, sizeof(__tree_dump.panel_declaration_code), &__tree_dump.panel_declaration_offset, __VA_ARGS__)

#define MSG_DECL_SNPRINTF(...) \
    __DUMP_SNPRINTF(__tree_dump.msgmap_declaration_code, sizeof(__tree_dump.msgmap_declaration_code), &__tree_dump.msgmap_declaration_offset, __VA_ARGS__)

#define MSG_DEF_SNPRINTF(...) \
    __DUMP_SNPRINTF(__tree_dump.msgmap_definition_code, sizeof(__tree_dump.msgmap_definition_code), &__tree_dump.msgmap_definition_offset, __VA_ARGS__)


#define SPACE_CNT_IN_TAB           4


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void code_dump_handler_t(vsf_tgui_control_t* control_ptr,
                                 uint_fast8_t tab_cnt, bool force, char* top_container_name);

typedef struct vsf_tgui_control_tree_dump_t {
    size_t msgmap_declaration_offset;
    size_t msgmap_definition_offset;
    size_t panel_declaration_offset;
    size_t panel_definition_offset;
    char msgmap_declaration_code[100 * 1024];
    char msgmap_definition_code[100 * 1024];
    char panel_declaration_code[100 * 1024];
    char panel_definition_code[100 * 1024];
} vsf_tgui_control_tree_dump_t;
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
vsf_tgui_control_tree_dump_t __tree_dump;
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static const char* __control_get_msgmap_name(vsf_tgui_control_t* control_ptr)
{
    static char __msgmap_name_buffer[128];

    if (control_ptr->tMSGMap.name_ptr != NULL) {
        return control_ptr->tMSGMap.name_ptr;
    } else {
        snprintf(__msgmap_name_buffer, sizeof(__msgmap_name_buffer),
            "%s_msgmap", vsf_tgui_control_get_var_name(control_ptr));
        return __msgmap_name_buffer;
    }
}

static const char* __control_get_user_evt_handler_name(vsf_tgui_control_t* control_ptr, const vsf_tgui_user_evt_handler* handler)
{
    static char __msgmap_name_buffer[128];

    if (handler->handler_name_ptr != NULL) {
        return handler->handler_name_ptr;
    } else {
        snprintf(__msgmap_name_buffer, sizeof(__msgmap_name_buffer),
            "__on_%s_%s", vsf_tgui_control_get_var_name(control_ptr), vsf_tgui_msg_evt_to_short_name(handler->msg));
        return __msgmap_name_buffer;
    }
}

static void __control_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_control_t */\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    // TODO : use HOR and VER macro
    const vsf_tgui_region_t* control_region = &control_ptr->tRegion;
    if (  force ||
          control_region->tLocation.iX != 0 || control_region->tLocation.iY != 0 ||
          control_region->tSize.iWidth != 0 || control_region->tSize.iHeight != 0) {
        TOP_DEF_SNPRINTF("%*ctgui_region(/*iX*/%d, /*iY*/%d, /*iWidth*/%d, /*iHeight*/%d),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ',
            control_region->tLocation.iX, control_region->tLocation.iY,
            control_region->tSize.iWidth, control_region->tSize.iHeight);
    }

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR == ENABLED
#   error "TODO"
#endif

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK == ENABLED
#   error "TODO"
#endif

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
    const vsf_tgui_margin_t* margin = &control_ptr->tMargin;
    if (  force ||
          margin->chLeft != 0 || margin->chTop != 0 ||
          margin->chRight != 0 || margin->chBottom != 0) {
        TOP_DEF_SNPRINTF("%*ctgui_margin(/*left*/%d, /*top*/%d, /*right*/%d, /*bottom*/%d),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', margin->chLeft, margin->chTop, margin->chRight, margin->chBottom);
    }
#endif

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN == ENABLED
#   error "TODO"
#endif

    if (control_ptr->tBackground.ptTile != NULL) {
        TOP_DEF_SNPRINTF("%*ctgui_background((vsf_tgui_tile_t *)&%s, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', vsf_tgui_tile_get_var_name(control_ptr->tBackground.ptTile), vsf_tgui_align_mode_to_string(control_ptr->tBackground.tAlign));
    } else if (force) {
        TOP_DEF_SNPRINTF("%*ctgui_background(NULL, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', vsf_tgui_align_mode_to_string(control_ptr->tBackground.tAlign));
    }

    if (control_ptr->tMSGMap.chCount != 0 || control_ptr->tMSGMap.ptItems != NULL) {
        VSF_TGUI_ASSERT(control_ptr->tMSGMap.chCount != 0);
        VSF_TGUI_ASSERT(control_ptr->tMSGMap.ptItems != NULL);
        const char* msgmap_name = __control_get_msgmap_name(control_ptr);
        bool is_need = false;

        for (int i = 0; i < control_ptr->tMSGMap.chCount; i++) {
            const vsf_tgui_user_evt_handler* handler = &control_ptr->tMSGMap.ptItems[i];
            if (handler->msg < VSF_TGUI_MSG_AVAILABLE) {
                continue;
            }
            is_need = true;
            const char* evt_handler_name = __control_get_user_evt_handler_name(control_ptr, handler);
            if (strstr(__tree_dump.msgmap_declaration_code, evt_handler_name) == NULL) {
                char* code;
                if ((handler->msg == VSF_TGUI_EVT_ON_LOAD) && (tab_cnt == 2)) { // todo
                    code = "    vsf_tgui_control_refresh(control_ptr, NULL);\n";
                } else {
                    code = "";
                }
                MSG_DEF_SNPRINTF("static fsm_rt_t %s(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr)\n{\n%s    return (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;\n}\n\n",
                    evt_handler_name, code);
                MSG_DECL_SNPRINTF("static fsm_rt_t %s(vsf_tgui_control_t* control_ptr, vsf_msgt_msg_t* msg_ptr);\n", evt_handler_name);
            }
        }

        if (is_need && strstr(__tree_dump.msgmap_declaration_code, msgmap_name) == NULL) {
            TOP_DEF_SNPRINTF("%*ctgui_msgmap(%s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', msgmap_name);
            MSG_DECL_SNPRINTF("static const describe_tgui_msgmap(%s,\n", msgmap_name);
            for (int i = 0; i < control_ptr->tMSGMap.chCount; i++) {
                const vsf_tgui_user_evt_handler* handler = &control_ptr->tMSGMap.ptItems[i];
                if (handler->msg < VSF_TGUI_MSG_AVAILABLE) {
                    continue;
                }

                const char* evt_str = vsf_tgui_msg_evt_to_string(handler->msg);
                const char* evt_handler_name = __control_get_user_evt_handler_name(control_ptr, handler);
                const char* evt_mask_str = vsf_tgui_evt_mask_to_string(handler->u10EvtMask);
                MSG_DECL_SNPRINTF("    tgui_msg_handler(%s, %s%s),\n", evt_str, evt_handler_name, evt_mask_str);
            }
            MSG_DECL_SNPRINTF(")\n\n");
        }
    }

    // control_ptr->bIsEnabled init by macro __tgui_control_base
    // control_ptr->bIsVisible init by macro __tgui_control_base

    if (force || control_ptr->bIsHideContentInsideContainer) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsHideContentInsideContainer, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', control_ptr->bIsHideContentInsideContainer ? "true" : "false");
    }

    if (force || control_ptr->is_control_transparent) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(is_control_transparent, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', control_ptr->is_control_transparent ? "true" : "false");
    }

    if (force || control_ptr->tBackgroundColor.tColor.Value != 0) {
        const char *color_buffer = vsf_tgui_color_to_string(control_ptr->tBackgroundColor);
        TOP_DEF_SNPRINTF("%*ctgui_attribute(tBackgroundColor, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', color_buffer);
    }
}

static void __label_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __control_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_label_t */\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    vsf_tgui_label_t* label = (vsf_tgui_label_t*)control_ptr;

    if (force || label->bIsUseRawView) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsUseRawView, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', label->bIsUseRawView ? "true" : "false");
    }

    if (force || label->bIsNoBackgroundColor) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsNoBackgroundColor, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', label->bIsNoBackgroundColor ? "true" : "false");
    }

    if (force || label->chFontIndex != 0) {
        // TODO : using font name
        TOP_DEF_SNPRINTF("%*ctgui_attribute(chFontIndex, %s),\n",
            tab_cnt * SPACE_CNT_IN_TAB, ' ', vsf_tgui_font_get_var_name(label->chFontIndex));
    }

    if (force || label->tFontColor.tColor.Value != 0) {
        const char *color_buffer = vsf_tgui_color_to_string(label->tFontColor);
        TOP_DEF_SNPRINTF("%*ctgui_attribute(tFontColor, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', color_buffer);
    }

    if (force || label->tLabel.tString.pstrText != NULL || label->tLabel.u4Align != VSF_TGUI_ALIGN_CENTER) {
        char __string_buffer[1024];
        if (label->tLabel.tString.pstrText != NULL) {
            memset(__string_buffer, 0x0, sizeof(__string_buffer));
            vsf_tgui_string_to_c_lang_format(__string_buffer, label->tLabel.tString.pstrText);
        } else {
            memcpy(__string_buffer, "Text", sizeof("Text"));
        }
        TOP_DEF_SNPRINTF("%*ctgui_text(tLabel, \"%s\", %s, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            __string_buffer, label->tLabel.bIsAutoSize ? "true" : "false", vsf_tgui_align_mode_to_string(label->tLabel.u4Align));
    }

    if (force || label->tLabel.chInterLineSpace != 0) {
        TOP_DEF_SNPRINTF("%*ctgui_line_space(tLabel, %d),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', label->tLabel.chInterLineSpace);
    }
}

static void __button_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __label_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_button_t */\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    vsf_tgui_button_t* button = (vsf_tgui_button_t*)control_ptr;

    if (force || button->bIsCheckButton) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsCheckButton, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', button->bIsCheckButton ? "true" : "false");
    }

    if (force || button->bIsChecked) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsChecked, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', button->bIsChecked ? "true" : "false");
    }

    if (force || button->bIsAllowEmphasize) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsAllowEmphasize, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', button->bIsAllowEmphasize ? "true" : "false");
    }

    if (force || button->bIsEmphasized) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsEmphasized, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', button->bIsEmphasized ? "true" : "false");
    }
}

static void __container_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __control_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_container_t*/\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    vsf_tgui_container_t* container = (vsf_tgui_container_t*)control_ptr;

    // container->use_as__vsf_msgt_container_t init by macro

#if VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
    if (force || container->bIsShowCornerTile) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsShowCornerTile, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->bIsShowCornerTile ? "true" : "false");
    }
    if (force || container->bIsNoBackgroundColor) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsNoBackgroundColor, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->bIsNoBackgroundColor ? "true" : "false");
    }
    if (force || container->bIsTileTransparency) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(bIsTileTransparency, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->bIsTileTransparency ? "true" : "false");
    }
    if (force || container->chTileTransparencyRate != 0xFF) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(chTileTransparencyRate, 0x%02X),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->chTileTransparencyRate);
    }
#endif

    // container->u5Type init by macro
    // container->bIsAutoSize init by macro
    if (force || container->bIsAutoSize || container->u5Type) {
        TOP_DEF_SNPRINTF("%*ctgui_container_type(%s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', vsf_tgui_container_type_to_string(container->u5Type));
    }

    if (force || container->is_forced_to_refresh_whole_background) {
        TOP_DEF_SNPRINTF("%*ctgui_attribute(is_forced_to_refresh_whole_background, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->is_forced_to_refresh_whole_background ? "true" : "false");
    }

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    if (  force ||
          container->tContainerPadding.chTop != 0 || container->tContainerPadding.chBottom != 0 ||
          container->tContainerPadding.chLeft != 0 || container->tContainerPadding.chRight != 0) {
        TOP_DEF_SNPRINTF("%*ctgui_padding(/*left*/%d, /*top*/%d, /*right*/%d, /*bottom*/%d),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            container->tContainerPadding.chLeft, container->tContainerPadding.chTop,
            container->tContainerPadding.chRight, container->tContainerPadding.chBottom);
    }
#endif
}

static void __panel_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __container_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_panel_t*/\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    vsf_tgui_panel_t* panel = (vsf_tgui_panel_t*)control_ptr;

    if (force || panel->tTitle.tString.pstrText != NULL || panel->tTitle.u4Align != VSF_TGUI_ALIGN_CENTER) {
        const char* str = (panel->tTitle.tString.pstrText != NULL) ? panel->tTitle.tString.pstrText : "Title";
        TOP_DEF_SNPRINTF("%*ctgui_text(tTitle, \"%s\", %s, %s),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ',
            str, panel->tTitle.bIsAutoSize ? "true" : "false", vsf_tgui_align_mode_to_string(panel->tTitle.u4Align));
    }
}

static void __list_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __container_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_list_t */\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    vsf_tgui_list_t* list = (vsf_tgui_list_t*)control_ptr;

    const char* str = vsf_tgui_list_work_mode_to_string(list->u2WorkMode);
    TOP_DEF_SNPRINTF("%*ctgui_attribute(u2WorkMode, %s),\n\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', str);
}

static void __text_list_code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force, char* top_container_name)
{
    __container_code_dump(control_ptr, tab_cnt, force, top_container_name);

    TOP_DEF_SNPRINTF("%*c/* vsf_tgui_text_list_t*/\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

    //vsf_tgui_text_list_t* text_list = (vsf_tgui_text_list_t*)control_ptr;
}

static void __control_gen_define_header(vsf_tgui_control_t* control_ptr, uint_fast8_t *current_tab_cnt_ptr,
                                        char* control_name, char* parent_name, char* previous_name, char* next_name)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(control_name != NULL);

    vsf_msgt_node_t* node = &control_ptr->use_as__vsf_msgt_node_t;

    static const char* __header_names[] = {
        [VSF_TGUI_COMPONENT_ID_CONTROL] = "control",
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = "container",
        [VSF_TGUI_COMPONENT_ID_LABEL] = "label",
        [VSF_TGUI_COMPONENT_ID_BUTTON] = "button",
        [VSF_TGUI_COMPONENT_ID_PANEL] = "panel",
        [VSF_TGUI_COMPONENT_ID_LIST] = "list",
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = "text_list",
    };

    static const char* __header_decl_names[] = {
        [VSF_TGUI_COMPONENT_ID_CONTROL] = "vsf_tgui_control_t ",
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = "use_tgui_container(",
        [VSF_TGUI_COMPONENT_ID_LABEL] = "vsf_tgui_label_t ",
        [VSF_TGUI_COMPONENT_ID_BUTTON] = "vsf_tgui_button_t ",
        [VSF_TGUI_COMPONENT_ID_PANEL] = "use_tgui_panel(",
        [VSF_TGUI_COMPONENT_ID_LIST] = "use_tgui_list(",
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = "vsf_tgui_text_list_t ",
    };
    static const char* __header_decl_end_strs[] = {
        [VSF_TGUI_COMPONENT_ID_CONTROL] = "",
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = ",\n",
        [VSF_TGUI_COMPONENT_ID_LABEL] = "",
        [VSF_TGUI_COMPONENT_ID_BUTTON] = "",
        [VSF_TGUI_COMPONENT_ID_PANEL] = ",\n",
        [VSF_TGUI_COMPONENT_ID_LIST] = ", tgui_contains(\n",
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = "",
    };

    if (*current_tab_cnt_ptr == 0) {
        if (node->id == VSF_TGUI_COMPONENT_ID_PANEL) {
            TOP_DEF_SNPRINTF("\n%s_t* my_%s_init(%s_t * %s, vsf_tgui_t * gui_ptr)\n{"
                             "\n%*cif (NULL == %s && NULL != gui_ptr) {\n%*creturn NULL;\n%*c}\n\n"
                             "%*cdescrib_tgui_panel(%s_t, *%s,\n",
                             control_name, control_name, control_name, control_name,
                             SPACE_CNT_IN_TAB, ' ', control_name, 8, ' ', SPACE_CNT_IN_TAB, ' ',
                             (*current_tab_cnt_ptr + 1) * SPACE_CNT_IN_TAB, ' ', control_name, control_name);

            TOP_DECL_SNPRINTF("declare_tgui_panel(%s_t)\n\ndef_tgui_panel(%s_t,\n%*ctgui_contains(\n",
                            control_name, control_name, SPACE_CNT_IN_TAB, ' ');
        } else {
            VSF_TGUI_ASSERT(0);
        }
        *current_tab_cnt_ptr = 2;
    } else {
        VSF_TGUI_ASSERT(parent_name != NULL);
        VSF_TGUI_ASSERT(previous_name != NULL);
        VSF_TGUI_ASSERT(next_name != NULL);

        TOP_DEF_SNPRINTF("\n%*ctgui_%s(%s, %s, %s, %s,\n",
            (*current_tab_cnt_ptr) * SPACE_CNT_IN_TAB, ' ', __header_names[node->id], control_name, parent_name, previous_name, next_name);

        TOP_DECL_SNPRINTF("%*c%s%s%s", (*current_tab_cnt_ptr) * SPACE_CNT_IN_TAB, ' ', __header_decl_names[node->id], control_name, __header_decl_end_strs[node->id]);

        *current_tab_cnt_ptr += 1;
    }
}

static void __control_gen_define_tail(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, char* control_name)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(control_name != NULL);

    vsf_msgt_node_t* node = &control_ptr->use_as__vsf_msgt_node_t;
    if (tab_cnt == 0) {
        if (node->id == VSF_TGUI_COMPONENT_ID_PANEL) {
            TOP_DEF_SNPRINTF("%*c);\n\n%*creturn %s;\n}\n\n", SPACE_CNT_IN_TAB, ' ', SPACE_CNT_IN_TAB, ' ', control_name);

            TOP_DECL_SNPRINTF("%*c))\n\nend_def_tgui_panel(%s)\n\n", SPACE_CNT_IN_TAB, ' ', control_name);
        } else {
            VSF_TGUI_ASSERT(0);
        }
    } else {
        TOP_DEF_SNPRINTF("%*c),\n", tab_cnt * SPACE_CNT_IN_TAB, ' ');

        static const char* __tail_decl_names[] = {
            [VSF_TGUI_COMPONENT_ID_CONTROL] = ";",
            [VSF_TGUI_COMPONENT_ID_CONTAINER] = ")",
            [VSF_TGUI_COMPONENT_ID_LABEL] = "; ",
            [VSF_TGUI_COMPONENT_ID_BUTTON] = ";",
            [VSF_TGUI_COMPONENT_ID_PANEL] = ")",
            [VSF_TGUI_COMPONENT_ID_LIST] = "))",
            [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = ";",
        };

        if (__tail_decl_names[node->id][0] == ';') {
            TOP_DECL_SNPRINTF("%s\n", __tail_decl_names[node->id]);
        } else {
            TOP_DECL_SNPRINTF("%*c%s\n", tab_cnt * SPACE_CNT_IN_TAB, ' ', __tail_decl_names[node->id]);
        }
    }
}

static void __control_gen_struct_init(vsf_tgui_control_t* control_ptr, uint_fast8_t current_tab_cnt, bool force, char* control_name)
{
    vsf_msgt_node_t* node = &control_ptr->use_as__vsf_msgt_node_t;
    static code_dump_handler_t* const __code_dump_handlers[] = {
        [VSF_TGUI_COMPONENT_ID_CONTROL] = __control_code_dump,
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = __container_code_dump,
        [VSF_TGUI_COMPONENT_ID_LABEL] = __label_code_dump,
        [VSF_TGUI_COMPONENT_ID_BUTTON] = __button_code_dump,
        [VSF_TGUI_COMPONENT_ID_PANEL] = __panel_code_dump,
        [VSF_TGUI_COMPONENT_ID_LIST] = __list_code_dump,
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = __text_list_code_dump,
    };

    code_dump_handler_t* handler = __code_dump_handlers[node->id];
    VSF_TGUI_ASSERT(handler != NULL);
    handler(control_ptr, current_tab_cnt, force, control_name);
}

static void __container_gen_header(vsf_msgt_node_t* node, uint_fast8_t tab_cnt,uint_fast8_t *current_tab_cnt_ptr)
{
    static const char * __container_items[] = {
        [VSF_TGUI_COMPONENT_ID_CONTAINER] = "contains",
        [VSF_TGUI_COMPONENT_ID_PANEL] = "contains",
        [VSF_TGUI_COMPONENT_ID_LIST] = "list_items",
        [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = "text_list_content",
    };

    if (tab_cnt != 0 || node->id != VSF_TGUI_COMPONENT_ID_PANEL) {
        const char* items_str = __container_items[node->id];
        VSF_TGUI_ASSERT(items_str != NULL);
        TOP_DEF_SNPRINTF("%*ctgui_%s(\n",
            (*current_tab_cnt_ptr) * SPACE_CNT_IN_TAB, ' ', items_str);
        (*current_tab_cnt_ptr)++;
    }
}

static void __container_gen_tail(vsf_msgt_node_t* node, uint_fast8_t tab_cnt, uint_fast8_t current_tab_cnt)
{
    if (tab_cnt != 0 || node->id != VSF_TGUI_COMPONENT_ID_PANEL) {
        TOP_DEF_SNPRINTF("%*c)\n", current_tab_cnt * SPACE_CNT_IN_TAB, ' ');
    }
}

static void __control_get_parent_var_name(char* parent_name_buffer, vsf_msgt_node_t* node, char *control_name, char* parent_name)
{
    if (parent_name == NULL) {                     // current top
        strcat(parent_name_buffer, control_name);
    } else {
        if (parent_name[0] != '&') {                // parent is top
            strcat(parent_name_buffer, "&(");
            strcat(parent_name_buffer, parent_name);
            strcat(parent_name_buffer, "->");
        } else {
            strcpy(parent_name_buffer, parent_name);
            char* right_brackets = strchr(parent_name_buffer, ')'); // search second [
            VSF_TGUI_ASSERT(right_brackets != NULL);
            *right_brackets = '\0';
            strcat(parent_name_buffer, ".");
        }
        strcat(parent_name_buffer, control_name);

        if (node->id == VSF_TGUI_COMPONENT_ID_LIST) {
            strcat(parent_name_buffer, ".list");
        }
        strcat(parent_name_buffer, ")");
    }
}

static void __code_dump(vsf_tgui_control_t* control_ptr, uint_fast8_t tab_cnt, bool force,
                        char* parent_name, char* previous_name, char* next_name)
{

    uint_fast8_t current_tab_cnt = tab_cnt;

    char control_name[128];
    strcpy(control_name, vsf_tgui_control_get_var_name(control_ptr));

    __control_gen_define_header(control_ptr, &current_tab_cnt, control_name, parent_name, previous_name, next_name);
    __control_gen_struct_init(control_ptr, current_tab_cnt, force, control_name);

    // init childs
    do {
        vsf_msgt_node_t* node = &control_ptr->use_as__vsf_msgt_node_t;
        if (node->Attribute._.is_container) {
            __container_gen_header(node, tab_cnt, &current_tab_cnt);

            if (node->id == VSF_TGUI_COMPONENT_ID_TEXT_LIST) {
                // text list only dump test_list->list's structure member
                vsf_tgui_text_list_t* text_list = (vsf_tgui_text_list_t*)control_ptr;
                vsf_tgui_control_t* label_control = &text_list->tList.tContent.use_as__vsf_tgui_control_t;
                __control_gen_struct_init(label_control, current_tab_cnt, force, control_name);
            } else {
                const vsf_msgt_container_t* container_node_ptr = (const vsf_msgt_container_t*)node;

                if (node->id == VSF_TGUI_COMPONENT_ID_LIST) {
                    container_node_ptr = (const vsf_msgt_container_t*)container_node_ptr->node_ptr;
                    __control_gen_struct_init((vsf_tgui_control_t*)container_node_ptr, current_tab_cnt, force, control_name);
                }

                char parent_name_buffer[256] = {'\0'};
                __control_get_parent_var_name(parent_name_buffer, node, control_name, parent_name);

                vsf_tgui_control_t* current_item_ptr = (vsf_tgui_control_t *)container_node_ptr->node_ptr;
                vsf_tgui_control_t* prev_item_ptr = current_item_ptr;

                while (NULL != current_item_ptr) {
                    vsf_tgui_control_t* next_item_ptr = (vsf_tgui_control_t*)vsf_msgt_get_next_node_within_container(&current_item_ptr->use_as__vsf_msgt_node_t);

                    char preverious_name_buffer[128];
                    char next_name_buffer[128];
                    strcpy(preverious_name_buffer, vsf_tgui_control_get_var_name(prev_item_ptr));
                    strcpy(next_name_buffer, vsf_tgui_control_get_var_name(next_item_ptr == NULL ? current_item_ptr : next_item_ptr));

                    __code_dump(current_item_ptr,
                                current_tab_cnt, force,
                                parent_name_buffer, preverious_name_buffer, next_name_buffer);

                    prev_item_ptr = current_item_ptr;
                    current_item_ptr = next_item_ptr;
                }
            }

            __container_gen_tail(node, tab_cnt, current_tab_cnt - 1);
        }
    } while (0);

    // end
    __control_gen_define_tail(control_ptr, tab_cnt, control_name);
}

void vsf_tgui_control_tree_dump(vsf_tgui_panel_t* panel_ptr, bool force,
                                char *declaration_buffer, size_t declaration_buffer_size,
                                char *definition_buffer, size_t definition_buffer_size)
{
    VSF_TGUI_ASSERT(NULL != panel_ptr);
    VSF_TGUI_ASSERT(NULL != declaration_buffer);
    VSF_TGUI_ASSERT(NULL != definition_buffer);
    VSF_TGUI_ASSERT(declaration_buffer_size > 0);
    VSF_TGUI_ASSERT(definition_buffer_size > 0);

    memset(&__tree_dump, 0x0, sizeof(__tree_dump));


    __code_dump((vsf_tgui_control_t*)panel_ptr, 0, force, NULL, NULL, NULL);

    ASSERT(strlen(declaration_buffer) < declaration_buffer_size);
    strcat(declaration_buffer, __tree_dump.panel_declaration_code);

    ASSERT((strlen(__tree_dump.msgmap_declaration_code) + strlen(__tree_dump.panel_definition_code) + strlen(__tree_dump.msgmap_definition_code)) < definition_buffer_size);
    strcat(definition_buffer, __tree_dump.msgmap_declaration_code);
    strcat(definition_buffer, __tree_dump.panel_definition_code);
    strcat(definition_buffer, __tree_dump.msgmap_definition_code);
}

#endif


/* EOF */
