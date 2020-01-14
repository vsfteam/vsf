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
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tTopRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 200 - 12, .nY = 0},
        },
    },
    .tBottomLeft = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 0, .nY = 200 - 12},
        },
    },
    .tBottomRight = {
        .tChild = {
            .tSize = {.nWidth = 12, .nHeight = 12, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.nX = 200 - 12, .nY = 200 - 12},
        },
    },
};

const vsf_tgui_sv_label_tiles_t c_tLabelAdditionalTiles = {
    .tLeft = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.nX = 0, .nY = 0},
        },
    },
    .tRight = {
        .tChild = {
            .tSize = {.nWidth = 16, .nHeight = 32, },
            .ptParent = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.nX = 16, .nY = 0},
        },
    },
};

/*============================ LOCAL VARIABLES ===============================*/
static vsf_tgui_font_t g_tUserFonts[] = {
    [VSF_TGUI_FONT_WQY_MICROHEI_S24] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 24,
        .pchFontPath = "../../usrapp/mvc_demo_win/tgui_demo/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_WQY_MICROHEI_S20] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 20,
        .pchFontPath = "../../usrapp/mvc_demo_win/tgui_demo/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_WQY_MICROHEI_S16] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 16,
        .pchFontPath = "../../usrapp/mvc_demo_win/tgui_demo/wqy-microhei.ttc",
    },

    [VSF_TGUI_FONT_DEJAVUSERIF_S24] = {
        .chFlags = VSF_TGUI_FONT_PROPORTIONAL,
        .chFontSize = 24,
        .pchFontPath = "../../usrapp/mvc_demo_win/tgui_demo/DejaVuSerif.ttf",
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
