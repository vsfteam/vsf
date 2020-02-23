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
#include "../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW
declare_class(vsf_tgui_t)
#include "./vsf_tgui_color.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsf_tgui_color_t vsf_tgui_color_mix(vsf_tgui_color_t c0, vsf_tgui_color_t c1, uint8_t mix)
{
    // todo: support other color
    vsf_tgui_color_t result;
    result.tChannel.chR = ((uint32_t)c0.tChannel.chR * mix + (uint32_t)c1.tChannel.chR * (255 - mix)) >> 8;
    result.tChannel.chG = ((uint32_t)c0.tChannel.chG * mix + (uint32_t)c1.tChannel.chG * (255 - mix)) >> 8;
    result.tChannel.chB = ((uint32_t)c0.tChannel.chB * mix + (uint32_t)c1.tChannel.chB * (255 - mix)) >> 8;
    result.tChannel.chA = 0xFF;

    return result;
}

#endif


/* EOF */
