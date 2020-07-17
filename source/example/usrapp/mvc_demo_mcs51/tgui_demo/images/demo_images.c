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

#if VSF_USE_UI == ENABLED && VSF_USE_TINY_GUI == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
const vsf_tgui_tile_buf_root_t bg1_RGB  = {
    tgui_size(200, 200),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_RGB8_USER_TEMPLATE,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t bg2_RGB  = {
    tgui_size(1280, 718),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_RGB8_USER_TEMPLATE,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t bg3_RGB  = {
    tgui_size(32, 32),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_RGB8_USER_TEMPLATE,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_autorenew_black_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const 
vsf_tgui_tile_buf_root_t ic_build_black_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_done_black_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_find_in_page_white_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_help_outline_black_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_https_white_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_power_settings_new_18pt_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_settings_applications_RGBA  = {
    tgui_size(24, 24),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_settings_bluetooth_white_18dp_RGBA  = {
    tgui_size(18, 18),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};

const vsf_tgui_tile_buf_root_t ic_settings_phone_RGBA  = {
    tgui_size(24, 24),
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0,
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u3ColorSize = VSF_TGUI_COLOR_ARGB_8888,
            .bIsRootTile = 1,
        },
    },
};


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



#endif


/* EOF */

