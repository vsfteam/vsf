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
declare_class(vsf_tgui_t)
#include "./vsf_tgui_sv_style.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
/*============================ IMPLEMENTATION ================================*/

typedef uint8_t vsf_tgui_opacity_t;

struct vsf_tgui_sv_style_control_t {
    vsf_tgui_color_t tColor;
    vsf_tgui_opacity_t tOpacity;
};
typedef struct vsf_tgui_sv_style_control_t vsf_tgui_sv_style_control_t;

struct vsf_tgui_sv_style_text_t {
    vsf_tgui_color_t tColor;
    vsf_tgui_opacity_t tOpacity;
};
typedef struct vsf_tgui_sv_style_text_t vsf_tgui_sv_style_text_t;

struct vsf_tgui_sv_theme_t {
    vsf_tgui_sv_style_control_t tControl;
};
typedef struct vsf_tgui_sv_theme_t vsf_tgui_sv_theme_t;


#endif


/* EOF */
