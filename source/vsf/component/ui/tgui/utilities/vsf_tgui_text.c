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

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

declare_class(vsf_tgui_t)
#include "../controls/vsf_tgui_controls.h"
#include "./vsf_tgui_text.h"
#include "./vsf_tgui_font.h"
#include "../view/simple_view/vsf_tgui_sv_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
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
    size_t tStart = 0;

    VSF_TGUI_ASSERT(NULL != ptStringIn);
    VSF_TGUI_ASSERT(NULL != piOffSet);
    VSF_TGUI_ASSERT(NULL != ptStringOut);

    tCharOffset = *piOffSet;

    if (    (NULL == ptStringIn->pstrText)
        ||  (ptStringIn->iSize <= 0)
        ||  (tCharOffset >= ptStringIn->iSize)) {
        return NULL;
    }

    ptStringOut->pstrText = (uintptr_t)(ptStringIn->pstrText);

    while ( ((wChar = vsf_tgui_text_get_next(ptStringIn->pstrText, &tCharOffset)) != '\0')
        &&  (tCharOffset <= ptStringIn->iSize)
    ) {
        if (wChar == '\n') {
            break;
        } else if (wChar == '\r') {
            continue;
        }

        if (bFindFirstChar) {
            bFindFirstChar = false;
            ptStringOut->pstrText = (uintptr_t)(ptStringIn->pstrText) + tCharOffset - 1;
            tStart = (tCharOffset - 1);
        }
    }

    ptStringOut->iSize = tCharOffset - tStart - 1;
    *piOffSet = tCharOffset;

    return ptStringOut;
}       
#endif                                   

vsf_tgui_size_t vsf_tgui_text_get_size( const uint8_t chFontIndex, 
                                        vsf_tgui_string_t* ptString,
                                        uint16_t *phwLineCount,
                                        int_fast8_t chInterlineSpace)
{
    vsf_tgui_size_t tSize = {0, 0};
    int16_t iWidth = 0;
    int16_t nFontHeight;
    uint32_t wChar;
    size_t tCharOffset = 0;
    uint8_t chWidth;
    uint_fast16_t hwLineCount = 0;
    bool bFoundANewLine = true;

    VSF_TGUI_ASSERT(NULL != ptString);

    if (    (NULL != ptString->pstrText)
    #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (ptString->iSize > 0)
    #endif
    ) {
        
        nFontHeight = vsf_tgui_font_get_char_height(chFontIndex);

        while ( ((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
        #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
            &&  (tCharOffset <= ptString->iSize)
        #endif
        ) {
        #if VSF_TGUI_SV_CFG_MULTI_LINE_TEXT == ENABLED
            if (wChar == '\n') {
                tSize.iHeight += nFontHeight + chInterlineSpace;
                tSize.iWidth = max(tSize.iWidth, iWidth);
                iWidth = 0;
                bFoundANewLine = true;
            } else if (wChar == '\r') {
                continue;
            } else
        #endif
            {
                if (bFoundANewLine) {
                    bFoundANewLine = false;
                    hwLineCount++;
                }
                chWidth = vsf_tgui_font_get_char_width(chFontIndex, wChar);
                if (chWidth > 0) {
                    iWidth += chWidth;
                }
            }
        }

        if (iWidth) {
            tSize.iHeight += nFontHeight;
        }

        tSize.iWidth = max(tSize.iWidth, iWidth);
    }

    if (NULL != phwLineCount) {
        *phwLineCount = hwLineCount;
    }

    return tSize;
}

void vsf_tgui_text_draw(vsf_tgui_location_t* ptLocation,
                        vsf_tgui_region_t* ptRelativeRegion,
                        vsf_tgui_string_t* ptString,
                        const uint8_t chFontIndex,
                        vsf_tgui_color_t tColor,
                        int_fast8_t chInterLineSpace)
{
    vsf_tgui_region_t tVisualCharRegion = {0};
    vsf_tgui_location_t tLocation;
    vsf_tgui_region_t tDirtyCharRegion = {0};
    vsf_tgui_location_t tRelativeCharLocation = {0};
    uint32_t wChar;
    size_t tCharOffset = 0;

    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptRelativeRegion != NULL);
    VSF_TGUI_ASSERT(ptString != NULL);
    VSF_TGUI_ASSERT(ptString->pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(ptString->iSize > 0);
#endif

    VSF_TGUI_ASSERT((0 <= ptLocation->iX) && (ptLocation->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->iY) && (ptLocation->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->iX + ptRelativeRegion->tSize.iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iX + ptRelativeRegion->tSize.iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->iY + ptRelativeRegion->tSize.iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iY + ptRelativeRegion->tSize.iHeight) <= VSF_TGUI_VER_MAX);

    tVisualCharRegion.tSize.iHeight = vsf_tgui_font_get_char_height(chFontIndex) + chInterLineSpace;

    tLocation = *ptLocation;

    while ( ((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (tCharOffset <= ptString->iSize)
#endif
    ) {
#if VSF_TGUI_SV_CFG_MULTI_LINE_TEXT == ENABLED
        if (wChar == '\n') {
            tVisualCharRegion.tLocation.iX = 0;
            tVisualCharRegion.tLocation.iY += tVisualCharRegion.tSize.iHeight;

            tLocation.iX = ptLocation->iX;
            if (tDirtyCharRegion.tSize.iHeight) {
                tLocation.iY += tDirtyCharRegion.tSize.iHeight;
            }
        } else if (wChar == '\r') {
            continue;
        } else
#endif
        {
            tVisualCharRegion.tSize.iWidth = vsf_tgui_font_get_char_width(chFontIndex, wChar);
            if (vsf_tgui_region_intersect(&tDirtyCharRegion, &tVisualCharRegion, ptRelativeRegion)) {
                tRelativeCharLocation.iX = tDirtyCharRegion.tLocation.iX - tVisualCharRegion.tLocation.iX,
                tRelativeCharLocation.iY = tDirtyCharRegion.tLocation.iY - tVisualCharRegion.tLocation.iY,
                
                //tLocation.iY = ptLocation->iY + tDirtyCharRegion.iY;
                
                vsf_tgui_draw_char(&tLocation, &tRelativeCharLocation, &tDirtyCharRegion.tSize, chFontIndex, wChar, tColor);
                tLocation.iX += tDirtyCharRegion.tSize.iWidth;
            }
            tVisualCharRegion.tLocation.iX += tVisualCharRegion.tSize.iWidth;
        }
    }
}


#endif


/* EOF */
