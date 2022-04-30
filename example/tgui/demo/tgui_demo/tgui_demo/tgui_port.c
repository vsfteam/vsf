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

#if APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB565
typedef uint16_t vk_disp_sdl_pixel_t;
#elif (APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_ARGB8888) || (APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB666_32)
typedef uint32_t vk_disp_sdl_pixel_t;
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef struct vsf_tgui_port_info_t {
    vk_disp_sdl_pixel_t* pixmap;
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
    vsf_systimer_tick_t start_cnt;
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
static vsf_tgui_sv_color_t __sdl_color_to_color(uint32_t pixel)
{
#if APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB565
    vsf_tgui_sv_color_rgb565_t rgb565_color;
    rgb565_color.value = pixel;
#if VSF_TGUI_CFG_COLOR_MODE == VSF_TGUI_COLOR_ARGB_8888
    rgb565_color.alpha = 0xFF;
#endif
    return vsf_tgui_sv_rgb565_to_color(rgb565_color);

#elif APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_ARGB8888
    vsf_tgui_sv_color_argb8888_t argb8888_color;
    argb8888_color.value = pixel;
    return vsf_tgui_sv_argb8888_to_color(rgb565_color);

#elif APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB666_32
#   error "Unsupported VSF_TGUI_CFG_COLOR_MODE and APP_DISP_SDL2_COLOR combination"
#endif

    return VSF_TGUI_COLOR_BLUE;
}

static uint32_t __color_to_sdl_color(vsf_tgui_sv_color_t color)
{
#if APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB565
    vsf_tgui_sv_color_rgb565_t rgb565_color = vsf_tgui_sv_color_to_rgb565(color);
    return rgb565_color.value;
#elif APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_ARGB8888
    vsf_tgui_sv_color_rgb565_t argb8888_color = vsf_tgui_sv_color_to_argb8888(color);
    return argb8888_color.value;
#elif APP_DISP_SDL2_COLOR == VSF_DISP_COLOR_RGB666_32
#   error "Unsupported VSF_TGUI_CFG_COLOR_MODE and APP_DISP_SDL2_COLOR combination"
#endif
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

static unsigned char* vsf_tgui_sdl_tile_get_pixelmap(const vsf_tgui_tile_t* tile_ptr)
{
    VSF_TGUI_ASSERT(tile_ptr != NULL);

    if (tile_ptr->_.tCore.Attribute.u2RootTileType == 0) {     // buf tile
        return (unsigned char*)&__tiles_data[(uint32_t)tile_ptr->tBufRoot.ptBitmap];
    } else {                                                // index tile
        VSF_TGUI_ASSERT(0);
        return NULL;
    }
}

static void vsf_tgui_sdl_tile_get_pixel(const char* pixelmap_ptr, vsf_tgui_sv_color_t* color_ptr, uint_fast8_t type)
{
    vsf_tgui_sv_color_argb8888_t argb_color;

    VSF_TGUI_ASSERT(color_ptr != NULL);

    argb_color.red   = *pixelmap_ptr++;
    argb_color.green = *pixelmap_ptr++;
    argb_color.blue  = *pixelmap_ptr++;

    if (type == VSF_TGUI_COLORTYPE_RGBA) {
        argb_color.alpha = *pixelmap_ptr++;
    } else if (type == VSF_TGUI_COLORTYPE_RGB) {
        argb_color.alpha = 0xFF;
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
     return face->size->metrics.height >> 6;
}

uint8_t vsf_tgui_font_get_char_width(const uint8_t font_index, uint32_t char_u32)
{
    const vsf_tgui_font_t* font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);

    FT_Face face = (FT_Face)font_ptr->data_ptr;
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

    return font_ptr->height;
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
    vk_disp_refresh(disp, &area, disp->ui_data);
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

    rect_trans_rate = vsf_tgui_sv_color_get_trans_rate(rect_color);

    VSF_TGUI_LOG(VSF_TRACE_INFO,
                 "[Simple View Port] draw rect(%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
                 rect_color.value, location_ptr->iX, location_ptr->iY, size_ptr->iWidth, size_ptr->iHeight);

    vsf_tgui_port_info_t info_ptr;
    __vsf_tgui_get_info(location_ptr, &info_ptr);

    for (int16_t i = 0; i < height; i++) {
        uint32_t pixel_location = i * info_ptr.pixmap_width + info_ptr.pixmap_location_x;
        for (int16_t j = 0; j < width; j++) {
            if (rect_trans_rate != 0xFF) {
                info_ptr.pixmap[pixel_location] = __color_to_sdl_color(vsf_tgui_sv_color_mix(rect_color, __sdl_color_to_color(info_ptr.pixmap[pixel_location]), rect_trans_rate));
            } else {
                info_ptr.pixmap[pixel_location] = __color_to_sdl_color(rect_color);
            }
            pixel_location++;
        }
    }
    __vsf_tgui_draw_wait_for_done();
}

void vsf_tgui_sv_port_draw_root_tile(vsf_tgui_location_t* location_ptr,
                                     vsf_tgui_location_t* tile_location_ptr,
                                     vsf_tgui_size_t* size_ptr,
                                     const vsf_tgui_tile_t* tile_ptr,
                                     uint_fast8_t trans_rate)
{
    vsf_tgui_size_t tile_size;
    vsf_tgui_region_t display;
    uint32_t pixel_size;
    const unsigned char* pixelmap_u8;

    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(tile_location_ptr != NULL);
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
    VSF_TGUI_ASSERT(tile_location_ptr->iX < tile_size.iWidth);
    VSF_TGUI_ASSERT(tile_location_ptr->iY < tile_size.iHeight);

    display.tLocation = *location_ptr;
    display.tSize.iWidth = vsf_min(size_ptr->iWidth, tile_size.iWidth - tile_location_ptr->iX);
    display.tSize.iHeight = vsf_min(size_ptr->iHeight, tile_size.iHeight - tile_location_ptr->iY);
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
        uint32_t u32_offset = pixel_size * ((tile_location_ptr->iY + i) * tile_size.iWidth + tile_location_ptr->iX);
        const char* data_ptr = (const char*)(pixelmap_u8 + u32_offset);
        uint32_t pixel_location = i * info_ptr.pixmap_width + info_ptr.pixmap_location_x;

        for (uint16_t j = 0; j < display.tSize.iWidth; j++) {
            vsf_tgui_sv_color_t sv_color;
            vsf_tgui_sdl_tile_get_pixel(data_ptr, &sv_color, tile_ptr->_.tCore.Attribute.u2ColorType);
            data_ptr += pixel_size;
            info_ptr.pixmap[pixel_location] = __color_to_sdl_color(vsf_tgui_sv_color_mix(sv_color,
			                                                              __sdl_color_to_color(info_ptr.pixmap[pixel_location]),
																		  vsf_tgui_sv_color_get_trans_rate(sv_color) * trans_rate / 255));
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

    FT_Face face = (FT_Face)font_ptr->data_ptr;
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
        .iX = vsf_max(0, left),    // todo: support negative advance
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

    uint_fast8_t trans_rate = vsf_tgui_sv_color_get_trans_rate(char_color);

    vsf_tgui_port_info_t info_ptr;
    __vsf_tgui_get_info(location_ptr, &info_ptr);

    for (uint16_t i = 0; i < real_bitmap_region.tSize.iHeight; i++) {
        uint32_t pixel_location = (i + resource_draw_location.iY) * info_ptr.pixmap_width + (info_ptr.pixmap_location_x + resource_draw_location.iX);
        uint32_t bitmap_location = (i + bitmap_draw_location.iY) * glyph->bitmap.width + bitmap_draw_location.iX;

        for (uint16_t j = 0; j < real_bitmap_region.tSize.iWidth; j++) {
            uint8_t mix = glyph->bitmap.buffer[bitmap_location++];
            info_ptr.pixmap[pixel_location] = __color_to_sdl_color(vsf_tgui_sv_color_mix(char_color, __sdl_color_to_color(info_ptr.pixmap[pixel_location]), ((uint16_t)mix * trans_rate) / 255));
            pixel_location++;
        }
    }
    __vsf_tgui_draw_wait_for_done();
}

/**********************************************************************************/
/*! \brief begin a refresh loop
 *! \param gui_ptr the tgui object address
 *! \param planned_refresh_region_ptr the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 *!
 *! \note When NULL is returned, current refresh iteration (i.e. a refresh activites
 *!       between vsf_tgui_v_refresh_loop_begin and vsf_tgui_v_refresh_loop_end )
 *!       will be ignored and vsf_tgui_v_refresh_loop_end is called immediately
 **********************************************************************************/

vsf_tgui_region_t* vsf_tgui_v_refresh_loop_begin( vsf_tgui_t* gui_ptr, const vsf_tgui_region_t* planned_refresh_region_ptr)
{
    if (!__vsf_tgui_port_is_ready_to_refresh()) {
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] no ready to refresh" VSF_TRACE_CFG_LINEEND);
        return NULL;
    }

    vsf_tgui_region_t* current_region_ptr = &__vsf_tgui_port.current_region;
    vsf_tgui_region_t* request_region_ptr = &__vsf_tgui_port.request_region;

    if (current_region_ptr->tSize.iHeight <= 0 || current_region_ptr->tSize.iWidth <= 0) {
        const vsf_tgui_region_t s_displayRegion = {0,0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX};
        if (!vsf_tgui_region_intersect(request_region_ptr, planned_refresh_region_ptr, &s_displayRegion)) {
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
            height = vsf_min(request_region_ptr->tSize.iHeight, height);

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

            vk_disp_sdl_pixel_t* color_ptr = __vsf_tgui_port.disp->ui_data;
            uint32_t sdl_color = __color_to_sdl_color(VSF_TGUI_CFG_SV_PORT_INIT_BG_COLOR);
            for (int i = 0; i < __vsf_tgui_port.bitmap_size; i++) {
                color_ptr[i] = sdl_color;
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
    vk_disp_sdl_pixel_t* pixmap = disp->ui_data;
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
        height = vsf_min(height, remain);
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

static void __vsf_tgui_on_refreshed(vk_disp_t* disp)
{
	__vsf_tgui_on_ready(disp);
}

static void __vsf_tgui_on_inited(vk_disp_t* disp)
{
	disp->ui_on_ready = __vsf_tgui_on_refreshed;
	__vsf_tgui_on_ready(disp);
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

        char path[256];
        strcpy(path, "font/");
        strcat(path, font_ptr->font_path_ptr);
        if (FT_Err_Ok != FT_New_Face(library, path, 0, &face)) {
            VSF_TGUI_ASSERT(0);
            return false;
        }

        if (font_ptr->font_size != 0) {
            if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_ptr->font_size * 64, 0, 0)) {
                VSF_TGUI_ASSERT(0);
                return false;
            }
            font_ptr->height = freetype_get_char_height(face);
        } else if (font_ptr->height != 0) {
            font_size = font_ptr->height;
            do {
                if (FT_Err_Ok != FT_Set_Char_Size(face, 0, font_size * 64, 0, 0)) {
                    VSF_TGUI_ASSERT(0);
                    return false;
                }

                font_height = freetype_get_char_height(face);
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
        font_ptr->data_ptr = face;

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
    __vsf_tgui_port.is_ready = false;

    disp->ui_data = bitmap_data;
    disp->ui_on_ready = __vsf_tgui_on_inited;
    vk_disp_init(disp);
}
#endif

/* EOF */
