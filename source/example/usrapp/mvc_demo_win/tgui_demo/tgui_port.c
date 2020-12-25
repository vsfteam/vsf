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

#define __VSF_DISP_CLASS_INHERIT__
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED

#include "./images/demo_images.h"
#include "./images/demo_images_data.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <math.h>
#include <string.h>

/*============================ MACROS ========================================*/
#define FREETYPE_FONT_PATH              "../usrapp/mvc_demo_win/tgui_demo/wqy-microhei.ttc"
#define FREETYPE_FONT_SIZE              (24)
#define FREETYPE_LOAD_FLAGS             (FT_LOAD_RENDER)
#define FREETYPE_DEFAULT_DPI            72

#ifndef VSF_TGUI_CFG_SV_PORT_SET_INIT_BG_COLOR
#   define VSF_TGUI_CFG_SV_PORT_SET_INIT_BG_COLOR   ENABLED
#endif

#ifndef VSF_TGUI_CFG_SV_PORT_INIT_BG_COLOR
#   define VSF_TGUI_CFG_SV_PORT_INIT_BG_COLOR       VSF_TGUI_COLOR_WHITE
#endif

#ifndef VSF_TGUI_CFG_SV_PORT_LOG
#   define VSF_TGUI_CFG_SV_PORT_LOG                 DISABLED
#endif

#ifndef VSF_TGUI_CFG_SV_REFRESH_RATE
#   define VSF_TGUI_CFG_SV_REFRESH_RATE             ENABLED
#endif

#ifndef VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY
#   define VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY         DISABLED
#endif

#if VSF_TGUI_CFG_SV_PORT_LOG != ENABLED
#   undef VSF_TGUI_LOG
#   define VSF_TGUI_LOG
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct vsf_tgui_port_info_t {
    vsf_tgui_color_t* pixmap;
    uint32_t pixmap_width;
    uint32_t pixmap_location_x;
} vsf_tgui_port_info_t;

typedef struct {
    vk_disp_t* disp;
    size_t bitmap_size;

    bool is_ready;
#if VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY == ENABLED
    volatile bool is_draw_ready;
#endif
    vsf_tgui_region_t request_region;
    vsf_tgui_region_t current_region;
#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    vsf_systimer_cnt_t start_cnt;
    uint16_t refresh_cnt;
    uint16_t fps;
#endif
} vsf_tgui_port_t;

/*============================ GLOBAL VARIABLES ==============================*/
extern vsf_tgui_font_t g_tUserFonts[];
/*============================ LOCAL VARIABLES ===============================*/
static vsf_tgui_port_t __vsf_tgui_port;
/*============================ PROTOTYPES ====================================*/
static bool __vsf_tgui_port_is_ready_to_refresh(void);
extern void vsf_tgui_low_level_on_ready_to_refresh(void);
/*============================ IMPLEMENTATION ================================*/
static vsf_tgui_color_t vk_disp_sdl_get_pixel(vsf_tgui_location_t* location_ptr)
{
    vk_disp_t* disp = __vsf_tgui_port.disp;
    vsf_tgui_color_t* pixmap = disp->ui_data;
    return pixmap[location_ptr->iY * VSF_TGUI_HOR_MAX + location_ptr->iX];
}

static void vk_disp_sdl_set_pixel(vsf_tgui_location_t* location_ptr, vsf_tgui_color_t color)
{
    vk_disp_t* disp = __vsf_tgui_port.disp;
    vsf_tgui_color_t* pixmap = disp->ui_data;
    pixmap[location_ptr->iY * VSF_TGUI_HOR_MAX + location_ptr->iX] = color;
}

#ifdef WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE
vsf_tgui_size_t vsf_tgui_idx_root_tile_get_size(const vsf_tgui_tile_t* tile_ptr)
{
    VSF_TGUI_ASSERT(tile_ptr != NULL);
    VSF_TGUI_ASSERT(tile_ptr->_.tCore.Attribute.bIsRootTile);
    VSF_TGUI_ASSERT(tile_ptr->_.tCore.Attribute.u2RootTileType == 0);

    return tile_ptr->tBufRoot.tSize;
}
#endif

static char* vsf_tgui_sdl_tile_get_pixelmap(const vsf_tgui_tile_t* tile_ptr)
{
    VSF_TGUI_ASSERT(tile_ptr != NULL);

    if (tile_ptr->_.tCore.Attribute.u2RootTileType == 0) {     // buf tile
        return (char*)&__tiles_data[(uint32_t)tile_ptr->tBufRoot.ptBitmap];
    } else {                                                // index tile
        VSF_TGUI_ASSERT(0);
        return NULL;
    }
}

static void vsf_tgui_sdl_tile_get_pixel(const char* ptPixelMap, vsf_tgui_sv_color_t* color_ptr, uint_fast8_t type)
{
    vsf_tgui_sv_argb8888_color_t argb_color;

    ASSERT(color_ptr != NULL);

    argb_color.tChannel.chR = *ptPixelMap++;
    argb_color.tChannel.chG = *ptPixelMap++;
    argb_color.tChannel.chB = *ptPixelMap++;

    if (type == VSF_TGUI_COLORTYPE_RGBA) {
        argb_color.tChannel.chA = *ptPixelMap++;
    } else if (type == VSF_TGUI_COLORTYPE_RGB) {
        argb_color.tChannel.chA = 0xFF;
    } else {
        VSF_TGUI_ASSERT(0);
    }

    *color_ptr = vsf_tgui_sv_argb8888_to_color(argb_color);
}

/*********************************************************************************/
static uint8_t freetype_get_char_height(FT_Face face)
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

     //return face->size->metrics.height >> 6;

     // https://stackoverflow.com/questions/26486642/whats-the-proper-way-of-getting-text-bounding-box-in-freetype-2
     // https://github.com/cocos2d/cocos2d-x/blob/b26e1bb08648c11a9482d0736a236aebc345008a/cocos/2d/CCFontFreeType.cpp#L180
     //return (face->size->metrics.ascender - face->size->metrics.descender) >> 6;

     //double pixel_size = FREETYPE_FONT_SIZE * FREETYPE_DEFAULT_DPI / 72;
     //return round((face->bbox.yMax - face->bbox.yMin) * pixel_size / face->units_per_EM);

    //return (face->size->metrics.height >> 6) * 12 / 10;
    return (face->size->metrics.ascender >> 6) * 12 / 10;
}

uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32)
{
    const vsf_tgui_font_t* font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    FT_Face face = (FT_Face)font_ptr->ptData;
    VSF_TGUI_ASSERT(face != NULL);
    if (FT_Err_Ok == FT_Load_Char(face, char_u32, FREETYPE_LOAD_FLAGS)) {
        return face->glyph->advance.x >> 6;
    }

    return 0;
}

uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index)
{
    const vsf_tgui_font_t* font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    return font_ptr->chHeight;
}

/*********************************************************************************/

static void __vsf_tgui_get_info(vsf_tgui_location_t* location_ptr, vsf_tgui_port_info_t* info_ptr)
{
    vk_disp_t* disp = __vsf_tgui_port.disp;
    vsf_tgui_region_t* current_region_ptr = &__vsf_tgui_port.current_region;

    info_ptr->pixmap = disp->ui_data;
    info_ptr->pixmap_width = current_region_ptr->tSize.iWidth;
    info_ptr->pixmap_location_x = (location_ptr->iY - current_region_ptr->tLocation.iY) * info_ptr->pixmap_width
        + location_ptr->iX - current_region_ptr->tLocation.iX;
}

#if VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY == ENABLED
static void __vsf_tgui_draw_debug_on_ready(vk_disp_t* disp)
{
    __vsf_tgui_port.is_draw_ready = true;
}

static void __vsf_tgui_draw_wait_for_done(void)
{
    vk_disp_t* disp = __vsf_tgui_port.disp;
    vk_disp_on_ready_t old_ready_func = disp->ui_on_ready;
    disp->ui_on_ready = __vsf_tgui_draw_debug_on_ready;
    __vsf_tgui_port.is_draw_ready = false;
    vsf_tgui_region_t* current_region_ptr = &__vsf_tgui_port.current_region;
    vk_disp_area_t area = {
        .pos = {
            .x = current_region_ptr->tLocation.iX,
            .y = current_region_ptr->tLocation.iY,
        },
        .size = {
            .x = current_region_ptr->tSize.iWidth,
            .y = current_region_ptr->tSize.iHeight,
        },
    };
    vsf_tgui_color_t* pixmap = disp->ui_data;
    vk_disp_refresh(disp, &area, pixmap);
    while (!__vsf_tgui_port.is_draw_ready);
    disp->ui_on_ready = old_ready_func;
}
#else
static void __vsf_tgui_draw_wait_for_done(void)
{
}
#endif


void vsf_tgui_sv_port_draw_rect(vsf_tgui_location_t* location_ptr,
                                vsf_tgui_size_t* size_ptr,
                                vsf_tgui_sv_color_t rect_color)
{
    int16_t height;
    int16_t width;
    vsf_tgui_color_t color;
    uint_fast8_t rect_trans_rate;
    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

    height = size_ptr->iHeight;
    width = size_ptr->iWidth;

    VSF_TGUI_ASSERT((0 <= location_ptr->iX) && (location_ptr->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= location_ptr->iY) && (location_ptr->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (location_ptr->iX + size_ptr->iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT(0 <= (location_ptr->iY + height));                               // y_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iX + width) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT((location_ptr->iY + height) <= VSF_TGUI_VER_MAX);

    color = vsf_tgui_sv_color_get_color(rect_color);
    rect_trans_rate = vsf_tgui_sv_color_get_trans_rate(rect_color);

    VSF_TGUI_LOG(VSF_TRACE_INFO,
                 "[Simple View Port] draw rect(%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
                 color.Value, location_ptr->iX, location_ptr->iY, size_ptr->iWidth, size_ptr->iHeight);

    vsf_tgui_port_info_t info_ptr;
    __vsf_tgui_get_info(location_ptr, &info_ptr);

    for (int16_t i = 0; i < height; i++) {
        uint32_t pixel_location = i * info_ptr.pixmap_width + info_ptr.pixmap_location_x;
        for (int16_t j = 0; j < width; j++) {
            if (rect_trans_rate != 0xFF) {
                info_ptr.pixmap[pixel_location] = vsf_tgui_color_mix(color, info_ptr.pixmap[pixel_location], rect_trans_rate);
            } else {
                info_ptr.pixmap[pixel_location] = color;
            }
            pixel_location++;
        }
    }
    __vsf_tgui_draw_wait_for_done();
}

void vsf_tgui_sv_port_draw_root_tile(vsf_tgui_location_t* location_ptr,
                                     vsf_tgui_location_t* tile_ptrLocation,
                                     vsf_tgui_size_t* size_ptr,
                                     const vsf_tgui_tile_t* tile_ptr,
                                     uint_fast8_t trans_rate)
{
    vsf_tgui_size_t tile_size;
    vsf_tgui_region_t display;
    uint32_t pixel_size;
    const char* pixelmap_u8;

    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(tile_ptrLocation != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);
    VSF_TGUI_ASSERT(tile_ptr != NULL);
    VSF_TGUI_ASSERT((0 <= location_ptr->iX) && (location_ptr->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= location_ptr->iY) && (location_ptr->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (location_ptr->iX + size_ptr->iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iX + size_ptr->iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (location_ptr->iY + size_ptr->iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iY + size_ptr->iHeight) <= VSF_TGUI_VER_MAX);
    VSF_TGUI_ASSERT(vsf_tgui_tile_is_root(tile_ptr));

    tile_size = vsf_tgui_root_tile_get_size(tile_ptr);
    VSF_TGUI_ASSERT(tile_ptrLocation->iX < tile_size.iWidth);
    VSF_TGUI_ASSERT(tile_ptrLocation->iY < tile_size.iHeight);

    display.tLocation = *location_ptr;
    display.tSize.iWidth = min(size_ptr->iWidth, tile_size.iWidth - tile_ptrLocation->iX);
    display.tSize.iHeight = min(size_ptr->iHeight, tile_size.iHeight - tile_ptrLocation->iY);
    if (display.tSize.iHeight <= 0 || display.tSize.iWidth <= 0) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] display size error" VSF_TRACE_CFG_LINEEND);
        VSF_TGUI_ASSERT(0);
        return;
    }

    if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_32IT) {
        pixel_size = 4;
    } else if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_24IT) {
        pixel_size = 3;
    } else {
        VSF_TGUI_ASSERT(0);
    }

    vsf_tgui_port_info_t info_ptr;
    __vsf_tgui_get_info(location_ptr, &info_ptr);
    pixelmap_u8 = vsf_tgui_sdl_tile_get_pixelmap(tile_ptr);

    for (uint16_t i = 0; i < display.tSize.iHeight; i++) {
        uint32_t u32_offset = pixel_size * ((tile_ptrLocation->iY + i) * tile_size.iWidth + tile_ptrLocation->iX);
        const char* data_ptr = pixelmap_u8 + u32_offset;
        uint32_t pixel_location = i * info_ptr.pixmap_width + info_ptr.pixmap_location_x;

        for (uint16_t j = 0; j < display.tSize.iWidth; j++) {
            vsf_tgui_sv_color_t sv_color;
            vsf_tgui_sdl_tile_get_pixel(data_ptr, &sv_color, tile_ptr->_.tCore.Attribute.u2ColorType);
            data_ptr += pixel_size;
            info_ptr.pixmap[pixel_location] = vsf_tgui_color_mix(sv_color.tColor,
                                                                info_ptr.pixmap[pixel_location],
                                                                vsf_tgui_sv_color_get_trans_rate(sv_color) * trans_rate / 255);
            pixel_location++;
        }
    }
    __vsf_tgui_draw_wait_for_done();
}

void vsf_tgui_sv_port_draw_char(vsf_tgui_location_t* location_ptr,
                                vsf_tgui_location_t* font_location_ptr,
                                vsf_tgui_size_t* size_ptr,
                                uint8_t font_index,
                                uint32_t char_u32,
                                vsf_tgui_sv_color_t char_color)
{
    const vsf_tgui_font_t* font_ptr;
    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(font_location_ptr != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

    font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    FT_Face face = (FT_Face)font_ptr->ptData;
    VSF_TGUI_ASSERT(face != NULL);

    if (FT_Err_Ok != FT_Load_Char(face, char_u32, FREETYPE_LOAD_FLAGS)) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] freetype load char faild: %d" VSF_TRACE_CFG_LINEEND, char_u32);
        VSF_TGUI_ASSERT(0);
        return;
    }

    FT_GlyphSlot glyph = face->glyph;
    uint32_t base_line = face->size->metrics.ascender >> 6;
    int32_t top = glyph->bitmap_top;
    int32_t left = glyph->bitmap_left;

    vsf_tgui_location_t bitmap_start = {
        .iX = max(0, left),    // todo: support negative advance
        .iY = base_line - top,
    };
    vsf_tgui_region_t resource_region = {
        .tLocation = *font_location_ptr,
        .tSize = *size_ptr,
    };
    vsf_tgui_region_t bitmap_region = {
        .tLocation = bitmap_start,
        .tSize = {.iWidth = glyph->bitmap.width, .iHeight = glyph->bitmap.rows},
    };
    vsf_tgui_region_t real_bitmap_region;
    if (!vsf_tgui_region_intersect(&real_bitmap_region, &resource_region, &bitmap_region)) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] draw char do nothing and exit" VSF_TRACE_CFG_LINEEND, char_u32);
        return;
    }
    vsf_tgui_location_t bitmap_draw_location = {
        .iX = real_bitmap_region.tLocation.iX - bitmap_region.tLocation.iX,
        .iY = real_bitmap_region.tLocation.iY - bitmap_region.tLocation.iY,
    };
    vsf_tgui_location_t resource_draw_location = {
        .iX = real_bitmap_region.tLocation.iX - resource_region.tLocation.iX,
        .iY = real_bitmap_region.tLocation.iY - resource_region.tLocation.iY,
    };

    vsf_tgui_color_t color = vsf_tgui_sv_color_get_color(char_color);
    uint_fast8_t trans_rate = vsf_tgui_sv_color_get_trans_rate(char_color);

    vsf_tgui_port_info_t info_ptr;
    __vsf_tgui_get_info(location_ptr, &info_ptr);

    for (uint16_t i = 0; i < real_bitmap_region.tSize.iHeight; i++) {
        uint32_t pixel_location = (i + resource_draw_location.iY) * info_ptr.pixmap_width + (info_ptr.pixmap_location_x + resource_draw_location.iX);
        uint32_t bitmap_location = (i + bitmap_draw_location.iY) * glyph->bitmap.width + bitmap_draw_location.iX;

        for (uint16_t j = 0; j < real_bitmap_region.tSize.iWidth; j++) {
            uint8_t mix = glyph->bitmap.buffer[bitmap_location++];
            info_ptr.pixmap[pixel_location] = vsf_tgui_color_mix(color, info_ptr.pixmap[pixel_location], ((uint16_t)mix * trans_rate) / 255);
            pixel_location++;
        }
    }
    __vsf_tgui_draw_wait_for_done();
}

/**********************************************************************************/
/*! \brief begin a refresh loop
 *! \param gui_ptr the tgui object address
 *! \param ptPlannedRefreshRegion the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 *!
 *! \note When NULL is returned, current refresh iteration (i.e. a refresh activites
 *!       between vsf_tgui_v_refresh_loop_begin and vsf_tgui_v_refresh_loop_end )
 *!       will be ignored and vsf_tgui_v_refresh_loop_end is called immediately
 **********************************************************************************/

vsf_tgui_region_t* vsf_tgui_v_refresh_loop_begin( vsf_tgui_t* gui_ptr, const vsf_tgui_region_t* ptPlannedRefreshRegion)
{
    if (!__vsf_tgui_port_is_ready_to_refresh()) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] no ready to refresh" VSF_TRACE_CFG_LINEEND);
        return NULL;
    }

    vsf_tgui_region_t* current_region_ptr = &__vsf_tgui_port.current_region;
    vsf_tgui_region_t* request_region_ptr = &__vsf_tgui_port.request_region;

    if (current_region_ptr->tSize.iHeight <= 0 || current_region_ptr->tSize.iWidth <= 0) {
        const vsf_tgui_region_t s_displayRegion = {0,0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX};
        if (!vsf_tgui_region_intersect(request_region_ptr, ptPlannedRefreshRegion, &s_displayRegion)) {
            VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] refresh region is not in screen" VSF_TRACE_CFG_LINEEND);
            return NULL;
        } else {
            VSF_TGUI_LOG(VSF_TRACE_INFO,
                         "[Simple View Port] request low level refresh start in area, location(%d, %d), size(%d, %d)" VSF_TRACE_CFG_LINEEND,
                         request_region_ptr->tLocation.iX,
                         request_region_ptr->tLocation.iY,
                         request_region_ptr->tSize.iWidth,
                         request_region_ptr->tSize.iHeight);

            int16_t height = __vsf_tgui_port.bitmap_size / request_region_ptr->tSize.iWidth;
            height = min(request_region_ptr->tSize.iHeight, height);

            current_region_ptr->tLocation.iX = request_region_ptr->tLocation.iX;
            current_region_ptr->tLocation.iY = request_region_ptr->tLocation.iY;
            current_region_ptr->tSize.iWidth = request_region_ptr->tSize.iWidth;
            current_region_ptr->tSize.iHeight = height;

            VSF_TGUI_LOG(VSF_TRACE_INFO,
                         "[Simple View Port] current region location(%d, %d), size(%d, %d)" VSF_TRACE_CFG_LINEEND,
                         current_region_ptr->tLocation.iX,
                         current_region_ptr->tLocation.iY,
                         current_region_ptr->tSize.iWidth,
                         current_region_ptr->tSize.iHeight);

            vsf_tgui_color_t* color_ptr = __vsf_tgui_port.disp->ui_data;
            vsf_tgui_sv_color_t screen_sv_color = VSF_TGUI_CFG_SV_PORT_INIT_BG_COLOR;
            for (int i = 0; i < __vsf_tgui_port.bitmap_size; i++) {
                color_ptr[i] = screen_sv_color.tColor;
            }
        }
    }

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    if (0 == __vsf_tgui_port.refresh_cnt) {
        __vsf_tgui_port.start_cnt = vsf_systimer_get_tick();
    }
#endif

    return current_region_ptr;
}

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t* gui_ptr)
{
    __vsf_sched_safe(
        if (__vsf_tgui_port.is_ready) {
            __vsf_tgui_port.is_ready = false;
        }
    )

    vsf_tgui_region_t* current_region_ptr = &__vsf_tgui_port.current_region;
    vsf_tgui_region_t* request_region_ptr = &__vsf_tgui_port.request_region;

    vk_disp_area_t area = {
        .pos = {
            .x = current_region_ptr->tLocation.iX,
            .y = current_region_ptr->tLocation.iY,
        },
        .size = {
            .x = current_region_ptr->tSize.iWidth,
            .y = current_region_ptr->tSize.iHeight,
        },
    };
    vk_disp_t* disp = __vsf_tgui_port.disp;
    vsf_tgui_color_t* pixmap = disp->ui_data;
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port]vk_disp_refresh (%d %d) (%d %d)." VSF_TRACE_CFG_LINEEND,
                 area.pos.x, area.pos.y,
                 area.size.x, area.size.y);
    vk_disp_refresh(disp, &area, pixmap);

    int request_location_y2 = request_region_ptr->tLocation.iY + request_region_ptr->tSize.iHeight;
    int current_location_y2 = current_region_ptr->tLocation.iY + current_region_ptr->tSize.iHeight;
    int remain = request_location_y2 - current_location_y2;

    if (remain > 0) {
        int16_t height = __vsf_tgui_port.bitmap_size / request_region_ptr->tSize.iWidth;
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port]iHeight: %d, current_region_ptr->tLocation.iY:%d, request_location_y2:%d" VSF_TRACE_CFG_LINEEND,
                     height, current_region_ptr->tLocation.iY, request_location_y2);
        height = min(height, remain);
        current_region_ptr->tLocation.iY += current_region_ptr->tSize.iHeight;
        current_region_ptr->tSize.iHeight = height;
        return true;
    } else {
        memset(current_region_ptr, 0, sizeof(*current_region_ptr));
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port]End Refresh Loop." VSF_TRACE_CFG_LINEEND);
        return false;
    }
}

#ifndef WEAK_VSF_TGUI_LOW_LEVEL_ON_READY_TO_REFRESH
WEAK(vsf_tgui_low_level_on_ready_to_refresh)
void vsf_tgui_low_level_on_ready_to_refresh(void)
{
}
#endif


static void __vsf_tgui_on_ready(vk_disp_t* disp)
{
    __vsf_tgui_port.is_ready = true;

    vsf_tgui_low_level_on_ready_to_refresh();

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    {
        uint32_t elapse;

        __vsf_tgui_port.refresh_cnt++;
        elapse = vsf_systimer_tick_to_ms(vsf_systimer_get_tick() - __vsf_tgui_port.start_cnt);

        if (elapse >= 1000) {
            __vsf_tgui_port.fps = __vsf_tgui_port.refresh_cnt;
            __vsf_tgui_port.refresh_cnt = 0;
        }
    }
#endif
}

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
uint32_t vsf_tgui_port_get_refresh_rate(void)
{
    return __vsf_tgui_port.fps;
}
#endif

static bool __vsf_tgui_port_is_ready_to_refresh(void)
{
    return __vsf_tgui_port.is_ready;
}

static bool vsf_tgui_sv_fonts_init(vsf_tgui_font_t* font_ptr, size_t size)
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

        if (FT_Err_Ok != FT_New_Face(library, font_ptr->pchFontPath, 0, &face)) {
            VSF_TGUI_ASSERT(0);
            return false;
        }

        if (font_ptr->chFontSize != 0) {
            if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_ptr->chFontSize * 64, 0, 0)) {
                VSF_TGUI_ASSERT(0);
                return false;
            }
            font_ptr->chHeight = freetype_get_char_height(face);
        } else if (font_ptr->chHeight != 0) {
            font_size = font_ptr->chHeight;
            do {
                if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_size * 64, 0, 0)) {
                    VSF_TGUI_ASSERT(0);
                    return false;
                }

                font_height = freetype_get_char_height(face);
                if (font_ptr->chHeight < font_height) {
                    font_size--;
                } else {
                    font_ptr->chHeight = font_height;
                    break;
                }
            } while (1);
        } else {
            VSF_TGUI_ASSERT(0);
            return false;
        }
        font_ptr->ptData = face;

        font_ptr++;
    }

    return true;
}

void vsf_tgui_bind_disp(vk_disp_t* disp, void* bitmap_data, size_t bitmap_size)
{
    VSF_TGUI_ASSERT(disp != NULL);
    VSF_TGUI_ASSERT(bitmap_data != NULL);
    VSF_TGUI_ASSERT(bitmap_size >= VSF_TGUI_HOR_MAX);


    vsf_tgui_sv_fonts_init((vsf_tgui_font_t*)vsf_tgui_font_get(0),
                           vsf_tgui_font_number());

    __vsf_tgui_port.disp = disp;
    __vsf_tgui_port.bitmap_size = bitmap_size;

    disp->ui_data = bitmap_data;
    disp->ui_on_ready = __vsf_tgui_on_ready;
    vk_disp_init(disp);

    __vsf_tgui_port.is_ready = true;
}
#endif

/* EOF */
