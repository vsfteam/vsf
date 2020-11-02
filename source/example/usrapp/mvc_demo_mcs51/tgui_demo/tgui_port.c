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
#include "./fonts/demo_font.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
extern void vsf_tgui_demo_on_ready(void);
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
static vsf_tgui_color_t vsf_tgui_get_pixel(vsf_tgui_location_t* ptLocation)
{
    vsf_tgui_sv_color_t tColor = VSF_TGUI_COLOR_BLACK;
    return tColor.tColor;
}

static void vsf_tgui_set_pixel(vsf_tgui_location_t* ptLocation, vsf_tgui_color_t tColor)
{
}

static uint8_t vsf_tgui_font_get_pixel_color(void* ptFont, uint32_t wChar, vsf_tgui_location_t* ptLocation)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    return 0x55;
}

uint8_t vsf_tgui_proportional_font_get_char_width(const vsf_tgui_font_t* ptFont, uint32_t wChar)
{
    return 55;
}

/********************************************************************************/

void vsf_tgui_draw_rect(vsf_tgui_location_t* ptLocation, vsf_tgui_size_t* ptSize, vsf_tgui_color_t tRectColor)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT((0 <= ptLocation->iX) && (ptLocation->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->iY) && (ptLocation->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->iX + ptSize->iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iX + ptSize->iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->iY + ptSize->iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iY + ptSize->iHeight) <= VSF_TGUI_VER_MAX);

    for (uint16_t i = 0; i < ptSize->iHeight; i++) {
        for (uint16_t j = 0; j < ptSize->iWidth; j++) {
            vsf_tgui_location_t tPixelLocation = { .iX = ptLocation->iX + j, .iY = ptLocation->iY + i };
            vsf_tgui_color_t tPixelColor = vsf_tgui_get_pixel(&tPixelLocation);
            tPixelColor = vsf_tgui_color_mix(tRectColor, tPixelColor, tRectColor.tChannel.chA);
            vsf_tgui_set_pixel(&tPixelLocation, tPixelColor);
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
    VSF_TGUI_ASSERT((0 <= ptLocation->iX) && (ptLocation->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->iY) && (ptLocation->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->iX + ptSize->iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iX + ptSize->iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->iY + ptSize->iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iY + ptSize->iHeight) <= VSF_TGUI_VER_MAX);
    VSF_TGUI_ASSERT(vsf_tgui_tile_is_root(ptTile));

    vsf_tgui_size_t tTileSize = vsf_tgui_root_tile_get_size(ptTile);
    VSF_TGUI_ASSERT(ptTileLocation->iX < tTileSize.iWidth);
    VSF_TGUI_ASSERT(ptTileLocation->iY < tTileSize.iHeight);

    const vsf_tgui_tile_buf_root_t* ptBufTile = &ptTile->tBufRoot;
    const vsf_tgui_tile_core_t* ptCoreTile = &ptBufTile->use_as__vsf_tgui_tile_core_t;
    uint32_t u32_size = (ptCoreTile->Attribute.u3ColorSize == VSF_TGUI_COLOR_ARGB_8888) ? 4 : 3;

    vsf_tgui_region_t tDisplay;
    tDisplay.tLocation = *ptLocation;
    tDisplay.tSize.iWidth = min(ptSize->iWidth, tTileSize.iWidth - ptTileLocation->iX);
    tDisplay.tSize.iHeight = min(ptSize->iHeight, tTileSize.iHeight - ptTileLocation->iY);
    if (tDisplay.tSize.iHeight <= 0 || tDisplay.tSize.iWidth <= 0) {
        return ;
    }

    for (uint16_t i = 0; i < tDisplay.tSize.iHeight; i++) {
        uint32_t u32_offset = u32_size * ((ptTileLocation->iY + i) * tTileSize.iWidth + ptTileLocation->iX);
        const char* pchData = (const char *)ptBufTile->ptBitmap;
        pchData += u32_offset;

        for (uint16_t j = 0; j < tDisplay.tSize.iWidth; j++) {
            vsf_tgui_location_t tPixelLocation = { .iX = tDisplay.tLocation.iX + j, .iY = tDisplay.tLocation.iY + i };
            vsf_tgui_color_t tTileColor;
            tTileColor.tChannel.chR = *pchData++;
            tTileColor.tChannel.chG = *pchData++;
            tTileColor.tChannel.chB = *pchData++;

            if (ptCoreTile->Attribute.u3ColorSize == VSF_TGUI_COLOR_ARGB_8888) {
                tTileColor.tChannel.chA = *pchData++;
                vsf_tgui_color_t tPixelColor = vsf_tgui_get_pixel(&tPixelLocation);
                tPixelColor = vsf_tgui_color_mix(tTileColor, tPixelColor, tTileColor.tChannel.chA);
                vsf_tgui_set_pixel(&tPixelLocation, tPixelColor);
            } else if (ptCoreTile->Attribute.u3ColorSize == VSF_TGUI_COLOR_RGB8_USER_TEMPLATE) {
                tTileColor.tChannel.chA = 0xFF;
                vsf_tgui_set_pixel(&tPixelLocation, tTileColor);
            }
        }
    }
}

void vsf_tgui_draw_char(vsf_tgui_location_t* ptLocation, vsf_tgui_location_t* ptFontLocation, vsf_tgui_size_t* ptSize, uint32_t wChar, vsf_tgui_color_t tCharColor)
{
    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptFontLocation != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);

    for (uint16_t j = 0; j < ptSize->iHeight; j++) {
        for (uint16_t i = 0; i < ptSize->iWidth; i++) {
            vsf_tgui_location_t tFontLocation = { .iX = ptFontLocation->iX + i, .iY = ptFontLocation->iY + j };
            vsf_tgui_location_t tPixelLocation = { .iX = ptLocation->iX + i, .iY = ptLocation->iY + j };
            uint8_t mix = vsf_tgui_font_get_pixel_color(NULL, wChar, &tFontLocation);
            vsf_tgui_color_t tPixelColor = vsf_tgui_get_pixel(&tPixelLocation);
            tPixelColor = vsf_tgui_color_mix(tCharColor, tPixelColor, mix);
            vsf_tgui_set_pixel(&tPixelLocation, tPixelColor);
        }
    }
}

const vsf_tgui_sv_container_corner_tiles_t g_tContainerCornerTiles = {
    .tTopLeft = {
        .tChild = {
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg1_RGB,
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    .tTopRight = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg1_RGB,
            .tLocation = {.iX = 200 - 12, .iY = 0},
        },
    },
    .tBottomLeft = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg1_RGB,
            .tLocation = {.iX = 0, .iY = 200 - 12},
        },
    },
    .tBottomRight = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg1_RGB,
            .tLocation = {.iX = 200-12, .iY = 200-12},
        },
    },
};

const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles = {
    .tLeft = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg3_RGB,
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    .tRight = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t *)&bg3_RGB,
            .tLocation = {.iX = 16, .iY = 0},
        },
    },
 };


/**********************************************************************************/

/*! \brief begin a refresh loop
 *! \param gui_ptr the tgui object address
 *! \param ptPlannedRefreshRegion the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 */
vsf_tgui_region_t *vsf_tgui_v_refresh_loop_begin(   
                    vsf_tgui_t *gui_ptr, 
                    const vsf_tgui_region_t *ptPlannedRefreshRegion)
{
    return (vsf_tgui_region_t *)ptPlannedRefreshRegion;
}


volatile static bool s_bIsReadyToRefresh = true;

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t* gui_ptr)
{
    vk_disp_area_t area = {
        .pos = {.x = 0, .y = 0},
        .size = {.x = VSF_TGUI_HOR_MAX, .y = VSF_TGUI_VER_MAX},
    };
    __vsf_sched_safe(
        if (s_bIsReadyToRefresh) {
            s_bIsReadyToRefresh = false;
            // todo: refresh
        }
    )
    return false;
}

static void vsf_tgui_on_ready(vk_disp_t* disp)
{
    __vsf_sched_safe(
        if (!s_bIsReadyToRefresh) {
            s_bIsReadyToRefresh = true;
        }
        vsf_tgui_demo_on_ready();
    )
}

bool vsf_tgui_port_is_ready_to_refresh(void)
{
    return s_bIsReadyToRefresh;
}

#endif

/* EOF */
