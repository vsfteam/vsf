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

#ifndef __VSF_TGUI_CONTROLS_CONTROL_H__
#define __VSF_TGUI_CONTROLS_CONTROL_H__

/*============================ INCLUDES ======================================*/
#include "./__vsf_tgui_controls_common.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_TGUI_CONTROLS_CONTROL_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef __VSF_TGUI_CONTROLS_CONTROL_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __VSF_TGUI_INTERFACE_CONTROLS_CONTROL         {                         \
            {                                                                   \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_control_msg_handler          \
            },                                                                  \
            (vsf_msgt_method_status_t *)&vsf_tgui_control_status_get,           \
            (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot                  \
        }

#define __VSF_TGUI_INTERFACE_CONTROLS_CONTAINER           {                     \
            {                                                                   \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_container_msg_handler        \
            },                                                                  \
            (vsf_msgt_method_status_t *)&vsf_tgui_control_status_get,           \
            (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot                  \
        }
#else
#define __VSF_TGUI_INTERFACE_CONTROLS_CONTROL         {                         \
            .tMessageHandler = {                                                \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_control_msg_handler,         \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }

#define __VSF_TGUI_INTERFACE_CONTROLS_CONTAINER           {                     \
            .tMessageHandler = {                                                \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_container_msg_handler,       \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }
#endif

#define VSF_TGUI_CTRL_STATUS_INITIALISED                _BV(0)
#define VSF_TGUI_CTRL_STATUS_ENABLED                    _BV(1)
#define VSF_TGUI_CTRL_STATUS_VISIBLE                    _BV(2)
#define VSF_TGUI_CTRL_STATUS_ACTIVE                     _BV(3)
#define VSF_TGUI_CTRL_STATUS_HIDE_CONTAINER_CONTENT     _BV(4)

/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#if VSF_MSG_TREE_CFG_SUPPORT_DUAL_LIST == ENABLED
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   define __tgui_control_base( __NAME,                                         \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME =  {                                                        \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iPrevious =  (intptr_t)&((__PARENT_ADDR)->__NAME)          \
                                - (intptr_t)&((__PARENT_ADDR)->__PREVIOUS),     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .pchNodeName = "["#__TYPE"]["#__NAME"]",                        \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#   define __tgui_control_base_const( __NAME,                                   \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME = (__TYPE) {                                                \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iPrevious =  (intptr_t)&((__PARENT_ADDR)->__NAME)          \
                                - (intptr_t)&((__PARENT_ADDR)->__PREVIOUS),     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .pchNodeName = "["#__TYPE"]["#__NAME"]",                        \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#else
#   define __tgui_control_base( __NAME,                                         \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME =  {                                                        \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iPrevious =  (intptr_t)&((__PARENT_ADDR)->__NAME)          \
                                - (intptr_t)&((__PARENT_ADDR)->__PREVIOUS),     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#   define __tgui_control_base_const( __NAME,                                   \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME = (__TYPE){                                                 \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iPrevious =  (intptr_t)&((__PARENT_ADDR)->__NAME)          \
                                - (intptr_t)&((__PARENT_ADDR)->__PREVIOUS),     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }
#endif
#else
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   define __tgui_control_base( __NAME,                                         \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME =  {                                                        \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .pchNodeName = "["#__TYPE"]["#__NAME"]",                        \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#   define __tgui_control_base_const( __NAME,                                   \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME = (__TYPE) {                                                \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .pchNodeName = "["#__TYPE"]["#__NAME"]",                        \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#else
#   define __tgui_control_base( __NAME,                                         \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME =  {                                                        \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }

#   define __tgui_control_base_const( __NAME,                                   \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            .__NAME = (__TYPE){                                                 \
                .ptParent = (vsf_msgt_container_t *)                            \
                                &((__PARENT_ADDR)->use_as__vsf_msgt_node_t),    \
                .chID = (__ID),                                                 \
                .Offset = {                                                     \
                    .iNext =  (intptr_t)&((__PARENT_ADDR)->__NEXT)              \
                            - (intptr_t)&((__PARENT_ADDR)->__NAME),             \
                },                                                              \
                .bIsEnabled = true,                                             \
                .bIsVisible = true,                                             \
                VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                          \
                __VA_ARGS__                                                     \
                VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE                         \
            }
#endif
#endif


#define tgui_control_base(   __NAME,                                            \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            __tgui_control_base(                                                \
                        __NAME,                                                 \
                        (__ID),                                                 \
                        __TYPE,                                                 \
                        (__PARENT_ADDR),                                        \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        __VA_ARGS__)

#define tgui_control_base_const(   __NAME,                                      \
                        __ID,                                                   \
                        __TYPE,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            __tgui_control_base_const(                                          \
                        __NAME,                                                 \
                        (__ID),                                                 \
                        __TYPE,                                                 \
                        (__PARENT_ADDR),                                        \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        __VA_ARGS__)


#define __tgui_control(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)          \
            tgui_control_base(   __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_CONTROL,                      \
                            vsf_tgui_control_t,                                 \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#define tgui_control(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)            \
            __tgui_control(   __NAME,                                           \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#define __tgui_control_const(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)    \
            tgui_control_base_const(__NAME,                                     \
                            VSF_TGUI_COMPONENT_ID_CONTROL,                      \
                            vsf_tgui_control_t,                                 \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#define tgui_control_const(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)      \
            __tgui_control_const(   __NAME,                                     \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#define __use_tgui_container(__NAME, ...)                                       \
    struct  {                                                                   \
        implement(vsf_tgui_container_t)                                         \
        union {                                                                 \
            vsf_msgt_node_t __NAME##_FirstNode ;                                \
            struct {                                                            \
                __VA_ARGS__;                                                    \
            };                                                                  \
        };                                                                      \
    } __NAME;

#define use_tgui_container(__NAME, ...) __use_tgui_container(__NAME, __VA_ARGS__)

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   define __tgui_container(   __NAME,                                          \
                            __PARENT_ADDR,                                      \
                            __PREVIOUS,                                         \
                            __NEXT, ...)                                        \
            tgui_control_base(   __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_CONTAINER,                    \
                            vsf_tgui_container_t,                               \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            .bIsContainer = true,                               \
                            .ptNode =                                           \
                                (vsf_msgt_node_t*)                              \
                               &((__PARENT_ADDR)->__NAME.__NAME##_FirstNode),   \
                            .pchNodeName =                                      \
                                "[vsf_tgui_container_t]["#__NAME"]",            \
                            .bIsEnabled = true,                                 \
                            .bIsVisible = true,                                 \
                            VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT            \
                            __VA_ARGS__                                         \
                            VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE           \
                            )
#else
#   define __tgui_container(   __NAME,                                          \
                            __PARENT_ADDR,                                      \
                            __PREVIOUS,                                         \
                            __NEXT, ...)                                        \
        tgui_control_base(       __NAME,                                        \
                            VSF_TGUI_COMPONENT_ID_CONTAINER,                    \
                            vsf_tgui_container_t,                               \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            .bIsContainer = true,                               \
                            .ptNode =                                           \
                                (vsf_msgt_node_t*)                              \
                               &((__PARENT_ADDR)->__NAME.__NAME##_FirstNode),   \
                            .bIsEnabled = true,                                 \
                            .bIsVisible = true,                                 \
                            VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT            \
                            __VA_ARGS__                                         \
                            VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE           \
                            )
#endif

#define tgui_container( __NAME,                                                 \
                        __PARENT_ADDR,                                          \
                        __PREVIOUS,                                             \
                        __NEXT,                                                 \
                        ...)                                                    \
            __tgui_container(__NAME,                                            \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#endif

/*============================ TYPES =========================================*/



typedef union vsf_tgui_status_t {
    uint8_t chStatus;
    struct {
        //! \name Status bits
        //! @{
        uint8_t bIsInitialised                  : 1;
        uint8_t bIsEnabled                      : 1;
        uint8_t bIsVisible                      : 1;
        uint8_t bIsActive                       : 1;
        uint8_t bIsHideContentInsideContainer   : 1;
        uint8_t bIsControlTransparent           : 1;
        uint8_t                                 : 2;
        //! @}

        //! \name internal bits
        //! @{
        uint8_t __bIsTheFirstRefreshNode        : 1;
        uint8_t __bContainBuiltInStructure      : 1;
        uint8_t                                 : 6;
        //! @}
    } tValues;
}vsf_tgui_status_t;

declare_class(__vsf_tgui_control_core_t)


typedef struct vsf_tgui_control_subcall_t {
    vsf_param_eda_evthandler_t  *fnSub;
    vsf_tgui_control_t          *ptControl;
} vsf_tgui_control_subcall_t;

typedef fsm_rt_t vsf_tgui_controal_fsm_t(vsf_tgui_control_t* ptNode, vsf_tgui_msg_t* ptMSG);

typedef struct vsf_tgui_control_handler_t {
    uint16_t    u2Type              : 2;                    //!< vsf_msgt_handler_type_t
    uint16_t                        : 4;
    uint16_t    u10EvtMask          : 10;                   //!< 0 means no mask

    implement_ex(
        union {
            vsf_tgui_controal_fsm_t *FSM;
            vsf_eda_t* ptEDA;
            vsf_tgui_control_subcall_t* ptSubCall;
        },
        fn
    )
} vsf_tgui_control_handler_t;

typedef struct vsf_tgui_user_evt_handler {
    implement(vsf_msgt_msg_t)
    implement(vsf_tgui_control_handler_t)
} vsf_tgui_user_evt_handler;


#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
typedef struct vsf_tgui_margin_t {
    uint8_t chLeft;
    uint8_t chTop;
    uint8_t chRight;
    uint8_t chBottom;
} vsf_tgui_margin_t;
#endif

def_class(__vsf_tgui_control_core_t,

    public_member(
        implement_ex(vsf_tgui_region_t, tRegion)

    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR == ENABLED
        vsf_tgui_anchor_mode_t  tAnchor;
    #endif

    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK == ENABLED
        vsf_tgui_dock_mode_t    tDock;
    #endif

    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        vsf_tgui_margin_t       tMargin;
    #endif

    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN == ENABLED
        struct {
            vsf_tgui_control_t *ptAlignTo;

            /* \note Only following mode supported:
                    VSF_TGUI_ALIGN_LEFT     = _BV(0),
                    VSF_TGUI_ALIGN_RIGHT    = _BV(1),
                    VSF_TGUI_ALIGN_TOP      = _BV(2),
                    VSF_TGUI_ALIGN_BOTTOM   = _BV(3),
             */
            vsf_tgui_align_mode_t  tMode;
        } tAlign;
    #endif

        struct {
            const vsf_tgui_tile_t*  ptTile;
            vsf_tgui_align_mode_t   tAlign;
        } tBackground;

        struct {
            const vsf_tgui_user_evt_handler  *ptItems;
            uint8_t chCount;
            uint8_t chState;
            uint8_t chIndex;
        } tMSGMap;
    )

    union {
        protected_member(
            implement_ex(vsf_tgui_status_t, tStatus)
        )
        struct {
            //! \name Status bits
            //! @{
            uint8_t                                 : 1;
            uint8_t bIsEnabled                      : 1;
            uint8_t bIsVisible                      : 1;
            uint8_t                                 : 1;
            uint8_t bIsHideContentInsideContainer   : 1;
            uint8_t bIsControlTransparent           : 1;
            uint8_t                                 : 2;
            //! @}
        };
    };
)
end_def_class(__vsf_tgui_control_core_t)


def_class(vsf_tgui_control_t,
    which(
        implement(vsf_msgt_node_t)
        implement(__vsf_tgui_control_core_t)
        implement(vsf_tgui_v_control_t)
    )
)
end_def_class(vsf_tgui_control_t)

declare_class(vsf_tgui_container_t)

def_class(vsf_tgui_container_t,
    which(
        implement(vsf_msgt_container_t)
        implement(__vsf_tgui_control_core_t)
        implement(vsf_tgui_v_container_t)
    )

    implement_ex(
        struct {
            uint8_t         u5Type                          : 5;    /* vsf_tgui_container_type_t */
            uint8_t         bIsAutoSize                     : 1;
            uint8_t         bIsForceRefreshWholeBackground  : 1;
            uint8_t                                         : 1;
        },
        tContainerAttribute
    )

    protected_member(
        uint8_t chVisibleItemCount;
    )

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    vsf_tgui_margin_t       tConatinerPadding;
#endif


)
end_def_class(vsf_tgui_container_t)

declare_class(vsf_tgui_top_container_t)

def_class(vsf_tgui_top_container_t,
    which(implement(vsf_tgui_container_t))
    public_member(
        vsf_tgui_t* ptGUI;
    )
)

end_def_class(vsf_tgui_top_container_t)


typedef enum vsf_tgui_control_refresh_mode_t {
    VSF_TGUI_CONTROL_REFRESHED_BY_PARENT = 0,
    VSF_TGUI_CONTROL_REFRESHED_DIRECTLY_BY_USER
} vsf_tgui_control_refresh_mode_t;

typedef fsm_rt_t vsf_tgui_method_t  (vsf_tgui_control_t* ptControl);
typedef fsm_rt_t vsf_tgui_v_method_render_t(vsf_tgui_control_t* ptControl,
                                            vsf_tgui_region_t* ptDirtyRegion,
                                            vsf_tgui_control_refresh_mode_t tMode);

typedef struct i_tgui_v_vtable_t{
    vsf_tgui_method_t          *Init;
    vsf_tgui_method_t          *Depose;
    vsf_tgui_v_method_render_t *Render;
    vsf_tgui_v_method_render_t *ContainerPostRender;
    vsf_tgui_method_t          *Update;
}i_tgui_v_vtable_t;

typedef struct i_tgui_control_vtable_t {
    i_tgui_v_vtable_t           tView;
    vsf_tgui_method_t          *Init;
    vsf_tgui_method_t          *Update;
} i_tgui_control_methods_t;

#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
declare_class(vsf_tgui_timer_t)

def_class(vsf_tgui_timer_t,

    private_member(
        implement(vsf_callback_timer_t)
        const vsf_tgui_control_t *ptTarget;
    )

    union {
        public_member(
            uint32_t u29Interval    : 29;
            uint32_t                : 1;
            uint32_t bIsRepeat      : 1;
            uint32_t bEnabled       : 1;
        )
        private_member(
            struct {
                uint32_t u29Interval    : 29;
                uint32_t bIsWorking     : 1;
                uint32_t bIsRepeat      : 1;
                uint32_t bEnabled       : 1;
            }tStatus;
        )
    };
)
end_def_class(vsf_tgui_timer);

#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
extern
void vsf_tgui_timer_init(   vsf_tgui_timer_t *ptTimer,
                            const vsf_tgui_control_t *ptControl);

extern
void vsf_tgui_timer_enable(vsf_tgui_timer_t *ptTimer);

extern
void vsf_tgui_timer_disable(vsf_tgui_timer_t *ptTimer);

extern
bool vsf_tgui_timer_is_working(vsf_tgui_timer_t *ptTimer);
#endif
/*----------------------------------------------------------------------------*
 *  Region                                                                    *
 *----------------------------------------------------------------------------*/
extern 
vsf_tgui_location_t *vsf_tgui_control_get_location(
                                    const vsf_tgui_control_t* ptControl);

extern 
vsf_tgui_size_t *vsf_tgui_control_get_size(
                                    const vsf_tgui_control_t* ptControl);

extern
bool vsf_tgui_control_is_in_range(  const vsf_tgui_region_t *ptRegion,
                                    const vsf_tgui_location_t *ptLocation);

/*! \brief get the absolute location information base on the location information
 *!        of or derived from target control.
 *! \param ptControl    the Target Control Address
 *! \param ptLocation   the Location buffer which has already stored the location
 *!                     information of or derived from the target control
 *! \return the location buffer address passed with ptLocation
 *!
 *! \note DO NOT USE THIS FUNCTION UNLESS YOU KNOW WHAT YOU ARE DOING!!!
 */
extern
vsf_tgui_location_t* __vk_tgui_calculate_absolute_location_from_control_location(
                                                const vsf_tgui_control_t *ptControl,
                                                vsf_tgui_location_t *ptLocation);

extern
vsf_tgui_region_t * vsf_tgui_get_absolute_control_region(
                                                const vsf_tgui_control_t* ptControl,
                                                vsf_tgui_region_t* ptRegionBuffer);

extern
vsf_tgui_location_t * vsf_tgui_control_get_absolute_location(
                                            const vsf_tgui_control_t* ptControl,
                                            vsf_tgui_location_t* ptOffset);

extern
vsf_tgui_region_t* vsf_tgui_control_get_absolute_region(
                                                const vsf_tgui_control_t *ptControl,
                                                vsf_tgui_region_t *ptRegion);

extern
vsf_tgui_region_t *vsf_tgui_control_generate_dirty_region_from_parent_dirty_region(
                                    const vsf_tgui_control_t *ptParent,
                                    const vsf_tgui_region_t *ptParentDirtyRegion,
                                    const vsf_tgui_control_t *ptPrivate,
                                    vsf_tgui_region_t *ptNewDirtyRegionBuffer);



/*! \brief get the visible region (with absolute location)
 *! \param ptControl    the target control
 *! \param ptRegion     the region buffer
 *! \retval true        visible
 *! \retval false       invisible
 */
extern
bool vsf_tgui_control_get_visible_region(   const vsf_tgui_control_t* ptControl,
                                            vsf_tgui_region_t* ptRegionBuffer);

extern
vsf_tgui_region_t * vsf_tgui_control_get_relative_region(
                                        const vsf_tgui_control_t* ptControl,
                                        vsf_tgui_region_t *ptAbsoluteRegion);

extern
bool vsf_tgui_control_shoot(    const vsf_tgui_control_t* ptControl,
                                const vsf_tgui_location_t *ptLocation);


/*----------------------------------------------------------------------------*
 *  Status and Attributes                                                     *
 *----------------------------------------------------------------------------*/

/*! \brief update bIsControlTransparent bit in control status
 *! \parame ptControl target control address
 *! \retval true the original value of bIsControlTransparent is changed
 *! \retval false the set value is the same as the original value, no change is
 *!               made.
 */
extern
bool vsf_tgui_control_set_is_transparent_bit(   vsf_tgui_control_t* ptControl,
                                                bool bIsControlTransparent);

extern
vsf_tgui_status_t vsf_tgui_control_status_get(const vsf_tgui_control_t* ptControl);


extern
void vsf_tgui_control_status_set(   vsf_tgui_control_t* ptControl,
                                    vsf_tgui_status_t tStatus);

extern
bool vsf_tgui_control_is_container(const vsf_tgui_control_t* ptControl);

extern
__vsf_tgui_control_core_t* vsf_tgui_control_get_core(
                                        const vsf_tgui_control_t* ptControl);

extern
vsf_tgui_control_t* vsf_tgui_control_get_parent(
                                        const vsf_tgui_control_t* ptControl);

extern
uint_fast8_t vk_tgui_container_visible_item_get(
                                        const vsf_tgui_container_t *ptContainer);


/*----------------------------------------------------------------------------*
 *  Methods and Others                                                        *
 *----------------------------------------------------------------------------*/
extern
const vsf_tgui_control_t* __vk_tgui_control_get_next_visible_one_within_container(
                                            const vsf_tgui_control_t* ptItem);

extern const vsf_tgui_top_container_t* vk_tgui_control_get_top(
                                        const vsf_tgui_control_t* ptControl);

#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
extern
bool vsf_tgui_control_refresh(  const vsf_tgui_control_t *ptControl,
                                const vsf_tgui_region_t *ptRegion);
#endif

extern
bool vsf_tgui_control_send_message( const vsf_tgui_control_t* ptControl, 
                                    vsf_tgui_evt_t tEvent);

extern
bool vsf_tgui_control_update(const vsf_tgui_control_t* ptControl);

extern
bool vsf_tgui_control_update_tree(const vsf_tgui_control_t* ptControl);

extern
bool vsf_tgui_control_set_active(const vsf_tgui_control_t* ptControl);

extern
fsm_rt_t vsf_tgui_control_msg_handler(  vsf_tgui_control_t* ptNode,
                                        vsf_tgui_msg_t* ptMSG);

extern
fsm_rt_t vsf_tgui_container_msg_handler(vsf_tgui_container_t* ptNode,
                                        vsf_tgui_msg_t* ptMSG);

extern
fsm_rt_t __vsf_tgui_control_msg_handler(vsf_tgui_control_t* ptControl,
                                        vsf_tgui_msg_t* ptMSG,
                                        const i_tgui_control_methods_t* ptMethods);

extern
fsm_rt_t __vk_tgui_control_user_message_handling(   vsf_tgui_control_t* ptControl,
                                                    const vsf_tgui_evt_t* ptEvent);

extern
fsm_rt_t vk_tgui_control_init(vsf_tgui_control_t* ptControl);

extern
fsm_rt_t vk_tgui_container_init(vsf_tgui_container_t *ptContainer);

extern
fsm_rt_t vk_tgui_control_update(vsf_tgui_control_t* ptControl);

extern
fsm_rt_t vk_tgui_container_update(vsf_tgui_container_t* ptContainer);

#endif
/* EOF */
