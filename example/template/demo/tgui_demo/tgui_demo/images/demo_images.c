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
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "bg1_RGB",
#endif
    },
    .tSize = {
        .iWidth = 200,
        .iHeight = 200,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x00,
};

const vsf_tgui_tile_buf_root_t bg2_RGB  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "bg2_RGB",
#endif
    },
    .tSize = {
        .iWidth = 1280,
        .iHeight = 718,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x1D4C0,
};

const vsf_tgui_tile_buf_root_t bg3_RGB  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 0,
            .u3ColorSize = 6,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "bg3_RGB",
#endif
    },
    .tSize = {
        .iWidth = 32,
        .iHeight = 32,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2BE6C0,
};

const vsf_tgui_tile_buf_root_t ic_autorenew_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_autorenew_black_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2BF2C0,
};

const vsf_tgui_tile_buf_root_t ic_build_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_build_black_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2BF7D0,
};

const vsf_tgui_tile_buf_root_t ic_done_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_done_black_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2BFCE0,
};

const vsf_tgui_tile_buf_root_t ic_find_in_page_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_find_in_page_white_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C01F0,
};

const vsf_tgui_tile_buf_root_t ic_help_outline_black_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_help_outline_black_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C0700,
};

const vsf_tgui_tile_buf_root_t ic_https_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_https_white_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C0C10,
};

const vsf_tgui_tile_buf_root_t ic_power_settings_new_18pt_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_power_settings_new_18pt_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C1120,
};

const vsf_tgui_tile_buf_root_t ic_settings_applications_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_settings_applications_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 24,
        .iHeight = 24,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C1630,
};

const vsf_tgui_tile_buf_root_t ic_settings_bluetooth_white_18dp_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_settings_bluetooth_white_18dp_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 18,
        .iHeight = 18,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C1F30,
};

const vsf_tgui_tile_buf_root_t ic_settings_phone_RGBA  = {
    .use_as__vsf_tgui_tile_core_t = {
        .Attribute = {
            .u2RootTileType = 0,
            .u2ColorType = 1,
            .u3ColorSize = 5,
            .bIsRootTile = 1,
        },
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
        .name_ptr = "ic_settings_phone_RGBA",
#endif
    },
    .tSize = {
        .iWidth = 24,
        .iHeight = 24,
    },
    .ptBitmap = (vsf_tgui_disp_ram_uintptr_t)0x2C2440,
};

const vsf_tgui_tile_t* tiles_list[13] = {
    (const vsf_tgui_tile_t *)&bg1_RGB, //bg1:RGB, width: 200, height: 200
    (const vsf_tgui_tile_t *)&bg2_RGB, //bg2:RGB, width: 1280, height: 718
    (const vsf_tgui_tile_t *)&bg3_RGB, //bg3:RGB, width: 32, height: 32
    (const vsf_tgui_tile_t *)&ic_autorenew_black_18dp_RGBA, //ic_autorenew_black_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_build_black_18dp_RGBA, //ic_build_black_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_done_black_18dp_RGBA, //ic_done_black_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_find_in_page_white_18dp_RGBA, //ic_find_in_page_white_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_help_outline_black_18dp_RGBA, //ic_help_outline_black_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_https_white_18dp_RGBA, //ic_https_white_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_power_settings_new_18pt_RGBA, //ic_power_settings_new_18pt:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_settings_applications_RGBA, //ic_settings_applications:RGBA, width: 24, height: 24
    (const vsf_tgui_tile_t *)&ic_settings_bluetooth_white_18dp_RGBA, //ic_settings_bluetooth_white_18dp:RGBA, width: 18, height: 18
    (const vsf_tgui_tile_t *)&ic_settings_phone_RGBA, //ic_settings_phone:RGBA, width: 24, height: 24
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/



#endif


/* EOF */

