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
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW



#define __VSF_TGUI_CONTROLS_LABEL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_label.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern vsf_tgui_sv_color_t vsf_tgui_sv_get_text_color(vsf_tgui_label_t* label_ptr);

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_label_t* label_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) label init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)label_ptr), label_ptr);
#endif
    return vsf_tgui_control_v_init(&label_ptr->use_as__vsf_tgui_control_t);
}

int_fast16_t __vk_tgui_label_get_line_height( const vsf_tgui_label_t* label_ptr)
{
    VSF_TGUI_ASSERT(NULL != label_ptr);
    return (int_fast16_t) vsf_tgui_font_get_char_height( label_ptr->font_index);
}

vsf_tgui_size_t __vk_tgui_label_v_text_get_size(vsf_tgui_label_t* label_ptr,
                                                uint16_t *line_count_ptr,
                                                uint8_t *char_height_ptr)
{
    VSF_TGUI_ASSERT(label_ptr != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED

    label_ptr->tLabel.bIsChanged = false;
    label_ptr->tLabel.tInfoCache.tStringSize = vsf_tgui_text_get_size(label_ptr->font_index,
                                                                    &(label_ptr->tLabel.tString),
                                                                    &(label_ptr->tLabel.tInfoCache.hwLines),
                                                                    &(label_ptr->tLabel.tInfoCache.chCharHeight),
                                                                    label_ptr->tLabel.chInterLineSpace);

    if (NULL != char_height_ptr) {
        *char_height_ptr = label_ptr->tLabel.tInfoCache.chCharHeight;
    }

    return label_ptr->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t size = vsf_tgui_text_get_size(
                                    label_ptr->font_index,
                                    &(label_ptr->tLabel.tString),
                                    line_count_ptr,
                                    char_height_ptr,
                                    label_ptr->tLabel.chInterLineSpace);

    return size;
#endif

}


vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* label_ptr)
{
    VSF_TGUI_ASSERT(label_ptr != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    vsf_tgui_size_t size = label_ptr->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t size = __vk_tgui_label_v_text_get_size(label_ptr, NULL, NULL);
#endif

#if VSF_TGUI_CFG_SV_SUPPORT_CORNER_TILE == ENABLED
    if (label_ptr->show_corner_tile) {
        vsf_tgui_region_t regions[__CORNOR_TILE_NUM] = { {0} };

        for (int i = 0; i < dimof(regions); i++) {
            const vsf_tgui_tile_t* tile_ptr = vsf_tgui_control_v_get_corner_tile((vsf_tgui_control_t *)label_ptr, i);
            if (tile_ptr != NULL) {
                vsf_tgui_tile_get_root(tile_ptr, &regions[i]);
            }
        }

        size.iWidth  += vsf_max(regions[CORNOR_TILE_IN_TOP_LEFT].iWidth,
                           regions[CORNOR_TILE_IN_BOTTOM_LEFT].iWidth)
                     +  vsf_max(regions[CORNOR_TILE_IN_TOP_RIGHT].iWidth,
                           regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iWidth);

        int16_t height  = vsf_max(regions[CORNOR_TILE_IN_TOP_LEFT].iHeight,
                              regions[CORNOR_TILE_IN_TOP_RIGHT].iHeight)
                        + vsf_max(regions[CORNOR_TILE_IN_BOTTOM_LEFT].iHeight,
                              regions[CORNOR_TILE_IN_BOTTOM_RIGHT].iHeight);
        size.iHeight = vsf_max(size.iHeight, height);
    }
#endif

    return size;
}

#ifndef WEAK_VSF_TGUI_SV_GET_TEXT_COLOR
vsf_tgui_sv_color_t vsf_tgui_sv_get_text_color(vsf_tgui_label_t* label_ptr)
{
#if VSF_TGUI_CFG_SV_LABLE_SUPPORT_TEXT_COLOR == ENABLED
    return label_ptr->font_color;
#elif VSF_TGUI_CFG_SV_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#   error "todo: "
#else
#   error "todo: "
#endif
}
#endif

fsm_rt_t vsf_tgui_label_v_rendering(vsf_tgui_label_t* label_ptr,
                                    vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t mode)
{
    VSF_TGUI_ASSERT(label_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) label rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)label_ptr), label_ptr);
#endif

    vsf_tgui_control_v_rendering((vsf_tgui_control_t *)label_ptr, dirty_region_ptr, mode);

    if (    (label_ptr->tLabel.tString.pstrText != NULL)
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (label_ptr->tLabel.tString.s16_size > 0)
#endif
    ) {
        vsf_tgui_control_v_draw_text((vsf_tgui_control_t *)label_ptr,
                                     dirty_region_ptr,
                                     &(label_ptr->tLabel),
                                     label_ptr->font_index,
                                     vsf_tgui_sv_get_text_color(label_ptr),
                                     label_ptr->tLabel.u4Align);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_label_t* label_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_update(vsf_tgui_label_t* label_ptr)
{
    return fsm_rt_cpl;
}


#endif


/* EOF */
