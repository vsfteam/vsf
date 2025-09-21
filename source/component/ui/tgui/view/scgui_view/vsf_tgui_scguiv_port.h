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

#ifndef __VSF_TINY_GUI_SCGUIV_PORT_H__
#define __VSF_TINY_GUI_SCGUIV_PORT_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SCGUI_VIEW

#define __VSF_DISP_CLASS_INHERIT__
#include "component/ui/disp/vsf_disp.h"

#include "./scgui.h"

#ifdef __cplusplus
extern "C" {
#endif

declare_class(vsf_tgui_t)

/*============================ MACROS ========================================*/

#if VSF_TGUI_CFG_COLOR_MODE != VSF_TGUI_COLOR_MODE_RGB565
#   error "SCGui support RGB565 ONLY"
#endif

#if VSF_TGUI_CFG_FONT_USE_FREETYPE == ENABLED
#   error "SCGui has internal font implementation, can not use freetype"
#endif
#if VSF_TGUI_CFG_FONT_USE_LVGL != ENABLED
#   error "SCGui uses lvgl as internal font, please enable VSF_TGUI_CFG_FONT_USE_LVGL"
#endif

#ifndef VSF_TGUI_HOR_MAX
#   define VSF_TGUI_HOR_MAX                         800
#endif

#ifndef VSF_TGUI_VER_MAX
#   define VSF_TGUI_VER_MAX                         600
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_tgui_vport_init(__vsf_tgui_v_port_ptr)                              \
    do {                                                                        \
        (__vsf_tgui_v_port_ptr)->current_region.tSize.iHeight = 0;              \
        (__vsf_tgui_v_port_ptr)->current_region.tSize.iWidth = 0;               \
    } while(0)

/*============================ TYPES =========================================*/

typedef uint16_t vsf_tgui_v_color_t;

def_structure(vsf_tgui_v_port_t)
    vk_disp_t *disp;
    bool is_disp_inited;
    bool refresh_is_first_pfb;
    bool refresh_pending;
    bool refresh_pending_notify;
    uint8_t refresh_pending_cnt;
    vk_disp_area_t refresh_pending_area;
    uint16_t *refresh_pending_buffer;

    SC_tile cur_tile;
    vsf_tgui_region_t request_region;
    vsf_tgui_region_t current_region;
end_def_structure(vsf_tgui_v_port_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void __vsf_tgui_v_update_dirty_region(vsf_tgui_t *gui_ptr,
                                        const vsf_tgui_control_t* control_ptr,
                                        const vsf_tgui_region_t* dirty_region_ptr);

extern void vsf_tgui_control_v_draw_rect(vsf_tgui_t* gui_ptr,
                                         const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         const vsf_tgui_region_t* ptRectRegion,
                                         const vsf_tgui_v_color_t color);

extern void vsf_tgui_control_v_draw_text(vsf_tgui_t* gui_ptr,
                                         const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         vsf_tgui_text_info_t *ptStringInfo,
                                         const uint8_t font_index,
                                         const vsf_tgui_v_color_t color,
                                         const vsf_tgui_align_mode_t mode);

extern void vsf_tgui_control_v_draw_tile(vsf_tgui_t* gui_ptr,
                                         const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         const vsf_tgui_tile_t* tile_ptr,
                                         const vsf_tgui_align_mode_t mode,
                                         const uint8_t trans_rate,
                                         vsf_tgui_region_t* ptPlacedRegion,
                                         vsf_tgui_v_color_t color,
                                         vsf_tgui_v_color_t bg_color);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
