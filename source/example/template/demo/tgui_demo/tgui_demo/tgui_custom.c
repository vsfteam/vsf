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

#define __VSF_DISP_CLASS_INHERIT
#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED
#include "./tgui_custom.h"
#include "./images/demo_images.h"
#include "./images/demo_images_data.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_tgui_sv_container_corner_tiles_t g_tContainerCornerTiles = {
    .tTopLeft = {
        .tChild = {
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    .tTopRight = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 200 - 12, .iY = 0},
        },
    },
    .tBottomLeft = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 0, .iY = 200 - 12},
        },
    },
    .tBottomRight = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 200 - 12, .iY = 200 - 12},
        },
    },
};

const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles = {
    .tLeft = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    .tRight = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.iX = 16, .iY = 0},
        },
    },
};

/*============================ LOCAL VARIABLES ===============================*/
static vsf_tgui_font_t g_tUserFonts[] = {
    [VSF_TGUI_FONT_WQY_MICROHEI_S24] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 24,
        .pchFontPath = "font/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_WQY_MICROHEI_S20] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 20,
        .pchFontPath = "font/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_WQY_MICROHEI_S16] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 16,
        .pchFontPath = "font/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_DEJAVUSERIF_S24] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 24,
        .pchFontPath = "font/DejaVuSerif.ttf",
    },

};


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
/*********************************************************************************/

const vsf_tgui_font_t* vsf_tgui_font_get(uint8_t chFontIndex)
{
    VSF_TGUI_ASSERT(chFontIndex < dimof(g_tUserFonts));
    return &g_tUserFonts[chFontIndex];
}

uint8_t vsf_tgui_font_number(void)
{
    return dimof(g_tUserFonts);
}
#endif

/* EOF */
