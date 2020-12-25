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

#ifndef __VSF_TGUI_CONTROLS_PANEL_H__
#define __VSF_TGUI_CONTROLS_PANEL_H__

/*============================ INCLUDES ======================================*/
#include "./__vsf_tgui_controls_common.h"
#include "./vsf_tgui_label.h"
#include "./vsf_tgui_control.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_TGUI_CONTROLS_PANEL_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_TGUI_CONTROLS_PANEL_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CONTROLS_PANEL_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_TGUI_CONTROLS_PANEL_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#   define __VSF_TGUI_INTERFACE_CONTROLS_PANEL           {                      \
            .msg_handler = {                                                    \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_panel_msg_handler,           \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __tgui_set_internal_panel( __OWNER_ADDR,                                \
                                __MEMBER,                                       \
                                __PREVIOUS,                                     \
                                __NEXT, ...)                                    \
            (*__OWNER_ADDR) tgui_panel(                                         \
                                __MEMBER,                                       \
                                (__OWNER_ADDR),                                 \
                                __PREVIOUS,                                     \
                                __NEXT,                                         \
                                __VA_ARGS__)

#define tgui_set_internal_panel( __OWNER_ADDR,                                  \
                                __MEMBER,                                       \
                                __PREVIOUS,                                     \
                                __NEXT, ...)                                    \
        __tgui_set_internal_panel(   (__OWNER_ADDR),                            \
                                    __MEMBER,                                   \
                                    __PREVIOUS,                                 \
                                    __NEXT,                                     \
                                    __VA_ARGS__)


#define tgui_set_priv_panel( __OWNER_ADDR ,                                     \
                                __MEMBER,                                       \
                                ...)                                            \
        __tgui_set_internal_panel(   (__OWNER_ADDR),                            \
                                    __MEMBER,                                   \
                                    __MEMBER,                                   \
                                    __MEMBER,                                   \
                                    __VA_ARGS__)


#define __declare_tgui_panel(__NAME)                                            \
    typedef struct __NAME __NAME;
#define declare_tgui_panel(__NAME)      __declare_tgui_panel(__NAME)

#define __def_tgui_panel(__NAME, ...)                                           \
    struct __NAME {                                                             \
        implement(vsf_tgui_panel_t)                                             \
                                                                                \
        union {                                                                 \
            vsf_msgt_node_t __NAME##_FirstNode ;                                \
            struct {                                                            \
                __VA_ARGS__;                                                    \
            };                                                                  \
        };

#define def_tgui_panel(__NAME, ...)     __def_tgui_panel(__NAME, __VA_ARGS__)

#define end_def_tgui_panel(...)                                                 \
    };
                                            
#define __describ_tgui_panel(__NAME, __VAR, ...)                                \
        describe_tgui_container_base(                                           \
            __VAR,                                                              \
            VSF_TGUI_COMPONENT_ID_PANEL,                                        \
            __NAME,                                                             \
            VSF_TGUI_V_PANEL_STATIC_INIT_DEFAULT                                \
            __VA_ARGS__                                                         \
            VSF_TGUI_V_PANEL_STATIC_INIT_OVERRIDE                               \
            .is_top = true,                                                     \
        )  


#define describ_tgui_panel(__NAME, __VAR, ...)                                  \
            __describ_tgui_panel(__NAME, __VAR, __VA_ARGS__)

#define __use_tgui_panel(__NAME, ...)                                           \
    struct {                                                                    \
        implement(vsf_tgui_panel_t)                                             \
                                                                                \
        union {                                                                 \
            vsf_msgt_node_t __NAME##_FirstNode ;                                \
            struct {                                                            \
                __VA_ARGS__;                                                    \
            };                                                                  \
        };                                                                      \
    } __NAME;

#define use_tgui_panel(__NAME, ...) __use_tgui_panel(__NAME, ##__VA_ARGS__)


#define __tgui_panel(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)            \
        tgui_container_base(                                                    \
                        __NAME,                                                 \
                        VSF_TGUI_COMPONENT_ID_PANEL,                            \
                        vsf_tgui_panel_t,                                       \
                        (__PARENT_ADDR),                                        \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        VSF_TGUI_V_PANEL_STATIC_INIT_DEFAULT                    \
                        __VA_ARGS__                                             \
                        VSF_TGUI_V_PANEL_STATIC_INIT_OVERRIDE                   \
                        .is_top = false,                                        \
                    )

#define tgui_panel(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)              \
            __tgui_panel(   __NAME,                                             \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

/*============================ TYPES =========================================*/

declare_class(vsf_tgui_panel_t)

def_class(vsf_tgui_panel_t,
    which(
        implement(vsf_tgui_root_container_t)
        implement(vsf_tgui_v_panel_t)
    )

    //! \note make sure vsf_tgui_container_t must be the last member
    public_member(
        vsf_tgui_text_info_t tTitle;
    )

#if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
    protected_member(
        vsf_tgui_label_t     tTitleLabel;
    )
#endif
)

end_def_class(vsf_tgui_panel_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
fsm_rt_t vk_tgui_panel_init(vsf_tgui_panel_t* ptPanel);

extern
fsm_rt_t vk_tgui_panel_update(vsf_tgui_panel_t* ptPanel);

extern
fsm_rt_t vsf_tgui_panel_msg_handler( vsf_tgui_panel_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG);
#endif
/* EOF */
