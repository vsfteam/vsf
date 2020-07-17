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

#ifndef __VSF_TINY_GUI_COMMON_H__
#define __VSF_TINY_GUI_COMMON_H__

/*============================ INCLUDES ======================================*/

#if VSF_USE_TINY_GUI == ENABLED
#include "osa_service/vsf_osa_service.h"
/*============================ MACROS ========================================*/
#define VSF_TGUI_COLORSIZE_1BIT     0
#define VSF_TGUI_COLORSIZE_4BIT     2
#define VSF_TGUI_COLORSIZE_8BIT     3
#define VSF_TGUI_COLORSIZE_16BIT    4
#define VSF_TGUI_COLORSIZE_24IT     6
#define VSF_TGUI_COLORSIZE_32IT     5

/*============================ MACROFIED FUNCTIONS ===========================*/

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#   define tgui_contains(...)          __VA_ARGS__

#   define __tgui_attribute(__NAME, ...)  .__NAME = __VA_ARGS__
#   define tgui_attribute(__NAME, ...)     __tgui_attribute(__NAME, __VA_ARGS__)

#   if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN == ENABLED
#       define tgui_alignto(...)    __tgui_attribute(tAlign,        {__VA_ARGS__})
#   endif

#   if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR == ENABLED
#       define tgui_anchor(...)     __tgui_attribute(tAnchor,       __VA_ARGS__)
#   endif

#   if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK == ENABLED
#       define tgui_dock(...)       __tgui_attribute(tDock,         __VA_ARGS__)
#   endif

#   if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
#       define tgui_margin(...)     __tgui_attribute(tMargin,       {__VA_ARGS__})
#       define tgui_padding(...)    __tgui_attribute(tConatinerPadding,      {__VA_ARGS__})
#   endif

#   if VSF_TGUI_CFG_SUPPORT_MULTI_LANGUAGE_EXTERNSION == ENABLED
#       define __tgui_text(__NAME, __ID, ...)                                   \
                .__NAME.tString.tID = (__ID),                                   \
                .__NAME.bIsChanged = true,                                      \
                .__NAME.u4Align = (0, ##__VA_ARGS__)
#   else

#       if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
#           define __tgui_text(__NAME, __TEXT, ...)                             \
                .__NAME.tString.pstrText = __TEXT,                              \
                .__NAME.tString.iSize = sizeof(__TEXT) - 1,                     \
                .__NAME.bIsChanged = true,                                      \
                .__NAME.u4Align = (0, ##__VA_ARGS__)
#       else
#           define __tgui_text(__NAME, __TEXT, ...)                             \
                .__NAME.tString.pstrText = __TEXT,                              \
                .__NAME.bIsChanged = true,                                      \
                .__NAME.u4Align = (0, ##__VA_ARGS__)
#       endif
#   endif

#   define __tgui_line_space(__NAME, __PIX)                                     \
                .__NAME.chInterLineSpace = (__PIX)

#   define tgui_size(...)                                                       \
                __tgui_attribute(tSize,                 {__VA_ARGS__})
#   define tgui_location(...)                                                   \
                __tgui_attribute(tLocation,             {__VA_ARGS__})
#   define tgui_region(...)                                                     \
                __tgui_attribute(tRegion,               {__VA_ARGS__})
#   define tgui_background(...)                                                 \
                __tgui_attribute(tBackground,           {__VA_ARGS__})
#   define tgui_text(__NAME, __TEXT, ...)                                       \
                __tgui_text(__NAME, __TEXT, __VA_ARGS__)
#   define tgui_line_space(__NAME, __PIX)                                       \
                __tgui_line_space(__NAME, (__PIX))

#   if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
#       define tgui_timer(__NAME, __INTERVAL, __REPEATE, ...)                   \
                 __tgui_attribute(__NAME,   {.u29Interval = (__INTERVAL),       \
                                             .bIsRepeat = (__REPEATE),          \
                                             .bEnabled = (true, ##__VA_ARGS__), \
                                            })
#   endif

#   define tgui_handler(...)                                                    \
                __tgui_attribute(msg_handler,       {__VA_ARGS__})
#   define tgui_container_type(__TYPE)                                          \
                tgui_attribute(u5Type, (__TYPE))

#define def_tgui_msgmap(__NAME)
#define end_def_tgui_msgmap(...)
#define implement_tgui_msgmap(...)

#define __declare_tgui_msgmap(__NAME, ...)                                      \
            const vsf_tgui_user_evt_handler[__VA_ARGS__] __NAME;
#define declare_tgui_msgmap(__NAME, ...)                                        \
            __declare_tgui_msgmap(__NAME, __VA_ARGS__)

#define __describe_tgui_msgmap(__NAME, ...)                                     \
            const vsf_tgui_user_evt_handler __NAME[] = {                        \
                __VA_ARGS__                                                     \
            };

#define describe_tgui_msgmap(__NAME, ...)                                       \
            __describe_tgui_msgmap(__NAME, __VA_ARGS__)

#define __tgui_msgmap(__MSGMAP)                                                 \
                .tMSGMap = {                                                    \
                    .ptItems = __MSGMAP,                                        \
                    .chCount = UBOUND(__MSGMAP),                                \
                }
#define tgui_msgmap(__MSGMAP)               __tgui_msgmap(__MSGMAP)

#define __tgui_msg_handler(__MSG, __FUNC, ...)                                  \
            {                                                                   \
                .tMSG = (__MSG),                                                \
                .u2Type = VSF_MSGT_NODE_HANDLER_TYPE_CALLBACK,                  \
                .FSM = (vsf_tgui_controal_fsm_t *)&__FUNC,                      \
                .u10EvtMask = ((uint16_t)-1, ##__VA_ARGS__),                    \
            }

#define tgui_msg_handler(__MSG, __FUNC, ...)                                    \
            __tgui_msg_handler(__MSG, __FUNC, __VA_ARGS__)

#define __tgui_msg_mux(__MSG, __FUNC, ...)                                      \
            {                                                                   \
                .tMSG = (__MSG),                                                \
                .u2Type = VSF_MSGT_NODE_HANDLER_TYPE_CALLBACK,                  \
                .FSM = (vsf_tgui_controal_fsm_t *)&__FUNC,                      \
                .u10EvtMask = (0, ##__VA_ARGS__),                               \
            }

#define tgui_msg_mux(__MSG, __FUNC, ...)                                        \
            __tgui_msg_mux((__MSG), __FUNC, __VA_ARGS__)
#endif
/*============================ TYPES =========================================*/

/*----------------------------------------------------------------------------*
 *  Color                                                                     *
 *----------------------------------------------------------------------------*/


/*! \note vsf_tgui_color is for most used by view (rendering) part
 */
typedef union vsf_tgui_color_t vsf_tgui_color_t;

#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_BGR_565
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint16_t     u5R    : 5;
            uint16_t     u6G    : 6;
            uint16_t     u5B    : 5;
        },
        tChannel
    )
    uint16_t        hwValue;
    uint16_t        Value;          //!< generic symbol name
};
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB_565
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint16_t     u5B    : 5;
            uint16_t     u6G    : 6;
            uint16_t     u5R    : 5;
        },
        tChannel
    )
    uint16_t        hwValue;
    uint16_t        Value;          //!< generic symbol name
};
#elif VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_RGB8_USER_TEMPLATE
union vsf_tgui_color_t {
    uint8_t chColorID;
    uint8_t Value;                  //!< generic symbol name
};
#else /*VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888 */
union vsf_tgui_color_t {
    implement_ex(
        struct {
            uint8_t     chB;
            uint8_t     chG;
            uint8_t     chR;
            uint8_t     chA;
        },
        tChannel
    )
    uint8_t         chValues[4];
    uint32_t        wValue;
    uint32_t        Value;          //!< generic symbol name
};
#endif


/*----------------------------------------------------------------------------*
 *  Layout                                                                    *
 *----------------------------------------------------------------------------*/
typedef struct vsf_tgui_location_t {
    int16_t iX;
    int16_t iY;
}vsf_tgui_location_t;

typedef struct vsf_tgui_size_t  {
    int16_t iWidth;
    int16_t iHeight;
} vsf_tgui_size_t;

typedef struct vsf_tgui_region_t {
    implement_ex(vsf_tgui_location_t, tLocation)
    implement_ex(vsf_tgui_size_t, tSize)
}vsf_tgui_region_t;


typedef enum {
    VSF_TGUI_ALIGN_CENTER   = 0,
    VSF_TGUI_ALIGN_LEFT     = _BV(0),
    VSF_TGUI_ALIGN_RIGHT    = _BV(1),
    VSF_TGUI_ALIGN_TOP      = _BV(2),
    VSF_TGUI_ALIGN_BOTTOM   = _BV(3),
    VSF_TGUI_ALIGN_FILL     = VSF_TGUI_ALIGN_LEFT
                            | VSF_TGUI_ALIGN_RIGHT
                            | VSF_TGUI_ALIGN_TOP
                            | VSF_TGUI_ALIGN_BOTTOM,
}vsf_tgui_align_mode_t;

typedef enum {
    VSF_TGUI_DOCK_NONE      = 0,
    VSF_TGUI_DOCK_LEFT      = _BV(0),
    VSF_TGUI_DOCK_RIGHT     = _BV(1),
    VSF_TGUI_DOCK_TOP       = _BV(2),
    VSF_TGUI_DOCK_BOTTOM    = _BV(3),
    VSF_TGUI_DOCK_FILL      =   VSF_TGUI_DOCK_LEFT
                            |   VSF_TGUI_DOCK_RIGHT
                            |   VSF_TGUI_DOCK_TOP
                            |   VSF_TGUI_DOCK_BOTTOM,
}vsf_tgui_dock_mode_t;

typedef enum {
    VSF_TGUI_ANCHOR_NONE    = 0,
    VSF_TGUI_ANCHOR_LEFT    = _BV(0),
    VSF_TGUI_ANCHOR_RIGHT   = _BV(1),
    VSF_TGUI_ANCHOR_TOP     = _BV(2),
    VSF_TGUI_ANCHOR_BOTTOM  = _BV(3),
}vsf_tgui_anchor_mode_t;


/*----------------------------------------------------------------------------*
 *  String & Language                                                         *
 *----------------------------------------------------------------------------*/

#if VSF_TGUI_CFG_SUPPORT_MULTI_LANGUAGE_EXTERNSION == ENABLED
#   if !defined(VSF_TGUI_CFG_LANGUAGE_ID_TYPE)
#       define VSF_TGUI_CFG_LANGUAGE_ID_TYPE            uint8_t
#   endif

typedef VSF_TGUI_CFG_LANGUAGE_ID_TYPE                   vsf_tgui_text_id_t;

typedef struct vsf_tgui_string_t {
    vsf_tgui_text_id_t                                  tID;
} vsf_tgui_string_t;

#else

#if !defined(VSF_TGUI_CFG_STRING_TYPE)
#   define VSF_TGUI_CFG_STRING_TYPE                     const char
#endif

typedef struct vsf_tgui_string_t {
    VSF_TGUI_CFG_STRING_TYPE    *pstrText;
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    int16_t                     iSize;
#endif
} vsf_tgui_string_t;

#endif

typedef struct vsf_tgui_text_info_t {
    vsf_tgui_string_t   tString;
    uint8_t             u4Align     : 4;
    uint8_t                         : 2;
    uint8_t             bIsChanged  : 1;
    uint8_t             bIsAutoSize : 1;
    int8_t              chInterLineSpace; 
#   if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    struct {
        vsf_tgui_size_t tStringSize;
        uint16_t        hwLines;
    } tInfoCache;
#   endif

}vsf_tgui_text_info_t;

/*----------------------------------------------------------------------------*
 *  Tile                                                                      *
 *----------------------------------------------------------------------------*/

#ifndef VSF_TGUI_CFG_DRAM_POINTER_TYPE
#   define VSF_TGUI_CFG_DRAM_POINTER_TYPE           uintptr_t
#endif

typedef VSF_TGUI_CFG_DRAM_POINTER_TYPE vsf_tgui_disp_ram_uintptr_t;

typedef struct vsf_tgui_tile_core_t {
    struct {
        uint8_t         u2RootTileType      : 2;    /* 0: buf tile, 1: index tile, 2 and 3 undefined*/
        uint8_t         u2ColorType         : 2;    /* reserved */
        uint8_t         u3ColorSize         : 3;    /* 0: 1bit, 2: 4bit, 3: 8bit 4: 16bit, 5: 32bit, 6: 24bit */
        uint8_t         bIsRootTile         : 1;    /* 0: Child Tile, u3ColorSize and u4RootTileType have no meaning 
                                                       1: Root Tile, u3ColorSize and u4RootTileType have meaning*/
    } Attribute;
} vsf_tgui_tile_core_t;

typedef struct vsf_tgui_tile_idx_root_t {
    uint8_t chIndex;                                /*index is stored in the lower 8bit*/
    implement(vsf_tgui_tile_core_t)
}vsf_tgui_tile_idx_root_t;

typedef struct vsf_tgui_tile_buf_root_t {
    uint8_t                 : 8;
    implement(vsf_tgui_tile_core_t)
    implement_ex(vsf_tgui_size_t, tSize)            /* the size of the tile */
    vsf_tgui_disp_ram_uintptr_t   ptBitmap;         /*A pointer points to bitmap buffer*/
}vsf_tgui_tile_buf_root_t;

typedef struct vsf_tgui_tile_child_t {

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    union {
        union {
            struct {
                uint8_t : 8;
                implement(vsf_tgui_tile_core_t)
            };
            implement_ex(vsf_tgui_location_t, tLocation)
        };
#endif
        union {
            struct {
                uint8_t : 8;
                implement(vsf_tgui_tile_core_t)
            }_;
            implement_ex(vsf_tgui_location_t, tLocation)
        }_;
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    };
#endif

    implement_ex(vsf_tgui_size_t, tSize)            /* the size of the tile */
    vsf_tgui_tile_core_t    *parent_ptr;              /* points to the parent */
} vsf_tgui_tile_child_t;

typedef union vsf_tgui_tile_t {
    implement_ex(
        struct {
            uint8_t : 8;
            vsf_tgui_tile_core_t    tCore;
        },
        _
    )
    vsf_tgui_tile_idx_root_t    tIndexRoot;
    vsf_tgui_tile_buf_root_t    tBufRoot;
    vsf_tgui_tile_child_t       tChild;
} vsf_tgui_tile_t;


/*----------------------------------------------------------------------------*
 *  Container                                                                 *
 *----------------------------------------------------------------------------*/
typedef enum vsf_tgui_container_type_t {
    VSF_TGUI_CONTAINER_TYPE_PLANE,
    VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL,
    VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL,
    VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL,
    VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL,
}vsf_tgui_container_type_t;


/*----------------------------------------------------------------------------*
 *  tGUI Message & Event                                                      *
 *----------------------------------------------------------------------------*/
#define VSF_TGUI_MSG_MSK    (0x1F0)
#define VSF_TGUI_EVT_MSK    (0x00F)

enum {
    VSF_TGUI_MSG_AVAILABLE          = 0x200,    //VSF_EVT_SYSTEM + 0x100,
    VSF_TGUI_MSG_LOW_LEVEL_READY_TO_REFRESH,
    VSF_TGUI_MSG = 0x210,

    //! control events
    VSF_TGUI_MSG_CONTROL_EVT        = VSF_TGUI_MSG + 0x00,
        VSF_TGUI_EVT_ON_SET_TOP_CONTAINER,
        VSF_TGUI_EVT_ON_LOAD,
        VSF_TGUI_EVT_ON_DEPOSE,
        VSF_TGUI_EVT_UPDATE,
        VSF_TGUI_EVT_UPDATE_TREE,
        VSF_TGUI_EVT_REFRESH,
        VSF_TGUI_EVT_POST_REFRESH,
        VSF_TGUI_EVT_GET_ACTIVE,
        VSF_TGUI_EVT_LOST_ACTIVE,
        VSF_TGUI_EVT_ON_TIME,

    //! pointer events
    VSF_TGUI_MSG_POINTER_EVT        = VSF_TGUI_MSG + 0x10,

        VSF_TGUI_EVT_POINTER_DOWN = VSF_TGUI_MSG_POINTER_EVT,
        VSF_TGUI_EVT_POINTER_HOLD,                                              //!< not all device support this
        VSF_TGUI_EVT_POINTER_UP,
        VSF_TGUI_EVT_POINTER_CLICK,
        VSF_TGUI_EVT_POINTER_DOUBLE_CLICK,

        /*! pointer first entering the region of ]a control*/
        VSF_TGUI_EVT_POINTER_ENTER,                                             //!< not all device support this

        /*! pointer move out of the region of the target control*/
        VSF_TGUI_EVT_POINTER_LEFT,                                              //!< not all device support this

        /*! pointer stay in the region of a control for a while,
         *! it is different from VSF_TGUI_EVT_POINTER_HOLD. Hold requires that
         *! the pointer is down at the same control. Hover requires no pointer 
         *! down at the target control.
         *! 
         *! this event is currently not supported but reserved.
         */
        VSF_TGUI_EVT_POINTER_HOVER,                                             //!< not all device support this

        VSF_TGUI_EVT_POINTER_MOVE,                                              //!< not all device support this

        

    //! key events
    VSF_TGUI_MSG_KEY_EVT            = VSF_TGUI_MSG + 0x20,
        VSF_TGUI_EVT_KEY_DOWN = VSF_TGUI_MSG_KEY_EVT,
        VSF_TGUI_EVT_KEY_REPEATE,                                               //!< not all device support this
        VSF_TGUI_EVT_KEY_UP,
        VSF_TGUI_EVT_KEY_PRESSED,
        VSF_TGUI_EVT_KEY_LONG_PRESSED,                                          //!< not all device support this
        VSF_TGUI_EVT_KEY_DOUBLE_CLICK,                                          //!< not all device support this

    //! gesture events
    VSF_TGUI_MSG_GESTURE_EVT        = VSF_TGUI_MSG + 0x30,

        VSF_TGUI_EVT_GESTURE_SLIDE = VSF_TGUI_MSG_GESTURE_EVT,
        VSF_TGUI_EVT_GESTURE_ZOOM_IN,                                           //!< not all device support this
        VSF_TGUI_EVT_GESTURE_ZOOM_OUT,                                          //!< not all device support this
        VSF_TGUI_EVT_GESTURE_ROTATE_CLOCKWISE,                                  //!< not all device support this
        VSF_TGUI_EVT_GESTURE_ROTATE_ANTICLOCKWISE,                              //!< not all device support this

    //! All control specific events share the same code region
    VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT   = VSF_TGUI_MSG + 0x40,  
    
    VSF_TGUI_MSG_LIST_EVT           = VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT,
        VSF_TGUI_EVT_LIST_SELECTION_CHANGED = VSF_TGUI_MSG_LIST_EVT,
        VSF_TGUI_EVT_LIST_SLIDING_STARTED,
        VSF_TGUI_EVT_LIST_SLIDING_STOPPED,
};

enum {
    VSF_TGUI_MSG_RT_UNHANDLED   = fsm_rt_err,
    VSF_TGUI_MSG_RT_DONE        = fsm_rt_cpl,
    VSF_TGUI_MSG_RT_REFRESH     = fsm_rt_user,
    VSF_TGUI_MSG_RT_REFRESH_PARENT,
};

typedef struct vsf_tgui_control_t vsf_tgui_control_t;
typedef struct vsf_tgui_top_container_t vsf_tgui_top_container_t;

typedef struct vsf_tgui_msg_t {
    implement(vsf_msgt_msg_t)
    vsf_tgui_control_t *ptTarget;
} vsf_tgui_msg_t;

typedef struct vsf_tgui_refresh_evt_t {
    implement(vsf_tgui_msg_t)
    vsf_tgui_region_t *ptRegion;
} vsf_tgui_refresh_evt_t;

typedef struct vsf_tgui_pointer_evt_t {
    implement(vsf_tgui_msg_t)
    implement(vsf_tgui_location_t)
} vsf_tgui_pointer_evt_t;

typedef struct vsf_tgui_gesture_evt_t {
    implement(vsf_tgui_msg_t)
    struct {
        implement(vsf_tgui_location_t)
        uint16_t    hwMillisecond;
    } tDelta;
}vsf_tgui_gesture_evt_t;

typedef struct vsf_tgui_key_evt_t {
    implement(vsf_tgui_msg_t)
    uint16_t hwKeyValue;                                                        //!< control, alt, shift, Fn and etc can be encoded wit
} vsf_tgui_key_evt_t;

typedef union vsf_tgui_evt_t {
    implement(vsf_tgui_msg_t)
    vsf_tgui_refresh_evt_t  tRefreshEvt;
    vsf_tgui_key_evt_t      tKeyEvt;
    vsf_tgui_pointer_evt_t  tPointerEvt;
    vsf_tgui_gesture_evt_t  tGestureEvt;
}  vsf_tgui_evt_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
vsf_tgui_tile_t* vsf_tgui_tile_get_root(    const vsf_tgui_tile_t* ptTile,
                                            vsf_tgui_region_t* ptRegion);

extern
vsf_tgui_size_t vsf_tgui_root_tile_get_size(const vsf_tgui_tile_t* ptTile);

extern
bool vsf_tgui_tile_is_root(const vsf_tgui_tile_t* ptTile);

extern
void vsf_tgui_text_set( vsf_tgui_text_info_t *ptTextInfo, 
                        const vsf_tgui_string_t *pstrNew);
#endif

#endif
/* EOF */
