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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED && VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED

#include "../vsf_tgui_port.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/*============================ MACROS ========================================*/

#define FREETYPE_LOAD_FLAGS                     (FT_LOAD_RENDER)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static inline uint8_t __freetype_get_char_height(FT_Face face)
{
    /*
     * msyh test
        | fontsize | fontsize | fontsize | ��(U+2589) height |
        | -------- | -------- | -------- | :--------------- |
        | 12       | 16       | 17       | 18               |
        | 13       | 17       | 18       | 19               |
        | 14       | 18       | 19       | 20               |
        | 15       | 20       | 20       | 21               |
        | 16       | 21       | 22       | 22               |
        | 17       | 22       | 23       | 25               |
        | 18       | 24       | 25       | 26               |
        | 19       | 25       | 26       | 27               |
        | 20       | 26       | 28       | 28               |
        | 21       | 28       | 29       | 30               |
        | 22       | 29       | 30       | 32               |
        | 23       | 30       | 32       | 33               |
        | 24       | 32       | 33       | 34               |
        | 25       | 33       | 34       | 35               |
        | 26       | 34       | 35       | 38               |
        | 27       | 36       | 37       | 39               |
        | 28       | 37       | 38       | 40               |
        | 29       | 38       | 39       | 41               |
        | 30       | 40       | 40       | 43               |
        | 31       | 41       | 42       | 45               |
        | 32       | 42       | 43       | 46               |
        | 33       | 44       | 44       | 47               |
        | 34       | 45       | 45       | 48               |
        | 35       | 46       | 48       | 50               |
        | 36       | 48       | 49       | 52               |
        | 37       | 49       | 50       | 53               |
        | 38       | 50       | 51       | 54               |
        | 39       | 51       | 53       | 56               |
        | 40       | 53       | 54       | 57               |
        | 41       | 54       | 55       | 59               |
        | 42       | 55       | 56       | 60               |
        | 43       | 57       | 58       | 61               |
        | 44       | 58       | 59       | 63               |
        | 45       | 59       | 60       | 65               |
        | 46       | 61       | 62       | 66               |
        | 47       | 62       | 63       | 67               |
        | 48       | 63       | 64       | 68               |
     */
     return face->size->metrics.height >> 6;
}

uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    FT_Face face = (FT_Face)font_ptr->ft2.data_ptr;
    VSF_TGUI_ASSERT(face != NULL);
    if (FT_Err_Ok == FT_Load_Char(face, char_u32, FREETYPE_LOAD_FLAGS)) {
        return face->glyph->advance.x >> 6;
    }

    return 0;
}

uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    return font_ptr->height;
}

void vsf_tgui_font_release_char(const uint8_t font_index, uint32_t char_u32, void *bitmap)
{
}

void * vsf_tgui_font_get_char(const uint8_t font_index, uint32_t char_u32, vsf_tgui_region_t *char_region_ptr)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    FT_Face face = (FT_Face)font_ptr->ft2.data_ptr;
    VSF_TGUI_ASSERT(face != NULL);

    if (FT_Err_Ok != FT_Load_Char(face, char_u32, FREETYPE_LOAD_FLAGS)) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Freetype] load char failed: %d" VSF_TRACE_CFG_LINEEND, char_u32);
        VSF_TGUI_ASSERT(0);
        return NULL;
    }

    FT_GlyphSlot glyph = face->glyph;
    uint32_t base_line = face->size->metrics.ascender >> 6;
    int32_t top = glyph->bitmap_top;
    int32_t left = glyph->bitmap_left;

    char_region_ptr->tLocation.iX = vsf_max(0, left);    // todo: support negative advance
    char_region_ptr->tLocation.iY = base_line - top;
    char_region_ptr->tSize.iWidth = glyph->bitmap.width;
    char_region_ptr->tSize.iHeight = glyph->bitmap.rows;
    return glyph->bitmap.buffer;
}

bool vsf_tgui_fonts_init(vsf_tgui_font_t *font_ptr, size_t size)
{
    int i;
    FT_Library library;
    FT_Face face;
    uint8_t font_size;
    uint8_t font_height;

    VSF_TGUI_ASSERT(font_ptr != NULL);

    for (i = 0; i < size; i++) {
        if (FT_Err_Ok != FT_Init_FreeType(&library)) {
            VSF_TGUI_ASSERT(0);
            return false;
        }

        char path[256];
        strcpy(path, "font/");
        strcat(path, font_ptr->ft2.font_path_ptr);
        if (FT_Err_Ok != FT_New_Face(library, path, 0, &face)) {
            VSF_TGUI_ASSERT(0);
            return false;
        }

        if (font_ptr->ft2.font_size != 0) {
            if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_ptr->ft2.font_size * 64, 0, 0)) {
                VSF_TGUI_ASSERT(0);
                return false;
            }
            font_ptr->height = __freetype_get_char_height(face);
        } else if (font_ptr->height != 0) {
            font_size = font_ptr->height;
            do {
                if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_size * 64, 0, 0)) {
                    VSF_TGUI_ASSERT(0);
                    return false;
                }

                font_height = __freetype_get_char_height(face);
                if (font_ptr->height < font_height) {
                    font_size--;
                } else {
                    font_ptr->height = font_height;
                    break;
                }
            } while (1);
        } else {
            VSF_TGUI_ASSERT(0);
            return false;
        }
        font_ptr->ft2.data_ptr = face;

        font_ptr++;
    }

    return true;
}

#endif

/* EOF */
