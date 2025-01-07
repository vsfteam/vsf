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

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_TGUI_CLASS_INHERIT
#include "../../vsf_tgui.h"

#if     VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED                    \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_EXAMPLE

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/**********************************************************************************/
/*! \brief begin a refresh loop
 *! \param gui_ptr the tgui object address
 *! \param planned_refresh_region_ptr the planned refresh region
 *! \retval NULL    No need to refresh (or rendering service is not ready)
 *! \retval !NULL   The actual refresh region
 *!
 *! \note When NULL is returned, current refresh iteration (i.e. a refresh activities
 *!       between vsf_tgui_v_refresh_loop_begin and vsf_tgui_v_refresh_loop_end )
 *!       will be ignored and vsf_tgui_v_refresh_loop_end is called immediately
 **********************************************************************************/

vsf_tgui_region_t* vsf_tgui_v_refresh_loop_begin(vsf_tgui_t *gui_ptr, const vsf_tgui_region_t *planned_refresh_region_ptr)
{
    return NULL;
}

bool vsf_tgui_v_refresh_loop_end(vsf_tgui_t *gui_ptr)
{
    return false;
}

void vsf_tgui_v_bind_disp(vsf_tgui_t *gui_ptr, vk_disp_t *disp, void *pfb, size_t pfb_size)
{
}

#endif
