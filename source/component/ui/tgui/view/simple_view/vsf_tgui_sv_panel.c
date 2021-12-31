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

#define __VSF_TGUI_CONTROLS_PANEL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_panel.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_panel_v_init(vsf_tgui_panel_t* panel_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) panel init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)panel_ptr), panel_ptr);
#endif

    if (fsm_rt_cpl == vsf_tgui_container_v_init(&(panel_ptr->use_as__vsf_tgui_container_t))) {
#if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
        int16_t iWidth = panel_ptr->iWidth - panel_ptr->tContainerPadding.chLeft - panel_ptr->tContainerPadding.chRight;
    #else
        int16_t iWidth = panel_ptr->iWidth;
    #endif

        tgui_set_priv_label(panel_ptr, tTitleLabel,
            tgui_attribute(tLabel, panel_ptr->tTitle),
            tgui_region(
                tgui_location(0, 0),
                tgui_size(iWidth, 32),
            )
        );

        vk_tgui_label_init(&(panel_ptr->tTitleLabel));
#endif

        return fsm_rt_cpl;
    }
    return fsm_rt_on_going;
}

fsm_rt_t vsf_tgui_panel_v_rendering(vsf_tgui_panel_t* panel_ptr,
                                    vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t mode)
{
    //vsf_tgui_label_t* ptLabel;
    //vsf_tgui_region_t tLableDirtyRegion;

    VSF_TGUI_ASSERT(panel_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) panel rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)panel_ptr), panel_ptr);
#endif

    vsf_tgui_container_v_rendering(&(panel_ptr->use_as__vsf_tgui_container_t), dirty_region_ptr, mode);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_panel_v_post_rendering(vsf_tgui_panel_t* panel_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,
                                        vsf_tgui_control_refresh_mode_t mode)
{
    return vsf_tgui_container_v_post_rendering((vsf_tgui_container_t *)panel_ptr, dirty_region_ptr, mode);
}

fsm_rt_t vsf_tgui_panel_v_depose(vsf_tgui_panel_t* panel_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_panel_v_update(vsf_tgui_panel_t* panel_ptr)
{
#if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
    if (!panel_ptr->tTitle.bIsAutoSize) {
    #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
        panel_ptr->tTitleLabel.tRegion.tSize.iWidth = panel_ptr->iWidth - panel_ptr->tContainerPadding.chLeft - panel_ptr->tContainerPadding.chRight;
    #else
        panel_ptr->tTitleLabel.tRegion.tSize.iWidth = panel_ptr->iWidth;
    #endif
    }
#endif
    return fsm_rt_cpl;
}


#endif


/* EOF */
