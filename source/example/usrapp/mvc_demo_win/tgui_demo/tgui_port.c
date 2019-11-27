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
#include "./fonts/demo_font.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_pt(tgui_demo_t)

def_vsf_pt(tgui_demo_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static vsf_disp_t* __disp;

/*============================ PROTOTYPES ====================================*/

/*============================ IMPLEMENTATION ================================*/
static vsf_tgui_color_t vsf_disp_sdl_get_pixel(vsf_tgui_location_t* ptLocation)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    return pixmap[ptLocation->nY * VSF_TGUI_HOR_MAX + ptLocation->nX];
}

static void vsf_disp_sdl_set_pixel(vsf_tgui_location_t* ptLocation, vsf_tgui_color_t tColor)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    pixmap[ptLocation->nY * VSF_TGUI_HOR_MAX + ptLocation->nX] = tColor;
}

static uint8_t vsf_tgui_font_get_pixel_color(void* ptFont, uint32_t wChar, vsf_tgui_location_t* ptLocation)
{
    //VSF_TGUI_ASSERT(font!= NULL);
    // todo: check wChar
    VSF_TGUI_ASSERT(ptLocation != NULL);
    // todo: check font location

    return _fonts[(wChar - 0x20) * FONT_SIZE * FONT_SIZE + ptLocation->nY * FONT_SIZE + ptLocation->nX];
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
            return rgb_pixmap_array[chIndex];
        } else if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGBA) {    // RGBA color
            VSF_TGUI_ASSERT(chIndex < dimof(rgba_pixmap_array));
            return rgba_pixmap_array[chIndex];
        } else {
            VSF_TGUI_ASSERT(0);
        }

        return NULL;
    }
}

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
            vsf_tgui_color_t tPixelColor = vsf_disp_sdl_get_pixel(&tPixelLocation);
            tPixelColor = vsf_tgui_color_mix(tColor, tPixelColor, tColor.chA);
            vsf_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
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
                vsf_tgui_color_t tPixelColor = vsf_disp_sdl_get_pixel(&tPixelLocation);
                tPixelColor = vsf_tgui_color_mix(tTileColor, tPixelColor, tTileColor.chA);
                vsf_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
            } else if (ptTile->tCore.tAttribute.u2ColorType == VSF_TGUI_COLORTYPE_RGB) {
                tTileColor.chA = 0xFF;
                vsf_disp_sdl_set_pixel(&tPixelLocation, tTileColor);
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

    for (uint16_t j = 0; j < ptSize->nHeight; j++) {
        for (uint16_t i = 0; i < ptSize->nWidth; i++) {
            vsf_tgui_location_t tFontLocation = { .nX = ptFontLocation->nX + i, .nY = ptFontLocation->nY + j };
            vsf_tgui_location_t tPixelLocation = { .nX = ptLocation->nX + i, .nY = ptLocation->nY + j };
            uint8_t mix = vsf_tgui_font_get_pixel_color(NULL, wChar, &tFontLocation);
            vsf_tgui_color_t tPixelColor = vsf_disp_sdl_get_pixel(&tPixelLocation);
            tPixelColor = vsf_tgui_color_mix(tColor, tPixelColor, mix);
            vsf_disp_sdl_set_pixel(&tPixelLocation, tPixelColor);
        }
    }
}

const vsf_tgui_sv_panel_tiles_t gPanelAdditionalTiles = {
    .tTopLeft = {
        .tChild = {
            .ptParent = &bg1_RGB,
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tTopRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = &bg1_RGB,
            .tLocation = {.nX = 200 - 12, .nY = 0},
        },
    },
    .tBottomLeft = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = &bg1_RGB,
            .tLocation = {.nX = 0, .nY = 200 - 12},
        },
    },
    .tBottomRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = &bg1_RGB,
            .tLocation = {.nX = 200-12, .nY = 200-12},
        },
    },
};

const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles = {
    .tLeft = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = &bg3_RGB,
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tRight = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = &bg3_RGB,
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
    return ptPlannedRefreshRegion;
}

volatile static __is_ready_to_refresh = true;

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t *ptGUI)
{
    vsf_tgui_color_t* pixmap = __disp->ui_data;
    vsf_disp_area_t area = {
        .pos = {.x = 0, .y = 0},
        .size = {.x = VSF_TGUI_HOR_MAX, .y = VSF_TGUI_VER_MAX},
    };
    __vsf_sched_safe(
        if (__is_ready_to_refresh) {
            __is_ready_to_refresh = false;
            vsf_disp_refresh(__disp, &area, pixmap);
        }
    )
    return false;
}



static void vsf_tgui_on_ready(vsf_disp_t* disp)
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


void vsf_tgui_bind(vsf_disp_t* disp, void* ui_data)
{
    disp->ui_data = ui_data;
    disp->ui_on_ready = vsf_tgui_on_ready;
    vsf_disp_init(disp);

    extern void my_stopwatch_init(void);
    my_stopwatch_init();

    __disp = disp;
}
#endif

/* EOF */
