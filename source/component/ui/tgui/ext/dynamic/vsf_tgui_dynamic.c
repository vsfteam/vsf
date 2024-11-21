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

#include "../../vsf_tgui.h"

#if VSF_USE_TINY_GUI == ENABLED && VSF_TGUI_CFG_DYNAMIC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_tgui_control_info_t {
    uint16_t size;
} vsf_tgui_control_info_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vsf_tgui_control_info_t __vsf_tgui_controls[] = {
    [VSF_TGUI_COMPONENT_ID_CONTROL] = {
        .size = sizeof(vsf_tgui_control_t),
    },
    [VSF_TGUI_COMPONENT_ID_LABEL] = {
        .size = sizeof(vsf_tgui_label_t),
    },
    [VSF_TGUI_COMPONENT_ID_BUTTON] = {
        .size = sizeof(vsf_tgui_button_t),
    },
    [VSF_TGUI_COMPONENT_ID_CONTAINER] = {
        .size = sizeof(vsf_tgui_container_t),
    },
    [VSF_TGUI_COMPONENT_ID_TEXT_LIST] = {
        .size = sizeof(vsf_tgui_text_list_t),
    },
    [VSF_TGUI_COMPONENT_ID_LIST] = {
        .size = sizeof(vsf_tgui_list_t),
    },
    [VSF_TGUI_COMPONENT_ID_PANEL] = {
        .size = sizeof(vsf_tgui_panel_t),
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

vsf_tgui_control_t * vsf_tgui_control_new(vsf_tree_msg_node_id_t id, char *name)
{
    VSF_TGUI_ASSERT((id >= 0) && (id < dimof(__vsf_tgui_controls)));
    VSF_TGUI_ASSERT(name != NULL);

    int size = __vsf_tgui_controls[id].size;

#if VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING == ENABLED
    vsf_tgui_control_t *control = vsf_tgui_malloc(size + strlen(name) + 1);
    char *name_buff = (char *)((uintptr_t)control + size);
#else
    vsf_tgui_control_t *control = vsf_tgui_malloc(size);
#endif
    if (control != NULL) {
        memset(control, 0, size);
#if VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING == ENABLED
        strcpy(name_buff, name);
        control->node_name_ptr = name_buff;
#endif

        control->id = id;
        control->bIsEnabled = true;
        control->bIsVisible = true;
        control->show_corner_tile = false;
        control->tile_trans_rate = 0xFF;
        control->background_color = VSF_TGUI_CFG_SV_CONTROL_BACKGROUND_COLOR;
    }
    return control;
}

void vsf_tgui_control_destroy(vsf_tgui_control_t *control)
{
    vsf_tgui_free(control);
}

void vsf_tgui_control_add(vsf_tgui_container_t *parent, vsf_tgui_control_t *control_prev, vsf_tgui_control_t *control)
{
    vsf_tgui_container_t *parent_real = parent->id == VSF_TGUI_COMPONENT_ID_LIST ? (vsf_tgui_container_t *)parent->node_ptr : parent;

    control->parent_ptr = &parent_real->use_as__vsf_msgt_container_t;
    if (NULL == control_prev) {
        if (parent_real->node_ptr != NULL) {
            control->Offset.next = (intptr_t)parent_real->node_ptr - (intptr_t)control;
        }
        parent_real->node_ptr = &control->use_as__vsf_msgt_node_t;
    } else {
        if (0 == control_prev->Offset.next) {
            control->Offset.next = 0;
        } else {
            control->Offset.next = (intptr_t)control_prev + control_prev->Offset.next - (intptr_t)control;
        }
        control_prev->Offset.next = (intptr_t)control - (intptr_t)control_prev;
    }
}

void vsf_tgui_control_remove(vsf_tgui_container_t *parent, vsf_tgui_control_t *control)
{
    vsf_tgui_container_t *parent_real = parent->id == VSF_TGUI_COMPONENT_ID_LIST ? (vsf_tgui_container_t *)parent->node_ptr : parent;
    vsf_msgt_node_t *node_prev = parent_real->node_ptr;
    vsf_msgt_node_t *node_next = (vsf_msgt_node_t *)((intptr_t)control + control->Offset.next);

    if ((uintptr_t)node_prev == (uintptr_t)control) {
        parent_real->node_ptr = node_next;
        node_prev = NULL;
    } else {
#if VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST == ENABLED
        node_prev = (vsf_msgt_node_t *)((intptr_t)control + control->Offset.previous);
#else
        vsf_msgt_node_t *node_tmp = node_prev;
        do {
            node_prev = node_tmp;
            node_tmp = (vsf_msgt_node_t *)((intptr_t)node_prev + node_prev->Offset.next);
        } while (node_tmp != &control->use_as__vsf_msgt_node_t);
#endif
    }

    if (node_prev != NULL) {
        node_prev->Offset.next = !node_next ? 0 : (intptr_t)node_next - (intptr_t)node_prev;
    }
}

void vsf_tgui_control_sync(vsf_tgui_container_t *parent, vsf_tgui_control_t *control)
{
    const vsf_tgui_root_container_t *top_container_ptr = vk_tgui_control_get_top(control);
    vk_tgui_send_message(top_container_ptr->gui_ptr, (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_ON_LOAD,
        .target_ptr = control,
    });
    vk_tgui_send_message(top_container_ptr->gui_ptr, (vsf_tgui_evt_t) {
        .msg = VSF_TGUI_EVT_UPDATE,
        .target_ptr = &parent->use_as__vsf_tgui_control_t,
    });
}

// container

vsf_tgui_container_t * vsf_tgui_container_new(char *name)
{
    vsf_tgui_container_t *container = (vsf_tgui_container_t *)vsf_tgui_control_new(VSF_TGUI_COMPONENT_ID_CONTAINER, name);
    if (container != NULL) {
        container->is_container = true;
        container->ContainerAttribute.bIsAutoSize = true;
        container->background_color = VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR;
    }
    return container;
}

// label

vsf_tgui_label_t * vsf_tgui_label_new(char *name)
{
    vsf_tgui_label_t *label = (vsf_tgui_label_t *)vsf_tgui_control_new(VSF_TGUI_COMPONENT_ID_LABEL, name);
    if (label != NULL) {
        label->show_corner_tile = true;
        label->background_color = VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR;
        label->font_color = VSF_TGUI_CFG_SV_LABEL_TEXT_COLOR;
    }
    return label;
}

void vsf_tgui_label_set_text_static(vsf_tgui_label_t *label, const char *text)
{
    label->tLabel.tString.pstrText = text;
    if (NULL == text) {
        label->tLabel.tString.s16_size = 0;
    } else {
        label->tLabel.tString.s16_size = strlen(text);
    }
    label->tLabel.bIsChanged = true;
}

const char * vsf_tgui_label_get_text(vsf_tgui_label_t *label)
{
    return label->tLabel.tString.pstrText;
}

// button

vsf_tgui_button_t * vsf_tgui_button_new(char *name)
{
    vsf_tgui_button_t *button = (vsf_tgui_button_t *)vsf_tgui_control_new(VSF_TGUI_COMPONENT_ID_BUTTON, name);
    if (button != NULL) {
        button->show_corner_tile = true;
        button->background_color = VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR;
        button->font_color = VSF_TGUI_CFG_SV_BUTTON_TEXT_COLOR;
    }
    return button;
}

#endif

/* EOF */
