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
#include "../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED

declare_class(vsf_tgui_t)
#include "../controls/vsf_tgui_controls.h"
#include "./vsf_tgui_text.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index);
extern uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32);

/*============================ IMPLEMENTATION ================================*/

uint32_t vsf_tgui_text_get_next(const char* string_ptr, size_t* size_ptr)
{
#if VSF_TGUI_CFG_TEXT_MODE == VSF_TGUI_TEXT_ASCII
    uint32_t char_u32;

    VSF_TGUI_ASSERT(string_ptr != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

    char_u32 = string_ptr[*size_ptr];
    (*size_ptr)++;
    return char_u32;
#elif VSF_TGUI_CFG_TEXT_MODE == VSF_TGUI_TEXT_UTF8
    uint32_t first;
    uint32_t second;
    uint32_t third;
    uint32_t fourth;

    VSF_TGUI_ASSERT(string_ptr != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

    // UTF-8
    // 0zzzzzzz (00-7F)                              ==> 00000000 00000000 00000000 0zzzzzzz
    // 110yyyyy（C0-DF） 10zzzzzz（80-BF）           ==> 00000000 00000000 00000yyy yyzzzzzz
    // 1110xxxx（E0-EF） 10yyyyyy 10zzzzzz           ==> 00000000 00000000 xxxxyyyy yyzzzzzz
    // 11110www（F0-F7） 10xxxxxx 10yyyyyy 10zzzzzz  ==> 00000000 000wwwxx xxxxyyyy yyzzzzzz
    first = (uint8_t)string_ptr[*size_ptr];
    (*size_ptr)++;

    if ((first & 0x80) != 0x80) {                  // U+0000 ~ U+007F
        return first;
    }

    if ((first & 0xE0) == 0xC0) {                  // U+0080 ~ U+07FF
        second = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((second & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((first & 0x1F) << 6) + (second & 0x3F);
    }

    if ((first & 0xF0) == 0xE0) {                  // U+0800 ~ U+FFFF
        second = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((second & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        third = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((third & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((first & 0x0F) << 12) + ((second & 0x3F) << 6) + (third & 0x3F);
    }

    if ((first & 0xF8) == 0xF0) {                  // U+10000 ~ U+10FFFF
        second = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((second & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        third = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((third & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        fourth = (uint8_t)string_ptr[*size_ptr];
        (*size_ptr)++;

        if ((fourth & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((first & 0x07) << 18) + ((second & 0x3F) << 12) + ((third & 0x3F) << 6) + (fourth & 0x3F);
    }

    VSF_TGUI_ASSERT(0);
    return 0;
#else
#   error "Unsupported encoding format"
#endif
}

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
vsf_tgui_string_t* vsf_tgui_text_get_line(  vsf_tgui_string_t* string_in_ptr,
                                            int16_t *offSet_ptr,
                                            vsf_tgui_string_t* string_out_ptr)
{
    uint32_t char_u32;
    size_t char_offset;
    bool find_first_char = true;
    size_t start;

    VSF_TGUI_ASSERT(NULL != string_in_ptr);
    VSF_TGUI_ASSERT(NULL != offSet_ptr);
    VSF_TGUI_ASSERT(NULL != string_out_ptr);

    char_offset = *offSet_ptr;
    start = char_offset;

    if (    (NULL == string_in_ptr->pstrText)
        ||  (string_in_ptr->s16_size <= 0)
        ||  (char_offset >= string_in_ptr->s16_size)) {
        return NULL;
    }

    string_out_ptr->pstrText = (VSF_TGUI_CFG_STRING_TYPE *)(string_in_ptr->pstrText);

    while ( ((char_u32 = vsf_tgui_text_get_next(string_in_ptr->pstrText, &char_offset)) != '\0')
        &&  (char_offset <= string_in_ptr->s16_size)
    ) {
        if (char_u32 == '\n') {
            break;
        }

        if (find_first_char) {
            if (char_u32 == '\r') {
                start = char_offset; // next char offset
            } else {
                find_first_char = false;
                string_out_ptr->pstrText = (VSF_TGUI_CFG_STRING_TYPE*)(string_in_ptr->pstrText) + start;
            }
        }
    }

    string_out_ptr->s16_size = char_offset - start - 1; // \n always is 1 byte
    *offSet_ptr = char_offset;

    return string_out_ptr;
}
#endif

vsf_tgui_size_t vsf_tgui_text_get_size( const uint8_t font_index,
                                        vsf_tgui_string_t* string_ptr,
                                        uint16_t *line_count_ptr,
                                        uint8_t *char_height_ptr,
                                        int_fast8_t inter_line_space)
{
    vsf_tgui_size_t tSize = {0, 0};
    int16_t width = 0;
    int16_t font_height;
    uint32_t char_u32;
    size_t char_offset = 0;
    uint8_t char_width;
    uint_fast16_t hwLineCount = 0;

    VSF_TGUI_ASSERT(NULL != string_ptr);

    if (    (NULL != string_ptr->pstrText)
    #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (string_ptr->s16_size > 0)
    #endif
    ) {

        font_height = vsf_tgui_font_get_char_height(font_index);

        while ( ((char_u32 = vsf_tgui_text_get_next(string_ptr->pstrText, &char_offset)) != '\0')
        #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
            &&  (char_offset <= string_ptr->s16_size)
        #endif
        ) {
            if (char_u32 == '\n') {
        #if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
                tSize.iWidth = vsf_max(tSize.iWidth, width);
                width = 0;
                hwLineCount++;
        #endif
                continue;
            }

            if (char_u32 == '\r') {
                continue;
            }

            char_width = vsf_tgui_font_get_char_width(font_index, char_u32);
            if (char_width > 0) {
                width += char_width;
            }
        }

        tSize.iWidth = vsf_max(tSize.iWidth, width);
        tSize.iHeight = hwLineCount * (font_height + inter_line_space) + font_height;
        VSF_TGUI_ASSERT(tSize.iHeight > 0);
        hwLineCount++;
    }

    if (NULL != line_count_ptr) {
        *line_count_ptr = hwLineCount;
    }
    if (NULL != char_height_ptr) {
        *char_height_ptr = font_height;
    }

    return tSize;
}

#endif


/* EOF */
