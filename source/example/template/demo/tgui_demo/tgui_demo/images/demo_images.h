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

#ifndef __VSF_TINY_GUI_IMAGE_DATA_H__
#define __VSF_TINY_GUI_IMAGE_DATA_H__

/*============================ INCLUDES ======================================*/
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
#define VSF_TGUI_COLORTYPE_RGB         0
#define VSF_TGUI_COLORTYPE_RGBA        1
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
extern const vsf_tgui_tile_t bg1_RGB; //bg1:RGB, width: 200, height: 200
extern const vsf_tgui_tile_t bg2_RGB; //bg2:RGB, width: 1280, height: 718
extern const vsf_tgui_tile_t bg3_RGB; //bg3:RGB, width: 32, height: 32
extern const vsf_tgui_tile_t ic_autorenew_black_18dp_RGBA; //ic_autorenew_black_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_build_black_18dp_RGBA; //ic_build_black_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_done_black_18dp_RGBA; //ic_done_black_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_find_in_page_white_18dp_RGBA; //ic_find_in_page_white_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_help_outline_black_18dp_RGBA; //ic_help_outline_black_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_https_white_18dp_RGBA; //ic_https_white_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_power_settings_new_18pt_RGBA; //ic_power_settings_new_18pt:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_settings_applications_RGBA; //ic_settings_applications:RGBA, width: 24, height: 24
extern const vsf_tgui_tile_t ic_settings_bluetooth_white_18dp_RGBA; //ic_settings_bluetooth_white_18dp:RGBA, width: 18, height: 18
extern const vsf_tgui_tile_t ic_settings_phone_RGBA; //ic_settings_phone:RGBA, width: 24, height: 24

extern vsf_tgui_size_t gIdxRootRGBTileSizes[3];

extern vsf_tgui_size_t gIdxRootRGBATileSizes[10];

/*============================ PROTOTYPES ====================================*/


#endif

#endif
/* EOF */

