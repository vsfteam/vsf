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

#define __VSF_TGUI_CONTROLS_CONTROLE_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_control.h"
#include "./vsf_tgui_sv_draw.h"

#include "../../utilities/vsf_tgui_color.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_control_t* ptControl)
{   
#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]control init" VSF_TRACE_CFG_LINEEND);
#endif

    /*
    vsf_tgui_status_t tStatus = vsf_tgui_control_status_get((vsf_tgui_control_t*)ptControl);
    tStatus.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED    |
                        VSF_TGUI_CTRL_STATUS_VISIBLE        |
                        VSF_TGUI_CTRL_STATUS_ENABLED;

    vsf_tgui_control_status_set((vsf_tgui_control_t *)ptControl, tStatus);
    
    return vk_tgui_control_update(ptControl);
    */
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_rendering(  vsf_tgui_control_t* ptControl,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode)
{
    __vsf_tgui_control_core_t* ptCore;
    const vsf_tgui_tile_t* ptTile;

    VSF_TGUI_ASSERT(ptControl != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]control rendering" VSF_TRACE_CFG_LINEEND);
#endif

    ptCore = vsf_tgui_control_get_core(ptControl);
    ptTile = ptCore->tBackground.ptTile;
    if (ptTile != NULL) {
        vsf_tgui_control_v_draw_tile(ptControl, ptDirtyRegion, ptTile, ptCore->tBackground.tAlign);
    }

#if 0
    do {
        int16_t nLineWidth = 2;
        int i;
        vsf_tgui_size_t tSize = vsf_tgui_control_get_size(ptControl);
        vsf_tgui_region_t tRectRegions[] = {
            {.tLocation = {.iX = 0, .iY = 0}, .tSize = {.iWidth = tSize.iWidth, .iHeight = nLineWidth},},  // Top
            {.tLocation = {.iX = 0, .iY = tSize.iHeight - nLineWidth},  .tSize = {.iWidth = tSize.iWidth, .iHeight = nLineWidth},}, // bottom
            {.tLocation = {.iX = 0, .iY = nLineWidth}, .tSize = {.iWidth = nLineWidth, .iHeight = tSize.iHeight - 2 * nLineWidth},}, // left
            {.tLocation = {.iX = tSize.iWidth - nLineWidth, .iY = nLineWidth}, .tSize = {.iWidth = nLineWidth, .iHeight = tSize.iHeight - 2 * nLineWidth},}, // right
        };
        vsf_tgui_color_t tColor = VSF_TGUI_COLOR_BLACK; 

        for (i = 0; i < dimof(tRectRegions); i++) {
            vsf_tgui_control_v_draw_rect(ptControl,
                                         ptDirtyRegion,
                                         &tRectRegions[i],
                                         tColor);
        }
    } while (0);
#endif

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_depose(vsf_tgui_control_t* ptControl)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_update(vsf_tgui_control_t* ptControl)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
