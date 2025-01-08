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

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
#define __VSF_TGUI_CLASS_INHERIT
#include "component/ui/tgui/vsf_tgui.h"

#include "./vsf_tgui_scguiv_control.h"

#include "./scgui/SCGUI/sc_gui.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_t* gui_ptr, vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

VSF_CAL_WEAK(vsf_tgui_v_get_background_color)
vsf_tgui_v_color_t vsf_tgui_v_get_background_color(vsf_tgui_control_t* control_ptr)
{
#if VSF_TGUI_CFG_V_SUPPORT_FLUXIBLE_BACKGROUND_COLOR == ENABLED
    vsf_tgui_v_color_t color = control_ptr->background_color;

    if (control_ptr->id == VSF_TGUI_COMPONENT_ID_BUTTON) {
        vsf_tgui_button_t* button_ptr = (vsf_tgui_button_t*)control_ptr;
        if (button_ptr->_.bIsChecked) {
            color = alphaBlend(color, VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_COLOR,
                VSF_TGUI_CFG_V_BUTTON_CLICKED_MIX_VALUE);
        }
    }

    return color;
#else
    return VSF_TGUI_COLOR_BLUE;
#endif
}

fsm_rt_t vsf_tgui_control_v_rendering(  vsf_tgui_t* gui_ptr,
                                        vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t mode)
{
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[SCgui View]%s(%p) control view rendering at ((%d, %d), (%d, %d))" VSF_TRACE_CFG_LINEEND,
        control_ptr->node_name_ptr, control_ptr, control_ptr->iX, control_ptr->iY, control_ptr->iWidth, control_ptr->iHeight);

    vsf_tgui_v_color_t bg_color = vsf_tgui_v_get_background_color(control_ptr);
    vsf_tgui_v_color_t border_color = control_ptr->border_width ? control_ptr->border_color : bg_color;
    int border_tweak = control_ptr->border_width ? 1 : 0;

    vsf_tgui_location_t location = { 0 };
    vsf_tgui_control_calculate_absolute_location(control_ptr, &location);

    SC_pfb_RoundFrame(&gui_ptr->cur_tile,
        location.iX, location.iY,
        location.iX + control_ptr->tRegion.iWidth - 1,
        location.iY + control_ptr->tRegion.iHeight - 1,
        control_ptr->border_radius, control_ptr->border_radius - control_ptr->border_width + border_tweak,
        border_color, bg_color);
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_depose(vsf_tgui_t* gui_ptr, vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_update(vsf_tgui_t* gui_ptr, vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
