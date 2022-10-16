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

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_DID == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_did_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_did_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_did = {
    .init           = __vk_disp_did_init,
    .refresh        = __vk_disp_did_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vk_disp_did_init(vk_disp_t *disp)
{
    vk_disp_did_t *pthis = (vk_disp_did_t *)disp;
    disp = pthis->disp;

    if (    ((pthis->pos.x + pthis->param.width) > disp->param.width)
        ||  ((pthis->pos.y + pthis->param.height) > disp->param.height)) {
        return VSF_ERR_FAIL;
    }
    vk_disp_on_ready(disp);
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_did_refresh(vk_disp_t *disp, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_did_t *pthis = (vk_disp_did_t *)disp;
    area->pos.x += pthis->pos.x;
    area->pos.y += pthis->pos.y;
    return vk_disp_refresh(disp, area, disp_buff);
}

#endif

/* EOF */
