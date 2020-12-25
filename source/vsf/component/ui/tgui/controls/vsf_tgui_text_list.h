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

/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#ifndef __VSF_TGUI_CONTROLS_TEXT_LIST_H__
#define __VSF_TGUI_CONTROLS_TEXT_LIST_H__

/*============================ INCLUDES ======================================*/
#include "./__vsf_tgui_controls_common.h"
#include "./vsf_tgui_list.h"
#include "./vsf_tgui_label.h"
#include "./__vk_tgui_slider.h"

#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#define __VSF_TGUI_INTERFACE_CONTROLS_TEXT_LIST           {                     \
            .msg_handler = {                                                    \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_text_list_msg_handler,       \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }

/*============================ MACROFIED FUNCTIONS ===========================*/


#define __tgui_text_list(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)        \
            tgui_control_base(   __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_TEXT_LIST,                    \
                            vsf_tgui_text_list_t,                               \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT                        \
                                                                                \
                tgui_container( tList, &((__PARENT_ADDR)->__NAME), tList, tList,\
                    __tgui_name_string_tag(__NAME.tList, vsf_tgui_text_list_t)  \
                ),                                                              \
                                                                                \
                VSF_TGUI_V_TEST_LIST_STATIC_INIT_DEFAULT                        \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_TEST_LIST_STATIC_INIT_OVERRIDE                       \
                .tList.tContent.parent_ptr = (vsf_msgt_container_t *)           \
                                &((__PARENT_ADDR)->__NAME.tList),               \
                .tList.tContent.id = VSF_TGUI_COMPONENT_ID_LABEL,               \
                .tList.tContent.Offset = {                                      \
                    .next =  sizeof(vsf_tgui_label_t),                          \
                },                                                              \
                .tList.tContent                                                 \
                    __tgui_name_string_tag(tContent, vsf_tgui_label_t)          \
                .tList.tContent.bIsEnabled = true,                              \
                .tList.tContent.bIsVisible = true,                              \
                .tList.tContent.tLabel.bIsAutoSize = true,                      \
                                                                                \
                .tList.is_control_transparent = true,                           \
                .tList.bIsEnabled = true,                                       \
                .tList.bIsVisible = true,                                       \
                .tList.ContainerAttribute                                       \
                    .u5Type = VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL,     \
                .tList.ContainerAttribute                                       \
                    .bIsAutoSize = true,                                        \
                .is_container = true,                                           \
                .node_ptr =                                                     \
                    (vsf_msgt_node_t*)                                          \
                        &((__PARENT_ADDR)->__NAME.tList),                       \
                .ContainerAttribute                                             \
                    .u5Type = VSF_TGUI_CONTAINER_TYPE_PLANE,                    \
                .bIsHideContentInsideContainer = true,                          \
                VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE                       \
            )                                                                   


#define tgui_text_list(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)          \
            __tgui_text_list(   __NAME,                                         \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#   define tgui_text_list_content(...)                                          \
                .tList.tContent = {                                             \
                    VSF_TGUI_V_LABEL_STATIC_INIT_DEFAULT                        \
                    __VA_ARGS__                                                 \
                    VSF_TGUI_V_LABEL_STATIC_INIT_OVERRIDE                       \
                },

/*============================ TYPES =========================================*/

declare_class(vsf_tgui_text_list_t)

def_class(vsf_tgui_text_list_t,
    which(
        implement(vsf_tgui_container_t)
        implement(vsf_tgui_v_text_list_t)
    )


    use_tgui_container(tList,
        public_member(
            vsf_tgui_label_t tContent;
        )
        private_member(
            vsf_tgui_label_t tBuffer;
        )
    )

#if VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE == ENABLED
    union {
        private_member(
            implement(__vk_tgui_slider_t);
            int16_t  iOldLineSelect;
            uint16_t hwLineCount;
            int16_t  iLineSelect;
        )
        inherit_ex(__vk_tgui_slider_t, tSlider);
    };
#else
    private_member(
        uint16_t hwLineCount;
        int16_t  iLineSelect;
    )
#endif

)
end_def_class(vsf_tgui_text_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
fsm_rt_t vsf_tgui_text_list_msg_handler(vsf_tgui_text_list_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG);
extern
fsm_rt_t vk_tgui_text_list_init(vsf_tgui_text_list_t* ptLabel);

extern
fsm_rt_t vk_tgui_text_list_update(vsf_tgui_text_list_t* ptLabel);

extern
int_fast16_t vsf_tgui_text_list_select_get(vsf_tgui_text_list_t* ptTextList);

extern
void vsf_tgui_text_list_select_set( vsf_tgui_text_list_t* ptTextList,
                                    int_fast16_t iSelect);

#endif
#endif
/* EOF */

