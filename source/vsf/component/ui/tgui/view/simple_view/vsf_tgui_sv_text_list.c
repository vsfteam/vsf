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
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW  \
    &&  VSF_TGUI_CFG_SUPPORT_TEXT_LIST == ENABLED

#define __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_text_list.h"

/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR   VSF_TGUI_COLOR_BLUE
#endif


/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_text_list_v_init(vsf_tgui_text_list_t* ptTextList)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptTextList), ptTextList);
#endif
    vsf_tgui_container_v_init(&(ptTextList->use_as__vsf_tgui_container_t));
    vsf_tgui_container_v_init(&(ptTextList->tList.use_as__vsf_tgui_container_t));
    return fsm_rt_on_going;
}

fsm_rt_t vsf_tgui_text_list_v_rendering( vsf_tgui_text_list_t* ptTextList,
                                    vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t tMode)
{
    //vsf_tgui_label_t* ptLabel;
    //vsf_tgui_region_t tLableDirtyRegion;

    VSF_TGUI_ASSERT(ptTextList != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptTextList), ptTextList);
#endif

    __vk_tgui_container_v_rendering(&(ptTextList->use_as__vsf_tgui_container_t),
                                    ptDirtyRegion,
                                    tMode,
                                    VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_text_list_v_post_rendering(   vsf_tgui_text_list_t* ptTextList,
                                                vsf_tgui_region_t* ptDirtyRegion,
                                                vsf_tgui_control_refresh_mode_t tMode)
{
    return vsf_tgui_container_v_post_rendering((vsf_tgui_container_t *)ptTextList, ptDirtyRegion, tMode);
}


fsm_rt_t vsf_tgui_text_list_v_depose(vsf_tgui_text_list_t* ptTextList)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_text_list_v_update(vsf_tgui_text_list_t* ptTextList)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
