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

#ifndef __VSF_TINY_GUI_V_CONTROL_H__
#define __VSF_TINY_GUI_V_CONTROL_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include "../../controls/vsf_tgui_controls.h"
#include "./vsf_tgui_sv_color.h"
#include "./vsf_tgui_sv_font.h"
#include "./vsf_tgui_sv_draw.h"
#include "./vsf_tgui_sv_style.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef enum vsf_tgui_sv_cornor_tile_mode_t {
    CORNOR_TILE_IN_TOP_LEFT      = 0,
    CORNOR_TILE_IN_TOP_RIGHT,
    CORNOR_TILE_IN_BOTTOM_LEFT,
    CORNOR_TILE_IN_BOTTOM_RIGHT,

    __CORNOR_TILE_NUM,
} vsf_tgui_sv_cornor_tile_mode_t;
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_control_t* control_ptr);

extern fsm_rt_t vsf_tgui_control_v_rendering(vsf_tgui_control_t* control_ptr,
                                            vsf_tgui_region_t* dirty_region_ptr,
                                            vsf_tgui_control_refresh_mode_t mode);

extern fsm_rt_t vsf_tgui_control_v_depose(vsf_tgui_control_t* control_ptr);

extern fsm_rt_t vsf_tgui_control_v_update(vsf_tgui_control_t* control_ptr);

extern const vsf_tgui_tile_t* vsf_tgui_control_v_get_corner_tile(vsf_tgui_control_t* control_ptr, vsf_tgui_sv_cornor_tile_mode_t mode);

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
extern const char* vsf_tgui_control_get_node_name(vsf_tgui_control_t* control_ptr);
#endif

#endif

#endif
/* EOF */
