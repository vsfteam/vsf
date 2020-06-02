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

#ifndef __VSF_TINY_GUI_V_LABEL_H__
#define __VSF_TINY_GUI_V_LABEL_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include "./vsf_tgui_sv_control.h"
#include "../../utilities/vsf_tgui_text.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef union {
    vsf_tgui_tile_t tTiles[2];
    implement_ex(
        struct {
            vsf_tgui_tile_t tLeft;
            vsf_tgui_tile_t tRight;
        },
        _
    )
} vsf_tgui_sv_label_tiles_t;

/*============================ GLOBAL VARIABLES ==============================*/
#if VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES == ENABLED
extern const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles;
#endif

/*============================ PROTOTYPES ====================================*/

extern fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_label_t* ptLabel);

extern fsm_rt_t vsf_tgui_label_v_rendering( vsf_tgui_label_t* ptLabel,
                                            vsf_tgui_region_t* ptDirtyRegion,
                                            vsf_tgui_control_refresh_mode_t tMode);

extern fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_label_t* ptLabel);

extern fsm_rt_t vsf_tgui_label_v_update(vsf_tgui_label_t* ptLabel);

extern
void __vsf_tgui_label_v_rendering(  vsf_tgui_label_t* ptLabel,
                                    vsf_tgui_region_t* ptDirtyRegion,
                                    vsf_tgui_control_refresh_mode_t tMode);

extern
vsf_tgui_size_t __vk_tgui_label_v_text_get_size(vsf_tgui_label_t* ptLabel,
                                                uint16_t *phwLineCount);

extern
vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* ptLabel);

extern
int_fast16_t __vk_tgui_label_get_line_height( const vsf_tgui_label_t* ptLabel);

#endif

#endif
/* EOF */
