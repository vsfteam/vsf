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
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SCGUI_VIEW

#include "./scgui.h"

/*============================ MACROS ========================================*/

#if VSF_TGUI_CFG_V_PORT_LOG != ENABLED
#   undef VSF_TGUI_LOG
#   define VSF_TGUI_LOG(...)
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
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
    if (!gui_ptr->is_disp_inited || gui_ptr->refresh_pending) {
        gui_ptr->refresh_pending_notify = true;
        vsf_unprotect_int(orig);
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View Port] no ready to refresh" VSF_TRACE_CFG_LINEEND);
        return NULL;
    }
    vsf_unprotect_int(orig);

    vsf_tgui_region_t *current_region_ptr = &gui_ptr->current_region;
    vsf_tgui_region_t *request_region_ptr = &gui_ptr->request_region;

    if (current_region_ptr->tSize.iHeight <= 0 || current_region_ptr->tSize.iWidth <= 0) {
        static const vsf_tgui_region_t s_displayRegion = {0,0, VSF_TGUI_HOR_MAX, VSF_TGUI_VER_MAX};
        if (!vsf_tgui_region_intersect(request_region_ptr, planned_refresh_region_ptr, &s_displayRegion)) {
            VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View Port] refresh region is not in screen" VSF_TRACE_CFG_LINEEND);
            return NULL;
        } else {
            VSF_TGUI_LOG(VSF_TRACE_INFO,
                         "[SCgui View Port] request low level refresh start in area, location(%d, %d), size(%d, %d)" VSF_TRACE_CFG_LINEEND,
                         request_region_ptr->tLocation.iX,
                         request_region_ptr->tLocation.iY,
                         request_region_ptr->tSize.iWidth,
                         request_region_ptr->tSize.iHeight);

            int16_t height = SC_PFB_BUF_SIZE / (request_region_ptr->tSize.iWidth << 1);
            height = vsf_min(request_region_ptr->tSize.iHeight, height);
            current_region_ptr->tLocation.iX = request_region_ptr->tLocation.iX;
            current_region_ptr->tLocation.iY = request_region_ptr->tLocation.iY;
            current_region_ptr->tSize.iWidth = request_region_ptr->tSize.iWidth;
            current_region_ptr->tSize.iHeight = height;

            SC_tile *cur_tile = &gui_ptr->cur_tile;
            SC_pfb_clip(cur_tile, request_region_ptr->tLocation.iX, request_region_ptr->tLocation.iY,
                request_region_ptr->tLocation.iX + request_region_ptr->tSize.iWidth - 1,
                request_region_ptr->tLocation.iY + request_region_ptr->tSize.iHeight - 1,
                0);

            VSF_TGUI_LOG(VSF_TRACE_INFO,
                         "[SCgui View Port] current region location(%d, %d), size(%d, %d)" VSF_TRACE_CFG_LINEEND,
                         cur_tile->xs, cur_tile->ys, cur_tile->w, cur_tile->h);
        }
    } else {
        SC_pfb_clip(&gui_ptr->cur_tile, request_region_ptr->tLocation.iX, request_region_ptr->tLocation.iY,
                request_region_ptr->tLocation.iX + request_region_ptr->tSize.iWidth - 1,
                request_region_ptr->tLocation.iY + request_region_ptr->tSize.iHeight - 1,
                0);
    }

    return current_region_ptr;
}

static void __vsf_tgui_v_disp_refresh(vsf_tgui_t *gui_ptr, vk_disp_area_t *area, uint16_t *buffer)
{
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View Port]vk_disp_refresh (%d %d) (%d %d)." VSF_TRACE_CFG_LINEEND,
                 area->pos.x, area->pos.y, area->size.x, area->size.y);

    if(++gui_ptr->cur_tile.stup >= gui_ptr->cur_tile.num) {
        gui_ptr->cur_tile.stup = 0;
    }
    vk_disp_refresh(gui_ptr->disp, area, buffer);

    vsf_protect_t orig = vsf_protect_int();
        gui_ptr->refresh_pending_cnt++;
    vsf_unprotect_int(orig);

    // switch SCGui pfb
    if (gui->dma_i) {
        if (gui->dma_prt != gui->pfb_buf) {
            gui->dma_prt = gui->pfb_buf;
        } else {
            gui->dma_prt = gui->pfb_buf + (SC_PFB_BUF_SIZE >> 1);
        }
        gui->dma_i = 0;
    }
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

    vsf_protect_t orig = vsf_protect_int();
    if (gui_ptr->refresh_pending_cnt > 0) {
        gui_ptr->refresh_pending = true;
        gui_ptr->refresh_pending_area = area;
        gui_ptr->refresh_pending_buffer = gui_ptr->cur_tile.buf;
        vsf_unprotect_int(orig);
    } else {
        vsf_unprotect_int(orig);
        __vsf_tgui_v_disp_refresh(gui_ptr, &area, gui_ptr->cur_tile.buf);
    }

    int request_location_y2 = request_region_ptr->tLocation.iY + request_region_ptr->tSize.iHeight;
    int current_location_y2 = current_region_ptr->tLocation.iY + current_region_ptr->tSize.iHeight;
    int remain = request_location_y2 - current_location_y2;

    if (remain > 0) {
        int16_t height = SC_PFB_BUF_SIZE / (request_region_ptr->tSize.iWidth << 1);
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View Port]iHeight: %d, current_region_ptr->tLocation.iY:%d, request_location_y2:%d" VSF_TRACE_CFG_LINEEND,
                     height, current_region_ptr->tLocation.iY, request_location_y2);
        height = vsf_min(height, remain);
        current_region_ptr->tLocation.iY += current_region_ptr->tSize.iHeight;
        current_region_ptr->tSize.iHeight = height;
        return true;
    } else {
        memset(current_region_ptr, 0, sizeof(*current_region_ptr));
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View Port]End Refresh Loop." VSF_TRACE_CFG_LINEEND);
        return false;
    }
}

static void __vsf_tgui_on_disp_ready(vk_disp_t *disp)
{
    vsf_tgui_t *gui_ptr = (vsf_tgui_t *)disp->ui_data;

    vsf_protect_t orig = vsf_protect_int();
    gui_ptr->refresh_pending_cnt--;
    if (gui_ptr->refresh_pending) {
        vk_disp_area_t area = gui_ptr->refresh_pending_area;
        gui_ptr->refresh_pending = false;
        vsf_unprotect_int(orig);

        __vsf_tgui_v_disp_refresh(gui_ptr, &area, gui_ptr->refresh_pending_buffer);
    } else {
        vsf_unprotect_int(orig);
    }

    if (gui_ptr->refresh_pending_notify) {
        vsf_tgui_low_level_refresh_ready(gui_ptr);
    }
}

static void __vsf_tgui_on_disp_inited(vk_disp_t *disp)
{
    vsf_tgui_t *gui_ptr = (vsf_tgui_t *)disp->ui_data;
    gui_ptr->is_disp_inited = true;
	disp->ui_on_ready = __vsf_tgui_on_disp_ready;
	__vsf_tgui_on_disp_ready(disp);
}

void vsf_tgui_v_bind_disp(vsf_tgui_t *gui_ptr, vk_disp_t *disp, void *pfb, size_t pfb_size)
{
    VSF_TGUI_ASSERT(disp != NULL);
    VSF_TGUI_ASSERT(pfb != NULL);
    VSF_TGUI_ASSERT(pfb_size >= VSF_TGUI_HOR_MAX);

    extern lv_font_t lv_font_14;
    SC_GUI_Init(0,C_WHITE,C_RED);

    if ((pfb != NULL) && (pfb_size > 0)) {
        vsf_trace_warning("pdf not necessary for scgui view" VSF_TRACE_CFG_LINEEND);
    }

    gui_ptr->disp = disp;
    // refresh_pending_cnt will be decreased once in __vsf_tgui_on_inited
    gui_ptr->refresh_pending_cnt = 1;
    gui_ptr->refresh_pending_notify = false;
    gui_ptr->refresh_pending = false;

    gui_ptr->is_disp_inited = false;
    disp->ui_data = gui_ptr;
    disp->ui_on_ready = __vsf_tgui_on_disp_inited;
    vk_disp_init(disp);
}

// draw

void vsf_tgui_control_v_draw_rect(  vsf_tgui_t* gui_ptr,
                                    const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    const vsf_tgui_region_t* region_ptr,
                                    const vsf_tgui_v_color_t color)
{
    vsf_tgui_region_t control_abs_region = { 0 };
    vsf_tgui_control_calculate_absolute_location(control_ptr, &control_abs_region.tLocation);
    vsf_tgui_region_t dirty_abs_region = *dirty_region_ptr;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &dirty_abs_region.tLocation);

    gui->lcd_area.xs = dirty_abs_region.iX;
    gui->lcd_area.ys = dirty_abs_region.iY;
    gui->lcd_area.xe = dirty_abs_region.iX + dirty_abs_region.iWidth - 1;
    gui->lcd_area.ye = dirty_abs_region.iY + dirty_abs_region.iHeight - 1;
    SC_pfb_DrawFill(&gui_ptr->cur_tile,
        control_abs_region.iX + region_ptr->iX, control_abs_region.iY + region_ptr->iY,
        control_abs_region.iX + region_ptr->iX + region_ptr->iWidth - 1,
        control_abs_region.iY + region_ptr->iY + region_ptr->iHeight - 1,
        color
    );
}

void vsf_tgui_control_v_draw_text(  vsf_tgui_t* gui_ptr,
                                    const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    vsf_tgui_text_info_t *ptStringInfo,
                                    const uint8_t font_index,
                                    const vsf_tgui_v_color_t color,
                                    const vsf_tgui_align_mode_t mode)
{
    const vsf_tgui_font_t *font_ptr = vsf_tgui_font_get(font_index);
    VSF_TGUI_ASSERT(font_ptr != NULL);
    lv_font_t *font = (lv_font_t *)font_ptr->font;

    vsf_tgui_region_t temp_region = {
        .tSize      = vsf_tgui_text_get_size(font_index, ptStringInfo, NULL, NULL),
    };
    vsf_tgui_region_t text_abs_region = {
        .tSize      = control_ptr->tSize,
    };
    vsf_tgui_region_update_with_align(&text_abs_region, &temp_region, mode);
    text_abs_region.iX = 0;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &text_abs_region.tLocation);

    vsf_tgui_region_t dirty_abs_region = *dirty_region_ptr;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &dirty_abs_region.tLocation);
    gui->lcd_area.xs = dirty_abs_region.iX;
    gui->lcd_area.ys = dirty_abs_region.iY;
    gui->lcd_area.xe = dirty_abs_region.iX + dirty_abs_region.iWidth - 1;
    gui->lcd_area.ye = dirty_abs_region.iY + dirty_abs_region.iHeight - 1;

    uint16_t alpha = gui->alpha;
    gui->alpha = 0;
    SC_pfb_printf(&gui_ptr->cur_tile, text_abs_region.iX, text_abs_region.iY,
        ptStringInfo->tString.pstrText, color, 0, font, control_ptr->iWidth,
        ptStringInfo->chInterLineSpace, (SC_ALIGN)mode);
    gui->alpha = alpha;
}

void vsf_tgui_control_v_draw_tile(  vsf_tgui_t* gui_ptr,
                                    const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    const vsf_tgui_tile_t* tile_ptr,
                                    const vsf_tgui_align_mode_t mode,
                                    const uint8_t trans_rate,
                                    vsf_tgui_region_t* placed_region_ptr,
                                    vsf_tgui_v_color_t color,
                                    vsf_tgui_v_color_t bg_color)
{
    vsf_tgui_region_t resource_region;

    tile_ptr = vsf_tgui_tile_get_root(tile_ptr, &resource_region);
    VSF_TGUI_ASSERT(tile_ptr != NULL);
    vsf_tgui_size_t tile_size = vsf_tgui_root_tile_get_size(tile_ptr);

    if (placed_region_ptr != NULL) {
        *placed_region_ptr = resource_region;
    }

    vsf_tgui_region_t image_abs_region = {
        .tSize      = control_ptr->tSize,
    };
    vsf_tgui_region_update_with_align(&image_abs_region, &resource_region, mode);
    vsf_tgui_control_calculate_absolute_location(control_ptr, &image_abs_region.tLocation);

    vsf_tgui_region_t dirty_abs_region = *dirty_region_ptr;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &dirty_abs_region.tLocation);
    gui->lcd_area.xs = dirty_abs_region.iX;
    gui->lcd_area.ys = dirty_abs_region.iY;
    gui->lcd_area.xe = dirty_abs_region.iX + dirty_abs_region.iWidth - 1;
    gui->lcd_area.ye = dirty_abs_region.iY + dirty_abs_region.iHeight - 1;

    uint16_t pixel_size = vsf_tgui_root_tile_get_pixel_bitsize(tile_ptr);
    SC_img_t img = {
        .map = vsf_tgui_tile_get_pixelmap(tile_ptr) + resource_region.iX * 16,
        .w = vsf_min(resource_region.iWidth, tile_size.iWidth - resource_region.iX),
        .h = vsf_min(resource_region.iHeight, tile_size.iHeight - resource_region.iY),
        .pitch = tile_ptr->tBufRoot.iWidth,
    };

    switch (tile_ptr->tCore.Attribute.u2ColorType) {
    case VSF_TGUI_TILE_COLORTYPE_RGB:
        switch (pixel_size) {
        case 16:    img.format = SC_IMAGE_RGB565;   break;
        default:
            VSF_TGUI_ASSERT(false);
            return;
        }
        break;
    case VSF_TGUI_TILE_COLORTYPE_RGBA:
        switch (pixel_size) {
        case 32:    img.format = SC_IMAGE_ARGB8888;  break;
        default:
            VSF_TGUI_ASSERT(false);
            return;
        }
        break;
    default:
        VSF_TGUI_ASSERT(false);
        return;
    }
    SC_pfb_Image(&gui_ptr->cur_tile, image_abs_region.iX, image_abs_region.iY, trans_rate, &img);
}

#endif
