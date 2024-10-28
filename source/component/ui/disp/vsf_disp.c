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

#if VSF_USE_UI == ENABLED

#define __VSF_DISP_CLASS_IMPLEMENT
#define __VSF_EDA_CLASS_INHERIT__
#include "./vsf_disp.h"

/*============================ MACROS ========================================*/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED && VSF_SYNC_CFG_SUPPORT_ISR != ENABLED
#   error VSF_SYNC_CFG_SUPPORT_ISR is needed for reentrant_disp
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_KERNEL == ENABLED
enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static vsf_err_t __vk_reentrant_disp_init(vk_disp_t *pthis);
static vsf_err_t __vk_reentrant_disp_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
const vk_disp_drv_t vk_reentrant_disp_drv = {
    .init                   = __vk_reentrant_disp_init,
    .refresh                = __vk_reentrant_disp_refresh,
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
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

    if (drv->init != NULL) {
        return drv->init(pthis);
    } else {
        vk_disp_on_ready(pthis);
    }
    return VSF_ERR_NONE;
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

/*******************************************************************************
* reentrant disp                                                               *
*******************************************************************************/
#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static void __vk_reentrant_disp_on_ready(vk_disp_t *disp)
{
    vk_reentrant_disp_t *pthis = (vk_reentrant_disp_t *)disp->ui_data;
    vk_disp_on_ready(&pthis->use_as__vk_disp_t);
    if (pthis->mutex != NULL) {
        // use vsf_eda_mutex_leave_isr in case __vk_reentrant_disp_on_ready is called in isr
        vsf_eda_mutex_leave_isr(pthis->mutex);
    }
}

static void __vk_reentrant_disp_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_reentrant_disp_t *pthis = vsf_container_of(eda, vk_reentrant_disp_t, eda);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->disp->ui_on_ready = __vk_reentrant_disp_on_ready;
        pthis->disp->ui_data = pthis;
        vk_disp_on_ready(&pthis->use_as__vk_disp_t);
        break;
    case VSF_EVT_REFRESH:
        if (pthis->mutex != NULL) {
            err = vsf_eda_mutex_enter(pthis->mutex);
            if (err < 0) {
                VSF_UI_ASSERT(false);
            } else if (err != VSF_ERR_NONE) {
                break;
            }
        }
        // fall througn
    case VSF_EVT_SYNC:
        vk_disp_refresh(pthis->disp, &pthis->area, pthis->buffer);
        break;
    }
}

static vsf_err_t __vk_reentrant_disp_init(vk_disp_t *disp)
{
    vk_reentrant_disp_t *pthis = (vk_reentrant_disp_t *)disp;
    disp = pthis->disp;

    if (    ((pthis->pos.x + pthis->param.width) > disp->param.width)
        ||  ((pthis->pos.y + pthis->param.height) > disp->param.height)) {
        return VSF_ERR_FAIL;
    }

    pthis->eda.fn.evthandler = __vk_reentrant_disp_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    pthis->eda.on_terminate = NULL;
#endif
    return vsf_eda_init(&pthis->eda);
}

static vsf_err_t __vk_reentrant_disp_refresh(vk_disp_t *disp, vk_disp_area_t *area, void *disp_buff)
{
    vk_reentrant_disp_t *pthis = (vk_reentrant_disp_t *)disp;

    pthis->area = *area;
    pthis->area.pos.x += pthis->pos.x;
    pthis->area.pos.y += pthis->pos.y;
    pthis->buffer = disp_buff;
    return vsf_eda_post_evt(&pthis->eda, VSF_EVT_REFRESH);
}
#endif
#endif

/* EOF */
