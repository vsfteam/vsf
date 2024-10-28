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
#ifndef __VSF_DISP_FRAME_BUFFER_H__
#define __VSF_DISP_FRAME_BUFFER_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_FB == ENABLED

#include "kernel/vsf_kernel.h"
#include "hal/vsf_hal.h"

#if     defined(__VSF_DISP_FB_CLASS_IMPLEMENT)
#   undef __VSF_DISP_FB_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// keey vk_disp_fb_drv_t independent of ui module, because maybe it's implemented in hal
typedef struct vk_disp_fb_drv_t {
    vsf_err_t (*init)(void *fb);
    vsf_err_t (*fini)(void *fb);
    vsf_err_t (*enable)(void *fb);
    vsf_err_t (*disable)(void *fb);

    struct {
        vsf_err_t (*config)(void *fb, int layer,
                    uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                    int color_format, uint_fast8_t alpha, uint32_t default_color,
                    void *initial_pixel_buffer);
        vsf_err_t (*enable)(void *fb, int layer);
        vsf_err_t (*disable)(void *fb, int layer);
        vsf_err_t (*present)(void *fb, int layer, void *pixel_buffer);
    } layer;

    struct {
        vsf_err_t (*fill_color)(void *fb,
            uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h,
            uint32_t color, uint32_t color_format);
        vsf_err_t (*fill_colors)(void *fb,
            uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h,
            void *colors, uint32_t color_format,
            uint_fast16_t rotate_degrees, float xscale, float yscale);
        vsf_err_t (*blend_colors)(void *fb,
            uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h,
            void *colors0, uint32_t color0_format, uint_fast8_t color0_alpha,
            void *colors1, uint32_t color1_format, uint_fast8_t color1_alpha);
    } gpu;
} vk_disp_fb_drv_t;

vsf_class(vk_disp_fb_t) {
    public_member(
        implement(vk_disp_t)

        // user can provide the frame buffer here
        //  if NULL, frame buffer will be allocated from heap
        void                    *buffer;

        const vk_disp_fb_drv_t  *drv;
        void                    *drv_param;

        uint32_t                fb_size;
        uint32_t                layer_default_color;
        vk_disp_color_type_t    layer_color_type;
        vk_disp_area_t          layer_area;
        uint8_t                 fb_num;
        uint8_t                 layer_idx;
        uint8_t                 layer_alpha;
    )

    private_member(
        void                    *fb_buffer;
        uint8_t                 front_buffer_idx;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_fb;

#if VSF_HAL_USE_FB == ENABLED
// hal can not include display module, so declare hal structures here
// vsf_disp_hw_fb_drv is used for the first layer,
//  while vsf_disp_hw_fb_layer_drv is used for other layers, which has only layer APIs
extern const vk_disp_fb_drv_t vsf_disp_hw_fb_drv;
extern const vk_disp_fb_drv_t vsf_disp_hw_fb_layer_drv;
#endif

/*============================ PROTOTYPES ====================================*/

extern void * vk_disp_fb_get_buffer(vk_disp_t *disp, uint_fast8_t idx);
extern void * vk_disp_fb_get_front_buffer(vk_disp_t *disp);
extern void * vk_disp_fb_get_back_buffer(vk_disp_t *disp);
// if idx < 0, then set back buffer(returned by get_back_buffer) as front buffer
extern void * vk_disp_fb_set_front_buffer(vk_disp_t *disp, int_fast8_t idx);

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI && VSF_DISP_USE_FB
#endif  // __VSF_DISP_FRAME_BUFFER_H__
