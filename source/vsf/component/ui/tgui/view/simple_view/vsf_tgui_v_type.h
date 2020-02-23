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

#ifndef __VSF_TINY_GUI_V_TMPLATE_TYPE_H__
#define __VSF_TINY_GUI_V_TMPLATE_TYPE_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
#include "../../utilities/vsf_tgui_font.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_structure(vsf_tgui_v_control_t)

def_structure(vsf_tgui_v_control_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    RESERVED_U8
#endif
end_def_structure(vsf_tgui_v_control_t)

declare_structure(vsf_tgui_v_container_t)

def_structure(vsf_tgui_v_container_t)
    uint8_t bIsShowCornerTile   : 1;
    uint8_t bIsNoBackgroundColor: 1;
end_def_structure(vsf_tgui_v_container_t)

declare_structure(vsf_tgui_v_label_t)
def_structure(vsf_tgui_v_label_t)
    uint8_t bIsUseRawView       : 1;
    uint8_t bIsNoBackgroundColor: 1;
    uint8_t chFontIndex;
    vsf_tgui_color_t tFontColor;
end_def_structure(vsf_tgui_v_label_t)

declare_structure(vsf_tgui_v_button_t)
def_structure(vsf_tgui_v_button_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    RESERVED_U8
#endif
end_def_structure(vsf_tgui_v_button_t)

declare_structure(vsf_tgui_v_panel_t)
def_structure(vsf_tgui_v_panel_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    RESERVED_U8
#endif
end_def_structure(vsf_tgui_v_panel_t)

declare_structure(vsf_tgui_v_list_t)
def_structure(vsf_tgui_v_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    RESERVED_U8
#endif
end_def_structure(vsf_tgui_v_list_t)

declare_structure(vsf_tgui_v_text_list_t)
def_structure(vsf_tgui_v_text_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    RESERVED_U8
#endif
end_def_structure(vsf_tgui_v_text_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif

#endif
/* EOF */
