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

#define __VSF_TGUI_CONTROLS_CONTAINER_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_container.h"
#include "./vsf_tgui_sv_draw.h"
#include "./vsf_tgui_sv_control.h"
#include "./vsf_tgui_sv_style.h"

#include "../../utilities/vsf_tgui_color.h"

/*============================ MACROS ========================================*/
// Support Panel additional tiles
#ifndef VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES
#   define VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES 				    DISABLED
#endif

#ifndef VSF_TGUI_SV_CFG_PANEL_BACKGROUND_COLOR
#   define VSF_TGUI_SV_CFG_PANEL_BACKGROUND_COLOR       {0}
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
#if VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES == ENABLED
static const vsf_tgui_align_mode_t s_tTilesAlign[] = {
    {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT},
    {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT},
};
#endif

/*============================ PROTOTYPES ====================================*/
#if VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES == ENABLED
extern const vsf_tgui_sv_container_corner_tiles_t g_tContainerCornerTiles;
#endif


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_container_v_init(vsf_tgui_container_t* ptContainer)
{
#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]container init" VSF_TRACE_CFG_LINEEND);
#endif
    return fsm_rt_cpl;
}


fsm_rt_t __vk_tgui_container_v_rendering(vsf_tgui_container_t* ptContainer,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode,
                                        vsf_tgui_color_t tBackground)
{
    //fsm_rt_t tResult = fsm_rt_cpl;

    VSF_TGUI_ASSERT(ptContainer != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

    if (!ptContainer->use_as__vsf_tgui_v_container_t.bIsNoBackgroundColor) {
        vsf_tgui_control_t* ptControl = (vsf_tgui_control_t*)ptContainer;
        vsf_tgui_color_t tColor = tBackground;
        vsf_tgui_region_t tRegion = { 0 };

        tRegion.tSize = vsf_tgui_control_get_size(ptControl);
        vsf_tgui_control_v_draw_rect(   ptControl,
                                        ptDirtyRegion,
                                        &tRegion,
                                        tColor);
    }

#if VSF_TGUI_SV_CFG_PANEL_ADDITIONAL_TILES == ENABLED
    if (ptContainer->use_as__vsf_tgui_v_container_t.bIsShowCornerTile) {
        for (int i = 0; i < dimof(s_tTilesAlign); i++) {
            vsf_tgui_control_v_draw_tile(   (vsf_tgui_control_t*)ptContainer,
                                            ptDirtyRegion,
                                            &g_tContainerCornerTiles.tTiles[i],
                                            s_tTilesAlign[i]);
        }
    }
#endif

    return vsf_tgui_control_v_rendering((vsf_tgui_control_t *)ptContainer, ptDirtyRegion, tMode);
}


fsm_rt_t vsf_tgui_container_v_rendering(vsf_tgui_container_t* ptContainer,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode)
{
    //fsm_rt_t tResult = fsm_rt_cpl;

    VSF_TGUI_ASSERT(ptContainer != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]container rendering" VSF_TRACE_CFG_LINEEND);
#endif

    return __vk_tgui_container_v_rendering(ptContainer, ptDirtyRegion, tMode, (vsf_tgui_color_t)VSF_TGUI_SV_CFG_PANEL_BACKGROUND_COLOR);
}

fsm_rt_t vsf_tgui_container_v_depose(vsf_tgui_container_t* ptContainer)
{
    return fsm_rt_cpl;
}


fsm_rt_t vsf_tgui_container_v_update(vsf_tgui_container_t* ptContainer)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
