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

#ifndef __VSF_TINY_GUI_COLOR_H__
#define __VSF_TINY_GUI_COLOR_H__

/*============================ INCLUDES ======================================*/
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
#ifdef VSF_TGUI_COLOR_ARGB_8888
#   if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#       define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	((vsf_tgui_color_t){.tChannel = {.chB = _B, .chG =_G, .chR = _R, .chA = _A}})
#   else
#       define VSF_TGUI_COLOR_RGBA(_R, _G, _B, _A)	{_B, _G, _R, _A}
#   endif

#else
#	error "TODO: add more color support"
#endif

#define VSF_TGUI_COLOR_RGB(_R, _G, _B)	    VSF_TGUI_COLOR_RGBA(_R, _G, _B, 0xFF)
#define VSF_TGUI_COLOR_DEF(_R, _G, _B)	    VSF_TGUI_COLOR_RGB(_R, _G, _B)

#define VSF_TGUI_COLOR_WHITE	VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0xFF)    //  Red:100%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_SILVER	VSF_TGUI_COLOR_RGB(0xC0, 0xC0, 0xC0)    //  Red: 75%  Green: 75%  Blue: 75%
#define VSF_TGUI_COLOR_GRAY		VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x80)    //  Red: 50%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLACK	VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x00)    //  Red:  0%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_RED      VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0x00)    //  Red:100%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_MAROON	VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x00)    //  Red: 50%  Green: 0%   Blue:  0%
#define VSF_TGUI_COLOR_YELLOW	VSF_TGUI_COLOR_RGB(0xFF, 0xFF, 0x00)    //  Red:100%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_OLIVE	VSF_TGUI_COLOR_RGB(0x80, 0x80, 0x00)    //  Red: 50%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_LIME		VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0x00)    //  Red:  0%  Green:100%  Blue:  0%
#define VSF_TGUI_COLOR_GREEN	VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x00)    //  Red:  0%  Green: 50%  Blue:  0%
#define VSF_TGUI_COLOR_AQUA		VSF_TGUI_COLOR_RGB(0x00, 0xFF, 0xFF)    //  Red:  0%  Green:100%  Blue:100%
#define VSF_TGUI_COLOR_TEAL		VSF_TGUI_COLOR_RGB(0x00, 0x80, 0x80)    //  Red:  0%  Green: 50%  Blue: 50%
#define VSF_TGUI_COLOR_BLUE		VSF_TGUI_COLOR_RGB(0x00, 0x00, 0xFF)    //  Red:  0%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_NAVY		VSF_TGUI_COLOR_RGB(0x00, 0x00, 0x80)    //  Red:  0%  Green: 0%   Blue: 50%
#define VSF_TGUI_COLOR_FUCHSIA	VSF_TGUI_COLOR_RGB(0xFF, 0x00, 0xFF)    //  Red:100%  Green: 0%   Blue:100%
#define VSF_TGUI_COLOR_PURPLE	VSF_TGUI_COLOR_RGB(0x80, 0x00, 0x80)    //  Red: 50%  Green: 0%   Blue: 50%

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
extern vsf_tgui_color_t vsf_tgui_color_mix(vsf_tgui_color_t c0, vsf_tgui_color_t c1, uint8_t mix);


#endif

#endif
/* EOF */
