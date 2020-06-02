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
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#define __VSF_TGUI_CONTROLS_BUTTON_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_button.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
#if VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES == ENABLED
static const vsf_tgui_align_mode_t sTilesAlign[] = {
    {VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_RIGHT},
};
#endif
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_button_v_init(vsf_tgui_button_t* ptButton)
{
#if (VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) button init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptButton), ptButton);
#endif

    return vsf_tgui_label_v_init(&(ptButton->use_as__vsf_tgui_label_t));
}

fsm_rt_t vsf_tgui_button_v_rendering(   vsf_tgui_button_t* ptButton,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode)
{

    VSF_TGUI_ASSERT(ptButton != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) button rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptButton), ptButton);
#endif

#ifdef VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR
    if (!ptButton->use_as__vsf_tgui_v_label_t.bIsNoBackgroundColor) {
        vsf_tgui_control_t* ptControl = (vsf_tgui_control_t*)ptButton;
        vsf_tgui_sv_color_t tColor = VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR;
        vsf_tgui_region_t tRegion = { 0 };

        tRegion.tSize = *vsf_tgui_control_get_size(ptControl);

#if VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES == ENABLED
        do {
            vsf_tgui_tile_t* ptTile;
            vsf_tgui_region_t tLeftRegion;
            vsf_tgui_region_t tRightRegion;

            ptTile = vsf_tgui_tile_get_root(&c_tLabelAdditionalTiles._.tLeft, &tLeftRegion);
            VSF_TGUI_ASSERT(ptTile != NULL);

            ptTile = vsf_tgui_tile_get_root(&c_tLabelAdditionalTiles._.tRight, &tRightRegion);
            VSF_TGUI_ASSERT(ptTile != NULL);

            tRegion.tLocation.iX += tLeftRegion.tSize.iWidth;
            tRegion.tSize.iWidth -= tLeftRegion.tSize.iWidth + tRightRegion.tSize.iWidth;
        } while (0);
#endif

        vsf_tgui_control_v_draw_rect(   ptControl,
                                        ptDirtyRegion,
                                        &tRegion,
                                        tColor);
    }
#endif

#if VSF_TGUI_CFG_SV_BUTTON_ADDITIONAL_TILES == ENABLED
    do {
        vsf_tgui_control_t* ptControl = &ptButton->use_as__vsf_tgui_label_t.use_as__vsf_tgui_control_t;
    #ifdef VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR
        uint_fast8_t tRate = vsf_tgui_control_v_get_tile_trans_rate(ptControl);
        uint_fast8_t tBackGroundRate = vsf_tgui_sv_color_get_trans_rate(VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR);
        vsf_tgui_control_v_set_tile_trans_rate(ptControl, tBackGroundRate);
    #endif

        for (int i = 0; i < dimof(sTilesAlign); i++) {
            vsf_tgui_control_v_draw_tile(   ptControl,
                                            ptDirtyRegion,
                                            &c_tLabelAdditionalTiles.tTiles[i],
                                            sTilesAlign[i]);
        }

    #ifdef VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR
        vsf_tgui_control_v_set_tile_trans_rate(ptControl, tRate);
#endif
    } while (0);
#endif

    __vsf_tgui_label_v_rendering((vsf_tgui_label_t *)ptButton, ptDirtyRegion, tMode);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_button_v_depose(vsf_tgui_button_t* ptButton)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_button_v_update(vsf_tgui_button_t* ptButton)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
