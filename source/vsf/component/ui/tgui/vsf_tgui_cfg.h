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
//! \note do not move this pre-processor statement to other places
#include "../vsf_ui_cfg.h"

#ifndef __VSF_TINY_GUI_CFG_H__
#define __VSF_TINY_GUI_CFG_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_ASSERT
#   define VSF_TGUI_ASSERT               ASSERT
#endif

#define VSF_TGUI_V_TEMPLATE_EXAMPLE         1
#define VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW     2

#ifndef VSF_TGUI_HOR_MAX
#	define VSF_TGUI_HOR_MAX					800
#endif

#ifndef VSF_TGUI_VER_MAX
#	define VSF_TGUI_VER_MAX					600
#endif

#define VSF_TGUI_COLOR_RGB16_565            0
#define VSF_TGUI_COLOR_ARGB_8888            1
#define VSF_TGUI_COLOR_RGB8_USER_TEMPLATE   2

#ifndef VSF_TGUI_CFG_COLOR_MODE
#   define VSF_TGUI_CFG_COLOR_MODE          VSF_TGUI_COLOR_RGB16_565
#endif

#define VSF_TGUI_TEXT_ASCII                 0
#define VSF_TGUI_TEXT_UTF8                  1

#ifndef VST_TGUI_CFG_TEXT_MODE
#   define VSF_TGUI_CFG_TEXT_MODE           VSF_TGUI_TEXT_UTF8
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR   DISABLED
#endif

#ifndef  VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK     DISABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN    DISABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING   ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN    ENABLED
#endif

#ifndef VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL
#   define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL          VSF_TGUI_V_TEMPLATE_EXAMPLE
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_NAME_STRING
#   define VSF_TGUI_CFG_SUPPORT_NAME_STRING             DISABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_REFRESH_SCHEME
#   define VSF_TGUI_CFG_SUPPORT_REFRESH_SCHEME          ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME
#   define VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME       ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME
#   define VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME      ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_TRANSPARENT_CONTROL
#   define VSF_TGUI_CFG_SUPPORT_TRANSPARENT_CONTROL     ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_DIRTY_REGION
#   define VSF_TGUI_CFG_SUPPORT_DIRTY_REGION            ENABLED
#endif

#ifndef VSF_TGUI_CFG_SAFE_STRING_MODE
#   define VSF_TGUI_CFG_SAFE_STRING_MODE                DISABLED
#endif

#ifndef VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING
#   define VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING          ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_MOUSE
#   define VSF_TGUI_CFG_SUPPORT_MOUSE                   ENABLED
#endif
#ifndef VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING
#   define VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING     DISABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_TIMER
#   define VSF_TGUI_CFG_SUPPORT_TIMER                   ENABLED
#endif
#ifndef VSF_TGUI_CFG_SHOW_REFRESH_LOG
#   define VSF_TGUI_CFG_SHOW_REFRESH_LOG                DISABLED
#endif
#ifndef VSF_TGUI_CFG_SHOW_ON_TIME_LOG
#   define VSF_TGUI_CFG_SHOW_ON_TIME_LOG                DISABLED
#endif

#ifndef VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE
#   define VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE         ENABLED
#endif
#ifndef VSF_TGUI_CFG_LIST_SUPPORT_SLIDE
#   define VSF_TGUI_CFG_LIST_SUPPORT_SLIDE              ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_TGUI_LOG
#   if VSF_USE_TRACE == ENABLED

#       warning 'macro VSF_TGUI_LOG() is not defined. Use vsf_trace() by default \
as VSF_USE_TRACE is ENABLED. You can remove the log output by putting following\
 code in vsf_usr_cfg.h:   #define VSF_TGUI_LOG'
   
#       define VSF_TGUI_LOG         vsf_trace
#   elif defined(__STDC_VERSION__) && __STDC_VERSION__ > 199901L

#       warning 'macro VSF_TGUI_LOG() is not defined. Use printf() by default \
as VSF_USE_TRACE is DISABLED.You can remove the log output by putting following\
code in vsf_usr_cfg.h:   #define VSF_TGUI_LOG'

#       define VSF_TGUI_LOG(__LEVEL, __STRING, ...)     printf(__STRING, ##__VA_ARGS__)
#   else 
#       define VSF_TGUI_LOG         
#   endif
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


/*============================ INCLUDES ======================================*/
#include "vsf_tgui_common.h"
#endif
#endif
/* EOF */