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

#ifndef __VSF_TINY_GUI_V_FONT_H__
#define __VSF_TINY_GUI_V_FONT_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_FONT_SUPPORT_PROPORTIONAL
#   define VSF_TGUI_FONT_SUPPORT_PROPORTIONAL  ENABLED
#endif

#ifndef VSF_TGUI_FONT_SUPPORT_MONOSPACE
#   define VSF_TGUI_FONT_SUPPORT_MONOSPACE     ENABLED
#endif

#if !defined(VSF_TGUI_FONT_SUPPORT_PROPORTIONAL) && !defined(VSF_TGUI_FONT_SUPPORT_MONOSPACE)
#   error "Choose at least one of VSF_TGUI_FONT_SUPPORT_MONOSPACE of VSF_TGUI_FONT_SUPPORT_PROPORTIONAL"
#endif

#ifndef VSF_TGUI_FONT_SUPPORT_DETERMINE_HEIGHT_BASE_ON_SIZE
#   define VSF_TGUI_FONT_SUPPORT_DETERMINE_HEIGHT_BASE_ON_SIZE ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum vsf_tgui_font_flags_t {
    VSF_TGUI_FONT_MONOSPACE = (0 << 0),
    VSF_TGUI_FONT_PROPORTIONAL = (1 << 0),
};
struct vsf_tgui_font_t {
    uint8_t chFlags;

    uint8_t chHeight;

#ifdef VSF_TGUI_FONT_SUPPORT_MONOSPACE
    uint8_t chWidth;
#endif

#ifdef VSF_TGUI_FONT_SUPPORT_DETERMINE_HEIGHT_BASE_ON_SIZE
    uint8_t chFontSize;
    const char* pchFontPath;
    void *ptData;
#endif

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    const char* name_ptr;
#endif
};
typedef struct vsf_tgui_font_t vsf_tgui_font_t;
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t chFontIndex);

extern
uint8_t vsf_tgui_font_number(void);

extern
uint8_t vsf_tgui_font_get_char_height(const uint8_t chFontIndex);

extern
uint8_t vsf_tgui_font_get_char_width(const uint8_t chFontIndex, uint32_t wChar);

#endif

#endif
/* EOF */
