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

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_control.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
const char* vsf_tgui_control_get_node_name(vsf_tgui_control_t* ptControl)
{
    return ptControl->use_as__vsf_msgt_node_t.pchNodeName;
}
#endif

fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_control_t* ptControl)
{
#if (VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view init" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(ptControl), ptControl);
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

#if (VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view rendering" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(ptControl), ptControl);
#endif

    ptCore = vsf_tgui_control_get_core(ptControl);
    ptTile = ptCore->tBackground.ptTile;
    if (ptTile != NULL) {
        vsf_tgui_control_v_draw_tile(ptControl, ptDirtyRegion, ptTile, ptCore->tBackground.tAlign);
    }

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
