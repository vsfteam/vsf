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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_ST7789 == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_ST7789_CLASS_IMPLEMENT

#include "kernel/vsf_kernel.h"
#include "../../vsf_disp.h"
#include "./vsf_disp_st7789.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_st7789_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_st7789_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_st7789 = {
    .init           = __vk_disp_st7789_init,
    .refresh        = __vk_disp_st7789_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vk_disp_st7789_init(vk_disp_t *pthis)
{
    vk_disp_st7789_t *disp_st7789 = (vk_disp_st7789_t *)pthis;
    VSF_UI_ASSERT(disp_st7789 != NULL);

    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_st7789_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_st7789_t *disp_st7789 = (vk_disp_st7789_t *)pthis;
    VSF_UI_ASSERT(disp_st7789 != NULL);

    return VSF_ERR_NONE;
}

#endif

/* EOF */
