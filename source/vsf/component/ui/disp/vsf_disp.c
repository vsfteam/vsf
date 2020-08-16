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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED

#define __VSF_DISP_CLASS_IMPLEMENT

#include "./vsf_disp.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vk_disp_on_ready(vk_disp_t *pthis)
{
    if (pthis->ui_on_ready != NULL) {
        pthis->ui_on_ready(pthis);
    }
}

vsf_err_t vk_disp_init(vk_disp_t *pthis)
{
    VSF_UI_ASSERT(pthis != NULL);
    const vk_disp_drv_t *drv = pthis->param.drv;
    VSF_UI_ASSERT(drv != NULL);

    return drv->init(pthis);
}

vsf_err_t vk_disp_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    VSF_UI_ASSERT(pthis != NULL);
    const vk_disp_drv_t *drv = pthis->param.drv;
    VSF_UI_ASSERT(drv != NULL);

    vk_disp_area_t area_tmp;
    if (NULL == area) {
        area = &area_tmp;
        area_tmp.pos.x = 0;
        area_tmp.pos.y = 0;
        area_tmp.size.x = pthis->param.width;
        area_tmp.size.y = pthis->param.height;
    }
    return drv->refresh(pthis, area, disp_buff);
}

#endif

/* EOF */
