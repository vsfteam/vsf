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
#ifndef __VSF_DISP_ST7735_H__
#define __VSF_DISP_ST7735_H__

/*============================ INCLUDES ======================================*/

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_ST7735 == ENABLED

#include "../common/vsf_disp_sitronix_common.h"

#if     defined(__VSF_DISP_ST7735_CLASS_IMPLEMENT)
#   undef __VSF_DISP_ST7735_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define ST7735_SLPIN                        (0x10)
#define ST7735_SLPOUT                       (0x11)

#define ST7735_INV(__ON)                    (0x20 | ((__ON) ? 1 : 0))
#define ST7735_DISP(__ON)                   (0x28 | ((__ON) ? 1 : 0))
#define ST7735_CASET                        (0x2A)
#define ST7735_RASET                        (0x2B)
#define ST7735_RAMWR                        (0x2C)

#define ST7735_MADCTL                       (0x36)
#   define ST7735_MADCTL_COLOR_RGB          (0 << 3)
#   define ST7735_MADCTL_COLOR_BGR          (1 << 3)
#   define ST7735_MADCTL_MV                 (1 << 5)
#   define ST7735_MADCTL_MX                 (1 << 6)
#   define ST7735_MADCTL_MY                 (1 << 7)

#define ST7735_COLMOD                       (0x3A)
#   define ST7735_COLMOD_PIXEL_12B          (3 << 0)
#   define ST7735_COLMOD_PIXEL_16B          (5 << 0)
#   define ST7735_COLMOD_PIXEL_18B          (6 << 0)

#define ST7735_DISPLAY_SETTING_COLOR_RGB    ST7735_MADCTL_COLOR_RGB
#define ST7735_DISPLAY_SETTING_COLOR_BGR    ST7735_MADCTL_COLOR_BGR
#define ST7735_DISPLAY_SETTING_XY_EXCHANGE  ST7735_MADCTL_MV
#define ST7735_DISPLAY_SETTING_X_REVERSE    ST7735_MADCTL_MX
#define ST7735_DISPLAY_SETTING_Y_REVERSE    ST7735_MADCTL_MY

#define ST7735_PIXEL_12B                    ST7735_COLMOD_PIXEL_12B
#define ST7735_PIXEL_16B                    ST7735_COLMOD_PIXEL_16B
#define ST7735_PIXEL_18B                    ST7735_COLMOD_PIXEL_18B

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DISP_ST7735_INIT_SEQ(__PIXEL_FORMAT, __DISPLAY_SETTING, __INVERSE)  \
            VSF_DISP_SITRONIX_WRITE(0xB1, 3, 0x05, 0x3C, 0x3C),                 \
            VSF_DISP_SITRONIX_WRITE(0xB2, 3, 0x05, 0x3C, 0x3C),                 \
            VSF_DISP_SITRONIX_WRITE(0xB3, 6, 0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C),\
            VSF_DISP_SITRONIX_WRITE(0xB4, 1, 0x03),                             \
            VSF_DISP_SITRONIX_WRITE(0xC0, 3, 0xAB, 0x0B, 0x04),                 \
            VSF_DISP_SITRONIX_WRITE(0xC1, 1, 0xC5),                             \
            VSF_DISP_SITRONIX_WRITE(0xC2, 2, 0x0D, 0x00),                       \
            VSF_DISP_SITRONIX_WRITE(0xC3, 2, 0x8D, 0x6A),                       \
            VSF_DISP_SITRONIX_WRITE(0xC4, 2, 0x8D, 0xEE),                       \
            VSF_DISP_SITRONIX_WRITE(0xC5, 1, 0x0F),                             \
            VSF_DISP_SITRONIX_WRITE(0xE0, 16, 0x07, 0x0E, 0x08, 0x07, 0x10, 0x07, 0x02, 0x07, 0x09, 0x0F, 0x25, 0x36, 0x00, 0x08, 0x04, 0x10),\
            VSF_DISP_SITRONIX_WRITE(0xE1, 16, 0x0A, 0x0D, 0x08, 0x07, 0x0F, 0x07, 0x02, 0x07, 0x09, 0x0F, 0x25, 0x35, 0x00, 0x09, 0x04, 0x10),\
            VSF_DISP_SITRONIX_WRITE(0xFC, 1, 0x80),                             \
            VSF_DISP_SITRONIX_WRITE(0xF0, 1, 0x11),                             \
            VSF_DISP_SITRONIX_WRITE(0xD6, 1, 0xCB),                             \
            VSF_DISP_SITRONIX_WRITE(ST7735_COLMOD, 1, __PIXEL_FORMAT),          \
            VSF_DISP_SITRONIX_WRITE(ST7735_MADCTL, 1, __DISPLAY_SETTING),       \
            VSF_DISP_SITRONIX_WRITE(ST7735_INV(__INVERSE), 0),                  \
            VSF_DISP_SITRONIX_WRITE(ST7735_DISP(1), 0)

/*============================ TYPES =========================================*/

dcl_simple_class(vk_disp_st7735_t)

def_simple_class(vk_disp_st7735_t) {
    public_member(
        implement(vk_disp_sitronix_t);

        const uint8_t *init_seq;
        const uint16_t init_seq_len;
        const uint8_t colume_offset;
        const uint8_t page_offset;
    )
    private_member(
        vsf_teda_t teda;
        struct {
            struct {
                vk_disp_area_t area;
                uint8_t *buffer;
            } refresh;
        } ctx;

        uint8_t state;
        bool is_inited;
        bool is_area_set;
        uint8_t set_area_cmd_buffer[12];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vk_disp_drv_t vk_disp_drv_st7735;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_ST7735_H__
