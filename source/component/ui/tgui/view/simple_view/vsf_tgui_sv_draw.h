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

#ifndef __VSF_TINY_GUI_V_DRAW_H__
#define __VSF_TINY_GUI_V_DRAW_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include "../../controls/vsf_tgui_controls.h"
#include "./vsf_tgui_sv_control.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_tgui_control_v_draw_rect(const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         const vsf_tgui_region_t* ptRectRegion,
                                         const vsf_tgui_sv_color_t color);

extern void vsf_tgui_control_v_draw_tile(const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         const vsf_tgui_tile_t* tile_ptr,
                                         const vsf_tgui_align_mode_t mode,
                                         const uint8_t trans_rate,
                                         vsf_tgui_region_t* ptPlacedRegion);

extern void vsf_tgui_control_v_draw_text(const vsf_tgui_control_t* control_ptr,
                                         const vsf_tgui_region_t* dirty_region_ptr,
                                         vsf_tgui_text_info_t *ptStringInfo,
                                         const uint8_t font_index,
                                         const vsf_tgui_sv_color_t color,
                                         const vsf_tgui_align_mode_t mode);

#endif

#endif
/* EOF */
