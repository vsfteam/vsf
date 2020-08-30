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

#ifndef __VSF_TGUI_CONTROLS_LIST_H__
#define __VSF_TGUI_CONTROLS_LIST_H__

/*============================ INCLUDES ======================================*/
#include "./__vsf_tgui_controls_common.h"
#include "./vsf_tgui_control.h"
#include "./__vk_tgui_slider.h"

#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_TGUI_CONTROLS_LIST_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_TGUI_CONTROLS_LIST_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CONTROLS_LIST_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_TGUI_CONTROLS_LIST_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"


/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   define __VSF_TGUI_INTERFACE_CONTROLS_LIST           {                       \
            {                                                                   \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_list_msg_handler             \
            },                                                                  \
            (vsf_msgt_method_status_t *)&vsf_tgui_control_status_get,           \
            (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot                  \
        }
#else
#   define __VSF_TGUI_INTERFACE_CONTROLS_LIST           {                       \
            .msg_handler = {                                                \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_list_msg_handler,            \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED

#   define __tgui_list(     __NAME,                                             \
                            __PARENT_ADDR,                                      \
                            __PREVIOUS,                                         \
                            __NEXT, ...)                                        \
            tgui_control_base(   __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_LIST,                         \
                            vsf_tgui_list_t,                                    \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            VSF_TGUI_V_LIST_STATIC_INIT_DEFAULT                 \
                            __VA_ARGS__                                         \
                            VSF_TGUI_V_LIST_STATIC_INIT_OVERRIDE                \
                                                                                \
                            .is_container = true,                               \
                            .node_ptr =                                           \
                                (vsf_msgt_node_t*)                              \
                                    ((__PARENT_ADDR)->__NAME.ptList),           \
                            .node_name_ptr =                                      \
                                "[vsf_tgui_list_t]["#__NAME"]",                 \
                            .u5Type = VSF_TGUI_CONTAINER_TYPE_PLANE,            \
                            .bIsAutoSize = false,                               \
                            .bIsHideContentInsideContainer = false,             \
                                                                                \
                            .list.parent_ptr =                                    \
                                (vsf_msgt_container_t *)&((__PARENT_ADDR)->     \
                                    __NAME.use_as__vsf_msgt_node_t),            \
                            .list.id = VSF_TGUI_COMPONENT_ID_CONTAINER,       \
                            .list.Offset = {0},                                 \
                            .list.is_container = true,                          \
                            .list.is_control_transparent = true,                 \
                            .list.bIsAutoSize = true,                           \
                            .list.bIsEnabled = true,                            \
                            .list.bIsVisible = true,                            \
                            .list.node_ptr =                                      \
                                (vsf_msgt_node_t*)                              \
                                    &((__PARENT_ADDR)->                         \
                                        __NAME.list.list_FirstNode),            \
                            .list.node_name_ptr =                                 \
                                "[vsf_tgui_list_t]["#__NAME".list]",            \
                        )

#else
#   define __tgui_list(     __NAME,                                             \
                            __PARENT_ADDR,                                      \
                            __PREVIOUS,                                         \
                            __NEXT, ...)                                        \
            tgui_control_base(   __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_LIST,                         \
                            vsf_tgui_list_t,                                    \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            VSF_TGUI_V_LIST_STATIC_INIT_DEFAULT                 \
                            __VA_ARGS__                                         \
                            VSF_TGUI_V_LIST_STATIC_INIT_OVERRIDE                \
                                                                                \
                            .is_container = true,                               \
                            .node_ptr =                                           \
                                (vsf_msgt_node_t*)                              \
                                    ((__PARENT_ADDR)->__NAME.ptList),           \
                            .u5Type = VSF_TGUI_CONTAINER_TYPE_PLANE,            \
                            .bIsAutoSize = false,                               \
                            .bIsHideContentInsideContainer = false,             \
                                                                                \
                            .list.parent_ptr =                                    \
                                (vsf_msgt_container_t *)&((__PARENT_ADDR)->     \
                                    __NAME.use_as__vsf_msgt_node_t),            \
                            .list.id = VSF_TGUI_COMPONENT_ID_CONTAINER,       \
                            .list.Offset = {0},                                 \
                            .list.is_container = true,                          \
                            .list.is_control_transparent = true,                 \
                            .list.bIsAutoSize = true,                           \
                            .list.bIsEnabled = true,                            \
                            .list.bIsVisible = true,                            \
                            .list.node_ptr =                                      \
                                (vsf_msgt_node_t*)                              \
                                    &((__PARENT_ADDR)->                         \
                                        __NAME.list.list_FirstNode),            \
                        )
#endif

#define tgui_list(  __NAME,                                                     \
                    __PARENT_ADDR,                                              \
                    __PREVIOUS,                                                 \
                    __NEXT,                                                     \
                        ...)                                                    \
            __tgui_list(    __NAME,                                             \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#   define tgui_list_items(...)             .list = {__VA_ARGS__},


#define __use_tgui_list(__NAME, ...)                                            \
    struct {                                                                    \
        implement(vsf_tgui_list_t);                                             \
        use_tgui_container(list, __VA_ARGS__)                                   \
    } __NAME;

#define use_tgui_list(__NAME, ...) __use_tgui_list(__NAME, __VA_ARGS__)



#endif

/*============================ TYPES =========================================*/

typedef enum {
    VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM      ,
    VSF_TGUI_LIST_MODE_FREE_MOVE                    ,   /* reserve for future */
    VSF_TGUI_LIST_MODE_ITEM_SELECTION               ,   /* reserve for future */
    VSF_TGUI_LIST_MODE_ITEM_SELECTION_CENTER_ALIGN  ,   /* reserve for future */
} vsf_tgui_list_work_mode_t;

typedef struct vsf_tgui_list_scrollbar_region_t {
    vsf_tgui_region_t tTrack;
    vsf_tgui_region_t tBar;
} vsf_tgui_list_scrollbar_region_t;

declare_class(vsf_tgui_list_t)

def_class(vsf_tgui_list_t,
    which(
        implement(vsf_tgui_container_t)
        implement(vsf_tgui_v_list_t)
    )
#if VSF_TGUI_CFG_LIST_SUPPORT_SLIDE == ENABLED
    union {
        private_member(
            implement(__vk_tgui_slider_t)
            /* use vsf_tgui_list_work_mode_t value*/
            struct {
                uint8_t u2WorkMode          : 2;
                uint8_t bIsSliding          : 1;
                uint8_t                     : 5;
            }tMode;
            union {
                uint8_t chStartIndex;
                uint8_t chSelectIndex;
            };
        )
        struct {
            inherit_ex(__vk_tgui_slider_t, tSlider)
            /* use vsf_tgui_list_work_mode_t value*/
            uint8_t u2WorkMode              : 2;
        };
    };
#else
    union {
        private_member(
            /* use vsf_tgui_list_work_mode_t value*/
            struct {
                uint8_t u2WorkMode          : 2;
                uint8_t bIsSliding          : 1;
                uint8_t                     : 5;
            }tMode;
            union {
                uint8_t chStartIndex;
                uint8_t chSelectIndex;
            };
        )
        struct {
            /* use vsf_tgui_list_work_mode_t value*/
            uint8_t u2WorkMode              : 2;
        };
    };
#endif
    vsf_tgui_container_t ptList[];
)

end_def_class(vsf_tgui_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
fsm_rt_t vk_tgui_list_update(vsf_tgui_list_t* ptList);

extern
fsm_rt_t vk_tgui_list_init(vsf_tgui_list_t* ptList);

extern
fsm_rt_t vsf_tgui_list_msg_handler( vsf_tgui_list_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG);

#if VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR == ENABLED
SECTION(".text.vsf.component.tgui.vsf_tgui_list_scrollbar_regions_generate")
extern
vsf_tgui_list_scrollbar_region_t * vsf_tgui_list_scrollbar_regions_generate(
                            const vsf_tgui_control_t* control_ptr,
                            const vsf_tgui_list_t* ptList, 
                            uint_fast8_t chScalingRatio, 
                            vsf_tgui_list_scrollbar_region_t *ptOutputBuffer);
#endif

/*----------------------------------------------------------------------------*
 *  APIs for VSF_TGUI_LIST_MODE_FREE_MOVE_STICK_TO_ITEM mode                  *
 *----------------------------------------------------------------------------*/
extern
int_fast16_t vk_tgui_list_display_item_start_get(vsf_tgui_list_t* ptList);

extern
int_fast16_t vk_tgui_list_display_item_start_set(vsf_tgui_list_t* ptList,
                                                 uint_fast8_t chStartIndex);

/*----------------------------------------------------------------------------*
 *  APIs for VSF_TGUI_LIST_MODE_ITEM_SELECTION(_*) mode                       *
 *----------------------------------------------------------------------------*/
extern
int_fast16_t vk_tgui_list_selected_item_get(vsf_tgui_list_t* ptList);

extern
int_fast16_t vk_tgui_list_selected_item_set(vsf_tgui_list_t* ptList,
                                            uint_fast8_t chSelectIndex);

#endif
#endif
/* EOF */
