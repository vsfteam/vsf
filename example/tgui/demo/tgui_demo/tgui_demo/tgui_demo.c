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

/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#if VSF_USE_TINY_GUI == ENABLED
#include <stdio.h>
#include "./images/demo_images.h"
#if APP_USE_TGUI_DESIGNER_DEMO == ENABLED
#   include "./tgui_designer/tgui_designer.h"
#endif
#include "./stopwatch/stopwatch.h"

#include "./images/demo_images.h"
#include "./images/demo_images_data.h"

/*============================ MACROS ========================================*/
#define DEMO_OFFSET            0

#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_TOP)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_BOTTOM)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_LEFT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_RIGHT)
//#define DEMO_BACKGROUND_ALIGN   (VSF_TGUI_ALIGN_CENTER)

#ifndef APP_TGUI_DEMO_CFG_QUEUE_MAX
#   if APP_USE_TGUI_DESIGNER_DEMO == ENABLED
#       define APP_TGUI_DEMO_CFG_QUEUE_MAX      320
#   else
#       define APP_TGUI_DEMO_CFG_QUEUE_MAX      32
#endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/

VSF_CAL_NO_INIT vsf_tgui_t g_tTGUIDemo;

#if APP_USE_TGUI_DESIGNER_DEMO == ENABLED
static VSF_CAL_NO_INIT tgui_designer_t s_tDesigner;
#endif

VSF_CAL_NO_INIT union {
    stopwatch_t stopwatch;
    popup_t popup;
} panels;

/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

unsigned char * vsf_tgui_tile_get_pixelmap(const vsf_tgui_tile_t *tile_ptr)
{
    VSF_TGUI_ASSERT(tile_ptr != NULL);

    if (tile_ptr->_.tCore.Attribute.u2RootTileType == 0) {  // buf tile
        return (unsigned char *)&__tiles_data[(uint32_t)tile_ptr->tBufRoot.ptBitmap];
    } else {                                                // index tile
        VSF_TGUI_ASSERT(0);
        return NULL;
    }
}

vsf_tgui_t * tgui_demo_init(void)
{
    VSF_CAL_NO_INIT static vsf_tgui_evt_t s_tEvtQueueBuffer[APP_TGUI_DEMO_CFG_QUEUE_MAX];

#if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
    VSF_CAL_NO_INIT static uint16_t s_tBFSBuffer[APP_TGUI_DEMO_CFG_QUEUE_MAX];
#endif

    static const vsf_tgui_cfg_t cfg = {
        .evt_queue = {
            .obj_ptr = s_tEvtQueueBuffer,
            .s32_size = sizeof(s_tEvtQueueBuffer)
        },
#if VSF_TGUI_CFG_REFRESH_SCHEME == VSF_TGUI_REFRESH_SCHEME_BREADTH_FIRST_TRAVERSAL
        .bfs_queue = {
            .obj_ptr = s_tBFSBuffer,
            .s32_size = sizeof(s_tBFSBuffer),
        },
#endif
    };

    if (vk_tgui_init(&g_tTGUIDemo, &cfg) != VSF_ERR_NONE) {
        return NULL;
    }

#if APP_USE_TGUI_DESIGNER_DEMO == ENABLED
    tgui_designer_init(&s_tDesigner, &g_tTGUIDemo);
    vk_tgui_set_root_container(&g_tTGUIDemo, (vsf_tgui_root_container_t *)&s_tDesigner, true);
#else
    //my_stopwatch_init(&g_tMyStopwatch, &g_tTGUIDemo);
    popup_init(&panels.popup);

    vk_tgui_set_root_container(&g_tTGUIDemo, (vsf_tgui_root_container_t *)&panels.popup, true);
#endif

    return &g_tTGUIDemo;
}

#endif


/* EOF */
