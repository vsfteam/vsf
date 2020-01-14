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
#include "vsf.h"

#ifndef __TGUI_CUSTOM_H__
#define __TGUI_CUSTOM_H__

#if VSF_USE_TINY_GUI == ENABLED
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum {
    VSF_TGUI_FONT_DEFAULT = 0,

    VSF_TGUI_FONT_WQY_MICROHEI_S24 = 0,
    VSF_TGUI_FONT_WQY_MICROHEI_S20,
    VSF_TGUI_FONT_WQY_MICROHEI_S16,

    VSF_TGUI_FONT_DEJAVUSERIF_S24, 
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
extern const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t chFontIndex);

extern uint8_t vsf_tgui_font_number(void);
#endif
#endif


/* EOF */
