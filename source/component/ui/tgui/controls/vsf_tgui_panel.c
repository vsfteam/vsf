/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
#include "osa_service/vsf_osa_service.h"

#define __VSF_TGUI_CONTROLS_PANEL_CLASS_IMPLEMENT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_panel.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
static fsm_rt_t __vsf_tgui_panel_v_rendering(
                                        vsf_tgui_panel_t* ptPanel,
                                        vsf_tgui_region_t* ptDirtyRegion,
                                        vsf_tgui_control_refresh_mode_t tMode);

/*============================ LOCAL VARIABLES ===============================*/

static const i_tgui_control_methods_t c_tVPanel= {

    .tView = {
        .Init =     (vsf_tgui_method_t*)&vsf_tgui_panel_v_init,
        .Depose =   (vsf_tgui_method_t*)&vsf_tgui_panel_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&__vsf_tgui_panel_v_rendering,
        .ContainerPostRender = (vsf_tgui_v_method_render_t *)&vsf_tgui_panel_v_post_rendering,
        .Update =   (vsf_tgui_method_t *)&vsf_tgui_panel_v_update,
    },
    .Init =     (vsf_tgui_method_t*)&vk_tgui_panel_init,
    .Update =   (vsf_tgui_method_t*)&vk_tgui_panel_update,

};

/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_panel_msg_handler( vsf_tgui_panel_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t *)control_ptr,
                                            ptMSG,
                                            &c_tVPanel);
}

fsm_rt_t vk_tgui_panel_update(vsf_tgui_panel_t* ptPanel)
{
    if (fsm_rt_cpl == vk_tgui_container_update(
                &(ptPanel->
                    use_as__vsf_tgui_root_container_t.
                        use_as__vsf_tgui_container_t))) {

    #if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
        ptPanel->tTitleLabel.tLabel = ptPanel->tTitle;

        vk_tgui_label_update(&(ptPanel->tTitleLabel));
    #endif

        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

fsm_rt_t vk_tgui_panel_init(vsf_tgui_panel_t* ptPanel)
{
    if (fsm_rt_cpl == vk_tgui_container_init(
            &(ptPanel->use_as__vsf_tgui_root_container_t.use_as__vsf_tgui_container_t))) {

    #if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
        vk_tgui_label_init(&(ptPanel->tTitleLabel));
    #endif

        do {
            vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)ptPanel);
            Status.Values.__does_contain_builtin_structure = true;

            vsf_tgui_control_status_set((vsf_tgui_control_t*)ptPanel, Status);
        } while(0);

        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

static fsm_rt_t __vsf_tgui_panel_v_rendering(vsf_tgui_panel_t* ptPanel,
                                            vsf_tgui_region_t* ptDirtyRegion,
                                            vsf_tgui_control_refresh_mode_t tMode)
{
    if (fsm_rt_cpl == vsf_tgui_panel_v_rendering(ptPanel, ptDirtyRegion, tMode)) {
    #if VSF_TGUI_CFG_PANEL_HAS_LABEL == ENABLED
        vsf_tgui_label_t* ptLabel = &(ptPanel->tTitleLabel);
        if (    (NULL != ptPanel->tTitle.tString.pstrText)
    #if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
            &&  (ptPanel->tTitle.tString.s16_size > 0)
    #endif
            ) {

            vsf_tgui_region_t tRegion;
            if (NULL != vsf_tgui_control_generate_dirty_region_from_parent_dirty_region(
                                                (const vsf_tgui_control_t *)ptPanel,
                                                ptDirtyRegion,
                                                (const vsf_tgui_control_t *)ptLabel,
                                                &tRegion)) {

                return vsf_tgui_label_v_rendering(  ptLabel,
                                                &tRegion,
                                                VSF_TGUI_CONTROL_REFRESHED_BY_PARENT);
            }

        }
    #endif
        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

#endif


/* EOF */
