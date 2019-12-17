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

#define __VSF_DISP_CLASS_INHERIT
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
#include "./images/demo_images.h"
#include "./images/demo_images_data.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <math.h>

/*============================ MACROS ========================================*/
#define FREETYPE_FONT_PATH              "../../usrapp/mvc_demo_win/tgui_demo/wqy-microhei.ttc"
#define FREETYPE_FONT_SIZE              (24)
#define FREETYPE_LOAD_FLAGS             (FT_LOAD_RENDER)
#define FREETYPE_DEFAULT_DPI            72
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_pt(tgui_demo_t)
def_vsf_pt(tgui_demo_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static vk_disp_t* __disp;
static FT_Library __library;
static FT_Face __face;
static vsf_tgui_font_t __default_font;
/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/
static vsf_tgui_color_t vk_disp_sdl_get_pixel(vsf_tgui_location_t* ptLocation)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    return pixmap[ptLocation->nY * VSF_TGUI_HOR_MAX + ptLocation->nX];
}

static void vk_disp_sdl_set_pixel(vsf_tgui_location_t* ptLocation, vsf_tgui_color_t tColor)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    pixmap[ptLocation->nY * VSF_TGUI_HOR_MAX + ptLocation->nX] = tColor;
}

/********************************************************************************/

vsf_tgui_size_t vsf_tgui_sdl_idx_root_tile_get_size(const vsf_tgui_tile_t* ptTile)
{
    VSF_TGUI_ASSERT(ptTile != NULL);

    uint8_t chIndex = ptTile->tIndexRoot.chIndex;
    if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGB) {           // RGB color
        VSF_TGUI_ASSERT(chIndex < dimof(gIdxRootRGBTileSizes));
        return gIdxRootRGBTileSizes[chIndex];
    } else if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGBA) {    // RGBA color
        VSF_TGUI_ASSERT(chIndex < dimof(gIdxRootRGBATileSizes));
        return gIdxRootRGBATileSizes[chIndex];
    } else {
        VSF_TGUI_ASSERT(0);
    }
}

char* vsf_tgui_sdl_tile_get_pixelmap(const vsf_tgui_tile_t* ptTile)
{
    VSF_TGUI_ASSERT(ptTile != NULL);

    if (ptTile->tCore.tAttribute.u2RootTileType == 0) {     // buf tile
        return ptTile->tBufRoot.ptBitmap;
    } else {                                                // index tile
        uint8_t chIndex = ptTile->tIndexRoot.chIndex;
        if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGB) {           // RGB color
            VSF_TGUI_ASSERT(chIndex < dimof(rgb_pixmap_array));
            return (char *)rgb_pixmap_array[chIndex];
        } else if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGBA) {    // RGBA color
            VSF_TGUI_ASSERT(chIndex < dimof(rgba_pixmap_array));
            return (char *)rgba_pixmap_array[chIndex];
        } else {
            VSF_TGUI_ASSERT(0);
        }

        return NULL;
    }
}

/*********************************************************************************/
static uint8_t freetype_get_char_height(void)
{
    /*
     * msyh test
        | fontsize | fontsize | fontsize | ¨(U+2589) height |
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

     //return __face->size->metrics.height >> 6;

     // https://stackoverflow.com/questions/26486642/whats-the-proper-way-of-getting-text-bounding-box-in-freetype-2
     // https://github.com/cocos2d/cocos2d-x/blob/b26e1bb08648c11a9482d0736a236aebc345008a/cocos/2d/CCFontFreeType.cpp#L180
     //return (__face->size->metrics.ascender - __face->size->metrics.descender) >> 6;

     //double pixel_size = FREETYPE_FONT_SIZE * FREETYPE_DEFAULT_DPI / 72;
     //return round((__face->bbox.yMax - __face->bbox.yMin) * pixel_size / __face->units_per_EM);

     // Line Spacing: 1.5 * height
    return (__face->size->metrics.height >> 6) * 12 / 10;
}

uint8_t vsf_tgui_proportional_font_get_char_width(const vsf_tgui_font_t* ptFont, uint32_t wChar)
{
    if (FT_Err_Ok == FT_Load_Char(__face, wChar, FREETYPE_LOAD_FLAGS)) {
        return __face->glyph->advance.x >> 6;
    } else {
        return 0;
    }
}

const vsf_tgui_font_t* vsf_tgui_sdl_font_get_default(void)
{
    return &__default_font;
}


/*********************************************************************************/

void vsf_tgui_draw_rect(vsf_tgui_location_t* ptLocation, vsf_tgui_size_t* ptSize, vsf_tgui_color_t tColor)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT((0 <= ptLocation->nX) && (ptLocation->nX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->nY) && (ptLocation->nY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->nX + ptSize->nWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->nX + ptSize->nWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->nY + ptSize->nHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->nY + ptSize->nHeight) <= VSF_TGUI_VER_MAX);

    vsf_tgui_color_t* pixmap = __disp->ui_data;

    for (uint16_t i = 0; i < ptSize->nHeight; i++) {
        for (uint16_t j = 0; j < ptSize->nWidth; j++) {
            vsf_tgui_location_t tPixelLocation = { .nX = ptLocation->nX + j, .nY = ptLocation->nY + i };
            vsf_tgui_color_t tPixelColor = vk_disp_sdl_get_pixel(&tPixelLocation);
            tPixelColor = vsf_tgui_color_mix(tColor, tPixelColor, tColor.chA);
            vk_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
        }
    }
}

void vsf_tgui_draw_root_tile(vsf_tgui_location_t* ptLocation,
                        vsf_tgui_location_t* ptTileLocation,
                        vsf_tgui_size_t* ptSize,
                        const vsf_tgui_tile_t* ptTile)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptTileLocation != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);
    VSF_TGUI_ASSERT(ptTile != NULL);
    VSF_TGUI_ASSERT((0 <= ptLocation->nX) && (ptLocation->nX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->nY) && (ptLocation->nY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->nX + ptSize->nWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->nX + ptSize->nWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->nY + ptSize->nHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->nY + ptSize->nHeight) <= VSF_TGUI_VER_MAX);
    VSF_TGUI_ASSERT(vsf_tgui_tile_is_root(ptTile));

    vsf_tgui_size_t tTileSize = vsf_tgui_root_tile_get_size(ptTile);
    VSF_TGUI_ASSERT(ptTileLocation->nX < tTileSize.nWidth);
    VSF_TGUI_ASSERT(ptTileLocation->nY < tTileSize.nHeight);

    vsf_tgui_region_t tDisplay;
    tDisplay.tLocation = *ptLocation;
    tDisplay.tSize.nWidth = min(ptSize->nWidth, tTileSize.nWidth - ptTileLocation->nX);
    tDisplay.tSize.nHeight = min(ptSize->nHeight, tTileSize.nHeight - ptTileLocation->nY);
    if (tDisplay.tSize.nHeight <= 0 || tDisplay.tSize.nWidth <= 0) {
        return ;
    }

    uint32_t wSize;
    if (ptTile->tCore.tAttribute.u3ColorSize == VSF_TGUI_COLORSIZE_32IT) {
        wSize = 4;
    } else if (ptTile->tCore.tAttribute.u3ColorSize == VSF_TGUI_COLORSIZE_24IT) {
        wSize = 3;
    } else {
        VSF_TGUI_ASSERT(0);
    }
    const char* pchPixelmap = vsf_tgui_sdl_tile_get_pixelmap(ptTile);

    for (uint16_t i = 0; i < tDisplay.tSize.nHeight; i++) {
        uint32_t wOffset = wSize * ((ptTileLocation->nY + i) * tTileSize.nWidth + ptTileLocation->nX);
        const char* pchData = pchPixelmap + wOffset;

        for (uint16_t j = 0; j < tDisplay.tSize.nWidth; j++) {
            vsf_tgui_location_t tPixelLocation = { .nX = tDisplay.tLocation.nX + j, .nY = tDisplay.tLocation.nY + i };
            vsf_tgui_color_t tTileColor;
            tTileColor.chR = *pchData++;
            tTileColor.chG = *pchData++;
            tTileColor.chB = *pchData++;

            if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGBA) {
                tTileColor.chA = *pchData++;
                vsf_tgui_color_t tPixelColor = vk_disp_sdl_get_pixel(&tPixelLocation);
                tPixelColor = vsf_tgui_color_mix(tTileColor, tPixelColor, tTileColor.chA);
                vk_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
            } else if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGB) {
                tTileColor.chA = 0xFF;
                vk_disp_sdl_set_pixel(&tPixelLocation, tTileColor);
            } else {
                VSF_TGUI_ASSERT(0);
            }
        }
    }
}

void vsf_tgui_draw_char(vsf_tgui_location_t* ptLocation, vsf_tgui_location_t* ptFontLocation, vsf_tgui_size_t* ptSize, uint32_t wChar, vsf_tgui_color_t tColor)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptFontLocation != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);

    if (FT_Err_Ok == FT_Load_Char(__face, wChar, FREETYPE_LOAD_FLAGS)) {
        FT_GlyphSlot glyph = __face->glyph;
        uint32_t wBaseLine = __face->size->metrics.ascender >> 6;
        int32_t wTop = glyph->bitmap_top;
        int32_t wLeft = glyph->bitmap_left;
        VSF_TGUI_ASSERT(wBaseLine >= wTop);

        vsf_tgui_location_t tBitmapStart = {
            .nX = max(0, wLeft),
            .nY = wBaseLine - wTop,
        };
        vsf_tgui_region_t tUpdateRegion = {
            .tLocation = *ptFontLocation,
            .tSize = *ptSize,
        };
        vsf_tgui_region_t tBitmapRegion = {
            .tLocation = tBitmapStart,
            .tSize = {.nWidth = glyph->bitmap.width, .nHeight = glyph->bitmap.rows},
        };
        vsf_tgui_region_t tRealBitmapRegion;
        if (vsf_tgui_region_intersect(&tRealBitmapRegion, &tUpdateRegion, &tBitmapRegion)) {
            uint32_t wXOffset = tRealBitmapRegion.tLocation.nX - tBitmapStart.nX;
            uint32_t wYOffset = tRealBitmapRegion.tLocation.nY - tBitmapStart.nY;

            for (uint16_t j = 0; j < tRealBitmapRegion.tSize.nHeight; j++) {
                for (uint16_t i = 0; i < tRealBitmapRegion.tSize.nWidth; i++) {
                    vsf_tgui_location_t tFontLocation = { 
                        .nX = wXOffset + i,
                        .nY = wYOffset + j
                    };
                    vsf_tgui_location_t tPixelLocation = { 
                        .nX = ptLocation->nX + tRealBitmapRegion.tLocation.nX + i,
                        .nY = ptLocation->nY + tRealBitmapRegion.tLocation.nY + j
                    };
                    uint8_t mix = glyph->bitmap.buffer[j * glyph->bitmap.width + i];// vsf_tgui_font_get_pixel_color(NULL, wChar, &tFontLocation);
                    vsf_tgui_color_t tPixelColor = vk_disp_sdl_get_pixel(&tPixelLocation);
                    tPixelColor = vsf_tgui_color_mix(tColor, tPixelColor, mix);
                    vk_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
                }
            }

        }
    } else {
        VSF_TGUI_ASSERT(0);
    }
}

const vsf_tgui_sv_panel_tiles_t gPanelAdditionalTiles = {
    .tTopLeft = {
        .tChild = {
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tTopRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 200 - 12, .nY = 0},
        },
    },
    .tBottomLeft = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 0, .nY = 200 - 12},
        },
    },
    .tBottomRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 200 - 12, .nY = 200 - 12},
        },
    },
};

const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles = {
    .tLeft = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tRight = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.nX = 16, .nY = 0},
        },
    },
};


/**********************************************************************************/
void refresh_all(void)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    memset(pixmap, 0xFF, VSF_TGUI_HOR_MAX * VSF_TGUI_VER_MAX * sizeof(vsf_tgui_color_t));

    extern void refresh_my_stopwatch(void);
    refresh_my_stopwatch();
}

/*! \brief begin a refresh loop
 *! \param ptGUI the tgui object address
 *! \param ptPlannedRefreshRegion the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 */
vsf_tgui_region_t *vsf_tgui_v_refresh_loop_begin(   
                    vsf_tgui_t *ptGUI, 
                    const vsf_tgui_region_t *ptPlannedRefreshRegion)
{
    return (vsf_tgui_region_t *)ptPlannedRefreshRegion;
}

volatile static bool __is_ready_to_refresh = true;

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t* ptGUI)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    vk_disp_area_t area = {
        .pos = {.x = 0, .y = 0},
        .size = {.x = VSF_TGUI_HOR_MAX, .y = VSF_TGUI_VER_MAX},
    };
    __vsf_sched_safe(
        if (__is_ready_to_refresh) {
            __is_ready_to_refresh = false;
            vk_disp_refresh(__disp, &area, pixmap);
        }
    )
    return false;
}

static void vsf_tgui_on_ready(vk_disp_t* disp)
{
    __vsf_sched_safe(
        if (!__is_ready_to_refresh) {
            __is_ready_to_refresh = true;
        }
        extern void vsf_tgui_demo_on_ready(void);

        vsf_tgui_demo_on_ready();
        
    )
}

bool vsf_tgui_port_is_ready_to_refresh(void)
{
    return __is_ready_to_refresh;
}


bool vsf_tgui_font_init(void)
{
    if (FT_Err_Ok != FT_Init_FreeType(&__library)) {
        return false;
    }
    if (FT_Err_Ok != FT_New_Face(__library, FREETYPE_FONT_PATH, 0, &__face)) {
        return false;
    }

    if (FT_Err_Ok != FT_Set_Char_Size(__face, 0, FREETYPE_FONT_SIZE * 64, 0, 0)) {
        return false;
    }

    __default_font.chHeight = freetype_get_char_height();
    __default_font.chFlags = VSF_TGUI_FONT_CONTINUOUS | VSF_TGUI_FONT_PROPORTIONAL;
    __default_font.chIndex = 0;

    return true;
}


void vsf_tgui_bind(vk_disp_t* disp, void* ui_data)
{
    disp->ui_data = ui_data;
    disp->ui_on_ready = vsf_tgui_on_ready;
    vk_disp_init(disp);

    bool result = vsf_tgui_font_init();
    VSF_TGUI_ASSERT(result == true);

    __disp = disp;
}
#endif

/* EOF */
