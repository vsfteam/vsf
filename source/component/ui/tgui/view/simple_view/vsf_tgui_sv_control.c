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
#define __VSF_TGUI_CONTROLS_BUTTON_CLASS_IMPLEMENT

#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_control.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern vsf_tgui_sv_color_t vsf_tgui_sv_get_background_color(vsf_tgui_control_t* control_ptr);
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
const char* vsf_tgui_control_get_node_name(vsf_tgui_control_t* control_ptr)
{
    return control_ptr->use_as__vsf_msgt_node_t.node_name_ptr;
}
#endif

#ifndef WEAK_VSF_TGUI_SV_GET_CORNOR_TILE
const vsf_tgui_tile_t* vsf_tgui_control_v_get_corner_tile(vsf_tgui_control_t* control_ptr, vsf_tgui_sv_cornor_tile_mode_t mode)
{
    return NULL;
}
#endif

#ifndef WEAK_VSF_TGUI_SV_GET_BACKGROUND_COLOR
vsf_tgui_sv_color_t vsf_tgui_sv_get_background_color(vsf_tgui_control_t* control_ptr)
{
#if VSF_TGUI_CFG_SV_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
    vsf_tgui_sv_color_t color = control_ptr->background_color;

    if (control_ptr->id == VSF_TGUI_COMPONENT_ID_BUTTON) {
        vsf_tgui_button_t* button_ptr = (vsf_tgui_button_t*)control_ptr;
        if (button_ptr->_.bIsChecked) {
            color = vsf_tgui_sv_color_mix(color, VSF_TGUI_CFG_SV_BUTTON_CLICKED_MIX_COLOR, VSF_TGUI_CFG_SV_BUTTON_CLICKED_MIX_VALUE);
        }
    }

    return color;
#else
    return VSF_TGUI_COLOR_BLUE;
#endif
}
#endif

fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_control_t* control_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view init" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(control_ptr), control_ptr);
#endif

    /*
    vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)control_ptr);
    Status.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED    |
                        VSF_TGUI_CTRL_STATUS_VISIBLE        |
                        VSF_TGUI_CTRL_STATUS_ENABLED;

    vsf_tgui_control_status_set((vsf_tgui_control_t *)control_ptr, Status);

    return vk_tgui_control_update(control_ptr);
    */
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_rendering(  vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t mode)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view rendering" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(control_ptr), control_ptr);
#endif

#if VSF_TGUI_CFG_SV_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED || VSF_TGUI_CFG_SV_SUPPORT_FIXED_BACKGROUND_COLOR == ENABLED
    vsf_tgui_sv_color_t color = vsf_tgui_sv_get_background_color(control_ptr);
    uint_fast8_t trans_rate = vsf_tgui_sv_color_get_trans_rate(color);
    if (trans_rate) {
        vsf_tgui_region_t region = {
            .tLocation = {.iX = 0, .iY = 0},
            .tSize = *vsf_tgui_control_get_size(control_ptr),
        };

#if VSF_TGUI_CFG_SV_SUPPORT_CORNER_TILE != ENABLED
        vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
#else
        if (!control_ptr->show_corner_tile) {
            vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
        }else {
            static const vsf_tgui_align_mode_t __cornor_tiles_align_mode[] = {
                {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT},
                {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT},
                {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT},
                {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT},
            };
            vsf_tgui_region_t regions[dimof(__cornor_tiles_align_mode)];

            for (int i = 0; i < dimof(__cornor_tiles_align_mode); i++) {
                const vsf_tgui_tile_t* tile_ptr = vsf_tgui_control_v_get_corner_tile(control_ptr, i);
                if (tile_ptr) {
                    vsf_tgui_control_v_draw_tile(control_ptr, dirty_region_ptr, tile_ptr, __cornor_tiles_align_mode[i], trans_rate, &regions[i]);
                } else {
                    memset(&regions[i], 0, sizeof(vsf_tgui_region_t));
                }
            }

            int16_t top_width = control_ptr->iWidth - regions[CORNOR_TILE_IN_TOP_LEFT].iWidth - regions[CORNOR_TILE_IN_TOP_RIGHT].iWidth;
            int16_t bottom_width = control_ptr->iWidth - regions[CORNOR_TILE_IN_BOTTOM_LEFT].iWidth - regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iWidth;
            int16_t left_height = control_ptr->iHeight - regions[CORNOR_TILE_IN_TOP_LEFT].iHeight - regions[CORNOR_TILE_IN_BOTTOM_LEFT].iHeight;
            int16_t right_height = control_ptr->iHeight - regions[CORNOR_TILE_IN_TOP_RIGHT].iHeight - regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iHeight;

            int16_t top_height = vsf_max(regions[CORNOR_TILE_IN_TOP_LEFT].iHeight, regions[CORNOR_TILE_IN_TOP_RIGHT].iHeight);
            if (top_width > 0 && top_height > 0) {
                vsf_tgui_region_t region = {
                    .tLocation = {
                        .iX = regions[CORNOR_TILE_IN_TOP_LEFT].iWidth,
                        .iY = 0,
                    },
                    .tSize = {
                        .iWidth = top_width,
                        .iHeight = top_height,
                    },
                };
                vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
            }

            int16_t botton_height = vsf_max(regions[CORNOR_TILE_IN_BOTTOM_LEFT].iHeight, regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iHeight);
            if (bottom_width > 0 && botton_height > 0) {
                vsf_tgui_region_t region = {
                    .tLocation = {
                        .iX = regions[CORNOR_TILE_IN_BOTTOM_LEFT].iWidth,
                        .iY = control_ptr->iHeight - botton_height,
                    },
                    .tSize = {
                        .iWidth = bottom_width,
                        .iHeight = botton_height,
                    },
                };
                vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
            }

            int16_t left_width = vsf_min(regions[CORNOR_TILE_IN_TOP_LEFT].iWidth, regions[CORNOR_TILE_IN_TOP_RIGHT].iWidth);
            if (left_width > 0 && left_height > 0) {
                vsf_tgui_region_t region = {
                    .tLocation = {
                        .iX = 0,
                        .iY = regions[CORNOR_TILE_IN_TOP_LEFT].iHeight,
                    },
                    .tSize = {
                        .iWidth = left_width,
                        .iHeight = left_height,
                    },
                };
                vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
            }

            int16_t right_width = vsf_min(regions[CORNOR_TILE_IN_BOTTOM_LEFT].iWidth, regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iWidth);
            if (right_width > 0 && right_height > 0) {
                vsf_tgui_region_t region = {
                    .tLocation = {
                        .iX = control_ptr->iWidth - right_width,
                        .iY = regions[CORNOR_TILE_IN_BOTTOM_LEFT].iHeight,
                    },
                    .tSize = {
                        .iWidth = right_width,
                        .iHeight = right_height,
                    },
                };
                vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
            }

            int16_t remain_width = control_ptr->iWidth - left_width - right_width;
            int16_t remain_height = control_ptr->iHeight - top_height - botton_height;
            if (remain_width > 0 && remain_height > 0) {
                vsf_tgui_region_t region = {
                    .tLocation = {
                        .iX = left_width,
                        .iY = top_height,
                    },
                    .tSize = {
                        .iWidth = remain_width,
                        .iHeight = remain_height,
                    },
                };
                vsf_tgui_control_v_draw_rect(control_ptr, dirty_region_ptr, &region, color);
            }
        }
#endif
    }
#endif

    const vsf_tgui_tile_t* ptTile = control_ptr->tBackground.ptTile;
    if (ptTile != NULL) {
#if VSF_TGUI_CFG_SV_SUPPORT_TILE_TRANSPARENCY == ENABLED
        uint8_t tile_trans_rate = control_ptr->tile_trans_rate;
#else
        uint8_t tile_trans_rate = 0xFF;
#endif
        vsf_tgui_control_v_draw_tile(control_ptr, dirty_region_ptr, ptTile, control_ptr->tBackground.tAlign, tile_trans_rate, NULL);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_depose(vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_update(vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
