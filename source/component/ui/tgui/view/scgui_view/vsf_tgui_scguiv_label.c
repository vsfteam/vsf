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
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SCGUI_VIEW

#define __VSF_TGUI_CONTROLS_LABEL_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_scguiv_label.h"

#include "./scgui/SCGUI/sc_gui.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_t *gui_ptr, vsf_tgui_label_t* label_ptr)
{
    return fsm_rt_cpl;
}

VSF_CAL_WEAK(vsf_tgui_sv_get_text_color)
vsf_tgui_v_color_t vsf_tgui_v_get_text_color(vsf_tgui_label_t* label_ptr)
{
#if VSF_TGUI_CFG_V_LABLE_SUPPORT_TEXT_COLOR == ENABLED
    if (label_ptr->bIsEnabled) {
        return label_ptr->text_color;
    } else {
        return alphaBlend(label_ptr->text_color,
            VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_COLOR,
            VSF_TGUI_CFG_V_LABEL_DISABLED_TEXT_MIX_VALUE);
    }
#elif VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
#   error "todo: "
#else
#   error "todo: "
#endif
}

fsm_rt_t vsf_tgui_label_v_rendering(vsf_tgui_t* gui_ptr,
                                    vsf_tgui_label_t* label_ptr,
                                    vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t mode)
{
    vsf_tgui_control_v_rendering(gui_ptr, (vsf_tgui_control_t *)label_ptr, dirty_region_ptr, mode);
    if (    (label_ptr->tLabel.tString.pstrText != NULL)
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (label_ptr->tLabel.tString.s16_size > 0)
#endif
    ) {
        vsf_tgui_control_v_draw_text(gui_ptr,
                                     (vsf_tgui_control_t *)label_ptr,
                                     dirty_region_ptr,
                                     &(label_ptr->tLabel),
                                     label_ptr->font_index,
                                     vsf_tgui_v_get_text_color(label_ptr),
                                     label_ptr->tLabel.u4Align);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_t *gui_ptr, vsf_tgui_label_t* label_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_update(vsf_tgui_t *gui_ptr, vsf_tgui_label_t* label_ptr)
{
    return fsm_rt_cpl;
}

uint8_t __vk_tgui_label_v_get_font(vsf_tgui_label_t* label_ptr)
{
    return label_ptr->font_index;
}

vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* label_ptr)
{
    VSF_TGUI_ASSERT(label_ptr != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    vsf_tgui_size_t size = label_ptr->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t size = __vk_tgui_label_v_text_get_size(label_ptr, NULL, NULL);
#endif

#if VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER == ENABLED
    size.iWidth += label_ptr->border_radius * 2;
    size.iHeight += label_ptr->border_radius * 2;
#endif
    return size;
}

int_fast16_t __vk_tgui_label_get_line_height(const vsf_tgui_label_t* label_ptr)
{
    lv_font_t *font = (lv_font_t *)gui->font;
    return font->line_height;
}

#endif


/* EOF */
