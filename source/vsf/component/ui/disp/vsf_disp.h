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
#ifndef __VSF_DISP_H__
#define __VSF_DISP_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_DISP_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISP_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_DISP_COLOR_IDX_DEF(__NAME)                                          \
            VSF_DISP_COLOR_IDX_##__NAME

// bit0 - 7:    index
// bit8 - 12:   bitlen - 1
// bit13 - 14:  bytelen - 1
// bit15:       msb_fisrt, reserved, not used now
#define VSF_DISP_COLOR_DEF(__NAME, __BITLEN, __BYTELEN, __MSB)                  \
            VSF_DISP_COLOR_##__NAME =   ((VSF_DISP_COLOR_IDX_##__NAME)          \
                                    |   ((((__BITLEN) - 1) & 0x1F) << 8)        \
                                    |   ((((__BYTELEN) - 1) & 0x03) << 13)      \
                                    |   ((__MSB) << 15))

#define vsf_disp_get_pixel_format_bytesize(__color_format)                      \
            ((((__color_format) >> 13) & 0x03) + 1)
#define vsf_disp_get_pixel_format_bitsize(__color_format)                       \
            ((((__color_format) >> 8) & 0x1F) + 1)

#define vsf_disp_get_pixel_format(__disp)                                       \
            ((vk_disp_t *)(__disp))->param.color

#define vsf_disp_get_pixel_bitsize(__disp)                                      \
            vsf_disp_get_pixel_format_bitsize(vsf_disp_get_pixel_format(__disp))
#define vsf_disp_get_pixel_bytesize(__disp)                                     \
            vsf_disp_get_pixel_format_bytesize(vsf_disp_get_pixel_format(__disp))

#ifndef vk_disp_coord_t
#   define vk_disp_coord_t          uint16_t
#endif
#ifndef vk_disp_fast_coord_t
#   define vk_disp_fast_coord_t     uint_fast16_t
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_t)
dcl_simple_class(vk_disp_drv_t)

typedef enum vk_disp_color_idx_t {
    VSF_DISP_COLOR_IDX_DEF(INVALID),
    VSF_DISP_COLOR_IDX_DEF(RGB565),
    VSF_DISP_COLOR_IDX_DEF(ARGB8888),
    VSF_DISP_COLOR_IDX_DEF(RGB666_32),
} vk_disp_color_idx_t;

typedef enum vk_disp_color_type_t {
    // avoid vk_disp_color_type_t to be optimized to 8bit
    __VSF_DISP_COLOR_LEAST_MAX	= INT16_MAX,
    __VSF_DISP_COLOR_LEAST_MIN  = INT16_MIN,
    VSF_DISP_COLOR_DEF(INVALID, 0, 0, 0),
    VSF_DISP_COLOR_DEF(RGB565, 16, 2, 0),
    VSF_DISP_COLOR_DEF(ARGB8888, 32, 4, 0),
    VSF_DISP_COLOR_DEF(RGB666_32, 18, 4, 0),
} vk_disp_color_type_t;

typedef struct vk_disp_point {
    vk_disp_coord_t x;
    vk_disp_coord_t y;
} vk_disp_point;

typedef struct vk_disp_area_t {
    vk_disp_point pos;
    vk_disp_point size;
} vk_disp_area_t;

def_simple_class(vk_disp_drv_t) {
    protected_member(
        vsf_err_t (*init)(vk_disp_t *pthis);
        vsf_err_t (*refresh)(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

        // interfaces for gpu acceleration
#if VSF_DISP_USE_GPU == ENABLED
        void (*blend)(vk_disp_t *pthis, void *target_buff, vk_disp_fast_coord_t target_width,
                        vk_disp_area_t *area, void *disp_buff);
        void (*fill)(vk_disp_t *pthis, void *target_buff, vk_disp_fast_coord_t target_width,
                        vk_disp_area_t *area, uint_fast32_t color);
#endif
    )
    // interfaces for specific driver
#if VSF_DISP_USE_FB == ENABLED
    protected_member(
        // TODO: add a enum for driver type
        union {
#   if VSF_DISP_USE_FB == ENABLED
            struct {
                void * (*switch_buffer)(vk_disp_t *pthis, bool is_to_copy);
            } fb;
#   endif
        };
    )
#endif
};

typedef struct vk_disp_param_t {
    const vk_disp_drv_t *drv;
    uint16_t width;
    uint16_t height;
    vk_disp_color_type_t color;
} vk_disp_param_t;

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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_disp_init(vk_disp_t *pthis);
extern vsf_err_t vk_disp_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

#ifdef __VSF_DISP_CLASS_INHERIT__
extern void vk_disp_on_ready(vk_disp_t *pthis);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#include "./driver/sdl2/vsf_disp_sdl2.h"
#include "./driver/usbd_uvc/vsf_disp_usbd_uvc.h"
#include "./driver/sitronix/st7789/vsf_disp_st7789.h"
#include "./driver/sitronix/st7735/vsf_disp_st7735.h"
#include "./driver/solomon-systech/ssd1306/vsf_disp_ssd1306.h"
#include "./driver/vga/m480/vsf_disp_vga_m480.h"
#include "./driver/fb/vsf_disp_fb.h"

#undef __VSF_DISP_CLASS_IMPLEMENT
#undef __VSF_DISP_CLASS_INHERIT__

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_H__
