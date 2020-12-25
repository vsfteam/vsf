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
extern uint8_t vsf_tgui_font_get_char_height(const uint8_t chFontIndex);
extern uint8_t vsf_tgui_font_get_char_width(const uint8_t chFontIndex, uint32_t wChar);

/*============================ IMPLEMENTATION ================================*/

uint32_t vsf_tgui_text_get_next(const char* pchString, size_t* ptSize)
{
#if VSF_TGUI_CFG_TEXT_MODE == VSF_TGUI_TEXT_ASCII
    uint32_t wChar;

    VSF_TGUI_ASSERT(pchString != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);

    wChar = pchString[*ptSize];
    (*ptSize)++;
    return wChar;
#elif VSF_TGUI_CFG_TEXT_MODE == VSF_TGUI_TEXT_UTF8
    uint32_t wFirst;
    uint32_t wSecond;
    uint32_t wThird;
    uint32_t wFourth;

    VSF_TGUI_ASSERT(pchString != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);

    // UTF-8
    // 0zzzzzzz (00-7F)                              ==> 00000000 00000000 00000000 0zzzzzzz
    // 110yyyyy（C0-DF） 10zzzzzz（80-BF）           ==> 00000000 00000000 00000yyy yyzzzzzz
    // 1110xxxx（E0-EF） 10yyyyyy 10zzzzzz           ==> 00000000 00000000 xxxxyyyy yyzzzzzz
    // 11110www（F0-F7） 10xxxxxx 10yyyyyy 10zzzzzz  ==> 00000000 000wwwxx xxxxyyyy yyzzzzzz
    wFirst = (uint8_t)pchString[*ptSize];
    (*ptSize)++;

    if ((wFirst & 0x80) != 0x80) {                  // U+0000 ~ U+007F
        return wFirst;
    }

    if ((wFirst & 0xE0) == 0xC0) {                  // U+0080 ~ U+07FF
        wSecond = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wSecond & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((wFirst & 0x1F) << 6) + (wSecond & 0x3F);
    }

    if ((wFirst & 0xF0) == 0xE0) {                  // U+0800 ~ U+FFFF
        wSecond = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wSecond & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        wThird = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wThird & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((wFirst & 0x0F) << 12) + ((wSecond & 0x3F) << 6) + (wThird & 0x3F);
    }

    if ((wFirst & 0xF8) == 0xF0) {                  // U+10000 ~ U+10FFFF
        wSecond = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wSecond & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        wThird = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wThird & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        wFourth = (uint8_t)pchString[*ptSize];
        (*ptSize)++;

        if ((wFourth & 0xC0) != 0x80) {
            VSF_TGUI_ASSERT(0);
            return 0;
        }

        return ((wFirst & 0x07) << 18) + ((wSecond & 0x3F) << 12) + ((wThird & 0x3F) << 6) + (wFourth & 0x3F);
    }

    VSF_TGUI_ASSERT(0);
    return 0;
#else
#   error "Unsupported encoding format"
#endif
}

#if VSF_TGUI_CFG_TEXT_MODE == VSF_TGUI_TEXT_UTF8

#if 0
struct text_test_t {
    uint32_t wValue;
    char* pchString;
};
static struct text_test_t _test_case[] = {
    {/*.wValue = */0x00000001, /*.pchString = */"\x01"},
    {/*.wValue = */0x0000007F, /*.pchString = */"\x7F"},
    {/*.wValue = */0x00000080, /*.pchString = */"\xC2\x80"},
    {/*.wValue = */0x000007FF, /*.pchString = */"\xDF\xBF"},
    {/*.wValue = */0x00000800, /*.pchString = */"\xE0\xA0\x80"},
    {/*.wValue = */0x0000FFFF, /*.pchString = */"\xEF\xBF\xBF"},
    {/*.wValue = */0x00010000, /*.pchString = */"\xF0\x90\x80\x80"},
    {/*.wValue = */0x0010FFFF, /*.pchString = */"\xF4\x8F\xBF\xBF"},

};

void vsf_tgui_text_get_next_test_case0(void)
{
    int i;
    size_t tSize;
    uint32_t wValue;

    for (i = 0; i < dimof(_test_case); i++) {
        tSize = 0;
        wValue = vsf_tgui_text_get_next(_test_case[i].pchString, &tSize);

        if (_test_case[i].wValue != wValue) {
            while (1);
        }
    }
}
#endif
#endif

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
vsf_tgui_string_t* vsf_tgui_text_get_line(  vsf_tgui_string_t* ptStringIn,
                                            int16_t *piOffSet,
                                            vsf_tgui_string_t* ptStringOut)
{
    uint32_t wChar;
    size_t tCharOffset;
    bool bFindFirstChar = true;
    size_t tStart;

    VSF_TGUI_ASSERT(NULL != ptStringIn);
    VSF_TGUI_ASSERT(NULL != piOffSet);
    VSF_TGUI_ASSERT(NULL != ptStringOut);

    tCharOffset = *piOffSet;
    tStart = tCharOffset;

    if (    (NULL == ptStringIn->pstrText)
        ||  (ptStringIn->s16_size <= 0)
        ||  (tCharOffset >= ptStringIn->s16_size)) {
        return NULL;
    }

    ptStringOut->pstrText = (VSF_TGUI_CFG_STRING_TYPE *)(ptStringIn->pstrText);

    while ( ((wChar = vsf_tgui_text_get_next(ptStringIn->pstrText, &tCharOffset)) != '\0')
        &&  (tCharOffset <= ptStringIn->s16_size)
    ) {
        if (wChar == '\n') {
            break;
        }

        if (bFindFirstChar) {
            if (wChar == '\r') {
                tStart = tCharOffset; // next char offset
            } else {
                bFindFirstChar = false;
                ptStringOut->pstrText = (VSF_TGUI_CFG_STRING_TYPE*)(ptStringIn->pstrText) + tStart;
            }
        }
    }

    ptStringOut->s16_size = tCharOffset - tStart - 1; // \n always is 1 byte
    *piOffSet = tCharOffset;

    return ptStringOut;
}
#endif

vsf_tgui_size_t vsf_tgui_text_get_size( const uint8_t chFontIndex,
                                        vsf_tgui_string_t* ptString,
                                        uint16_t *phwLineCount,
                                        uint8_t *pchCharHeight,
                                        int_fast8_t chInterlineSpace)
{
    vsf_tgui_size_t tSize = {0, 0};
    int16_t iWidth = 0;
    int16_t iFontHeight;
    uint32_t wChar;
    size_t tCharOffset = 0;
    uint8_t chWidth;
    uint_fast16_t hwLineCount = 0;

    VSF_TGUI_ASSERT(NULL != ptString);

    if (    (NULL != ptString->pstrText)
    #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (ptString->s16_size > 0)
    #endif
    ) {

        iFontHeight = vsf_tgui_font_get_char_height(chFontIndex);

        while ( ((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
        #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
            &&  (tCharOffset <= ptString->s16_size)
        #endif
        ) {
            if (wChar == '\n') {
        #if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
                tSize.iWidth = max(tSize.iWidth, iWidth);
                iWidth = 0;
                hwLineCount++;
        #endif
                continue;
            }

            if (wChar == '\r') {
                continue;
            }

            chWidth = vsf_tgui_font_get_char_width(chFontIndex, wChar);
            if (chWidth > 0) {
                iWidth += chWidth;
            }
        }

        tSize.iWidth = max(tSize.iWidth, iWidth);
        tSize.iHeight = hwLineCount * (iFontHeight + chInterlineSpace) + iFontHeight;
        VSF_TGUI_ASSERT(tSize.iHeight > 0);
        hwLineCount++;
    }

    if (NULL != phwLineCount) {
        *phwLineCount = hwLineCount;
    }
    if (NULL != pchCharHeight) {
        *pchCharHeight = iFontHeight;
    }

    return tSize;
}




#endif


/* EOF */
