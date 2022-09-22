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

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_FB == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_DISP_FB_CLASS_IMPLEMENT

#include "../../vsf_disp.h"
#include "./vsf_disp_fb.h"

/*============================ MACROS ========================================*/

#ifndef VSF_DISP_FB_CFG_COPY_FRAME
#   define VSF_DISP_FB_CFG_COPY_FRAME               true
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vk_disp_fb_init(vk_disp_t *pthis);
static vsf_err_t __vk_disp_fb_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_fb = {
    .init                   = __vk_disp_fb_init,
    .refresh                = __vk_disp_fb_refresh,
};

/*============================ IMPLEMENTATION ================================*/

static uint8_t __vk_disp_fb_get_back_buffer_idx(vk_disp_fb_t *disp_fb)
{
    uint8_t back_buffer_idx = disp_fb->front_buffer_idx + 1;
    if (back_buffer_idx >= disp_fb->fb.num) {
        back_buffer_idx = 0;
    }
    return back_buffer_idx;
}

void * vk_disp_fb_get_buffer(vk_disp_t *pthis, uint_fast8_t idx)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    return ((uint8_t *)disp_fb->fb_buffer + idx * disp_fb->fb.size);
}

void * vk_disp_fb_set_front_buffer(vk_disp_t *pthis, int_fast8_t idx)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    disp_fb->front_buffer_idx = idx < 0 ? __vk_disp_fb_get_back_buffer_idx(disp_fb) : idx;
    void *buffer = vk_disp_fb_get_buffer(pthis, disp_fb->front_buffer_idx);
    disp_fb->fb.drv->fb.present(disp_fb->fb.param, buffer);
    return buffer;
}

void * vk_disp_fb_get_front_buffer(vk_disp_t *pthis)
{
    return vk_disp_fb_get_buffer(pthis, ((vk_disp_fb_t *)pthis)->front_buffer_idx);
}

void * vk_disp_fb_get_back_buffer(vk_disp_t *pthis)
{
    return vk_disp_fb_get_buffer(pthis, __vk_disp_fb_get_back_buffer_idx((vk_disp_fb_t *)pthis));
}

static vsf_err_t __vk_disp_fb_init(vk_disp_t *pthis)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    VSF_UI_ASSERT((disp_fb != NULL) && (disp_fb->fb.drv != NULL) && (disp_fb->fb.num > 0));

    if (disp_fb->fb.drv->init != NULL) {
        disp_fb->fb.drv->init(pthis);
    }

    if (disp_fb->fb.buffer != NULL) {
        disp_fb->fb_buffer = disp_fb->fb.buffer;
    } else {
#if VSF_USE_HEAP == ENABLED
        uint_fast32_t size = disp_fb->fb.num * disp_fb->fb.size;
        disp_fb->fb_buffer = vsf_heap_malloc(size);
        memset(disp_fb->fb_buffer, 0, size);
#else
        VSF_UI_ASSERT(false);
#endif
    }

    disp_fb->front_buffer_idx = 0;
    vsf_err_t err = disp_fb->fb.drv->fb.init(disp_fb->fb.param, disp_fb->param.color,
                vk_disp_fb_get_buffer(pthis, disp_fb->front_buffer_idx));
    if (VSF_ERR_NONE == err) {
        vk_disp_on_ready(pthis);
    }
    return VSF_ERR_NONE;
}

static vsf_err_t __vk_disp_fb_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    VSF_UI_ASSERT((disp_fb != NULL) && (disp_buff != NULL));

    vk_disp_area_t real_area;
    if (NULL == area) {
        real_area.pos.x = 0;
        real_area.pos.y = 0;
        real_area.size.x = disp_fb->param.width;
        real_area.size.y = disp_fb->param.height;
    } else {
        real_area = *area;
    }

    uint_fast32_t line_size = disp_fb->fb.pixel_byte_size * disp_fb->param.width;
    uint_fast32_t copy_size = disp_fb->fb.pixel_byte_size * real_area.size.x;
    uint_fast32_t x_offset = disp_fb->fb.pixel_byte_size * real_area.pos.x;
    uint_fast32_t y_end = real_area.pos.y + real_area.size.y;
    void *cur_buffer = vk_disp_fb_get_back_buffer(pthis);

    for (uint_fast32_t y = real_area.pos.y; y < y_end; y++) {
        memcpy((uint8_t *)cur_buffer + x_offset, disp_buff, copy_size);
        disp_buff = (uint8_t *)disp_buff + copy_size;
        cur_buffer = (uint8_t *)cur_buffer + line_size;
    }
    vk_disp_fb_set_front_buffer(pthis, -1);
    return VSF_ERR_NONE;
}
#endif

/* EOF */
