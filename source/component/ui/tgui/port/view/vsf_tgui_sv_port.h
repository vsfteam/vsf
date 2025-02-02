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

#ifndef __VSF_TINY_GUI_SV_PORT_H__
#define __VSF_TINY_GUI_SV_PORT_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#define __VSF_DISP_CLASS_INHERIT__
#include "component/ui/disp/vsf_disp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TGUI_CFG_SV_REFRESH_RATE
#   define VSF_TGUI_CFG_SV_REFRESH_RATE                 ENABLED
#endif

#ifndef VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY
#   define VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY             DISABLED
#endif

#ifndef VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER
#   define VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER           ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_tgui_vport_init(__vsf_tgui_v_port_ptr)                              \
    do {                                                                        \
        (__vsf_tgui_v_port_ptr)->current_region.tSize.iHeight = 0;              \
        (__vsf_tgui_v_port_ptr)->current_region.tSize.iWidth = 0;               \
    } while(0)

/*============================ TYPES =========================================*/

declare_class(vsf_tgui_t)
declare_structure(vsf_tgui_v_port_t)
def_structure(vsf_tgui_v_port_t)
    vk_disp_t *disp;
    void *pfb;
    bool is_disp_inited;
#if VSF_TGUI_CFG_SV_DRAW_DOUBLE_BUFFER == ENABLED
    bool refresh_is_first_pfb;
    bool refresh_pending;
#endif
    bool refresh_pending_notify;
    uint8_t refresh_pending_cnt;
    size_t pfb_size;
    vk_disp_area_t refresh_pending_area;

#if VSF_TGUI_CFG_SV_DRAW_IMMEDIATELY == ENABLED
    volatile bool is_draw_ready;
#endif
    vsf_tgui_region_t request_region;
    vsf_tgui_region_t current_region;
#if VSF_TGUI_CFG_SV_REFRESH_RATE == ENABLED
    vsf_systimer_tick_t start_cnt;
    uint16_t refresh_cnt;
    uint16_t fps;
#endif
end_def_structure(vsf_tgui_v_port_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
