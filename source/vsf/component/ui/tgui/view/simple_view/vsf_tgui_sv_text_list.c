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

#define __VSF_TGUI_CONTROLS_TEXT_LIST_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_text_list.h"
#include "./vsf_tgui_sv_container.h"
#include "./vsf_tgui_sv_label.h"
#include "./vsf_tgui_sv_draw.h"
#include "./vsf_tgui_sv_style.h"

#include "../../utilities/vsf_tgui_color.h"
#include "vsf_tgui_sv_port.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_text_list_v_init(vsf_tgui_text_list_t* ptTextList)
{
#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]text list init" VSF_TRACE_CFG_LINEEND);
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

#if VSF_TGUI_SV_CFG_RENDERING_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]text list rendering" VSF_TRACE_CFG_LINEEND);
#endif

    __vk_tgui_container_v_rendering(&(ptTextList->use_as__vsf_tgui_container_t), 
                                    ptDirtyRegion, 
                                    tMode, 
                                    (vsf_tgui_color_t)VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR);

    //! draw indicator
    do {
        vsf_tgui_control_t* ptControl = (vsf_tgui_control_t*)ptTextList;
        vsf_tgui_color_t tColor = VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR;
        vsf_tgui_region_t tRegion = {0};

        tRegion.tSize = vsf_tgui_control_get_size(ptControl);

        tRegion.tLocation.iY = tRegion.tSize.iHeight / 2 - 1;
        tRegion.tLocation.iX = 4;
        tRegion.tSize.iHeight = 2;
        tRegion.tSize.iWidth -= 8;

        vsf_tgui_control_v_draw_rect(   ptControl,
                                        ptDirtyRegion,
                                        &tRegion,
                                        tColor);
    } while(0);

    return fsm_rt_cpl;
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
