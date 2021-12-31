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

 /*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_font.h"

/*============================ MACROS ========================================*/
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
#   define __TGUI_SV_FONT_NAME(__NAME)         .name_ptr = #__NAME,
#else
#   define __TGUI_SV_FONT_NAME(__NAME)
#endif

#undef __TGUI_SV_FONT_DEF
#define __TGUI_SV_FONT_DEF(__NAME, __HEIGHT, ...)                               \
    [__NAME] = {                                                                \
        __TGUI_SV_FONT_NAME(__NAME)                                             \
        .height = __HEIGHT,                                                     \
        __VA_ARGS__                                                             \
    }

#undef TGUI_SV_FONT_DEF
#define TGUI_SV_FONT_DEF(__NAME, __HEIGHT)                                      \
    __TGUI_SV_FONT_DEF(__NAME, __HEIGHT)

#define __TGUI_SV_FT2_FONT_INIT(__PATH, __SIZE)                                 \
    .font_path_ptr = __PATH,                                                    \
    .font_size = __SIZE,

#undef TGUI_SV_FT2_FONT_DEF
#define TGUI_SV_FT2_FONT_DEF(__NAME, __PATH, __SIZE)                            \
    __TGUI_SV_FONT_DEF(__NAME, 0, __TGUI_SV_FT2_FONT_INIT(__PATH, __SIZE))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#if VSF_TGUI_CFG_SV_FONTS == ENABLED
#if VSF_TGUI_CFG_SUPPORT_FONT_USE_FREETYPE != ENABLED
const
#endif
static vsf_tgui_font_t __tgui_fonts[] = {
    VSF_TGUI_SV_FONTS
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t font_index)
{
    VSF_TGUI_ASSERT(font_index < dimof(__tgui_fonts));
    return &__tgui_fonts[font_index];
}

uint8_t vsf_tgui_font_number(void)
{
    return dimof(__tgui_fonts);
}

#endif


#endif


/* EOF */
