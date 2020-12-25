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

 /*============================ INCLUDES ======================================*/
//! \note do not move this pre-processor statement to other places
#include "../vsf_ui_cfg.h"

#ifndef __VSF_TINY_GUI_CFG_H__
#define __VSF_TINY_GUI_CFG_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#    error tGUI requies ANSI-C99 and above
#endif


/*----------------------------------------------------------------------------*
 *  Rendering                                                                 *
 *----------------------------------------------------------------------------*/

#define VSF_TGUI_V_TEMPLATE_EXAMPLE                             1
#define VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW                         2

#ifndef VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL
#   define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL                  VSF_TGUI_V_TEMPLATE_EXAMPLE
#endif

/*----------------------------------------------------------------------------*
 *  Colour Space                                                              *
 *----------------------------------------------------------------------------*/

#define VSF_TGUI_COLOR_RGB_565                                  0
#define VSF_TGUI_COLOR_BGR_565                                  1
#define VSF_TGUI_COLOR_ARGB_8888                                2
#define VSF_TGUI_COLOR_RGB8_USER_TEMPLATE                       3

#ifndef VSF_TGUI_CFG_COLOR_MODE
#   define VSF_TGUI_CFG_COLOR_MODE                              VSF_TGUI_COLOR_ARGB_8888
#endif

#undef __VSF_TGUI_IS_COLOR_SUPPORT_ALPHA__
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
#   define __VSF_TGUI_IS_COLOR_SUPPORT_ALPHA__                  1
#else
#   define __VSF_TGUI_IS_COLOR_SUPPORT_ALPHA__                  0
#endif

/*----------------------------------------------------------------------------*
 *  Designer & Layout                                                         *
 *----------------------------------------------------------------------------*/

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING          ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN           ENABLED
#endif


#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ANCHOR           DISABLED        /* haven't support */
#endif

#ifndef  VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_DOCK             DISABLED        /* haven't support */
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN
#   define VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_ALIGN            DISABLED        /* haven't support */
#endif

/*----------------------------------------------------------------------------*
 *  Font and Text                                                             *
 *----------------------------------------------------------------------------*/
#define VSF_TGUI_TEXT_ASCII                                     0
#define VSF_TGUI_TEXT_UTF8                                      1

#ifndef VSF_TGUI_CFG_TEXT_MODE
#   define VSF_TGUI_CFG_TEXT_MODE                               VSF_TGUI_TEXT_UTF8
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_NAME_STRING
#   define VSF_TGUI_CFG_SUPPORT_NAME_STRING                     ENABLED
#endif
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   if VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING != ENABLED
#       warning "VSF_TGUI_CFG_SUPPORT_NAME_STRING is ENABLED and depends \
on VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING, but it is set to DISABLED. \
In order to continue the compilation process, this macro \
VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING is forced to be ENABLED. Please \
enable it in vsf_usr_cfg.h or top vsf_tgui_cfg.h to supress this \
warning."
#       undef VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING
#       define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING             ENABLED
#   endif
#endif

#ifndef VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING
#   define VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING                  ENABLED
#endif

#ifndef VSF_TGUI_CFG_SAFE_STRING_MODE
#   define VSF_TGUI_CFG_SAFE_STRING_MODE                        ENABLED
#endif

/*----------------------------------------------------------------------------*
 *  Refresh Control                                                           *
 *----------------------------------------------------------------------------*/
#define VSF_TGUI_REFRESH_SCHEME_NONE                            0
#define VSF_TGUI_REFRESH_SCHEME_Z_ORDER                         1
#define VSF_TGUI_REFRESH_SCHEME_PRE_ORDER_TRAVERSAL             1
#define VSF_TGUI_REFRESH_SCHEME_LAYER_BY_LAYER                  2
#define VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL         2

#ifndef VSF_TGUI_CFG_REFRESH_SCHEME
#   define VSF_TGUI_CFG_REFRESH_SCHEME                          VSF_TGUI_REFRESH_SCHEME_Z_ORDER
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_TRANSPARENT_CONTROL
#   define VSF_TGUI_CFG_SUPPORT_TRANSPARENT_CONTROL             ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_DIRTY_REGION
#   define VSF_TGUI_CFG_SUPPORT_DIRTY_REGION                    ENABLED
#endif

/*----------------------------------------------------------------------------*
 *  Message Handling Control                                                  *
 *----------------------------------------------------------------------------*/
#ifndef VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME
#   define VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME               ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME
#   define VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME              ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_MOUSE
#   define VSF_TGUI_CFG_SUPPORT_MOUSE                           ENABLED
#endif
#ifndef VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING
#   define VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING             DISABLED
#endif

/*----------------------------------------------------------------------------*
 * Control Features                                                           *
 *----------------------------------------------------------------------------*/

#ifndef VSF_TGUI_CFG_SUPPORT_TIMER
#   define VSF_TGUI_CFG_SUPPORT_TIMER                           ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER
#   define VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER                ENABLED        /*!< haven't implemented yet*/
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER
#   define VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER           ENABLED
#endif 

#ifndef VSF_TGUI_CFG_SUPPORT_SLIDER
#   define VSF_TGUI_CFG_SUPPORT_SLIDER                          ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_TEXT_LIST
#   define VSF_TGUI_CFG_SUPPORT_TEXT_LIST                       ENABLED
#endif

#if VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED
#   ifndef VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE
#       define VSF_TGUI_CFG_TEXT_LIST_SUPPORT_SLIDE             ENABLED
#   endif
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_LIST
#   define VSF_TGUI_CFG_SUPPORT_LIST                            ENABLED
#endif

#if VSF_TGUI_CFG_SUPPORT_LIST == ENABLED
#   ifndef VSF_TGUI_CFG_LIST_SUPPORT_SLIDE
#       define VSF_TGUI_CFG_LIST_SUPPORT_SLIDE                  ENABLED
#   endif
#endif

#ifndef VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE
#   define VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE  DISABLED
#endif

#ifndef VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR
#   define VSF_TGUI_CFG_LIST_SUPPORT_SCROOLBAR                  ENABLED
#endif

#ifndef VSF_TGUI_CFG_PANEL_HAS_LABEL
#   define VSF_TGUI_CFG_PANEL_HAS_LABEL                         ENABLED
#endif

/*----------------------------------------------------------------------------*
 * miscellaneous                                                              *
 *----------------------------------------------------------------------------*/
#ifndef VSF_TGUI_ASSERT
#   define VSF_TGUI_ASSERT                                      ASSERT
#endif

#ifndef VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG                    DISABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG                    ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG                     ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG                  ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG                    DISABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG                
#   define VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG                 ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG                
#   define VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG                ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG                    ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_KEY_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_KEY_EVT_LOG                        ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG                    ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG
#   define VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG           ENABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_SYSTEM_LOG
#   define VSF_TGUI_CFG_SHOW_SYSTEM_LOG                         ENABLED
#endif
#if VSF_TGUI_CFG_SHOW_SYSTEM_LOG != ENABLED
#   undef VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG
#   undef VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG

#   define VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG                    DISABLED
#   define VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG                    DISABLED
#   define VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG                     DISABLED
#   define VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG                  DISABLED
#   define VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG                    DISABLED
#   define VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG           DISABLED
#   define VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG                 DISABLED
#   define VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG                DISABLED
#endif

#ifndef VSF_TGUI_CFG_SHOW_USER_INPUT_LOG
#   define VSF_TGUI_CFG_SHOW_USER_INPUT_LOG                     ENABLED
#endif
#if VSF_TGUI_CFG_SHOW_USER_INPUT_LOG != ENABLED
#   undef   VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG
#   undef   VSF_TGUI_CFG_SHOW_KEY_EVT_LOG
#   undef   VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG
#   define  VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG                   DISABLED
#   define  VSF_TGUI_CFG_SHOW_KEY_EVT_LOG                       DISABLED
#   define  VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG                   DISABLED
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