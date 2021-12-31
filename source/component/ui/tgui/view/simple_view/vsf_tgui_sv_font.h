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

#ifndef __VSF_TINY_GUI_V_FONT_H__
#define __VSF_TINY_GUI_V_FONT_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_CFG_SV_FONTS
#   define VSF_TGUI_CFG_SV_FONTS ENABLED
#endif

#ifndef VSF_TGUI_CFG_SUPPORT_FONT_USE_FREETYPE
#   define VSF_TGUI_CFG_SUPPORT_FONT_USE_FREETYPE  ENABLED
#endif

#define __TGUI_SV_FONT_DEF(__NAME, __HEIGHT)  __NAME
#define TGUI_SV_FONT_DEF(__NAME, __HEIGHT)                                      \
    __TGUI_SV_FONT_DEF(__NAME, __HEIGHT)

#define TGUI_SV_FT2_FONT_DEF(__NAME, __PATH, __SIZE)                            \
    __TGUI_SV_FONT_DEF(__NAME, 0)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
struct vsf_tgui_font_t {
    uint8_t height;

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    const char* name_ptr;
#endif

#if VSF_TGUI_CFG_SUPPORT_FONT_USE_FREETYPE == ENABLED
    struct {
        uint8_t font_size;
        const char* font_path_ptr;
        void *data_ptr;
    };
#endif
};
typedef struct vsf_tgui_font_t vsf_tgui_font_t;

#if VSF_TGUI_CFG_SV_FONTS == ENABLED
enum {
    VSF_TGUI_SV_FONTS
};
#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern
const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t font_index);

extern
uint8_t vsf_tgui_font_number(void);

extern
uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index);

extern
uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32);

#endif

#endif
/* EOF */
