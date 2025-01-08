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
#define __VSF_TGUI_CLASS_INHERIT
#include "../../vsf_tgui.h"

#if     VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED                    \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include <math.h>
#include <string.h>

/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_CFG_SV_PORT_SET_INIT_BG_COLOR
#   define VSF_TGUI_CFG_SV_PORT_SET_INIT_BG_COLOR   ENABLED
#endif

#ifndef VSF_TGUI_CFG_SV_PORT_LOG
#   define VSF_TGUI_CFG_SV_PORT_LOG                 DISABLED
#endif

#if VSF_TGUI_CFG_SV_PORT_LOG != ENABLED
#   undef VSF_TGUI_LOG
#   define VSF_TGUI_LOG(...)
#endif

#if VSF_TGUI_CFG_DISP_COLOR == VSF_TGUI_COLOR_RGB_565
typedef uint16_t vsf_tgui_disp_pixel_t;
#   define vsf_tgui_disp_color_prefix               rgb565
typedef vsf_tgui_sv_color_rgb565_t vsf_tgui_disp_tgui_pixel_t;
#elif VSF_TGUI_CFG_DISP_COLOR == VSF_TGUI_COLOR_BGR_565
typedef uint16_t vsf_tgui_disp_pixel_t;
#   define vsf_tgui_disp_color_prefix               bgr565
#elif VSF_TGUI_CFG_DISP_COLOR == VSF_TGUI_COLOR_ARGB_8888
typedef uint32_t vsf_tgui_disp_pixel_t;
#   define vsf_tgui_disp_color_prefix               argb8888
#else
#   error "Unsupported VSF_TGUI_CFG_DISP_COLOR"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_TGUI_CFG_DISP_COLOR == VSF_TGUI_CFG_COLOR_MODE
#   define __disp_color_to_tgui_color(__disp_color) ((vsf_tgui_sv_color_t){ .value = __disp_color})
#   define __tgui_color_to_disp_color(__tgui_color) (__tgui_color.value)
#else
#   define __disp_color_to_tgui_color(__disp_color) ({                          \
        VSF_MCONNECT(vsf_tgui_sv_color_, vsf_tgui_disp_color_prefix, _t)        \
            VSF_MACRO_SAFE_NAME(color) = {                                      \
            .value = (__disp_color),                                            \
        };                                                                      \
        VSF_MCONNECT(vsf_tgui_sv_, vsf_tgui_disp_color_prefix, _to_color)(      \
            VSF_MACRO_SAFE_NAME(color));                                        \
    })
#   define __tgui_color_to_disp_color(__tgui_color) ({                          \
        VSF_MCONNECT(vsf_tgui_sv_color_, vsf_tgui_disp_color_prefix, _t)        \
            VSF_MACRO_SAFE_NAME(color) = VSF_MCONNECT(vsf_tgui_sv_color_to_, vsf_tgui_disp_color_prefix)(__tgui_color);\
            VSF_MACRO_SAFE_NAME(color).value;                                   \
    })
#endif

/*============================ TYPES =========================================*/

typedef struct vsf_tgui_sv_port_info_t {
    vsf_tgui_disp_pixel_t *pixmap;
    uint32_t pixmap_width;
    uint32_t pixmap_location_x;
} vsf_tgui_sv_port_info_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(vsf_tgui_tile_get_pixelmap)
unsigned char* vsf_tgui_tile_get_pixelmap(const vsf_tgui_tile_t *tile_ptr)
{
    VSF_TGUI_ASSERT(0);
    return NULL;
}

#if VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY == ENABLED
static void __vsf_tgui_draw_debug_on_ready(vk_disp_t *disp)
{
    vsf_tgui_t *gui_ptr = (vsf_tgui_t *)disp->ui_data;
    gui_ptr->is_draw_ready = true;
}

static void __vsf_tgui_draw_wait_for_done(vsf_tgui_t *gui_ptr)
{
    vk_disp_t *disp = gui_ptr->disp;
    vk_disp_on_ready_t old_ready_func = disp->ui_on_ready;
    disp->ui_on_ready = __vsf_tgui_draw_debug_on_ready;
    gui_ptr->is_draw_ready = false;
    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
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
    vk_disp_refresh(disp, &area, gui_ptr->pfb);
    while (!gui_ptr->is_draw_ready);
    disp->ui_on_ready = old_ready_func;
}
#else
static void __vsf_tgui_draw_wait_for_done(vsf_tgui_t *gui_ptr)
{
}
#endif


void vsf_tgui_sv_port_draw_rect(vsf_tgui_t *gui_ptr,
                                vsf_tgui_location_t *location_ptr,
                                vsf_tgui_size_t *size_ptr,
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
    VSF_TGUI_ASSERT(0 <= (location_ptr->iY + height));                                  // y_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iX + width) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT((location_ptr->iY + height) <= VSF_TGUI_VER_MAX);

    rect_trans_rate = vsf_tgui_sv_color_get_trans_rate(rect_color);

    VSF_TGUI_LOG(VSF_TRACE_INFO,
                 "[Simple View Port] draw rect(%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
                 rect_color.value, location_ptr->iX, location_ptr->iY, size_ptr->iWidth, size_ptr->iHeight);

    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
    vsf_tgui_disp_pixel_t *pixmap = (vsf_tgui_disp_pixel_t *)gui_ptr->pfb;
    uint32_t pixmap_width = current_region_ptr->tSize.iWidth;
    uint32_t pixmap_location_x = (location_ptr->iY - current_region_ptr->tLocation.iY) * pixmap_width
        + location_ptr->iX - current_region_ptr->tLocation.iX;

    if (rect_trans_rate == 0) {
    } else if (rect_trans_rate != 0xFF) {
        uint32_t pixel_location;
        for (int16_t i = 0; i < height; i++) {
            pixel_location = i * pixmap_width + pixmap_location_x;
            for (int16_t j = 0; j < width; j++) {
                pixmap[pixel_location++] = __tgui_color_to_disp_color(vsf_tgui_sv_color_mix(rect_color,
                    __disp_color_to_tgui_color(pixmap[pixel_location]), rect_trans_rate));
            }
        }
    } else {
        vsf_tgui_disp_pixel_t disp_color = __tgui_color_to_disp_color(rect_color);
        vsf_tgui_disp_pixel_t *ptr = &pixmap[pixmap_location_x];
        uint32_t step = pixmap_width - width;
        for (int16_t i = 0; i < height; i++, ptr += step) {
            uint32_t round = (width + 7) >> 3;
            switch (width & 7) {
            case 0: do {    *ptr++ = disp_color;
            case 7:         *ptr++ = disp_color;
            case 6:         *ptr++ = disp_color;
            case 5:         *ptr++ = disp_color;
            case 4:         *ptr++ = disp_color;
            case 3:         *ptr++ = disp_color;
            case 2:         *ptr++ = disp_color;
            case 1:         *ptr++ = disp_color;
                    } while (--round);
            }
        }
    }
    __vsf_tgui_draw_wait_for_done(gui_ptr);
}

void vsf_tgui_sv_port_draw_root_tile(vsf_tgui_t *gui_ptr,
                                     vsf_tgui_location_t *location_ptr,
                                     vsf_tgui_location_t *tile_location_ptr,
                                     vsf_tgui_size_t *size_ptr,
                                     const vsf_tgui_tile_t *tile_ptr,
                                     uint_fast8_t trans_rate,
                                     vsf_tgui_sv_color_t color,
                                     vsf_tgui_sv_color_t bg_color)
{
    vsf_tgui_size_t tile_size;
    vsf_tgui_region_t display;
    uint32_t pixel_size;
    const unsigned char *pixelmap_u8;

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

    if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_32BIT) {
        pixel_size = 4;
    } else if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_24BIT) {
        pixel_size = 3;
    } else if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_16BIT) {
        pixel_size = 2;
    } else if (tile_ptr->_.tCore.Attribute.u3ColorSize == VSF_TGUI_COLORSIZE_8BIT) {
        pixel_size = 1;
    } else {
        VSF_TGUI_ASSERT(0);
    }

    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
    uint32_t pixmap_width = current_region_ptr->tSize.iWidth;
    uint32_t pixmap_location_x = (location_ptr->iY - current_region_ptr->tLocation.iY) * pixmap_width
        + location_ptr->iX - current_region_ptr->tLocation.iX;
    pixelmap_u8 = vsf_tgui_tile_get_pixelmap(tile_ptr);

    uint_fast8_t type = tile_ptr->_.tCore.Attribute.u2ColorType;
    bool is_mask = type == VSF_TGUI_TILE_COLORTYPE_A;
    vsf_tgui_sv_color_t sv_color;
    vsf_tgui_sv_color_argb8888_t argb_color;
    uint_fast8_t pixel_trans_rate;
    vsf_tgui_disp_pixel_t *pixel_ptr = &((vsf_tgui_disp_pixel_t *)gui_ptr->pfb)[pixmap_location_x];
    uint32_t step = pixmap_width - display.tSize.iWidth;

    for (uint16_t i = 0; i < display.tSize.iHeight; i++, pixel_ptr += step) {
        uint32_t u32_offset = pixel_size * ((tile_location_ptr->iY + i) * tile_size.iWidth + tile_location_ptr->iX);
        const char *data_ptr = (const char*)(pixelmap_u8 + u32_offset);

        for (uint16_t j = 0; j < display.tSize.iWidth; j++) {
            if (is_mask) {
                argb_color          = vsf_tgui_sv_color_to_argb8888(color);
                argb_color.alpha    = *data_ptr++;
            } else {
                argb_color.red      = *data_ptr++;
                argb_color.green    = *data_ptr++;
                argb_color.blue     = *data_ptr++;

                if (type == VSF_TGUI_TILE_COLORTYPE_RGBA) {
                    argb_color.alpha = *data_ptr++;
                } else {
                    argb_color.alpha = 0xFF;
                }
            }
            sv_color = vsf_tgui_sv_argb8888_to_color(argb_color);

            pixel_trans_rate = (vsf_tgui_sv_color_get_trans_rate(sv_color) * trans_rate) >> 8;
            if (is_mask) {
                *pixel_ptr = __tgui_color_to_disp_color(vsf_tgui_sv_color_mix(sv_color,
                    bg_color, pixel_trans_rate));
            } else {
                *pixel_ptr = __tgui_color_to_disp_color(vsf_tgui_sv_color_mix(sv_color,
                    __disp_color_to_tgui_color(*pixel_ptr), pixel_trans_rate));
            }
            pixel_ptr++;
        }
    }
    __vsf_tgui_draw_wait_for_done(gui_ptr);
}

void vsf_tgui_sv_port_draw_char(vsf_tgui_t *gui_ptr,
                                vsf_tgui_location_t *location_ptr,
                                vsf_tgui_location_t *font_location_ptr,
                                vsf_tgui_size_t *size_ptr,
                                uint8_t font_index,
                                uint32_t char_u32,
                                vsf_tgui_sv_color_t char_color)
{
    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(font_location_ptr != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

    vsf_tgui_region_t bitmap_region, real_bitmap_region, resource_region = {
        .tLocation = *font_location_ptr,
        .tSize = *size_ptr,
    };
    uint8_t *bitmap = (uint8_t *)vsf_tgui_font_get_char(font_index, char_u32, &bitmap_region);
    if (NULL == bitmap) {
        return;
    }

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

    if (trans_rate != 0) {
        vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
        vsf_tgui_disp_pixel_t *pixmap = (vsf_tgui_disp_pixel_t *)gui_ptr->pfb;
        uint32_t pixmap_width = current_region_ptr->tSize.iWidth;
        uint32_t pixmap_location_x = (location_ptr->iY - current_region_ptr->tLocation.iY) * pixmap_width
            + location_ptr->iX - current_region_ptr->tLocation.iX;

        for (uint16_t i = 0; i < real_bitmap_region.tSize.iHeight; i++) {
            uint32_t pixel_location = (i + resource_draw_location.iY) * pixmap_width + (pixmap_location_x + resource_draw_location.iX);
            uint32_t bitmap_location = (i + bitmap_draw_location.iY) * bitmap_region.tSize.iWidth + bitmap_draw_location.iX;

            for (uint16_t j = 0; j < real_bitmap_region.tSize.iWidth; j++) {
                uint8_t mix = bitmap[bitmap_location++];
                pixmap[pixel_location] = __tgui_color_to_disp_color(vsf_tgui_sv_color_mix(char_color,
                    __disp_color_to_tgui_color(pixmap[pixel_location]), ((uint16_t)mix * trans_rate) >> 8));
                pixel_location++;
            }
        }
    }
    vsf_tgui_font_release_char(font_index, char_u32, bitmap);

    __vsf_tgui_draw_wait_for_done(gui_ptr);
}

/**********************************************************************************/
/*! \brief begin a refresh loop
 *! \param gui_ptr the tgui object address
 *! \param planned_refresh_region_ptr the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 *!
 *! \note When NULL is returned, current refresh iteration (i.e. a refresh activities
 *!       between vsf_tgui_v_refresh_loop_begin and vsf_tgui_v_refresh_loop_end )
 *!       will be ignored and vsf_tgui_v_refresh_loop_end is called immediately
 **********************************************************************************/

vsf_tgui_region_t* vsf_tgui_v_refresh_loop_begin(vsf_tgui_t *gui_ptr, const vsf_tgui_region_t *planned_refresh_region_ptr)
{
    vsf_protect_t orig = vsf_protect_int();
    if (    !gui_ptr->is_disp_inited ||
#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
            gui_ptr->refresh_pending) {
#else
            (gui_ptr->refresh_pending_cnt > 0)) {
#endif
        gui_ptr->refresh_pending_notify = true;
        vsf_unprotect_int(orig);
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port] no ready to refresh" VSF_TRACE_CFG_LINEEND);
        return NULL;
    }
    vsf_unprotect_int(orig);

    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
    vsf_tgui_region_t *request_region_ptr = &gui_ptr->request_region;

    if (current_region_ptr->tSize.iHeight <= 0 || current_region_ptr->tSize.iWidth <= 0) {
        static const vsf_tgui_region_t s_displayRegion = {0,0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX};
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

            int16_t height = gui_ptr->pfb_size / request_region_ptr->tSize.iWidth;
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
        }
    }

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    if (0 == gui_ptr->refresh_cnt) {
        gui_ptr->start_cnt = vsf_systimer_get_tick();
    }
#endif

    return current_region_ptr;
}

static void __vsf_tgui_v_disp_refresh(vsf_tgui_t *gui_ptr, vk_disp_area_t *area)
{
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View Port]vk_disp_refresh (%d %d) (%d %d)." VSF_TRACE_CFG_LINEEND,
                 area->pos.x, area->pos.y,
                 area->size.x, area->size.y);

    vk_disp_refresh(gui_ptr->disp, area, gui_ptr->pfb);

#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
    if (gui_ptr->refresh_is_first_pfb) {
        gui_ptr->pfb = (void *)((uint8_t *)gui_ptr->pfb + gui_ptr->pfb_size * sizeof(vsf_tgui_disp_pixel_t));
    } else {
        gui_ptr->pfb = (void *)((uint8_t *)gui_ptr->pfb - gui_ptr->pfb_size * sizeof(vsf_tgui_disp_pixel_t));
    }
    gui_ptr->refresh_is_first_pfb = !gui_ptr->refresh_is_first_pfb;
#endif
    vsf_protect_t orig = vsf_protect_int();
        gui_ptr->refresh_pending_cnt++;
    vsf_unprotect_int(orig);
}

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t *gui_ptr)
{
    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
    vsf_tgui_region_t *request_region_ptr = &gui_ptr->request_region;

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

#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
    vsf_protect_t orig = vsf_protect_int();
    if (gui_ptr->refresh_pending_cnt > 0) {
        gui_ptr->refresh_pending = true;
        gui_ptr->refresh_pending_area = area;
        vsf_unprotect_int(orig);
    } else {
        vsf_unprotect_int(orig);
        __vsf_tgui_v_disp_refresh(gui_ptr, &area);
    }
#else
    __vsf_tgui_v_disp_refresh(gui_ptr, &area);
#endif

    int request_location_y2 = request_region_ptr->tLocation.iY + request_region_ptr->tSize.iHeight;
    int current_location_y2 = current_region_ptr->tLocation.iY + current_region_ptr->tSize.iHeight;
    int remain = request_location_y2 - current_location_y2;

    if (remain > 0) {
        int16_t height = gui_ptr->pfb_size / request_region_ptr->tSize.iWidth;
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


static void __vsf_tgui_on_ready(vk_disp_t *disp)
{
    vsf_tgui_t *gui_ptr = (vsf_tgui_t *)disp->ui_data;

    vsf_protect_t orig = vsf_protect_int();
    gui_ptr->refresh_pending_cnt--;
#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
    if (gui_ptr->refresh_pending) {
        vk_disp_area_t area = gui_ptr->refresh_pending_area;
        gui_ptr->refresh_pending = false;
        vsf_unprotect_int(orig);

        __vsf_tgui_v_disp_refresh(gui_ptr, &area);
    } else {
        vsf_unprotect_int(orig);
    }
#else
    vsf_unprotect_int(orig);
#endif

    if (gui_ptr->refresh_pending_notify) {
        vsf_tgui_low_level_refresh_ready(gui_ptr);
    }

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    uint32_t elapse = vsf_systimer_tick_to_ms(vsf_systimer_get_tick() - gui_ptr->start_cnt);
    gui_ptr->refresh_cnt++;
    if (elapse >= 1000) {
        gui_ptr->fps = gui_ptr->refresh_cnt;
        gui_ptr->refresh_cnt = 0;
    }
#endif
}

static void __vsf_tgui_on_inited(vk_disp_t *disp)
{
    vsf_tgui_t *gui_ptr = (vsf_tgui_t *)disp->ui_data;
    gui_ptr->is_disp_inited = true;
	disp->ui_on_ready = __vsf_tgui_on_ready;
	__vsf_tgui_on_ready(disp);
}

#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
uint32_t vsf_tgui_port_get_refresh_rate(vsf_tgui_t *gui_ptr)
{
    return gui_ptr->fps;
}
#endif

void vsf_tgui_v_bind_disp(vsf_tgui_t *gui_ptr, vk_disp_t *disp, void *pfb, size_t pfb_size)
{
    VSF_TGUI_ASSERT(disp != NULL);
    VSF_TGUI_ASSERT(pfb != NULL);
    VSF_TGUI_ASSERT(pfb_size >= VSF_TGUI_HOR_MAX);

    gui_ptr->disp = disp;
    gui_ptr->pfb = pfb;
    // refresh_pending_cnt will be decreased once in __vsf_tgui_on_inited
    gui_ptr->refresh_pending_cnt = 1;
    gui_ptr->refresh_pending_notify = false;
#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
    gui_ptr->refresh_pending = false;
    gui_ptr->refresh_is_first_pfb = true;
    gui_ptr->pfb_size = pfb_size >> 1;
#else
    gui_ptr->pfb_size = pfb_size;
#endif

    gui_ptr->is_disp_inited = false;
    disp->ui_data = gui_ptr;
    disp->ui_on_ready = __vsf_tgui_on_inited;
    vk_disp_init(disp);
}
#endif

/* EOF */
