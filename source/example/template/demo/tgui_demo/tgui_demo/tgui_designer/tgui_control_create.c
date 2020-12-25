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
/*============================ MACROS ========================================*/
#define VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST \
            (__VSF_TGUI_COMPONENT_TYPE_NUM + 0)
#define VSF_TGUI_COMPONENT_ID_LABEL_IN_TEXT_LIST \
            (__VSF_TGUI_COMPONENT_TYPE_NUM + 1)
#define VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST \
            (__VSF_TGUI_COMPONENT_TYPE_NUM + 2)
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_tgui_panel(tgui_dync_template_t)

def_tgui_panel(tgui_dync_template_t,
    tgui_contains(
        vsf_tgui_control_t   control;
        vsf_tgui_label_t     label;
        vsf_tgui_button_t    button;
        vsf_tgui_text_list_t text_list;
        use_tgui_container(container)
        use_tgui_panel(panel)
        use_tgui_list(list)
	))
end_def_tgui_panel(tgui_dync_template_t)

/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
static tgui_dync_template_t __template;

static const struct {
    vsf_tgui_control_t* control;
    uint16_t size;
} __controls[] = {
    [VSF_TGUI_COMPONENT_ID_CONTROL] = {(vsf_tgui_control_t*)&__template.control, sizeof(vsf_tgui_control_t) },
    [VSF_TGUI_COMPONENT_ID_LABEL] = {(vsf_tgui_control_t*)&__template.label, sizeof(vsf_tgui_label_t) },
    [VSF_TGUI_COMPONENT_ID_BUTTON] = {(vsf_tgui_control_t*)&__template.button, sizeof(vsf_tgui_button_t) },
    [VSF_TGUI_COMPONENT_ID_CONTAINER] = {(vsf_tgui_control_t*)&__template.container, sizeof(vsf_tgui_container_t) },
    [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = {(vsf_tgui_control_t*)&__template.text_list, sizeof(vsf_tgui_text_list_t) },
    [VSF_TGUI_COMPONENT_ID_LIST] = {(vsf_tgui_control_t*)&__template.list, sizeof(vsf_tgui_list_t)},
    [VSF_TGUI_COMPONENT_ID_PANEL] = {(vsf_tgui_control_t*)&__template.panel, sizeof(vsf_tgui_panel_t) },
    [VSF_TGUI_COMPONENT_ID_LABEL_IN_TEXT_LIST] = {(vsf_tgui_control_t*)&__template.text_list.tList.tContent, 0},
    [VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST] = {(vsf_tgui_control_t*)&__template.text_list.tList, 0},
    [VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST] = {(vsf_tgui_control_t*)&__template.list.list, sizeof(vsf_tgui_container_t) },
};
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
static const struct {
    char* str;
    uint8_t len;
} __controls_name[] = {
    [VSF_TGUI_COMPONENT_ID_CONTROL]                = {"[vsf_tgui_control_t][%s]",   sizeof("[vsf_tgui_control_t][]") },
    [VSF_TGUI_COMPONENT_ID_LABEL]                  = {"[vsf_tgui_label_t][%s]",     sizeof("[vsf_tgui_label_t][]") },
    [VSF_TGUI_COMPONENT_ID_BUTTON]                 = {"[vsf_tgui_button_t][%s]",    sizeof("[vsf_tgui_button_t][]") },
    [VSF_TGUI_COMPONENT_ID_CONTAINER]              = {"[vsf_tgui_container_t][%s]", sizeof("[vsf_tgui_container_t][]"), },
    [VSF_TGUI_COMPONENT_ID_TEXT_LIST]              = {"[vsf_tgui_text_list_t][%s]", sizeof("[vsf_tgui_text_list_t][]"), },
    [VSF_TGUI_COMPONENT_ID_LIST]                   = {"[vsf_tgui_list_t][%s]",      sizeof("[vsf_tgui_list_t][]"), },
    [VSF_TGUI_COMPONENT_ID_PANEL]                  = {"[vsf_tgui_panel_t][%s]",     sizeof("[vsf_tgui_panel_t][]"), },
    [VSF_TGUI_COMPONENT_ID_LABEL_IN_TEXT_LIST]     = {"[vsf_tgui_label_t][tContent]", sizeof("[vsf_tgui_text_list_t][tContent]") },
    [VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST] = {"[vsf_tgui_text_list_t][%s.tList]", sizeof("[vsf_tgui_text_list_t][.tList]"), },
    [VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST]      = {"[vsf_tgui_list_t][%s.list]",      sizeof("[vsf_tgui_list_t][.list]"), },
};
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_tgui_create_init(vsf_tgui_t* gui_ptr)
{
    describ_tgui_panel(tgui_dync_template_t, __template,
        tgui_region(0, 0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX),
        tgui_attribute(bIsAutoSize, false),

        tgui_control(control, &__template, control, label,
            tgui_region(0, 0, 20, 20),
        ),

        tgui_label(label, &__template, control, button,
            tgui_attribute(bIsUseRawView, true),
            tgui_region(0, 0, 100, 30),
            tgui_text(tLabel, "label", false),
        ),

        tgui_button(button, &__template, label, text_list,
            tgui_attribute(bIsUseRawView, true),
            tgui_region(0, 0, 100, 30),
            tgui_text(tLabel, "button", false),
        ),

        tgui_text_list(text_list, &__template, button, container,
            tgui_region(0, 0, 100, 150),
            tgui_margin(8, 0, 8, 0),

            tgui_text_list_content(
                tgui_size(100, 0),
                tgui_text(tLabel, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", true),
                tgui_line_space(tLabel, 8),
                tgui_attribute(tFontColor, VSF_TGUI_COLOR_GRAY),
            )
        ),

        tgui_panel(panel, &__template, text_list, container,
            tgui_attribute(bIsAutoSize, false),
            tgui_attribute(bIsShowCornerTile, false),
            tgui_region(0, 0, 150, 150),
        ),

        tgui_container(container, &__template, text_list, list,
            tgui_region(0, 00, 100, 100),
            tgui_attribute(bIsAutoSize, false),
        ),

        tgui_list(list, &__template, list, list,
            tgui_region(0, 0, 100, 32),
            tgui_attribute(bIsAutoSize, false),

            tgui_margin(0, 2, 0, 2),
            tgui_attribute(u2WorkMode, VSF_TGUI_LIST_MODE_ITEM_SELECTION),

            tgui_list_items(
                //tgui_container_type(VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL)
                tgui_container_type(VSF_TGUI_CONTAINER_TYPE_PLANE),
            )
        ),
    );

    __template.container.node_ptr = NULL;
}

static void __vsf_tgui_control_init(vsf_tgui_container_t* parent_ptr, vsf_tgui_control_t* control_ptr, vsf_tree_msg_node_id_t id, char* node_name_ptr, bool first)
{
    VSF_TGUI_ASSERT(parent_ptr != NULL);
    VSF_TGUI_ASSERT(control_ptr != NULL);

    vsf_msgt_container_t* parent_node_ptr = &parent_ptr->use_as__vsf_msgt_container_t;
    VSF_TGUI_ASSERT(parent_node_ptr->Attribute._.is_container);

    memcpy(control_ptr, __controls[id].control, __controls[id].size);
    control_ptr->parent_ptr = parent_node_ptr;
    control_ptr->node_name_ptr = node_name_ptr;
    if (control_ptr->Attribute._.is_container) {
        vsf_msgt_container_t* container_node_ptr = (vsf_msgt_container_t*)control_ptr;
        container_node_ptr->node_ptr = NULL;
    }

    vsf_msgt_node_t* node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
    vsf_msgt_node_t* item_ptr = parent_node_ptr->node_ptr;
    if (item_ptr == NULL) {
        parent_node_ptr->node_ptr = node_ptr;
        node_ptr->Offset.next = 0;
        return ;
    }

    if (first) {
        parent_node_ptr->node_ptr = node_ptr;
        node_ptr->Offset.next = (intptr_t)item_ptr - (intptr_t)node_ptr;
    } else {
        do {
            vsf_msgt_node_t* next_ptr = (vsf_msgt_node_t *)vsf_msgt_get_next_node_within_container(item_ptr);
            if (next_ptr == NULL) {
                item_ptr->Offset.next = (intptr_t)node_ptr - (intptr_t)item_ptr;
                node_ptr->Offset.next = 0;
                break;
            }
            item_ptr = next_ptr;
        } while (1);
    }
}

vsf_tgui_control_t* vsf_tgui_control_create(vsf_tgui_container_t* parent_ptr, vsf_tree_msg_node_id_t id, char* control_name, bool first, uint16_t head_size)
{
    VSF_TGUI_ASSERT(parent_ptr != NULL);
    VSF_TGUI_ASSERT(0 <= id && id < __VSF_TGUI_COMPONENT_TYPE_NUM);
    VSF_TGUI_ASSERT(control_name != NULL);

    if (parent_ptr->id == VSF_TGUI_COMPONENT_ID_LIST) {
        parent_ptr = (vsf_tgui_container_t*)parent_ptr->node_ptr; // create new item in list, list is the grandfather of item
    }

    // memory layout:
    //     n byte head
    //     componment
    //         child componment
    //     string with '\0'
    //         child string with '\0'
    int name_len = strlen(control_name);

    int full_name_len = __controls_name[id].len + name_len;
    int child_name_len = 0;
    int child_control_size = 0;
    if (id == VSF_TGUI_COMPONENT_ID_LIST) {
        child_name_len = __controls_name[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST].len + name_len;
        child_control_size = __controls[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST].size;
    } else if (id == VSF_TGUI_COMPONENT_ID_TEXT_LIST) {
        child_name_len = __controls_name[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST].len + name_len;
    }
    int alloc_size = head_size + __controls[id].size + child_control_size + full_name_len + child_name_len; // control name include '\0'

    uint8_t * buf = vsf_heap_malloc(alloc_size);
    memset(buf, 0, head_size);
    buf += head_size;

    char* name_buf = (char*)(buf + __controls[id].size + child_control_size);
    snprintf(name_buf, full_name_len, __controls_name[id].str, control_name);

    vsf_tgui_control_t* control_ptr = (vsf_tgui_control_t *)buf;
    __vsf_tgui_control_init(parent_ptr, control_ptr, id, name_buf, first);

    if (id == VSF_TGUI_COMPONENT_ID_LIST) {
        char* child_name_buf = (char*)(name_buf + full_name_len);
        snprintf(child_name_buf, child_name_len, __controls_name[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST].str, control_name);

        vsf_tgui_list_t* list = (vsf_tgui_list_t*)control_ptr;
        //vsf_tgui_control_t* container = vsf_heap_malloc(__controls[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST].size);
        vsf_tgui_control_t* container = (vsf_tgui_control_t*)(buf + __controls[id].size);
        __vsf_tgui_control_init(&list->use_as__vsf_tgui_container_t, container, VSF_TGUI_COMPONENT_ID_CONTAINER_IN_LIST, child_name_buf, first);
    } else if (id == VSF_TGUI_COMPONENT_ID_TEXT_LIST) {
        char* child_name_buf = (char*)(name_buf + full_name_len);
        snprintf(child_name_buf, child_name_len, __controls_name[VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST].str, control_name);

        vsf_tgui_text_list_t* text_list = (vsf_tgui_text_list_t*)control_ptr;
        vsf_tgui_container_t* tlist = &text_list->tList.use_as__vsf_tgui_container_t;
        __vsf_tgui_control_init((vsf_tgui_container_t *)text_list, (vsf_tgui_control_t *)tlist, VSF_TGUI_COMPONENT_ID_CONTAINER_IN_TEXT_LIST, child_name_buf, first);
        __vsf_tgui_control_init(tlist, (vsf_tgui_control_t*)&text_list->tList.tContent, VSF_TGUI_COMPONENT_ID_LABEL_IN_TEXT_LIST, __controls_name[VSF_TGUI_COMPONENT_ID_LABEL_IN_TEXT_LIST].str, first);
    }

    const vsf_tgui_root_container_t* top_container_ptr = vk_tgui_control_get_top((vsf_tgui_control_t*)parent_ptr);
    bool result;
    (void)result;
    result = vk_tgui_send_message(top_container_ptr->gui_ptr,
                                  (vsf_tgui_evt_t) { .msg = VSF_TGUI_EVT_ON_LOAD, .target_ptr = control_ptr});
    ASSERT(result);
    result = vk_tgui_send_message(top_container_ptr->gui_ptr,
                                  (vsf_tgui_evt_t) { .msg = VSF_TGUI_EVT_UPDATE, .target_ptr = (vsf_tgui_control_t*)parent_ptr});
    ASSERT(result);

    return control_ptr;
}


#endif


/* EOF */
