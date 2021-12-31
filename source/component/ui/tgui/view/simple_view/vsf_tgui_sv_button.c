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

#define __VSF_TGUI_CONTROLS_BUTTON_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_button.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_button_v_init(vsf_tgui_button_t* button_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) button init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)button_ptr), button_ptr);
#endif

    return vsf_tgui_label_v_init(&(button_ptr->use_as__vsf_tgui_label_t));
}

fsm_rt_t vsf_tgui_button_v_rendering(   vsf_tgui_button_t* button_ptr,
                                        vsf_tgui_region_t* dirty_region_ptr,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t mode)
{

    VSF_TGUI_ASSERT(button_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);


    vsf_tgui_label_v_rendering((vsf_tgui_label_t *)button_ptr, dirty_region_ptr, mode);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_button_v_depose(vsf_tgui_button_t* button_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_button_v_update(vsf_tgui_button_t* button_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
