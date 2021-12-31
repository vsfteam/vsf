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

#define __VSF_TGUI_CONTROLS_CONTAINER_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_container.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_container_v_init(vsf_tgui_container_t* container_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) container init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t *)container_ptr), container_ptr);
#endif
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_container_v_rendering(vsf_tgui_container_t* container_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t mode)
{
    //fsm_rt_t result = fsm_rt_cpl;

    VSF_TGUI_ASSERT(container_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) container rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t *)container_ptr), container_ptr);
#endif

    return vsf_tgui_control_v_rendering((vsf_tgui_control_t *)container_ptr, dirty_region_ptr, mode);
}

fsm_rt_t vsf_tgui_container_v_post_rendering(vsf_tgui_container_t* container_ptr,
                                            vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                            vsf_tgui_control_refresh_mode_t mode)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_container_v_depose(vsf_tgui_container_t* container_ptr)
{
    return fsm_rt_cpl;
}


fsm_rt_t vsf_tgui_container_v_update(vsf_tgui_container_t* container_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
