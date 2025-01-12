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

#define __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_scguiv_text_list.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_text_list_v_init(vsf_tgui_t *gui_ptr, vsf_tgui_text_list_t* text_list_ptr)
{
    vsf_tgui_container_v_init(gui_ptr, &text_list_ptr->use_as__vsf_tgui_container_t);
    vsf_tgui_container_v_init(gui_ptr, &text_list_ptr->tList.use_as__vsf_tgui_container_t);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_text_list_v_rendering(vsf_tgui_t *gui_ptr,
                                        vsf_tgui_text_list_t* text_list_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t mode)
{
    vsf_tgui_container_v_rendering(gui_ptr, &text_list_ptr->use_as__vsf_tgui_container_t, dirty_region_ptr, mode);
#if VSF_TGUI_CFG_V_SUPPORT_ROUND_BORDER == ENABLED
    int16_t height = text_list_ptr->iHeight - 2 * text_list_ptr->border_radius;
#else
    int16_t height = text_list_ptr->iHeight;
#endif
    vsf_tgui_label_t *label_ptr = &text_list_ptr->tList.tContent;
    extern uint8_t vsf_tgui_font_get_char_height(const uint8_t font_index);
    int16_t text_height = vsf_tgui_font_get_char_height(label_ptr->font_index) + label_ptr->tLabel.chInterLineSpace;
    height = vsf_min(height, text_height);
    if (height > 0) {
        const vsf_tgui_region_t indicator_region = {
            .tSize          = {
                .iWidth     = text_list_ptr->iWidth,
                .iHeight    = height,
            },
            .tLocation      = {
                .iX         = 0,
                .iY         = (text_list_ptr->iHeight - height) >> 1,
            },
        };
        vsf_tgui_control_v_draw_rect(gui_ptr, &text_list_ptr->use_as__vsf_tgui_control_t,
            dirty_region_ptr, &indicator_region, VSF_TGUI_CFG_V_TEXT_LIST_INDICATOR_COLOR);
    }
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_text_list_v_post_rendering(vsf_tgui_t *gui_ptr,
                                        vsf_tgui_text_list_t* text_list_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,
                                        vsf_tgui_control_refresh_mode_t mode)
{
    return vsf_tgui_container_v_post_rendering(gui_ptr, &text_list_ptr->use_as__vsf_tgui_container_t, dirty_region_ptr, mode);
}

fsm_rt_t vsf_tgui_text_list_v_depose(vsf_tgui_t *gui_ptr, vsf_tgui_text_list_t* text_list_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_text_list_v_update(vsf_tgui_t *gui_ptr, vsf_tgui_text_list_t* text_list_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
