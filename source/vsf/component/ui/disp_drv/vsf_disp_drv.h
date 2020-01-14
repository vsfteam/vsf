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
#ifndef __VSF_DISP_DRV_H___
#define __VSF_DISP_DRV_H___

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED

#if     defined(__VSF_DISP_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(__VSF_DISP_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#define VSF_DISP_GET_PIXEL_SIZE(__disp)                                         \
            ((((vk_disp_t *)(__disp))->param.color >> 8) & 0xFF)

#define VSF_DISP_COLOR_IDX_DEF(__name)                                          \
            VSF_DISP_COLOR_IDX_##__name

#define VSF_DISP_COLOR_DEF(__name, __bitlen)                                    \
            VSF_DISP_COLOR_##__name = ((VSF_DISP_COLOR_IDX_##__name) | ((__bitlen) << 8))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_disp_t)

enum vk_disp_color_idx_t {
    VSF_DISP_COLOR_IDX_DEF(INVALID),
    VSF_DISP_COLOR_IDX_DEF(RGB565),
    VSF_DISP_COLOR_IDX_DEF(ARGB8888),
};
typedef enum vk_disp_color_idx_t vk_disp_color_idx_t;

enum vk_disp_color_t {
    // avoid vk_disp_color_t to be optimized to 8bit
    __VSF_DISP_COLOR_LEAST_MAX	= INT16_MAX,
    __VSF_DISP_COLOR_LEAST_MIN  = INT16_MIN,
    VSF_DISP_COLOR_DEF(INVALID, 0),
    VSF_DISP_COLOR_DEF(RGB565, 16),
    VSF_DISP_COLOR_DEF(ARGB8888, 32),
};
typedef enum vk_disp_color_t vk_disp_color_t;

struct vk_disp_point {
    uint16_t x;
    uint16_t y;
};
typedef struct vk_disp_point vk_disp_point;

struct vk_disp_area_t {
    vk_disp_point pos;
    vk_disp_point size;
};
typedef struct vk_disp_area_t vk_disp_area_t;

struct vk_disp_drv_t {
    vsf_err_t (*init)(vk_disp_t *pthis);
    vsf_err_t (*refresh)(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
};
typedef struct vk_disp_drv_t vk_disp_drv_t;

struct vk_disp_param_t {
    const vk_disp_drv_t *drv;
    uint16_t width;
    uint16_t height;
    vk_disp_color_t color; 
};
typedef struct vk_disp_param_t vk_disp_param_t;

typedef void (*vk_disp_on_ready_t)(vk_disp_t *disp);

def_simple_class(vk_disp_t) {
    public_member(
        const vk_disp_param_t   param;
        void                    *ui_data;
    )
    protected_member(
        vk_disp_on_ready_t      ui_on_ready;
    )
};

/*============================ INCLUDES ======================================*/

#if VSF_USE_DISP_DRV_SDL2 == ENABLED
#   include "./sdl2/vsf_disp_drv_sdl2.h"
#endif
#if VSF_USE_DISP_DRV_USBD_UVC == ENABLED
#   include "./usbd_uvc/vsf_disp_drv_usbd_uvc.h"
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_disp_init(vk_disp_t *pthis);
extern vsf_err_t vk_disp_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

#ifdef __VSF_DISP_CLASS_INHERIT
extern void vk_disp_on_ready(vk_disp_t *pthis);
#endif

#undef __VSF_DISP_CLASS_IMPLEMENT
#undef __VSF_DISP_CLASS_INHERIT

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_DRV_H___
