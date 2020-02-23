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

#define __VSF_TGUI_CONTROLS_PANEL_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_control.h"
#include "./vsf_tgui_sv_panel.h"
#include "./vsf_tgui_sv_label.h"
#include "./vsf_tgui_sv_container.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_panel_v_init(vsf_tgui_panel_t* ptPanel)
{
#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]panel init" VSF_TRACE_CFG_LINEEND);
#endif

    if (fsm_rt_cpl == vsf_tgui_container_v_init(&(ptPanel->use_as__vsf_tgui_container_t))) {
        /*! temporarily put this code here*/
        tgui_set_priv_label(ptPanel, tTitleLabel,
            tgui_attribute(tLabel, ptPanel->tTitle),
            tgui_region(
                tgui_location(0, 0),
                tgui_size(ptPanel->iWidth - ptPanel->tConatinerPadding.chLeft - ptPanel->tConatinerPadding.chRight, 32),
            )
        );
    
        vk_tgui_label_init(&(ptPanel->tTitleLabel));

        ptPanel->use_as__vsf_tgui_container_t.use_as__vsf_tgui_v_container_t.bIsShowCornerTile = true;

        return fsm_rt_cpl;
    }
    return fsm_rt_on_going;
}

fsm_rt_t vsf_tgui_panel_v_rendering(vsf_tgui_panel_t* ptPanel,
                                    vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t tMode)
{
    //vsf_tgui_label_t* ptLabel;
    //vsf_tgui_region_t tLableDirtyRegion;

    VSF_TGUI_ASSERT(ptPanel != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]panel rendering" VSF_TRACE_CFG_LINEEND);
#endif
    
    vsf_tgui_container_v_rendering(&(ptPanel->use_as__vsf_tgui_container_t), ptDirtyRegion, tMode);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_panel_v_depose(vsf_tgui_panel_t* ptPanel)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_panel_v_update(vsf_tgui_panel_t* ptPanel)
{
    if (!ptPanel->tTitle.bIsAutoSize) {
        __vsf_tgui_control_core_t *ptCore 
            = vsf_tgui_control_get_core((const vsf_tgui_control_t *)&(ptPanel->tTitleLabel));
        ptCore->tRegion.tSize.iWidth = ptPanel->iWidth - ptPanel->tConatinerPadding.chLeft - ptPanel->tConatinerPadding.chRight;
    }
    return fsm_rt_cpl;
}


#endif


/* EOF */
