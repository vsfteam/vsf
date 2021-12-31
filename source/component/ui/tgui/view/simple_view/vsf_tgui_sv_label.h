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
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_label_t* label_ptr);

extern fsm_rt_t vsf_tgui_label_v_rendering( vsf_tgui_label_t* label_ptr,
                                            vsf_tgui_region_t* dirty_region_ptr,
                                            vsf_tgui_control_refresh_mode_t mode);

extern fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_label_t* label_ptr);

extern fsm_rt_t vsf_tgui_label_v_update(vsf_tgui_label_t* label_ptr);

extern
vsf_tgui_size_t __vk_tgui_label_v_text_get_size(vsf_tgui_label_t* label_ptr,
                                                uint16_t *line_count_ptr,
                                                uint8_t *char_height_ptr);

extern
vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* label_ptr);

extern
int_fast16_t __vk_tgui_label_get_line_height( const vsf_tgui_label_t* label_ptr);

#endif

#endif
/* EOF */
