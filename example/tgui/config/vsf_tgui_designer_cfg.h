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


//! \note Top Level Application Configuration

#ifndef __TOP_TGUI_DESIGNER_CFG_H__
#define __TOP_TGUI_DESIGNER_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#undef VSF_TGUI_HOR_MAX
#undef VSF_TGUI_VER_MAX
#undef APP_DISP_SDL2_WIDTH
#undef APP_DISP_SDL2_HEIGHT
#undef VSF_TGUI_CFG_SUPPORT_NAME_STRING
#undef VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING
#undef VSF_MSGT_NODE_OFFSET_TYPE

#define APP_TGUI_DESIGNER_HOR_MAX                   800
#define APP_TGUI_DESIGNER_VER_MAX                   480

#define APP_TGUI_DESIGNER_PADDING_MAX               40

#define APP_TGUI_DESIGNER_CODE_GEN_VER              32
#define APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR           400
#define APP_TGUI_DESIGNER_RIGHT_ATTRS_VER           800
#define APP_TGUI_DESIGNER_BOTTOM_INFO_VER_MIN       200

#define APP_TGUI_LEFT_VER_MAX                       \
    (APP_TGUI_DESIGNER_VER_MAX + APP_TGUI_DESIGNER_BOTTOM_INFO_VER_MIN + APP_TGUI_DESIGNER_PADDING_MAX)

#define APP_TGUI_RIGHT_MAX                          \
    (APP_TGUI_DESIGNER_RIGHT_ATTRS_VER + APP_TGUI_DESIGNER_CODE_GEN_VER)

#define APP_TGUI_DESIGNER_CODE_GEN_HOR              APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR

#define VSF_TGUI_HOR_MAX                            \
    (APP_TGUI_DESIGNER_HOR_MAX + APP_TGUI_DESIGNER_RIGHT_ATTRS_HOR + APP_TGUI_DESIGNER_PADDING_MAX)

#if APP_TGUI_LEFT_VER_MAX > APP_TGUI_RIGHT_MAX
#   define VSF_TGUI_VER_MAX                         APP_TGUI_LEFT_VER_MAX
#else
#   define VSF_TGUI_VER_MAX                         APP_TGUI_RIGHT_MAX
#endif

#define APP_TGUI_DESIGNER_RIGHT_ATTRS_X             (APP_TGUI_DESIGNER_HOR_MAX + APP_TGUI_DESIGNER_PADDING_MAX)
#define APP_TGUI_DESIGNER_RIGHT_ATTRS_Y             0

#define APP_TGUI_DESIGNER_CODE_GEN_X                APP_TGUI_DESIGNER_RIGHT_ATTRS_X
#define APP_TGUI_DESIGNER_CODE_GEN_Y                (VSF_TGUI_VER_MAX - APP_TGUI_DESIGNER_CODE_GEN_VER)

#define APP_TGUI_DESIGNER_BOTTOM_INFO_X             0
#define APP_TGUI_DESIGNER_BOTTOM_INFO_Y             (APP_TGUI_DESIGNER_VER_MAX + APP_TGUI_DESIGNER_PADDING_MAX)
#define APP_TGUI_DESIGNER_BOTTOM_INFO_HOR           (APP_TGUI_DESIGNER_HOR_MAX + APP_TGUI_DESIGNER_PADDING_MAX)
#define APP_TGUI_DESIGNER_BOTTOM_INFO_VER           (VSF_TGUI_VER_MAX - APP_TGUI_DESIGNER_VER_MAX - APP_TGUI_DESIGNER_PADDING_MAX)


#define APP_DISP_SDL2_WIDTH                         VSF_TGUI_HOR_MAX
#define APP_DISP_SDL2_HEIGHT                        VSF_TGUI_VER_MAX
#define VSF_TGUI_CFG_SUPPORT_NAME_STRING            ENABLED
#define VSF_MSG_TREE_CFG_SUPPORT_NAME_STRING        ENABLED
#define VSF_MSGT_NODE_OFFSET_TYPE                   int32_t

#define VSF_TGUI_CFG_SV_PORT_COLOR_BUFFER_SIZE \
    (VSF_TGUI_HOR_MAX * VSF_TGUI_VER_MAX)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */
