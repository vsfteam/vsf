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

#ifndef __VSF_TINY_GUI_V_H__
#define __VSF_TINY_GUI_V_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
#include "../controls/vsf_tgui_controls.h"

#if VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#   define VSF_TGUI_V_TEMPLATE_HEADER_FILE      "./simple_view/vsf_tgui_v_template.h"
#else 
#   undef VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL
#   define VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL  VSF_TGUI_V_TEMPLATE_EXAMPLE
#   define VSF_TGUI_V_TEMPLATE_HEADER_FILE      "./template/vsf_tgui_v_template.h"
#endif

#include VSF_TGUI_V_TEMPLATE_HEADER_FILE

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/*
extern
void vsf_tgui_location_get_in_sizes_with_align( vsf_tgui_location_t* ptLocation,
                                                vsf_tgui_size_t* ptCtrlSize,
                                                vsf_tgui_size_t* ptDrawSize,
                                                vsf_tgui_align_mode_t tMode);
*/

extern 
void vsf_tgui_region_update_with_align( vsf_tgui_region_t* ptDrawRegion,
                                        vsf_tgui_region_t* ptResourceRegion,
                                        vsf_tgui_align_mode_t tMode);




#endif

#endif
/* EOF */
