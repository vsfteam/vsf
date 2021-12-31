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
/*============================ LOCAL VARIABLES ===============================*/

static const vsf_tgui_tile_t __controls_container_corner_tiles[__CORNOR_TILE_NUM] = {
    [CORNOR_TILE_IN_TOP_LEFT] = {
        .tChild = {
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    [CORNOR_TILE_IN_TOP_RIGHT] = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 200 - 12, .iY = 0},
        },
    },
    [CORNOR_TILE_IN_BOTTOM_LEFT] = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 0, .iY = 200 - 12},
        },
    },
    [CORNOR_TILE_IN_BOTTOM_RIGHT] = {
        .tChild = {
            .tSize = {.iWidth = 12, .iHeight = 12, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg1_RGB,
            .tLocation = {.iX = 200 - 12, .iY = 200 - 12},
        },
    },
};

static const vsf_tgui_tile_t __controls_label_corner_tiles[2] = {
    [CORNOR_TILE_IN_TOP_LEFT] = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.iX = 0, .iY = 0},
        },
    },
    [CORNOR_TILE_IN_TOP_RIGHT] = {
        .tChild = {
            .tSize = {.iWidth = 16, .iHeight = 32, },
            .parent_ptr = (vsf_tgui_tile_core_t*)&bg3_RGB,
            .tLocation = {.iX = 16, .iY = 0},
        },
    },
};



/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
#ifdef WEAK_VSF_TGUI_SV_GET_CORNOR_TILE
const vsf_tgui_tile_t* vsf_tgui_control_v_get_corner_tile(vsf_tgui_control_t* control_ptr, vsf_tgui_sv_cornor_tile_mode_t mode)
{
    if (control_ptr->id == VSF_TGUI_COMPONENT_ID_BUTTON || control_ptr->id == VSF_TGUI_COMPONENT_ID_LABEL) {
        if (mode < dimof(__controls_label_corner_tiles)) {
            return &__controls_label_corner_tiles[mode];
        }
    } else /*if (control_ptr->id == VSF_TGUI_COMPONENT_ID_CONTAINER || control_ptr->id == VSF_TGUI_COMPONENT_ID_PANEL)*/ {
        if (mode < dimof(__controls_container_corner_tiles)) {
            return &__controls_container_corner_tiles[mode];
        }
    }

    return NULL;
}
#endif

#endif

/* EOF */
