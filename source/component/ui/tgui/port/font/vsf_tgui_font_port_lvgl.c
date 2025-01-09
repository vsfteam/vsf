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

 /*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED && VSF_TGUI_CFG_FONT_USE_LVGL == ENABLED

#include "../vsf_tgui_port.h"

#ifdef VSF_TGUI_CFG_LVGL_HEADER
#   include VSF_TGUI_CFG_LVGL_HEADER
#else
#   include <lvgl.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);
    lv_font_t *font = (lv_font_t *)font_ptr->font;

    lv_font_glyph_dsc_t glyph;

    if (font->get_glyph_dsc(font, &glyph, char_u32, 0)) {
        lv_font_fmt_txt_dsc_t *dsc = font->dsc;
        return glyph.adv_w + dsc->kern_scale;
    }

    return 0;
}

uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);
    lv_font_t *font = (lv_font_t *)font_ptr->font;
    VSF_TGUI_ASSERT(font_ptr->height == font->line_height);
    VSF_UNUSED_PARAM(font);
    return font_ptr->height;
}

void vsf_tgui_font_release_char(const uint8_t font_index, uint32_t char_u32, void *bitmap)
{
}

void * vsf_tgui_font_get_char(const uint8_t font_index, uint32_t char_u32, vsf_tgui_region_t *char_region_ptr)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);
    lv_font_t *font = (lv_font_t *)font_ptr->font;

    // TODO: need fix because it seems lvgl font is not 8pp
    return (void *)font->get_glyph_bitmap(font, char_u32);
}

bool vsf_tgui_fonts_init(vsf_tgui_font_t *font_ptr, size_t font_num, const char *font_dir)
{
    VSF_TGUI_ASSERT(font_ptr != NULL);
    return true;
}

#endif

/* EOF */
