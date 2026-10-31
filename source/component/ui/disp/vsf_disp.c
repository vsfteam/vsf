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
#   error VSF_SYNC_CFG_SUPPORT_ISR is needed for disp_reentrant
#endif

#define __VK_DISP_CVRT_IMPLEMENT(__COLOR_SRC, __COLOR_DST)                      \
    static void VSF_MCONNECT(vk_disp_color_cvrt_, __COLOR_SRC, _to_, __COLOR_DST)\
        (uint16_t width, uint16_t height, void *pixels_src, uint32_t pitch_src, void *pixels_dst, uint32_t pitch_dst)
#define __VK_DISP_CVRT_DECLARE(__COLOR_SRC, __COLOR_DST)                        \
    __VK_DISP_CVRT_IMPLEMENT(__COLOR_SRC, __COLOR_DST)
#define __VK_DISP_CVRT_DEFINE(__COLOR_SRC, __COLOR_DST)                         \
    {                                                                           \
        .color_src  = VSF_MCONNECT(VSF_DISP_COLOR_, __COLOR_SRC),               \
        .color_dst  = VSF_MCONNECT(VSF_DISP_COLOR_, __COLOR_DST),               \
        .fn_cvrt    = VSF_MCONNECT(vk_disp_color_cvrt_, __COLOR_SRC, _to_, __COLOR_DST),\
    }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_USE_KERNEL == ENABLED
enum {
    VSF_EVT_REFRESH = VSF_EVT_USER,
};
#endif

typedef void (*vk_disp_fn_cvrt_t)(uint16_t width, uint16_t height,
            void *pixels_src, uint32_t pitch_src,
            void *pixels_dst, uint32_t pitch_dst);
typedef struct vk_disp_cvrt_ctx_t {
    vk_disp_color_type_t color_src;
    vk_disp_color_type_t color_dst;
    vk_disp_fn_cvrt_t fn_cvrt;
} vk_disp_cvrt_ctx_t;

/*============================ PROTOTYPES ====================================*/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static vsf_err_t __vk_disp_reentrant_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_reentrant_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
#endif

static vsf_err_t __vk_disp_dummy_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
static vsf_err_t __vk_disp_cvrt_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

__VK_DISP_CVRT_DECLARE(ARGB8888, RGB565);

/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
const vk_disp_drv_t vk_disp_reentrant_drv = {
    .init           = __vk_disp_reentrant_init,
    .refresh        = __vk_disp_reentrant_refresh,
};
#endif

const vk_disp_drv_t vk_disp_dummy_drv = {
    .refresh        = __vk_disp_dummy_refresh,
};

const vk_disp_drv_t vk_disp_cvrt_drv = {
    .refresh        = __vk_disp_cvrt_refresh,
};

static const vk_disp_cvrt_ctx_t __vk_disp_cvrt_ctx[] = {
    __VK_DISP_CVRT_DEFINE(ARGB8888, RGB565),
};

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

void vk_disp_fini(vk_disp_t *pthis)
{
    VSF_UI_ASSERT(pthis != NULL);
    const vk_disp_drv_t *drv = pthis->param.drv;
    VSF_UI_ASSERT(drv != NULL);

    if (drv->fini != NULL) {
        drv->fini(pthis);
    } else {
        vk_disp_on_ready(pthis);
    }
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

static vsf_err_t __vk_disp_dummy_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_on_ready(pthis);
    return VSF_ERR_NONE;
}

__VK_DISP_CVRT_IMPLEMENT(ARGB8888, RGB565)
{
    uint32_t *src = pixels_src, value_src;
    uint16_t *dst = pixels_dst;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            value_src = src[x];
            dst[x] =    ((value_src & 0x000000F8) >> 3)
                    |   ((value_src & 0x0000FC00) >> 5)
                    |   ((value_src & 0x00F80000) >> 8);
        }
        dst = (uint16_t *)((uintptr_t)dst + pitch_dst);
        src = (uint32_t *)((uintptr_t)src + pitch_src);
    }
}

static vsf_err_t __vk_disp_cvrt_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_cvrt_t *disp_cvrt = (vk_disp_cvrt_t *)pthis;
    vk_disp_t *disp_real = disp_cvrt->disp_real;
    disp_real->ui_on_ready = pthis->ui_on_ready;
    disp_real->ui_data = pthis->ui_data;

    if (disp_real->param.color != pthis->param.color) {
        uint32_t pitch_src = area->size.x * vsf_disp_get_pixel_format_bytesize(pthis->param.color);
        uint32_t pitch_dst = area->size.x * vsf_disp_get_pixel_format_bytesize(disp_real->param.color);

        if (pitch_dst * area->size.y > disp_cvrt->pixel_buffer_size) {
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }

        vk_disp_fn_cvrt_t fn_cvrt = NULL;
        for (int idx = 0; idx < dimof(__vk_disp_cvrt_ctx); idx++) {
            if (    (__vk_disp_cvrt_ctx[idx].color_src == pthis->param.color)
                &&  (__vk_disp_cvrt_ctx[idx].color_dst == disp_real->param.color)) {
                fn_cvrt = __vk_disp_cvrt_ctx[idx].fn_cvrt;
                break;
            }
        }
        if (NULL == fn_cvrt) {
            return VSF_ERR_NOT_SUPPORT;
        }

        fn_cvrt(area->size.x, area->size.y, disp_buff, pitch_src, disp_cvrt->pixel_buffer, pitch_dst);
    }
    return vk_disp_refresh(disp_real, area, disp_cvrt->pixel_buffer);
}

#if VSF_DISP_USE_MUX == ENABLED
/*******************************************************************************
* multiplexed disp                                                             *
*******************************************************************************/

enum {
    __VK_DISP_MUX_REAL_OFF = 0,
    __VK_DISP_MUX_REAL_ON,
};

enum {
    __VK_DISP_MUX_OP_NONE = 0,
    __VK_DISP_MUX_OP_INIT,
    __VK_DISP_MUX_OP_REFRESH,
    __VK_DISP_MUX_OP_FINI,
};

static vsf_err_t __vk_disp_mux_update(vk_disp_mux_ctx_t *ctx);

static vk_disp_mux_t * __vk_disp_mux_get_top(vk_disp_mux_ctx_t *ctx)
{
    vk_disp_mux_t *top = NULL;
    for (vk_disp_mux_t *layer = ctx->layers; layer != NULL; layer = layer->next) {
        if (layer->is_inited && ((top == NULL) || (layer->layer > top->layer))) {
            top = layer;
        }
    }
    return top;
}

static void __vk_disp_mux_notify(vk_disp_mux_t *pthis, vk_disp_mux_evt_t evt)
{
    if (pthis->on_evt != NULL) {
        pthis->on_evt(pthis, evt, pthis->on_evt_param);
    }
}

static void __vk_disp_mux_complete_pending(vk_disp_mux_ctx_t *ctx)
{
    for (vk_disp_mux_t *layer = ctx->layers; layer != NULL; layer = layer->next) {
        if (ctx->real_op != __VK_DISP_MUX_OP_NONE) {
            return;
        }
        if (layer->init_pending && layer->is_inited
                && (ctx->real_state == __VK_DISP_MUX_REAL_ON)) {
            layer->init_pending = false;
            vk_disp_on_ready(&layer->use_as__vk_disp_t);
        } else if (layer->fini_pending && !layer->is_inited) {
            layer->fini_pending = false;
            vk_disp_on_ready(&layer->use_as__vk_disp_t);
        }
    }
}

static void __vk_disp_mux_on_real_ready(vk_disp_t *disp)
{
    vk_disp_mux_ctx_t *ctx = (vk_disp_mux_ctx_t *)disp->ui_data;
    uint8_t op = ctx->real_op;

    ctx->real_op = __VK_DISP_MUX_OP_NONE;
    switch (op) {
    case __VK_DISP_MUX_OP_INIT:
        ctx->real_state = __VK_DISP_MUX_REAL_ON;
        break;
    case __VK_DISP_MUX_OP_REFRESH: {
            vk_disp_mux_t *owner = ctx->real_owner;
            ctx->real_owner = NULL;
            vk_disp_on_ready(&owner->use_as__vk_disp_t);
        }
        break;
    case __VK_DISP_MUX_OP_FINI:
        ctx->real_state = __VK_DISP_MUX_REAL_OFF;
        break;
    default:
        VSF_UI_ASSERT(false);
        return;
    }
    (void)__vk_disp_mux_update(ctx);
}

static void __vk_disp_mux_prepare_real(vk_disp_mux_ctx_t *ctx)
{
    ctx->disp_real->ui_data = ctx;
    ctx->disp_real->ui_on_ready = __vk_disp_mux_on_real_ready;
}

static vsf_err_t __vk_disp_mux_update(vk_disp_mux_ctx_t *ctx)
{
    if (ctx->real_op != __VK_DISP_MUX_OP_NONE) {
        return VSF_ERR_NONE;
    }

    vk_disp_mux_t *top = __vk_disp_mux_get_top(ctx);
    if (ctx->real_state == __VK_DISP_MUX_REAL_OFF) {
        if (top == NULL) {
            __vk_disp_mux_complete_pending(ctx);
            return VSF_ERR_NONE;
        }

        ctx->real_op = __VK_DISP_MUX_OP_INIT;
        __vk_disp_mux_prepare_real(ctx);
        vsf_err_t err = vk_disp_init(ctx->disp_real);
        if (err != VSF_ERR_NONE) {
            ctx->real_op = __VK_DISP_MUX_OP_NONE;
            return err;
        }
        return VSF_ERR_NONE;
    }

    if (top == NULL) {
        if (ctx->active != NULL) {
            vk_disp_mux_t *old = ctx->active;
            ctx->active = NULL;
            __vk_disp_mux_notify(old, VK_DISP_MUX_EVT_INACTIVE);
        }
        ctx->real_op = __VK_DISP_MUX_OP_FINI;
        __vk_disp_mux_prepare_real(ctx);
        vk_disp_fini(ctx->disp_real);
        return VSF_ERR_NONE;
    }

    if ((ctx->active != NULL) && (ctx->active != top)) {
        vk_disp_mux_t *old = ctx->active;
        ctx->active = NULL;
        __vk_disp_mux_notify(old, VK_DISP_MUX_EVT_INACTIVE);
        top = __vk_disp_mux_get_top(ctx);
    }

    if (ctx->active == NULL) {
        ctx->active = top;
        __vk_disp_mux_complete_pending(ctx);
        if ((ctx->active == top) && (top != NULL)) {
            __vk_disp_mux_notify(top, VK_DISP_MUX_EVT_ACTIVE);
        }
    } else {
        __vk_disp_mux_complete_pending(ctx);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_mux_request_update(vk_disp_mux_ctx_t *ctx)
{
    vk_disp_mux_t *top = __vk_disp_mux_get_top(ctx);
    if ((ctx->active != NULL) && (ctx->active != top)) {
        vk_disp_mux_t *old = ctx->active;
        ctx->active = NULL;
        __vk_disp_mux_notify(old, VK_DISP_MUX_EVT_INACTIVE);
    }
    return __vk_disp_mux_update(ctx);
}

void vk_disp_mux_config(vk_disp_mux_t *pthis, vk_disp_mux_ctx_t *ctx, uint8_t layer)
{
    VSF_UI_ASSERT((pthis != NULL) && (ctx != NULL) && (ctx->disp_real != NULL));
    VSF_UI_ASSERT(!pthis->is_registered);

    vk_disp_param_t *param = (vk_disp_param_t *)&pthis->param;
    *param = ctx->disp_real->param;
    param->drv = &vk_disp_mux_drv;
    pthis->ctx = ctx;
    pthis->layer = layer;
}

bool vk_disp_mux_is_active(vk_disp_mux_t *pthis)
{
    VSF_UI_ASSERT((pthis != NULL) && (pthis->ctx != NULL));
    return    (pthis->ctx->active == pthis)
           && (pthis->ctx->real_state == __VK_DISP_MUX_REAL_ON);
}

static vsf_err_t __vk_disp_mux_init(vk_disp_t *disp)
{
    vk_disp_mux_t *pthis = (vk_disp_mux_t *)disp;
    vk_disp_mux_ctx_t *ctx = pthis->ctx;
    VSF_UI_ASSERT((ctx != NULL) && (ctx->disp_real != NULL));

    if (pthis->is_inited || pthis->init_pending) {
        return VSF_ERR_BUSY;
    }
    if (!pthis->is_registered) {
        pthis->next = ctx->layers;
        ctx->layers = pthis;
        pthis->is_registered = true;
    }

    pthis->is_inited = true;
    pthis->init_pending = true;
    pthis->fini_pending = false;
    vsf_err_t err = __vk_disp_mux_request_update(ctx);
    if (err != VSF_ERR_NONE) {
        pthis->is_inited = false;
        pthis->init_pending = false;
        (void)__vk_disp_mux_request_update(ctx);
    }
    return err;
}

static void __vk_disp_mux_fini(vk_disp_t *disp)
{
    vk_disp_mux_t *pthis = (vk_disp_mux_t *)disp;
    vk_disp_mux_ctx_t *ctx = pthis->ctx;

    if (!pthis->is_inited) {
        vk_disp_on_ready(disp);
        return;
    }

    pthis->is_inited = false;
    pthis->init_pending = false;
    pthis->fini_pending = true;
    (void)__vk_disp_mux_request_update(ctx);
}

static vsf_err_t __vk_disp_mux_refresh(vk_disp_t *disp, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_mux_t *pthis = (vk_disp_mux_t *)disp;
    vk_disp_mux_ctx_t *ctx = pthis->ctx;

    VSF_UI_ASSERT(pthis->is_inited);
    if (!pthis->is_inited) {
        return VSF_ERR_FAIL;
    }
    if ((ctx->active != pthis) || (ctx->real_state != __VK_DISP_MUX_REAL_ON)) {
        vk_disp_on_ready(disp);
        return VSF_ERR_NONE;
    }
    if (ctx->real_op != __VK_DISP_MUX_OP_NONE) {
        return VSF_ERR_BUSY;
    }

    ctx->real_owner = pthis;
    ctx->real_op = __VK_DISP_MUX_OP_REFRESH;
    __vk_disp_mux_prepare_real(ctx);
    vsf_err_t err = vk_disp_refresh(ctx->disp_real, area, disp_buff);
    if (err != VSF_ERR_NONE) {
        ctx->real_owner = NULL;
        ctx->real_op = __VK_DISP_MUX_OP_NONE;
    }
    return err;
}

const vk_disp_drv_t vk_disp_mux_drv = {
    .init                   = __vk_disp_mux_init,
    .fini                   = __vk_disp_mux_fini,
    .refresh                = __vk_disp_mux_refresh,
};
#endif

/*******************************************************************************
* reentrant disp                                                               *
*******************************************************************************/

#if VSF_USE_KERNEL == ENABLED && VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
static void __vk_disp_reentrant_on_ready(vk_disp_t *disp)
{
    vk_disp_reentrant_t *pthis = (vk_disp_reentrant_t *)disp->ui_data;
    vk_disp_on_ready(&pthis->use_as__vk_disp_t);
    if (pthis->mutex != NULL) {
        // use vsf_eda_mutex_leave_isr in case __vk_disp_reentrant_on_ready is called in isr
        vsf_eda_mutex_leave_isr(pthis->mutex);
    }
}

static void __vk_disp_reentrant_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    vk_disp_reentrant_t *pthis = vsf_container_of(eda, vk_disp_reentrant_t, eda);
    vsf_err_t err;

    switch (evt) {
    case VSF_EVT_INIT:
        pthis->disp->ui_on_ready = __vk_disp_reentrant_on_ready;
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

static vsf_err_t __vk_disp_reentrant_init(vk_disp_t *disp)
{
    vk_disp_reentrant_t *pthis = (vk_disp_reentrant_t *)disp;
    disp = pthis->disp;

    if (    ((pthis->pos.x + pthis->param.width) > disp->param.width)
        ||  ((pthis->pos.y + pthis->param.height) > disp->param.height)) {
        return VSF_ERR_FAIL;
    }

    pthis->eda.fn.evthandler = __vk_disp_reentrant_evthandler;
#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    pthis->eda.on_terminate = NULL;
#endif
    return vsf_eda_init(&pthis->eda);
}

static vsf_err_t __vk_disp_reentrant_refresh(vk_disp_t *disp, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_reentrant_t *pthis = (vk_disp_reentrant_t *)disp;

    pthis->area = *area;
    pthis->area.pos.x += pthis->pos.x;
    pthis->area.pos.y += pthis->pos.y;
    pthis->buffer = disp_buff;
    return vsf_eda_post_evt(&pthis->eda, VSF_EVT_REFRESH);
}
#endif
#endif

/* EOF */
