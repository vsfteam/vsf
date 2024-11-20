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

#ifndef __VSF_TINY_GUI_EXT_DYNAMIC_H__
#define __VSF_TINY_GUI_EXT_DYNAMIC_H__

/*============================ INCLUDES ======================================*/

#if VSF_TGUI_CFG_DYNAMIC == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

// heap

#if !defined(VSF_TGUI_HEAP_HEADER) && VSF_USE_HEAP == ENABLED
#   define VSF_TGUI_HEAP_HEADER             "service/heap/vsf_heap.h"
#   define vsf_tgui_malloc(__size)          vsf_heap_malloc(__size)
#   define vsf_tgui_free(__ptr)             vsf_heap_free(__ptr)
#endif

// control

#define vsf_tgui_control_set_width(__control, __w)                              \
    do {                                                                        \
        (__control)->tSize.iWidth = (__w);                                      \
    } while(0)

#define vsf_tgui_control_set_height(__control, __h)                             \
    do {                                                                        \
        (__control)->tSize.iHeight = (__h);                                     \
    } while(0)

#define vsf_tui_control_set_size(__control, __w, __h)                           \
    do {                                                                        \
        (__control)->tSize.iWidth = (__w);                                      \
        (__control)->tSize.iHeight = (__h);                                     \
    } while(0)

#define vsf_tgui_control_set_margin(__control, __left, __top, __right, __bottom)\
    do {                                                                        \
        (__control)->tMargin.chLeft = (__left);                                 \
        (__control)->tMargin.chTop = (__top);                                   \
        (__control)->tMargin.chRight = (__right);                               \
        (__control)->tMargin.chBottom = (__bottom);                             \
    } while(0)

// container

#define vsf_tgui_container_set_type2(__container, __type, __autosize)           \
    do {                                                                        \
        (__container)->ContainerAttribute.u5Type = (__type),                    \
        (__container)->ContainerAttribute.bIsAutoSize = (__autosize);           \
    } while(0)
#define vsf_tgui_container_set_type1(__container, __type)                       \
    vsf_tgui_container_set_type2((__container), (__type), true)
// protocol: void vsf_tgui_container_set_type(vsf_tgui_container_t *container, int type, bool autosize = true);
#define vsf_tgui_container_set_type(__container, ...)                           \
    __PLOOC_EVAL(vsf_tgui_container_set_type, __VA_ARGS__)((__container), ##__VA_ARGS__)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_tgui_control_t * vsf_tgui_control_new(vsf_tree_msg_node_id_t id, char *name);
extern void vsf_tgui_control_destroy(vsf_tgui_control_t *control);

extern void vsf_tgui_control_add(vsf_tgui_container_t *parent, vsf_tgui_control_t *control_prev, vsf_tgui_control_t *control);
extern void vsf_tgui_control_sync(vsf_tgui_container_t *parent, vsf_tgui_control_t *control);

// container

extern vsf_tgui_container_t * vsf_tgui_container_new(char *name);

// label

extern vsf_tgui_label_t * vsf_tgui_label_new(char *name);
extern void vsf_tgui_label_set_text_static(vsf_tgui_label_t *label, const char *text);
extern const char * vsf_tgui_label_get_text(vsf_tgui_label_t *label);

// buttong

extern vsf_tgui_button_t * vsf_tgui_button_new(char *name);
#define vsf_tgui_button_set_text_static(__button, __text)                      \
        vsf_tgui_label_set_text_static((vsf_tgui_label_t *)(__button), (__text))

#endif
#endif
/* EOF */
