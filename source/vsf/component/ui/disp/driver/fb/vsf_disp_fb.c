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
static void * __vk_disp_fb_switch_buffer(vk_disp_t *pthis, bool is_to_copy);

/*============================ GLOBAL VARIABLES ==============================*/

const vk_disp_drv_t vk_disp_drv_fb = {
    .init               = __vk_disp_fb_init,
    .refresh            = __vk_disp_fb_refresh,
    .fb                 = {
        .switch_buffer  = __vk_disp_fb_switch_buffer,
    },
};

/*============================ IMPLEMENTATION ================================*/

static void *__vk_disp_fb_get_buffer(vk_disp_fb_t *disp_fb)
{
    return ((uint8_t *)disp_fb->fb_buffer + disp_fb->cur_fb_buffer * disp_fb->fb.size);
}

static void * __vk_disp_fb_next(vk_disp_fb_t *disp_fb)
{
    if (++disp_fb->cur_fb_buffer >= disp_fb->fb.num) {
        disp_fb->cur_fb_buffer = 0;
    }
    disp_fb->fb.buffer = __vk_disp_fb_get_buffer(disp_fb);
    return disp_fb->fb.buffer;
}

static void * __vk_disp_fb_switch_buffer(vk_disp_t *pthis, bool is_to_copy)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    void *cur_buffer, *next_buffer;

    VSF_UI_ASSERT((disp_fb != NULL) && (disp_fb->fb.drv != NULL));
    cur_buffer = __vk_disp_fb_get_buffer(disp_fb);
    next_buffer = __vk_disp_fb_next(disp_fb);
    if (is_to_copy) {
        memcpy(next_buffer, cur_buffer, disp_fb->fb.size);
    }
    disp_fb->fb.drv->Present(disp_fb->fb.param, cur_buffer);
    return next_buffer;
}

static vsf_err_t __vk_disp_fb_init(vk_disp_t *pthis)
{
    vk_disp_fb_t *disp_fb = (vk_disp_fb_t *)pthis;
    VSF_UI_ASSERT((disp_fb != NULL) && (disp_fb->fb.drv != NULL) && (disp_fb->fb.num > 0));

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

    disp_fb->cur_fb_buffer = 0;
    disp_fb->fb.drv->Init(disp_fb->fb.param, __vk_disp_fb_get_buffer(disp_fb));
    __vk_disp_fb_next(disp_fb);
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
    void *cur_buffer = (uint8_t *)__vk_disp_fb_get_buffer(disp_fb) + line_size * real_area.pos.y;

    for (uint_fast32_t y = real_area.pos.y; y < y_end; y++) {
        memcpy((uint8_t *)cur_buffer + x_offset, disp_buff, copy_size);
        disp_buff = (uint8_t *)disp_buff + copy_size;
        cur_buffer = (uint8_t *)cur_buffer + line_size;
    }

    // avoid copying for better performance
    __vk_disp_fb_switch_buffer(&disp_fb->use_as__vk_disp_t, VSF_DISP_FB_CFG_COPY_FRAME);
    vk_disp_on_ready(&disp_fb->use_as__vk_disp_t);
    return VSF_ERR_NONE;
}

#endif

/* EOF */
