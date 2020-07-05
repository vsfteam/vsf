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

#if     defined(__VSF_DISP_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(__VSF_DISP_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define VSF_DISP_COLOR_IDX_DEF(__name)                                          \
            VSF_DISP_COLOR_IDX_##__name

// bit0 - 7:    index
// bit8 - 12:   bitlen - 1
// bit13 - 14:  bytelen - 1
// bit15:       msb_fisrt, reserved, not used now
#define VSF_DISP_COLOR_DEF(__name, __bitlen, __bytelen, __msb)                  \
            VSF_DISP_COLOR_##__name =   ((VSF_DISP_COLOR_IDX_##__name)          \
                                    |   ((((__bitlen) - 1) & 0x1F) << 8)        \
                                    |   ((((__bytelen) - 1) & 0x03) << 13)      \
                                    |   ((__msb) << 15))

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

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vk_disp_t)
declare_simple_class(vk_disp_drv_t)

enum vk_disp_color_idx_t {
    VSF_DISP_COLOR_IDX_DEF(INVALID),
    VSF_DISP_COLOR_IDX_DEF(RGB565),
    VSF_DISP_COLOR_IDX_DEF(ARGB8888),
    VSF_DISP_COLOR_IDX_DEF(RGB666_32),
};
typedef enum vk_disp_color_idx_t vk_disp_color_idx_t;

enum vk_disp_color_t {
    // avoid vk_disp_color_t to be optimized to 8bit
    __VSF_DISP_COLOR_LEAST_MAX	= INT16_MAX,
    __VSF_DISP_COLOR_LEAST_MIN  = INT16_MIN,
    VSF_DISP_COLOR_DEF(INVALID, 0, 0, 0),
    VSF_DISP_COLOR_DEF(RGB565, 16, 2, 0),
    VSF_DISP_COLOR_DEF(ARGB8888, 32, 4, 0),
    VSF_DISP_COLOR_DEF(RGB666_32, 18, 4, 0),
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

def_simple_class(vk_disp_drv_t) {
    protected_member(
        vsf_err_t (*init)(vk_disp_t *pthis);
        vsf_err_t (*refresh)(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);
    )
    protected_member(
        // TODO: add a enum for driver type
#if VSF_USE_DISP_FB == ENABLED
        union {
#   if VSF_USE_DISP_FB == ENABLED
            struct {
                void * (*switch_buffer)(vk_disp_t *pthis, bool is_to_copy);
            } fb;
#   endif
        };
#endif
    )
};

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

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vk_disp_init(vk_disp_t *pthis);
extern vsf_err_t vk_disp_refresh(vk_disp_t *pthis, vk_disp_area_t *area, void *disp_buff);

#ifdef __VSF_DISP_CLASS_INHERIT
extern void vk_disp_on_ready(vk_disp_t *pthis);
#endif

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if VSF_USE_DISP_SDL2 == ENABLED
#   include "./driver/sdl2/vsf_disp_sdl2.h"
#endif
#if VSF_USE_DISP_USBD_UVC == ENABLED
#   include "./driver/usbd_uvc/vsf_disp_usbd_uvc.h"
#endif
#if VSF_USE_DISP_ST7789 == ENABLED
#   include "./driver/sitronix/st7789/vsf_disp_st7789.h"
#endif
#if VSF_USE_DISP_ST7735 == ENABLED
#   include "./driver/sitronix/st7735/vsf_disp_st7735.h"
#endif
#if VSF_USE_DISP_SSD1306 == ENABLED
#   include "./driver/solomon-systech/ssd1306/vsf_disp_ssd1306.h"
#endif
#if VSF_USE_DISP_VGA_M480 == ENABLED
#   include "./driver/vga/m480/vsf_disp_vga_m480.h"
#endif
#if VSF_USE_DISP_FB == ENABLED
#   include "./driver/fb/vsf_disp_fb.h"
#endif

#undef __VSF_DISP_CLASS_IMPLEMENT
#undef __VSF_DISP_CLASS_INHERIT

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_H__
