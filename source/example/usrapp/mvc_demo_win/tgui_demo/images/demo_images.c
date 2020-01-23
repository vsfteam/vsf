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
const vsf_tgui_tile_idx_root_t bg1_RGB  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 0
};

const vsf_tgui_tile_idx_root_t bg2_RGB  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 1
};

const vsf_tgui_tile_idx_root_t bg3_RGB  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 2
};

const vsf_tgui_tile_idx_root_t ic_autorenew_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 0
};

const vsf_tgui_tile_idx_root_t ic_build_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 1
};

const vsf_tgui_tile_idx_root_t ic_done_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 2
};

const vsf_tgui_tile_idx_root_t ic_find_in_page_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 3
};

const vsf_tgui_tile_idx_root_t ic_help_outline_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 4
};

const vsf_tgui_tile_idx_root_t ic_https_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 5
};

const vsf_tgui_tile_idx_root_t ic_power_settings_new_18pt_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 6
};

const vsf_tgui_tile_idx_root_t ic_settings_applications_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 7
};

const vsf_tgui_tile_idx_root_t ic_settings_bluetooth_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 8
};

const vsf_tgui_tile_idx_root_t ic_settings_phone_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .tAttribute = {
            .u2RootTileType = 1,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
    },
    .chIndex = 9
};


vsf_tgui_size_t gIdxRootRGBTileSizes[] = {
    {.iWidth = 200, .iHeight = 200},
    {.iWidth = 1280, .iHeight = 718},
    {.iWidth = 32, .iHeight = 32},

};

vsf_tgui_size_t gIdxRootRGBATileSizes[] = {
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 24, .iHeight = 24},
    {.iWidth = 18, .iHeight = 18},
    {.iWidth = 24, .iHeight = 24},

};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



#endif


/* EOF */

